#define BUTTONMASK          (ButtonPressMask | ButtonReleaseMask)
#define MOUSEMASK           (PointerMotionMask | BUTTONMASK)
#define CLEANMASK(mask)     (mask & ~(numlockmask|LockMask) & (ShiftMask|ControlMask|Mod1Mask|Mod2Mask|Mod3Mask|Mod4Mask|Mod5Mask))
#define MAX(a,b)    ((a) > (b) ? (a) : (b))

using namespace std;

class customwm
{
    public:
        vector<Key>KEYS;
        vector<string> PROGRAMS,
            LAYOUTS,
            wsp = { "1", "2", "3", "4", "5", "6", "7", "8", "9" },
            rulestemp;
        vector<vector<string>> K, P;
        vector<Client *> tiled_clients,
            fixed_clients,
            grouped_clients,
            hidden_clients;
        vector<Key>::iterator itr;
        vector<Rule> RULES;
        vector<Rule>::iterator ptr;
        Atom netatom[NetLast], wmatom[WMLast], utf8string;
        Display *dpy;
        Window checkwin;
        int screen, root, sw, sh, exist,
            GAPS, motion=0,
            numlockmask=0,
            current_desktop=0,
            BORDER_WIDTH=5,
            TITLE_HEIGHT = 30,
            TAB_HEIGHT = 10,
            PANEL_HEIGHT=30,
            current_layout=0;
        fstream logfile;
        const static int total_desktops=9;
        Desktop desktop[total_desktops];
        XWindowAttributes attr;
        Client *current = NULL,
               *head = NULL;
        string ACTIVE_BORDER,
               INACTIVE_BORDER,
               TITLE_BACKGROUND,
               FLOATING_BORDER,
               STICKY_BORDER,
               URGENT_BORDER,
               TITLE_BORDER,
               GROUPED_BORDER,
               FIXED_BORDER,
               conf, bracks, del;
        bool SHOW_PANEL,
             SLOPPY_FOCUS,
             show_mode=0,
             ATTACH_END,
             SIDE_DECORATION,
             TITLE_COLOR_FOR_MODES,
             ATTACH_MASTER,
             SINGLE_CLIENT_BORDER,
             SINGLE_CLIENT_GAPS,
             DECORATIONS_ON_FLOAT,
             hidden_client = false;
        float MASTER_FACTOR;
        XButtonEvent s;
    Cursor ResizeCursor, MoveCursor;
    void add_window(Window w, uint desk, Client *copy=NULL);
    void magnifier_helper(Window w, uint desk, string dir);
    void remove_window(Window w, uint desk);
    void setup();
    void setup_restartable();
    void loop();
    void init_ewmh();
    void spawn(string func, string arg);
    void grab_buttons();
    void update_current_client();
    void read_keys(vector<vector<string>> k);
    void read_config();
    void monitor_setup(void);

    void fake_fullscreen(Client *c);
    void set_fullscreen(Client *c, bool f);
    void toggle_fullscreen(Client *c);
    void set_float(Client *c, bool f, bool move=false);
    void set_grouped(Client *c, bool s);
    void toggle_grouped(Client *c);
    void toggle_fixed(Client *c);
    void toggle_float(Client *c);
    void set_sticky(Client *c, bool s);
    void toggle_sticky(Client *c);
    void toggle_hidden(Client *c);
    void set_hidden(Client *c, bool hidden);

    void getConfig(string a, string b, string c);
    void client_decoration_toggle(Client *c);
    void client_decorations_destroy(Client *c);
    void client_decorations_create(Client *c);
    void tabbed_decorations(Client *c);
    void killclient(Client *c);
    void send_kill_signal(Window w);
    void client_to_desktop(Client *c, uint desk);
    void change_desktop(uint desk);
    void select_desktop(uint desk);
    void save_desktop(uint desk);
    void cleanup();
    void clone_client(Client *c);
    void move_window_with_key(Client *c, string dir, int step);
    void resize_window_with_key(Client *c, string dir, int step);
    void resize_mouse();
    void move_mouse();
    void move_window_to_corner(Client *c, string corner);
    void center_client(Client *c);
    void hide_client(Client *c);
    void show_client(string pos);
    void toggle_panel();
    void focus_next();
    void focus_prev();
    void move_win_up();
    void move_win_down();
    void scratchpad_spawn(string scratchkey);
    void applylayout();
    void change_master_size(int);
    void send_stickies(int desk);
    void grab_win_buttons(Window w);
    void delete_pointer_vectors();
    void scan();
    void update_geometry(Window w, int x, int y, int wi, int h);
    void get_size_hints(Client *c);
    ulong getcolor(const char *color);

    void layout_tiled();
    void layout_monocle();
    void layout_column();
    void layout_tabbed();
    void layout_column_grid_bottom();
    void layout_column_grid_top();
    void layout_grid();
    void layout_magnifier();

    void change_height(int step);
    void change_layout();
    void change_gaps(int step);
    void swap_master(Client *c);
    void apply_rules(Client *c);
    void show_desktop();
    void ipc(string func, string arg);
    Client* get_client_from_window(Window w);
    Client* get_prev_client_from_tiled(Window w);
    Client* get_next_client_from_tiled(Window w);

    int sendevent(Client *c, Atom proto);
    int get_text_prop(Window w, Atom atom, char *text, uint32_t size);

    void ewmh_set_frame_extent(Client *c);
    void ewmh_set_desktop(Client *c, int desk);
    void ewmh_set_current_desktop();
    void ewmh_set_client_list();
    void ewmh_init_desktop_names();
    void ewmh_init_viewport();
    void ewmh_init_number_of_desktops();
    
    void manage_args(vector<string> args, int argc);
    string ipc_get_layout();
    string ipc_get_current_workspace();

    int manage_xsend_icccm(Client *c, Atom atom);

    void copy_client_prop(Client *c1, Client *c2);
    void set_border(Client *c, string color);
    void set_window_bg(Window w, string color);

    void client_message(XEvent *e);
    void property_notify(XEvent *e);
    void enter_notify(XEvent *e);
    void leave_notify(XEvent *e);
    void destroy_notify(XEvent *e);
    void unmap_notify(XEvent *e);
    void map_notify(XEvent *e);
    void button_press(XEvent *e);
    void key_press(XEvent *e);
    void map_request(XEvent *e);
    void configure_request(XEvent *e);
    void configure_notify(XEvent *e);
    void log(string msg, uint stick_out=0);
    
    vector<string> split(string STR, string DELIM);
};
