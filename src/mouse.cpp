void customwm::
resize_mouse()
{
    int nh, nw;
    XEvent ev;
    Client *c;
    Time lasttime =0;
    if(!(c = current) || !c->is_resizable)
        return;
    if(c->is_full)
        return;
    c->oldx = c->x;
    c->oldy = c->y;
    c->oldw = c->w;
    c->oldh = c->h;
    if(XGrabPointer(dpy, root, false, MOUSEMASK, GrabModeAsync, GrabModeAsync, None,
                ResizeCursor, CurrentTime) != GrabSuccess)
        return;
    do {
        XMaskEvent(dpy, MOUSEMASK|ExposureMask|SubstructureRedirectMask, &ev);
        switch(ev.type)
        {
            case MotionNotify:
                if((ev.xmotion.time - lasttime) <= 1000/60)
                    continue;
                lasttime = ev.xmotion.time;
                nw = MAX(ev.xmotion.x - c->oldx -2 * BORDER_WIDTH + 1, 1);
                nh = MAX(ev.xmotion.y - c->oldy -2 * BORDER_WIDTH + 1, 1);
                update_geometry(c->win, c->x, c->y, nw, nh);
                if(c->dec)
                {
                    if(!SIDE_DECORATION)
                        XMoveResizeWindow(dpy, c->dec, c->x, c->y - TITLE_HEIGHT, nw + 2*BORDER_WIDTH, TITLE_HEIGHT);
                    else
                        XMoveResizeWindow(dpy, c->dec, c->x - TITLE_HEIGHT, c->y, TITLE_HEIGHT, nh + 2*BORDER_WIDTH);
                }
        }
    } while(ev.type != ButtonRelease);
    XUngrabPointer(dpy, CurrentTime);
}

void customwm::
move_mouse()
{
    int nx, ny, x, y;
    XEvent ev;
    Client *c;
    Time lasttime =0;
    Window dummy;
    int di;
    uint dui;
    
    if(!(c = current))
        return;
    if(c->is_full)
        return;
    c->oldx = c->x;
    c->oldy = c->y;
    c->oldw = c->w;
    c->oldh = c->h;
    if(XGrabPointer(dpy, root, false, MOUSEMASK, GrabModeAsync, GrabModeAsync, None,
                MoveCursor, CurrentTime) != GrabSuccess)
        return;
    if(!(XQueryPointer(dpy, root, &dummy, &dummy, &x, &y, &di, &di, &dui)))
        return;

    do {
        XMaskEvent(dpy, MOUSEMASK|ExposureMask|SubstructureRedirectMask, &ev);
        switch(ev.type)
        {
            case MotionNotify:
                if((ev.xmotion.time - lasttime) <= 1000/60)
                    continue;
                lasttime = ev.xmotion.time;
                nx = c->oldx + (ev.xmotion.x - x);
                ny = c->oldy + (ev.xmotion.y - y);
                XMoveResizeWindow(dpy, c->win, nx, ny, c->w, c->h);
                if(c->dec)
                {
                    if(!SIDE_DECORATION)
                        XMoveResizeWindow(dpy, c->dec, nx, ny - TITLE_HEIGHT, c->w + 2*BORDER_WIDTH, TITLE_HEIGHT);
                    else
                        XMoveResizeWindow(dpy, c->dec, nx - TITLE_HEIGHT, ny, TITLE_HEIGHT, c->h + 2*BORDER_WIDTH);
                }
        }
    } while(ev.type != ButtonRelease);
    XUngrabPointer(dpy, CurrentTime);
    update_current_client();
}

