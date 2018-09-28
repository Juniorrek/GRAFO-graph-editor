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
/* * factory.c * */
/* ************* */


#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <stdlib.h>
#include <stdio.h>

#include "factory.h"

/* If using gtk 2.0 */
#define GTK_2


/* Error printing */
#define factory_perror(string, ...)\
    printf("factory error: %s[%d]: %s(): "string"!\n",\
           __FILE__ , __LINE__ , __FUNCTION__ , ##__VA_ARGS__)

    
GtkWidget *factory_menubar(GtkWidget *window, factory_item *item,
        GtkAccelGroup *window_accels, GtkTooltips *tooltips){
int count;                                 /* Number of items */
int i;                                     /* Item loop */
int l;                                     /* Current level */
int groups;                                /* Groups count */
GtkWidget *handlebox;                      /* Menubar handlebox */
GtkWidget *menubar;                        /* Menubar */
GtkWidget *level[FACTORY_MAX_LEVELS];      /* Parents  */
#ifndef GTK_2
GtkAccelGroup *accel[FACTORY_MAX_LEVELS];  /* Perents' accels */
guint tmp_key;                             /* Temporary accel key */ 
GtkWidget *hbox;                           /* Temporary menu item hbox */
GtkWidget *label;                          /* Temporary menu item label */
#endif
GSList **group;                            /* Radio button groups */
GtkWidget *menu_item;                      /* Temporary menu item */  
GtkWidget *menu;                           /* Temporary menu */ 
GtkWidget *tearoff;                        /* Temporary tearoff */
GdkPixmap *pixmap;                         /* Temporary pixmap */
GdkBitmap *mask;                           /* Temporary pixmap mask */
GtkWidget *image;                          /* Temporary image
                                              or pixmap widget */
GtkStyle *style;                           /* Window style */

    (void) window;
    
    /* Count number of items */
    for (count = 0;
         item[count].type != FACTORY_TYPE_END;
         count++)
        ;

    /* Create menubar and menubar handlebox */
    handlebox = gtk_handle_box_new();
    gtk_widget_show(handlebox);
    menubar = gtk_menu_bar_new();
    gtk_widget_show(menubar);
    gtk_container_add(GTK_CONTAINER(handlebox), menubar);
    l = 0;
    level[l] = menubar;

    /* Create radio button groups */
    groups = 0;
    for (i = 0; i < count; i++){
        if (item[i].type == FACTORY_TYPE_RADIO)
            if (item[i].group > groups)
                groups = item[i].group;
    }
    groups++;
    group = (GSList **) malloc(groups * sizeof(GSList *));
    for (i = 0; i < groups; i++)
        group[i] = NULL;

    /* Create items */
    for (i = 0; i < count; i++){

        /* Pixmap */
        image = NULL;
        if ((item[i].pixmap != FACTORY_PIXMAP_NONE) &&
            (item[i].type != FACTORY_TYPE_NONE) &&
            (item[i].type != FACTORY_TYPE_END) &&
            (item[i].type < FACTORY_TYPE_SEP)){
            if (item[i].pixmap == FACTORY_PIXMAP_DATA){
                style = gtk_widget_get_style(window);
                pixmap = gdk_pixmap_create_from_xpm_d(
                        window->window, &mask,
                        &style->bg[GTK_STATE_NORMAL],
                        (gchar **) item[i].pixmap_name);
                if (pixmap){
#ifdef GTK_2
                    image = gtk_image_new_from_pixmap(pixmap, mask);
                    gtk_widget_show(image);
#else
                    image = gtk_pixmap_new(pixmap, mask);
                    gtk_widget_show(image);
#endif
                }
            } else if (item[i].pixmap == FACTORY_PIXMAP_FILE){
                style = gtk_widget_get_style(window);
                pixmap = gdk_pixmap_create_from_xpm(
                        window->window, &mask,
                        &style->bg[GTK_STATE_NORMAL],
                        (gchar *) item[i].pixmap_name);
                if (pixmap){
#ifdef GTK_2
                    image = gtk_image_new_from_pixmap(pixmap, mask);
                    gtk_widget_show(image);
#else
                    image = gtk_pixmap_new(pixmap, mask);
                    gtk_widget_show(image);
#endif
                }
            } else if (item[i].pixmap > FACTORY_PIXMAP_FILE){
            }
        }

        switch (item[i].type){
            case FACTORY_TYPE_NONE :
                break;
            case FACTORY_TYPE_MENU :
                if (image){
#ifdef GTK_2
                    menu_item = gtk_image_menu_item_new_with_mnemonic(
                            item[i].label);
                    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(
                            menu_item), image);
#else
                    hbox = gtk_hbox_new(FALSE, 0);
                    gtk_widget_show(hbox);
                    label = gtk_accel_label_new("");
                    tmp_key = gtk_label_parse_uline(GTK_LABEL(label),
                            item[i].label);
                    gtk_widget_show(label);
                    gtk_box_pack_start(GTK_BOX(hbox), image, FALSE,
                            TRUE, 0);
                    gtk_box_pack_start(GTK_BOX(hbox), label, TRUE,
                            TRUE, 0);
                    menu_item = gtk_menu_item_new();
                    gtk_accel_label_set_accel_widget(GTK_ACCEL_LABEL(label),
                            menu_item);
                    gtk_container_add(GTK_CONTAINER(menu_item), hbox);
                    if (l == 0)
                        gtk_widget_add_accelerator(menu_item,
                                "activate_item", window_accels,
                                tmp_key, GDK_MOD1_MASK, (GtkAccelFlags) 0);
                    else 
                        gtk_widget_add_accelerator(menu_item,
                                "activate_item", accel[l], tmp_key, 0, 0);
#endif
                } else {
#ifdef GTK_2
                    menu_item = gtk_menu_item_new_with_mnemonic(
                            item[i].label);
#else
                    menu_item = gtk_menu_item_new_with_label("");
                    tmp_key = gtk_label_parse_uline(GTK_LABEL(
                            GTK_BIN(menu_item)->child),
                            item[i].label);
                    if (l == 0)
                        gtk_widget_add_accelerator(menu_item,
                                "activate_item", window_accels,
                                tmp_key, GDK_MOD1_MASK, (GtkAccelFlags) 0);
                    else 
                        gtk_widget_add_accelerator(menu_item,
                                "activate_item", accel[l], tmp_key, 0, 0);
#endif
                }
                gtk_widget_show(menu_item);
                gtk_container_add(GTK_CONTAINER(level[l]), menu_item);
                if (tooltips)
                    gtk_tooltips_set_tip(tooltips, menu_item,
                            item[i].tooltip, NULL);
                menu = gtk_menu_new();
                gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), menu);
#ifndef GTK_2
                accel[l+1] =
                        gtk_menu_ensure_uline_accel_group(GTK_MENU(menu));
#endif
                tearoff = gtk_tearoff_menu_item_new();
                gtk_widget_show(tearoff);
                gtk_container_add(GTK_CONTAINER(menu), tearoff);
                if (item[i].accel_key > 0)
                    gtk_widget_add_accelerator(menu_item, "activate",
                            window_accels, item[i].accel_key,
                            item[i].accel_mod, GTK_ACCEL_VISIBLE);
                l++;
                level[l] = menu;
                item[i].widget = menu_item;
                if (item[i].dwidget != NULL)
                    (*item[i].dwidget) = menu_item;
                break;
            case FACTORY_TYPE_ITEM :
                if (image){
#ifdef GTK_2
                    menu_item = gtk_image_menu_item_new_with_mnemonic(
                            item[i].label);
                    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(
                            menu_item), image);
#else
                    hbox = gtk_hbox_new(FALSE, 0);
                    gtk_widget_show(hbox);
                    label = gtk_accel_label_new("");
                    tmp_key = gtk_label_parse_uline(GTK_LABEL(label),
                            item[i].label);
                    gtk_widget_show(label);
                    gtk_box_pack_start(GTK_BOX(hbox), image, FALSE,
                            TRUE, 0);
                    gtk_box_pack_start(GTK_BOX(hbox), label, TRUE,
                            TRUE, 0);
                    menu_item = gtk_menu_item_new();
                    gtk_accel_label_set_accel_widget(GTK_ACCEL_LABEL(label),
                            menu_item);
                    gtk_container_add(GTK_CONTAINER(menu_item), hbox);
                    gtk_widget_add_accelerator(menu_item, "activate_item",
                            accel[l], tmp_key, 0, 0);
#endif
                } else {
#ifdef GTK_2
                    menu_item = gtk_menu_item_new_with_mnemonic(
                            item[i].label);
#else
                    menu_item = gtk_menu_item_new_with_label("");
                    tmp_key = gtk_label_parse_uline(GTK_LABEL(
                            GTK_BIN(menu_item)->child), item[i].label);
                    gtk_widget_add_accelerator(menu_item, "activate_item",
                            accel[l], tmp_key, 0, 0);
#endif
                }
                gtk_widget_show(menu_item);
                if (tooltips)
                    gtk_tooltips_set_tip(tooltips, menu_item,
                            item[i].tooltip, NULL);
                gtk_container_add(GTK_CONTAINER(level[l]), menu_item);
                if (item[i].accel_key > 0)
                    gtk_widget_add_accelerator(menu_item, "activate",
                            window_accels, item[i].accel_key,
                            item[i].accel_mod, GTK_ACCEL_VISIBLE);
                if (item[i].callback != NULL)
                    gtk_signal_connect(GTK_OBJECT(menu_item), "activate",
                            GTK_SIGNAL_FUNC(item[i].callback),
                            item[i].func_data);
                if (item[i].func_name != NULL)
                    gtk_object_set_data(GTK_OBJECT(menu_item), "func_name",
                            item[i].func_name);
                gtk_widget_set_sensitive(GTK_WIDGET(menu_item),
                        item[i].sensitive);
                item[i].widget = menu_item;
                if (item[i].dwidget != NULL)
                    (*item[i].dwidget) = menu_item;
                break;
            case FACTORY_TYPE_SEP :
                menu_item = gtk_menu_item_new();
                gtk_widget_show(menu_item);
                gtk_container_add(GTK_CONTAINER(level[l]), menu_item);
                gtk_widget_set_sensitive(menu_item, FALSE);
                item[i].widget = menu_item;
                if (item[i].dwidget != NULL)
                    (*item[i].dwidget) = menu_item;
                break;
            case FACTORY_TYPE_CHECK :
