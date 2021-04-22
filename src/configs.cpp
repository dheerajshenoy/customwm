void customwm::
getConfig(string c, string b, string d)
{
    conf = c; bracks = b, del = d; 
    if(conf == "")
    {
        conf = getenv("HOME");
        conf = conf + "/.config/customwm/config.ini";
    }
    if(bracks == "")
        bracks = "[]";
    if(del == "")
        del = "=";
}


void customwm::
read_config()
{
    log("Reading config");
    iniparser config;
    config.Open(conf, bracks, del);
    //config.Open("/home/redundant/.config/customwm/config.ini", "[]", "=");
    // CLIENT 
    BORDER_WIDTH = stoi(config.getValue("BORDER_WIDTH", "CLIENT", "0").at(0));
    TITLE_HEIGHT = stoi(config.getValue("TITLE_HEIGHT", "CLIENT", "0").at(0));
    GAPS = stoi(config.getValue("GAPS", "WM", "2").at(0)) - 1;
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
    
    if(config.getValue("NO_BORDERS", "WM", "true").at(0) == "true")
    {
        BORDER_WIDTH = 0;
    }
    
    if(config.getValue("TITLE_COLOR_FOR_MODES", "CLIENT", "true").at(0) == "true")
        TITLE_COLOR_FOR_MODES = true;
    else
        TITLE_COLOR_FOR_MODES = false;

    if(config.getValue("SIDE_DECORATION", "WM", "true").at(0) == "true")
        SIDE_DECORATION = true;
    else
        SIDE_DECORATION = false;

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
            change_desktop(stoi(arg)-1);
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
        /*else if(func == "SHOW")
            show_client(arg);*/
    }
}

