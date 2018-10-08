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


/* *************** */
/* * callbacks.c * */
/* *************** */


#include "../config.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
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
#include "../draw.h"
#include "../rw.h"
#include "../rc.h"
#include "../color.h"
#include "../pgin.h"
#include "../step.h"
#include "iface.h"
#include "callbacks.h"
#include "factory.h"


/* External globals */
/* Graph: gr.c */
extern Graph G;

/* Algorithm lists: alghandler.c */
extern AlgList L;
extern AlgList TL;
extern AlgListR RL;
extern AlgListS SL;
extern AlgListSS SSL;

/* Draw structure: draw.c */
extern DrawStruct DS;

/* Interface: iface.c */
extern Iface iface;

/* Configuration: gr.c (definition: rc.h) */
extern Config *config;

/* Read/write error: rw.c */
extern char rw_error[];

/* Call error: geometry.c */
extern char call_error[];

/* Color definitions: color.c */
extern Color_def color_def[];


#ifdef USE_GTKGL
/* Opengl aux vars */
GLuint SelectBuffer[64];
GLint viewport[4];
GLdouble modelMatrix[16], projMatrix[16];
GLuint fontbase;
#endif
int SelectedPoint = (int) ~0;
int SelectedCtrlPoint = 0;
int SCP[3] = {0,0,0};


/* Drawing area aux vars */
GdkGC *gc;
GdkFont *font;
GdkColor background_color  = { 0, 0x7CFF, 0x9DFF, 0xBFFF };
GdkColor lines_color  = { 0, 0xFFFF, 0xFFFF, 0xFFFF };
GdkColor special_lines_color  = { 0, 0xFFFF, 0xFFFF, 0x0000 };
GdkColor vertices_color  = { 0, 0xC7FF, 0x0000, 0x0000 };
GdkColor special_vertices_color  = { 0, 0xFFFF, 0xFFFF, 0x0000 };
GdkColor font_color  = { 0, 0x0000, 0x0000, 0x0000 };
GdkColor *gdk_color;

/* Aux global vars */
char *fontname = "-adobe-helvetica-medium-r-normal--*-100-*-*-*-*-*-*";
int IsDestiny = 0;
int ShowCtrlPts = 0;
char *EdInsString[2] = {"Click on Origin Vertex",
        "Click on Destiny Vertex"};
char *VeInsString = "Click to create a Vertex";
char *VeMovString = "Click on a Vertex and drag";
char *VeDelString = "Click to remove a Vertex";
char *VeEditString = "Click to edit vertex properties";
char *EdEditString[2] = {
    "Click to edit edge properties (Click on Origin Vertex)",
    "Click to edit edge properties (Click on Destiny Vertex)"};

char *VString = "Vertices";
char *AString = "Arcs";
char *EString = "Edges";

/* **** Drawing area and gtkgl area handle functions **** */
#ifdef USE_GTKGL
/* Initializes the gtkgl area */
gboolean init_glarea(GtkWidget *widget){
GdkFont *font;

    if (gtk_gl_area_make_current(GTK_GL_AREA(widget))){
        
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_POINT_SMOOTH);

        glClearColor(125.0 / 256.0, 158.0 / 256.0, 192.0 / 256.0, 1.0);

        /* Trecho experimental */
        glEnable(GL_MAP1_VERTEX_3); 
        /* */
        
        glPointSize(7.0);
        
        glSelectBuffer(sizeof(SelectBuffer), SelectBuffer);
        
        fontbase = glGenLists(128);
        font = gdk_font_load(fontname);
        if (!font) {
            g_print("Can't load font '%s'\n", fontname);
            gtk_exit(1);
        }

        gdk_gl_use_gdk_font(font, 0, 128, fontbase);
        //fontheight = font->ascent+font->descent;
        gdk_font_unref(font);
    }

    /* Event was handled */
    return TRUE;
}
#endif


/* Initializes the drawing area */
gboolean init_drawingarea(GtkWidget *widget){
GdkColormap *colormap;
int i;

    /* Allocs colors */
    colormap = gtk_widget_get_colormap(widget);
    gdk_colormap_alloc_color(colormap, &background_color, TRUE, TRUE);
    gdk_colormap_alloc_color(colormap, &lines_color, TRUE, TRUE);
    gdk_colormap_alloc_color(colormap, &special_lines_color, TRUE, TRUE);
    gdk_colormap_alloc_color(colormap, &special_vertices_color, TRUE, TRUE);
    gdk_colormap_alloc_color(colormap, &vertices_color, TRUE, TRUE);
    gdk_colormap_alloc_color(colormap, &font_color, TRUE, TRUE);

    gdk_color = (GdkColor *) malloc((LASTCOLOR+1) * sizeof(GdkColor));
    for (i = 0; i <= LASTCOLOR ; i++){
        gdk_color[i].red = (((int)color_def[i].r) * 0xFFFF) / 0xFF;
        gdk_color[i].green = (((int)color_def[i].g) * 0xFFFF) / 0xFF;
        gdk_color[i].blue = (((int)color_def[i].b) * 0xFFFF) / 0xFF;
        gdk_colormap_alloc_color(colormap, &(gdk_color[i]), TRUE, TRUE);
    }

    gc = gdk_gc_new(widget->window);

    font = gdk_font_load(fontname);

    /* Event was handled */
    return TRUE;
}


