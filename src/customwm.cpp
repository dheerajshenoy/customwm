#include "customwm.hpp"

void customwm::
setup()
{
    logfile.open("/home/redundant/customwm.log", ios::out | ios::trunc);
    //log("SETUP");
    if(!(dpy = XOpenDisplay(NULL)))
    {
        log("Unable to open display");
        exit(1);
    }
    screen = XDefaultScreen(dpy);
    root = RootWindow(dpy, screen);
    sw = XDisplayWidth(dpy, screen);
    sh = XDisplayHeight(dpy, screen);
    exist = 0;
    init_ewmh();
    read_config();
    for(int i=0; i<total_desktops; i++)
        desktop[i] = (Desktop) { .head = head,
                                 .current = current,
                                 .layout = current_layout,
                                 .gaps = GAPS,
                                 .show_panel = SHOW_PANEL,
                                 .mfact = MASTER_FACTOR };
    grab_buttons();
    checkwin = XCreateSimpleWindow(dpy, root, 0, 0, 1, 1, 0, 0, 0);
    XChangeProperty(dpy, checkwin, netatom[NetWMCheck], XA_WINDOW, 32,
            PropModeReplace, (unsigned char *) &checkwin, 1);
    XChangeProperty(dpy, checkwin, netatom[NetWMName], utf8string, 8,
            PropModeReplace, (unsigned char *) "customwm", 9);
    XChangeProperty(dpy, checkwin, netatom[NetWMCheck], XA_WINDOW, 32,
            PropModeReplace, (unsigned char *) &checkwin, 1);
    XChangeProperty(dpy, root, netatom[NetSupported], XA_ATOM, 32,
            PropModeReplace, (unsigned char *) netatom, NetLast);
    
    ewmh_set_current_desktop();
    ewmh_init_number_of_desktops();
    ewmh_init_viewport();
    ewmh_init_desktop_names();
    XSelectInput(dpy, root,
            SubstructureRedirectMask |
            SubstructureNotifyMask |
            KeyPressMask |
            ButtonPressMask |
            ButtonReleaseMask |
            PointerMotionMask |
            EnterWindowMask |
            LeaveWindowMask |
            PropertyChangeMask
            );
    s.subwindow = None;
    loop();
}

void customwm::
set_grouped(Client *c, bool s)
{



}

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
setup_restartable()
{
    log("Restarting WM");
    read_config();
    update_current_client();
    applylayout();
}

void customwm::
apply_rules(Client *c)
{
    log("RULES");
    string Class, instance;
    XClassHint ch = {NULL, NULL};
    c->is_float = 0;
    //c->monitor = -1;
    XGetClassHint(dpy, c->win, &ch);
    Class = ch.res_class ? ch.res_class : "broken";
    instance = ch.res_name ? ch.res_name : "broken";
    for(int i=0; i<RULES.size(); i++)
        if(RULES.at(i).Class == "^N" || RULES.at(i).Class == Class)
            if(RULES.at(i).title == instance || RULES.at(i).title == "^N")
            {
                if(RULES.at(i).desktop > 0)
                    c->desk = RULES.at(i).desktop - 1;
                if(RULES.at(i).floating == 1)
                    c->is_float = true;
            }
    if(ch.res_name)
        XFree(ch.res_name);
    if(ch.res_class)
        XFree(ch.res_class);
}

