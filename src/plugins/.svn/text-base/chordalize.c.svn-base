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
/* * chordalize.c * */
/* **************** */


/* Plugin example: Chordalize a graph: put edges in somewhat arbitrary way
 * to make the graph chordal */

#include <stdio.h>
#include <stdlib.h>
#include "../pgin.h"
#include "../graph.h"
#include "../list.h"


typedef enum {RESET, NEXT} PalOpt;

Pgin *pgin_info(void);
//int Chordalize(Graph *G, char *mess, List *arc_edges, List *verts);
int Chordalize(Graph *G, char *mess);
int Delete_Sink(Graph *G); 
int GetNextColor(PalOpt opt); 


Pgin *pgin_info(void){
Pgin *pgin;

    pgin = (Pgin *) malloc(2 * sizeof(Pgin));
    
    pgin[0].type = PGIN_TRANSF_ALG;
    pgin[0].label = "Chordalize a Graph";
    pgin[0].name = "Chordalize";
    pgin[0].flags = 0;

    pgin[1].type = PGIN_LIST_END;
    pgin[1].label = 0;
    pgin[1].name = 0;
    pgin[1].flags = 0;

    return pgin;
}

int Chordalize(Graph *G, char *mess) {
int vertice[MAX_POINTS]; /* cjto vertices retirados de G */
int v_aux[MAX_POINTS]; /* posicao original dos vertices em G */
int v_aux2[MAX_POINTS]; /* posicao original dos vertices em G */
int vert; /* vertice retirado de G em um dado instante */
int NVert=0; /* Numero de vertices retirados ate' o momento */
Graph *H; /* Grafos auxiliares */
int i,j,count, chordalized=0;

    if (G->size == 0) {
        sprintf(mess,"%s","No Vertices!");
        return 0;
    }

    while (!chordalized) {
        /* Aloca H e copia G nele */
        //H = (Graph *) malloc (sizeof (Graph));
        CopyGraph(H, G);

        /* Inicializa vetor auxiliar */
        for (i=0; i < G->size; i++) {
            v_aux[i] = i;
        }

        /* Caso existam sumidouros, eles sao eliminados 
         * ate' que apenas sobre 1 vertice em H */
        while ((H->size > 1) && ((vert=Delete_Sink(H)) != -1 )) {
            vertice[NVert] = v_aux[vert];

            /* Deve dar para fazer de um jeito mais eficiente */
            //for (i=vert; i < (G->size - NVert); i++) {
            for (i=vert; i < G->size; i++) {
                v_aux[i] = v_aux[i+1];
            }

            NVert++;
        }


        /* E' cordal */
        if (H->size == 1) {

            /* Puts the last vertex. (It ins't necessary) */
            vertice[NVert] = v_aux[0];

            sprintf(mess,"%s","Chordalized");
            chordalized = 1;

        /* If the graph is not chordal, "chordalize it"! */
        } else {
            /* Takes the adjacents of the first vertex... */
            count=0;
            for (i=0; i<G->size; i++) {
                if (G->edge[v_aux[0]][i]) {
                    v_aux2[count] = i;
                    count++;
                }
            }
            /* ...and "Cliquelize" it */
            for (i=0; i<count; i++) {
                for (j=0; j<count; j++) {
                    if (v_aux2[i] != v_aux2[j]) {
                        AddEdge(G,v_aux2[i],v_aux2[j]);
                    }
                }
            }
            //free(H);
            printf("Chordalizing\n");
        }

    }
    return 1;
}

/* 
 * Removes the sink and returns its position.
 * The algorithm treat the graph as an undirected one. The
 * sink is the vertex such that all its adjacents vertex
 * are a clique.
 * If there are no sink, it is returned -1 
 */
int Delete_Sink(Graph *G) {
int vertice[MAX_POINTS]; /* Holds the adj. of a given vertex */
int NAdj; /* Number of vertex in vertice[]  */
int fail; /* 1 if the set of vertex in vertice[] are not a clique */
int found = 0; /* 1 when a sink is found */
int i,j,k;
    
    /* Para cada vertice do grafo */
    i = 0;
    while ((!found) && (i<G->size)) {
        /* Guarda em vertice[] os k vertices adj. ao i-esimo vertice */	
        NAdj=0; 
        for (j=0; j<G->size; j++) {
            if (G->edge[i][j]) {
                vertice[NAdj] = j;
                NAdj++;
            }
        }
        /* Verifica se esses k adjacentes sao uma clique */
        fail = 0;
        for (j=0; j<NAdj-1; j++) {
            for (k=j+1; k<NAdj; k++) {
                if(!G->edge[vertice[j]][vertice[k]]) {
                    fail=1;
                    goto FASTSKIP;
                }
            }
        }
        FASTSKIP: 
		
        /* Caso os vertices em vertice[] sejam uma clique */
        if (!fail) {
            found = 1;
        }
		
        i++;
    }

    /* Retira o vertice e o retorna */
    if (found) {
        RemovePoint(G, i-1);
        return (i-1);
    } else {
        return -1;
    }
	
}


/* ******* */
/* * End * */
/* ******* */
