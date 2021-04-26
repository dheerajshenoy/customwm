typedef struct Monitor Monitor;
typedef struct Client Client;

struct Client {
    Window win, dec;
    bool is_full,
         is_sticky,
         is_float,
         is_hidden,
         is_dec,
         is_fixed,
         is_grouped,
         is_resizable,
         old_state;
    int x, y, oldx, oldy;
    uint desk, w, h, bw, oldw, oldh, oldbw, maxw, maxh;
    char name[256];
    Client *next;
    Client *prev;
};

struct Monitor {
    int x, y;
    uint width, height, screen;
};

struct Desktop {
    Client *head,
           *current;
    int layout;
    int gaps;
    bool show_panel;
    float mfact;
};

struct Key {
    unsigned int mod;
    KeySym keysym;
    std::string function;
    std::string arg;
};

struct Button {
    unsigned int mod;
    unsigned int button;
    std::string function;
};

struct Rule {
    std::string Class;
    std::string title;
    int floating;
    //int issticky;
    int desktop;
    int monitor;
};

enum { 
    NetSupported, 
    NetWMName, 
    NetWMState, 
    NetWMCheck, 
    NetWMDesktop,
    NetWMFrameExtents,
    NetWMFullscreen, 
    NetWMSticky,
    NetWMHidden,
    NetActiveWindow, 
    NetWMWindowType,
    NetWMWindowTypeDock,
    NetWMWindowTypeDialog, 
    NetWMWindowTypeSplash,
    NetWMWindowTypeMenu,
    NetWMWindowTypeToolbar,
    NetWMWindowTypeUtility,
    NetShowDesktopMode,
    NetClientList, 
    NetDesktopNames, 
    NetDesktopViewport, 
    NetCloseWindow,
    NetNumberOfDesktops, 
    NetCurrentDesktop, 
    NetWMMoveResize,
    NetLast 
}; 

enum {
    WMProtocols,
    WMDelete,
    WMState,
    WMTakeFocus,
    WMNormalHints,
    WMLast
};