void customwm::
init_ewmh()
{
    //log("EWMH atoms LOADING!");
    utf8string = XInternAtom(dpy, "UTF8_STRING", false);
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
read_config()
{
    log("Reading config");
    iniparser config;
    config.Open("/home/redundant/.config/customwm/config.ini", "[]", "=");
    // CLIENT 
    BORDER_WIDTH = stoi(config.getValue("BORDER_WIDTH", "CLIENT", "0").at(0));
    TITLE_HEIGHT = stoi(config.getValue("TITLE_HEIGHT", "CLIENT", "0").at(0));
    GAPS = stoi(config.getValue("GAPS", "WM", "2").at(0));
    PANEL_HEIGHT = stoi(config.getValue("PANEL_HEIGHT", "PANEL", "30").at(0));
    MASTER_FACTOR = stof(config.getValue("MASTER_FACTOR", "WM", "0.55").at(0)) * sw;
    if(config.getValue("SINGLE_CLIENT_BORDER", "WM", "false").at(0) == "true")
        SINGLE_CLIENT_BORDER = true;
    else
        SINGLE_CLIENT_BORDER = false;

    if(config.getValue("DECORATIONS_ON_FLOAT", "WM", "false").at(0) == "true")
        DECORATIONS_ON_FLOAT = true;
    else
        DECORATIONS_ON_FLOAT = false;

    if(config.getValue("SINGLE_CLIENT_GAPS", "WM", "false").at(0) == "true")
        SINGLE_CLIENT_GAPS = true;
    else
        SINGLE_CLIENT_GAPS = false;

    if(config.getValue("SLOPPY_FOCUS", "WM", "true").at(0) == "true")
        SLOPPY_FOCUS = true;
    else
        SLOPPY_FOCUS = false;

    if(config.getValue("ATTACH_MASTER", "WM", "false").at(0) == "true")
        ATTACH_MASTER = true;
    else
        ATTACH_MASTER = false;

    if(config.getValue("ATTACH_END", "STACK", "true").at(0) == "true")
        ATTACH_END = true;
    else
        ATTACH_END = false;

    if(config.getValue("SHOW_PANEL", "PANEL", "true").at(0) == "true")
        SHOW_PANEL = true;
    else
        SHOW_PANEL = false;

    //COLOR
    ACTIVE_BORDER = config.getValue("ACTIVE_BORDER", "COLOR", "#FF5000").at(0);
    INACTIVE_BORDER = config.getValue("INACTIVE_BORDER", "COLOR", "#444444").at(0);
    URGENT_BORDER = config.getValue("URGENT_BORDER", "COLOR", "#FF2200").at(0);
    FLOATING_BORDER = config.getValue("FLOATING_BORDER", "COLOR", "#FFFFFF").at(0);
    STICKY_BORDER = config.getValue("STICKY_BORDER", "COLOR", "#FA2").at(0);
    TITLE_BORDER = config.getValue("TITLE_BORDER", "COLOR", "#FF5000").at(0);
    TITLE_BACKGROUND = config.getValue("TITLE_BG", "COLOR", "#FF5000").at(0);
    FIXED_BORDER = config.getValue("FIXED_BORDER", "COLOR", "#FF7EAC").at(0);
    GROUPED_BORDER = config.getValue("GROUPED_BORDER", "COLOR", "#AFF").at(0);
    
    LAYOUTS = split(config.getValue("LAYOUTS", "WM").at(0), ",");
    if(LAYOUTS.size() == 0)
        LAYOUTS = { "Tiled", "Monocle", "Column", "Accordian", "Binary" };
    K.clear();
    P.clear();
    RULES.clear();
    KEYS.clear();
    if(!exist)
    {
        PROGRAMS = config.getLines("AUTOSTART");
        if(PROGRAMS.at(0) == "-") goto gohere;
        for(int i=0; i<PROGRAMS.size(); i++)
        {
            log(PROGRAMS.at(i));
            if(exist)
                break;
            system(PROGRAMS.at(i).c_str());
        }
        exist = 1;
    }
    gohere:
    P = config.getKeyValuePairTillNextSection("RULES", "=");
    for(int i=0; i<P.size(); i++)
        RULES.push_back({P.at(i).at(0), P.at(i).at(1), stoi(P.at(i).at(2)), stoi(P.at(i).at(3)), stoi(P.at(i).at(4))});
    K = config.getKeyValuePairTillNextSection("KEYS", "=");
    read_keys(K);
    config.Close();
}

void customwm::
read_keys(vector<vector<string>> K)
{
    //log("Reading keys");
    XUngrabKey(dpy, AnyKey, AnyModifier, root);
    KeySym keysym;
    vector<string>sp;
    string mod, key, func, arg, temp;
    unsigned int modifier;
    for(int i=0; i<K.size(); i++) { 
        modifier = 0;
        temp = K.at(i).at(0);
        sp = split(temp, "+");
        for(int i=0; i<sp.size(); i++) {
            if(sp.at(i) == "mod4" || sp.at(i) == "MOD4")
                modifier |= Mod4Mask;
            else if(sp.at(i) == "mod1" || sp.at(i) == "MOD1")
                modifier |= Mod1Mask;
            else if(sp.at(i) == "shift" || sp.at(i) == "Shift")
                modifier |= ShiftMask;
            else if(sp.at(i) == "ctrl" || sp.at(i) == "Ctrl" || sp.at(i) == "Control" || sp.at(i) == "control")
                modifier |= ControlMask;
            else {
                key = sp.at(i);
            }
        }
        func = K.at(i).at(1);
        if(K.at(i).size() > 2)
            arg = K.at(i).at(2);
        else
            arg = "";
        //const char *f = key.c_str();
        keysym = XStringToKeysym(key.c_str());
        KEYS.push_back({modifier, keysym, func, arg});
        XGrabKey(dpy, XKeysymToKeycode(dpy, keysym), modifier, root, True, GrabModeAsync, GrabModeAsync);
    }
    log("Keys added!");
}

void customwm::
save_desktop(int i)
{
    desktop[i].mfact = MASTER_FACTOR;
    desktop[i].layout = current_layout;
    desktop[i].gaps = GAPS;
    desktop[i].head = head;
    desktop[i].current = current;
    desktop[i].show_panel = SHOW_PANEL;
}

void customwm::
select_desktop(int i)
{
    head = desktop[i].head;
    current = desktop[i].current;
    GAPS = desktop[i].gaps;
    MASTER_FACTOR = desktop[i].mfact;
    current_layout = desktop[i].layout;
    SHOW_PANEL = desktop[i].show_panel;
    current_desktop = i;
    ewmh_set_current_desktop();
}

void customwm::
send_stickies(int desk)
{
}

void customwm::
change_desktop(int desk)
{
    log("Changing desktop");
    if(desk == current_desktop)
        return;
    Client *c, *t;
    if(head)
    {
        for(c=head; c; c=c->next)
        {
            XUnmapWindow(dpy, c->win);
        }
    }
    save_desktop(current_desktop);
    select_desktop(desk);
    save_desktop(desk);
    if(head)
    {
        for(c=head; c; c=c->next)
        {
            XMapWindow(dpy, c->win);
            if(c->is_float)
                set_float(c, true);
            else if(c->is_full)
                set_fullscreen(c, true);
        }
    }
    current_desktop = desk;
    ewmh_set_client_list();
    update_current_client();
    applylayout();
}

void customwm::
client_to_desktop(Client *c, int desk)
{
    Client *d;
    log("Sending client to desktop " + to_string(desk));
    if(!c || desk == current_desktop) return;
    int tmp2 = current_desktop;
    select_desktop(desk);
    if(DECORATIONS_ON_FLOAT && c->dec)
    {
        client_decorations_destroy(c);
    }
    add_window(c->win);
    XMoveResizeWindow(dpy, c->win, 10, 10, 100, 100);
    c->desk = desk;
    ewmh_set_desktop(c, c->desk);
    save_desktop(desk);
    select_desktop(tmp2);
    XUnmapWindow(dpy, c->win);
    remove_window(c->win);
    save_desktop(tmp2);
    ewmh_set_client_list();
    applylayout();
}

vector<string> customwm::
split(string STR, string DELIM)
{
    vector<string> temp;
    int start = 0;
    int end = STR.find(DELIM);
    if(end != -1) {
        while(end != -1) {
            temp.push_back(STR.substr(start, end-start));
            start = end + DELIM.size();
            end = STR.find(DELIM, start);
        }
        temp.push_back(STR.substr(start, end-start));
        return temp;
    }
    return {};
}

void customwm::
loop()
{
    //log("Event loop");
    XEvent ev;
    for(;;)
    {
        XNextEvent(dpy, &ev);
        switch(ev.type)
        {
            case ConfigureRequest:
                configure_request(&ev);
                break;
            case MapRequest:
                map_request(&ev);
                break;
            case ConfigureNotify:
                configure_notify(&ev);
                break;
            case DestroyNotify:
                destroy_notify(&ev);
                break;
            case UnmapNotify:
                unmap_notify(&ev);
                break;
            case MapNotify:
                map_notify(&ev);
            case ButtonPress:
                button_press(&ev);
                break;
            case KeyPress:
                key_press(&ev);
                break;
            case EnterNotify:
                enter_notify(&ev);
                break;
            case ClientMessage:
                client_message(&ev);
                break;
        }
    }
}

void customwm::
grab_buttons()
{
    //log("Grabbing buttons");
    XGrabButton(dpy, Button1, Mod4Mask, root, True, MOUSEMASK, GrabModeAsync,GrabModeAsync, None, None);
    XGrabButton(dpy, Button2, Mod4Mask, root, True, MOUSEMASK, GrabModeAsync,GrabModeAsync, None, None);
    XGrabButton(dpy, Button3, Mod4Mask, root, True, MOUSEMASK, GrabModeAsync,GrabModeAsync, None, None);
}
//EVENTS

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
    keysym = XKeycodeToKeysym(dpy, (KeyCode)ev->keycode, 0);
    for(itr=KEYS.begin(); itr!=KEYS.end(); itr++)
        if(itr->keysym == keysym && CLEANMASK(itr->mod) == CLEANMASK(ev->state))
            spawn(itr->function, itr->arg);
}

