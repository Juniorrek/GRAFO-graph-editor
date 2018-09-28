/*
 *  grafo - graph editor
 *  Copyright (c) 2008
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


/* ********************** */
/* * selectoddexample.c * */
/* ********************** */

/* Plugin example: This program does not compute anything useful.
 * It is an example of how to use the "step by step" structure. 
 * The odd vertex and edges between odd vertex will be highlighted in the GRAFO 
 * interface.
 *
 */ 

/* Plugin example: Select odd */

/* C language headers */
#include <stdio.h>
#include <stdlib.h>

/* Here the GRAFO headers */
/* Add plugins information */
#include "../pgin.h"

/* Add graph struct information */
#include "../graph.h"

/* Add step by step struct information */
#include "../step.h"

/* Mandatory function pgin_info */
Pgin *pgin_info(void);

/* algorithm function */
int SelectOdd(Graph *G, char *mess, int indx, List *arc_edges, List *verts);

/* Others functions */


/* The pgin_info() function should always be present. GRAFO will call
 * it to get the plugin information.
 */
Pgin *pgin_info(void){
Pgin *pgin;

    /* Alocate space for SelectOdd and PGIN_LIST_END */
    pgin = (Pgin *) malloc(2 * sizeof(Pgin));

    /* Includes SelectOdd as the first algorithm of plugin */    
    pgin[0].type = PGIN_STEP_ALG;
    pgin[0].label = "Select odd";
    pgin[0].name = "SelectOdd";
    pgin[0].flags = 0;

    /* It indicates the end of the list of algorithms */
    pgin[1].type = PGIN_LIST_END;
    pgin[1].label = 0;
    pgin[1].name = 0;
    pgin[1].flags = 0;

    return pgin;
}

/* This is the plugin's main function */
int SelectOdd(Graph *G, char *mess, int indx, List *arc_edges, List *verts) {
SEdge E; /* Single Edge */
Node N;
int i, j,oddvertex;

    /* Test if graph is empty */
    if (G->size < 1) {
        sprintf(mess, "%s", "Empty graph!");
        return 1;
    }

    /* Set to False (0) the property 'mark' in all vertices */
    ClearMarkedVerts(G);

    /* Add first step - No highlights */
    AddStep(G, arc_edges, verts);

    /* Find and mark oddvertex */
    oddvertex = 0;
    for (i = 1; i < G->size; i++) {

        /* Check if i is a odd number */
        if ((i % 2)) {
            /* Mark the vertex for future use */
            G->vertex[i].mark = 1;
            oddvertex = i;

            /* Place oddvertex in the Node */
            N = MakeNode(&oddvertex, sizeof(int));

            /* Place the node in the list of "special vertex" */
            InsertNode(verts, N);

            /* Add new step - Save highlight vertex */
            AddStep(G, arc_edges, verts);
        }
    }

    /* Set to False (0) the property 'mark' in all vertices */
    ClearMarkedEdges(G);

    /* Find the odd vertex adjacent with other odd vertex */
    for (i = 0; i < G->size; i++) {
        for (j = 0; j < G->size; j++) {

                /* Avoid saving the step [j,i] because [i,j] have been saved */
            if (!G->prop[j][i].mark && \
                /* True if vertex[i] is adjacent vertex[j] */
                G->edge[i][j] && \
                /* True if vertex[i] is a odd vertex */
                G->vertex[i].mark && \
                /* True if vertex[j] is a odd vertex */
                G->vertex[j].mark) {

                /* Mark the edge for future use */
                G->prop[i][j].mark = 1;

                /* Assign odd vertex adjacent with other odd vertex in a single edge */
                E.u = i;
                E.v = j;

                /* Place the "single edge" in the Node */
                N = MakeNode(&E, sizeof(SEdge));

                /* Place the node in the list of "special edges" */
                InsertNode(arc_edges, N);

                /* Add new step - Save highlight edge */
                AddStep(G, arc_edges, verts);
            }
        }
    }

    return 1;
}


/* ******* */
/* * End * */
/* ******* */