/* Draws the drawing area */
gboolean expose_event_callback(GtkWidget *widget,
        GdkEventExpose *event, gpointer user_data){
char str[20];
int i, j, k;

    (void) event;
    (void) user_data;

    if ((iface.view_mode == VIEW_MODE_GTK) &&
        GTK_IS_DRAWING_AREA(widget)){
    int cx[4], cy[4];
    Node N;
    SEdge *E;
    SVertex *V;
    int l;
    GdkPoint points[3];


        /* Draw only last expose */
        if (event->count > 0)
            return TRUE;

        if ((!iface.changed) && (iface.back)){
            gdk_draw_pixmap(widget->window,
                    widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
                    iface.back,
                    event->area.x, event->area.y,
                    event->area.x, event->area.y,
                    event->area.width, event->area.height);
        } else if ((iface.changed) && (iface.back)){

            /* Background */
            gdk_gc_set_foreground(gc, &background_color);
            gdk_draw_rectangle(iface.back, gc, 1, 0, 0,
                    widget->allocation.width, widget->allocation.height);

            /* Arcs or Edges */
            if (G.type == DIGRAPH) {
                for (j = 0; j < G.size; j++) {	
                    for (k = 0; k < G.size; k++) {
                        if ((G.edge[j][k]) && (j != k)) {
              				change_edge_color(&G, j, k, gc);
                            for (i = 0; i < 4; i++){
                                cx[i] = G.prop[j][k].ctrlpoints[i][0];
                                cy[i] = iface.height -
                                        G.prop[j][k].ctrlpoints[i][1];
                            }
                            gdk_draw_spline(iface.back, gc, 3, cx, cy);
                        }
                    }
                }
            } else {
                for (j = 0; j < G.size; j++) {	
                    for (k = j+1; k < G.size; k++) {
                        if (G.edge[j][k]) {
               				change_edge_color(&G, j, k, gc);
                            for (i = 0; i < 4; i++){
                                cx[i] = G.prop[j][k].ctrlpoints[i][0];
                                cy[i] = iface.height -
                                        G.prop[j][k].ctrlpoints[i][1];
                            }
                            gdk_draw_spline(iface.back, gc, 3, cx, cy);



							
                            //gdk_draw_line(iface.back, gc,
                            //        G.vertex[j].x,
                            //        iface.height - G.vertex[j].y,
                            //        G.vertex[k].x,
                            //        iface.height - G.vertex[k].y);

							
                        } else if (G.edge[k][j]) {
               				change_edge_color(&G, k, j, gc);
							for (i = 0; i < 4; i++){
                                cx[i] = G.prop[j][k].ctrlpoints[i][0];
                                cy[i] = iface.height -
                                        G.prop[j][k].ctrlpoints[i][1];
                            }
                            gdk_draw_spline(iface.back, gc, 3, cx, cy);
						
							
							//gdk_draw_line(iface.back, gc,
                            //        G.vertex[j].x,
                            //        iface.height - G.vertex[j].y,
                            //        G.vertex[k].x,
                            //        iface.height - G.vertex[k].y);
                        }	
                    }
                }
            }

            /* Control Points */
            if (ShowCtrlPts) { 
                if (G.type == DIGRAPH)  {
                    gdk_gc_set_foreground(gc, &special_vertices_color);
		
					for (i = 0; i < G.size; i++) {
						for (j = 0; j < G.size; j++) {
							if (G.edge[i][j]) {	
    							for (k = 0; k < 4; k++) {
	    								gdk_draw_arc(iface.back, gc, 1,
		    								G.prop[i][j].ctrlpoints[k][X]-3, 
			    								iface.height - 
				    							G.prop[i][j].ctrlpoints[k][Y]-3,
					    						7, 7, 0, 360*64);
						    		//gdk_draw_arc(iface.back, gc, 1,
							    	//		G.prop[j][i].ctrlpoints[k][X]-3, 
								   // 		iface.height - 
									//    	G.prop[j][i].ctrlpoints[k][Y]-3,
									//	    7, 7, 0, 360*64);
				
							    }
							}
						}
					}

				} else {

                    gdk_gc_set_foreground(gc, &special_vertices_color);
					for (i = 0; i < G.size; i++) {
						for (j = i; j < G.size; j++) {

							if (G.edge[i][j]) {	
								gdk_draw_arc(iface.back, gc, 1,
											G.prop[i][j].ctrlpoints[CTR1][X]-3, 
											iface.height - 
											G.prop[i][j].ctrlpoints[1][Y]-3,
											7, 7, 0, 360*64);
								 gdk_draw_arc(iface.back, gc, 1,
											G.prop[i][j].ctrlpoints[CTR2][X]-3, 
											iface.height - 
											G.prop[i][j].ctrlpoints[2][Y]-3,
											7, 7, 0, 360*64);
							}
						}
					}
				
    			}
				
       	    }
				


            /* Vertices */
            gdk_gc_set_foreground(gc, &vertices_color);
            for (i = 0; i < G.size; i++) {
                change_color(&G, i, gc);
                gdk_draw_arc(iface.back, gc, 1,
                        G.vertex[i].x-5, iface.height - G.vertex[i].y-5,
                        11, 11, 0, 360*64);
            }


			
            /* Draw other objects */
            CalculateArrows(&G, &DS); // colocar dentro do draw objects...
            //DrawObjects(&DS, &G);
            
            gdk_gc_set_foreground(gc, &lines_color);
            /* Draw the head of the arrows */
            if (G.type == DIGRAPH) {
                for (i = 0; i < G.size; i++) {
                    for (j = 0; j < G.size; j++) {
                        if ((i != j) && (G.edge[i][j] && (!G.edge[j][i]))) {
                            //for (k = 0; k < 3; k++) {
                                points[0].x = DS.arrow[i][j][0].x;
                                points[0].y = iface.height -
                                        DS.arrow[i][j][0].y;
                                points[1].x = DS.arrow[i][j][1].x;
                                points[1].y = iface.height -
                                    DS.arrow[i][j][1].y;
                                points[2].x = DS.arrow[i][j][2].x;
                                points[2].y = iface.height -
                                    DS.arrow[i][j][2].y;
                                gdk_draw_polygon(iface.back, gc, 1,
                                        points, 3);
                            //}
                        } else if ((i != j) &&
                                (G.edge[i][j] && (G.edge[j][i]))) {
                            //for (k = 0; k < 3; k++) {
                                points[0].x = DS.arrow[i][j][0].x;
                                points[0].y = iface.height -
                                    DS.arrow[i][j][0].y;
                                points[1].x = DS.arrow[i][j][1].x;
                                points[1].y = iface.height -
                                    DS.arrow[i][j][1].y;
                                points[2].x = DS.arrow[i][j][2].x;
                                points[2].y = iface.height -
                                        DS.arrow[i][j][2].y;
                                gdk_draw_polygon(iface.back, gc, 1,
                                        points, 3);
                            //}
                            //for (k = 0; k < 3; k++) {
                                points[0].x = DS.arrow[j][i][0].x;
                                points[0].y = iface.height -
                                        DS.arrow[j][i][0].y;
                                points[1].x = DS.arrow[j][i][1].x;
                                points[1].y = iface.height -
                                        DS.arrow[j][i][1].y;
                                points[2].x = DS.arrow[j][i][2].x;
                                points[2].y = iface.height -
                                        DS.arrow[j][i][2].y;
                                gdk_draw_polygon(iface.back, gc, 1,
                                        points, 3);
                            //}
                        }
                    }
                }
            }

            /* Draw special edges */
            N = DS.SpecEdges->head;
            gdk_gc_set_foreground(gc, &special_lines_color);
            E = (SEdge *) malloc (sizeof(SEdge));
            gdk_gc_set_line_attributes(gc, 3, GDK_LINE_SOLID,
                    GDK_CAP_NOT_LAST, GDK_JOIN_MITER);

            while (N != NULL) {

                memcpy(E, N->data, sizeof(SEdge));

                for (l = -1; l < 2; l++){
                    for (i = 0; i < 4; i++){
                        cx[i] = G.prop[E->u][E->v].ctrlpoints[i][0]+l;
                        cy[i] = iface.height -
                                G.prop[E->u][E->v].ctrlpoints[i][1]+l;
                    }
                    gdk_draw_spline(iface.back, gc, 3, cx, cy);
                }

                N = N->next;
            }

            free(E);
            gdk_gc_set_line_attributes(gc, 1, GDK_LINE_SOLID,
                    GDK_CAP_NOT_LAST, GDK_JOIN_MITER);

            /* Vertices */
            gdk_gc_set_foreground(gc, &vertices_color);
            for (i = 0; i < G.size; i++) {
                change_color(&G, i, gc);
                gdk_draw_arc(iface.back, gc, 1,
                        G.vertex[i].x-5, iface.height - G.vertex[i].y-5,
                        11, 11, 0, 360*64);
            }

            /* Special Vertices */
            gdk_gc_set_foreground(gc, &special_vertices_color);
            N = DS.SpecVerts->head;
            V = (SVertex *) malloc (sizeof(SVertex));

            gdk_gc_set_line_attributes(gc, 3, GDK_LINE_SOLID,
					                    GDK_CAP_NOT_LAST, GDK_JOIN_MITER);
			
            while (N != NULL) {
                memcpy(V, N->data, sizeof(SVertex));
                gdk_draw_arc(iface.back, gc, 0,
                        G.vertex[*V].x-9, iface.height - G.vertex[*V].y-9,
                        19, 19, 0, 360*64);
                N = N->next;
            }

            free(V);
	    
			gdk_gc_set_line_attributes(gc, 1, GDK_LINE_SOLID,
					                    GDK_CAP_NOT_LAST, GDK_JOIN_MITER);
	    
            /* Labels */
            gdk_gc_set_foreground(gc, &font_color);
            if (iface.vlabel_mode){
                for (i = 0; i < G.size; i++) {
                    str[0] = 0;
                    switch (iface.vlabel_mode){
                        case 1:
                            sprintf(str, "%d", i);
                            break;

                        case 2:
                            sprintf(str, "%d", G.vertex[i].id);
                            break;

                        case 3:
                            if (G.vertex[i].label != NULL)
                                sprintf(str, "%s", G.vertex[i].label);
                            break;

                        case 4:
                            sprintf(str, "%g", G.vertex[i].weight);
                            break;
                            
                        case 5:
                            sprintf(str, "%d", G.vertex[i].color);
                            break;
                    }
                    gdk_draw_text(iface.back, font, gc,
                            G.vertex[i].x+gdk_text_width(font, "M", 1),
                            iface.height -
                                G.vertex[i].y-gdk_text_height(font, "M", 1),
                            str, strlen(str));
                }
            }
            
            if (iface.elabel_mode){
                for (i = 0; i < G.size; i++) {
                    for (j = i+1; j < G.size; j++){
						if (G.edge[i][j]) {
                            str[0] = 0;
                            switch (iface.elabel_mode){
                                case 1:
                                    sprintf(str, "(%d,%d)", i, j);
                                    break;

								case 2:
									sprintf(str, "(%d,%d)",
											G.vertex[i].id, G.vertex[j].id);
									break;

								case 3:
									if (G.prop[i][j].label != NULL)
										sprintf(str, "%s", G.prop[i][j].label);
									break;

								case 4:
									sprintf(str, "%g", G.prop[i][j].weight);
									break;
									
								case 5:
									sprintf(str, "%d", G.prop[i][j].color);
									break;
							}
						    gdk_draw_text(iface.back, font, gc,
								((G.vertex[i].x+G.vertex[j].x)/2)+
								gdk_text_width(font, "M", 1),
								iface.height -
									((G.vertex[i].y+G.vertex[j].y)/2)-
									gdk_text_height(font, "M", 1),
								str, strlen(str));
						}
                    }
                }
            }

            gdk_draw_pixmap(widget->window,
                    widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
                    iface.back,
                    event->area.x, event->area.y,
                    event->area.x, event->area.y,
                    event->area.width, event->area.height);

            iface.changed = 0;

            /* Update status bar */
            update_status();
        }
    }

#ifdef USE_GTKGL
    else if ((iface.view_mode == VIEW_MODE_OPENGL) &&
             GTK_IS_GL_AREA(widget)){
    unsigned int l;
    Node N;
    SVertex *V;

        /* Draw only last expose */
        if (event->count > 0)
            return TRUE;
        
        if (gtk_gl_area_make_current(GTK_GL_AREA(widget))){

            glClear(GL_COLOR_BUFFER_BIT);
            
            /* Arcs or Edges */
            if (G.type == DIGRAPH) {
                for (j = 0; j < G.size; j++) {	
                    for (k = 0; k < G.size; k++) {
                        if ((G.edge[j][k]) && (j != k)) {	
                            glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, 
                                    &(G.prop[j][k].ctrlpoints[0][0]));
              				ChooseGlEdgeColor(&G, j, k);
                            glBegin(GL_LINE_STRIP);
                                for (i = 0; i <= 30; i++)
                                    glEvalCoord1f((GLfloat) i/30.0);
                            glEnd();
                        }
                    }
                }
            } else {
                for (j = 0; j < G.size; j++) {	
                    for (k = j+1; k < G.size; k++) {
                        if (G.edge[j][k]) {	
               				ChooseGlEdgeColor(&G, j, k);
                            glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, 
                                    &(G.prop[j][k].ctrlpoints[0][0]));
                            glBegin(GL_LINE_STRIP);
                                for (i = 0; i <= 30; i++)
                                    glEvalCoord1f((GLfloat) i/30.0);
                            glEnd();
                        } else if (G.edge[k][j]) {
               				ChooseGlEdgeColor(&G, k, j);
                            glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, 
                                    &(G.prop[k][j].ctrlpoints[0][0]));
                            glBegin(GL_LINE_STRIP);
                                for (i = 0; i <= 30; i++)
                                    glEvalCoord1f((GLfloat) i/30.0);
                            glEnd();

                        }	
                    }
                }
            } 


            if (ShowCtrlPts) { 

                /* Control Points */
                glPointSize(5.0);
                glColor3ub(255, 255, 0);
                glBegin(GL_POINTS);

                if (G.type == DIGRAPH) {
                    for (i = 0; i < G.size; i++) {
                        for (j = 0; j < G.size; j++) {
                            if (G.edge[i][j]) {
                            //for (k = 0; k < 4; k++) {
                                //glVertex2d(G.prop[i][j].ctrlpoints[k][X],
                                //          G.prop[i][j].ctrlpoints[k][Y]);
                                //glVertex2d(G.prop[j][i].ctrlpoints[k][X],
                                //          G.prop[j][i].ctrlpoints[k][Y]);
                            //}
                                glVertex2d(G.prop[i][j].ctrlpoints[1][X], G.prop[i][j].ctrlpoints[1][Y]);
                                glVertex2d(G.prop[i][j].ctrlpoints[2][X], G.prop[i][j].ctrlpoints[2][Y]);

                            }
                        }
                    }
				} else {
                    for (i = 0; i < G.size; i++) {
                        for (j = i; j < G.size; j++) {
                            if (G.edge[i][j]) {
                            //for (k = 0; k < 4; k++) {
                                //glVertex2d(G.prop[i][j].ctrlpoints[k][X],
                                //          G.prop[i][j].ctrlpoints[k][Y]);
                                //glVertex2d(G.prop[j][i].ctrlpoints[k][X],
                                //          G.prop[j][i].ctrlpoints[k][Y]);
                            //}
                                glVertex2d(G.prop[i][j].ctrlpoints[1][X], G.prop[i][j].ctrlpoints[1][Y]);
                                glVertex2d(G.prop[i][j].ctrlpoints[2][X], G.prop[i][j].ctrlpoints[2][Y]);

                            }
                        }
                    }
				}

            }
            glEnd();
		


	
            /* Draw other objects */
            CalculateArrows(&G, &DS); // colocar dentro do draw objects...
            DrawObjects(&DS, &G);

  	        /* Vertices */
            //glPointSize(7.0);
            glPointSize(10.0);
            glColor3ub(200, 0, 0);
            glBegin(GL_POINTS);
            for (i = 0; i < G.size; i++) {
                ChooseGlColor(&G, i);
                glVertex2d(G.vertex[i].x, G.vertex[i].y);
            }
            glEnd();


            /* Special Vertices - por no draw.c */
            N = DS.SpecVerts->head;
            V = (SVertex *) malloc (sizeof(SVertex));

            glLineWidth(3.0);
            //glBegin(GL_POINTS);
            glColor3f(1.0, 1.0, 0.0);
            //glPointSize(10.0);

            while (N != NULL) {
				double angle;
                memcpy(V, N->data, sizeof(SVertex));
                //glVertex2d(G.vertex[*V].x, G.vertex[*V].y);
                
				// draw a circle with redius 9
				glTranslatef(G.vertex[*V].x, G.vertex[*V].y, 0);
				glBegin(GL_LINE_STRIP);
                for (angle = 0.0; angle <= 2 * M_PI; angle += M_PI / 24) {
                    glVertex2f(9.0 * cos(angle), 9.0 * sin(angle));
                }
                glEnd();
				glTranslatef(-G.vertex[*V].x, -G.vertex[*V].y, 0);
				
                N = N->next;
            }
            //glEnd();

            free(V);

			glLineWidth(1.0);
		
 

			
            /* fazer um togle setavel na draw struct nessas coisas */
            /* Labels */
            if (iface.vlabel_mode){
                for (i = 0; i < G.size; i++) {
                    
                    glColor3ub(0, 0, 0);
                    
                    str[0] = 0;
                    switch (iface.vlabel_mode){
                        case 1:
                            sprintf(str, "%d", i);
                            break;

                        case 2:
                            sprintf(str, "%d", G.vertex[i].id);
                            break;

                        case 3:
                            if (G.vertex[i].label != NULL)
                                sprintf(str, "%s", G.vertex[i].label);
                            break;

                        case 4:
                            sprintf(str, "%g", G.vertex[i].weight);
                            break;
                            
                        case 5:
                            sprintf(str, "%d", G.vertex[i].color);
                            break;
                    }

                    //showMessage(G.vertex[i].x+10, G.vertex[i].y+10, str);
                    glRasterPos2f(G.vertex[i].x+10, G.vertex[i].y+10);
                    //widget->allocation.height-i*fontheight
                    for (l = 0; l < strlen(str); l++)
                        glCallList(fontbase+str[l]);
                }
            }

            if (iface.elabel_mode){
                for (i = 0; i < G.size; i++) {
                    for (j = i+1; j < G.size; j++){

						if (G.edge[i][j]) {
							glColor3ub(0, 0, 0);
							str[0] = 0;

							switch (iface.elabel_mode){
								case 1:
									sprintf(str, "(%d,%d)", i, j);
									break;

								case 2:
									sprintf(str, "(%d,%d)",
											G.vertex[i].id, G.vertex[j].id);
									break;

								case 3:
									if (G.prop[i][j].label != NULL)
										sprintf(str, "%s", G.prop[i][j].label);
									break;

								case 4:
									sprintf(str, "%g", G.prop[i][j].weight);
									break;
									
								case 5:
									sprintf(str, "%d", G.prop[i][j].color);
									break;
							}

							glRasterPos2f((G.vertex[i].x+G.vertex[j].x)/2+5,
									(G.vertex[i].y+G.vertex[j].y)/2+5);
							//widget->allocation.height-i*fontheight
							for (l = 0; l < strlen(str); l++)
								glCallList(fontbase+str[l]);
							}
                    }
                }
            }

            /* Returns to default color */
            glColor3ub(200, 0, 0);
            
            //gtk_gl_area_swapbuffers(GTK_GL_AREA(iface.glarea));
            glFlush();
        }

        if (iface.changed){

            iface.changed = 0;
            
            /* Update status bar */
            update_status();
        }
    }