void customwm::
destroy_notify(XEvent *e)
{

    log("Destroy Notify");
    int i=0;
    Client *c;
    XDestroyWindowEvent *ev = &e->xdestroywindow;

    if(!ev->window)
        return;

    for(c=head; c; c=c->next)
        if(ev->window == c->win)
            i++;
    
    if(i == 0)
    { 
        return;
    }
    save_desktop(current_desktop);
    remove_window(ev->window);
    if(current->dec)
        client_decorations_destroy(current);
    ewmh_set_client_list();
    update_current_client();
    applylayout();
}


void customwm::
map_notify(XEvent *e)
{
    log("Map Notify");
    XMapEvent *ev = &e->xmap;
    Window trans = None;
    XGetTransientForHint(dpy, ev->window, &trans);
    if(trans != None)
        XSetInputFocus(dpy, trans, RevertToParent, CurrentTime);
}

void customwm::
unmap_notify(XEvent *e)
{
    log("Unmap Notify");
    if(show_mode) return;
    XUnmapEvent *ev = &e->xunmap;
    Client *c;
    c = get_client_from_window(ev->window);
    if(!c) return;
    if(DECORATIONS_ON_FLOAT && c->dec)
    {
        XDestroyWindow(dpy, c->dec);
        c->is_dec = false;
    }
    remove_window(c->win);
    ewmh_set_client_list();
    update_current_client();
    applylayout();
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
    /*c->oldx = c->x;
    c->oldy = c->y;
    c->oldw = c->w;
    c->oldh = c->h;*/
    c->x = ev->x;
    c->y = ev->y;
    c->w = ev->width;
    c->h = ev->height;
    /*if(DECORATIONS_ON_FLOAT && c->dec)
        XMoveResizeWindow(dpy, c->dec, c->x, c->y + BORDER_WIDTH - TITLE_HEIGHT, c->w + 
        2*BORDER_WIDTH, TITLE_HEIGHT);*/
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

        /*else if (cme->message_type == netatom[NetActiveWindow])
            if(!c->is_urgent)
                set_urgent(c);*/

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
    //log("Button Press");
    Client *c;
    XButtonPressedEvent *ev = &e->xbutton;
    if(ev->window == None) return;
    c = get_client_from_window(ev->subwindow);
    if(!c) return;
    if(CLEANMASK(ev->state) == CLEANMASK(Mod4Mask))
    {
        if(current != c)
            current = c;
        if(!c->is_float)
            set_float(c, true);
        applylayout();
        if(ev->button == Button1)
        {
            move_mouse();
        }
        else if(ev->button == Button3)
            resize_mouse();
    }
}

