void customwm::
set_grouped(Client *c, bool s)
{}

void customwm::
toggle_grouped(Client *c)
{
    if(!c) return;
    log("Grouping client");
    if(!c->is_grouped)
        grouped_clients.push_back(c);
    else
        for(int i=0; i<grouped_clients.size(); i++)
            if(grouped_clients.at(i) == c)
                grouped_clients.erase(grouped_clients.begin() + i);
    c->is_grouped = !c->is_grouped;
    update_current_client();
}

void customwm::
fake_fullscreen(Client *c)
{
    log("Setting fake fullscreen");
    if(!c) return;
    if(!c->is_full)
        XChangeProperty(dpy, c->win, netatom[NetWMFullscreen], XA_ATOM, 32, PropModeReplace,
                (unsigned char *) &(c->win), 1);
    else
        XChangeProperty(dpy, c->win, netatom[NetWMFullscreen], XA_ATOM, 32, PropModeReplace,
                (unsigned char *) 0, 0);
    c->is_full = !c->is_full;
}

void customwm::
toggle_fullscreen(Client *c)
{
    log("Setting fullscreen");
    if(!c || !c->is_resizable) return;
    if(!c->is_full)
    {   
        c->oldx = c->x;
        c->oldy = c->y;
        c->oldw = c->w;
        c->oldh = c->h;
        c->old_state = c->is_float;
        c->is_float = false;
        XMoveResizeWindow(dpy,  c->win, 0, 0, sw, sh); 
        XSetWindowBorderWidth(dpy, c->win, 0);
        XRaiseWindow(dpy, c->win);
        XChangeProperty(dpy, c->win, netatom[NetWMFullscreen], XA_WINDOW, 32, PropModeReplace,
            (unsigned char *) &(c->win), 1);
    }
    else
    {
        XMoveResizeWindow(dpy, c->win, c->oldx, c->oldy, c->oldw, c->oldh);
        if(c->old_state)
            set_float(c, true);
        c->x = c->oldx;
        c->y = c->oldy;
        c->h = c->oldh;
        c->w = c->oldw;
        XChangeProperty(dpy, c->win, netatom[NetWMFullscreen], XA_WINDOW, 32, PropModeReplace,
            (unsigned char *) 0, 0);
    }
    c->is_full = !c->is_full;
    update_current_client();
}

void customwm::
set_fullscreen(Client *c, bool f)
{
    log("Setting fullscreen");
    if(!c) return;
    if(f)
    {   
        c->oldx = c->x;
        c->oldy = c->y;
        c->oldw = c->w;
        c->oldh = c->h;
        XMoveResizeWindow(dpy,  c->win, 0, 0, sw, sh); 
        XSetWindowBorderWidth(dpy, c->win, 0);
        XRaiseWindow(dpy, c->win);
    }
    else
    {
        XMoveResizeWindow(dpy, c->win, c->oldx, c->oldy, c->oldw, c->oldh);
        c->x = c->oldx;
        c->y = c->oldy;
        c->h = c->oldh;
        c->w = c->oldw;
    }
    c->is_full = f;
    update_current_client();
}

void customwm::
toggle_sticky(Client *c)
{
    log("Toggle Sticky");
    if(!c) return;
    if(c->is_sticky)
    {
        XChangeProperty(dpy, c->win, netatom[NetWMSticky], XA_WINDOW, 32,
                PropModeReplace,(unsigned char *) 0, 0);
    }
    else
    {
        XChangeProperty(dpy, c->win, netatom[NetWMSticky], XA_WINDOW, 32,
                PropModeReplace, (unsigned char *) &(c->win), 1);
    }
    c->is_sticky = !c->is_sticky;
    update_current_client();
}

void customwm::
set_sticky(Client *c, bool s)
{
    Client *t;
    if(!c) return;
    if(s)
        XChangeProperty(dpy, c->win, netatom[NetWMSticky], XA_WINDOW, 32,
                PropModeReplace,(unsigned char *) 0, 0);
    else
        XChangeProperty(dpy, c->win, netatom[NetWMSticky], XA_WINDOW, 32,
                PropModeReplace, (unsigned char *) &(c->win), 1);
    c->is_sticky = s;
    update_current_client();
}

