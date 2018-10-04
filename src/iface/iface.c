/*
 *  grafo - graph editor
 *  Copyright (c) 2003-2008
 *      Murilo Vicente Goncalves da Silva <murilo@pet.inf.ufpr.br>
 *      Oliver Matias van Kaick <oliver@pet.inf.ufpr.br>
 *      Ulisses Cordeiro Pereira <ulisses@cordeiropereira.com.br>
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
/* * iface.c * */
/* *********** */


#include "../config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#ifdef USE_GTKGL
#include <GL/gl.h>
#include <GL/glu.h>
#include <gtkgl-2.0/gtkgl/gtkglarea.h>
#endif

#include "../graph.h"
#include "../alghandler.h"
#include "../geometry.h"
#include "../rc.h"
#include "../plugins.h"
#include "iface.h"
#include "callbacks.h"
#include "factory.h"

#define STOCK_NEW
#define STOCK_OPEN
#define STOCK_SAVE
#define STOCK_SAVE_AS
//#define STOCK_CLOSE
#define STOCK_MENU_ABOUT
#define STOCK_EXIT
#define STOCK_CLEAR
#define STOCK_PREFERENCES
#define STOCK_LEFT_ARROW
#define STOCK_EXEC
#define STOCK_RIGHT_ARROW
#define STOCK_FIRST
#define STOCK_LAST
#include "pixmaps.h"
#include "pixmaps2.h"


/* External globals */
/* Algorithm lists: alghandler.c */
extern AlgList L;
extern AlgList TL;
extern AlgListR RL;
extern AlgListS SL;
extern AlgListS SSL;

/* Configuration: gr.c (definition: rc.h) */
extern Config *config;

/* Plugins info */
extern Plugins plugins;


/* Globals */
/* Interface struct */
Iface iface;

/* Global popup menu */
GtkWidget *popup;