#ifdef GTK_2
                menu_item = gtk_check_menu_item_new_with_mnemonic(
                        item[i].label);
#else
                menu_item = gtk_check_menu_item_new_with_label("");
                tmp_key = gtk_label_parse_uline(GTK_LABEL(
                        GTK_BIN(menu_item)->child), item[i].label);
                gtk_widget_add_accelerator(menu_item, "activate_item",
                        accel[l], tmp_key, 0, 0);
#endif
                gtk_widget_show(menu_item);
                if (tooltips)
                    gtk_tooltips_set_tip(tooltips, menu_item,
                            item[i].tooltip, NULL);
                gtk_container_add(GTK_CONTAINER(level[l]), menu_item);
                if (item[i].accel_key > 0)
                    gtk_widget_add_accelerator(menu_item, "activate",
                            window_accels, item[i].accel_key,
                            item[i].accel_mod, GTK_ACCEL_VISIBLE);
                gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(
                        menu_item), item[i].state);
                gtk_check_menu_item_set_show_toggle(GTK_CHECK_MENU_ITEM(
                        menu_item), TRUE);
                if (item[i].callback != NULL)
                    gtk_signal_connect(GTK_OBJECT(menu_item), "activate",
                            GTK_SIGNAL_FUNC(item[i].callback),
                            item[i].func_data);
                if (item[i].func_name != NULL)
                    gtk_object_set_data(GTK_OBJECT(menu_item), "func_name",
                            item[i].func_name);
                gtk_widget_set_sensitive(GTK_WIDGET(menu_item),
                        item[i].sensitive);
                item[i].widget = menu_item;
                if (item[i].dwidget != NULL)
                    (*item[i].dwidget) = menu_item;
                break;
            case FACTORY_TYPE_RADIO :
