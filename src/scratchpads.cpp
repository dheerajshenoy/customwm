void customwm::
scratchpad_spawn(string scratchkey)
{
    const char *cmd = "alacritty --title scratchpad &";
    system(cmd);
}
