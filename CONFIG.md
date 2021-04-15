# CustomWM Configuration
To see how to configure customWM, please check the config.ini file provided in the repo.
# NOTE
- The section names have to be exactly one of WM, PANEL, CLIENT, PANEL, COLOR, RULES, AUTOSTART, KEYS
- If there are any mistakes in the config, correct it manually (for now), otherwise the WM crashes.
- The keys can be in any order, but has to be withinthe correct section.

# WM section
* ``DEFAULT_DESKTOP`` **int** {1-9} Default desktop on starting the WM

* ``DEFAULT_LAYOUT`` **int** {0-size of LAYOUTS} Default Layout to be used

* ``MASTER_FACTOR`` **float** sets the MASTER WINDOW width, default is 0.52

* ``SINGLE_CLIENT_BORDER`` **bool** Border shown when there is single client in the workspace

* ``SLOPPY_FOCUS`` **bool** Focus when mouse is hovered over a window (Annoying)

* ``ATTACH_MASTER`` **bool** Adds new window to the master section instead of stack

* ``SINGLE_CLIENT_GAPS`` **bool** If gaps are required when there's only one client in the workspace

* ``SINGLE_CLIENT_BORDER`` **bool** If borders are required when there's only one client in the workspace

* ``CLICK_TO_FOCUS`` **bool**

* ``DECORATIONS_ON_FLOAT`` **bool** If window decorations are required for a client when it's floating

* ``LAYOUTS`` **Comma Separated Field** Possible values are *Tiled, Deck, Grid, Column, ColumnGridTop, ColumnGridBottom, Binary, Accordian*

* ``GAPS`` **int** Window gaps in pixels


# PANEL section
* ``PANEL_HEIGHT`` **int** Height of the panel

* ``SHOW_PANEL`` **bool**

* ``TOP_PANEL`` **bool**

# COLOR section
* ``ACTIVE_BORDER`` **hex** focused window border

* ``INACTIVE_BORDER`` **hex** unfocused window/s borders

* ``FLOATING_BORDER`` **hex** floating window border

* ``URGENT_BORDER`` **hex** Urgent window border

* ``STICKY_BORDER`` **hex** Sticky window border

* ``FIXED_BORDER`` **hex** Fixed window Border

* ``GROUPED_BORDER`` **hex** Grouped window Border

* ``TITLE_BORDER`` **hex** Titlebar border

* ``TITLE_BG`` **hex** Titlebar background

# RULES
- Because of the way in which I wrote iniparser library for C++, the RULES section requires that the user specify the rules as shown in the below example:
* Class = Title = Floating = Workspace = Monitor
* Pavucontrol = ^N = 1 = -1 = -1
* Specify ^N when there's no requirement of specific title or class

# AUTOSTART
* List the programs required to start when starting the WM.
* **NOTE** Please append & symbol at the end of each programs

# KEYS
* Before getting to know how keys are configured, let's look at some of the functions that customWM supports.

## In-built functions
- ``QUIT`` Quits the WM
- ``KILLCLIENT`` Kill the currently focused window
- ``RESTART`` Restarts the WM with changes made in the config (some of the options)
- ``SWAPMASTER`` Swaps currently focused window with the master
- ``ATTACHASIDE``
- ``SHOWDESKTOP`` Hides all the windows in the current workspace
- ``CENTER``
- ``DMODE``
- ``HIDE``
- ``LAYOUT``
- ``DESKTOP``
- ``CLDESKTOP``

