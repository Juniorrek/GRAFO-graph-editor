/*
 *  grafo - graph editor
 *  Copyright (c) 2003-2008
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


/* ******************* */
/* * stargraphexample.c * */
/* ******************* */

/* Plugin example: Generate a strar graph */

#include <stdio.h>
#include <stdlib.h>

/* Here the GRAFO headers */
/* Add plugins information */
#include "../pgin.h"

/* Add graph struct information */
#include "../graph.h"

/* Prototype functions */
Pgin *pgin_info(void);
int stargraph(Graph *G, char *mess);


/* This function returns the plugin info */
Pgin *pgin_info(void){
Pgin *pgin;

    pgin = (Pgin *) malloc(2 * sizeof(Pgin));

    pgin[0].type = PGIN_TRANSF_ALG;
    pgin[0].label = "Generate Star Graph";
    pgin[0].name = "stargraph";
    pgin[0].flags = PGIN_FLAG_REDRAWWITHCENTEREDVERTEX;

    pgin[1].type = PGIN_LIST_END;
    pgin[1].label = 0;
    pgin[1].name = 0;
    pgin[1].flags = 0;

    return pgin;
}

/* This is the plugin's main function */
int stargraph(Graph *G, char *mess){
int i, n;

    /* Get number of vertices */
    n = pgin_read_int("Generate Star Graph", "Number of vertices",
                5, 1, 2000000, 1);

    /* Generate graph */
    InitGraph(G, G->type);

    /* Adds N vertices in the position (0,0) */
    for (i = 0; i < n; i++)
        AddPoint(G, 0, 0);

    /* Adds N edges starting at the vertex 0 to N-1 */
    for (i = 1; i < n; i++) { 
        AddEdge(G, 0, i);
    }

    return 1;
}

/* ******* */
/* * End * */
/* ******* */
