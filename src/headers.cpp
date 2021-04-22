#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xproto.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/extensions/shape.h>
#include <X11/XKBlib.h>

#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "types.hpp"
#include "../include/iniparser.hpp"
#include "customwm.hpp"
#include "configs.cpp"
#include "events.cpp"
#include "utils.cpp"
#include "buttons.cpp"
#include "modes.cpp"
#include "ewmh.cpp"
#include "decorations.cpp"
#include "desktops.cpp"
#include "windows.cpp"
#include "layouts.cpp"
#include "mouse.cpp"
#include "scratchpads.cpp"
