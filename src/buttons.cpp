void customwm::
grab_buttons()
{
    //log("Grabbing buttons");
    XGrabButton(dpy, Button3, Mod4Mask, root, True, MOUSEMASK, GrabModeAsync,GrabModeAsync, None, None);
    XGrabButton(dpy, Button2, Mod4Mask, root, True, MOUSEMASK, GrabModeAsync,GrabModeAsync, None, None);
    XGrabButton(dpy, Button1, Mod4Mask, root, True, MOUSEMASK, GrabModeAsync,GrabModeAsync, None, None);
}
//EVENTS

void customwm::
grab_win_buttons(Window w)
{
    XGrabButton(dpy, Button1, AnyModifier, w, True, MOUSEMASK, GrabModeAsync, GrabModeAsync, None, None);
    XGrabButton(dpy, Button2, AnyModifier, w, True, MOUSEMASK, GrabModeAsync, GrabModeAsync, None, None);
    XGrabButton(dpy, Button3, AnyModifier, w, True, MOUSEMASK, GrabModeAsync, GrabModeAsync, None, None);
}