/* Menu bar */
factory_item main_menu[] = {
/* Type, name, label, tooltip,
   accel key, mod, sens, group, state, callback, func_data, func_name,
   pixmap, pixmap_name, widget */
{ FACTORY_TYPE_MENU, "file", "_File", "Input/output functions",
  0, 0, TRUE, 0, 0, NULL, NULL, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
{ FACTORY_TYPE_ITEM, "new", "_New", "New file",
  GDK_n, GDK_CONTROL_MASK, TRUE, 0, 0,
  (GtkSignalFunc) on_new_item_activate, NULL, NULL,
  FACTORY_PIXMAP_NONE, (char *) stock_new, NULL, NULL },
{ FACTORY_TYPE_ITEM, "open", "_Open...", "Open file",
  GDK_o, GDK_CONTROL_MASK, TRUE, 0, 0,
  (GtkSignalFunc) on_open_item_activate, NULL, NULL,
  FACTORY_PIXMAP_NONE, (char *) stock_open, NULL, NULL },
{ FACTORY_TYPE_ITEM, "save", "_Save", "Save file",
  GDK_s, GDK_CONTROL_MASK, TRUE, 0, 0,
  (GtkSignalFunc) on_save_item_activate, NULL, NULL,
  FACTORY_PIXMAP_NONE, (char *) stock_save, NULL, NULL },
{ FACTORY_TYPE_ITEM, "saveas", "Save _as...", "Save file with name",
  GDK_a, GDK_CONTROL_MASK, TRUE, 0, 0,
  (GtkSignalFunc) on_saveas_item_activate, NULL, NULL,
  FACTORY_PIXMAP_NONE, (char *) stock_open, NULL, NULL },
/*{ FACTORY_TYPE_ITEM, "close", "_Close", "Close file",
  GDK_c, GDK_CONTROL_MASK, FALSE, 0, 0,
  (GtkSignalFunc) on_close_item_activate, NULL, NULL,
  FACTORY_PIXMAP_NONE, (char *) stock_close, NULL, NULL },*/
FACTORY_SEP
{ FACTORY_TYPE_ITEM, "about", "A_bout", "About grafo",
  GDK_b, GDK_CONTROL_MASK, TRUE, 0, 0,
  (GtkSignalFunc) on_about_item_activate, NULL, NULL,
  FACTORY_PIXMAP_NONE, (char *) stock_menu_about, NULL, NULL },
FACTORY_SEP
{ FACTORY_TYPE_ITEM, "exit", "E_xit", "Exit grafo",
  GDK_x, GDK_CONTROL_MASK, TRUE, 0, 0, gtk_main_quit, NULL, NULL,
  FACTORY_PIXMAP_NONE, (char *) stock_exit, NULL, NULL },
FACTORY_RET
{ FACTORY_TYPE_MENU, "edit", "_Edit", "Graph edit mode",
  0, 0, TRUE, 0, 0, NULL, NULL, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
{ FACTORY_TYPE_RADIO, "donothing", "Do _nothing",
    "Do nothing with the mouse",
  GDK_n, 0, TRUE, 0, 0,
  (GtkSignalFunc) on_edit_mode_activate, (gpointer) NOTHING, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
{ FACTORY_TYPE_RADIO, "insvertex", "Insert _vertex", "Insert vertex",
  GDK_v, 0, TRUE, 0, 0,
  (GtkSignalFunc) on_edit_mode_activate, (gpointer) V_INSERT, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
{ FACTORY_TYPE_RADIO, "movevertex", "_Move vertex", "Move vertex",
  GDK_m, 0, TRUE, 0, 0,
  (GtkSignalFunc) on_edit_mode_activate, (gpointer) V_CHANGE, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
{ FACTORY_TYPE_RADIO, "remvertex", "_Remove vertex", "Remove vertex",
  GDK_r, 0, TRUE, 0, 0,
  (GtkSignalFunc) on_edit_mode_activate, (gpointer) V_REMOVE, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
{ FACTORY_TYPE_RADIO, "editvertex", "E_dit vertex",
  "Edit vertex properties",
  GDK_d, 0, TRUE, 0, 0,
  (GtkSignalFunc) on_edit_mode_activate, (gpointer) V_EDIT, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
{ FACTORY_TYPE_RADIO, "insedge", "_Insert edge", "Insert edge",
  GDK_e, 0, TRUE, 0, 0,
  (GtkSignalFunc) on_edit_mode_activate, (gpointer) E_INSERT, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
{ FACTORY_TYPE_RADIO, "remedge", "Remove _edge", "Remove edge",
  GDK_x, 0, TRUE, 0, 0,
  (GtkSignalFunc) on_edit_mode_activate, (gpointer) E_REMOVE, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
{ FACTORY_TYPE_RADIO, "editedge", "Edit ed_ge", "Edit edge",
  GDK_g, 0, TRUE, 0, 0,
  (GtkSignalFunc) on_edit_mode_activate, (gpointer) E_EDIT, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
FACTORY_SEP
{ FACTORY_TYPE_ITEM, "shctrlpts", "_Show control points",
  "Show control points",
  GDK_p, GDK_CONTROL_MASK, TRUE, 0, 0,
  (GtkSignalFunc) on_show_ctrlpts_item_activate, (gpointer) 7, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
FACTORY_RET
{ FACTORY_TYPE_MENU, "graph", "_Graph", "Graph edition functions",
  0, 0, TRUE, 0, 0, NULL, NULL, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
{ FACTORY_TYPE_ITEM, "resetg", "_Reset graph",
    "Remove vertices and edges",
  GDK_r, GDK_CONTROL_MASK, TRUE, 0, 0,
  (GtkSignalFunc) on_reset_graph_item_activate,
  NULL, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
{ FACTORY_TYPE_ITEM, "resetme", "Reset marked _edges",
    "Reset marked edges",
  GDK_e, GDK_CONTROL_MASK, TRUE, 0, 0,
  (GtkSignalFunc) on_reset_marked_edges_item_activate, NULL, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
{ FACTORY_TYPE_ITEM, "resetmv", "Reset marked _vertices",
    "Reset marked vertices",
  GDK_v, GDK_CONTROL_MASK, TRUE, 0, 0,
  (GtkSignalFunc) on_reset_marked_vertices_item_activate, NULL, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
FACTORY_SEP
{ FACTORY_TYPE_MENU, "vlabel", "Vertex _label", "Vertex label",
  0, 0, TRUE, 0, 0, NULL, NULL, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
{ FACTORY_TYPE_RADIO, "nonevlab", "_None", "No label",
  0, 0, TRUE, 2, 0,
  (GtkSignalFunc) on_vlabel_mode_activate, (gpointer) 0, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
{ FACTORY_TYPE_RADIO, "indxvlab", "_Index", "Index as label",
  0, 0, TRUE, 2, 0,
  (GtkSignalFunc) on_vlabel_mode_activate, (gpointer) 1, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
{ FACTORY_TYPE_RADIO, "idvlab", "I_d", "Id number as label",
  0, 0, TRUE, 2, 0,
  (GtkSignalFunc) on_vlabel_mode_activate, (gpointer) 2, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
{ FACTORY_TYPE_RADIO, "labelvlab", "_Label", "Text label",
  0, 0, TRUE, 2, 0,
  (GtkSignalFunc) on_vlabel_mode_activate, (gpointer) 3, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
{ FACTORY_TYPE_RADIO, "weightvlab", "_Weight", "Weight as label",
  0, 0, TRUE, 2, 0,
  (GtkSignalFunc) on_vlabel_mode_activate, (gpointer) 4, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
{ FACTORY_TYPE_RADIO, "colorvlab", "_Color", "Color index as label",
  0, 0, TRUE, 2, 0,
  (GtkSignalFunc) on_vlabel_mode_activate, (gpointer) 5, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
FACTORY_RET
FACTORY_SEP
{ FACTORY_TYPE_MENU, "elabel", "Edge l_abel", "Edge label",
  0, 0, TRUE, 0, 0, NULL, NULL, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
{ FACTORY_TYPE_RADIO, "noneelab", "_None", "No label",
  0, 0, TRUE, 2, 0,
  (GtkSignalFunc) on_elabel_mode_activate, (gpointer) 0, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
{ FACTORY_TYPE_RADIO, "indxelab", "_Index", "Vertex indices as label",
  0, 0, TRUE, 2, 0,
  (GtkSignalFunc) on_elabel_mode_activate, (gpointer) 1, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
{ FACTORY_TYPE_RADIO, "idelab", "I_d", "Vertex id's as label",
  0, 0, TRUE, 2, 0,
  (GtkSignalFunc) on_elabel_mode_activate, (gpointer) 2, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
{ FACTORY_TYPE_RADIO, "labelelab", "_Label", "Text label",
  0, 0, TRUE, 2, 0,
  (GtkSignalFunc) on_elabel_mode_activate, (gpointer) 3, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
{ FACTORY_TYPE_RADIO, "weightelab", "_Weight", "Weight as label",
  0, 0, TRUE, 2, 0,
  (GtkSignalFunc) on_elabel_mode_activate, (gpointer) 4, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
{ FACTORY_TYPE_RADIO, "colorelab", "_Color", "Color index as label",
  0, 0, TRUE, 2, 0,
  (GtkSignalFunc) on_elabel_mode_activate, (gpointer) 5, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
FACTORY_RET
FACTORY_SEP
{ FACTORY_TYPE_MENU, "generate", "_Generate graph", "Generate graphs",
  0, 0, TRUE, 0, 0, NULL, NULL, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
{ FACTORY_TYPE_ITEM, "complete", "_Complete graph",
  "Complete graph with n vertices",
  GDK_k, GDK_SHIFT_MASK, TRUE, 0, 0,
  (GtkSignalFunc) on_complete_graph_item_activate, NULL, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
{ FACTORY_TYPE_ITEM, "bipartite", "_Bipartite complete graph",
  "Bipartite graph with n1,n2 vertices",
  GDK_b, GDK_SHIFT_MASK, TRUE, 0, 0,
  (GtkSignalFunc) on_bipartite_graph_item_activate, NULL, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
{ FACTORY_TYPE_ITEM, "random", "_Random graph",
  "Random graph",
  GDK_z, GDK_SHIFT_MASK, TRUE, 0, 0,
  (GtkSignalFunc) on_random_graph_item_activate, NULL, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
{ FACTORY_TYPE_ITEM, "regular", "_Regular graph",
  "Regular graph with degree n",
  GDK_r, GDK_SHIFT_MASK, FALSE, 0, 0,
  (GtkSignalFunc) on_regular_graph_item_activate, NULL, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
{ FACTORY_TYPE_ITEM, "star", "_Star graph",
  "Star graph with n vertices",
  GDK_s, GDK_SHIFT_MASK, TRUE, 0, 0,
  (GtkSignalFunc) on_star_graph_item_activate, NULL, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
{ FACTORY_TYPE_ITEM, "mitzenmacher", "_Mitzenmacher random graph",
  "Mitzenmacher random graph",
  GDK_m, GDK_SHIFT_MASK, TRUE, 0, 0,
  (GtkSignalFunc) on_mitzenmacher_random_graph_item_activate, NULL, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
FACTORY_RET
FACTORY_SEP
{ FACTORY_TYPE_MENU, "redrawg", "Re_draw graph", "Redraw graph",
  0, 0, TRUE, 0, 0, NULL, NULL, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
{ FACTORY_TYPE_ITEM, "fittowindow", "Fit to _window",
  "Redistribute graph into entire window",
  GDK_w, GDK_SHIFT_MASK, TRUE, 0, 0,
  (GtkSignalFunc) on_fit_to_window_item_activate, NULL, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
{ FACTORY_TYPE_ITEM, "circle", "As _circle",
  "Redraw graph using internal circle algorithm",
  GDK_c, GDK_SHIFT_MASK, TRUE, 0, 0,
  (GtkSignalFunc) on_draw_as_circle_item_activate, NULL, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
{ FACTORY_TYPE_ITEM, "neato", "Using _neato", "Redraw graph using neato",
  GDK_n, GDK_SHIFT_MASK, TRUE, 0, 0, 
  (GtkSignalFunc) on_draw_using_neato_item_activate, NULL, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
{ FACTORY_TYPE_ITEM, "dot", "Using _dot", " Redraw graph using dot",
  GDK_d, GDK_SHIFT_MASK, TRUE, 0, 0,
  (GtkSignalFunc) on_draw_using_dot_item_activate, NULL, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
FACTORY_RET
FACTORY_SEP
{ FACTORY_TYPE_ITEM, "toggle", "_Toggle graph/digraph",
  "Toggle graph mode",
  GDK_t, 0, FALSE, 0, 0,
  (GtkSignalFunc) on_toggle_graph_item_activate, NULL, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
FACTORY_RET
{ FACTORY_TYPE_MENU, "algorithms", "_Algorithms", "Graph algorithms",
  0, 0, TRUE, 0, 0, NULL, NULL, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
{ FACTORY_TYPE_MENU, "test", "_Test algorithms",
  "Algorithms that test a given graph property",
  0, 0, TRUE, 0, 0, NULL, NULL, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
FACTORY_RET
{ FACTORY_TYPE_MENU, "result", "_Result algorithms",
  "Algorithms with result",
  0, 0, TRUE, 0, 0, NULL, NULL, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
FACTORY_RET
{ FACTORY_TYPE_MENU, "step", "Algorithms _step by step",
  "Algorithms viewed step by step",
  0, 0, TRUE, 0, 0, NULL, NULL, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
FACTORY_RET
{ FACTORY_TYPE_MENU, "transforms", "T_ransformations",
  "Transformation algorithms",
  0, 0, TRUE, 0, 0, NULL, NULL, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
FACTORY_RET
{ FACTORY_TYPE_MENU, "search", "Sear_ch algorithms",
  "Search algorithms",
  0, 0, TRUE, 0, 0, NULL, NULL, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
FACTORY_RET
FACTORY_RET
{ FACTORY_TYPE_MENU, "options", "_Options", "Program options",
  0, 0, TRUE, 0, 0, NULL, NULL, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
{ FACTORY_TYPE_ITEM, "prefs", "_Preferences...", "Program preferences",
  GDK_p, GDK_CONTROL_MASK, TRUE, 0, 0,
  (GtkSignalFunc) on_preferences_item_activate, NULL, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
{ FACTORY_TYPE_ITEM, "plugins", "Plugins _info...", "Plugins information",
  GDK_i, GDK_CONTROL_MASK, TRUE, 0, 0,
  (GtkSignalFunc) on_plugins_item_activate, NULL, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
{ FACTORY_TYPE_MENU, "view", "_View mode", "Interface graph view mode",
  0, 0, TRUE, 0, 0, NULL, NULL, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
{ FACTORY_TYPE_RADIO, "opengl", "_Opengl", "Use gtklarea widget",
  GDK_o, GDK_SHIFT_MASK, TRUE, 1, 1,
  (GtkSignalFunc) on_view_mode_item_activate, NULL, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
{ FACTORY_TYPE_RADIO, "gtk", "_Gtk drawing area",
  "Use gtk drawing area widget",
  GDK_g, GDK_SHIFT_MASK, TRUE, 1, 0,
  (GtkSignalFunc) on_view_mode_item_activate, NULL, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
FACTORY_RET
FACTORY_RET
FACTORY_END
};


/* Toolbar */
factory_item main_toolbar[] = {
/* Type, name, label, tooltip,
   accel key, mod, sens, group, state, callback, func_data, func_name,
   pixmap, pixmap_name */
{ FACTORY_TYPE_BUTTON, "new", "New", "New file",
  0, 0, TRUE, 0, 0, (GtkSignalFunc) on_new_item_activate, NULL, NULL,
  FACTORY_PIXMAP_DATA, (char *) stock_new, NULL, NULL },
{ FACTORY_TYPE_BUTTON, "open", "Open", "Open file",
  0, 0, TRUE, 0, 0, (GtkSignalFunc) on_open_item_activate, NULL, NULL,
  FACTORY_PIXMAP_DATA, (char *) stock_open, NULL, NULL },
{ FACTORY_TYPE_BUTTON, "save", "Save", "Save file",
  0, 0, TRUE, 0, 0, (GtkSignalFunc) on_save_item_activate, NULL, NULL,
  FACTORY_PIXMAP_DATA, (char *) stock_save, NULL, NULL },
{ FACTORY_TYPE_BUTTON, "saveas", "Save As", "Save file with name",
  0, 0, TRUE, 0, 0, (GtkSignalFunc) on_saveas_item_activate, NULL, NULL,
  FACTORY_PIXMAP_DATA, (char *) stock_save_as, NULL, NULL },
FACTORY_SEP
{ FACTORY_TYPE_BUTTON, "firststep", "First Step", "Show first step",
  0, 0, TRUE, 0, 0, (GtkSignalFunc) on_firststep_item_activate, NULL, NULL,
  FACTORY_PIXMAP_DATA, (char *) stock_first, NULL, NULL },
{ FACTORY_TYPE_BUTTON, "previousstep", "Prev Step", "Show previous step",
  0, 0, TRUE, 0, 0, (GtkSignalFunc) on_previousstep_item_activate, NULL, NULL,
  FACTORY_PIXMAP_DATA, (char *) stock_left_arrow, NULL, NULL },
{ FACTORY_TYPE_BUTTON, "execstep", "Execute", "Execute step by step",
  0, 0, TRUE, 0, 0, (GtkSignalFunc) on_execstep_item_activate, NULL, NULL,
  FACTORY_PIXMAP_DATA, (char *) stock_exec, NULL, NULL },
{ FACTORY_TYPE_BUTTON, "nextstep", "Next Step", "Show next step",
  0, 0, TRUE, 0, 0, (GtkSignalFunc) on_nextstep_item_activate, NULL, NULL,
  FACTORY_PIXMAP_DATA, (char *) stock_right_arrow, NULL, NULL },
{ FACTORY_TYPE_BUTTON, "laststep", "Last Step", "Show last step",
  0, 0, TRUE, 0, 0, (GtkSignalFunc) on_laststep_item_activate, NULL, NULL,
  FACTORY_PIXMAP_DATA, (char *) stock_last, NULL, NULL },  
FACTORY_END
};


/* Popup menu */
factory_item popup_menu[] = {
/* Type, name, label, tooltip,
   accel key, mod, sens, group, state, callback, func_data, func_name,
   pixmap, pixmap_name, widget */
{ FACTORY_TYPE_ITEM, "exit", "E_xit", "Exit",
  0, 0, TRUE, 0, 0, gtk_main_quit, NULL, NULL,
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },
FACTORY_END
};


/* Stock toolbar items */
struct stock_toolbar_items {
    char *name;
    char *label;
    char *tooltip;
    GtkSignalFunc callback;
    char **pixmap;
} stock_items[] = {
    { "new", "New", "New file",
        (GtkSignalFunc) on_new_item_activate, stock_new },
    { "open", "Open", "Open file",
        (GtkSignalFunc) on_open_item_activate, stock_open },
    { "save", "Save", "Save file",
        (GtkSignalFunc) on_save_item_activate, stock_save },
    { "saveas", "Save as", "Save file with name", 
        (GtkSignalFunc) on_saveas_item_activate, stock_save_as },
    { "firststep", "First step", "Show first step", 
        (GtkSignalFunc) on_firststep_item_activate, stock_first },
    { "previousstep", "Prev step", "Show previous step", 
        (GtkSignalFunc) on_previousstep_item_activate, stock_left_arrow },
    { "execstep", "Execute", "Execute step by step", 
        (GtkSignalFunc) on_execstep_item_activate, stock_exec },
    { "nextstep", "Next step", "Show next step", 
        (GtkSignalFunc) on_nextstep_item_activate, stock_right_arrow },                
    { "laststep", "Last step", "Show last step", 
        (GtkSignalFunc) on_laststep_item_activate, stock_last },                
    { "about", "About", "About grafo",
        (GtkSignalFunc) on_about_item_activate, stock_menu_about },
    { "exit", "Exit", "Exit grafo", gtk_main_quit, stock_exit },
    { "reset", "Reset", "Remove vertices and edges",
        (GtkSignalFunc) on_reset_graph_item_activate, stock_clear },
    { "resetme", "Reset edges", "Reset marked edges",
        (GtkSignalFunc) on_reset_marked_edges_item_activate, stock_clear },
    { "resetmv", "Reset vertices", "Reset marked vertices",
        (GtkSignalFunc) on_reset_marked_vertices_item_activate,
        stock_clear },
    { "shctrlpts", "Show Control Points", "Show Control Points",
        (GtkSignalFunc) on_show_ctrlpts_item_activate, NULL },
    { "complete", "Complete", "Generate complete graph",
        (GtkSignalFunc) on_complete_graph_item_activate, NULL },
    { "bipartite", "Bipartite", "Generate bipartite graph",
        (GtkSignalFunc) on_bipartite_graph_item_activate, NULL },
    { "random", "Random", "Generate random graph",
        (GtkSignalFunc) on_random_graph_item_activate, NULL },
    { "regular", "Regular", "Generate regular graph",
        (GtkSignalFunc) on_regular_graph_item_activate, NULL },
    { "mitzenmacher", "Mitzenmacher random", "Generate Mitzenmacher random graph",
        (GtkSignalFunc) on_mitzenmacher_random_graph_item_activate, NULL },
    { "circle", "Circle", "Redraw graph as circle",
        (GtkSignalFunc) on_draw_as_circle_item_activate, NULL },
    { "neato", "Neato", "Redraw graph using neato",
        (GtkSignalFunc) on_draw_using_neato_item_activate, NULL },
    { "dot", "Dot", "Redraw graph using dot",
        (GtkSignalFunc) on_draw_using_dot_item_activate, NULL },
    { "prefs", "Preferences", "Program preferences",
        (GtkSignalFunc) on_preferences_item_activate, stock_preferences },
    { "pugins", "Plugins", "Plugins information",
        (GtkSignalFunc) on_plugins_item_activate, stock_preferences },
    { NULL, NULL, NULL, NULL, NULL },
};


/* Creates the main window */
void create_main_window(void){
GdkPixmap *pixmap;
GdkBitmap *mask;
GtkStyle *style;
GdkColormap *colormap;
#ifdef USE_GTKGL
int attrlist[] = {
    GDK_GL_RGBA,
    //GDK_GL_DOUBLEBUFFER,
    GDK_GL_NONE
};
#endif
 
    iface.initialized = 0;

    /* Creates main window tooltips and accels structs */
    iface.tooltips = gtk_tooltips_new();
    iface.window_accels = gtk_accel_group_new();

    /* Creates main window */
    iface.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(iface.window), MAIN_WINDOW_TITLE);
    gtk_window_set_default_size(GTK_WINDOW(iface.window),
            MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT);
    gtk_window_set_position(GTK_WINDOW(iface.window), GTK_WIN_POS_CENTER);
    gtk_signal_connect(GTK_OBJECT(iface.window), "delete_event",
            GTK_SIGNAL_FUNC(gtk_main_quit), NULL);
    gtk_widget_show(iface.window);

    /* Icon */
    style = gtk_widget_get_style(iface.window);
    colormap = gdk_colormap_get_system();
    pixmap = gdk_pixmap_colormap_create_from_xpm_d(iface.window->window,
                     colormap, &mask, &style->bg[GTK_STATE_NORMAL], k5_xpm);
    gdk_window_set_icon(iface.window->window, NULL, pixmap, mask);
    gdk_window_set_icon_name(iface.window->window, "grafo");
    iface.pixmap = gtk_pixmap_new(pixmap, mask);

    /* Creates main vbox */
    iface.vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(iface.window), iface.vbox);
    gtk_widget_show(iface.vbox);

    /* Creates menu bar */
    iface.menubar = factory_menubar(iface.window, main_menu,
            iface.window_accels, iface.tooltips);
    gtk_box_pack_start(GTK_BOX(iface.vbox),
            iface.menubar, FALSE, TRUE, 0);
    
    /* Creates toolbar */
    iface.toolbar = NULL;

    init_toolbar();

    /* Creates popup menu */
    iface.popup = factory_menu(iface.window, popup_menu,
            iface.window_accels, iface.tooltips);

    /* Creates drawing area */
    iface.drawingarea = gtk_drawing_area_new();
    gtk_widget_set_usize(GTK_WIDGET(iface.drawingarea),
            MIN_DRAWING_XSIZE,
            MIN_DRAWING_YSIZE);
    gtk_signal_connect(GTK_OBJECT(iface.drawingarea),
            "expose_event",
            GTK_SIGNAL_FUNC(expose_event_callback),
            iface.drawingarea);
    gtk_signal_connect(GTK_OBJECT(iface.drawingarea),
            "configure_event",
            GTK_SIGNAL_FUNC(resize_event_callback),
            iface.drawingarea);
    gtk_signal_connect(GTK_OBJECT(iface.drawingarea),
            "button_press_event",
            GTK_SIGNAL_FUNC(button_press_event_callback),
            iface.drawingarea);
    gtk_signal_connect(GTK_OBJECT(iface.drawingarea),
            "button_release_event",
            GTK_SIGNAL_FUNC(button_press_event_callback),
            iface.drawingarea);
    gtk_signal_connect(GTK_OBJECT(iface.drawingarea),
            "enter_notify_event",
            GTK_SIGNAL_FUNC(enter_notify_event_callback),
            iface.drawingarea);
    gtk_signal_connect(GTK_OBJECT(iface.drawingarea),
            "leave_notify_event",
            GTK_SIGNAL_FUNC(leave_notify_event_callback),
            iface.drawingarea);
    gtk_signal_connect(GTK_OBJECT(iface.drawingarea),
            "motion_notify_event",
            GTK_SIGNAL_FUNC(motion_notify_event_callback),
            iface.drawingarea);
    gtk_signal_connect(GTK_OBJECT(iface.drawingarea),
            "realize",
            GTK_SIGNAL_FUNC(init_drawingarea),
            iface.drawingarea);
    gtk_widget_set_events(iface.drawingarea,
            gtk_widget_get_events(iface.drawingarea) |
                GDK_BUTTON_PRESS_MASK |  GDK_BUTTON_RELEASE_MASK |
                GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK |
                GDK_POINTER_MOTION_MASK);

    gtk_widget_show(iface.drawingarea);
    gtk_box_pack_start(GTK_BOX(iface.vbox),
            iface.drawingarea, TRUE, TRUE, 0);
    iface.view_mode = VIEW_MODE_GTK;
    gtk_check_menu_item_set_active(
            GTK_CHECK_MENU_ITEM(get_main_menu_item_by_name("gtk")), TRUE);

#ifdef USE_GTKGL
    /* Creates gtkglarea */
    if (gdk_gl_query() == FALSE) {
        g_print("OpenGL not supported\n");
        gtk_widget_set_sensitive(get_main_menu_item_by_name("opengl"),
                FALSE);
    } else {

        iface.glarea = GTK_WIDGET(gtk_gl_area_new(attrlist));
       
        if (!iface.glarea){
            g_print("OpenGL not supported\n");
            gtk_widget_set_sensitive(get_main_menu_item_by_name("opengl"),
                    FALSE);
        } else {
            gtk_widget_set_usize(GTK_WIDGET(iface.glarea),
                    MIN_DRAWING_XSIZE, MIN_DRAWING_YSIZE);
            gtk_signal_connect(GTK_OBJECT(iface.glarea),
                    "expose_event",
                    GTK_SIGNAL_FUNC(expose_event_callback),
                    iface.glarea);
            gtk_signal_connect(GTK_OBJECT(iface.glarea),
                    "configure_event",
                    GTK_SIGNAL_FUNC(resize_event_callback),
                    iface.glarea);
            gtk_signal_connect(GTK_OBJECT(iface.glarea),
                    "button_press_event",
                    GTK_SIGNAL_FUNC(button_press_event_callback),
                    iface.glarea);
            gtk_signal_connect(GTK_OBJECT(iface.glarea),
                    "button_release_event",
                    GTK_SIGNAL_FUNC(button_press_event_callback),
                    iface.glarea);
            gtk_signal_connect(GTK_OBJECT(iface.glarea),
                    "enter_notify_event",
                    GTK_SIGNAL_FUNC(enter_notify_event_callback),
                    iface.glarea);
            gtk_signal_connect(GTK_OBJECT(iface.glarea),
                    "leave_notify_event",
                    GTK_SIGNAL_FUNC(leave_notify_event_callback),
                    iface.glarea);
            gtk_signal_connect(GTK_OBJECT(iface.glarea),
                    "motion_notify_event",
                    GTK_SIGNAL_FUNC(motion_notify_event_callback),
                    iface.glarea);
            gtk_signal_connect(GTK_OBJECT(iface.glarea),
                    "realize",
                    GTK_SIGNAL_FUNC(init_glarea), NULL);
            gtk_widget_set_events(iface.glarea,
                    gtk_widget_get_events(iface.glarea) |
                        GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK |
                        GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK |
                        GDK_POINTER_MOTION_MASK);

            gtk_box_pack_start(GTK_BOX(iface.vbox),
                    iface.glarea, TRUE, TRUE, 0);

            if (config->view_mode == VIEW_MODE_OPENGL){
                gtk_widget_hide(iface.drawingarea);
                gtk_widget_show(iface.glarea);
                iface.view_mode = VIEW_MODE_OPENGL;
                gtk_check_menu_item_set_active(
                        GTK_CHECK_MENU_ITEM(
                            get_main_menu_item_by_name("opengl")), TRUE);
            }

            /* Detroy GLX context on window exit */
            //gtk_quit_add_destroy(1, GTK_OBJECT(iface.window));
        }
    }
#else
    gtk_widget_set_sensitive(get_main_menu_item_by_name("opengl"), FALSE);
#endif

    /* Default edit mode */
    iface.edit_mode = NOTHING;
    iface.changed = 0;
    iface.back = NULL;

    /* Creates statusbar */
    iface.statusbar = gtk_statusbar_new();
    gtk_widget_show(iface.statusbar);
    gtk_box_pack_start(GTK_BOX(iface.vbox),
            iface.statusbar, FALSE, FALSE, 0);
    
    /* Adds accels */
    gtk_window_add_accel_group(GTK_WINDOW(iface.window),
            iface.window_accels);

    /* Creates cursors */
    iface.cursor_indx = 0;
    iface.cursor[0] = gdk_cursor_new(GDK_TOP_LEFT_ARROW);
    iface.cursor[1] = gdk_cursor_new(GDK_ARROW);
    iface.cursor[2] = gdk_cursor_new(GDK_CROSSHAIR);
    iface.cursor[3] = gdk_cursor_new(GDK_PIRATE);
    iface.cursor[4] = gdk_cursor_new(GDK_QUESTION_ARROW);

    /* Reset status strings */
    iface.help_string = NULL;
    iface.status_string[0] = 0;

    /* Reset file name */
    iface.filename = NULL;
    iface.format = 0;
    
    /* Create dialogs */
    create_open_filesel_dialog();
    create_save_filesel_dialog();
    create_about_dialog();
    create_msg_dialog();
    create_conf_dialog();
    create_preferences_dialog();
    create_plugins_dialog();
    create_graph_dialog();
    create_vprop_dialog();
    create_eprop_dialog();

    iface.vertex_indx = 0;
    iface.edge_indx[0] = iface.edge_indx[1] = 0;
    iface.vlabel_mode = 1;
    iface.elabel_mode = 3;
    gtk_check_menu_item_set_active(
            GTK_CHECK_MENU_ITEM(get_main_menu_item_by_name("indxvlab")),
            TRUE);
    gtk_check_menu_item_set_active(
            GTK_CHECK_MENU_ITEM(get_main_menu_item_by_name("labelelab")),
            TRUE);

    iface.initialized = 1;
}


/* Creates menu items for format option menus */
void create_format_items(GtkWidget *menu, int plain, int psitem){
GtkWidget *menuitem;

    menuitem = gtk_menu_item_new_with_label(
            "Automatic (by file extension)");
    gtk_menu_append(GTK_MENU(menu), menuitem);
    gtk_widget_show(menuitem);
    gtk_object_set_data(GTK_OBJECT(menuitem), "itemindex", getintp(0));
    
    menuitem = gtk_menu_item_new_with_label(
            "gr file (Grafo format)");
    gtk_menu_append(GTK_MENU(menu), menuitem);
    gtk_widget_show(menuitem);
    gtk_object_set_data(GTK_OBJECT(menuitem), "itemindex", getintp(1));

    if (plain){
        menuitem = gtk_menu_item_new_with_label(
                "plain dot file (GraphViz format)");
    } else {
        menuitem = gtk_menu_item_new_with_label(
                "dot file (GraphViz format)");
    }
    gtk_menu_append(GTK_MENU(menu), menuitem);
    gtk_widget_show(menuitem);
    gtk_object_set_data(GTK_OBJECT(menuitem), "itemindex", getintp(2));

    if (psitem){
        menuitem = gtk_menu_item_new_with_label(
                "ps file (PostScript format)");
        gtk_menu_append(GTK_MENU(menu), menuitem);
        gtk_widget_show(menuitem);
        gtk_object_set_data(GTK_OBJECT(menuitem), "itemindex", getintp(3));
    }
}


void create_open_filesel_dialog(void){
GtkWidget *frame;

    iface.open_filesel = factory_filesel_dialog("Open file", 
            (GtkSignalFunc) on_open_filesel_ok,
            (GtkSignalFunc) factory_hide_dialog_callback);

    frame = gtk_frame_new("File type");
    gtk_widget_show(GTK_WIDGET(frame));
    gtk_box_pack_start(GTK_BOX(
            GTK_FILE_SELECTION(iface.open_filesel)->main_vbox),
            frame, TRUE, TRUE, 0);

    iface.open_filesel_menu = gtk_menu_new();
    create_format_items(iface.open_filesel_menu, 1, 0);

    iface.open_filesel_optionmenu = gtk_option_menu_new();
    gtk_option_menu_set_menu(
            GTK_OPTION_MENU(iface.open_filesel_optionmenu),
            iface.open_filesel_menu);
    gtk_container_add(GTK_CONTAINER(frame),
            iface.open_filesel_optionmenu);
    gtk_widget_show(iface.open_filesel_optionmenu);

    if ((config->datadir_path != NULL) &&
        (strlen(config->datadir_path) > 0))
        gtk_file_selection_set_filename(GTK_FILE_SELECTION(
                iface.open_filesel), config->datadir_path);
}


void create_save_filesel_dialog(void){
GtkWidget *frame;

    iface.save_filesel = factory_filesel_dialog("Save file", 
            (GtkSignalFunc) on_save_filesel_ok,
            (GtkSignalFunc) factory_hide_dialog_callback);

    frame = gtk_frame_new("File type");
    gtk_widget_show(GTK_WIDGET(frame));
    gtk_box_pack_start(GTK_BOX(
            GTK_FILE_SELECTION(iface.save_filesel)->main_vbox),
            frame, TRUE, TRUE, 0);

    iface.save_filesel_menu = gtk_menu_new();
    create_format_items(iface.save_filesel_menu, 0, 1);

    iface.save_filesel_optionmenu = gtk_option_menu_new();
    gtk_option_menu_set_menu(
            GTK_OPTION_MENU(iface.save_filesel_optionmenu),
            iface.save_filesel_menu);
    gtk_container_add(GTK_CONTAINER(frame),
            iface.save_filesel_optionmenu);
    gtk_widget_show(iface.save_filesel_optionmenu);
    
    if ((config->datadir_path != NULL) &&
        (strlen(config->datadir_path) > 0))
        gtk_file_selection_set_filename(GTK_FILE_SELECTION(
                iface.save_filesel), config->datadir_path);
}


void create_about_dialog(void){
GtkWidget *vbox, *hbox, *label, *action_area, *button;

    iface.about_dialog = gtk_dialog_new();
    gtk_window_set_title(GTK_WINDOW(iface.about_dialog), "About grafo");
    gtk_window_set_position(GTK_WINDOW(iface.about_dialog),
            GTK_WIN_POS_MOUSE); 
    gtk_window_set_policy(GTK_WINDOW(iface.about_dialog),
            TRUE, TRUE, FALSE);

    vbox = GTK_DIALOG(iface.about_dialog)->vbox;
    gtk_widget_show(vbox);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);  
   
    hbox = gtk_hbox_new(FALSE, 10);
    gtk_widget_show(hbox);
    gtk_container_add(GTK_CONTAINER(vbox), hbox);

    gtk_widget_show(iface.pixmap);
    gtk_container_add(GTK_CONTAINER(hbox), iface.pixmap);

    label = gtk_label_new("grafo - graph editor\n"
            "Copyright (c) 2003-2008\n"
            "Murilo Vicente Goncalves da Silva <murilo@pet.inf.ufpr.br>\n"
            "Oliver Matias van Kaick <oliver@pet.inf.ufpr.br>\n"
            "Ulisses Cordeiro Pereira <ulisses@cordeiropereira.com.br>\n");
    gtk_widget_show(label);
    gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 10);
    gtk_misc_set_padding(GTK_MISC(label), 12, 0);

    action_area = GTK_DIALOG(iface.about_dialog)->action_area;
    gtk_widget_show(action_area);
    gtk_container_set_border_width(GTK_CONTAINER(action_area), 10);

    button = gtk_button_new_with_label("  OK  ");
    gtk_widget_show(button);
    gtk_box_pack_start(GTK_BOX(action_area), button, FALSE, FALSE, 0);
    GTK_WIDGET_SET_FLAGS(button, GTK_CAN_DEFAULT);
    gtk_signal_connect(GTK_OBJECT(button), "clicked",
                       GTK_SIGNAL_FUNC(factory_hide_toplevel_dialog), NULL);
    gtk_widget_grab_default(button);
}


void create_msg_dialog(void){

    iface.msg_dialog = factory_msg_dialog("Message", "",
        (GtkSignalFunc) factory_hide_dialog_callback);
}


void create_conf_dialog(void){

    iface.conf_dialog = factory_confirm_dialog("Confirmation",
        "File exists, overwrite it?",
        (GtkSignalFunc) on_conf_yes,
        (GtkSignalFunc) factory_hide_dialog_callback);
}


void create_preferences_dialog(void){
GtkWidget *vbox, *notebook, *table, *label, *choose;
GtkWidget *action_area, *ok_button, *cancel_button;
GtkWidget *menuitem;

    /* Creates the preferences dialog */
    iface.pref_dialog = gtk_dialog_new();
    gtk_window_set_title(GTK_WINDOW(iface.pref_dialog), "Preferences");
    gtk_window_set_position(GTK_WINDOW(iface.pref_dialog),
            GTK_WIN_POS_MOUSE);
    gtk_window_set_policy(GTK_WINDOW(iface.pref_dialog),
            TRUE, TRUE, FALSE);

    vbox = GTK_DIALOG(iface.pref_dialog)->vbox;
    gtk_widget_show(vbox);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 2);

    notebook = gtk_notebook_new();
    gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_TOP);
    gtk_widget_show(notebook);
    gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 10);

    /* GraphViz page */
    table = gtk_table_new(2, 3, FALSE);
    gtk_container_set_border_width(GTK_CONTAINER(table), 10);
    gtk_widget_set_usize(table, 500, 200);
    gtk_widget_show(table);
   
    /* Neato path */
    label = gtk_label_new("Neato path");
    gtk_widget_show(label);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 0, 1,
            GTK_FILL, GTK_FILL, 4, 4);

    iface.pref_dialog_neato_text = gtk_entry_new();
    gtk_widget_show(iface.pref_dialog_neato_text);
    gtk_table_attach(GTK_TABLE(table),
        iface.pref_dialog_neato_text, 1, 2, 0, 1,
        GTK_FILL | GTK_EXPAND, GTK_FILL, 4, 4);

    if (config->neato_path != NULL)
        gtk_entry_set_text(GTK_ENTRY(iface.pref_dialog_neato_text),
                config->neato_path);
    
    choose = gtk_button_new_with_label("Choose...");
    gtk_widget_show(choose);
    gtk_table_attach(GTK_TABLE(table), choose, 2, 3, 0, 1,
            GTK_FILL, GTK_FILL, 4, 4);
    gtk_signal_connect(GTK_OBJECT(choose), "clicked",
            GTK_SIGNAL_FUNC(on_choose_neato_button_clicked), NULL);
    
    iface.pref_dialog_neato_filesel =
        factory_filesel_dialog("Neato path",
                (GtkSignalFunc) on_neato_filesel_ok,
                (GtkSignalFunc) factory_hide_dialog_callback);

    if ((config->neato_path != NULL) &&
        (strlen(config->neato_path) > 0))
        gtk_file_selection_set_filename(GTK_FILE_SELECTION(
            iface.pref_dialog_neato_filesel), config->neato_path);
    
    /* Dot path */
    label = gtk_label_new("Dot path");
    gtk_widget_show(label);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 1, 2,
            GTK_FILL, GTK_FILL, 4, 4);

    iface.pref_dialog_dot_text = gtk_entry_new();
    gtk_widget_show(iface.pref_dialog_dot_text);
    gtk_table_attach(GTK_TABLE(table),
        iface.pref_dialog_dot_text, 1, 2, 1, 2,
        GTK_FILL | GTK_EXPAND, GTK_FILL, 4, 4);

    if (config->dot_path != NULL)
        gtk_entry_set_text(GTK_ENTRY(iface.pref_dialog_dot_text),
                config->dot_path);
    
    choose = gtk_button_new_with_label("Choose...");
    gtk_widget_show(choose);
    gtk_table_attach(GTK_TABLE(table), choose, 2, 3, 1, 2,
            GTK_FILL, GTK_FILL, 4, 4);
    gtk_signal_connect(GTK_OBJECT(choose), "clicked",
            GTK_SIGNAL_FUNC(on_choose_dot_button_clicked), NULL);
    
    iface.pref_dialog_dot_filesel =
        factory_filesel_dialog("Dot path",
                (GtkSignalFunc) on_dot_filesel_ok,
                (GtkSignalFunc) factory_hide_dialog_callback);

    if ((config->dot_path != NULL) &&
        (strlen(config->dot_path) > 0))
        gtk_file_selection_set_filename(GTK_FILE_SELECTION(
            iface.pref_dialog_dot_filesel), config->dot_path);

    /* GraphViz page */
    label = gtk_label_new("GraphViz configuration");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), table, label);

    /* Default paths page */
    table = gtk_table_new(2, 3, FALSE);
    gtk_container_set_border_width(GTK_CONTAINER(table), 10);
    gtk_widget_set_usize(table, 500, 200);
    gtk_widget_show(table);  

    /* Plugins path */
    label = gtk_label_new("Plugins path");
    gtk_widget_show(label);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 0, 1,
            GTK_FILL, GTK_FILL, 4, 4);

    iface.pref_dialog_plugins_text = gtk_entry_new();
    gtk_widget_show(iface.pref_dialog_plugins_text);
    gtk_table_attach(GTK_TABLE(table),
        iface.pref_dialog_plugins_text, 1, 2, 0, 1,
        GTK_FILL | GTK_EXPAND, GTK_FILL, 4, 4);

    if (config->plugins_path != NULL)
        gtk_entry_set_text(GTK_ENTRY(iface.pref_dialog_plugins_text),
                config->plugins_path);
    
    choose = gtk_button_new_with_label("Choose...");
    gtk_widget_show(choose);
    gtk_table_attach(GTK_TABLE(table), choose, 2, 3, 0, 1,
            GTK_FILL, GTK_FILL, 4, 4);
    gtk_signal_connect(GTK_OBJECT(choose), "clicked",
            GTK_SIGNAL_FUNC(on_choose_plugins_button_clicked), NULL);
    
    iface.pref_dialog_plugins_filesel =
        factory_filesel_dialog("Plugins path",
                (GtkSignalFunc) on_plugins_filesel_ok,
                (GtkSignalFunc) factory_hide_dialog_callback);

    if ((config->plugins_path != NULL) &&
        (strlen(config->plugins_path) > 0))
        gtk_file_selection_set_filename(GTK_FILE_SELECTION(
            iface.pref_dialog_plugins_filesel), config->plugins_path);

    /* Datadir path */
    label = gtk_label_new("Datadir path");
    gtk_widget_show(label);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 1, 2,
            GTK_FILL, GTK_FILL, 4, 4);

    iface.pref_dialog_datadir_text = gtk_entry_new();
    gtk_widget_show(iface.pref_dialog_datadir_text);
    gtk_table_attach(GTK_TABLE(table),
        iface.pref_dialog_datadir_text, 1, 2, 1, 2,
        GTK_FILL | GTK_EXPAND, GTK_FILL, 4, 4);

    if (config->datadir_path != NULL)
        gtk_entry_set_text(GTK_ENTRY(iface.pref_dialog_datadir_text),
                config->datadir_path);
    
    choose = gtk_button_new_with_label("Choose...");
    gtk_widget_show(choose);
    gtk_table_attach(GTK_TABLE(table), choose, 2, 3, 1, 2,
            GTK_FILL, GTK_FILL, 4, 4);
    gtk_signal_connect(GTK_OBJECT(choose), "clicked",
            GTK_SIGNAL_FUNC(on_choose_datadir_button_clicked), NULL);
    
    iface.pref_dialog_datadir_filesel =
        factory_filesel_dialog("Plugins path",
                (GtkSignalFunc) on_datadir_filesel_ok,
                (GtkSignalFunc) factory_hide_dialog_callback);

    if ((config->datadir_path != NULL) &&
        (strlen(config->datadir_path) > 0))
        gtk_file_selection_set_filename(GTK_FILE_SELECTION(
            iface.pref_dialog_datadir_filesel), config->datadir_path);
    
    /* Default paths page */
    label = gtk_label_new("Default paths");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), table, label);
    
    /* Default view mode page */
    table = gtk_table_new(1, 2, FALSE);
    gtk_container_set_border_width(GTK_CONTAINER(table), 10);
    gtk_widget_set_usize(table, 500, 200);
    gtk_widget_show(table);
    
    /* Default view mode */
    label = gtk_label_new("Default view mode");
    gtk_widget_show(label);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 1, 2,
            GTK_FILL, GTK_FILL, 4, 4);

    iface.pref_dialog_view_mode_menu = gtk_menu_new();

    menuitem = gtk_menu_item_new_with_label(
            "Opengl (gtkgl area)");
    gtk_menu_append(GTK_MENU(iface.pref_dialog_view_mode_menu), menuitem);
    gtk_widget_show(menuitem);
    gtk_object_set_data(GTK_OBJECT(menuitem), "itemindex",
            getintp(VIEW_MODE_OPENGL));
    
    menuitem = gtk_menu_item_new_with_label(
            "Gtk (gtk drawing area)");
    gtk_menu_append(GTK_MENU(iface.pref_dialog_view_mode_menu), menuitem);
    gtk_widget_show(menuitem);
    gtk_object_set_data(GTK_OBJECT(menuitem), "itemindex",
            getintp(VIEW_MODE_GTK));
    
    iface.pref_dialog_view_mode_optionmenu = gtk_option_menu_new();
    gtk_option_menu_set_menu(
            GTK_OPTION_MENU(iface.pref_dialog_view_mode_optionmenu),
            iface.pref_dialog_view_mode_menu);
    gtk_option_menu_set_history(GTK_OPTION_MENU(
            iface.pref_dialog_view_mode_optionmenu), config->view_mode);
    gtk_widget_show(iface.pref_dialog_view_mode_optionmenu);
    gtk_table_attach(GTK_TABLE(table),
            iface.pref_dialog_view_mode_optionmenu, 1, 2, 1, 2,
            GTK_FILL, GTK_FILL, 4, 4);

    /* Default view mode page */
    label = gtk_label_new("Default view mode");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), table, label);

    /* Rest of the preferences dialog */
    action_area = GTK_DIALOG(iface.pref_dialog)->action_area;
    gtk_widget_show(action_area);
    gtk_container_set_border_width(GTK_CONTAINER(action_area), 10);

    ok_button = gtk_button_new_with_label("  OK  ");
    gtk_widget_show(ok_button);
    gtk_box_pack_start(GTK_BOX(action_area), ok_button, FALSE, FALSE, 0);
    GTK_WIDGET_SET_FLAGS(ok_button, GTK_CAN_DEFAULT);
    gtk_signal_connect(GTK_OBJECT(ok_button), "clicked",
                       GTK_SIGNAL_FUNC(on_preferences_ok_clicked),
                       NULL);
    gtk_widget_grab_default(ok_button);
    
    cancel_button = gtk_button_new_with_label(" Cancel ");
    gtk_widget_show(cancel_button);
    gtk_box_pack_start(GTK_BOX(action_area), cancel_button,
            FALSE, FALSE, 0);
    GTK_WIDGET_SET_FLAGS(cancel_button, GTK_CAN_DEFAULT);
    gtk_signal_connect(GTK_OBJECT(cancel_button), "clicked",
                       GTK_SIGNAL_FUNC(on_preferences_cancel_clicked),
                       NULL);
}


void create_plugins_dialog(void){
GtkWidget *vbox, *notebook, *action_area;
GtkWidget *scroll, *label, *list, *ok_button;
gchar *titles[2] = { "File", "Status" };
gchar *ftitles[3] = { "Funcion", "In file", "Status" };
gchar *items[2];
gchar *fitems[3];
int i;
gchar *string[5] = {
    "Loaded", "Error opening file",
    "Error retriving plugin information",
    "Error retriving symbol",
    "Unknown status" };

    /* Creates the plugins dialog */
    iface.pgin_dialog = gtk_dialog_new();
    gtk_window_set_title(GTK_WINDOW(iface.pgin_dialog),
            "Plugins information");
    gtk_window_set_position(GTK_WINDOW(iface.pgin_dialog),
            GTK_WIN_POS_CENTER);
    gtk_window_set_policy(GTK_WINDOW(iface.pgin_dialog),
            TRUE, TRUE, FALSE);

    vbox = GTK_DIALOG(iface.pgin_dialog)->vbox;
    gtk_widget_show(vbox);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 2);

    notebook = gtk_notebook_new();
    gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_TOP);
    gtk_widget_show(notebook);
    gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 10);

    /* Creates files info page */
    scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_usize(scroll, 600, 300);
    gtk_widget_show(scroll);

    label = gtk_label_new("Plugins files info");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scroll, label);
    
    list = gtk_clist_new_with_titles(2, titles);
    gtk_clist_set_column_width(GTK_CLIST(list), 0, 300);
    gtk_clist_set_column_width(GTK_CLIST(list), 1, 300);
    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scroll),
        list);
    gtk_widget_show(list);
    
    for (i = 0; i < plugins.file_count; i++){
        items[0] = plugins.file[i].name;
        if (plugins.file[i].status == PGIN_STATUS_LOADED)
            items[1] = string[0];
        else if (plugins.file[i].status == PGIN_STATUS_ERROR_OPEN)
            items[1] = string[1];
        else if (plugins.file[i].status == PGIN_STATUS_ERROR_INFO)
            items[1] = string[2];
        else
            items[1] = string[4];
        gtk_clist_append(GTK_CLIST(list), items);
    }

    if (plugins.file_count == 0){
        items[0] = "No files loaded";
        items[1] = "";
        gtk_clist_append(GTK_CLIST(list), items);
    }

    /* Creates functions info page */
    scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_usize(scroll, 600, 300);
    gtk_widget_show(scroll);

    label = gtk_label_new("Plugins functions info");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scroll, label);
    
    list = gtk_clist_new_with_titles(3, ftitles);
    gtk_clist_set_column_width(GTK_CLIST(list), 0, 150);
    gtk_clist_set_column_width(GTK_CLIST(list), 1, 300);
    gtk_clist_set_column_width(GTK_CLIST(list), 2, 200);
    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scroll),
        list);
    gtk_widget_show(list);   

    for (i = 0; i < plugins.func_count; i++){
        fitems[0] = plugins.func[i].name;
        fitems[1] = plugins.func[i].file;
        if (plugins.func[i].status == PGIN_STATUS_LOADED)
            fitems[2] = string[0];
        else if (plugins.func[i].status == PGIN_STATUS_ERROR_SYMBOL)
            fitems[2] = string[3];
        else
            fitems[2] = string[4];
        gtk_clist_append(GTK_CLIST(list), fitems);
    }
    
    if (plugins.func_count == 0){
        fitems[0] = "No functions loaded";
        fitems[1] = "";
        fitems[2] = "";
        gtk_clist_append(GTK_CLIST(list), fitems);
    }

    /* Bottom of the plugins dialog */
    action_area = GTK_DIALOG(iface.pgin_dialog)->action_area;
    gtk_widget_show(action_area);
    gtk_container_set_border_width(GTK_CONTAINER(action_area), 10);

    ok_button = gtk_button_new_with_label("  OK  ");
    gtk_widget_show(ok_button);
    gtk_box_pack_start(GTK_BOX(action_area), ok_button, FALSE, FALSE, 0);
    GTK_WIDGET_SET_FLAGS(ok_button, GTK_CAN_DEFAULT);
    gtk_signal_connect(GTK_OBJECT(ok_button), "clicked",
                       GTK_SIGNAL_FUNC(factory_hide_dialog_callback),
                       NULL);
    gtk_widget_grab_default(ok_button);
}


void create_graph_dialog(void){
GtkWidget *vbox, *table, *label;
GtkWidget *action_area, *ok_button, *cancel_button;
GtkObject *adj;

    /* Creates the graph dialog */
    iface.graph_dialog = gtk_dialog_new();
    gtk_window_set_title(GTK_WINDOW(iface.graph_dialog), "Generate graph");
    gtk_window_set_position(GTK_WINDOW(iface.graph_dialog),
            GTK_WIN_POS_MOUSE);
    gtk_window_set_policy(GTK_WINDOW(iface.graph_dialog),
            TRUE, TRUE, FALSE);

    vbox = GTK_DIALOG(iface.graph_dialog)->vbox;
    gtk_widget_show(vbox);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 2);

    iface.graph_dialog_notebook = gtk_notebook_new();
    gtk_notebook_set_tab_pos(GTK_NOTEBOOK(iface.graph_dialog_notebook),
            GTK_POS_TOP);
    gtk_widget_show(iface.graph_dialog_notebook);
    gtk_box_pack_start(GTK_BOX(vbox),
            iface.graph_dialog_notebook, TRUE, TRUE, 10);

    
    /* Complete graph */
    table = gtk_table_new(1, 3, FALSE);
    gtk_container_set_border_width(GTK_CONTAINER(table), 10);
    gtk_widget_set_usize(table, 500, 100);
    gtk_widget_show(table);
   
    label = gtk_label_new("Number of vertices");
    gtk_widget_show(label);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 0, 1,
            GTK_FILL, GTK_FILL, 4, 4);

    adj = gtk_adjustment_new(5, 1, 65535, 1, 10, 10);
    iface.graph_dialog_complete_spin_button =
            gtk_spin_button_new(GTK_ADJUSTMENT(adj), 0, 0);
    gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(
            iface.graph_dialog_complete_spin_button), TRUE);
    gtk_widget_show(iface.graph_dialog_complete_spin_button);
    gtk_table_attach(GTK_TABLE(table),
        iface.graph_dialog_complete_spin_button, 1, 2, 0, 1,
        GTK_FILL | GTK_EXPAND, GTK_FILL, 4, 4);

    label = gtk_label_new("Complete graph");
    gtk_notebook_append_page(GTK_NOTEBOOK(
            iface.graph_dialog_notebook), table, label);
    
    /* Bipartite graph */
    table = gtk_table_new(2, 3, FALSE);
    gtk_container_set_border_width(GTK_CONTAINER(table), 10);
    gtk_widget_set_usize(table, 500, 100);
    gtk_widget_show(table);
   
    label = gtk_label_new("Number of vertices of group 1");
    gtk_widget_show(label);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 0, 1,
            GTK_FILL, GTK_FILL, 4, 4);

    adj = gtk_adjustment_new(3, 1, 65535, 1, 10, 10);
    iface.graph_dialog_bipartite1_spin_button =
            gtk_spin_button_new(GTK_ADJUSTMENT(adj), 0, 0);
    gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(
            iface.graph_dialog_bipartite1_spin_button), TRUE);
    gtk_widget_show(iface.graph_dialog_bipartite1_spin_button);
    gtk_table_attach(GTK_TABLE(table),
        iface.graph_dialog_bipartite1_spin_button, 1, 2, 0, 1,
        GTK_FILL | GTK_EXPAND, GTK_FILL, 4, 4);
    
    label = gtk_label_new("Number of vertices of group 2");
    gtk_widget_show(label);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 1, 2,
            GTK_FILL, GTK_FILL, 4, 4);

    adj = gtk_adjustment_new(3, 1, 65535, 1, 10, 10);
    iface.graph_dialog_bipartite2_spin_button =
            gtk_spin_button_new(GTK_ADJUSTMENT(adj), 0, 0);
    gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(
            iface.graph_dialog_bipartite2_spin_button), TRUE);
    gtk_widget_show(iface.graph_dialog_bipartite2_spin_button);
    gtk_table_attach(GTK_TABLE(table),
        iface.graph_dialog_bipartite2_spin_button, 1, 2, 1, 2,
        GTK_FILL | GTK_EXPAND, GTK_FILL, 4, 4);
    
    label = gtk_label_new("Bipartite complete graph");
    gtk_notebook_append_page(GTK_NOTEBOOK(
            iface.graph_dialog_notebook), table, label);


    /* Random graph */
    table = gtk_table_new(3, 3, FALSE);
    gtk_container_set_border_width(GTK_CONTAINER(table), 10);
    gtk_widget_set_usize(table, 500, 100);
    gtk_widget_show(table);
   
    label = gtk_label_new("Number of vertices");
    gtk_widget_show(label);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 0, 1,
            GTK_FILL, GTK_FILL, 4, 4);

    adj = gtk_adjustment_new(3, 1, 65535, 1, 10, 10);
    iface.graph_dialog_random1_spin_button =
            gtk_spin_button_new(GTK_ADJUSTMENT(adj), 0, 0);
    gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(
            iface.graph_dialog_random1_spin_button), TRUE);
    gtk_widget_show(iface.graph_dialog_random1_spin_button);
    gtk_table_attach(GTK_TABLE(table),
        iface.graph_dialog_random1_spin_button, 1, 2, 0, 1,
        GTK_FILL | GTK_EXPAND, GTK_FILL, 4, 4);

    label = gtk_label_new("Edge probability");
    gtk_widget_show(label);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 1, 2,
            GTK_FILL, GTK_FILL, 4, 4);

    adj = gtk_adjustment_new(0.5, 0, 1, 0.1, 0.1, 0.1);
    iface.graph_dialog_random2_spin_button =
            gtk_spin_button_new(GTK_ADJUSTMENT(adj), 0, 4);
    gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(
            iface.graph_dialog_random2_spin_button), TRUE);
    gtk_widget_show(iface.graph_dialog_random2_spin_button);
    gtk_table_attach(GTK_TABLE(table),
        iface.graph_dialog_random2_spin_button, 1, 2, 1, 2,
        GTK_FILL | GTK_EXPAND, GTK_FILL, 4, 4);
    
    label = gtk_label_new("Random graph");
    gtk_notebook_append_page(GTK_NOTEBOOK(
            iface.graph_dialog_notebook), table, label);
    
    /* Regular graph */
    table = gtk_table_new(3, 3, FALSE);
    gtk_container_set_border_width(GTK_CONTAINER(table), 10);
    gtk_widget_set_usize(table, 500, 100);
    gtk_widget_show(table);
    gtk_widget_set_sensitive(GTK_WIDGET(table), FALSE);
   
    label = gtk_label_new("Number of vertices");
    gtk_widget_show(label);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 0, 1,
            GTK_FILL, GTK_FILL, 4, 4);

    adj = gtk_adjustment_new(3, 1, 65535, 1, 10, 10);
    iface.graph_dialog_regular1_spin_button =
            gtk_spin_button_new(GTK_ADJUSTMENT(adj), 0, 0);
    gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(
            iface.graph_dialog_regular1_spin_button), TRUE);
    gtk_widget_show(iface.graph_dialog_regular1_spin_button);
    gtk_table_attach(GTK_TABLE(table),
        iface.graph_dialog_regular1_spin_button, 1, 2, 0, 1,
        GTK_FILL | GTK_EXPAND, GTK_FILL, 4, 4);

    label = gtk_label_new("Vertex degree");
    gtk_widget_show(label);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 1, 2,
            GTK_FILL, GTK_FILL, 4, 4);

    adj = gtk_adjustment_new(2, 0, 65535, 1, 10, 10);
    iface.graph_dialog_regular2_spin_button =
            gtk_spin_button_new(GTK_ADJUSTMENT(adj), 0, 0);
    gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(
            iface.graph_dialog_regular2_spin_button), TRUE);
    gtk_widget_show(iface.graph_dialog_regular2_spin_button);
    gtk_table_attach(GTK_TABLE(table),
        iface.graph_dialog_regular2_spin_button, 1, 2, 1, 2,
        GTK_FILL | GTK_EXPAND, GTK_FILL, 4, 4);
    
    label = gtk_label_new("Regular graph");
    gtk_notebook_append_page(GTK_NOTEBOOK(
            iface.graph_dialog_notebook), table, label);

    /* Star graph */
    table = gtk_table_new(1, 3, FALSE);
    gtk_container_set_border_width(GTK_CONTAINER(table), 10);
    gtk_widget_set_usize(table, 500, 100);
    gtk_widget_show(table);
   
    label = gtk_label_new("Number of vertices");
    gtk_widget_show(label);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 0, 1,
            GTK_FILL, GTK_FILL, 4, 4);

    adj = gtk_adjustment_new(5, 1, 65535, 1, 10, 10);
    iface.graph_dialog_star_spin_button =
            gtk_spin_button_new(GTK_ADJUSTMENT(adj), 0, 0);
    gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(
            iface.graph_dialog_star_spin_button), TRUE);
    gtk_widget_show(iface.graph_dialog_star_spin_button);
    gtk_table_attach(GTK_TABLE(table),
        iface.graph_dialog_star_spin_button, 1, 2, 0, 1,
        GTK_FILL | GTK_EXPAND, GTK_FILL, 4, 4);

    label = gtk_label_new("Star graph");
    gtk_notebook_append_page(GTK_NOTEBOOK(
            iface.graph_dialog_notebook), table, label);

    /* Mitzenmacher random graph */
    table = gtk_table_new(3, 3, FALSE);
    gtk_container_set_border_width(GTK_CONTAINER(table), 10);
    gtk_widget_set_usize(table, 500, 100);
    gtk_widget_show(table);
   
    label = gtk_label_new("Number of vertices");
    gtk_widget_show(label);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 0, 1,
            GTK_FILL, GTK_FILL, 4, 4);

    adj = gtk_adjustment_new(3, 1, 65535, 1, 10, 10);
    iface.graph_dialog_mitzenmacher1_spin_button =
            gtk_spin_button_new(GTK_ADJUSTMENT(adj), 0, 0);
    gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(
            iface.graph_dialog_mitzenmacher1_spin_button), TRUE);
    gtk_widget_show(iface.graph_dialog_mitzenmacher1_spin_button);
    gtk_table_attach(GTK_TABLE(table),
        iface.graph_dialog_mitzenmacher1_spin_button, 1, 2, 0, 1,
        GTK_FILL | GTK_EXPAND, GTK_FILL, 4, 4);

    label = gtk_label_new("Edge probability");
    gtk_widget_show(label);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 1, 2,
            GTK_FILL, GTK_FILL, 4, 4);

    adj = gtk_adjustment_new(0.5, 0, 1, 0.1, 0.1, 0.1);
    iface.graph_dialog_mitzenmacher2_spin_button =
            gtk_spin_button_new(GTK_ADJUSTMENT(adj), 0, 4);
    gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(
            iface.graph_dialog_mitzenmacher2_spin_button), TRUE);
    gtk_widget_show(iface.graph_dialog_mitzenmacher2_spin_button);
    gtk_table_attach(GTK_TABLE(table),
        iface.graph_dialog_mitzenmacher2_spin_button, 1, 2, 1, 2,
        GTK_FILL | GTK_EXPAND, GTK_FILL, 4, 4);
    
    label = gtk_label_new("Mitzenmacher random graph");
    gtk_notebook_append_page(GTK_NOTEBOOK(
            iface.graph_dialog_notebook), table, label);
   
    /* Bottom of the graph dialog */
    action_area = GTK_DIALOG(iface.graph_dialog)->action_area;
    gtk_widget_show(action_area);
    gtk_container_set_border_width(GTK_CONTAINER(action_area), 10);

    ok_button = gtk_button_new_with_label("  OK  ");
    gtk_widget_show(ok_button);
    gtk_box_pack_start(GTK_BOX(action_area), ok_button, FALSE, FALSE, 0);
    GTK_WIDGET_SET_FLAGS(ok_button, GTK_CAN_DEFAULT);
    gtk_signal_connect(GTK_OBJECT(ok_button), "clicked",
                       GTK_SIGNAL_FUNC(on_graph_ok_clicked),
                       NULL);
    gtk_widget_grab_default(ok_button);
    
    cancel_button = gtk_button_new_with_label(" Cancel ");
    gtk_widget_show(cancel_button);
    gtk_box_pack_start(GTK_BOX(action_area), cancel_button,
            FALSE, FALSE, 0);
    GTK_WIDGET_SET_FLAGS(cancel_button, GTK_CAN_DEFAULT);
    gtk_signal_connect(GTK_OBJECT(cancel_button), "clicked",
                       GTK_SIGNAL_FUNC(factory_hide_toplevel_dialog),
                       NULL);
}


void create_vprop_dialog(void){
GtkWidget *vbox, *table, *label;
GtkWidget *action_area, *ok_button, *cancel_button;
GtkObject *adj;

    /* Creates the vertex properties dialog */
    iface.vprop_dialog = gtk_dialog_new();
    gtk_window_set_title(GTK_WINDOW(iface.vprop_dialog),
            "Vertex properties");
    gtk_window_set_position(GTK_WINDOW(iface.vprop_dialog),
            GTK_WIN_POS_MOUSE);
    gtk_window_set_policy(GTK_WINDOW(iface.vprop_dialog),
            TRUE, TRUE, FALSE);

    vbox = GTK_DIALOG(iface.vprop_dialog)->vbox;
    gtk_widget_show(vbox);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 2);
    
    /* Entries */
    table = gtk_table_new(4, 2, FALSE);
    gtk_container_set_border_width(GTK_CONTAINER(table), 10);
    gtk_widget_set_usize(table, 240, 130);
    gtk_widget_show(table);
    gtk_box_pack_start(GTK_BOX(vbox), table, TRUE, TRUE, 10);
   
    label = gtk_label_new("Id");
    gtk_widget_show(label);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 0, 1,
            GTK_FILL, GTK_FILL, 4, 4);

    iface.vprop_dialog_vertex_label = gtk_label_new("0");
    gtk_widget_show(iface.vprop_dialog_vertex_label);
    gtk_table_attach(GTK_TABLE(table), iface.vprop_dialog_vertex_label,
            1, 2, 0, 1, GTK_FILL, GTK_FILL, 4, 4);

    label = gtk_label_new("Label");
    gtk_widget_show(label);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 1, 2,
            GTK_FILL, GTK_FILL, 4, 4);

    iface.vprop_dialog_label_text = gtk_entry_new();
    gtk_widget_show(iface.vprop_dialog_label_text);
    gtk_table_attach(GTK_TABLE(table), iface.vprop_dialog_label_text,
            1, 2, 1, 2, GTK_FILL, GTK_FILL, 4, 4);
    
    label = gtk_label_new("Weight");
    gtk_widget_show(label);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 2, 3,
            GTK_FILL, GTK_FILL, 4, 4);

    adj = gtk_adjustment_new(1, 0, 1, 0.01, 0.1, 0.1);
    iface.vprop_dialog_weight_spin_button =
            gtk_spin_button_new(GTK_ADJUSTMENT(adj), 0, 2);
    gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(
            iface.vprop_dialog_weight_spin_button), TRUE);
    gtk_widget_show(iface.vprop_dialog_weight_spin_button);
    gtk_table_attach(GTK_TABLE(table),
        iface.vprop_dialog_weight_spin_button, 1, 2, 2, 3,
        GTK_FILL | GTK_EXPAND, GTK_FILL, 4, 4);
    
    label = gtk_label_new("Color");
    gtk_widget_show(label);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 3, 4,
            GTK_FILL, GTK_FILL, 4, 4);

    adj = gtk_adjustment_new(1, 0, 652, 1, 10, 10);
    iface.vprop_dialog_color_spin_button =
            gtk_spin_button_new(GTK_ADJUSTMENT(adj), 0, 0);
    gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(
            iface.vprop_dialog_color_spin_button), TRUE);
    gtk_widget_show(iface.vprop_dialog_color_spin_button);
    gtk_table_attach(GTK_TABLE(table),
        iface.vprop_dialog_color_spin_button, 1, 2, 3, 4,
        GTK_FILL | GTK_EXPAND, GTK_FILL, 4, 4);
  
    /* Bottom of the vprop dialog */
    action_area = GTK_DIALOG(iface.vprop_dialog)->action_area;
    gtk_widget_show(action_area);
    gtk_container_set_border_width(GTK_CONTAINER(action_area), 10);

    ok_button = gtk_button_new_with_label("  OK  ");
    gtk_widget_show(ok_button);
    gtk_box_pack_start(GTK_BOX(action_area), ok_button, FALSE, FALSE, 0);
    GTK_WIDGET_SET_FLAGS(ok_button, GTK_CAN_DEFAULT);
    gtk_signal_connect(GTK_OBJECT(ok_button), "clicked",
                       GTK_SIGNAL_FUNC(on_vprop_ok_clicked),
                       NULL);
    gtk_widget_grab_default(ok_button);
    
    cancel_button = gtk_button_new_with_label(" Cancel ");
    gtk_widget_show(cancel_button);
    gtk_box_pack_start(GTK_BOX(action_area), cancel_button,
            FALSE, FALSE, 0);
    GTK_WIDGET_SET_FLAGS(cancel_button, GTK_CAN_DEFAULT);
    gtk_signal_connect(GTK_OBJECT(cancel_button), "clicked",
                       GTK_SIGNAL_FUNC(factory_hide_toplevel_dialog),
                       NULL);
}

    
void create_eprop_dialog(void){
GtkWidget *vbox, *table, *label;
GtkWidget *action_area, *ok_button, *cancel_button;
GtkObject *adj;

    /* Creates the vertex properties dialog */
    iface.eprop_dialog = gtk_dialog_new();
    gtk_window_set_title(GTK_WINDOW(iface.eprop_dialog),
            "Edge properties");
    gtk_window_set_position(GTK_WINDOW(iface.eprop_dialog),
            GTK_WIN_POS_MOUSE);
    gtk_window_set_policy(GTK_WINDOW(iface.eprop_dialog),
            TRUE, TRUE, FALSE);

    vbox = GTK_DIALOG(iface.eprop_dialog)->vbox;
    gtk_widget_show(vbox);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 2);
    
    /* Complete graph */
    table = gtk_table_new(5, 2, FALSE);
    gtk_container_set_border_width(GTK_CONTAINER(table), 10);
    gtk_widget_set_usize(table, 240, 130);
    gtk_widget_show(table);
    gtk_container_add(GTK_CONTAINER(vbox), table);
   
    label = gtk_label_new("Id");
    gtk_widget_show(label);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 0, 1,
            GTK_FILL, GTK_FILL, 4, 4);

    iface.eprop_dialog_edge_label = gtk_label_new("0, 0");
    gtk_widget_show(iface.eprop_dialog_edge_label);
    gtk_table_attach(GTK_TABLE(table), iface.eprop_dialog_edge_label,
            1, 2, 0, 1, GTK_FILL, GTK_FILL, 4, 4);

    label = gtk_label_new("Label");
    gtk_widget_show(label);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 1, 2,
            GTK_FILL, GTK_FILL, 4, 4);

    iface.eprop_dialog_label_text = gtk_entry_new();
    gtk_widget_show(iface.eprop_dialog_label_text);
    gtk_table_attach(GTK_TABLE(table), iface.eprop_dialog_label_text,
            1, 2, 1, 2, GTK_FILL, GTK_FILL, 4, 4);
    
    label = gtk_label_new("Weight");
    gtk_widget_show(label);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 2, 3,
            GTK_FILL, GTK_FILL, 4, 4);

    adj = gtk_adjustment_new(1, 0, 1, 0.01, 0.1, 0.1);
    iface.eprop_dialog_weight_spin_button =
            gtk_spin_button_new(GTK_ADJUSTMENT(adj), 0, 2);
    gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(
            iface.eprop_dialog_weight_spin_button), TRUE);
    gtk_widget_show(iface.eprop_dialog_weight_spin_button);
    gtk_table_attach(GTK_TABLE(table),
        iface.eprop_dialog_weight_spin_button, 1, 2, 2, 3,
        GTK_FILL | GTK_EXPAND, GTK_FILL, 4, 4);

    label = gtk_label_new("Color");
    gtk_widget_show(label);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 3, 4,
            GTK_FILL, GTK_FILL, 4, 4);

    adj = gtk_adjustment_new(1, 0, 652, 1, 10, 10);
    iface.eprop_dialog_color_spin_button =
            gtk_spin_button_new(GTK_ADJUSTMENT(adj), 0, 0);
    gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(
            iface.eprop_dialog_color_spin_button), TRUE);
    gtk_widget_show(iface.eprop_dialog_color_spin_button);
    gtk_table_attach(GTK_TABLE(table),
        iface.eprop_dialog_color_spin_button, 1, 2, 3, 4,
        GTK_FILL | GTK_EXPAND, GTK_FILL, 4, 4);

    /* Bottm of the eprop dialog */
    action_area = GTK_DIALOG(iface.eprop_dialog)->action_area;
    gtk_widget_show(action_area);
    gtk_container_set_border_width(GTK_CONTAINER(action_area), 10);

    ok_button = gtk_button_new_with_label("  OK  ");
    gtk_widget_show(ok_button);
    gtk_box_pack_start(GTK_BOX(action_area), ok_button, FALSE, FALSE, 0);
    GTK_WIDGET_SET_FLAGS(ok_button, GTK_CAN_DEFAULT);
    gtk_signal_connect(GTK_OBJECT(ok_button), "clicked",
                       GTK_SIGNAL_FUNC(on_eprop_ok_clicked),
                       NULL);
    gtk_widget_grab_default(ok_button);
    
    cancel_button = gtk_button_new_with_label(" Cancel ");
    gtk_widget_show(cancel_button);
    gtk_box_pack_start(GTK_BOX(action_area), cancel_button,
            FALSE, FALSE, 0);
    GTK_WIDGET_SET_FLAGS(cancel_button, GTK_CAN_DEFAULT);
    gtk_signal_connect(GTK_OBJECT(cancel_button), "clicked",
                       GTK_SIGNAL_FUNC(factory_hide_toplevel_dialog),
                       NULL);
}