void customwm::
resize_mouse()
{
    int nh, nw;
    XEvent ev;
    Client *c;
    Time lasttime =0;
    
    if(!(c = current))
        return;
    if(c->is_full)
        return;
    c->oldx = c->x;
    c->oldy = c->y;
    c->oldw = c->w;
    c->oldh = c->h;
    if(XGrabPointer(dpy, root, false, MOUSEMASK, GrabModeAsync, GrabModeAsync, None,
                None, CurrentTime) != GrabSuccess)
        return;
    //XWarpPointer(dpy, None, c->win, 0, 0, 0, 0, c->w + BORDER_WIDTH - 1, c->h + BORDER_WIDTH - 1);
    do {
        XMaskEvent(dpy, MOUSEMASK|ExposureMask|SubstructureRedirectMask, &ev);
        switch(ev.type)
        {
            case ConfigureRequest:
                configure_request(&ev);
            case MapRequest:
                map_request(&ev);
            case MotionNotify:
                if((ev.xmotion.time - lasttime) <= 1000/60)
                    continue;
                lasttime = ev.xmotion.time;
                nw = MAX(ev.xmotion.x - c->oldx -2 * BORDER_WIDTH + 1, 1);
                nh = MAX(ev.xmotion.y - c->oldy -2 * BORDER_WIDTH + 1, 1);
                XMoveResizeWindow(dpy, c->win, c->x, c->y, nw, nh);
                if(c->dec)
                    XMoveResizeWindow(dpy, c->dec, c->x, c->y - TITLE_HEIGHT, nw + 2*BORDER_WIDTH, TITLE_HEIGHT);
        }
    } while(ev.type != ButtonRelease);
    //XWarpPointer(dpy, None, c->win, 0, 0, 0, 0, c->oldw + BORDER_WIDTH -1, c->oldh + BORDER_WIDTH -1);
    XUngrabPointer(dpy, CurrentTime);
    update_current_client();
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
    unsigned int dui;
    
    if(!(c = current))
        return;
    if(c->is_full)
        return;
    c->oldx = c->x;
    c->oldy = c->y;
    c->oldw = c->w;
    c->oldh = c->h;
    if(XGrabPointer(dpy, root, false, MOUSEMASK, GrabModeAsync, GrabModeAsync, None,
                None, CurrentTime) != GrabSuccess)
        return;
    if(!(XQueryPointer(dpy, root, &dummy, &dummy, &x, &y, &di, &di, &dui)))
        return;

    do {
        XMaskEvent(dpy, MOUSEMASK|ExposureMask|SubstructureRedirectMask, &ev);
        switch(ev.type)
        {
            case ConfigureRequest:
                configure_request(&ev);
                break;
            case MapRequest:
                map_request(&ev);
                break;
            case MotionNotify:
                if((ev.xmotion.time - lasttime) <= 1000/60)
                    continue;
                lasttime = ev.xmotion.time;
                nx = c->oldx + (ev.xmotion.x - x);
                ny = c->oldy + (ev.xmotion.y - y);
                XMoveResizeWindow(dpy, c->win, nx, ny, c->w, c->h);
                if(c->dec)
                    XMoveResizeWindow(dpy, c->dec, nx, ny - TITLE_HEIGHT, c->w + 2*BORDER_WIDTH, TITLE_HEIGHT);
        }
    } while(ev.type != ButtonRelease);
    XUngrabPointer(dpy, CurrentTime);
    update_current_client();
}
void customwm::
map_request(XEvent *e)
{
    log("Mapping Window");
    if(show_mode)
    {
        log("Desktop mode is active, not mapping the requested client");
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
        || prop == netatom[NetWMWindowTypeMenu])
        {
            XMapRaised(dpy, w);
            return;
        }
    }

    if(wa.override_redirect)
        return;

    Client *d;
    d = (Client *) calloc(1, sizeof(Client));
    d->win = ev->window;
    d->is_float = false;
    d->is_full = false;
    d->is_hidden = false;
    d->is_sticky = false;
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
    //client_decorations_create(d);
    add_window(d->win);
    ewmh_set_desktop(d, d->desk);
    ewmh_set_client_list();
    client_to_desktop(d, d->desk);
    //XMoveResizeWindow(dpy, d->win, (sw-d->w)/2, (sh-d->h)/2, sw/2, sh/2);
    XMapWindow(dpy, d->win);
    if(prop == netatom[NetWMWindowTypeDialog])
        d->is_float = true;
    if(d->is_float)
        set_float(current, true);
    else if(d->is_full)
        set_fullscreen(current, true);
    update_current_client();
    applylayout();
}

// EVENTS