#endif

    /* Event was handled */
    return TRUE;
}


/* If the widget was resized */
gboolean resize_event_callback(GtkWidget *widget,
        GdkEventConfigure *event, gpointer user_data){

    (void) event;
    (void) user_data;

    /* Get new window dimensions */
    iface.width = event->width;
    iface.height = event->height;
    
    /* Gtk drawing area */
    if ((iface.view_mode == VIEW_MODE_GTK) &&
        GTK_IS_DRAWING_AREA(widget)){
        if (iface.back)
            gdk_pixmap_unref(iface.back);

        iface.back = gdk_pixmap_new(widget->window,
                            widget->allocation.width,
                            widget->allocation.height, -1);
        iface.changed = 1;
    }

#ifdef USE_GTKGL
    /* Gtkgl area */
    else if ((iface.view_mode == VIEW_MODE_OPENGL) &&
             GTK_IS_GL_AREA(widget)){

        if (gtk_gl_area_make_current(GTK_GL_AREA(widget))){
    
            glViewport(0, 0, event->width, event->height);
            viewport[0] = 0;
            viewport[1] = 0;
            viewport[2] = event->width;
            viewport[3] = event->height;
         
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluOrtho2D(0, event->width, 0, event->height);
            glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);

            iface.changed = 1;
        }
    }
#endif

    /* Event was handled */
    return TRUE;
}


/* If a mouse button was pressed */
gboolean button_press_event_callback(GtkWidget *widget,
        GdkEventButton *event, gpointer user_data){
static int Origin, Destiny;
int p;

    (void) user_data;

    /* Gtk drawing area */
    if ((iface.view_mode == VIEW_MODE_GTK) &&
        GTK_IS_DRAWING_AREA(widget)){

        switch (iface.edit_mode) {

            /* Vertex insertion */
            case V_INSERT:
                if ((event->button == 1) &&
                    (event->type == GDK_BUTTON_PRESS)) {
                    AddPoint(&G, (double) event->x,
                                 (double) (iface.height - event->y));
                }
                break;
            
            /* Vertex move */
            case V_CHANGE:
                if (event->button == 1) {
                    if (event->type == GDK_BUTTON_PRESS) {
                        SelectedPoint = get_point(((int)event->x),
                                                  ((int)event->y));
                    } else {
                        SelectedPoint = -1;
                        SelectedCtrlPoint = 0;
                    }
                }
                break;
           
            /* Vertex deletion */
            case V_REMOVE:
                if (event->button == 1) {
                    if (event->type == GDK_BUTTON_PRESS) {
                        if ((p = get_point(((int)event->x),
                                           ((int)event->y))) != ~0) {
                            ClearSpecEdges(&DS);
                            ClearSpecVerts(&DS);
                            RemovePoint(&G, p);
                        }
                        if (SelectedCtrlPoint) {
							SelectedCtrlPoint = 0;
						}
                    }
                }
                break;
           
            /* Edge insertion */
            case E_INSERT:
                if (event->button == 1) {
                    if (event->type == GDK_BUTTON_PRESS) {
                        if (IsDestiny) {
                            if (((Destiny = get_point(((int)event->x),
                                            ((int)event->y))) == ~0) ||
                                (Destiny == Origin)) {
                                iface.help_string = EdInsString[0];
                                IsDestiny = 0;
                                gdk_window_set_cursor(widget->window,
                                        iface.cursor[0]);
                                iface.cursor_indx = 0;
                                break;
                            }
                            if (G.type == DIGRAPH) {
                                AddArc(&G, Origin, Destiny);
                            } else {
                                AddEdge(&G, Origin, Destiny);
                            }
                            IsDestiny = 0;
                            gdk_window_set_cursor(widget->window,
                                    iface.cursor[0]);
                            iface.cursor_indx = 0;
                            iface.help_string = EdInsString[0];
                        } else {
                            if ((Origin = get_point(((int)event->x),
                                          ((int)event->y))) == ~0) {
                                break;
                            }
                            IsDestiny = 1;
                            gdk_window_set_cursor(widget->window,
                                    iface.cursor[1]);
                            iface.cursor_indx = 1;
                            iface.help_string = EdInsString[1];
                        }
                    }
                } 
                break;
            
            /* Edge edition */
            case E_CHANGE:
                break;
            
            /* Edge deletion */
            case E_REMOVE:
                 if (event->button == 1) {
                    if (event->type == GDK_BUTTON_PRESS) {
                        if (IsDestiny) {
                            if (((Destiny = get_point(((int)event->x),
                                            ((int)event->y))) == ~0) ||
                                (Destiny == Origin)) {
                                iface.help_string = EdInsString[0];
                                IsDestiny = 0;
                                //gdk_window_set_cursor(widget->window,
                                //        iface.cursor[0]);
                                //iface.cursor_indx = 0;
                                break;
                            }
                            ClearSpecEdges(&DS);
                            ClearSpecVerts(&DS);
                            if (G.type == DIGRAPH) {
                                RemoveArc(&G, Origin, Destiny);
                            } else {
                                RemoveEdge(&G, Origin, Destiny);
                            }
                            IsDestiny = 0;
                            //gdk_window_set_cursor(widget->window,
                            //        iface.cursor[0]);
                            //iface.cursor_indx = 0;
                            iface.help_string = EdInsString[0];
                        } else {
                            if ((Origin = get_point(((int)event->x),
                                          ((int)event->y))) == ~0) {
                                break;
                            }
                            IsDestiny = 1;
                            //gdk_window_set_cursor(widget->window,
                            //        iface.cursor[1]);
                            //iface.cursor_indx = 1;
                            iface.help_string = EdInsString[1];
                        }
                    }
                } 
                break;

            /* Vertex edition */
            case V_EDIT :
                if (event->button == 1) {
                    if (event->type == GDK_BUTTON_PRESS) {
                        if ((p = get_point(((int)event->x),
                                           ((int)event->y))) != ~0) {
                            iface.vertex_indx = p;
                            load_vertex_properties();
                            factory_show_dialog(iface.vprop_dialog);
                        }
                    }
                }
                break;
                
            /* Edge edition */
            case E_EDIT:
                 if (event->button == 1) {
                    if (event->type == GDK_BUTTON_PRESS) {
                        if (IsDestiny) {
                            if (((Destiny = get_point(((int)event->x),
                                            ((int)event->y))) == ~0) ||
                                (Destiny == Origin)) {
                                iface.help_string = EdEditString[0];
                                IsDestiny = 0;
                                //gdk_window_set_cursor(widget->window,
                                //        iface.cursor[4]);
                                //iface.cursor_indx = 4;
                                break;
                            }
                            iface.edge_indx[1] = Destiny;
                            load_edge_properties();
                            factory_show_dialog(iface.eprop_dialog);
                            IsDestiny = 0;
                            //gdk_window_set_cursor(widget->window,
                            //        iface.cursor[4]);
                            //iface.cursor_indx = 4;
                            iface.help_string = EdEditString[0];
                        } else {
                            if ((Origin = get_point(((int)event->x),
                                          ((int)event->y))) == ~0) {
                                break;
                            }
                            IsDestiny = 1;
                            //gdk_window_set_cursor(widget->window,
                            //        iface.cursor[1]);
                            //iface.cursor_indx = 1;
                            iface.help_string = EdEditString[1];
                            iface.edge_indx[0] = Origin;
                        }
                    }
                } 
                break;
           
            case NOTHING:
                break;
        }

        iface.changed = 1;
    }

#ifdef USE_GTKGL
    /* Gtkglarea */
    if ((iface.view_mode == VIEW_MODE_OPENGL) &&
        GTK_IS_GL_AREA(widget)){
            
        if (gtk_gl_area_make_current(GTK_GL_AREA(widget))){
            switch (iface.edit_mode) {

                /* Vertex insertion */
                case V_INSERT:
                    if ((event->button == 1) &&
                        (event->type == GDK_BUTTON_PRESS)) {
                        AddPoint(&G, (double) event->x,
                                (double) (iface.height - event->y));
                    }
                    break;
                
                /* Vertex move */
                case V_CHANGE:
                    if (event->button == 1) {
                        if (event->type == GDK_BUTTON_PRESS) {
                            SelectedPoint = Pick(((int)event->x),
                                                 ((int)event->y));
                        } else {
                            SelectedPoint = -1;
                            SelectedCtrlPoint = 0;
                        }
                    }
                    break;
               
                /* Vertex deletion */
                case V_REMOVE:
                    if (event->button == 1) {
                        if (event->type == GDK_BUTTON_PRESS) {
                            if ((p = Pick(((int)event->x),
                                          ((int)event->y))) != ~0) {
                                ClearSpecEdges(&DS);
                                ClearSpecVerts(&DS);
                                RemovePoint(&G, p);
                            }else {
                                SelectedPoint = -1;
                                SelectedCtrlPoint = 0;
                            }
                        }
                    }
                    break;
                
                /* Edge insertion */
                case E_INSERT:
                    if (event->button == 1) {
                        if (event->type == GDK_BUTTON_PRESS) {
                            if (IsDestiny) {
                                if (((Destiny = Pick(((int)event->x),
                                                ((int)event->y))) == ~0) ||
                                    (Destiny == Origin)) {
                                    iface.help_string = EdInsString[0];
                                    IsDestiny = 0;
                                    gdk_window_set_cursor(widget->window,
                                            iface.cursor[0]);
                                    iface.cursor_indx = 0;
                                    break;
                                }
                                ClearSpecEdges(&DS);
                                ClearSpecVerts(&DS);
                                if (G.type == DIGRAPH) {
                                    AddArc(&G, Origin, Destiny);
                                } else {
                                    AddEdge(&G, Origin, Destiny);
                                }
                                IsDestiny = 0;
                                gdk_window_set_cursor(widget->window,
                                        iface.cursor[0]);
                                iface.cursor_indx = 0;
                                iface.help_string = EdInsString[0];
                            } else {
                                if ((Origin = Pick(((int)event->x),
                                              ((int)event->y))) == ~0) {
                                    break;
                                }
                                IsDestiny = 1;
                                gdk_window_set_cursor(widget->window,
                                        iface.cursor[1]);
                                iface.cursor_indx = 1;
                                iface.help_string = EdInsString[1];
                            }
                        }
                    } 
                    break;
                
                /* Edge edition */
                case E_CHANGE:
                    break;
                
                /* Edge deletion */
                case E_REMOVE:
                     if (event->button == 1) {
                        if (event->type == GDK_BUTTON_PRESS) {
                            if (IsDestiny) {
                                if (((Destiny = Pick(((int)event->x),
                                                ((int)event->y))) == ~0) ||
                                    (Destiny == Origin)) {
                                    iface.help_string = EdInsString[0];
                                    IsDestiny = 0;
                                    //gdk_window_set_cursor(widget->window,
                                    //        iface.cursor[0]);
                                    //iface.cursor_indx = 0;
                                    break;
                                }
                                if (G.type == DIGRAPH) {
                                    RemoveArc(&G, Origin, Destiny);
                                } else {
                                    RemoveEdge(&G, Origin, Destiny);
                                }
                                IsDestiny = 0;
                                //gdk_window_set_cursor(widget->window,
                                //        iface.cursor[0]);
                                //iface.cursor_indx = 0;
                                iface.help_string = EdInsString[0];
                            } else {
                                if ((Origin = Pick(((int)event->x),
                                              ((int)event->y))) == ~0) {
                                    break;
                                }
                                IsDestiny = 1;
                                //gdk_window_set_cursor(widget->window,
                                //        iface.cursor[1]);
                                //iface.cursor_indx = 1;
                                iface.help_string = EdInsString[1];
                            }
                        }
                    } 
                    break;

                /* Vertex edition */
                case V_EDIT:
                    if (event->button == 1) {
                        if (event->type == GDK_BUTTON_PRESS) {
                            if ((p = Pick(((int)event->x),
                                          ((int)event->y))) != ~0) {
                                iface.vertex_indx = p;
                                load_vertex_properties();
                                factory_show_dialog(iface.vprop_dialog);
                            }
                        }
                    }
                    break;

                /* Edge edition */
                case E_EDIT:
                     if (event->button == 1) {
                        if (event->type == GDK_BUTTON_PRESS) {
                            if (IsDestiny) {
                                if (((Destiny = Pick(((int)event->x),
                                                ((int)event->y))) == ~0) ||
                                    (Destiny == Origin)) {
                                    iface.help_string = EdEditString[0];
                                    IsDestiny = 0;
                                    gdk_window_set_cursor(widget->window,
                                            iface.cursor[4]);
                                    iface.cursor_indx = 4;
                                    break;
                                }
                                iface.edge_indx[1] = Destiny;
								if (G.edge[iface.edge_indx[0]]
										[iface.edge_indx[1]]){
	                                load_edge_properties();
    	                            factory_show_dialog(iface.eprop_dialog);
								}
                                IsDestiny = 0;
                                //gdk_window_set_cursor(widget->window,
                                //        iface.cursor[4]);
                                //iface.cursor_indx = 4;
                                iface.help_string = EdEditString[0];
                            } else {
                                if ((Origin = Pick(((int)event->x),
                                              ((int)event->y))) == ~0) {
                                    break;
                                }
                                iface.edge_indx[0] = Origin;
                                IsDestiny = 1;
                                //gdk_window_set_cursor(widget->window,
                                //        iface.cursor[1]);
                                //iface.cursor_indx = 1;
                                iface.help_string = EdEditString[1];
                            }
                        }
                    } 
                    break;
               
                case NOTHING:
                    break;
            }
            iface.changed = 1;
        }
    }    
#endif

	/* Clear the "mess" string */
    iface.status_string[0] = 0;
	
    /* Redraws screen */
    gtk_widget_queue_draw(widget);

    /* Event was handled */
    return TRUE;
}