#ifdef GTK_2
                menu_item = gtk_radio_menu_item_new_with_mnemonic(
                        group[item[i].group], item[i].label);
#else
                menu_item = gtk_radio_menu_item_new_with_label(
                        group[item[i].group], "");
                tmp_key = gtk_label_parse_uline(GTK_LABEL(
                        GTK_BIN(menu_item)->child), item[i].label);
                gtk_widget_add_accelerator(menu_item, "activate_item",
                        accel[l], tmp_key, 0, 0);
#endif
                group[item[i].group] = gtk_radio_menu_item_group(
                        GTK_RADIO_MENU_ITEM(menu_item));
                gtk_widget_show(menu_item);
                if (tooltips)
                    gtk_tooltips_set_tip(tooltips, menu_item,
                            item[i].tooltip, NULL);
                gtk_container_add(GTK_CONTAINER(level[l]), menu_item);
                if (item[i].accel_key > 0)
                    gtk_widget_add_accelerator(menu_item, "activate",
                            window_accels, item[i].accel_key,
                            item[i].accel_mod, GTK_ACCEL_VISIBLE);
                gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(
                        menu_item), item[i].state);
                gtk_check_menu_item_set_show_toggle(GTK_CHECK_MENU_ITEM(
                            menu_item), TRUE);
                if (item[i].callback != NULL)
                    gtk_signal_connect(GTK_OBJECT(menu_item), "activate",
                            GTK_SIGNAL_FUNC(item[i].callback),
                            item[i].func_data);
                if (item[i].func_name != NULL)
                    gtk_object_set_data(GTK_OBJECT(menu_item), "func_name",
                            item[i].func_name);
                gtk_widget_set_sensitive(GTK_WIDGET(menu_item),
                        item[i].sensitive);
                item[i].widget = menu_item;
                if (item[i].dwidget != NULL)
                    (*item[i].dwidget) = menu_item;
                break;
            case FACTORY_TYPE_RET :
                if (l > 0)
                    l--;
                break;
            default :
                factory_perror("invalid item type '%d'",
                        item[i].type);
        }
    }

    return handlebox;
}