void customwm::
update_current_client()
{
    log("Updating current client");
    Client *c;
    if(!head) return;
    for(c=head; c; c=c->next)
    {
        XSetWindowBorderWidth(dpy, c->win, BORDER_WIDTH);
        if(current == c)
        {
            if(c->is_full || !head->next && !c->is_float)
                XSetWindowBorderWidth(dpy, c->win, 0);
            else
            {
                if(c->is_sticky)
                    set_border(dpy, c->win, STICKY_BORDER);
                else if(c->is_float && !c->is_grouped)
                {
                    set_border(dpy, c->win, FLOATING_BORDER);
                    if(DECORATIONS_ON_FLOAT && c->dec)
                    {
                        set_window_bg(dpy, c->dec, ACTIVE_BORDER);
                        XRaiseWindow(dpy, c->dec);
                    }
                }
                else if(c->is_fixed)
                {
                    set_border(dpy, c->win, FIXED_BORDER);
                    set_float(c, true, true);
                }
                else if(c->is_grouped)
                    set_border(dpy, c->win, GROUPED_BORDER);
                else
                    set_border(dpy, c->win, ACTIVE_BORDER);
            }
                    XRaiseWindow(dpy, c->win);
            XSetInputFocus(dpy, c->win, RevertToPointerRoot, CurrentTime);
            XChangeProperty(dpy, c->win, netatom[NetActiveWindow],
                    XA_WINDOW, 32, PropModeReplace, (unsigned char *) &(c->win),1);
            manage_xsend_icccm(c, wmatom[WMTakeFocus]);
        }
        else
        {
            if(c->dec)
            {
                set_window_bg(dpy, c->dec, INACTIVE_BORDER);
            }
            set_border(dpy, c->win, INACTIVE_BORDER);
        }
    }
    XSync(dpy, false);
}

ulong customwm::
getcolor(const char *color)
{
    XColor c;
    Colormap map = DefaultColormap(dpy, screen);
    if(!(XAllocNamedColor(dpy, map, color, &c, &c)))
    {
        log("Unable to allocate memory for color");
        exit(1);
    }
    return c.pixel;
}