/* If the mouse was moved */
gboolean motion_notify_event_callback(GtkWidget *widget,
        GdkEventMotion *event, gpointer user_data){
int i;

    (void) user_data;

    /* Gtk drawing area */
    if ((iface.view_mode == VIEW_MODE_GTK) &&
        GTK_IS_DRAWING_AREA(widget)){
        
        if (SelectedPoint != (~0)) {
        
            G.vertex[SelectedPoint].x = (double) event->x;
            G.vertex[SelectedPoint].y = (double) iface.height - event->y;
            /* Atualizar control points das arestas que incidem
             * neste vertice */
            for (i = 0; i < G.size; i++) {
                /* Da pra fazer melhor */
                if (G.edge[SelectedPoint][i]) {
                    UpdateCtrlPoints(&G, (int) SelectedPoint, i, LINES);
                } else if (G.edge[i][SelectedPoint]){
                    UpdateCtrlPoints(&G, i, (int) SelectedPoint, LINES);
                }
            }
        } else if (SelectedCtrlPoint) {
            G.prop[SCP[0]][SCP[1]].ctrlpoints[SCP[2]][X] 
                       = (double) event->x;
            G.prop[SCP[0]][SCP[1]].ctrlpoints[SCP[2]][Y] 
                       = (double) iface.height - event->y;
        } 

		
        iface.changed = 1;
    }

#ifdef USE_GTKGL
    /* Gtkgl area */
    if ((iface.view_mode == VIEW_MODE_OPENGL) &&
        GTK_IS_GL_AREA(widget)){
    GLdouble objx, objy, objz;

        if (gtk_gl_area_make_current(GTK_GL_AREA(widget))){

            if (SelectedPoint != (~0)) {
            
                gluUnProject(event->x, iface.height - event->y, 0.95,
                        modelMatrix, projMatrix, viewport,
                        &objx, &objy, &objz);
               
                G.vertex[SelectedPoint].x = (double) objx;
                G.vertex[SelectedPoint].y = (double) objy;
                /* Atualizar control points das arestas que incidem
                 * neste vertice */
                for (i = 0; i < G.size; i++) {
                    /* Da pra fazer melhor */
                    if (G.edge[SelectedPoint][i]) {
                        UpdateCtrlPoints(&G, (int) SelectedPoint, i, LINES);
                    } else if (G.edge[i][SelectedPoint]){
                        UpdateCtrlPoints(&G, i, (int) SelectedPoint, LINES);
                    }
                }
            }

else if (SelectedCtrlPoint) {
                gluUnProject(event->x, iface.height - event->y, 0.95,
                        modelMatrix, projMatrix, viewport,
                        &objx, &objy, &objz);

            G.prop[SCP[0]][SCP[1]].ctrlpoints[SCP[2]][X] 
                       = (double) objx;
            G.prop[SCP[0]][SCP[1]].ctrlpoints[SCP[2]][Y] 
                       = (double) objy;
        } 

			

            iface.changed = 1;
        }
    }
#endif

    /* Redraws screen */
    gtk_widget_queue_draw(widget);

    /* Event was handled */
    return TRUE;
}    


/* If the mouse enters the drawing area */
gboolean enter_notify_event_callback(GtkWidget *widget,
        GdkEventMotion *event, gpointer user_data){

    (void) widget;
    (void) event;
    (void) user_data;

    /* Event was handled */
    return TRUE;
}


/* If the mouse leaves the drawing area */
gboolean leave_notify_event_callback(GtkWidget *widget,
        GdkEventMotion *event, gpointer user_data){

    (void) widget;
    (void) event;
    (void) user_data;

    /* Event was handled */
    return TRUE;
}


/* **** Auxiliary opengl functions **** */
#ifdef USE_GTKGL
GLuint Pick(int x, int y) {
int hits;
int i,j,k;

    (void) glRenderMode(GL_SELECT);
    glInitNames();
    glPushName(~0);
    glMatrixMode(GL_PROJECTION);

    glPushMatrix();
    glLoadIdentity();
    gluPickMatrix(x, iface.height - y, 20.0, 20.0, viewport);
  
    gluOrtho2D(0, iface.width, 0, iface.height);
    glMatrixMode(GL_MODELVIEW);

    SelectControlPoints();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    hits = glRenderMode(GL_RENDER);

    if (hits) {
        return SelectBuffer[3];
    } else {


    /* Ok, the point is not a vertex */

	if (ShowCtrlPts) {		
#define POINT_SEL_TOL 10
        for (i = 0; i < G.size; i++) {
            for (j = 0; j < G.size; j++) {
                for (k = 0; k < 4; k++) {

                    if ((G.prop[i][j].ctrlpoints[k][X]) >= (x-POINT_SEL_TOL) &&
                        (G.prop[i][j].ctrlpoints[k][X]) <= (x+POINT_SEL_TOL) &&
                        (G.prop[i][j].ctrlpoints[k][Y]) >= (iface.height 
                                                     - y-POINT_SEL_TOL) &&
                        (G.prop[i][j].ctrlpoints[k][Y]) <= (iface.height 
                                                     - y+POINT_SEL_TOL)) {
                        /* I found a control point */
                        SCP[0] = i;
                        SCP[1] = j;
                        SCP[2] = k;
                        SelectedCtrlPoint = 1;
                        return ~0;
    				}
	    		}
    		}
        }
	}

	SelectedCtrlPoint = 0;

    return ~0;
    }
}


void SelectControlPoints(void) {
int i;
GLfloat P[2];

    for (i = 0; i < G.size; i++) {
        glLoadName(i);
        
        glBegin(GL_POINTS);
        
        P[0] = (GLfloat) G.vertex[i].x;
        P[1] = (GLfloat) G.vertex[i].y;
        glVertex2fv(P);
 
        glEnd();
    }
}


/* Change the color */
void ChooseGlColor(Graph *g, int i){

    if ((g->vertex[i].color < 0) || (g->vertex[i].color > LASTCOLOR)){
         fprintf(stderr, "Unexpected color (%d) of vertex %d\n",
                     g->vertex[i].color, i);
         exit(1);
    }

    glColor3ub(color_def[g->vertex[i].color].r,
               color_def[g->vertex[i].color].g,
               color_def[g->vertex[i].color].b);
}


void ChooseGlEdgeColor(Graph *g, int i, int j){

    if ((g->prop[i][j].color < 0) || (g->prop[i][j].color > LASTCOLOR)){
         fprintf(stderr, "Unexpected color (%d) of edge (%d,%d)\n",
                     g->prop[i][j].color, i, j);
         exit(1);
    }

	if (g->prop[i][j].color > 0)
	    glColor3ub(color_def[g->prop[i][j].color].r,
    	           color_def[g->prop[i][j].color].g,
        	       color_def[g->prop[i][j].color].b);
	else	
       	glColor3f(1.0, 1.0, 1.0);
}
#endif


/* **** Auxiliary gtk functions **** */
int get_point(int x, int y){
int i,j,k;
#define POINT_SEL_TOL 10

    for (i = 0; i < G.size; i++){
        if ((G.vertex[i].x) >= (x-POINT_SEL_TOL) &&
            (G.vertex[i].x) <= (x+POINT_SEL_TOL) &&
            (G.vertex[i].y) >= (iface.height - y-POINT_SEL_TOL) &&
            (G.vertex[i].y) <= (iface.height - y+POINT_SEL_TOL)){
            /* I found a vertex */
            return i;
        }
    }
    /* Ok, the point is not a vertex */

	if (ShowCtrlPts) {		
    
        for (i = 0; i < G.size; i++) {
            for (j = 0; j < G.size; j++) {
                for (k = 0; k < 4; k++) {

                    if ((G.prop[i][j].ctrlpoints[k][X]) >= (x-POINT_SEL_TOL) &&
                        (G.prop[i][j].ctrlpoints[k][X]) <= (x+POINT_SEL_TOL) &&
                        (G.prop[i][j].ctrlpoints[k][Y]) >= (iface.height 
                                                     - y-POINT_SEL_TOL) &&
                        (G.prop[i][j].ctrlpoints[k][Y]) <= (iface.height 
                                                     - y+POINT_SEL_TOL)) {

                        /* I found a control point */
                        SCP[0] = i;
                        SCP[1] = j;
                        SCP[2] = k;
                        SelectedCtrlPoint = 1;
                        return -1;
    				}
	    		}
		    }
        }
	}

	SelectedCtrlPoint = 0;
	
    return -1;
}


/* Change the color */
void change_color(Graph *g, int i, GdkGC *gc){

    if ((g->vertex[i].color < 0) || (g->vertex[i].color > LASTCOLOR)){
         fprintf(stderr, "Unexpected color (%d) of vertex %d\n",
                     g->vertex[i].color, i);
         exit(1);
    }

    gdk_gc_set_foreground(gc, &(gdk_color[g->vertex[i].color]));
}