GtkWidget *factory_toolbar(GtkWidget *window, factory_item *item,
        GtkAccelGroup *window_accels){
int count;             /* Number of items */
int i;                 /* Item loop */
GtkWidget *handlebox;  /* Menubar handlebox */
GtkWidget *toolbar;    /* Toolbar */
GdkPixmap *pixmap;     /* Temporary pixmap */
GdkBitmap *mask;       /* Temporary pixmap mask */
GtkWidget *pixmapwid;  /* Temporary pixmap widget */
GtkStyle *style;       /* Window style */
GtkWidget **group;     /* Radio groups */
int groups;            /* Radio groups count */
GtkWidget *widget;     /* Temporary widget */


    /* Count number of items */
    for (count = 0;
         item[count].type != FACTORY_TYPE_END;
         count++)
        ;

    /* Creates menubar and menubar handlebox */
    handlebox = gtk_handle_box_new();
    gtk_widget_show(handlebox);
#ifdef GTK_2
    toolbar = gtk_toolbar_new();
    gtk_toolbar_set_orientation(GTK_TOOLBAR(toolbar),
            GTK_ORIENTATION_HORIZONTAL);
     gtk_toolbar_set_style(GTK_TOOLBAR(toolbar),
             GTK_TOOLBAR_BOTH);
#else
    toolbar = gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL,
            GTK_TOOLBAR_BOTH);
#endif
    gtk_widget_show(toolbar);
    gtk_container_add(GTK_CONTAINER(handlebox), toolbar);

    /* Create radio button groups */
    groups = 0;
    for (i = 0; i < count; i++){
        if((item[i].type != FACTORY_TYPE_NONE) &&
           (item[i].type != FACTORY_TYPE_END) &&
           (item[i].type != FACTORY_TYPE_SEP))
            if (item[i].group > groups)
                groups = item[i].group;
    }
    groups++;
    group = (GtkWidget **) malloc(groups * sizeof(GtkWidget *));
    for (i = 0; i < groups; i++)
        group[i] = NULL;
    
    /* Create items */
    for (i = 0; i < count; i++){

        /* Pixmap */
        pixmapwid = NULL;
        if ((item[i].type == FACTORY_TYPE_BUTTON)){
            if (item[i].pixmap == FACTORY_PIXMAP_DATA){
                style = gtk_widget_get_style(window);
                pixmap = gdk_pixmap_create_from_xpm_d(
                        window->window, &mask,
                        &style->bg[GTK_STATE_NORMAL],
                        (gchar **) item[i].pixmap_name);
                if (pixmap){
                    pixmapwid = gtk_pixmap_new(pixmap, mask);
                    gtk_widget_show(pixmapwid);
                }
            } else if (item[i].pixmap == FACTORY_PIXMAP_FILE){
                style = gtk_widget_get_style(window);
                pixmap = gdk_pixmap_create_from_xpm(
                        window->window, &mask,
                        &style->bg[GTK_STATE_NORMAL],
                        (gchar *) item[i].pixmap_name);
                if (pixmap){
                    pixmapwid = gtk_pixmap_new(pixmap, mask);
                    gtk_widget_show(pixmapwid);
                }
            } else if (item[i].pixmap > FACTORY_PIXMAP_FILE){
            }
        }

        /* Items */
        switch (item[i].type){
            case FACTORY_TYPE_NONE :
                break;
            case FACTORY_TYPE_SEP :
                gtk_toolbar_append_space(GTK_TOOLBAR(toolbar));
                break;
            case FACTORY_TYPE_BUTTON :
                if (pixmapwid)
                    widget = gtk_toolbar_append_item(GTK_TOOLBAR(toolbar),
                            item[i].label, item[i].tooltip, NULL,
                            pixmapwid, item[i].callback,
                            item[i].func_data);
                else
                    widget = gtk_toolbar_append_item(GTK_TOOLBAR(toolbar),
                            item[i].label, item[i].tooltip, NULL, NULL,
                            item[i].callback, item[i].func_data);
                gtk_widget_set_sensitive(widget, item[i].sensitive);
                if (item[i].func_name != NULL)
                    gtk_object_set_data(GTK_OBJECT(widget), "func_name",
                            item[i].func_name);
                if (item[i].accel_key != 0)
                    gtk_widget_add_accelerator(widget, "clicked",
                            window_accels, item[i].accel_key,
                            item[i].accel_mod, 0);
                item[i].widget = widget;
                if (item[i].dwidget != NULL)
                    (*item[i].dwidget) = widget;
                break;
            case FACTORY_TYPE_CHECK :
                if (pixmapwid)
                    widget = gtk_toolbar_append_element(
                            GTK_TOOLBAR( toolbar),
                            GTK_TOOLBAR_CHILD_TOGGLEBUTTON, NULL,
                            item[i].label, item[i].tooltip, NULL,
                            pixmapwid, item[i].callback,
                            item[i].func_data);
                else
                    widget = gtk_toolbar_append_element(
                            GTK_TOOLBAR(toolbar),
                            GTK_TOOLBAR_CHILD_TOGGLEBUTTON, NULL,
                            item[i].label, item[i].tooltip, NULL,
                            NULL, item[i].callback, item[i].func_data);
                gtk_widget_set_sensitive(widget, item[i].sensitive);
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
                        widget), item[i].state);
                if (item[i].func_name != NULL)
                    gtk_object_set_data(GTK_OBJECT(widget), "func_name",
                            item[i].func_name);
                if (item[i].accel_key != 0)
                    gtk_widget_add_accelerator(widget, "clicked",
                            window_accels, item[i].accel_key,
                            item[i].accel_mod, 0);
                item[i].widget = widget;
                if (item[i].dwidget != NULL)
                    (*item[i].dwidget) = widget;
                break;
            case FACTORY_TYPE_RADIO :
                if (pixmapwid)
                    widget = group[item[i].group] =
                        gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
                            GTK_TOOLBAR_CHILD_RADIOBUTTON,
                            group[item[i].group],
                            item[i].label, item[i].tooltip, NULL,
                            pixmapwid, item[i].callback,
                            item[i].func_data);
                else
                    widget = group[item[i].group] =
                        gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
                            GTK_TOOLBAR_CHILD_RADIOBUTTON,
                            group[item[i].group],
                            item[i].label, item[i].tooltip, NULL,
                            NULL, item[i].callback, item[i].func_data);
                gtk_widget_set_sensitive(widget, item[i].sensitive);
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
                        widget), item[i].state);
                if (item[i].func_name != NULL)
                    gtk_object_set_data(GTK_OBJECT(widget), "func_name",
                            item[i].func_name);
                if (item[i].accel_key != 0)
                    gtk_widget_add_accelerator(widget, "clicked",
                            window_accels, item[i].accel_key,
                            item[i].accel_mod, 0);
                item[i].widget = widget;
                if (item[i].dwidget != NULL)
                    (*item[i].dwidget) = widget;
                break;
            default :
                factory_perror("invalid item type '%d'",
                        item[i].type);
        }

    }

    return handlebox;
}


