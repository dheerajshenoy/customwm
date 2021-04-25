void customwm::
move_window_with_key(Client *c, string dir, int step)
{
    //log("Moving window with key");
    if(!c || c->is_full || !c->is_float) return;        // ADD ONLY FOR FLOAT
    if(dir == "up")
    {
        XMoveWindow(dpy, c->win, c->x, c->y + step);
        if(c->dec)
        {
            if(SIDE_DECORATION)
                XMoveWindow(dpy, c->dec, c->x - TITLE_HEIGHT, c->y + step);
            else
                XMoveWindow(dpy, c->dec, c->x, c->y + step - TITLE_HEIGHT);
        }
    }
    else if(dir == "down")
    {
        XMoveWindow(dpy, c->win, c->x, c->y - step);
        if(c->dec)
        {
            if(SIDE_DECORATION)
                XMoveWindow(dpy, c->dec, c->x - TITLE_HEIGHT, c->y - step);
            else
                XMoveWindow(dpy, c->dec, c->x, c->y - step - TITLE_HEIGHT);
        }
    }
    else if(dir == "left")
    {
        XMoveWindow(dpy, c->win, c->x - step, c->y);
        if(c->dec)
        {
            if(SIDE_DECORATION)
                XMoveWindow(dpy, c->dec, c->x - step - TITLE_HEIGHT, c->y);
            else
                XMoveWindow(dpy, c->dec, c->x - step, c->y - TITLE_HEIGHT);
        }
    }
    else if(dir == "right")
    {
        XMoveWindow(dpy, c->win, c->x + step, c->y);
        if(c->dec)
        {
            if(SIDE_DECORATION)
                XMoveWindow(dpy, c->dec, c->x + step - TITLE_HEIGHT, c->y);
            else
                XMoveWindow(dpy, c->dec, c->x + step, c->y - TITLE_HEIGHT);
        }
    }
}

void customwm::
resize_window_with_key(Client *c, string dir, int step)
{
    //log("Resizing window with key");
    if(!c || c->is_full || !c->is_float || !c->is_resizable) return;
    if(dir == "up")
    {
        if(c->h + step < sh)
            XResizeWindow(dpy, c->win, c->w, c->h + step);
        if(c->dec && SIDE_DECORATION)
            XResizeWindow(dpy, c->dec, TITLE_HEIGHT, c->h + step + 2*BORDER_WIDTH);
    }
    else if(dir == "down")
    {
        if(c->h - step > 0)
            XResizeWindow(dpy, c->win, c->w, c->h - step);
        if(c->dec && SIDE_DECORATION)
            XResizeWindow(dpy, c->dec, TITLE_HEIGHT, c->h - step + 2*BORDER_WIDTH);
    }
    else if(dir == "left")
    {
        if(c->w - step > 0)
        {
            XResizeWindow(dpy, c->win, c->w - step, c->h);
            if(c->dec && !SIDE_DECORATION)
                XResizeWindow(dpy, c->dec, c->w - step + 2*BORDER_WIDTH, TITLE_HEIGHT);
        }
    }
    else if(dir == "right")
        if(c->w + step < sw)
        {
            XResizeWindow(dpy, c->win, c->w + step, c->h);
            if(c->dec && !SIDE_DECORATION)
                XResizeWindow(dpy, c->dec, c->w + step + 2*BORDER_WIDTH, TITLE_HEIGHT);

        }
}

