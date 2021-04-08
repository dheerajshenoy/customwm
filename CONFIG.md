# CustomWM Configuration
To see how to configure customWM, please check the config.ini file provided in the repo.
# NOTE
- The section names have to be exactly one of WM, PANEL, CLIENT, PANEL, COLOR, RULES, AUTOSTART, KEYS
- If there are any mistakes in the config, correct it manually (for now), otherwise the WM crashes.
- The keys can be in any order, but has to be withinthe correct section.

# WM section
* ``MASTER_FACTOR`` **float** sets the MASTER WINDOW width, default is 0.52
* ``SINGLE_CLIENT_BORDER`` **bool** Border shown when there is single client in the workspace
* ``SLOPPY_FOCUS`` **bool** Focus when mouse is hovered over a window (Annoying)
* ``ATTACH_MASTER`` **bool** Adds new window to the master section instead of stack
* ``INNER_GAPS`` **int**
* ``OUTER_GAPS`` **int**

# PANEL section
* ``PANEL_HEIGHT`` **int** Height of the panel
* ``SHOW_PANEL`` **bool**

# COLOR section
* ``ACTIVE_BORDER`` **hex** focused window border
* ``INACTIVE_BORDER`` **hex** unfocused window/s borders
* ``FLOATING_BORDER`` **hex** floating window border
* ``URGENT_BORDER`` **hex** Urgent window border
* ``STICKY_BORDER`` **hex** Sticky window border