GtkWidget *factory_menu(GtkWidget *window, factory_item *item,
        GtkAccelGroup *window_accels, GtkTooltips *tooltips){
int count;                                 /* Number of items */
int i;                                     /* Item loop */
int l;                                     /* Current level */
GtkWidget *mainmenu;                       /* Menu */
GtkWidget *level[FACTORY_MAX_LEVELS];      /* Parents  */
#ifndef GTK_2
GtkAccelGroup *accel[FACTORY_MAX_LEVELS];  /* Perents' accels */
guint tmp_key;                             /* Temporary accel key */ 
GtkWidget *hbox;                           /* Temporary menu item hbox */
GtkWidget *label;                          /* Temporary menu item label */
#endif
GSList **group;                            /* Radio button groups */
int groups;                                /* Groups count */
GtkWidget *menu_item;                      /* Temporary menu item */  
GtkWidget *menu;                           /* Temporary menu */ 
GtkWidget *tearoff;                        /* Temporary tearoff */
GdkPixmap *pixmap;                         /* Temporary pixmap */
GdkBitmap *mask;                           /* Temporary pixmap mask */
GtkWidget *image;                          /* Temporary image
                                              or pixmap widget */
GtkStyle *style;                           /* Window style */

    (void) window;
    
    /* Count number of items */
    for (count = 0;
         item[count].type != FACTORY_TYPE_END;
         count++)
        ;

    /* Create menubar and menubar handlebox */
    mainmenu = gtk_menu_new();
    l = 0;
    level[l] = mainmenu;
#ifndef GTK_2
    accel[l] = gtk_menu_ensure_uline_accel_group(GTK_MENU(mainmenu));
#endif

    /* Create radio button groups */
    groups = 0;
    for (i = 0; i < count; i++){
        if (item[i].type == FACTORY_TYPE_RADIO)
            if (item[i].group > groups)
                groups = item[i].group;
    }
    groups++;
    group = (GSList **) malloc(groups * sizeof(GSList *));
    for (i = 0; i < groups; i++)
        group[i] = NULL;

    /* Create items */
    for (i = 0; i < count; i++){

        /* Pixmap */
        image = NULL;
        if ((item[i].pixmap != FACTORY_PIXMAP_NONE) &&
            (item[i].type != FACTORY_TYPE_NONE) &&
            (item[i].type != FACTORY_TYPE_END) &&
            (item[i].type < FACTORY_TYPE_SEP)){
            if (item[i].pixmap == FACTORY_PIXMAP_DATA){
                style = gtk_widget_get_style(window);
                pixmap = gdk_pixmap_create_from_xpm_d(
                        window->window, &mask,
                        &style->bg[GTK_STATE_NORMAL],
                        (gchar **) item[i].pixmap_name);
                if (pixmap){
#ifdef GTK_2
                    image = gtk_image_new_from_pixmap(pixmap, mask);
                    gtk_widget_show(image);
#else
                    image = gtk_pixmap_new(pixmap, mask);
                    gtk_widget_show(image);
#endif
                }
            } else if (item[i].pixmap == FACTORY_PIXMAP_FILE){
                style = gtk_widget_get_style(window);
                pixmap = gdk_pixmap_create_from_xpm(
                        window->window, &mask,
                        &style->bg[GTK_STATE_NORMAL],
                        (gchar *) item[i].pixmap_name);
                if (pixmap){
#ifdef GTK_2
                    image = gtk_image_new_from_pixmap(pixmap, mask);
                    gtk_widget_show(image);
#else
                    image = gtk_pixmap_new(pixmap, mask);
                    gtk_widget_show(image);
#endif
                }
            } else if (item[i].pixmap > FACTORY_PIXMAP_FILE){
            }
        }

        switch (item[i].type){
            case FACTORY_TYPE_NONE :
                break;
            case FACTORY_TYPE_MENU :
                if (image){
#ifdef GTK_2
                    menu_item = gtk_image_menu_item_new_with_mnemonic(
                            item[i].label);
                    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(
                            menu_item), image);
#else
                    hbox = gtk_hbox_new(FALSE, 0);
                    gtk_widget_show(hbox);
                    label = gtk_label_new("");
                    tmp_key = gtk_label_parse_uline(GTK_LABEL(label),
                            item[i].label);
                    gtk_widget_show(label);
                    gtk_container_add(GTK_CONTAINER(hbox), image);
                    gtk_container_add(GTK_CONTAINER(hbox), label);
                    menu_item = gtk_menu_item_new();
                    gtk_container_add(GTK_CONTAINER(menu_item), hbox);
                    gtk_widget_add_accelerator(menu_item, "activate_item",
                            accel[l], tmp_key, 0, 0);
#endif
                } else {
#ifdef GTK_2
                    menu_item = gtk_menu_item_new_with_mnemonic(
                            item[i].label);
#else
                    menu_item = gtk_menu_item_new_with_label("");
                    tmp_key = gtk_label_parse_uline(GTK_LABEL(
                            GTK_BIN(menu_item)->child),
                            item[i].label);
                    gtk_widget_add_accelerator(menu_item, "activate_item",
                            accel[l], tmp_key, 0, 0);
#endif
                }
                gtk_widget_show(menu_item);
                gtk_container_add(GTK_CONTAINER(level[l]), menu_item);
                if (tooltips)
                    gtk_tooltips_set_tip(tooltips, menu_item,
                            item[i].tooltip, NULL);
                menu = gtk_menu_new();
                gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), menu);