/* Returns an allocated pointer with value i */
int *getintp(int i){
int *p;

    p = (int *) malloc(sizeof(int));
    (*p) = i;
    return p;
}


/* Return widget with name from main menu factory list */
GtkWidget *get_main_menu_item_by_name(char *name){
int i;

    for (i = 0; main_menu[i].type != FACTORY_TYPE_END; i++){
        if (main_menu[i].name != NULL){
            if (!strcmp(main_menu[i].name, name))
                return main_menu[i].widget;
        }
    }

    return NULL;
}


/* Adds algorithm menu item */
void add_menu_item(GtkWidget *menu, char *s, int i, int type, int plugin){
GtkWidget *menu_item;
GtkWidget *hbox;
GtkWidget *label;
static GdkPixmap *pixmap;
static GdkBitmap *mask;
GtkWidget *pixmapwid;
GtkStyle *style;

    /* Create pixmap only on the first time */
    if (!pixmap){
        style = gtk_widget_get_style(iface.window);
        pixmap = gdk_pixmap_create_from_xpm_d(
                iface.window->window, &mask,
                &style->bg[GTK_STATE_NORMAL],
                (gchar **) plugin_xpm);
    }

    /* Create menu item */
    if (plugin){
        hbox = gtk_hbox_new(FALSE, 5);
        gtk_widget_show(hbox);
        label = gtk_accel_label_new(s);
        gtk_widget_show(label);
        if (pixmap){
            pixmapwid = gtk_pixmap_new(pixmap, mask);
            gtk_widget_show(pixmapwid);
            gtk_box_pack_start(GTK_BOX(hbox), pixmapwid, FALSE,
                    TRUE, 0);
        }
        gtk_box_pack_start(GTK_BOX(hbox), label, FALSE,
                TRUE, 0);
        menu_item = gtk_menu_item_new();
        gtk_accel_label_set_accel_widget(GTK_ACCEL_LABEL(label),
                menu_item);
        gtk_container_add(GTK_CONTAINER(menu_item), hbox);
    } else
        menu_item = gtk_menu_item_new_with_label(s);
    gtk_widget_show(menu_item);
    gtk_container_add(GTK_CONTAINER(menu), menu_item);
    if (type == 0)
        gtk_signal_connect(GTK_OBJECT(menu_item), "activate",
                GTK_SIGNAL_FUNC(on_test_algorithm), getintp(i));
    else if (type == 1)
        gtk_signal_connect(GTK_OBJECT(menu_item), "activate",
                GTK_SIGNAL_FUNC(on_result_algorithm), getintp(i));
    else if (type == 2){
        gtk_signal_connect(GTK_OBJECT(menu_item), "activate",
                GTK_SIGNAL_FUNC(on_step_algorithm), getintp(i));
		gtk_widget_set_sensitive(GTK_WIDGET(menu_item), getintp(i));
    } else if (type == 3)
        gtk_signal_connect(GTK_OBJECT(menu_item), "activate",
                GTK_SIGNAL_FUNC(on_transform_algorithm), getintp(i));
    else if (type == 4)
        gtk_signal_connect(GTK_OBJECT(menu_item), "activate",
                GTK_SIGNAL_FUNC(on_search_algorithm), getintp(i));

}


