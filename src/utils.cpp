void customwm::
set_border(Client *c, string color)
{
    XSetWindowBorder(dpy, c->win, getcolor(color.c_str()));
    if(TITLE_COLOR_FOR_MODES && c->dec)
        set_window_bg(c->dec, color);
}

void customwm::
set_window_bg(Window w, string color)
{
    XSetWindowBackground(dpy, w, getcolor(color.c_str()));
    XClearWindow(dpy, w);
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

void customwm::
update_geometry(Window w, int x, int y, int wi, int h)
{
    if(wi == 0 || h == 0)
        XMoveResizeWindow(dpy, w, x, y, wi + 10, h + 10);
    else
        XMoveResizeWindow(dpy, w, x, y, wi, h);
}

void customwm::
log(string msg, uint stick_out)
{
    if(stick_out)
        logfile << ">>>>>> " << msg << endl;
    else
        logfile << msg << endl;
}

void customwm::
delete_pointer_vectors()
{
    for(auto p: tiled_clients)
        delete(p);
    for(auto p: fixed_clients)
        delete(p);
    for(auto p: hidden_clients)
        delete(p);
    tiled_clients.clear();
    fixed_clients.clear();
    hidden_clients.clear();
}

Client* customwm::
get_prev_client_from_tiled(Window w)
{
    if(!w || tiled_clients.size() == 0) return NULL;

    for(uint i=1; i<tiled_clients.size(); i++)
        if(tiled_clients.at(i)->win == w)
            return tiled_clients.at(i-1);
    return NULL;
}

Client* customwm::
get_next_client_from_tiled(Window w)
{
    if(!w || tiled_clients.size() == 0) return NULL;

    for(uint i=0; i<tiled_clients.size()-1; i++)
        if(tiled_clients.at(i)->win == w)
            return tiled_clients.at(i+1);
    return NULL;
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
change_height(int step)
{

}

void customwm::
change_gaps(int step)
{
    if(GAPS + step >= -1 && GAPS + step < 40)
        GAPS = GAPS + step;
    applylayout();
}


void customwm::
change_layout()
{
    if(current_layout < LAYOUTS.size()-1)
        current_layout++;
    else
        current_layout = 0;
    ipc_set_layout(current_layout);
    applylayout();
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
copy_client_prop(Client *from, Client *to)
{
    to->is_sticky = from->is_sticky;
    to->is_full = from->is_full;
    to->is_sticky = from->is_sticky;
    to->is_resizable = from->is_resizable;
    //to->is_urgent = from->is_urgent;
    to->is_hidden = from->is_hidden;
    to->is_float = from->is_float;
    to->is_dec = from->is_dec;
    to->x = from->x;
    to->y = from->y;
    to->w = from->w;
    to->h = from->h;
    to->oldx = from->oldx;
    to->oldy = from->oldy;
    to->oldw = from->oldw;
    to->oldh = from->oldh;
    to->dec = from->dec;
    to->desk = from->desk;
}
