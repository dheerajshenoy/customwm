#include "headers.cpp"

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
            case PropertyNotify:
                property_notify(&ev);
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
update_current_client()
{
    log("Updating current client");
    Client *c;
    if(!head) return;
    for(c=head; c; c=c->next)
    {
        XUngrabButton(dpy, AnyButton, AnyModifier, c->win);
        XSetWindowBorderWidth(dpy, c->win, BORDER_WIDTH);
        if(current == c)
        {
            XGrabButton(dpy, Button1, Mod1Mask, current->win, false, BUTTONMASK, GrabModeAsync, GrabModeAsync, None, None);
            if(c->is_full || !head->next && !c->is_float)
                XSetWindowBorderWidth(dpy, c->win, 0);
            else
            {
                if(c->is_sticky)
                    set_border(c, STICKY_BORDER);
                else if(c->is_float && !c->is_grouped)
                {
                    set_border(c, FLOATING_BORDER);
                    if(DECORATIONS_ON_FLOAT && c->dec)
                    {
                        set_window_bg(c->dec, ACTIVE_BORDER);
                        XRaiseWindow(dpy, c->dec);
                    }
                    XRaiseWindow(dpy, c->win);
                }
                else if(c->is_fixed)
                {
                    set_border(c, FIXED_BORDER);
                    set_float(c, true, true);
                }
                else if(c->is_grouped)
                    set_border(c, GROUPED_BORDER);
                else
                    set_border(c, ACTIVE_BORDER);
            }
            XSetInputFocus(dpy, c->win, RevertToPointerRoot, CurrentTime);
            XChangeProperty(dpy, c->win, netatom[NetActiveWindow],
                    XA_WINDOW, 32, PropModeReplace, (unsigned char *) &(c->win),1);
            manage_xsend_icccm(c, wmatom[WMTakeFocus]);
        }
        else
        {
            if(c->dec)
            {
                set_window_bg(c->dec, INACTIVE_BORDER);
            }
            if(c->is_grouped)
                set_border(c, GROUPED_BORDER);
            else if(c->is_sticky)
                set_border(c, STICKY_BORDER);
            else
                set_border(c, INACTIVE_BORDER);
        }
    }
    XSync(dpy, false);
}

void customwm::
hide_client(Client *c)
{
    if(!c) return;
    set_hidden(c, true);
}

void customwm::
show_client(string pos)
{
    if(hidden_clients.size() == 0) return;
    if(pos == "first")
    {
        set_hidden(hidden_clients[0], false);
    }
    else if(pos == "last" || pos == "recent")
        set_hidden(hidden_clients.back(), false);
}

void customwm::
cleanup()
{
    //log("Shutting WM");
    Client *c;
    for(int i=0; i<total_desktops; i++)
        for(c=desktop[i].head; c; c=c->next)
            killclient(c);
    delete_pointer_vectors();
    free(current);
    free(head);
    XDestroyWindow(dpy, checkwin);
    XDeleteProperty(dpy, root, netatom[NetSupported]);
    XUngrabKey(dpy, AnyKey, AnyModifier, root);
    XCloseDisplay(dpy);
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
change_master_size(int step)
{
    //log("Changing master size");
    if(!current || current->is_float || tiled_clients.size() == 1) return;
    if(MASTER_FACTOR + 2*step > 0 && MASTER_FACTOR + step < sw - step)
        MASTER_FACTOR = MASTER_FACTOR + step;
    applylayout();
}

void customwm::
swap_master(Client *c)
{
    Window tmp;
    Client *temp;
    if(!head || !c || c->is_float) return;
    tmp = head->win;;
    head->win = c->win;
    c->win = tmp;
    copy_client_prop(head, c);
    c = head;
    current = c;
    update_current_client();
    applylayout();
}

void customwm::
show_desktop()
{
    log("Show desktop Mode");
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
        update_current_client();
    }
}

int main(int argc, char *argv[])
{
    string conf = getenv("HOME"), brackets = "[]", delims = "=";
    conf = conf + "/.config/customwm/config.ini";
    customwm *wm = new customwm();
    wm->getConfig(conf, brackets, delims);
    wm->setup();
    return 0;
}
