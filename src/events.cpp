// MAP NOTIFY
void customwm::
map_notify(XEvent *e)
{
    /*log("Map Notify");
    XMapEvent *ev = &e->xmap;
    Window trans = None;
    XGetTransientForHint(dpy, ev->window, &trans);
    if(trans != None)
        XSetInputFocus(dpy, trans, RevertToParent, CurrentTime);*/
}

// MAP REQUEST
void customwm::
map_request(XEvent *e)
{
    log("Mapping Window");
    if(show_mode)
    {
        log("Desktop mode is active, not mapping the requested client");
        system("notify-send 'WM in desktop Mode'");
        return;
    }
    XMapRequestEvent *ev = &e->xmaprequest;
    XWindowAttributes wa;
    XGetWindowAttributes(dpy, ev->window, &wa);
    unsigned long dl;
    char window_name[256];
    string wmname;
    int di, x, y, status, unmanage;
    unsigned char *prop_ret = NULL;
    uint dui;
    Atom da, prop;
    Window w = ev->window, dw;
    status = XGetWindowProperty(dpy, w, netatom[NetWMWindowType], 0L, sizeof(Atom), false, XA_ATOM, &da, &di, &dl, &dl, &prop_ret);
    if(status == Success && prop_ret)
    {
        prop = ((Atom *)prop_ret)[0]; 
        if(prop == netatom[NetWMWindowTypeDock]
        || prop == netatom[NetWMWindowTypeToolbar]
        || prop == netatom[NetWMWindowTypeUtility]
        || prop == netatom[NetWMWindowTypeMenu]
        || prop == netatom[NetWMWindowTypeSplash]
        || prop == netatom[NetWMWindowTypeNotification])
        {
            XMapWindow(dpy, w);
            return;
        }
    }
    // Make sure we aren't mapping the same window twice
    for(Client *c=head; c; c=c->next)
        if(c->win == w) return;

    if(wa.override_redirect)
        return;

    Client *d;
    d = (Client *) calloc(1, sizeof(Client));
    d->win = ev->window;
    d->is_float = false;
    d->is_full = false;
    d->is_hidden = false;
    d->is_sticky = false;
    d->is_resizable = true;
    //d->is_urgent = false;
    d->is_dec = false;
    d->oldx = d->x = wa.x;
    d->oldy = d->y = wa.y;
    d->oldw = d->w = wa.width;
    d->oldh = d->h = wa.height;
    d->oldbw = d->bw = wa.border_width;
    d->desk = current_desktop;
    d->dec = None;
    apply_rules(d);
    get_size_hints(d);
    add_window(d->win, current_desktop);
    ewmh_set_desktop(d, d->desk);
    ewmh_set_client_list();
    client_to_desktop(d, d->desk);
    XMapWindow(dpy, d->win);
    XLowerWindow(dpy, d->win);
    copy_client_prop(d, current);
    if(prop == netatom[NetWMWindowTypeDialog] || prop == netatom[NetWMWindowTypeModal])
    {
        set_float(current, true, true);
    }
    if(d->is_float)
        set_float(d, true, true);
    else if(d->is_full)
        set_fullscreen(d, true);
    update_current_client();
    applylayout();
}

void customwm::
unmap_notify(XEvent *e)
{
    log("Unmap Notify");
    Client *c;
    XUnmapEvent *ev = &e->xunmap;
    c = get_client_from_window(ev->window);
    if(!c) return;
    if(c->dec) XDestroyWindow(dpy, c->dec);
}

void customwm::
configure_request(XEvent *e)
{
    log("Configure Request");
    Client *c;
    XWindowChanges wc;
    XConfigureRequestEvent *ev = &e->xconfigurerequest;
    c = get_client_from_window(ev->window);
    wc.x = ev->x;
    wc.y = ev->y;
    wc.width = ev->width;
    wc.height = ev->height;
    wc.border_width = ev->border_width;
    wc.sibling = ev->above;
    wc.stack_mode = ev->detail;
    XConfigureWindow(dpy, ev->window, ev->value_mask, &wc);
}

