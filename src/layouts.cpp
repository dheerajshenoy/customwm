
void customwm::
applylayout()
{
    log("Applying layout");
    if(LAYOUTS.at(current_layout) == "Tiled")
        layout_tiled();
    else if(LAYOUTS.at(current_layout) == "Monocle")
        layout_monocle();
    else if(LAYOUTS.at(current_layout) == "Column")
        layout_column();
    else if(LAYOUTS.at(current_layout) == "Tabbed")
        layout_tabbed();
    else if(LAYOUTS.at(current_layout) == "ColumnGridBottom")
        layout_column_grid_bottom();
    else if(LAYOUTS.at(current_layout) == "ColumnGridTop")
        layout_column_grid_top();
    else if(LAYOUTS.at(current_layout) == "Grid")
        layout_grid();
    else if(LAYOUTS.at(current_layout) == "Magnifier")
        layout_magnifier();
}

void customwm::
layout_grid()
{
    log("Grid layout");
    if(!head) return;
    Client *c;
    uint y, n=0, mfact = MASTER_FACTOR, 
        gaps = GAPS, new_y, new_h,
        t = 0, s;
    tiled_clients.clear();
    (SHOW_PANEL) ? y = PANEL_HEIGHT : y = 0;
    if(!head->next)
    {
            if(!head->is_float)
            {
                if(!SINGLE_CLIENT_GAPS)
                    gaps = 0;
                if(!SINGLE_CLIENT_BORDER)
                    XSetWindowBorderWidth(dpy, head->win, 0);
                update_geometry(head->win, gaps, gaps+y-2,
                    sw-2*gaps, sh-2*gaps-y);
            }
    }
    else
    {
        for(c=head; c; c=c->next)
            if(!c->is_float)
                tiled_clients.push_back(c);
        n = tiled_clients.size();
        if(n == 1)
        {
            if(!SINGLE_CLIENT_GAPS)
                gaps = 0;
            update_geometry(tiled_clients.at(0)->win, BORDER_WIDTH+gaps-3, BORDER_WIDTH+gaps+y-3,
                sw-2*BORDER_WIDTH-2*gaps, sh-2*BORDER_WIDTH-2*gaps-y);
        }
        else if(n > 1)
        {
            for(int i=0; i<n; i++)
            {
                if(i%2==0)
                {
                    update_geometry(tiled_clients.at(i)->win, BORDER_WIDTH+gaps+t,
                            BORDER_WIDTH+gaps+y, sw/n - 2*gaps - 2*BORDER_WIDTH,
                            sh/2-2*gaps-2*BORDER_WIDTH-y);
                }
                else
                {
                    update_geometry(tiled_clients.at(i)->win, BORDER_WIDTH+gaps+t,
                            sh/2+BORDER_WIDTH+gaps+y, sw/(n-1) - 2*gaps - 2*BORDER_WIDTH,
                            sh/2-2*gaps-2*BORDER_WIDTH-y);
                    t = t + sw/n;
                }
            }
        }
    }
}

void customwm::
layout_column()
{
    log("Column layout");
    if(!head) return;
    Client *c;
    uint y, n=0, mfact = MASTER_FACTOR, 
        gaps = GAPS, new_y, new_h,
        t = 0, s;
    tiled_clients.clear();
    (SHOW_PANEL) ? y = PANEL_HEIGHT : y = 0;
    if(!head->next)
    {
            if(!head->is_float)
            {
                if(!SINGLE_CLIENT_GAPS)
                    gaps = 0;
                if(!SINGLE_CLIENT_BORDER)
                    XSetWindowBorderWidth(dpy, head->win, 0);
                XMoveResizeWindow(dpy, head->win, gaps, gaps+y-2,
                    sw-2*gaps, sh-2*gaps-y);
            }
    }
    else
    {
        for(c=head; c; c=c->next)
            if(!c->is_float)
                tiled_clients.push_back(c);
        n = tiled_clients.size();
        if(n == 1)
        {
            if(!SINGLE_CLIENT_GAPS)
                gaps = 0;
            XMoveResizeWindow(dpy, tiled_clients.at(0)->win, BORDER_WIDTH+gaps-3, BORDER_WIDTH+gaps+y-3,
                sw-2*BORDER_WIDTH-2*gaps, sh-2*BORDER_WIDTH-2*gaps-y);
        }
        else if(n > 1)
        {
            for(int i=0; i<n; i++)
            {
                XMoveResizeWindow(dpy, tiled_clients.at(i)->win, gaps+BORDER_WIDTH-5,
                    BORDER_WIDTH+gaps+y+t-2, sw-2*gaps-2*BORDER_WIDTH+1,
                    sh/n - 2*gaps -2*BORDER_WIDTH - y/n);
                t = t + (sh-y)/n;
            }
        }
    }
}