#ifndef GTK_2
                accel[l+1] =
                        gtk_menu_ensure_uline_accel_group(GTK_MENU(menu));
#endif
                tearoff = gtk_tearoff_menu_item_new();
                gtk_widget_show(tearoff);
                gtk_container_add(GTK_CONTAINER(menu), tearoff);
                if (item[i].accel_key > 0)
                    gtk_widget_add_accelerator(menu_item, "activate",
                            window_accels, item[i].accel_key,
                            item[i].accel_mod, GTK_ACCEL_VISIBLE);
                l++;
                level[l] = menu;
                item[i].widget = menu_item;
                if (item[i].dwidget != NULL)
                    (*item[i].dwidget) = menu_item;
                break;
            case FACTORY_TYPE_ITEM :
                if (image){
#ifdef GTK_2
                    menu_item = gtk_image_menu_item_new_with_mnemonic(
                            item[i].label);
                    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(
                            menu_item), image);
#else
                    hbox = gtk_hbox_new(FALSE, 0);
                    gtk_widget_show(hbox);
                    label = gtk_label_new("");
                    tmp_key = gtk_label_parse_uline(GTK_LABEL(label),
                            item[i].label);
                    gtk_widget_show(label);
                    gtk_container_add(GTK_CONTAINER(hbox), image);
                    gtk_container_add(GTK_CONTAINER(hbox), label);
                    menu_item = gtk_menu_item_new();
                    gtk_container_add(GTK_CONTAINER(menu_item), hbox);
                    gtk_widget_add_accelerator(menu_item, "activate_item",
                            accel[l], tmp_key, 0, 0);
#endif
                } else {
#ifdef GTK_2
                    menu_item = gtk_menu_item_new_with_mnemonic(
                            item[i].label);
#else
                    menu_item = gtk_menu_item_new_with_label("");
                    tmp_key = gtk_label_parse_uline(GTK_LABEL(
                            GTK_BIN(menu_item)->child), item[i].label);
                    gtk_widget_add_accelerator(menu_item, "activate_item",
                            accel[l], tmp_key, 0, 0);
#endif
                }
                gtk_widget_show(menu_item);
                if (tooltips)
                    gtk_tooltips_set_tip(tooltips, menu_item,
                            item[i].tooltip, NULL);
                gtk_container_add(GTK_CONTAINER(level[l]), menu_item);
                if (item[i].accel_key > 0)
                    gtk_widget_add_accelerator(menu_item, "activate",
                            window_accels, item[i].accel_key,
                            item[i].accel_mod, GTK_ACCEL_VISIBLE);
                if (item[i].callback != NULL)
                    gtk_signal_connect(GTK_OBJECT(menu_item), "activate",
                            GTK_SIGNAL_FUNC(item[i].callback),
                            item[i].func_data);
                if (item[i].func_name != NULL)
                    gtk_object_set_data(GTK_OBJECT(menu_item), "func_name",
                            item[i].func_name);
                gtk_widget_set_sensitive(GTK_WIDGET(menu_item),
                        item[i].sensitive);
                item[i].widget = menu_item;
                if (item[i].dwidget != NULL)
                    (*item[i].dwidget) = menu_item;
                break;
            case FACTORY_TYPE_SEP :
                menu_item = gtk_menu_item_new();
                gtk_widget_show(menu_item);
                gtk_container_add(GTK_CONTAINER(level[l]), menu_item);
                gtk_widget_set_sensitive(menu_item, FALSE);
                item[i].widget = menu_item;
                if (item[i].dwidget != NULL)
                    (*item[i].dwidget) = menu_item;
                break;
            case FACTORY_TYPE_CHECK :