void customwm::
configure_notify(XEvent *e)
{
    XConfigureEvent *ev = &e->xconfigure;
    Client *c;
    c= get_client_from_window(ev->window);
    if(!c) return;
    log("configure notify");
    if(c->x == ev->x && c->y == ev->y && c->w == ev->width && c->h == ev->height) return;
    c->x = ev->x;
    c->y = ev->y;
    c->w = ev->width;
    c->h = ev->height;
}

void customwm::
client_message(XEvent *e)
{
    log("Received client message");
    XClientMessageEvent *cme = &e->xclient;
    Client *c = current;
    if(!c)
        return;
    if(cme->message_type == netatom[NetWMState])
    {
        if(cme->data.l[1] == netatom[NetWMFullscreen] || cme->data.l[2] == netatom[NetWMFullscreen])
            toggle_fullscreen(c);
        else if (cme->message_type == netatom[NetActiveWindow])
        {
            current = c;
            update_current_client();
        }

        else if (cme->message_type == netatom[NetWMDesktop])
        {
            c->desk = cme->data.l[0];
            applylayout();
        }
        else if (cme->message_type == netatom[NetCloseWindow])
            killclient(c);
    }
}

void customwm::
button_press(XEvent *e)
{
    log("Button Press");
    Client *c;
    XButtonPressedEvent *ev = &e->xbutton;
    c = get_client_from_window(ev->subwindow);
    if(!c) return;

    if(CLEANMASK(ev->state) == CLEANMASK(Mod4Mask))
    {
        if(current != c)
            current = c;
        if(!c->is_float)
            set_float(c, true);
        update_current_client();
        applylayout();
        XRaiseWindow(dpy, c->win);
        if(c->dec)
            XRaiseWindow(dpy, c->dec);
        if(ev->button == Button1)
            move_mouse();
        else if(ev->button == Button3)
            resize_mouse();
    }
    if(ev->button == Button1)
    {
        if(current != c)
            current = c;
        update_current_client();
    }
}

void customwm::
property_notify(XEvent *e)
{
    Client *c;
    Window trans;
    XPropertyEvent *ev = &e->xproperty;

    if(ev->state == PropertyDelete)
        return;
    c = get_client_from_window(ev->window);
    if(!c) return;
    switch(ev->atom)
    {
        default: break;
    }
}

void customwm::
enter_notify(XEvent *e)
{
    log("Enter Notify!");
    Client *c;
    XCrossingEvent *ev = &e->xcrossing;
    c = get_client_from_window(ev->window);
    if(!c) return;
    if(SLOPPY_FOCUS && c->win != current->win)
    {
        current = c;
        update_current_client();
        XAllowEvents(dpy, ReplayPointer, CurrentTime);
    }
}

void customwm::
key_press(XEvent *e)
{
    //log("KEY PRESS");
    KeySym keysym;
    XKeyEvent *ev;
    ev = &e->xkey;
    //keysym = XKeycodeToKeysym(dpy, (KeyCode)ev->keycode, 0);
    keysym = XkbKeycodeToKeysym(dpy, (KeyCode) ev->keycode, 0, 0);
    for(itr=KEYS.begin(); itr!=KEYS.end(); itr++)
        if(itr->keysym == keysym && CLEANMASK(itr->mod) == CLEANMASK(ev->state))
            spawn(itr->function, itr->arg);
}

void customwm::
destroy_notify(XEvent *e)
{

    log("Destroy Notify");
    uint i=0;
    Client *c;
    XDestroyWindowEvent *ev = &e->xdestroywindow;
    if(!ev->window) return;
    for(c=head; c; c=c->next)
        if(ev->window == c->win) i++;
    if(i == 0) return;
    remove_window(ev->window, current_desktop);
    ewmh_set_client_list();
    update_current_client();
    applylayout();
}
