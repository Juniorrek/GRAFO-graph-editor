/*
 *  grafo - graph editor
 *  Copyright (c) 2003-2008
 *      Murilo Vicente Goncalves da Silva <murilo@pet.inf.ufpr.br>
 *      Oliver Matias van Kaick <oliver@pet.inf.ufpr.br>
 *      Ulisses Cordeiro Pereira <ulisses@cordeiropereira.com.brr>
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


/* ******************* */
/* * vertexexample.c * */
/* ******************* */

/* Plugin example: This program does not compute anything useful.
 * It is an example of how to use the "special vertex" list. 
 * The vertices in this list will be highlighted in the GRAFO 
 * interface.
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
 * -> The second (in this case "Highest Degree") is the name that will 
 * appear in the menu of GRAFO. 
 * -> The third is the name of the function in the code (in this 
 * case "MaxDegree").
 * -> The fourth is a set of plugin flags. In the example of the 
 * fouth funtion the flag prevents that GRAFO redraws the resulting graph.
 * If you do not know what is a "plugin flag", only put 0 in this field.
 */


/* Plugin Information */
Pgin pgin[2] = {
    { PGIN_RESULT_ALG, "Highest Degree", "MaxDegree" },
    { PGIN_LIST_END, NULL, NULL },
};



/* The pgin_info() function should always be present. GRAFO will call
 * it to get the plugin information.
 */
Pgin *pgin_info(void);

Pgin *pgin_info(void){

    return pgin;
}

/* Plugin example: This program does not compute anything useful.
 * It is an example of how to use the "special vertex" list. 
 * Here this list have the name verts. The vertices in this list will 
 * be highlighted in the GRAFO interface.

 * In this example, the function finds the vertex with the highest 
 * degree (acctualy one of them) and put it in the verts list. */

int MaxDegree(Graph *G, char *mess, List *arc_edges, List *verts);

int MaxDegree(Graph *G, char *mess, List *arc_edges, List *verts){
Node N;
int i,maxdeg;

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

    /* Place maxdeg in the Node */
    N = MakeNode(&maxdeg, sizeof(int));

    /* Place the node in the list of "special vertex" */
    InsertNode(verts, N);

    return 1;
}


/* ******* */
/* * End * */
/* ******* */
