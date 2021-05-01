void customwm::
save_desktop(uint i)
{
    desktop[i].mfact = MASTER_FACTOR;
    desktop[i].layout = current_layout;
    desktop[i].gaps = GAPS;
    desktop[i].head = head;
    desktop[i].current = current;
    desktop[i].show_panel = SHOW_PANEL;
}

void customwm::
select_desktop(uint i)
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
change_desktop(uint desk)
{
    log("Changing desktop");
    if(desk == current_desktop)
        return;
    Client *c;
    if(head)
    {
        for(c=head; c; c=c->next)
        {
            XUnmapWindow(dpy, c->win);
            if(c->dec) XUnmapWindow(dpy, c->dec);
            if(c->is_sticky)
            {
                client_to_desktop(c, desk);
                remove_window(c->win, current_desktop);
            }
        }
    }
    save_desktop(current_desktop);
    select_desktop(desk);
    if(head)
    {
        for(c=head; c; c=c->next)
        {
            if(c->is_float)
                if(DECORATIONS_ON_FLOAT)
                    XMapWindow(dpy, c->dec);
            get_size_hints(c);
            XMapWindow(dpy, c->win);
        }
    }
    current_desktop = desk;
    ewmh_set_client_list();
    update_current_client();
    ipc_set_layout(current_layout);
    applylayout();
}

void customwm::
client_to_desktop(Client *c, uint desk)
{
    log("Sending client to desktop " + to_string(desk));
    if(!c) return;
    if(desk == current_desktop) return;
    uint tmp2 = current_desktop;
    select_desktop(desk);
    if(DECORATIONS_ON_FLOAT && c->dec)
    {
        client_decorations_destroy(c);
        c->is_dec = false;
    }
    XUnmapWindow(dpy, c->win);
    get_size_hints(c);
    add_window(c->win, desk, c);
    XMoveResizeWindow(dpy, c->win, c->x, c->y, c->w, c->h);
    ewmh_set_desktop(c, desk);
    save_desktop(desk);
    select_desktop(tmp2);
    remove_window(c->win, desk);
    save_desktop(tmp2);
    ewmh_set_client_list();
    //update_current_client();
    applylayout();
}