void customwm::
layout_magnifier()
{
    log("Magnifier layout");
    if(!head) return;
    Client *c;
    uint y, n=0, mfact = MASTER_FACTOR, 
        gaps = GAPS, new_y, new_h,
        t = 0, s;
    tiled_clients.clear();
    (SHOW_PANEL) ? y = PANEL_HEIGHT-1 : y = 0;
    if(!head->next)
    {
            if(!head->is_float)
            {
                if(!SINGLE_CLIENT_GAPS)
                    gaps = 0;
                if(!SINGLE_CLIENT_BORDER)
                    XSetWindowBorderWidth(dpy, head->win, 0);
                XMoveResizeWindow(dpy, head->win, gaps, gaps+y-2,
                    sw-2*gaps, sh-2*gaps-y);
            }
    }
    else
    {
        for(c=head; c; c=c->next)
            if(!c->is_float)
                tiled_clients.push_back(c);
        if(tiled_clients.size() == 1)
        {
            if(!SINGLE_CLIENT_GAPS)
                gaps = 0;
            XMoveResizeWindow(dpy, tiled_clients.at(0)->win, BORDER_WIDTH+gaps-3, BORDER_WIDTH+gaps+y-3,
                sw-2*BORDER_WIDTH-2*gaps-2, sh-2*BORDER_WIDTH-2*gaps-y);
        }
        if(tiled_clients.size() > 1)
        {
            XMoveResizeWindow(dpy, tiled_clients.at(0)->win, BORDER_WIDTH+gaps-3, BORDER_WIDTH+gaps+y-2,
                    mfact-2*gaps-2*BORDER_WIDTH-1, sh-2*BORDER_WIDTH-2*gaps-y-2);
            n = tiled_clients.size() - 1;

            for(int i=1; i<tiled_clients.size(); i++)
            {
                XMoveResizeWindow(dpy, tiled_clients.at(i)->win, mfact+BORDER_WIDTH-4,
                        BORDER_WIDTH+gaps+y+t-3, sw-mfact-gaps-2*BORDER_WIDTH+1,
                        sh/n -gaps -2*BORDER_WIDTH - y/n);
                t = i * (sh-y-gaps)/n;
            }
        }
    }

}

void customwm::
layout_tabbed()
{}

void customwm::
layout_tiled()
{
    log("Tiled layout");
    if(!head) return;
    Client *c;
    uint y, n=0, mfact = MASTER_FACTOR, 
        gaps = GAPS, new_y, new_h,
        t = 0, s;
    tiled_clients.clear();
    (SHOW_PANEL) ? y = PANEL_HEIGHT-1 : y = 0;
    if(!head->next)
    {
            if(!head->is_float)
            {
                if(!SINGLE_CLIENT_GAPS)
                    gaps = 0;
                if(!SINGLE_CLIENT_BORDER)
                    XSetWindowBorderWidth(dpy, head->win, 0);
                update_geometry(head->win, gaps, gaps+y-2,
                    sw-2*gaps, sh-2*gaps-y);
            }
    }
    else
    {
        for(c=head; c; c=c->next)
            if(!c->is_float)
                tiled_clients.push_back(c);
        if(tiled_clients.size() == 1)
        {
            if(!SINGLE_CLIENT_GAPS)
                gaps = 0;
            update_geometry(tiled_clients.at(0)->win, BORDER_WIDTH+gaps-2, BORDER_WIDTH+gaps+y-3,
                sw-2*BORDER_WIDTH-2*gaps-3, sh-2*BORDER_WIDTH-2*gaps-y);
        }
        if(tiled_clients.size() > 1)
        {
            update_geometry(tiled_clients.at(0)->win, BORDER_WIDTH+gaps-2, BORDER_WIDTH+gaps+y-2,
                    mfact-2*gaps-2*BORDER_WIDTH-2, sh-2*BORDER_WIDTH-2*gaps-y-2);
            n = tiled_clients.size() - 1;

            for(int i=1; i<tiled_clients.size(); i++)
            {
                update_geometry(tiled_clients.at(i)->win, mfact+BORDER_WIDTH-4,
                        BORDER_WIDTH+gaps+y+t-3, sw-mfact-gaps-2*BORDER_WIDTH+1,
                        sh/n - 2*gaps -2*BORDER_WIDTH - y/n);
                t = i * (sh-y)/n;
            }
        }
    }
}