#ifdef GTK_2
                menu_item = gtk_check_menu_item_new_with_mnemonic(
                        item[i].label);
#else
                menu_item = gtk_check_menu_item_new_with_label("");
                tmp_key = gtk_label_parse_uline(GTK_LABEL(
                        GTK_BIN(menu_item)->child), item[i].label);
                gtk_widget_add_accelerator(menu_item, "activate_item",
                        accel[l], tmp_key, 0, 0);
#endif
                gtk_widget_show(menu_item);
                if (tooltips)
                    gtk_tooltips_set_tip(tooltips, menu_item,
                            item[i].tooltip, NULL);
                gtk_container_add(GTK_CONTAINER(level[l]), menu_item);
                if (item[i].accel_key > 0)
                    gtk_widget_add_accelerator(menu_item, "activate",
                            window_accels, item[i].accel_key,
                            item[i].accel_mod, GTK_ACCEL_VISIBLE);
                gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(
                        menu_item), item[i].state);
                gtk_check_menu_item_set_show_toggle(GTK_CHECK_MENU_ITEM(
                        menu_item), TRUE);
                if (item[i].callback != NULL)
                    gtk_signal_connect(GTK_OBJECT(menu_item), "activate",
                            GTK_SIGNAL_FUNC(item[i].callback),
                            item[i].func_data);
                if (item[i].func_name != NULL)
                    gtk_object_set_data(GTK_OBJECT(menu_item), "func_name",
                            item[i].func_name);
                gtk_widget_set_sensitive(GTK_WIDGET(menu_item),
                        item[i].sensitive);
                item[i].widget = menu_item;
                if (item[i].dwidget != NULL)
                    (*item[i].dwidget) = menu_item;
                break;
            case FACTORY_TYPE_RADIO :
#ifdef GTK_2
                menu_item = gtk_radio_menu_item_new_with_mnemonic(
                        group[item[i].group], item[i].label);
#else
                menu_item = gtk_radio_menu_item_new_with_label(
                        group[item[i].group], "");
                tmp_key = gtk_label_parse_uline(GTK_LABEL(
                        GTK_BIN(menu_item)->child), item[i].label);
                gtk_widget_add_accelerator(menu_item, "activate_item",
                        accel[l], tmp_key, 0, 0);
#endif
                group[item[i].group] = gtk_radio_menu_item_group(
                        GTK_RADIO_MENU_ITEM(menu_item));
                gtk_widget_show(menu_item);
                if (tooltips)
                    gtk_tooltips_set_tip(tooltips, menu_item,
                            item[i].tooltip, NULL);
                gtk_container_add(GTK_CONTAINER(level[l]), menu_item);
                if (item[i].accel_key > 0)
                    gtk_widget_add_accelerator(menu_item, "activate",
                            window_accels, item[i].accel_key,
                            item[i].accel_mod, GTK_ACCEL_VISIBLE);
                gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(
                        menu_item), item[i].state);
                gtk_check_menu_item_set_show_toggle(GTK_CHECK_MENU_ITEM(
                            menu_item), TRUE);
                if (item[i].callback != NULL)
                    gtk_signal_connect(GTK_OBJECT(menu_item), "activate",
                            GTK_SIGNAL_FUNC(item[i].callback),
                            item[i].func_data);
                if (item[i].func_name != NULL)
                    gtk_object_set_data(GTK_OBJECT(menu_item), "func_name",
                            item[i].func_name);
                gtk_widget_set_sensitive(GTK_WIDGET(menu_item),
                        item[i].sensitive);
                item[i].widget = menu_item;
                if (item[i].dwidget != NULL)
                    (*item[i].dwidget) = menu_item;
                break;
            case FACTORY_TYPE_RET :
                if (l > 0)
                    l--;
                break;
            default :
                factory_perror("invalid item type '%d'",
                        item[i].type);
        }
    }

    return mainmenu;
}


