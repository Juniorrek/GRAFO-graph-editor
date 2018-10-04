/*
 *  grafo - graph editor
 *  Copyright (c) 2003-2008
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


/* *************** */
/* * callbacks.h * */
/* *************** */

#ifndef __CALLBACKS_H_
#define __CALLBACKS_H_


#ifdef USE_GTKGL
gboolean init_glarea(GtkWidget *widget);
#endif
gboolean init_drawingarea(GtkWidget *widget);
gboolean expose_event_callback(GtkWidget *widget,
        GdkEventExpose *event, gpointer user_data);
gboolean resize_event_callback(GtkWidget *widget,
        GdkEventConfigure *event, gpointer user_data);
gboolean button_press_event_callback(GtkWidget *widget,
        GdkEventButton *event, gpointer user_data);
gboolean motion_notify_event_callback(GtkWidget *widget,
        GdkEventMotion *event, gpointer user_data);
gboolean enter_notify_event_callback(GtkWidget *widget,
        GdkEventMotion *event, gpointer user_data);
gboolean leave_notify_event_callback(GtkWidget *widget,
        GdkEventMotion *event, gpointer user_data);

#ifdef USE_GTKGL
GLuint Pick(int x, int y);
void SelectControlPoints(void);
void ChooseGlColor(Graph *g, int i);
void ChooseGlEdgeColor(Graph *g, int i, int j);
#endif
int get_point(int x, int y);
void change_color(Graph *g, int i, GdkGC *gc);
void change_edge_color(Graph *g, int i, int j, GdkGC *gc);
//double hornbez(int degree, int *coeff, double t);
void gdk_draw_spline(GdkWindow *window, GdkGC *gc, int degree,
        int *ctrl_x, int *ctrl_y);

void on_edit_mode_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_reset_graph_item_activate(GtkMenuItem *menuitem,
        gpointer user_data);
void on_reset_marked_edges_item_activate(GtkMenuItem *menuitem,
        gpointer user_data);
void on_reset_marked_vertices_item_activate(GtkMenuItem *menuitem,
        gpointer user_data);
void on_complete_graph_item_activate(GtkMenuItem *menuitem,
        gpointer user_data);
void on_bipartite_graph_item_activate(GtkMenuItem *menuitem,
        gpointer user_data);
void on_random_graph_item_activate(GtkMenuItem *menuitem,
        gpointer user_data);
void on_regular_graph_item_activate(GtkMenuItem *menuitem,
        gpointer user_data);
void on_star_graph_item_activate(GtkMenuItem *menuitem,
        gpointer user_data);
void on_mitzenmacher_random_graph_item_activate(GtkMenuItem *menuitem,
        gpointer user_data);
void on_fit_to_window_item_activate(GtkMenuItem *menuitem,
        gpointer user_data);
void on_draw_as_circle_item_activate(GtkMenuItem *menuitem,
        gpointer user_data);
void on_draw_using_dot_item_activate(GtkMenuItem *menuitem,
        gpointer user_data);
void on_draw_using_neato_item_activate(GtkMenuItem *menuitem,
        gpointer user_data);
void on_toggle_graph_item_activate(GtkMenuItem *menuitem,
        gpointer user_data);
void on_show_ctrlpts_item_activate(GtkMenuItem *menuitem,
		gpointer user_data);
void on_test_algorithm(GtkMenuItem *menuitem, gpointer user_data);
void on_result_algorithm(GtkMenuItem *menuitem, gpointer user_data);
void on_step_algorithm(GtkMenuItem *menuitem, gpointer user_data);
void on_transform_algorithm(GtkMenuItem *menuitem, gpointer user_data);
void on_search_algorithm(GtkMenuItem *menuitem, gpointer user_data);

void on_new_item_activate(GtkWidget *widget, gpointer user_data);
void on_open_item_activate(GtkWidget *widget, gpointer user_data);
void on_save_item_activate(GtkWidget *widget, gpointer user_data);
void on_saveas_item_activate(GtkWidget *widget, gpointer user_data);
void on_firststep_item_activate(GtkWidget *widget, gpointer user_data);
void on_previousstep_item_activate(GtkWidget *widget, gpointer user_data);
void execstep(void);
void on_execstep_item_activate(GtkWidget *widget, gpointer user_data);
void on_nextstep_item_activate(GtkWidget *widget, gpointer user_data);
void on_laststep_item_activate(GtkWidget *widget, gpointer user_data);
/*void on_close_item_activate(GtkWidget *widget, gpointer user_data);*/
void on_about_item_activate(GtkWidget *widget, gpointer user_data);
void on_preferences_item_activate(GtkWidget *widget, gpointer user_data);
void on_plugins_item_activate(GtkWidget *widget, gpointer user_data);
void on_view_mode_item_activate(GtkWidget *widget, gpointer user_data);
    
void on_open_filesel_ok(GtkWidget *widget, gpointer user_data);
void on_save_filesel_ok(GtkWidget *widget, gpointer user_data);
void on_conf_yes(GtkWidget *widget, gpointer user_data);

void on_preferences_ok_clicked(GtkWidget *widget, gpointer user_data);
void on_preferences_cancel_clicked(GtkWidget *widget, gpointer user_data);
void on_choose_neato_button_clicked(GtkWidget *widget, gpointer user_data);
void on_neato_filesel_ok(GtkWidget *widget, gpointer user_data);
void on_choose_dot_button_clicked(GtkWidget *widget, gpointer user_data);
void on_dot_filesel_ok(GtkWidget *widget, gpointer user_data);
void on_choose_plugins_button_clicked(GtkWidget *widget,
        gpointer user_data);
void on_plugins_filesel_ok(GtkWidget *widget, gpointer user_data);
void on_choose_datadir_button_clicked(GtkWidget *widget,
        gpointer user_data);
void on_datadir_filesel_ok(GtkWidget *widget, gpointer user_data);

void on_graph_ok_clicked(GtkWidget *widget, gpointer user_data);
void on_vprop_ok_clicked(GtkWidget *widget, gpointer user_data);
void on_eprop_ok_clicked(GtkWidget *widget, gpointer user_data);

void load_vertex_properties(void);
void save_vertex_properties(void);
void load_edge_properties(void);
void save_edge_properties(void);

void on_vlabel_mode_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_elabel_mode_activate(GtkMenuItem *menuitem, gpointer user_data);

void open_file(void);
void save_file(void);
void update_status(void);
void rescan_filesels(void);
void change_window_title(void);
void showmsg(char *format,...);

void on_run_algorithm(GtkWidget *widget, gpointer user_data);


#endif

/* ******* */
/* * End * */
/* ******* */

