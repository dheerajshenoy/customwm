
void customwm::
client_decoration_toggle(Client *c)
{
    if(!c || !c->dec) return;
    if(c->is_dec)
        XUnmapWindow(dpy, c->dec);
    else
        XMapWindow(dpy, c->dec);
    c->is_dec = !c->is_dec;
    ewmh_set_frame_extent(c);
}

void customwm::
client_decorations_create(Client *c)
{
    log("Setting client decoration");
    int x, y, w, h;
    x = c->x + BORDER_WIDTH;
    y = c->y + TITLE_HEIGHT;
    w = c->w + 2*BORDER_WIDTH;
    h = TITLE_HEIGHT;
    Window dec = XCreateSimpleWindow(dpy, root, x, y, w, h, 0,
            getcolor(TITLE_BACKGROUND.c_str()),
            getcolor(TITLE_BORDER.c_str()));
    c->dec = dec;
    c->is_dec = true;
    XSelectInput(dpy, c->dec, ExposureMask | EnterWindowMask);
    ewmh_set_frame_extent(c);
}

void customwm::
client_decorations_destroy(Client *c)
{
    if(!c) return;
    //log("Destroying client decoration");
    XUnmapWindow(dpy, c->dec);
    XDestroyWindow(dpy, c->dec);
    c->is_dec = false;
    ewmh_set_frame_extent(c);
}

void customwm::
tabbed_decorations(Client *c)
{
    log("Setting client decoration");
    int x, y, w, h;
    x = c->x + BORDER_WIDTH;
    y = c->y;
    w = c->w + 2*BORDER_WIDTH;
    h = TAB_HEIGHT;
    Window tabdec = XCreateSimpleWindow(dpy, root, x, y, w, h, 0,
            getcolor(TITLE_BACKGROUND.c_str()),
            getcolor(TITLE_BORDER.c_str()));
    c->dec = tabdec;
    ewmh_set_frame_extent(c);
}

