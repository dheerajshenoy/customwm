void customwm::
ipc_init()
{
    gapFile.open("/home/redundant/.config/customwm/gap", ios::out| ios::trunc);
    layoutFile.open("/home/redundant/.config/customwm/layout", ios::out | ios::trunc);
    layoutFile << LAYOUTS.at(current_layout) << endl;
}

string customwm::
ipc_get_layout()
{   
    log("IPC get layout");
}

void customwm::
ipc_set_layout(uint l)
{
    log("IPC set layout");
    string cmd = "sed -i '1s/.*/'" + LAYOUTS.at(l) + "'/' ~/.config/customwm/layout";
    system(cmd.c_str());
}

uint customwm::
ipc_get_gaps()
{
    log("IPC get gaps");
}

void customwm::
ipc_set_gaps(uint gap)
{
    log("IPC set gaps");
    string cmd = "sed -i '1s/.*/'" + to_string(gap) + "'/' ~/.config/customwm/gap";
    system(cmd.c_str());
}