void change_edge_color(Graph *g, int i, int j, GdkGC *gc){

    if ((g->prop[i][j].color < 0) || (g->prop[i][j].color > LASTCOLOR)){
         fprintf(stderr, "Unexpected color (%d) of edge (%d,%d)\n",
                     g->prop[i][j].color, i, j);
         exit(1);
    }

	if (g->prop[i][j].color > 0)
	    gdk_gc_set_foreground(gc, &(gdk_color[g->prop[i][j].color]));
	else	
        gdk_gc_set_foreground(gc, &lines_color);
}


/* Splines points */
#if 0
double hornbez(int degree, int *coeff, double t){
int i, n_choose_i;
double fact, t1, aux;

    t1 = 1.0 - t;
    fact = 1.0;
    n_choose_i = 1;
    aux = ((double)coeff[0])*t1;

    for (i = 1; i < degree; i++){
        fact = fact*t;
        n_choose_i = n_choose_i * (degree-i+1)/i;
        aux = (aux + fact*n_choose_i*((double)coeff[i]))*t1;
    }

    aux = aux + fact*t*((double)coeff[degree]);

    return aux;
}
#endif

/* Draws a spline */
void gdk_draw_spline(GdkWindow *window, GdkGC *gc, int degree,
        int *ctrl_x, int *ctrl_y){
double t, delt;
int i, dx, dy, npoints;
int lastx, lasty, x, y;

    dx = abs(ctrl_x[degree] - ctrl_x[0])+1;
    dy = abs(ctrl_y[degree] - ctrl_y[0])+1;
    if (dx > dy)
        npoints = dx;
    else
        npoints = dy;

    delt = 1.0/npoints;
    t = 0.0;
    lastx = hornbez(degree, ctrl_x, t);
    lasty = hornbez(degree, ctrl_y, t);
    
    for (i = 0; i < npoints; i++){
        x = hornbez(degree, ctrl_x, t);
        y = hornbez(degree, ctrl_y, t);
        
        gdk_draw_line(window, gc, lastx, lasty, x, y);       
        lastx = x;
        lasty = y;
        
        t = t + delt;
    }
}

#if 0
/* Returns nth point of a spline */
void spline_point(int degree,
        int *ctrl_x, int *ctrl_y, int *x, int *y, int n){
double t, delt;
int i, dx, dy, npoints;

    dx = abs(ctrl_x[degree] - ctrl_x[0])+1;
    dy = abs(ctrl_y[degree] - ctrl_y[0])+1;
    if (dx > dy)
        npoints = dx;
    else
        npoints = dy;

    delt = 1.0/npoints;
    t = 0.0;
    
    for (i = 0; i < n; i++){
        (*x) = hornbez(degree, ctrl_x, t);
        (*y) = hornbez(degree, ctrl_y, t);
        t = t + delt;
    }
}
#endif

/* **** Gtk events handle functions **** */
void on_edit_mode_activate(GtkMenuItem *menuitem, gpointer user_data){

    (void) menuitem;
    iface.edit_mode = ((int) user_data);

    switch (((int) user_data)){
        case NOTHING:
            if (iface.view_mode == VIEW_MODE_GTK)
                gdk_window_set_cursor(iface.drawingarea->window,
                        iface.cursor[0]);
            else if (iface.view_mode == VIEW_MODE_OPENGL)
                gdk_window_set_cursor(iface.glarea->window,
                        iface.cursor[0]);
            iface.cursor_indx = 0;
            iface.help_string = NULL;
            break;
        case V_INSERT:
            if (iface.view_mode == VIEW_MODE_GTK)
                gdk_window_set_cursor(iface.drawingarea->window,
                        iface.cursor[0]);
            else if (iface.view_mode == VIEW_MODE_OPENGL)
                gdk_window_set_cursor(iface.glarea->window,
                        iface.cursor[0]);
            iface.cursor_indx = 0;
            iface.help_string = VeInsString;
            break;
        case E_INSERT:
            if (iface.view_mode == VIEW_MODE_GTK)
                gdk_window_set_cursor(iface.drawingarea->window,
                        iface.cursor[0]);
            else if (iface.view_mode == VIEW_MODE_OPENGL)
                gdk_window_set_cursor(iface.glarea->window,
                        iface.cursor[0]);
            iface.cursor_indx = 0;
            iface.help_string = EdInsString[0];
            break;
        case V_CHANGE:
            if (iface.view_mode == VIEW_MODE_GTK)
                gdk_window_set_cursor(iface.drawingarea->window,
                        iface.cursor[2]);
            else if (iface.view_mode == VIEW_MODE_OPENGL)
                gdk_window_set_cursor(iface.glarea->window,
                        iface.cursor[2]);
            iface.cursor_indx = 2;
            iface.help_string = VeMovString;
            break;
        case E_CHANGE:
            if (iface.view_mode == VIEW_MODE_GTK)
                gdk_window_set_cursor(iface.drawingarea->window,
                        iface.cursor[2]);
            else if (iface.view_mode == VIEW_MODE_OPENGL)
                gdk_window_set_cursor(iface.glarea->window,
                        iface.cursor[2]);
            iface.cursor_indx = 2;
            iface.help_string = NULL;
            break;
        case V_REMOVE :
            if (iface.view_mode == VIEW_MODE_GTK)
                gdk_window_set_cursor(iface.drawingarea->window,
                        iface.cursor[3]);
            else if (iface.view_mode == VIEW_MODE_OPENGL)
                gdk_window_set_cursor(iface.glarea->window,
                        iface.cursor[3]);
            iface.cursor_indx = 3;
            iface.help_string = VeDelString;
            break;
        case E_REMOVE :
            if (iface.view_mode == VIEW_MODE_GTK)
                gdk_window_set_cursor(iface.drawingarea->window,
                        iface.cursor[3]);
            else if (iface.view_mode == VIEW_MODE_OPENGL)
                gdk_window_set_cursor(iface.glarea->window,
                        iface.cursor[3]);
            iface.cursor_indx = 3;
            iface.help_string = EdInsString[0];
            break;
        case V_EDIT :
            if (iface.view_mode == VIEW_MODE_GTK)
                gdk_window_set_cursor(iface.drawingarea->window,
                        iface.cursor[4]);
            else if (iface.view_mode == VIEW_MODE_OPENGL)
                gdk_window_set_cursor(iface.glarea->window,
                        iface.cursor[4]);
            iface.cursor_indx = 4;
            iface.help_string = VeEditString;
            break;
        case E_EDIT :
            if (iface.view_mode == VIEW_MODE_GTK)
                gdk_window_set_cursor(iface.drawingarea->window,
                        iface.cursor[4]);
            else if (iface.view_mode == VIEW_MODE_OPENGL)
                gdk_window_set_cursor(iface.glarea->window,
                        iface.cursor[4]);
            iface.cursor_indx = 4;
            iface.help_string = EdEditString[0];
            break;

    }

    update_status();
}


void on_reset_graph_item_activate(GtkMenuItem *menuitem,
        gpointer user_data){

    (void) menuitem;
    (void) user_data;
    
    if (G.type == DIGRAPH) {
        InitGraph(&G, DIGRAPH);
    } else {
        InitGraph(&G, GRAPH);
    }
    ClearDrawStruct(&DS);

    iface.status_string[0] = 0;

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
}


void on_reset_marked_edges_item_activate(GtkMenuItem *menuitem,
        gpointer user_data){

    (void) menuitem;
    (void) user_data;

    ClearSpecEdges(&DS);

    iface.status_string[0] = 0;

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
}


void on_reset_marked_vertices_item_activate(GtkMenuItem *menuitem,
        gpointer user_data){

    (void) menuitem;
    (void) user_data;

    ClearSpecVerts(&DS);
    
    iface.status_string[0] = 0;

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
}



void on_complete_graph_item_activate(GtkMenuItem *menuitem,
        gpointer user_data){

    (void) menuitem;
    (void) user_data;

    gtk_notebook_set_page(GTK_NOTEBOOK(iface.graph_dialog_notebook), 0);
    factory_show_dialog(iface.graph_dialog);
}


void on_bipartite_graph_item_activate(GtkMenuItem *menuitem,
        gpointer user_data){

    (void) menuitem;
    (void) user_data;
    
    gtk_notebook_set_page(GTK_NOTEBOOK(iface.graph_dialog_notebook), 1);
    factory_show_dialog(iface.graph_dialog);
}


void on_random_graph_item_activate(GtkMenuItem *menuitem,
        gpointer user_data){

    (void) menuitem;
    (void) user_data;

    gtk_notebook_set_page(GTK_NOTEBOOK(iface.graph_dialog_notebook), 2);
    factory_show_dialog(iface.graph_dialog);
}


void on_regular_graph_item_activate(GtkMenuItem *menuitem,
        gpointer user_data){

    (void) menuitem;
    (void) user_data;

    gtk_notebook_set_page(GTK_NOTEBOOK(iface.graph_dialog_notebook), 3);
    factory_show_dialog(iface.graph_dialog);
}


void on_star_graph_item_activate(GtkMenuItem *menuitem,
        gpointer user_data){

    (void) menuitem;
    (void) user_data;

    gtk_notebook_set_page(GTK_NOTEBOOK(iface.graph_dialog_notebook), 4);
    factory_show_dialog(iface.graph_dialog);
}


void on_mitzenmacher_random_graph_item_activate(GtkMenuItem *menuitem,
        gpointer user_data){

    (void) menuitem;
    (void) user_data;

    gtk_notebook_set_page(GTK_NOTEBOOK(iface.graph_dialog_notebook), 5);
    factory_show_dialog(iface.graph_dialog);
}


void on_fit_to_window_item_activate(GtkMenuItem *menuitem,
        gpointer user_data){
int j, k;

    (void) menuitem;
    (void) user_data;

    /* Change point coordinates */
    FitToWindow(&G, iface.width, iface.height);

    /* Update drawing */
    ClearDrawStruct(&DS);
    for (j = 0; j < G.size; j++) {
        for (k = 0; k < G.size; k++) {
            if ((G.edge[j][k]) && (j != k)) {
                UpdateCtrlPoints(&G, j, k, LINES); 
            }
         }
    }

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
}


void on_draw_as_circle_item_activate(GtkMenuItem *menuitem,
        gpointer user_data){
int j, k;

    (void) menuitem;
    (void) user_data;

    /* Change point coordinates */
    RedrawAsCircle(&G, iface.width, iface.height);

    /* Update drawing */
    ClearDrawStruct(&DS);
    for (j = 0; j < G.size; j++) {
        for (k = 0; k < G.size; k++) {
            if ((G.edge[j][k]) && (j != k)) {
                UpdateCtrlPoints(&G, j, k, LINES); 
            }
         }
    }

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
}


void on_draw_using_dot_item_activate(GtkMenuItem *menuitem,
        gpointer user_data){

    (void) menuitem;
    (void) user_data;

    if (!CallDot(&G))
        showmsg(call_error);
    
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
}


void on_draw_using_neato_item_activate(GtkMenuItem *menuitem,
        gpointer user_data){

    (void) menuitem;
    (void) user_data;

    if (!CallNeato(&G))
        showmsg(call_error);

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
}


void on_toggle_graph_item_activate(GtkMenuItem *menuitem,
        gpointer user_data){
int j, k;

    (void) menuitem;
    (void) user_data;
    
    if (G.type == GRAPH) {
        //glutSetMenu(menu_draw);
        //glutChangeToMenuEntry(5, "Insert Arcs", SUB_ITEM_14);
        //glutChangeToMenuEntry(6, "Move Arcs", SUB_ITEM_15);
        //glutChangeToMenuEntry(7, "Remove Arcs", SUB_ITEM_16);
        //glutChangeToMenuEntry(6, "Remove Arcs", SUB_ITEM_16);
        G.type = DIGRAPH;
        for (j = 0; j < G.size; j++) {
            for (k = 0; k < G.size; k++) {
                if ((G.edge[j][k]) && (j != k)) {
                    UpdateCtrlPoints(&G, j, k, LINES); 
                }
             }
        }
    } else {
        //glutSetMenu(menu_draw);
        //glutChangeToMenuEntry(5, "Insert Edges", SUB_ITEM_14);
        //glutChangeToMenuEntry(6, "Move Edges", SUB_ITEM_15);
        //glutChangeToMenuEntry(7, "Remove Edges", SUB_ITEM_16);
        //glutChangeToMenuEntry(6, "Remove Edges", SUB_ITEM_16);
        G.type = GRAPH;
        for (j = 0; j < G.size; j++) {
            for (k = j+1; k < G.size; k++) {
                if ((G.edge[j][k]) && (j != k)) {
                    UpdateCtrlPoints(&G, j, k, LINES); 
                }
            }
        }
    }

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
}