/* Inits algorithms menu */
void init_algorithms(void){
int i;
Algor *node;
AlgorR *noder;
AlgorS *nodes;
GtkWidget *menu;

    /* Menu */ 
    
    /* "Test Algorithms" submenus */
    if (L.size > 0) {
        menu = get_main_menu_item_by_name("test");
        menu = GTK_MENU_ITEM(menu)->submenu;
        node = L.first;
        i = 1;
        while (node) {
            add_menu_item(menu, node->string, i++, 0, node->plugin);
            node = node->next;
        }
    }
    /* End: "Test Algorithms" submenus */

    /* "Result Algorithms" submenus */
    if (RL.size > 0) {
        menu = get_main_menu_item_by_name("result");
        menu = GTK_MENU_ITEM(menu)->submenu;
        noder = RL.first;
        i = 1;
        while (noder) {
            add_menu_item(menu, noder->string, i++, 1, noder->plugin);
            noder = noder->next;
        }
    }
    /* End: "Result Algorithms" submenus */
      
    /* "Step Algorithms" submenus */
    if (SSL.size > 0) {

        menu = get_main_menu_item_by_name("step");
        menu = GTK_MENU_ITEM(menu)->submenu;
        nodes = SSL.first;
        i = 1;
        while (nodes) {
            add_menu_item(menu, nodes->string, i++, 2, nodes->plugin);
            nodes = nodes->next;
        }
    }
    /* End: "Step Algorithms" submenus */
    
    /* "Transformation Algorithms" submenus */
    if (TL.size > 0) {
        menu = get_main_menu_item_by_name("transforms");
        menu = GTK_MENU_ITEM(menu)->submenu;
        node = TL.first;
        i = 1;
        while (node) {
            add_menu_item(menu, node->string, i++, 3, node->plugin);
            node = node->next;
        }
    }
    /* End: "Transformation Algorithms" submenus */
    
    /* "Search Algorithms" submenus */
    if (SL.size > 0) {

        menu = get_main_menu_item_by_name("search");
        menu = GTK_MENU_ITEM(menu)->submenu;
        nodes = SL.first;
        i = 1;
        while (nodes) {
            add_menu_item(menu, nodes->string, i++, 4, nodes->plugin);
            nodes = nodes->next;
        }
    }
    /* End: "Search Algorithms" submenus */

	
}