Client* customwm::
get_client_from_window(Window w)
{
    Client *c;
    for(c=head; c; c=c->next)
    {
        if(c->win == w)
            return c;
    }
    return NULL;
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
    if(!c) return;
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
        for(int i=0; i<grouped_clients.size(); i++)
        {
            if(!grouped_clients.at(i)->is_float)
            {
                XMoveWindow(dpy, grouped_clients.at(i)->win, (sw - grouped_clients.at(i)->w)/2, (sh - grouped_clients.at(i)->h)/2);
            }
            grouped_clients.at(i)->is_float = !grouped_clients.at(i)->is_float;
            toggle_grouped(grouped_clients.at(i));
        }
    }
    else
    {
        if(!c->is_float)
        {
            c->oldx = c->x;
            c->oldy = c->y;
            c->oldw = c->w;
            c->oldh = c->h;
            c->w = c->w/2;
            c->h = c->h/2;
            c->x = (sw - c->w)/2;
            c->y = (sh - c->h)/2;
            if(DECORATIONS_ON_FLOAT)
            {
                if(!c->dec)
                    client_decorations_create(c);
                XMapWindow(dpy, c->dec);
                XMoveResizeWindow(dpy, c->dec, c->x, c->y - TITLE_HEIGHT,
                        c->w + 2*BORDER_WIDTH,
                        TITLE_HEIGHT);
                XMoveResizeWindow(dpy, c->win, c->x, c->y, c->w, c->h);
            }
            else
                XMoveResizeWindow(dpy, c->win, c->x, c->y, c->w, c->h);
            c->is_float = true;
        }
        else
        {
            if(DECORATIONS_ON_FLOAT && c->dec)
            {
                client_decoration_toggle(c);
            }
            c->is_float = false;
            c->x = c->oldx;
            c->y = c->oldy;
            c->w = c->oldw;
            c->h = c->oldh;
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
    if(f && move)
    {
        if(DECORATIONS_ON_FLOAT)
        {
            if(!c->dec)
                client_decorations_create(c);
            XMapWindow(dpy, c->dec);
        }
        XMoveWindow(dpy, c->win, (sw - c->w)/2, (sh - c->h)/2);
    }
    else if(f && DECORATIONS_ON_FLOAT)
    {
        if(!c->dec)
            client_decorations_create(c);
        XMapWindow(dpy, c->dec);
    }
    else if(!f)
    {
        if(c->dec && DECORATIONS_ON_FLOAT)
        {
            client_decoration_toggle(c);
        }
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

void customwm::
add_window(Window w)
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
    current = c;
    tail = c;
    save_desktop(current_desktop);
    XSelectInput(dpy, c->win, SLOPPY_FOCUS ? EnterWindowMask: PointerMotionMask);
}

void customwm::
remove_window(Window w)
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
log(string msg, int stick_out)
{
    if(stick_out)
        logfile << ">>>>>> " << msg << endl;
    else
        logfile << msg << endl;
}

void customwm::
spawn(string func, string arg)
{

    if(arg == "")
    {
        if(func == "KILLCLIENT")
            killclient(current);
        else if(func == "RESTART")
            setup_restartable();
        else if(func == "QUIT")
            cleanup();
        else if(func == "CENTER")
            center_client(current);
        else if(func == "SWAPMASTER")
            swap_master();
        else if(func == "DMODE")
            show_desktop();
        else if(func == "LAYOUT")
            change_layout();
        /*else if(func == "HIDE")
            hide_client(current);*/
    }
    else
    {
        if(func == "TOGGLE")
        {
            if(arg == "full")
                toggle_fullscreen(current);
            else if(arg == "fakefull")
                fake_fullscreen(current);
            else if(arg == "float")
                toggle_float(current);
            else if(arg == "sticky")
                toggle_sticky(current);
            else if(arg == "panel")
                toggle_panel();
            /*else if(arg == "fixed")
                toggle_fixed(current);*/
            /*else if(arg == "group")
                toggle_grouped(current);*/
        }
        else if(func == "CLDESKTOP")
            client_to_desktop(current, stoi(arg)-1);

        else if(func == "DESKTOP")
        {
            send_stickies(stoi(arg)-1);
            change_desktop(stoi(arg)-1);
        }
        else if(func == "MOVEWIN")
        {
            if(arg == "up")
                move_win_up();
            else if(arg == "down")
                move_win_down();
        }
        else if(func == "MOVEKEY")
            move_window_with_key(current, arg, 50);
        else if(func == "RESIZEKEY")
            resize_window_with_key(current, arg, 50);
        else if(func == "CORNER")
            move_window_to_corner(current, arg);
        else if(func == "SCRATCHPAD")
            scratchpad_spawn("Alacritty");
        else if(func == "FOCUS")
        {
            if(arg == "prev")
                focus_prev();
            else if(arg == "next")
                focus_next();
        }
        else if(func == "MASTERSIZE")
            change_master_size(stoi(arg));
        else if(func == "GAPS")
            change_gaps(stoi(arg));
    }
}

void customwm::
killclient(Client *c)
{
    log("Client killed");
        if(current && !sendevent(current, wmatom[WMDelete]))
        {
            XEvent ke;
            ke.type = ClientMessage;
            ke.xclient.window = current->win;
            ke.xclient.message_type = XInternAtom(dpy, "WM_PROTOCOLS", True);
            ke.xclient.format = 32;
            ke.xclient.data.l[0] = XInternAtom(dpy, "WM_DELETE_WINDOW", True);
            ke.xclient.data.l[1] = CurrentTime;
            XSendEvent(dpy, current->win, false, NoEventMask, &ke);
            send_kill_signal(current->win);
        }
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

void customwm::
cleanup()
{
    //log("Shutting WM");
    Client *c;
    for(int i=0; i<total_desktops; i++)
        for(c=desktop[i].head; c; c=c->next)
            killclient(c);
    for(auto p: tiled_clients)
        delete(p);
    for(auto p: fixed_clients)
        delete(p);
    tiled_clients.clear();
    fixed_clients.clear();
    free(current);
    free(head);
    XDestroyWindow(dpy, checkwin);
    XDeleteProperty(dpy, root, netatom[NetSupported]);
    XUngrabKey(dpy, AnyKey, AnyModifier, root);
    XCloseDisplay(dpy);
}

void customwm::
move_window_with_key(Client *c, string dir, int step)
{
    //log("Moving window with key");
    if(!c || c->is_full || !c->is_float) return;        // ADD ONLY FOR FLOAT
    if(dir == "up")
    {
        XMoveWindow(dpy, c->win, c->x, c->y + step);
        if(c->dec)
            XMoveWindow(dpy, c->dec, c->x, c->y + step - TITLE_HEIGHT);
    }
    else if(dir == "down")
    {
        XMoveWindow(dpy, c->win, c->x, c->y - step);
        if(c->dec)
            XMoveWindow(dpy, c->dec, c->x, c->y - step - TITLE_HEIGHT);
    }
    else if(dir == "left")
    {
        XMoveWindow(dpy, c->win, c->x - step, c->y);
        if(c->dec)
            XMoveWindow(dpy, c->dec, c->x - step, c->y - TITLE_HEIGHT);
    }
    else if(dir == "right")
    {
        XMoveWindow(dpy, c->win, c->x + step, c->y);
        if(c->dec)
            XMoveWindow(dpy, c->dec, c->x + step, c->y - TITLE_HEIGHT);
    }
}

void customwm::
resize_window_with_key(Client *c, string dir, int step)
{
    //log("Resizing window with key");
    if(!c || c->is_full || !c->is_float) return;
    if(dir == "up")
    {
        if(c->h + step < sh)
            XResizeWindow(dpy, c->win, c->w, c->h + step);
    }
    else if(dir == "down")
    {
        if(c->h - step > 0)
            XResizeWindow(dpy, c->win, c->w, c->h - step);
    }
    else if(dir == "left")
    {
        if(c->w - step > 0)
        {
            XResizeWindow(dpy, c->win, c->w - step, c->h);
            if(c->dec)
                XResizeWindow(dpy, c->dec, c->w - step + 2*BORDER_WIDTH, TITLE_HEIGHT);
        }
    }
    else if(dir == "right")
        if(c->w + step < sw)
        {
            XResizeWindow(dpy, c->win, c->w + step, c->h);
            if(c->dec)
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
scratchpad_spawn(string scratchkey)
{
}

int customwm::
get_text_prop(Window w, Atom atom, char *text, uint32_t size)
{
    XTextProperty name;
    char **list = NULL;
    int n;
    
    if(!text || size == 0)
        return 0;

    text[0] = '\0';
    if(!XGetTextProperty(dpy, w, &name, atom) || !name.nitems)
        return 0;
    if(name.encoding == XA_STRING)
        strncpy(text, (char *) name.value, size-1);
    else
    {
        if(XmbTextPropertyToTextList(dpy, &name, &list, &n) >= Success && n > 0 && *list)
        {
            strncpy(text, *list, size-1);
            XFreeStringList(list);
        }
    }
    text[size-1] = '\0';
    XFree(name.value);
    return 1;

}

void customwm::
change_layout()
{
    if(current_layout < LAYOUTS.size()-1)
        current_layout++;
    else
        current_layout = 0;
    applylayout();
}

void customwm::
applylayout()
{
    log("Applying layout");
    if(LAYOUTS.at(current_layout) == "Tiled")
        layout_tiled();
    else if(LAYOUTS.at(current_layout) == "Monocle")
        layout_monocle();
    else if(LAYOUTS.at(current_layout) == "Accordian")
        layout_accordian();
    else if(LAYOUTS.at(current_layout) == "Column")
        layout_column();
    else if(LAYOUTS.at(current_layout) == "Tabbed")
        layout_tabbed();
    else if(LAYOUTS.at(current_layout) == "Binary")
        layout_binary();
    else if(LAYOUTS.at(current_layout) == "Fibonacci")
        layout_fibonacci();
    else if(LAYOUTS.at(current_layout) == "ColumnGridBottom")
        layout_column_grid_bottom();
    else if(LAYOUTS.at(current_layout) == "ColumnGridTop")
        layout_column_grid_top();
    else if(LAYOUTS.at(current_layout) == "Grid")
        layout_grid();
    else if(LAYOUTS.at(current_layout) == "Deck")
        layout_deck();
}

void customwm::
layout_grid()
{
    log("Grid layout");
    if(!head) return;
    Client *c;
    int y, n=0, mfact = MASTER_FACTOR, 
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
                if(i%2==0)
                {
                    XMoveResizeWindow(dpy, tiled_clients.at(i)->win, BORDER_WIDTH+gaps+t,
                            BORDER_WIDTH+gaps+y, sw/n - 2*gaps - 2*BORDER_WIDTH,
                            sh/2-2*gaps-2*BORDER_WIDTH-y);
                }
                else
                {
                    XMoveResizeWindow(dpy, tiled_clients.at(i)->win, BORDER_WIDTH+gaps+t,
                            sh/2+BORDER_WIDTH+gaps+y, sw/(n-1) - 2*gaps - 2*BORDER_WIDTH,
                            sh/2-2*gaps-2*BORDER_WIDTH-y);
                    t = t + sw/n;
                }
            }
        }
    }
}

void customwm::
layout_deck()
{
    log("Deck layout");
    if(!head) return;
    Client *c;
    int y, n=0, mfact = MASTER_FACTOR, 
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
            XMoveResizeWindow(dpy, tiled_clients.at(0)->win, BORDER_WIDTH+gaps-3, BORDER_WIDTH+gaps+y,
                mfact-2*BORDER_WIDTH-2*gaps, sh-2*BORDER_WIDTH-2*gaps-y);
            n--;
            for(int i=n; i>0; i--)
            {
                XMoveResizeWindow(dpy, tiled_clients.at(i)->win, mfact+BORDER_WIDTH+gaps-3,
                    BORDER_WIDTH+gaps+y+10*(1-i),
                    (sw-mfact)-2*BORDER_WIDTH-2*gaps, sh-2*BORDER_WIDTH-2*gaps-y-10*(1-i));
            }
        }
    }
}