void on_show_ctrlpts_item_activate(GtkMenuItem *menuitem,
        gpointer user_data){

    (void) menuitem;
    (void) user_data;

	if (ShowCtrlPts) { 
        ShowCtrlPts = 0;
	} else {
		ShowCtrlPts = 1;
	}

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
	
}



void on_test_algorithm(GtkMenuItem *menuitem, gpointer user_data){
Algor *node;
int value;

    (void) menuitem;
    
    /* Gets algorithm index */
    value = (*((int *) user_data));

    if ((value > 0) && (value <= L.size)) {
        node = L.first;
        while (--value) {
            node = node->next;
        }
        node->alg(&G, iface.status_string);
    }
    
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
}


void on_result_algorithm(GtkMenuItem *menuitem, gpointer user_data){
AlgorR *node;
List E, V;
int value;

    (void) menuitem;

    /* Gets algorithm index */
    value = (*((int *) user_data));

    InitList(&E);
    InitList(&V);
    
    if ((value > 0) && (value <= RL.size)) {
        node = RL.first;
        while (--value) {
            node = node->next;
        }
        node->alg(&G, iface.status_string, &E, &V);
    }

    // poe na DS as arestas e vertices especiais
    // alias uma boa ideia e' padronizar e por a MessString la' tbem.

    SetSpecEdges(&DS, &E);
    SetSpecVerts(&DS, &V);

    CleanList(&E);
    CleanList(&V);
    
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
}

void on_step_algorithm(GtkMenuItem *menuitem, gpointer user_data){
AlgorSS *node;
List E, V;
int value;

    (void) menuitem;

    /* Gets algorithm index */
    value = (*((int *) user_data));

    InitList(&E);
    InitList(&V);
    
    InitStepStruct();
    
    if ((value > 0) && (value <= SSL.size)) {
        node = SSL.first;
        while (--value) {
            node = node->next;
        }
        // ------> tem que pegar da iface ao inves de passar 0 <------- //
        node->alg(&G, iface.status_string, 0, &E, &V);
    }
    
    ShowStep(&G, &E, &V, 0);    
    
    SetSpecEdges(&DS, &E);
    SetSpecVerts(&DS, &V);

    CleanList(&E);
    CleanList(&V);
        
    execstep();

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
}


void on_transform_algorithm(GtkMenuItem *menuitem, gpointer user_data){
Algor *node;
int value, j, k;

    (void) menuitem;

    /* Gets algorithm index */
    value = (*((int *) user_data));

    if ((value > 0) && (value <= TL.size)) {
        node = TL.first;
        while (--value) {
            node = node->next;
        }
        node->alg(&G, iface.status_string);
    }

    /* Change point coordinates */   
    if ((node->flags == PGIN_FLAG_REDRAWASCIRCLE))
        RedrawAsCircle(&G, iface.width, iface.height);
    else if ((node->flags == PGIN_FLAG_REDRAWWITHCENTEREDVERTEX))
        RedrawWithCenteredVertex(&G, iface.width, iface.height);

    /* Update drawing */
    ClearDrawStruct(&DS);
    for (j = 0; j < G.size; j++) {
        for (k = 0; k < G.size; k++) {
            if ((G.edge[j][k]) && (j != k)) {
                UpdateCtrlPoints(&G, j, k, LINES); 
            }
         }
    }

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
}


void on_search_algorithm(GtkMenuItem *menuitem, gpointer user_data){
AlgorS *node;
List E, V;
int value;

    (void) menuitem;

    /* Gets algorithm index */
    value = (*((int *) user_data));

    InitList(&E);
    InitList(&V);
    
    if ((value > 0) && (value <= SL.size)) {
        node = SL.first;
        while (--value) {
            node = node->next;
        }
        // ------> tem que pegar da iface ao inves de passar 0 <------- //
        node->alg(&G, iface.status_string, 0, &E, &V);
    }

    // poe na DS as arestas e vertices especiais
    // alias uma boa ideia e' padronizar e por a MessString la' tbem.

    SetSpecEdges(&DS, &E);
    SetSpecVerts(&DS, &V);

    CleanList(&E);
    CleanList(&V);
    
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
}


void on_new_item_activate(GtkWidget *widget, gpointer user_data){

    (void) widget;
    (void) user_data;

    /* Reset file name */
    iface.filename = NULL;

    /* Reset graph */
    if (G.type == DIGRAPH) {
        InitGraph(&G, DIGRAPH);
    } else {
        InitGraph(&G, GRAPH);
    }
    ClearDrawStruct(&DS);

    change_window_title();

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
}


void on_open_item_activate(GtkWidget *widget, gpointer user_data){

    (void) widget;
    (void) user_data;

    factory_show_dialog(iface.open_filesel);
}


void on_save_item_activate(GtkWidget *widget, gpointer user_data){
    
    (void) widget;
    (void) user_data;

    if (iface.filename)
        save_file();
    else
        factory_show_dialog(iface.save_filesel);
}


void on_saveas_item_activate(GtkWidget *widget, gpointer user_data){
    
    (void) widget;
    (void) user_data;
    
    factory_show_dialog(iface.save_filesel);
}

void on_firststep_item_activate(GtkWidget *widget, gpointer user_data){
List E, V;
    
    (void) widget;
    (void) user_data;
    
    if (GetExecStateStep() == EXECUTING_STEPS)
	    SetExecStateStep(STOP_EXEC_STEPS);
    
    InitList(&E);
    InitList(&V);
    
    FirstStep(&G, &E, &V);
    
    SetSpecEdges(&DS, &E);
    SetSpecVerts(&DS, &V);

    CleanList(&E);
    CleanList(&V);

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
}

void on_previousstep_item_activate(GtkWidget *widget, gpointer user_data){
List E, V;
    
    (void) widget;
    (void) user_data;
    
    if (GetExecStateStep() == EXECUTING_STEPS)
	    SetExecStateStep(STOP_EXEC_STEPS);
    
    InitList(&E);
    InitList(&V);
    
    PreviousStep(&G, &E, &V);
    
    SetSpecEdges(&DS, &E);
    SetSpecVerts(&DS, &V);

    CleanList(&E);
    CleanList(&V);

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
}

void execstep(void) {
List E, V;
int i;

    if (GetExecStateStep() == EXECUTING_STEPS)
    	return;
    
    SetExecStateStep(START_EXEC_STEPS);
    
    InitList(&E);
    InitList(&V);
    
    for(i =0; i < GetLastStep(); i++) {
    
        if (GetExecStateStep() == EXECUTING_STEPS) {
    
	    	NextStep(&G, &E, &V);
	    
		    SetSpecEdges(&DS, &E);
	    	SetSpecVerts(&DS, &V);
	    	
	    	iface_refresh();
    	}    
    }

    CleanList(&E);
    CleanList(&V);

    SetExecStateStep(STOP_EXEC_STEPS);
}

void on_execstep_item_activate(GtkWidget *widget, gpointer user_data){   
    (void) widget;
    (void) user_data;
             
	execstep();
    
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
}


void on_nextstep_item_activate(GtkWidget *widget, gpointer user_data){
List E, V;
    
    (void) widget;
    (void) user_data;
    
    if (GetExecStateStep() == EXECUTING_STEPS)
	    SetExecStateStep(STOP_EXEC_STEPS);
    
    InitList(&E);
    InitList(&V);
    
    NextStep(&G, &E, &V);
    
    SetSpecEdges(&DS, &E);
    SetSpecVerts(&DS, &V);

    CleanList(&E);
    CleanList(&V);

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
}

void on_laststep_item_activate(GtkWidget *widget, gpointer user_data){
List E, V;
    
    (void) widget;
    (void) user_data;
    
    if (GetExecStateStep() == EXECUTING_STEPS)
	    SetExecStateStep(STOP_EXEC_STEPS);
    
    InitList(&E);
    InitList(&V);
    
    LastStep(&G, &E, &V);
    
    SetSpecEdges(&DS, &E);
    SetSpecVerts(&DS, &V);

    CleanList(&E);
    CleanList(&V);

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
}

/*void on_close_item_activate(GtkWidget *widget, gpointer user_data){
    
    (void) widget;
    (void) user_data;
    
}*/


void on_about_item_activate(GtkWidget *widget, gpointer user_data){
    
    (void) widget;
    (void) user_data;
    
    factory_show_dialog(iface.about_dialog);
}


void on_preferences_item_activate(GtkWidget *widget, gpointer user_data){
    
    (void) widget;
    (void) user_data;
    
    factory_show_dialog(iface.pref_dialog);
}


void on_plugins_item_activate(GtkWidget *widget, gpointer user_data){
    
    (void) widget;
    (void) user_data;
    
    factory_show_dialog(iface.pgin_dialog);
}


void on_view_mode_item_activate(GtkWidget *widget, gpointer user_data){
    
    (void) widget;
    (void) user_data;

    if (iface.initialized){
        if (GTK_CHECK_MENU_ITEM(get_main_menu_item_by_name(
                        "gtk"))->active){
            gtk_widget_hide(iface.glarea);
            iface.view_mode = VIEW_MODE_GTK;
            iface.changed = 1;
            gtk_widget_show(iface.drawingarea);
            gdk_window_set_cursor(iface.drawingarea->window,
                    iface.cursor[iface.cursor_indx]);
        } else if (GTK_CHECK_MENU_ITEM(get_main_menu_item_by_name(
                        "opengl"))->active){
            gtk_widget_hide(iface.drawingarea);
            iface.view_mode = VIEW_MODE_OPENGL;
            iface.changed = 1;
            gtk_widget_show(iface.glarea);
            gdk_window_set_cursor(iface.glarea->window,
                    iface.cursor[iface.cursor_indx]);
        }
    }
}


void on_open_filesel_ok(GtkWidget *widget, gpointer user_data){
GtkWidget *menuitem;
int *i;

    (void) widget;
    (void) user_data;

    factory_hide_dialog(iface.open_filesel);

    iface.filename = gtk_file_selection_get_filename(
            GTK_FILE_SELECTION(iface.open_filesel));

    menuitem = gtk_menu_get_active(GTK_MENU(iface.open_filesel_menu));
    i = gtk_object_get_data(GTK_OBJECT(menuitem), "itemindex");
    iface.format = (*i);

    open_file();
}


void on_save_filesel_ok(GtkWidget *widget, gpointer user_data){
GtkWidget *menuitem;
int *i;

    (void) widget;
    (void) user_data;

    factory_hide_dialog(iface.save_filesel);

    iface.filename = gtk_file_selection_get_filename(
            GTK_FILE_SELECTION(iface.save_filesel));

    menuitem = gtk_menu_get_active(GTK_MENU(iface.save_filesel_menu));
    i = gtk_object_get_data(GTK_OBJECT(menuitem), "itemindex");
    iface.format = (*i);

    if (!access(iface.filename, F_OK)){
        factory_show_confirm_dialog(iface.conf_dialog,
                "File exists, overwrite it?");
    } else {
        save_file();
        rescan_filesels(); 
    }
}


void on_conf_yes(GtkWidget *widget, gpointer user_data){

    (void) widget;
    (void) user_data;

    factory_hide_dialog(iface.conf_dialog);

    save_file();

    //rescan_filesels(); 
}