/* Parse toolbar items string */
char ***parse_toolbar_items(char *s, int *count){
int i, j, k, l, len, size;
char ***items;

    /* Count number of items */
    len = strlen(s);
    (*count) = 0;
    for (i = 0; i < len; i++){
        if (s[i] == ':')
            (*count)++;
    }
    (*count)++;

    items = (char ***) malloc((*count) * sizeof(char **));
    for (i = 0; i < (*count); i++)
        items[i] = (char **) malloc(4 * sizeof(char *));
    
    /* Count items size */
    size = j = k = 0;
    for (i = 0; i < len; i++){
        if (s[i] == ':') {
            items[j][k] = (char *) malloc(size+1);
            if (k < 2){
                if (k < 1)
                    items[j][1] = NULL;
                items[j][2] = NULL;
            }
            j++;
            k = 0;
            size = 0;
        } else if (s[i] == ','){
            items[j][k] = (char *) malloc(size+1);
            k++;
            size = 0;
        } else 
            size++;
    }

    items[j][k] = (char *) malloc(size+1);
    if (k < 2){
        if (k < 1)
            items[j][1] = NULL;
        items[j][2] = NULL;
    }

    /* Get items */
    j = k = l = 0;
    for (i = 0; i < len; i++){
        if (s[i] == ':'){
            j++;
            k = 0;
            l = 0;
        } else if (s[i] == ','){
            k++;
            l = 0;
        } else {
            items[j][k][l] = s[i];
            items[j][k][l+1] = 0;
            l++;
        }
    }

    /* Get items type */
    for (i = 0; i < (*count); i++){
        if (!strncmp(items[i][0], "alg(", 4)){
            items[i][0] = items[i][0] + 4;
            items[i][0][strlen(items[i][0])-1] = 0;
            items[i][3] = (char *) malloc(1);
            items[i][3][0] = 1;
        } else {
            items[i][3] = (char *) malloc(1);
            items[i][3][0] = 0;
        }
    }

    return items;
}