void customwm::
layout_monocle()
{
    log("Monocle layout");
    if(!head) return;
    Client *c;
    uint n=0, mfact = MASTER_FACTOR, 
        gaps = GAPS,
        y;
    (SHOW_PANEL) ? y = PANEL_HEIGHT : y = 0;
    for(c=head; c; c=c->next)
        if(!c->is_float)
            tiled_clients.push_back(c);
    for(int i=0; i < tiled_clients.size(); i++)
    {
        XMoveResizeWindow(dpy, tiled_clients.at(i)->win, BORDER_WIDTH, BORDER_WIDTH+y,
            sw-2*BORDER_WIDTH-2, sh-2*BORDER_WIDTH-y-2);
        XSetWindowBorderWidth(dpy, tiled_clients.at(i)->win, 0);
    }
}

void customwm::
layout_column_grid_top()
{
    log("Column Grid Top layout");
    if(!head) return;
    Client *c;
    uint y, n=0, mfact = MASTER_FACTOR, 
        gaps = GAPS, new_y, new_h,
        t = 0, s;
    tiled_clients.clear();
    (SHOW_PANEL) ? y = PANEL_HEIGHT : y = 0;
    if(!head->next)
    {
            if(!head->is_float)
            {
                if(!SINGLE_CLIENT_GAPS)
                    gaps = 0;
                if(!SINGLE_CLIENT_BORDER)
                    XSetWindowBorderWidth(dpy, head->win, 0);
                XMoveResizeWindow(dpy, head->win, gaps, gaps+y-2,
                    sw-2*gaps, sh-2*gaps-y);
            }
    }
    else
    {
        for(c=head; c; c=c->next)
            if(!c->is_float)
                tiled_clients.push_back(c);
        if(tiled_clients.size() == 1)
        {
            if(!SINGLE_CLIENT_GAPS)
                gaps = 0;
            XMoveResizeWindow(dpy, tiled_clients.at(0)->win, BORDER_WIDTH+gaps-3, BORDER_WIDTH+gaps+y-3,
                sw-2*BORDER_WIDTH-2*gaps, sh-2*BORDER_WIDTH-2*gaps-y);
        }

        if(tiled_clients.size() > 1)
        {
            n = tiled_clients.size() - 1;
            for(int i=1; i<tiled_clients.size(); i++)
            {
                XMoveResizeWindow(dpy, tiled_clients.at(i)->win, t + BORDER_WIDTH+gaps,
                        BORDER_WIDTH+gaps+y,
                        sw/n-2*gaps-2*BORDER_WIDTH, 
                        sh/2-2*BORDER_WIDTH-2*gaps);
                t = t + sw/n;
            }
            XMoveResizeWindow(dpy, tiled_clients.at(0)->win, BORDER_WIDTH+gaps-3, sh/2+gaps + y,
                    sw-2*gaps-2*BORDER_WIDTH-2, sh/2-2*BORDER_WIDTH-2*gaps -y);
        }
    }
}

void customwm::
layout_column_grid_bottom()
{
    log("Column Grid Bottom layout");
    if(!head) return;
    Client *c;
    uint y, n=0, mfact = MASTER_FACTOR, 
        gaps = GAPS, new_y, new_h,
        t = 0, s;
    tiled_clients.clear();
    (SHOW_PANEL) ? y = PANEL_HEIGHT : y = 0;
    if(!head->next)
    {
            if(!head->is_float)
            {
                if(!SINGLE_CLIENT_GAPS)
                    gaps = 0;
                if(!SINGLE_CLIENT_BORDER)
                    XSetWindowBorderWidth(dpy, head->win, 0);
                XMoveResizeWindow(dpy, head->win, gaps, gaps+y-2,
                    sw-2*gaps, sh-2*gaps-y);
            }
    }
    else
    {
        for(c=head; c; c=c->next)
            if(!c->is_float)
                tiled_clients.push_back(c);
        if(tiled_clients.size() == 1)
        {
            if(!SINGLE_CLIENT_GAPS)
                gaps = 0;
            XMoveResizeWindow(dpy, tiled_clients.at(0)->win, BORDER_WIDTH+gaps-3, BORDER_WIDTH+gaps+y-3,
                sw-2*BORDER_WIDTH-2*gaps, sh-2*BORDER_WIDTH-2*gaps-y);
        }

        if(tiled_clients.size() > 1)
        {
            XMoveResizeWindow(dpy, tiled_clients.at(0)->win, BORDER_WIDTH+gaps-3, BORDER_WIDTH+gaps+y,
                    sw-2*gaps-2*BORDER_WIDTH-2, sh/2-2*BORDER_WIDTH-2*gaps);
            n = tiled_clients.size() - 1;
            for(int i=1; i<tiled_clients.size(); i++)
            {
                XMoveResizeWindow(dpy, tiled_clients.at(i)->win, t + BORDER_WIDTH+gaps, sh/2+gaps+y,
                        sw/n-2*gaps-2*BORDER_WIDTH, 
                        sh/2-2*BORDER_WIDTH-2*gaps-y);
                t = t + sw/n;
            }
        }
    }
}