void customwm::
move_window_to_corner(Client *c, string corner)
{
    //log("Cornering client");
    if(!c || !c->is_float) return;
    c->oldh = c->h;
    c->oldw = c->w;
    c->w = sw/3;
    c->h = sh/2;
    if(corner == "top-left" || corner == "tl")
    {
        XMoveResizeWindow(dpy, c->win, 0, BORDER_WIDTH+(DECORATIONS_ON_FLOAT ? TITLE_HEIGHT : 0), c->w-2*BORDER_WIDTH, c->h);
        if(c->dec)
            XMoveResizeWindow(dpy, c->dec, 0, 0, c->w-2*BORDER_WIDTH, TITLE_HEIGHT);
    }
    else if(corner == "top-right" || corner == "tr")
    {
        XMoveResizeWindow(dpy, c->win, sw-c->w, BORDER_WIDTH+(DECORATIONS_ON_FLOAT ? TITLE_HEIGHT : 0), c->w-2*BORDER_WIDTH, c->h);
        if(c->dec)
            XMoveResizeWindow(dpy, c->dec, sw-c->w, 0, c->w-2*BORDER_WIDTH, TITLE_HEIGHT);
    }
    else if(corner == "bottom-left" || corner == "bl")
    {
        XMoveResizeWindow(dpy, c->win, 0, c->h + BORDER_WIDTH+(DECORATIONS_ON_FLOAT ? TITLE_HEIGHT : 0), c->w-2*BORDER_WIDTH, c->h-2*BORDER_WIDTH-(DECORATIONS_ON_FLOAT ? TITLE_HEIGHT : 0));
        if(c->dec)
            XMoveResizeWindow(dpy, c->dec, 0, c->h, c->w-2*BORDER_WIDTH, TITLE_HEIGHT);
    }
    else if(corner == "bottom-right" || corner == "br")
    {
        XMoveResizeWindow(dpy, c->win, sw-c->w, c->h + BORDER_WIDTH+(DECORATIONS_ON_FLOAT ? TITLE_HEIGHT : 0), c->w-2*BORDER_WIDTH, c->h-2*BORDER_WIDTH-(DECORATIONS_ON_FLOAT ? TITLE_HEIGHT : 0));
        if(c->dec)
            XMoveResizeWindow(dpy, c->dec, sw-c->w, c->h, c->w-2*BORDER_WIDTH, TITLE_HEIGHT);
    }
}

void customwm::
center_client(Client *c)
{
    if(!c || !c->is_float) return;
    //log("Centering client");
    XMoveWindow(dpy, c->win, (sw - c->w)/2, (sh - c->h)/2);
}

void customwm::
remove_window(Window w, uint desk)
{
    log("Removing window");
    Client *c;
    c = get_client_from_window(w);
    if(!c) return;
    if(head != NULL && head->next == NULL && head->prev == NULL)
    {
        free(head);
        head = NULL;
        current = NULL;
        return;
    }
    if(c->prev == NULL)
    {
        head = c->next;
        c->next->prev = NULL;
        current = c->next;
    }
    else if(c->next == NULL)
    {
            c->prev->next = NULL;
            current = c->prev;
    }
    else
    {
            c->prev->next = c->next;
            c->next->prev = c->prev;
            current = c->prev;
    }
    free(c);
}

void customwm::
add_window(Window w, uint desk, Client *copy)
{
    log("Adding window");
    Client *c, *t;
    if(!(c = (Client *) calloc(1, sizeof(Client))))
    {
        log("Unable to allocate memory for client");
        exit(1);
    }
    if(head == NULL)
    {
        c->next = NULL;
        c->prev = NULL;
        c->win = w;
        head = c;
    }
    else
    {
        if(ATTACH_MASTER)
        {
            t = head;
            c->win = w;
            c->prev = NULL;
            c->next = t;
            t->prev = c;
            head = c;
        }
        else
        {
            if(ATTACH_END)
            {
                for(t=head; t->next; t=t->next);
                t->next = c;
                c->win = w;
                c->prev = t;
                c->next = NULL;
            }
            else
            {
                t = head->next;
                c->win = w;
                c->prev = head;
                c->next = t;
                head->next = c;
            }
        }
    }
    if(copy)
        copy_client_prop(copy, c);
    current = c;
    save_desktop(current_desktop);
    XSelectInput(dpy, c->win, SLOPPY_FOCUS ? EnterWindowMask: PointerMotionMask);
    XGrabButton(dpy, Button1, AnyModifier, c->win, True, MOUSEMASK, GrabModeAsync, GrabModeAsync, None, None);
}

void customwm::
move_win_up()
{
    //log("Moving win up");
    Window tmp;
    Client *c;
    if(!current || current->is_float) return;
    tmp = current->win;
    c = get_prev_client_from_tiled(current->win);
    if(!c) return;
    current->win = c->win;
    c->win = tmp;
    current = c;
    update_current_client();
    applylayout();
}

void customwm::
move_win_down()
{
    //log("Moving win down");
    Window tmp;
    Client *c;
    if(!current || current->is_float) return;
    tmp = current->win;
    c = get_next_client_from_tiled(current->win);
    if(!c) return;
    current->win = c->win;
    c->win = tmp;
    current = c;
    update_current_client();
    applylayout();
}

void customwm::
focus_prev()
{
    Client *c;
    if(!current || current->is_full || !head->next) return;
    if(current->prev)
        current = current->prev;
    else if(!current->prev)
    {
        for(c=head; c->next; c=c->next);
        current = c;
    }
    update_current_client();
}

void customwm::
focus_next()
{
    if(!current || current->is_full || !head->next) return;
    if(current->next)
        current = current->next;
    else if(!current->next)
        current = head;
    update_current_client();
}
