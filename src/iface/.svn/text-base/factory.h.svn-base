/*
 *  factory - gtk menu factory (another one)
 *  Copyright (c) 2003, 2004
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


/* ************* */
/* * factory.h * */
/* ************* */

#ifndef __FACTORY_H__
#define __FACTORY_H__


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#ifndef __GTK_H__
#include <gtk.h>
#endif


/* Item types: menus or toolbars */
#define FACTORY_TYPE_NONE    0  /* Nothing */
#define FACTORY_TYPE_END     1  /* Items list end */
#define FACTORY_TYPE_MENU    2  /* Item with related menu */
#define FACTORY_TYPE_ITEM    3  /* Menu item */
#define FACTORY_TYPE_SEP     4  /* Separator */
#define FACTORY_TYPE_CHECK   5  /* Check (toggle) button */
#define FACTORY_TYPE_RADIO   6  /* Radio button */
#define FACTORY_TYPE_RET     7  /* One level menu return */
#define FACTORY_TYPE_BUTTON  8  /* Toolbar button */

/* Pixmap types */
#define FACTORY_PIXMAP_NONE  0
#define FACTORY_PIXMAP_DATA  1
#define FACTORY_PIXMAP_FILE  2

/* Maximum branch levels (for menus) */
#define FACTORY_MAX_LEVELS  10


/* Factory item: menu or toolbar item */
typedef struct factory_item {
    int type;                   /* Item type */
    char *name;                 /* Item unique string id */
    char *label;                /* Item text label */
    char *tooltip;              /* Tooltip text */
    guint accel_key;            /* Accel key */
    GdkModifierType accel_mod;  /* Accel key modificator */
    gboolean sensitive;         /* If sensitive (active) */
    int group;                  /* Group index: for radios */
    gboolean state;             /* State: for toggles */
    GtkSignalFunc callback;     /* Item callback */
    gpointer func_data;         /* Callback data */
    char *func_name;            /* Another string id for the item */
    int pixmap;                 /* Pixmap type */
    char *pixmap_name;          /* Pixmap file name or data */
    GtkWidget *widget;          /* Item widget */
    GtkWidget **dwidget;        /* Destiny user's widget variable */
} factory_item;


GtkWidget *factory_menubar(GtkWidget *window, factory_item *item,
        GtkAccelGroup *window_accels, GtkTooltips *tooltips);
GtkWidget *factory_toolbar(GtkWidget *window, factory_item *item,
        GtkAccelGroup *window_accels);
GtkWidget *factory_menu(GtkWidget *window, factory_item *item,
        GtkAccelGroup *window_accels, GtkTooltips *tooltips);

GtkWidget *factory_msg_dialog(char *title, char *text,
        GtkSignalFunc ok_callback);
GtkWidget *factory_confirm_dialog(char *title, char *text,
        GtkSignalFunc yes_callback, GtkSignalFunc no_callback);
GtkWidget *factory_filesel_dialog(char *title,
        GtkSignalFunc ok_callback, GtkSignalFunc cancel_callback);

void factory_show_msg_dialog(GtkWidget *dialog, char *text);
void factory_show_fmsg_dialog(GtkWidget *dialog, char *format,...);
void factory_show_confirm_dialog(GtkWidget *dialog, char *text);
void factory_show_dialog(GtkWidget *dialog);
void factory_hide_dialog(GtkWidget *dialog);
void factory_hide_toplevel_dialog(GtkWidget *widget);

void factory_hide_dialog_callback(GtkWidget *widget,
        gpointer user_data);


/* Default items */
/* Items list end */
#define FACTORY_END \
{ FACTORY_TYPE_END, NULL, NULL, NULL,\
  0, 0, 0, 0, 0, NULL, NULL, NULL,\
  0, NULL, NULL, NULL },
/* Separator */
#define FACTORY_SEP \
{ FACTORY_TYPE_SEP, NULL, NULL, NULL,\
  0, 0, 0, 0, 0, NULL, NULL, NULL,\
  0, NULL, NULL, NULL },
/* One level menu return */
#define FACTORY_RET \
{ FACTORY_TYPE_RET, NULL, NULL, NULL,\
  0, 0, 0, 0, 0, NULL, NULL, NULL,\
  0, NULL, NULL, NULL },
/* Simple menu */
#define FACTORY_MENU(name, label, tooltip) \
{ FACTORY_TYPE_MENU, name, label, tooltip, \
  0, 0, TRUE, 0, 0, NULL, NULL, NULL,\
  FACTORY_PIXMAP_NONE, NULL, NULL, NULL },

  
#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __FACTORY_H__ */

/* ******* */
/* * End * */
/* ******* */