/* Find algorithm label */
char *find_alg_label(char *name){
Algor *node;
AlgorR *noder;
AlgorS *nodes;

    if (L.size > 0) {
        node = L.first;
        while (node) {
            if (!strcmp(node->string, name))
                return node->string;
            node = node->next;
        }
    }

    if (RL.size > 0) {
        noder = RL.first;
        while (noder) {
            if (!strcmp(noder->string, name))
                return noder->string;
            noder = noder->next;
        }
    }
      
    if (TL.size > 0) {
        node = TL.first;
        while (node) {
            if (!strcmp(node->string, name))
                return node->string;
            node = node->next;
        }
    }

    if (SL.size > 0) {
        nodes = SL.first;
        while (node) {
            if (!strcmp(nodes->string, name))
                return nodes->string;
            nodes = nodes->next;
        }
    }

    if (SSL.size > 0) {
        nodes = SSL.first;
        while (node) {
            if (!strcmp(nodes->string, name))
                return nodes->string;
            nodes = nodes->next;
        }
    }
	
    return NULL;
}

/* Init toolbar */
void init_toolbar(void){
int count;
char ***items;
factory_item *toolbar;
int i, j, found;
char *s;

    /* Destroy previous toolbar */
    if (iface.toolbar){
        gtk_widget_destroy(iface.toolbar);
    }

    /* Default toolbar */
    if (!config->toolbar_items){
        iface.toolbar = factory_toolbar(iface.window,
                main_toolbar, iface.window_accels);
        gtk_box_pack_start(GTK_BOX(iface.vbox),
                iface.toolbar, FALSE, TRUE, 0);
    }
    
    /* Configured toolbar */
    else {
        items = parse_toolbar_items(config->toolbar_items, &count);
        toolbar = (factory_item *) malloc((count+1) * sizeof(factory_item));
        for (i = 0; i < count; i++){
            if (!strcmp(items[i][0],"sep")){
                toolbar[i].type = FACTORY_TYPE_SEP;
            } else {
                toolbar[i].type = FACTORY_TYPE_BUTTON;
                toolbar[i].accel_key = 0;
                toolbar[i].accel_mod = 0;
                toolbar[i].sensitive = TRUE;
                toolbar[i].group = 0;
                toolbar[i].state = 0;
                toolbar[i].func_data = NULL;
                toolbar[i].func_name = NULL;
                toolbar[i].pixmap = FACTORY_PIXMAP_DATA;
                toolbar[i].pixmap_name = (char *) k3_xpm;
                toolbar[i].widget = NULL;
                toolbar[i].dwidget = NULL;
                if (items[i][3][0] == 1){
                    toolbar[i].name = items[i][0];
                    toolbar[i].label = find_alg_label(items[i][0]);
                    toolbar[i].tooltip = "Run algorithm";
                    toolbar[i].callback = (GtkSignalFunc) on_run_algorithm;
                    toolbar[i].func_data = items[i][0];
                } else if (items[i][3][0] == 0){
                    j = found = 0;
                    s = stock_items[j].name;
                    while ((s != NULL) && (!found)){
                        if (!strcmp(items[i][0], s)){
                            toolbar[i].name = stock_items[j].name;
                            toolbar[i].label = stock_items[j].label;
                            toolbar[i].tooltip = stock_items[j].tooltip;
                            toolbar[i].callback = stock_items[j].callback;
                                if (stock_items[j].pixmap != NULL){
                                    toolbar[i].pixmap = FACTORY_PIXMAP_DATA;
                                    toolbar[i].pixmap_name = (char *)
                                        stock_items[j].pixmap;
                                }
                            found = 1;
                        } 
                        j++;
                        s = stock_items[j].name;
                    }
                }
                if (items[i][1] != NULL){
                    toolbar[i].label = items[i][1];
                }
                if (items[i][2] != NULL){
                    toolbar[i].pixmap = FACTORY_PIXMAP_FILE;
                    toolbar[i].pixmap_name = items[i][2];
                }
            }
        }
        toolbar[count].type = FACTORY_TYPE_END;
        iface.toolbar = factory_toolbar(iface.window,
                toolbar, iface.window_accels);
        gtk_box_pack_start(GTK_BOX(iface.vbox),
                iface.toolbar, FALSE, TRUE, 0);
    }
}

/* process iface events pending */
void iface_refresh(void) {

    /* Redraws screen */
    if (iface.view_mode == VIEW_MODE_GTK){
        iface.changed = 1;
        gtk_widget_queue_draw(iface.drawingarea);
    }
#ifdef USE_GTKGL
    else if (iface.view_mode == VIEW_MODE_OPENGL){
        iface.changed = 1;
        gtk_widget_queue_draw(iface.glarea);
    }
#endif
	usleep(400000);
	
    while (gtk_events_pending())
            gtk_main_iteration();
}

void IfaceLoop(int *argc, char ***argv) {

    /* Gtk init */
    gtk_init(argc, argv);

    /* Creates main window and all dialogs */
    create_main_window();
    init_algorithms();

    /* Opens file parameter */
    if ((*argc) > 1){
        iface.filename = (*argv)[1];
        iface.format = 0;
        gtk_file_selection_set_filename(GTK_FILE_SELECTION(
                iface.open_filesel), iface.filename),
        open_file();
    }

    /* Main loop */
    gtk_main();
}


/* ******* */
/* * End * */
/* ******* */

