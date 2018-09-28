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


/* ***************** */
/* * edgeexample.c * */
/* ***************** */

/* Plugin example: This program does not compute anything useful.
 * It is an example of how to use the "special edge" list. 
 * The edges in this list will be highlighted in the GRAFO 
 * interface.
 *
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* Here the GRAFO headers */
#include "../pgin.h"
#include "../graph.h"
#include "../list.h"


/* You have to fill the pgin struct with information about your plugins.
 * Each position of the pgin struct is related with some function you
 * have programed. Below one example where you programmed five functions:
 *
 * Pgin pgin[6] = {
 *     { PGIN_RESULT_ALG, "Find a Hamilton Path", "HPath", 0 },
 *     { PGIN_TEST_ALG, "Test Planarity", "PlanarGraph", 0 },
 *     { PGIN_TEST_ALG, "Test Bergeness", "BergeGraph", 0 },
 *     { PGIN_TRANSF, "Compute the complement", "Compl", PGIN_FLAG_NOREDRAW },
 *     { PGIN_TRANSF, "Line Graph", "LineGraph", 0 },
 *     { PGIN_LIST_END, NULL, NULL, 0 },
 * };
 *
 * In the last position you should put { PGIN_LIST_END, NULL, NULL }, to
 * end the list of plugins.
 * You have to put four pieces of information in the pgin struct:
 * -> The first is the class of your algorithm. The first entry
 * above is a Result Algorithm, so you have to put PGIN_RESULT_ALG
 * -> The second (in this case "Some Edge") is the name that will 
 * appear in the menu of GRAFO. 
 * -> The third is the name of the function in the code.
 * -> The fourth is a set of plugin flags. In the example of the 
 * fouth funtion the flag prevents that GRAFO redraws the resulting graph.
 */


/* Plugin Information */
Pgin pgin[2] = {
    { PGIN_RESULT_ALG, "Some Edge", "MaxMinEdge",0 },
    { PGIN_LIST_END, NULL, NULL, 0 },
};


/* The pgin_info() function should always be present. GRAFO will call
 * it to get the plugin information.
 */
Pgin *pgin_info(void);

Pgin *pgin_info(void){

    return pgin;
}


/* The plugin example: It is an example of how to use the "special edge" list.
 * Here the list have the name arc_edges. The edges in this list will 
 * be highlighted in the GRAFO interface.
 * 
 * The following edge will be put in the list: 
 * Let the egde be called E. We will define it now: Let v be one vertex of the 
 * graph such that no other vertex has degree greater than v. Let 
 * u be the vertex adjacent to v with the lowest degree. The
 * edge E links u to v. */

int MaxMinEdge(Graph *G, char *mess, List *arc_edges, List *verts);

int MaxMinEdge(Graph *G, char *mess, List *arc_edges, List *verts){
SEdge E; /* Single Edge */
Node N;
int i,maxdeg,mindeg;

    /* Test if graph is empty */
    if (G->size < 1) {
        sprintf(mess, "%s", "Empty graph!");
        return 1;
    }

    /* Find the vertex with max degree */
    maxdeg = 0;
    for (i = 1; i < G->size; i++) {
        if (G->vertex[i].degree > G->vertex[maxdeg].degree) {
            maxdeg = i;
        }
    }

    /* Find the vertex with min degree adjacent with max degree */
    mindeg = -1;
    for (i = 0; i < G->size; i++) {
        if (G->edge[maxdeg][i]) {
            if (mindeg == -1) {
                mindeg = i;
            } else if (G->vertex[i].degree < G->vertex[mindeg].degree) {
                mindeg = i;
            }
        }
    }

    /* Assign maxdeg and mindeg in a single edge */
    E.u = maxdeg;
    E.v = mindeg;

    /* Place the "single edge" in the Node */
    N = MakeNode(&E, sizeof(SEdge));

    /* Place the node in the list of "special edges" */
    InsertNode(arc_edges, N);

    return 1;
}


/* ******* */
/* * End * */
/* ******* */