void customwm::
layout_accordian()
{}

void customwm::
layout_column()
{
    log("Column layout");
    if(!head) return;
    Client *c;
    int y, n=0, mfact = MASTER_FACTOR, 
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
layout_tabbed()
{
    log("Tabbed layout");
    if(!head) return;
    Client *c;
    int y, n=0, mfact = MASTER_FACTOR, 
        gaps = GAPS, new_y, new_h,
        t = 0, s;
    tiled_clients.clear();
    (SHOW_PANEL) ? y = PANEL_HEIGHT : y = 0;
    if(!head->next)
    {
            if(!head->is_float)
            {
                tabbed_decorations(head);
                XMapWindow(dpy, head->dec);
                if(!SINGLE_CLIENT_GAPS)
                    gaps = 0;
                if(!SINGLE_CLIENT_BORDER)
                    XSetWindowBorderWidth(dpy, head->win, 0);
                XMoveResizeWindow(dpy, head->win, gaps, gaps+y+TAB_HEIGHT,
                    sw-2*gaps, sh-2*gaps-y-TAB_HEIGHT);
            }
    }
    else
    {
        for(c=head; c; c=c->next)
            if(!c->is_float)
                tiled_clients.push_back(c);
        if(n == 1)
        {
            if(!SINGLE_CLIENT_GAPS)
                gaps = 0;
            tabbed_decorations(tiled_clients.at(0));
            XMapWindow(dpy, tiled_clients.at(0)->dec);
            XMoveResizeWindow(dpy, tiled_clients.at(0)->win, BORDER_WIDTH+gaps-3,
                BORDER_WIDTH+gaps+y+TAB_HEIGHT,
                sw-2*BORDER_WIDTH-2*gaps, sh-2*BORDER_WIDTH-2*gaps-y-TAB_HEIGHT);
        }
        if(n > 1)
        {
            for(int i=0; i<n; i++)
            {
                XMoveResizeWindow(dpy, tiled_clients.at(i)->win, 0, TAB_HEIGHT, sw, sh-TAB_HEIGHT);
                /*XMoveResizeWindow(dpy, tiled_clients.at(i)->dec, t,
                        tiled_clients.at(i)->y - TAB_HEIGHT,
                        sw/n,
                        TAB_HEIGHT);*/
                t = t + sw/n;
            }
            
        }
    }
}

void customwm::
layout_tiled()
{
    log("Tiled layout");
    if(!head) return;
    Client *c;
    int y, n=0, mfact = MASTER_FACTOR, 
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
            XMoveResizeWindow(dpy, tiled_clients.at(0)->win, BORDER_WIDTH+gaps-3, BORDER_WIDTH+gaps+y-2,
                    mfact-2*gaps-2*BORDER_WIDTH-2, sh-2*BORDER_WIDTH-2*gaps-y-2);
            n = tiled_clients.size() - 1;

            for(int i=1; i<tiled_clients.size(); i++)
            {
                XMoveResizeWindow(dpy, tiled_clients.at(i)->win, mfact+gaps+BORDER_WIDTH-5,
                        BORDER_WIDTH+gaps+y+t-2, sw-mfact-2*gaps-2*BORDER_WIDTH+1,
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
    int n=0, mfact = MASTER_FACTOR, 
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
layout_binary()
{
    log("Binary layout");
    if(!head) return;
    Client *c;
    int y, n=0, mfact = MASTER_FACTOR, 
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
        else if(tiled_clients.size() > 1)
        {
            for(int i=1; i < tiled_clients.size(); i++)
            {
                if(i%2!=0)
                    XMoveResizeWindow(dpy, tiled_clients.at(i)->win, BORDER_WIDTH+gaps,
                            BORDER_WIDTH+gaps+y,
                            mfact-2*gaps-2*BORDER_WIDTH,
                            sh-2*gaps-2*BORDER_WIDTH-y);
                else
                    XMoveResizeWindow(dpy, tiled_clients.at(i)->win, mfact+BORDER_WIDTH+gaps,
                            BORDER_WIDTH+gaps+y,
                            (sw-mfact)-2*gaps-2*BORDER_WIDTH,
                            sh-2*gaps-2*BORDER_WIDTH-y);
            }
        }
    }
}

void customwm::
layout_column_grid_top()
{
    log("Column Grid Top layout");
    if(!head) return;
    Client *c;
    int y, n=0, mfact = MASTER_FACTOR, 
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
layout_fibonacci()
{}

void customwm::
layout_column_grid_bottom()
{
    log("Column Grid Bottom layout");
    if(!head) return;
    Client *c;
    int y, n=0, mfact = MASTER_FACTOR, 
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

void customwm::
change_master_height()
{

}

void customwm::
toggle_panel()
{
    if(current->is_full) return;
    SHOW_PANEL = !SHOW_PANEL;
    const char *panel_show = "polybar-msg cmd show &";
    const char *panel_hide = "polybar-msg cmd hide &";
    if(SHOW_PANEL)
        system(panel_show);
    else
        system(panel_hide);
    applylayout();
}

void customwm::
focus_prev()
{
    if(!head->next) return;
    if(current)
    {
        if(current->prev)
            current = current->prev;
        /*else if(!current->prev)
            current = tail;*/
    }
    update_current_client();
}

void customwm::
focus_next()
{
    if(!head->next) return;
    if(current)
    {
        if(current->next)
            current = current->next;
        /*else if(!current->next)
            current = head;*/
    }
    update_current_client();
}

Client* customwm::
get_prev_client_from_tiled(Window w)
{
    if(!w || tiled_clients.size() == 0) return NULL;

    for(int i=1; i<tiled_clients.size(); i++)
        if(tiled_clients.at(i)->win == w)
            return tiled_clients.at(i-1);
    return NULL;
}

Client* customwm::
get_next_client_from_tiled(Window w)
{
    if(!w || tiled_clients.size() == 0) return NULL;

    for(int i=0; i<tiled_clients.size()-1; i++)
        if(tiled_clients.at(i)->win == w)
            return tiled_clients.at(i+1);
    return NULL;
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
change_master_size(int step)
{
    //log("Changing master size");
    if(!current || current->is_float || tiled_clients.size() == 1) return;
    if(MASTER_FACTOR + 2*step > 0 && MASTER_FACTOR + step < sw - step)
        MASTER_FACTOR = MASTER_FACTOR + step;
    applylayout();
}

void customwm::
change_gaps(int step)
{
    if(GAPS + step >= 0)
        GAPS = GAPS + step;
    applylayout();
}

void customwm::
swap_master()
{
    Window tmp;
    if(!head || !current || current->is_float) return;
    tmp = head->win;;
    head->win = current->win;
    current->win = tmp;
    current = head;
    update_current_client();
    applylayout();
}

void customwm::
show_desktop()
{
    Client *c;
    if(!head) return;
    if(!show_mode)
    {
        for(c=head; c; c=c->next)
            XUnmapWindow(dpy, c->win);
        show_mode = true;
    }
    else
    {
        for(c=head; c; c=c->next)
            XMapWindow(dpy, c->win);
        show_mode = false;
    }
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

void customwm::
hide_client(Client *c)
{
    if(!c) return;
    if(!hidden_client)
    {
        temp = c;
        XUnmapWindow(dpy, c->win);
        remove_window(c->win);
        hidden_client = true;
    }
    else
    {
        add_window(temp->win);
        XMapWindow(dpy, temp->win);
        hidden_client = false;
    }
    applylayout();
}

void customwm::
set_border(Display *dpy, Window w, string color)
{
    XSetWindowBorder(dpy, w, getcolor(color.c_str()));
}

void customwm::
set_window_bg(Display *dpy, Window w, string color)
{
    XSetWindowBackground(dpy, w, getcolor(color.c_str()));
    XClearWindow(dpy, w);
}

int main(int argc, char *argv[])
{
    customwm *wm = new customwm();
    wm->setup();
    return 0;
}
