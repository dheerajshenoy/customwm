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

