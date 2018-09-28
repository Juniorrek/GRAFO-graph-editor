/*
 *  grafo - graph editor
 *  Copyright (c) 2003
 *      Murilo Vicente Goncalves da Silva <murilo@pet.inf.ufpr.br>
 *      Oliver Matias van Kaick <oliver@pet.inf.ufpr.br>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *  See the GNU General Public License for more details (file COPYING).
 */ 


/* *********** */
/* * iface.h * */
/* *********** */

#ifndef __IFACE_H_
#define __IFACE_H_


#ifndef __GTK_H__
#include <gtk/gtk.h>
#endif


/* Main window attributes */
#define MAIN_WINDOW_TITLE   "grafo"
#define MAIN_WINDOW_WIDTH   800
#define MAIN_WINDOW_HEIGHT  600
#define MAX_TITLE_SIZE      256

/* Drawing area and gtkgl area attributes */
#define MIN_DRAWING_XSIZE   600
#define MIN_DRAWING_YSIZE   400

/* View modes: gtk drawing area or gtkgl area */
#define VIEW_MODE_OPENGL  0
#define VIEW_MODE_GTK     1

/* Edit modes */
#define NOTHING   0
#define V_INSERT  1 
#define V_CHANGE  2
#define V_REMOVE  3
#define V_EDIT    4
#define E_INSERT  5
#define E_CHANGE  6
#define E_REMOVE  7
#define E_EDIT    8


/* Interface struct */
typedef struct iface {
    /* Main window */
    GtkWidget *window;
    GtkWidget *pixmap;
    GtkWidget *vbox;
    GtkWidget *menubar;
    GtkWidget *toolbar;
    GtkWidget *drawingarea;
    GtkWidget *glarea;
    GtkWidget *statusbar;
    GtkWidget *popup;
    GtkAccelGroup *window_accels;
    GtkTooltips *tooltips;

    /* Open filesel */
    GtkWidget *open_filesel;
    GtkWidget *open_filesel_menu;
    GtkWidget *open_filesel_optionmenu;
    
    /* Save filesel */
    GtkWidget *save_filesel;
    GtkWidget *save_filesel_menu;
    GtkWidget *save_filesel_optionmenu;

    /* About dialog */
    GtkWidget *about_dialog;

    /* Message dialog */
    GtkWidget *msg_dialog;

    /* Confirmation dialog */
    GtkWidget *conf_dialog;

    /* Preferences dialog */
    GtkWidget *pref_dialog;
    GtkWidget *pref_dialog_neato_text;
    GtkWidget *pref_dialog_dot_text;
    GtkWidget *pref_dialog_neato_filesel;
    GtkWidget *pref_dialog_dot_filesel;
    GtkWidget *pref_dialog_plugins_text;
    GtkWidget *pref_dialog_plugins_filesel;
    GtkWidget *pref_dialog_datadir_text;
    GtkWidget *pref_dialog_datadir_filesel;
    GtkWidget *pref_dialog_view_mode_menu;
    GtkWidget *pref_dialog_view_mode_optionmenu;

    /* Plugins dialog */
    GtkWidget *pgin_dialog;

    /* Graph dialog */
    GtkWidget *graph_dialog;
    GtkWidget *graph_dialog_notebook;
    GtkWidget *graph_dialog_random1_spin_button;
    GtkWidget *graph_dialog_random2_spin_button;
    GtkWidget *graph_dialog_regular1_spin_button;
    GtkWidget *graph_dialog_regular2_spin_button;
    GtkWidget *graph_dialog_complete_spin_button;
    GtkWidget *graph_dialog_bipartite1_spin_button;
    GtkWidget *graph_dialog_bipartite2_spin_button;
    GtkWidget *graph_dialog_star_spin_button;

    /* Edit vertex properties dialog */
    GtkWidget *vprop_dialog; 
    GtkWidget *vprop_dialog_vertex_label;
    GtkWidget *vprop_dialog_label_text;
    GtkWidget *vprop_dialog_weight_spin_button;
    GtkWidget *vprop_dialog_color_spin_button;
    int vertex_indx;
    
    /* Edit edge properties dialog */
    GtkWidget *eprop_dialog;
    GtkWidget *eprop_dialog_edge_label;
    GtkWidget *eprop_dialog_label_text;
    GtkWidget *eprop_dialog_weight_spin_button;
    GtkWidget *eprop_dialog_color_spin_button;
    int edge_indx[2];

    /* Drawing area or glarea dimensions */
    int width;
    int height;

    /* Draw changed */
    int changed;

    /* Backing pixmap */
    GdkPixmap *back;

    /* View mode: gtk drawing area or glarea */
    int view_mode;

    /* Edit mode */
    int edit_mode;

    /* Vertex label type */
    int vlabel_mode;
    
    /* Edge label type */
    int elabel_mode;

    /* Status strings */
    char *help_string;
    char status_string[1000];

    /* Cursors */
    int cursor_indx;
    GdkCursor *cursor[5];

    /* File */
    const char *filename;
    int format;

    /* Interface ready */
    int initialized;

    /* Window title */
    char title[MAX_TITLE_SIZE];
} Iface;


void create_main_window(void);
void create_format_items(GtkWidget *menu, int plain, int psitem);
void create_open_filesel_dialog(void);
void create_save_filesel_dialog(void);
void create_about_dialog(void);
void create_msg_dialog(void);
void create_conf_dialog(void);
void create_preferences_dialog(void);
void create_plugins_dialog(void);
void create_graph_dialog(void);
void create_vprop_dialog(void);
void create_eprop_dialog(void);
    
int *getintp(int i);
GtkWidget *get_main_menu_item_by_name(char *name);
void add_menu_item(GtkWidget *menu, char *s, int i, int type, int plugin);
void init_algorithms(void);

char ***parse_toolbar_items(char *s, int *count);
char *find_alg_label(char *name);
void init_toolbar(void);

void IfaceLoop(int *argc, char ***argv);


#endif

/* ******* */
/* * End * */
/* ******* */

