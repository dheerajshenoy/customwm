void customwm::
init_ewmh()
{
    //log("EWMH atoms LOADING!");
    utf8string = XInternAtom(dpy, "UTF8_STRING", false);
    wmatom[WMNormalHints] = XInternAtom(dpy, "WM_NORMAL_HINTS", false);
    wmatom[WMProtocols] = XInternAtom(dpy, "WM_PROTOCOLS", false);
    wmatom[WMTakeFocus] = XInternAtom(dpy, "WM_TAKE_FOCUS", false);
    wmatom[WMState] = XInternAtom(dpy, "WM_STATE", false);
    netatom[NetActiveWindow] = XInternAtom(dpy, "_NET_ACTIVE_WINDOW", false);
    netatom[NetSupported] = XInternAtom(dpy, "_NET_SUPPORTED", false);
    netatom[NetWMName] = XInternAtom(dpy, "_NET_WM_NAME", false);
    netatom[NetWMCheck] = XInternAtom(dpy, "_NET_SUPPORTING_WM_CHECK", false);
    netatom[NetWMState] = XInternAtom(dpy, "_NET_WM_STATE", false);
    netatom[NetWMFullscreen] = XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", false);
    netatom[NetWMSticky] = XInternAtom(dpy, "_NET_WM_STATE_STICKY", false);
    netatom[NetWMHidden] = XInternAtom(dpy, "_NET_WM_STATE_HIDDEN", false);
    netatom[NetWMFrameExtents] = XInternAtom(dpy, "_NET_FRAME_EXTENTS", false);
    netatom[NetWMWindowType] = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE", false);
    netatom[NetWMWindowTypeDock] = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE_DOCK", false);
    netatom[NetWMWindowTypeDialog] = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE_DIALOG", false);
    netatom[NetWMWindowTypeUtility] = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE_UTILITY", false);
    netatom[NetWMWindowTypeMenu] = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE_MENU", false);
    netatom[NetWMWindowTypeToolbar] = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE_TOOLBAR", false);
    netatom[NetWMWindowTypeSplash] = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE_SPLASH", false);
    netatom[NetShowDesktopMode] = XInternAtom(dpy, "_NET_SHOWING_DESKTOP", false);
    netatom[NetClientList] = XInternAtom(dpy, "_NET_CLIENT_LIST", false);
    netatom[NetCurrentDesktop]= XInternAtom(dpy, "_NET_CURRENT_DESKTOP", false);
    netatom[NetDesktopViewport] = XInternAtom(dpy, "_NET_DESKTOP_VIEWPORT", false);
    netatom[NetDesktopNames] = XInternAtom(dpy, "_NET_DESKTOP_NAMES", false);
    netatom[NetNumberOfDesktops] = XInternAtom(dpy, "_NET_NUMBER_OF_DESKTOPS", false);
    netatom[NetWMDesktop] = XInternAtom(dpy, "_NET_WM_DESKTOP", false);
    netatom[NetCloseWindow] = XInternAtom(dpy, "_NET_CLOSE_WINDOW", false);
    //log("EWMH atoms LOADED!");
}

void customwm::
ewmh_set_frame_extent(Client *c)
{
    ulong data[4];
    int left, right, top, bottom;
    //log("Setting frame extent");
    if(c->is_dec)
    {
        left = right = bottom = BORDER_WIDTH;
        top = TITLE_HEIGHT;
    }
    else left = right = top = bottom = 0;

    data[0] = left;
    data[1] = right;
    data[2] = top;
    data[3] = bottom;
    XChangeProperty(dpy, c->win, netatom[NetWMFrameExtents], XA_CARDINAL, 32,
            PropModeReplace, (unsigned char *) data, 4);
}

void customwm::
ewmh_set_client_list()
{
    log("Client List Updated");
    Client *c;
    XDeleteProperty(dpy, root, netatom[NetClientList]);
    for(int i=0; i<total_desktops; i++)
    for(c=desktop[i].head; c; c=c->next)
        XChangeProperty(dpy, root, netatom[NetClientList], XA_WINDOW, 32,
            PropModeAppend, (unsigned char *) &(c->win), 1);
}

void customwm::
ewmh_set_desktop(Client *c, int desktop)
{
    long data[] = { desktop };
    log("Updating desktop for client");
    XChangeProperty(dpy, c->win, netatom[NetWMDesktop],
            XA_CARDINAL, 32, PropModeReplace, (unsigned char *) data, 1);
}

void customwm::
ewmh_set_current_desktop()
{
    long data[] = { current_desktop };
    XChangeProperty(dpy, root, netatom[NetCurrentDesktop], XA_CARDINAL, 32, PropModeReplace, (unsigned char *) data, 1);
    XSync(dpy, false);
}