void on_preferences_ok_clicked(GtkWidget *widget, gpointer user_data){
const char *s;
GtkWidget *menuitem;
int *i;

    (void) widget;
    (void) user_data;

    factory_hide_dialog(iface.pref_dialog);
    
    s = gtk_entry_get_text(GTK_ENTRY(iface.pref_dialog_neato_text));
    if (s != NULL){
        if (config->neato_path)
            free(config->neato_path);
        config->neato_path = (char *) malloc((strlen(s)+1)*sizeof(char));
        strcpy(config->neato_path, s);
    } else {
        if (config->neato_path)
            free(config->neato_path);
        config->neato_path = NULL;
    }
    
    s = gtk_entry_get_text(GTK_ENTRY(iface.pref_dialog_dot_text));
    if (s != NULL){
        if (config->dot_path)
            free(config->dot_path);
        config->dot_path = (char *) malloc((strlen(s)+1)*sizeof(char));
        strcpy(config->dot_path, s);
    } else {
        if (config->dot_path)
            free(config->dot_path);
        config->dot_path = NULL;
    }
    
    s = gtk_entry_get_text(GTK_ENTRY(iface.pref_dialog_plugins_text));
    if (s != NULL){
        if (config->plugins_path)
            free(config->plugins_path);
        config->plugins_path = (char *) malloc((strlen(s)+1)*sizeof(char));
        strcpy(config->plugins_path, s);
    } else {
        if (config->plugins_path)
            free(config->plugins_path);
        config->plugins_path = NULL;
    }

    s = gtk_entry_get_text(GTK_ENTRY(iface.pref_dialog_datadir_text));
    if (s != NULL){
        if (config->datadir_path != NULL){
            if (strcmp(config->datadir_path, s)){
                if (strlen(s) > 0){
                    gtk_file_selection_set_filename(GTK_FILE_SELECTION(
                            iface.open_filesel), s);
                    gtk_file_selection_set_filename(GTK_FILE_SELECTION(
                            iface.save_filesel), s);
                }
            }
        } else {
            gtk_file_selection_set_filename(GTK_FILE_SELECTION(
                    iface.open_filesel), s);
            gtk_file_selection_set_filename(GTK_FILE_SELECTION(
                    iface.save_filesel), s);
        }
        if (config->datadir_path)
            free(config->datadir_path);
        config->datadir_path = (char *) malloc((strlen(s)+1)*sizeof(char));
        strcpy(config->datadir_path, s);
    } else {
        if (config->datadir_path)
            free(config->datadir_path);
        config->datadir_path = NULL;
    }
    
    menuitem = gtk_menu_get_active(GTK_MENU(
                    iface.pref_dialog_view_mode_menu));
    i = gtk_object_get_data(GTK_OBJECT(menuitem), "itemindex");
    config->view_mode = (*i);
     
    write_config(config);
}


void on_preferences_cancel_clicked(GtkWidget *widget, gpointer user_data){
    
    (void) widget;
    (void) user_data;

    factory_hide_dialog(iface.pref_dialog);

    /* Set old values */
    if (config->neato_path != NULL)
        gtk_entry_set_text(GTK_ENTRY(iface.pref_dialog_neato_text),
                config->neato_path);
    if (config->dot_path != NULL)
        gtk_entry_set_text(GTK_ENTRY(iface.pref_dialog_dot_text),
                config->dot_path);
}


void on_choose_neato_button_clicked(GtkWidget *widget, gpointer user_data){

    (void) widget;
    (void) user_data;

    if (config->neato_path)
        gtk_file_selection_set_filename(GTK_FILE_SELECTION(
                iface.pref_dialog_neato_filesel), config->neato_path);
    
    factory_show_dialog(iface.pref_dialog_neato_filesel);
}


void on_neato_filesel_ok(GtkWidget *widget, gpointer user_data){

    (void) widget;
    (void) user_data;

    factory_hide_dialog(iface.pref_dialog_neato_filesel);
    gtk_entry_set_text(GTK_ENTRY(iface.pref_dialog_neato_text),
            gtk_file_selection_get_filename(
                    GTK_FILE_SELECTION(iface.pref_dialog_neato_filesel)));
}


void on_choose_dot_button_clicked(GtkWidget *widget, gpointer user_data){

    (void) widget;
    (void) user_data;
    
    if (config->dot_path)
        gtk_file_selection_set_filename(GTK_FILE_SELECTION(
                iface.pref_dialog_dot_filesel), config->dot_path);
    
    factory_show_dialog(iface.pref_dialog_dot_filesel);
}


void on_dot_filesel_ok(GtkWidget *widget, gpointer user_data){
    
    (void) widget;
    (void) user_data;

    factory_hide_dialog(iface.pref_dialog_dot_filesel);
    gtk_entry_set_text(GTK_ENTRY(iface.pref_dialog_dot_text),
            gtk_file_selection_get_filename(
                    GTK_FILE_SELECTION(iface.pref_dialog_dot_filesel)));
}


void on_choose_plugins_button_clicked(GtkWidget *widget,
        gpointer user_data){
    
    (void) widget;
    (void) user_data;

    if (config->plugins_path)
        gtk_file_selection_set_filename(GTK_FILE_SELECTION(
                iface.pref_dialog_plugins_filesel), config->plugins_path);
    
    factory_show_dialog(iface.pref_dialog_plugins_filesel);
}


void on_plugins_filesel_ok(GtkWidget *widget, gpointer user_data){

    (void) widget;
    (void) user_data;

    factory_hide_dialog(iface.pref_dialog_plugins_filesel);
    gtk_entry_set_text(GTK_ENTRY(iface.pref_dialog_plugins_text),
            gtk_file_selection_get_filename(
                    GTK_FILE_SELECTION(iface.pref_dialog_plugins_filesel)));
}


void on_choose_datadir_button_clicked(GtkWidget *widget,
        gpointer user_data){

    (void) widget;
    (void) user_data;

    if (config->datadir_path)
        gtk_file_selection_set_filename(GTK_FILE_SELECTION(
                iface.pref_dialog_datadir_filesel), config->datadir_path);
    
    factory_show_dialog(iface.pref_dialog_datadir_filesel);
}


void on_datadir_filesel_ok(GtkWidget *widget, gpointer user_data){

    (void) widget;
    (void) user_data;

    factory_hide_dialog(iface.pref_dialog_datadir_filesel);
    gtk_entry_set_text(GTK_ENTRY(iface.pref_dialog_datadir_text),
            gtk_file_selection_get_filename(
                    GTK_FILE_SELECTION(iface.pref_dialog_datadir_filesel)));
}


void on_graph_ok_clicked(GtkWidget *widget, gpointer user_data){
int i, j, k, p;
float prob;

    (void) widget;
    (void) user_data;
   
    factory_hide_dialog(iface.graph_dialog);

    /* Get graph type and parameters */
    p = gtk_notebook_get_current_page(GTK_NOTEBOOK(
                iface.graph_dialog_notebook));
    if (p == 0){
        i = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(
                    iface.graph_dialog_complete_spin_button));
        if ((i > 0) && (i < 65535)){
            if (G.type == DIGRAPH) {
                InitGraph(&G, DIGRAPH);
            } else {
                InitGraph(&G, GRAPH);
            }

            CreateKn(&G, i);
        }
    } else if (p == 1){
        i = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(
                    iface.graph_dialog_bipartite1_spin_button));
        j = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(
                    iface.graph_dialog_bipartite2_spin_button));
        if ((i > 0) && (i < 65535) && (j > 0) && (j < 65535)){
            if (G.type == DIGRAPH) {
                InitGraph(&G, DIGRAPH);
            } else {
                InitGraph(&G, GRAPH);
            }
            
            CreateKnn(&G, i, j);
        }
    } else if (p == 2){
        i = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(
                    iface.graph_dialog_random1_spin_button));
        prob = gtk_spin_button_get_value_as_float(GTK_SPIN_BUTTON(
                    iface.graph_dialog_random2_spin_button));

        if ((i > 0) && (i < 65535) && (prob >= 0) && (prob <= 1)){
            if (G.type == DIGRAPH) {
                InitGraph(&G, DIGRAPH);
            } else {
                InitGraph(&G, GRAPH);
            }

            CreateRandomGraph(&G, i, prob);
        }
    } else if (p == 3){
        /*i = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(
                    iface.graph_dialog_regular1_spin_button));
        j = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(
                    iface.graph_dialog_regular2_spin_button));

        if ((i > 0) && (i < 65535) && (j >= 0) && (j < i)){
            if (G.type == DIGRAPH) {
                InitGraph(&G, DIGRAPH);
            } else {
                InitGraph(&G, GRAPH);
            }

            CreateRegular(&G, i, j);
        }*/
    } else if (p == 4){
        i = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(
                    iface.graph_dialog_star_spin_button));
        if ((i > 0) && (i < 65535)){
            if (G.type == DIGRAPH) {
                InitGraph(&G, DIGRAPH);
            } else {
                InitGraph(&G, GRAPH);
            }
            CreateStar(&G, i);
        }
    } else if (p == 5){
        i = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(
                    iface.graph_dialog_mitzenmacher1_spin_button));
        prob = gtk_spin_button_get_value_as_float(GTK_SPIN_BUTTON(
                    iface.graph_dialog_mitzenmacher2_spin_button));

        if ((i > 0) && (i < 65535) && (prob >= 0) && (prob <= 1)){
            if (G.type == DIGRAPH) {
                InitGraph(&G, DIGRAPH);
            } else {
                InitGraph(&G, GRAPH);
            }

            CreateMitzenmacherRandomGraph(&G, i, prob);
        }
    }

    /* Change point coordinates */
    if ((p == 0) || (p == 2) || (p == 3) || (p == 5))
        RedrawAsCircle(&G, iface.width, iface.height);
    else if (p == 1)
        RedrawAsBipartite(&G, iface.width, iface.height, i, j);
	else if (p == 4)
		RedrawWithCenteredVertex(&G, iface.width, iface.height);
    
    /* Update drawing */
    ClearDrawStruct(&DS);
    for (j = 0; j < G.size; j++) {
        for (k = 0; k < G.size; k++) {
            if ((G.edge[j][k]) && (j != k)) {
                UpdateCtrlPoints(&G, j, k, LINES); 
            }
         }
    } 
    
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
}


void on_vprop_ok_clicked(GtkWidget *widget, gpointer user_data){

    (void) widget;
    (void) user_data;

    save_vertex_properties();
    factory_hide_dialog(iface.vprop_dialog);
}


void on_eprop_ok_clicked(GtkWidget *widget, gpointer user_data){

    (void) widget;
    (void) user_data;
   
    save_edge_properties();
    factory_hide_dialog(iface.eprop_dialog);
}


void load_vertex_properties(void){
char msg[200];

    sprintf(msg, "%d", G.vertex[iface.vertex_indx].id);
    gtk_label_set_text(GTK_LABEL(iface.vprop_dialog_vertex_label), msg);
    if (G.vertex[iface.vertex_indx].label != NULL)
        gtk_entry_set_text(GTK_ENTRY(iface.vprop_dialog_label_text),
            G.vertex[iface.vertex_indx].label);
    else
        gtk_entry_set_text(GTK_ENTRY(iface.vprop_dialog_label_text), "");
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(
            iface.vprop_dialog_weight_spin_button),
            G.vertex[iface.vertex_indx].weight);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(
            iface.vprop_dialog_color_spin_button),
            G.vertex[iface.vertex_indx].color);
}


void save_vertex_properties(void){
const char *s;
int len;

	/* Remove last label */
	if (G.vertex[iface.vertex_indx].label){
		free(G.vertex[iface.vertex_indx].label);
		G.vertex[iface.vertex_indx].label = NULL;
	}
	
	/* Set new label */
    s = gtk_entry_get_text(GTK_ENTRY(iface.vprop_dialog_label_text));
    if (s != NULL){
        len = strlen(s);
        if (len > 0){
            G.vertex[iface.vertex_indx].label = (char *) malloc(len+1);
            strcpy(G.vertex[iface.vertex_indx].label, s);
        }
    }

	/* Set weight and color */
    G.vertex[iface.vertex_indx].weight =
        gtk_spin_button_get_value_as_float(GTK_SPIN_BUTTON(
            iface.vprop_dialog_weight_spin_button));

    G.vertex[iface.vertex_indx].color =
        gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(
            iface.vprop_dialog_color_spin_button));
}


