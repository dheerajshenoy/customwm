# CustomWM - A dynamic tiling window manager written in C++

# Why ?
* Being a window manager enthusiast and a user, I wanted to learn how they actually worked and how they're written.

# Pros of writing your own WM
* Memory management
* Window management (Obviously)
* Data structures
* Configuration strategies
* It's fun

# Cons
* It's tedious
* Xlib documentation is not well documented
* Time consuming

# Installation
* Libraries required : Xlib
* Programming Language C++ STL

# Features of CustomWM
* Layouts - Tiling, Monocle
* Configuration using INI file
* Toggle window modes - floating, tiling
* Move and resize floating windows with key press
* Corner floating windows
* Fullscreen windows
* Partial EWMH & ICCCM support
* Workspaces with per-workspace properties
* sloppy focus (buggy)
* Show Desktop Mode (Hide all clients)
* Autostart apps
* Works with panel like [Polybar](https://github.com/polybar/polybar)

# TODO
* Layouts - accordian, magnifier, tabbed
* Hiding clients, sticky clients
* Grouping windows to perform grouped actions
* Gaps
* Client decorations

# Configuration
The WM is configured using ini file, which gets parsed at run-time (using my very own [INI parser](https://github.com/dheerajshenoy/iniparser) (not well documented)). Check [CONFIG.md]() file for more info about configuration.

# WM's I took help from
* [BerryWM](https://berrywm.org/)
* [tinyWM](https://github.com/mackstann/tinywm)
* [SimpleWM](https://github.com/kcirick/simplewm)
* [catWM](https://github.com/pyknite/catwm)
* [dwm](https://dwm.suckless.org/)
