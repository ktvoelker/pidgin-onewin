
// #define PURPLE_PLUGINS

#include <glib.h>

#include "internal.h"
#include "pidgin.h"
#include "gtkblist.h"
#include "gtkconv.h"
#include "gtkconvwin.h"
#include "gtkplugin.h"
#include "notify.h"
#include "plugin.h"
#include "version.h"

static PidginWindow *get_fake_window(void);

static PidginWindow *
get_fake_window()
{
	static PidginWindow *win = NULL;
	GtkPositionType pos;
	GtkWidget *testidea;
	GtkWidget *menubar;
	// GdkModifierType state;

  if (win != NULL) {
    return win;
  }

	win = g_malloc0(sizeof(PidginWindow));

	/* Create the window. */
  // TODO is this a good idea?
  //
  // If this causes problems, perhaps we should create a window but
  // never show it?
  win->window = pidgin_blist_get_default_gtk_blist()->window;

  // TODO is this okay?
	// if (available_list == NULL) {
	// 	 create_icon_lists(win->window);
	// }

  // TODO are we watching for these events on the right widget?
  g_signal_connect(G_OBJECT(win->window), "delete_event",
                   G_CALLBACK(conv_close_win_cb), win);
	g_signal_connect(G_OBJECT(win->window), "focus_in_event",
	                 G_CALLBACK(conv_focus_win_cb), win);


	/* Create the notebook. */
	win->notebook = gtk_notebook_new();

	/* Intercept keystrokes from the menu items */
  // TODO are we watching for this event on the right widget?
  // It was originally on win->window.
	g_signal_connect(G_OBJECT(win->window), "key_press_event",
	                 G_CALLBACK(conv_window_keypress_cb), win);

	pos = purple_prefs_get_int(PIDGIN_PREFS_ROOT "/conversations/tab_side");

#if 0
	gtk_notebook_set_tab_hborder(GTK_NOTEBOOK(win->notebook), 0);
	gtk_notebook_set_tab_vborder(GTK_NOTEBOOK(win->notebook), 0);
#endif
	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(win->notebook), pos);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(win->notebook), TRUE);
	gtk_notebook_popup_enable(GTK_NOTEBOOK(win->notebook));
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(win->notebook), FALSE);
	gtk_notebook_set_show_border(GTK_NOTEBOOK(win->notebook), TRUE);

	g_signal_connect(G_OBJECT(win->notebook), "button-press-event",
				G_CALLBACK(right_click_menu_cb), win);

	gtk_widget_show(win->notebook);

	g_signal_connect(G_OBJECT(win->notebook), "switch_page",
	                 G_CALLBACK(before_switch_conv_cb), win);
	g_signal_connect_after(G_OBJECT(win->notebook), "switch_page",
	                       G_CALLBACK(switch_conv_cb), win);

	/* Setup the tab drag and drop signals. */
	gtk_widget_add_events(win->notebook,
	                      GDK_BUTTON1_MOTION_MASK | GDK_LEAVE_NOTIFY_MASK);
	g_signal_connect(G_OBJECT(win->notebook), "button_press_event",
	                 G_CALLBACK(notebook_press_cb), win);
	g_signal_connect(G_OBJECT(win->notebook), "button_release_event",
	                 G_CALLBACK(notebook_release_cb), win);

	testidea = gtk_vbox_new(FALSE, 0);

	/* Setup the menubar. */
  // TODO make the menus appear in the Buddy List's menubar
	menubar = conv_setup_menubar(win);
	gtk_box_pack_start(GTK_BOX(testidea), menubar, FALSE, TRUE, 0);

	gtk_box_pack_start(GTK_BOX(testidea), win->notebook, TRUE, TRUE, 0);

  // TODO add testidea to the Buddy List window
	// gtk_container_add(GTK_CONTAINER(win->window), testidea);
	gtk_widget_show(testidea);
  gtk_paned_pack2(
      GTK_PANED(pidgin_blist_get_default_gtk_blist()->hpaned), testidea,
      TRUE, FALSE);
  gtk_widget_set_size_request(
      gtk_paned_get_child1(
        GTK_PANED(pidgin_blist_get_default_gtk_blist()->hpaned)),
      300, -1);

	/* Update the plugin actions when plugins are (un)loaded */
	purple_signal_connect(purple_plugins_get_handle(), "plugin-load",
			win, PURPLE_CALLBACK(conv_plugin_changed_cb), win);
	purple_signal_connect(purple_plugins_get_handle(), "plugin-unload",
			win, PURPLE_CALLBACK(conv_plugin_changed_cb), win);

	return win;
}

static void
conv_place(PidginConversation *conv)
{
  pidgin_conv_window_add_gtkconv(get_fake_window(), conv);
}

static gboolean
plugin_load(PurplePlugin *plugin)
{
    // purple_notify_message(plugin, PURPLE_NOTIFY_MSG_INFO, "Hello World!",
    //                    "This is the Hello World! plugin :)", NULL, NULL, NULL);
    pidgin_conv_placement_add_fnc("onewin", _("Keep everything in the Buddy List window"),
                         &conv_place);
    purple_prefs_trigger_callback(PIDGIN_PREFS_ROOT "/conversations/placement");
    return TRUE;
}

static gboolean
plugin_unload(PurplePlugin *plugin)
{
  pidgin_conv_placement_remove_fnc("number");
  purple_prefs_trigger_callback(PIDGIN_PREFS_ROOT "/conversations/placement");
  return TRUE;
}

static PurplePluginInfo info = {
    PURPLE_PLUGIN_MAGIC,
    PURPLE_MAJOR_VERSION,
    PURPLE_MINOR_VERSION,
    PURPLE_PLUGIN_STANDARD,
    PIDGIN_PLUGIN_TYPE,
    0,
    NULL,
    PURPLE_PRIORITY_DEFAULT,

    "core-onewin",
    "One Window",
    "1.1",

    "One Window Plugin",          
    "One Window Plugin",          
    "Karl Voelker <ktvoelker@gmail.com>",
    "http://karlv.net",     
    
    plugin_load,                   
    plugin_unload,
    NULL,                          
                                   
    NULL,                          
    NULL,                          
    NULL,                        
    NULL,                   
    NULL,                          
    NULL,                          
    NULL,                          
    NULL                           
};                               
   
static void                        
init_plugin(PurplePlugin *plugin)
{                                  
}

PURPLE_INIT_PLUGIN(hello_world, init_plugin, info)