void load_edge_properties(void){
char msg[200];

    sprintf(msg, "(%d,%d)",iface.edge_indx[0], iface.edge_indx[1]);
    gtk_label_set_text(GTK_LABEL(iface.eprop_dialog_edge_label), msg);
    if (G.prop[iface.edge_indx[0]][iface.edge_indx[1]].label != NULL)
        gtk_entry_set_text(GTK_ENTRY(iface.eprop_dialog_label_text),
            G.prop[iface.edge_indx[0]][iface.edge_indx[1]].label);
    else
        gtk_entry_set_text(GTK_ENTRY(iface.eprop_dialog_label_text), "");
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(
            iface.eprop_dialog_weight_spin_button),
            G.prop[iface.edge_indx[0]][iface.edge_indx[1]].weight);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(
            iface.eprop_dialog_color_spin_button),
            G.prop[iface.edge_indx[0]][iface.edge_indx[1]].color);
}


void save_edge_properties(void){
const char *s;
int len;

	/* Remove last label */
	if (G.prop[iface.edge_indx[0]][iface.edge_indx[1]].label){
		free(G.prop[iface.edge_indx[0]][iface.edge_indx[1]].label);
		G.prop[iface.edge_indx[0]][iface.edge_indx[1]].label = NULL;
	}

	/* Set new label */
    s = gtk_entry_get_text(GTK_ENTRY(iface.eprop_dialog_label_text));
    if (s != NULL){
        len = strlen(s);
        if (len > 0){
            G.prop[iface.edge_indx[0]][iface.edge_indx[1]].label =
                    (char *) malloc(len+1);
            strcpy(G.prop[iface.edge_indx[0]][iface.edge_indx[1]].label, s);
        }
    }

	/* Set weight and color */
    G.prop[iface.edge_indx[0]][iface.edge_indx[1]].weight =
        gtk_spin_button_get_value_as_float(GTK_SPIN_BUTTON(
            iface.eprop_dialog_weight_spin_button));

    G.prop[iface.edge_indx[0]][iface.edge_indx[1]].color =
        gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(
            iface.eprop_dialog_color_spin_button));

	/* If graph is not directed, copy information to another arc */
    if (G.type == GRAPH){
		if (G.prop[iface.edge_indx[1]][iface.edge_indx[0]].label){
			free(G.prop[iface.edge_indx[1]][iface.edge_indx[0]].label);
			G.prop[iface.edge_indx[1]][iface.edge_indx[0]].label = NULL;
		}
			
        if (G.prop[iface.edge_indx[0]][iface.edge_indx[1]].label) {
			len = strlen(G.prop[iface.edge_indx[0]][iface.edge_indx[1]].label);
			G.prop[iface.edge_indx[1]][iface.edge_indx[0]].label =
					(char *) malloc(len+1);
			strcpy(G.prop[iface.edge_indx[1]][iface.edge_indx[0]].label,
				   G.prop[iface.edge_indx[0]][iface.edge_indx[1]].label);
		}
        G.prop[iface.edge_indx[1]][iface.edge_indx[0]].weight =
                G.prop[iface.edge_indx[0]][iface.edge_indx[1]].weight;
        G.prop[iface.edge_indx[1]][iface.edge_indx[0]].color =
                G.prop[iface.edge_indx[0]][iface.edge_indx[1]].color;
    }
}


void on_vlabel_mode_activate(GtkMenuItem *menuitem, gpointer user_data){

    (void) menuitem;
    iface.vlabel_mode = ((int) user_data);

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
}


void on_elabel_mode_activate(GtkMenuItem *menuitem, gpointer user_data){

    (void) menuitem;
    iface.elabel_mode = ((int) user_data);

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
}


void open_file(void){

    switch (iface.format){
        case 0:
            if (!strcasecmp(iface.filename+strlen(iface.filename)-3,
                            ".gr")){
                G.type = GRAPH;
                if (!LoadGraph(iface.filename, &G)){
                    showmsg(rw_error);
                    iface.filename = NULL;
                }
            } else if (!strcasecmp(iface.filename+strlen(iface.filename)-4,
                            ".dot")){
                if (!ReadPlainDotGraph(iface.filename, &G, G.type)){
                    showmsg(rw_error);
                    iface.filename = NULL;
                }
            } else {
                showmsg("Could not determine file type!");
            }
            break;
        case 1:
            G.type = GRAPH;
            if (!LoadGraph(iface.filename, &G)){
                showmsg(rw_error);
                iface.filename = NULL;
            }
            break;
        case 2:
            if (!ReadPlainDotGraph(iface.filename, &G, G.type)){
                showmsg(rw_error);
                iface.filename = NULL;
            }
            break;
    }

    /*if (G.type == GRAPH)
        gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(
                get_main_menu_item_by_name("toggle")), FALSE);
    else
        gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(
                get_main_menu_item_by_name("toggle")), TRUE);*/

    change_window_title();

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
}


void save_file(void){

    switch (iface.format){
        case 0:
            if (!strcasecmp(iface.filename+strlen(iface.filename)-3,
                            ".gr")){
                if (!WriteGraph(iface.filename, &G))
                    showmsg(rw_error);
            } else if (!strcasecmp(iface.filename+strlen(iface.filename)-4,
                            ".dot")){
                if (!WriteDotGraph(iface.filename, &G))
                    showmsg(rw_error);
            } else if (!strcasecmp(iface.filename+strlen(iface.filename)-3,
                            ".ps")){
                if (!WritePsGraph(iface.filename, &G))
                    showmsg(rw_error);
            } else {
                showmsg("Could not determine file type!");
            }
            break;
        case 1:
            if (!WriteGraph(iface.filename, &G))
                showmsg(rw_error);
            break;
        case 2:
            if (!WriteDotGraph(iface.filename, &G))
                showmsg(rw_error);
            break;
        case 3:
            if (!WritePsGraph(iface.filename, &G))
                showmsg(rw_error);
            break;
    }

    change_window_title();
}


void update_status(void){
char string[1000];

    if (iface.help_string != NULL){
        if (strlen(iface.status_string) > 0)
            sprintf(string, "%s [%s] (%s = %d, %s = %d)", iface.help_string,
                    iface.status_string, VString, G.size,
                    (G.type == DIGRAPH) ? (AString):(EString),
                    (G.type == DIGRAPH) ? (G.narcs):(G.nedges));
        else
            sprintf(string, "%s (%s = %d, %s = %d)", iface.help_string,
                    VString, G.size,
                    (G.type == DIGRAPH) ? (AString):(EString),
                    (G.type == DIGRAPH) ? (G.narcs):(G.nedges));
    } else {
        if (strlen(iface.status_string) > 0)
            sprintf(string, "[%s] (%s = %d, %s = %d)",
                    iface.status_string, VString, G.size,
                    (G.type == DIGRAPH) ? (AString):(EString),
                    (G.type == DIGRAPH) ? (G.narcs):(G.nedges));
        else
            sprintf(string, "(%s = %d, %s = %d)",
                    VString, G.size,
                    (G.type == DIGRAPH) ? (AString):(EString),
                    (G.type == DIGRAPH) ? (G.narcs):(G.nedges));
    }
    gtk_statusbar_pop(GTK_STATUSBAR(iface.statusbar), 1);
    gtk_statusbar_push(GTK_STATUSBAR(iface.statusbar), 1, string);
}


void rescan_filesels(void){
const char *old;
char *real_old;

    /* Open filesel */
    old = gtk_file_selection_get_filename(GTK_FILE_SELECTION(
                iface.open_filesel));
    real_old = (char *) malloc(strlen(old)+1);
    strcpy(real_old, old);
    gtk_file_selection_set_filename(GTK_FILE_SELECTION(iface.open_filesel),
            "/");
    gtk_file_selection_set_filename(GTK_FILE_SELECTION(iface.open_filesel),
            real_old);
    free(real_old);

    /* Save filesel */
    old = gtk_file_selection_get_filename(GTK_FILE_SELECTION(
                iface.save_filesel));
    real_old = (char *) malloc(strlen(old)+1);
    strcpy(real_old, old);
    gtk_file_selection_set_filename(GTK_FILE_SELECTION(iface.save_filesel),
            "/");
    gtk_file_selection_set_filename(GTK_FILE_SELECTION(iface.save_filesel),
            real_old);
    free(real_old);
}


void change_window_title(void){
int i, j;

    if ((iface.filename != NULL) && (strlen(iface.filename) > 0)){
        if ((strlen(MAIN_WINDOW_TITLE)+strlen(iface.filename)+4) >
                MAX_TITLE_SIZE){
            /* Just for fun... */
            sprintf(iface.title, "%s - ", MAIN_WINDOW_TITLE);
            for (i = strlen(MAIN_WINDOW_TITLE)+3, j = 0;
                 i < (MAX_TITLE_SIZE-1); i++, j++){
                iface.title[i] = iface.filename[j];
                iface.title[i+1] = 0;
            }
        } else
            sprintf(iface.title, "%s - %s",
                    MAIN_WINDOW_TITLE, iface.filename);
    } else
        sprintf(iface.title, "%s", MAIN_WINDOW_TITLE);
        
    gtk_window_set_title(GTK_WINDOW(iface.window), iface.title);
}


void showmsg(char *format,...){
va_list args;
char buf[2000];
GtkWidget *label;

    va_start(args, format);
    vsprintf(buf, format, args);
    va_end(args);

    label = gtk_object_get_data(GTK_OBJECT(iface.msg_dialog), "label");
    gtk_label_set_text(GTK_LABEL(label), buf);
    gtk_widget_show(iface.msg_dialog);
    gdk_window_raise(iface.msg_dialog->window);
}


void on_run_algorithm(GtkWidget *widget, gpointer user_data){
Algor *node;
AlgorR *noder;
AlgorS *nodes;
List E, V;
char *name;
int found;

    (void) widget;
    name = (char *) user_data;
    found = 0;

    if (L.size > 0) {
        node = L.first;
        while ((node) && (!found)) {
            if (!strcmp(node->string, name)){
                node->alg(&G, iface.status_string);
                found = 1;
            }
            node = node->next;
        }
    }

    if ((RL.size > 0) && (!found)) {
        noder = RL.first;
        while ((noder) && (!found)) {
            if (!strcmp(noder->string, name)){
                InitList(&E);
                InitList(&V);
                noder->alg(&G, iface.status_string, &E, &V);
                SetSpecEdges(&DS, &E);
                SetSpecVerts(&DS, &V);
                CleanList(&E);
                found = 1;
            }
            noder = noder->next;
        }
    }
      
    if ((TL.size > 0) && (!found)) {
        node = TL.first;
        while ((node) && (!found)) {
            if (!strcmp(node->string, name)){
                node->alg(&G, iface.status_string);
                found = 1;
            }
            node = node->next;
        }
    }

    if ((SL.size > 0) && (!found)) {
        nodes = SL.first;
        while ((nodes) && (!found)) {
            if (!strcmp(nodes->string, name)){
                InitList(&E);
                InitList(&V);
                // -------- --------//
                nodes->alg(&G, iface.status_string, 0, &E, &V);
                SetSpecEdges(&DS, &E);
                SetSpecVerts(&DS, &V);
                CleanList(&E);
                found = 1;
            }
            nodes = nodes->next;
        }
    }

    if ((SSL.size > 0) && (!found)) {
        nodes = SSL.first;
        while ((nodes) && (!found)) {
            if (!strcmp(nodes->string, name)){
                InitList(&E);
                InitList(&V);
                // -------- --------//
                nodes->alg(&G, iface.status_string, 0, &E, &V);
                SetSpecEdges(&DS, &E);
                SetSpecVerts(&DS, &V);
                CleanList(&E);
                found = 1;
            }
            nodes = nodes->next;
        }
    }
	
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
}


/* ******* */
/* * End * */
/* ******* */

