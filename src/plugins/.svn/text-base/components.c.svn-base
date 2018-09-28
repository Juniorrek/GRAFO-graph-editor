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


/* **************** */
/* * components.c * */
/* **************** */


/* Plugin example: Fill G->ncomp with the number of components of
 * the graph. */

#include <stdio.h>
#include <stdlib.h>
/* Here the GRAFO headers */
#include "../pgin.h"
#include "../graph.h"
#include "../alghandler.h"

Pgin pgin[2] = {
    { PGIN_RESULT_ALG, "Conected Components", "Components" },
    { PGIN_LIST_END, NULL, NULL },
};


Pgin *pgin_info(void);
int Components(Graph *G, char *mess, List *arc_edges, List *verts);


Pgin *pgin_info(void){

    return pgin;
}


/* Fill G->ncomp with the number of components of the graph (let say k) 
 * and for each vertex put in vertex.concomp the index of the conected
 * component (1,2,...,k) where the vertex is lying. */
int Components(Graph *G, char *mess, List *arc_edges, List *verts){
List E, V;
Graph *H;
Node N;
SVertex v;
int concomp, i, n=0, start=0;


    if (G->size < 1) {
        sprintf(mess, "%s", "Empty graph!");
        G->ncomp = 0;
        return 0;
    }

    /* Allocs H and copies G in it */
    H = (Graph *) malloc (sizeof (Graph));
	CopyGraph(H,G);
			
    InitList(&E);
    InitList(&V);
    concomp = 0;

    for (i=0; i < G->size;i++) {
        G->vertex[i].concomp = -1;
    }

	
    while (n < G->size) {
        CleanList(&E); 
        CleanList(&V); 
        RunAlgorithm("Depth First",H,mess, start, &E,&V);
        N = V.head;
        while (N != NULL) { 
            memcpy(&v, N->data, sizeof(SVertex));
            G->vertex[v].concomp = concomp;
            N = N->next;
            n++;
        }
        concomp++;
        start = 0;
        while (start < G->size) {
            if (G->vertex[start].concomp == -1) {
                break;
            }
            start++;
        }
    }

    G->ncomp = concomp;
		
    sprintf(mess,"%d componente(s) conexo(s)",G->ncomp);

    CleanList(&E); 
    CleanList(&V); 
    
    free(H);
     
    return G->ncomp; 
}


/* ******* */
/* * End * */
/* ******* */