void customwm::
ewmh_init_number_of_desktops()
{
    log("Setting up number of desktops!");
    long data[] = { total_desktops };
    XChangeProperty(dpy, root, netatom[NetNumberOfDesktops], XA_CARDINAL, 32, PropModeReplace, (unsigned char *) data, 1);
}

void customwm::
ewmh_init_desktop_names()
{
    log("Setting up desktop names!");
    XTextProperty text;
    
    vector<const char *> workspaces;
    workspaces.reserve(wsp.size());
    for(const auto& s: wsp)
        workspaces.push_back(s.c_str());

    Xutf8TextListToTextProperty(dpy, (char **) workspaces.data(), total_desktops, XUTF8StringStyle, &text);
    XSetTextProperty(dpy, root, &text, netatom[NetDesktopNames]);
    XFree(text.value);
    workspaces.clear();
}

void customwm::
ewmh_init_viewport()
{
    log("Setting up desktop viewport!");
    long data[] = { 0,0 };
    XChangeProperty(dpy, root, netatom[NetDesktopViewport], XA_CARDINAL, 32, PropModeReplace, (unsigned char *) data, 2);
}

int customwm::
manage_xsend_icccm(Client *c, Atom atom)
{
    int n;
    Atom *protocols;
    int exists = 0;
    XEvent ev;

    if (XGetWMProtocols(dpy, c->win, &protocols, &n)) {
        while (!exists && n--)
            exists = protocols[n] == atom;
        XFree(protocols);
    }

    if (exists) {
        ev.type = ClientMessage;
        ev.xclient.window = c->win;
        ev.xclient.message_type = wmatom[WMProtocols];
        ev.xclient.format = 32;
        ev.xclient.data.l[0] = atom;
        ev.xclient.data.l[1] = CurrentTime;
        XSendEvent(dpy, c->win, False, NoEventMask, &ev);
    }

    return exists;
}

int customwm::
sendevent(Client *c, Atom proto)
{
    int n;
    Atom *protocols;
    int exists = 0;
    XEvent ev;
    if(XGetWMProtocols(dpy, c->win, &protocols, &n))
    {
        while(!exists && n--)
            exists = protocols[n] == proto;
        XFree(protocols);
    }
    if(exists)
    {
        ev.type = ClientMessage;
        ev.xclient.window = c->win;
        ev.xclient.message_type = wmatom[WMProtocols];
        ev.xclient.format = 32;
        ev.xclient.data.l[0] = proto;
        ev.xclient.data.l[1] = CurrentTime;
        XSendEvent(dpy, c->win, false, NoEventMask, &ev);
    }
    return exists;
}

void customwm::
send_kill_signal(Window w)
{ 
    XEvent ke;
    Client *c;
    c = get_client_from_window(w);
    if(!c) return;
    ke.type = ClientMessage;
    ke.xclient.window = w;
    ke.xclient.message_type = XInternAtom(dpy, "WM_PROTOCOLS", True);
    ke.xclient.format = 32;
    ke.xclient.data.l[0] = XInternAtom(dpy, "WM_DELETE_WINDOW", True);
    ke.xclient.data.l[1] = CurrentTime;
    XSendEvent(dpy, w, false, NoEventMask, &ke);
}

void customwm::
killclient(Client *c)
{
    log("Client killed");
        if(c && !sendevent(c, wmatom[WMDelete]))
        {
            XEvent ke;
            ke.type = ClientMessage;
            ke.xclient.window = c->win;
            ke.xclient.message_type = XInternAtom(dpy, "WM_PROTOCOLS", True);
            ke.xclient.format = 32;
            ke.xclient.data.l[0] = XInternAtom(dpy, "WM_DELETE_WINDOW", True);
            ke.xclient.data.l[1] = CurrentTime;
            XSendEvent(dpy, c->win, false, NoEventMask, &ke);
            send_kill_signal(c->win);
        }
}

void customwm::
get_size_hints(Client *c)
{
    log("Getting size hints");
    XSizeHints *hints_return = XAllocSizeHints();
    long *supplied_return = (long *) malloc(sizeof(long));
    XGetWMNormalHints(dpy, c->win, hints_return, supplied_return);
    uint minh, maxh, minw, maxw;
    minh = hints_return->min_height;
    minw = hints_return->min_width;
    maxh = hints_return->max_height;
    maxw = hints_return->max_width;
    if(minw == maxw && minh == maxh && maxw !=0 && maxh !=0)
        c->is_resizable = false;
    else
        c->is_resizable = true;
    c->maxh = maxh;
    c->maxw = maxw;
    XFree(hints_return);
    return;
}