void customwm::
toggle_float(Client *c)
{
    log("Setting float");
    if(!c || c->is_fixed || c->is_full) return;
    if(c->is_grouped)
    {
    }
    else
    {
        if(!c->is_float)
        {
            c->oldx = c->x;
            c->oldy = c->y;
            c->oldw = c->w;
            c->oldh = c->h;
            if(!c->is_resizable)
            {
                c->w = c->maxw;
                c->h = c->maxh;
                if(c->w == 0 || c->h == 0) get_size_hints(c);
            }
            else
            {
                c->w = sw/2;
                c->h = sh/2;
            }
            c->x = (sw - c->w)/2;
            c->y = (sh - c->h)/2;
            if(DECORATIONS_ON_FLOAT)
            {
                if(!c->dec)
                    client_decorations_create(c);
                XMapWindow(dpy, c->dec);
                if(!SIDE_DECORATION)
                    XMoveResizeWindow(dpy, c->dec, c->x, c->y - TITLE_HEIGHT,
                            c->w + 2*BORDER_WIDTH,
                            TITLE_HEIGHT);
                else
                    XMoveResizeWindow(dpy, c->dec, c->x - TITLE_HEIGHT, c->y,
                            TITLE_HEIGHT,
                            c->h + 2*BORDER_WIDTH);
                XMoveResizeWindow(dpy, c->win, c->x, c->y, c->w, c->h);
            }
            else
                XMoveResizeWindow(dpy, c->win, c->x, c->y, c->w, c->h);
            XRaiseWindow(dpy, c->win);
            c->is_float = true;
        }
        else
        {
            if(DECORATIONS_ON_FLOAT && c->dec)
            {
                XUnmapWindow(dpy, c->dec);
            }
            c->is_float = false;
            c->x = c->oldx;
            c->y = c->oldy;
            c->w = c->oldw;
            c->h = c->oldh;
            XLowerWindow(dpy, c->win);
        }
    }
    update_current_client();
    applylayout();

}

void customwm::
set_float(Client *c, bool f, bool move)
{
    log("Setting float");
    if(!c) return;
    c->oldx = c->x;
    c->oldy = c->y;
    c->oldw = c->w;
    c->oldh = c->h;
    if(f && move)
    {
        if(DECORATIONS_ON_FLOAT)
        {
            if(!c->dec)
                client_decorations_create(c);
            XMapWindow(dpy, c->dec);
            XRaiseWindow(dpy, c->dec);
        }
        if(!c->is_resizable)
        {
            c->w = c->maxw;
            c->h = c->maxh;
            if(c->w == 0 || c->h == 0) get_size_hints(c);
        }
        else
        {
            c->w = sw/2;
            c->h = sh/2;
        }
        c->x = (sw - c->w)/2;
        c->y = (sh - c->h)/2;
        XMoveResizeWindow(dpy, c->win, c->x, c->y, c->w, c->h);
        XRaiseWindow(dpy, c->win);
    }
    else if(f && DECORATIONS_ON_FLOAT)
    {
        XRaiseWindow(dpy, c->win);
        if(!c->dec)
            client_decorations_create(c);
        XMapWindow(dpy, c->dec);
        XRaiseWindow(dpy, c->dec);
    }
    else if(!f)
    {
        if(c->dec && DECORATIONS_ON_FLOAT)
        {
            client_decoration_toggle(c);
        }
        XLowerWindow(dpy, c->win);
        XLowerWindow(dpy, c->dec);
    }
    c->is_float = f;
    update_current_client();
    applylayout();
}

void customwm::
toggle_fixed(Client *c)
{
    if(!c) return;
    c->is_fixed = !c->is_fixed;
    update_current_client();
    //log("Setting client as fixed");
}