GtkWidget *factory_msg_dialog(char *title, char *text,
        GtkSignalFunc ok_callback){
GtkWidget *dialog, *vbox, *label;
GtkWidget *action_area, *button;

    dialog = gtk_dialog_new();
    gtk_window_set_title(GTK_WINDOW(dialog), title);
    gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_MOUSE);
    gtk_window_set_policy(GTK_WINDOW(dialog), TRUE, TRUE, FALSE);

    vbox = GTK_DIALOG(dialog)->vbox;
    gtk_widget_show(vbox);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 2);

    label = gtk_label_new(text);
    gtk_widget_show(label);
    gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 10);
    gtk_misc_set_padding(GTK_MISC(label), 12, 0);
    gtk_object_set_data(GTK_OBJECT(dialog), "label", label);

    action_area = GTK_DIALOG(dialog)->action_area;
    gtk_widget_show(action_area);
    gtk_container_set_border_width(GTK_CONTAINER(action_area), 10);

    button = gtk_button_new_with_label("  OK  ");
    gtk_widget_show(button);
    gtk_box_pack_start(GTK_BOX(action_area), button, FALSE, FALSE, 0);
    GTK_WIDGET_SET_FLAGS(button, GTK_CAN_DEFAULT);
    gtk_signal_connect(GTK_OBJECT(button), "clicked",
                       GTK_SIGNAL_FUNC(ok_callback),
                       NULL);
    gtk_widget_grab_default(button);

    return dialog;
}


GtkWidget *factory_confirm_dialog(char *title, char *text,
        GtkSignalFunc yes_callback, GtkSignalFunc no_callback){
GtkWidget *dialog, *vbox, *label;
GtkWidget *action_area, *yesbtn, *nobtn;

    dialog = gtk_dialog_new();
    gtk_window_set_title(GTK_WINDOW(dialog), title);
    gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_MOUSE);
    gtk_window_set_policy(GTK_WINDOW(dialog), TRUE, TRUE, FALSE);

    vbox = GTK_DIALOG(dialog)->vbox;
    gtk_widget_show(vbox);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 2);

    label = gtk_label_new(text);
    gtk_widget_show(label);
    gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 10);
    gtk_misc_set_padding(GTK_MISC(label), 12, 0);
    gtk_object_set_data(GTK_OBJECT(dialog), "label", label);

    action_area = GTK_DIALOG(dialog)->action_area;
    gtk_widget_show(action_area);
    gtk_container_set_border_width(GTK_CONTAINER(action_area), 10);

    yesbtn = gtk_button_new_with_label("Yes");
    gtk_widget_show(yesbtn);
    gtk_box_pack_start(GTK_BOX(action_area), yesbtn, FALSE, FALSE, 0);
    GTK_WIDGET_SET_FLAGS(yesbtn, GTK_CAN_DEFAULT);
    gtk_signal_connect(GTK_OBJECT(yesbtn), "clicked",
            GTK_SIGNAL_FUNC(yes_callback), NULL);
    gtk_widget_grab_default(yesbtn);
    
    nobtn = gtk_button_new_with_label("No");
    gtk_widget_show(nobtn);
    gtk_box_pack_start(GTK_BOX(action_area), nobtn, FALSE, FALSE, 0);
    gtk_signal_connect(GTK_OBJECT(nobtn), "clicked",
            GTK_SIGNAL_FUNC(no_callback), NULL);

    return dialog;
}


GtkWidget *factory_filesel_dialog(char *title,
        GtkSignalFunc ok_callback, GtkSignalFunc cancel_callback){
GtkWidget *filesel;

    filesel = gtk_file_selection_new(title);
    gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(
            filesel)->ok_button),
            "clicked", GTK_SIGNAL_FUNC(ok_callback), NULL);
    gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(
            filesel)->cancel_button),
            "clicked", GTK_SIGNAL_FUNC(cancel_callback), NULL);

    return filesel;
}


void factory_show_msg_dialog(GtkWidget *dialog, char *text){
GtkWidget *label;

    label = gtk_object_get_data(GTK_OBJECT(dialog), "label");
    gtk_label_set_text(GTK_LABEL(label), text);
    gtk_widget_show(dialog);
    gdk_window_raise(dialog->window);
}


void factory_show_fmsg_dialog(GtkWidget *dialog, char *format,...){
GtkWidget *label;
va_list args;
char buf[2000];

    va_start(args, format);
    vsprintf(buf, format, args);
    va_end(args);

    label = gtk_object_get_data(GTK_OBJECT(dialog), "label");
    gtk_label_set_text(GTK_LABEL(label), buf);
    gtk_widget_show(dialog);
    gdk_window_raise(dialog->window);
}


void factory_show_confirm_dialog(GtkWidget *dialog, char *text){
GtkWidget *label;

    label = gtk_object_get_data(GTK_OBJECT(dialog), "label");
    gtk_label_set_text(GTK_LABEL(label), text);
    gtk_widget_show(dialog);
    gdk_window_raise(dialog->window);
}


void factory_show_dialog(GtkWidget *dialog){
    
    gtk_widget_show(dialog);
    gdk_window_raise(dialog->window);
}


void factory_hide_dialog(GtkWidget *dialog){

    gtk_widget_hide(dialog);
}


void factory_hide_toplevel_dialog(GtkWidget *widget){
GtkWidget *dialog;

    dialog = gtk_widget_get_toplevel(widget);
    gtk_widget_hide(dialog);
}


void factory_hide_dialog_callback(GtkWidget *widget,
        gpointer user_data){

    (void) user_data;

    gtk_widget_hide(gtk_widget_get_toplevel(widget));
}


/* ******* */
/* * End * */
/* ******* */
