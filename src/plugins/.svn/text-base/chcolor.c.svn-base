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


/* ************* */
/* * chcolor.c * */
/* ************* */


/* Plugin example: finds a minimum coloring of a chordal graph */

#include <stdio.h>
#include <stdlib.h>
#include "../pgin.h"
#include "../graph.h"
#include "../list.h"


typedef enum {RESET, NEXT} PalOpt;


Pgin pgin[2] = {
    { PGIN_RESULT_ALG, "Chordal Graph Coloring", "ChColor" },
    { PGIN_LIST_END, NULL, NULL },
};


Pgin *pgin_info(void);
int ChColor(Graph *G, char *mess, List *arc_edges, List *verts);
int Delete_Sink(Graph *G); 
int GetNextColor(PalOpt opt); 


Pgin *pgin_info(void){

    return pgin;
}

/* 
 * Colore um grafo cordal usando algoritmo descrito em Gavril [72]. 
 * Retorna 0 caso o grafo nao seja cordal. 
 *
 *  F. Gavril [72],
 * "ALGORITHMS FOR MINIMUM COLORING, MAXIMUM CLIQUE, MINIMUM
 *  COVERING BY CLIQUES, AND MAXIMUM INDEPENDENT SET OF A 
 *  CHORDAL GRAPH",
 *  SIAM J. Comput., Vol. 1, No. 2, June 1972.
 */

int ChColor(Graph *G, char *mess, List *arc_edges, List *verts){
int vertice[MAX_POINTS]; /* cjto vertices retirados de G */
int v_aux[MAX_POINTS]; /* posicao original dos vertices em G */
int vert; /* vertice retirado de G em um dado instante */
int NVert=0; /* Numero de vertices retirados ate' o momento */
Graph *H, *DG; /* Grafos auxiliares */
List *SameColor; /* Ptr to List of vertices with same color */
List ColorSet; /* List of SameColor lists*/
Node TmpNode1, TmpNode2;
int fail;
int i,j,*v;

    if (G->size == 0) {
        sprintf(mess,"%s","No Vertices!");
        return 0;
    }	    

    /* Aloca H e copia G nele */
    H = (Graph *) malloc (sizeof (Graph));
    memcpy(H, G, sizeof(Graph));
 
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

    vertice[NVert] = v_aux[0];

    /* E' cordal */
    if (H->size == 1) {

        /* Constroi grafo R-orientado */
        DG = (Graph *) malloc (sizeof (Graph));
        InitGraph(DG, DIGRAPH); 
       
        /* Creates the points of the digraph */
        for (i=0; i < G->size; i++) {
            /* Acctually it is not necessary use some specific
             * values for x and y. It can be used any value. */
            AddPoint(DG, G->point[i].x, G->point[i].y);
        }
	
        /* The topological ordering under consideration 
         * is that from last to first */
        for (i=NVert; i>=0; i--) {
            for (j=0; j<G->size; j++) {
                /* Caso haja aresta no grafo original e ela nao tenha
                 * sido orientada ainda */
                if (G->edge[vertice[i]][j] && (!DG->edge[j][vertice[i]])) {
                    DG->edge[vertice[i]][j] = 1;
                }
            }
        }

        /* At last the graph coloring */

        /* The firstvertex maintain its color (red) */
        SameColor = CreateList();
        InitList(SameColor);
        
        TmpNode1 = MakeNode((void*)&NVert, sizeof(int));
        InsertNode(SameColor, TmpNode1);

        /* Inserts the first list in ColorSet */
        InitList(&ColorSet);
        TmpNode1 = MakeNode(SameColor, sizeof(List));
        InsertNode(&ColorSet, TmpNode1);
      
        /* Initialize the coloring pallete */
        GetNextColor(RESET);

        /* Now, for each new vertex, it is checked if there is a
         * vertex with the color already defined without arcs to it.
         * In this case it is used the same color. */

        for (i=NVert-1; i>=0; i--) {

            /* Gets each list of vertices */
            TmpNode1 = ColorSet.head;
            while (TmpNode1 != NULL) {
                /* For each vertex in list check if there are arcs */
                fail = 0;
                SameColor = (List *) TmpNode1->data;
                TmpNode2 = SameColor->head;
                while(TmpNode2 != NULL) {
                    v = (int*) TmpNode2->data;
                    if (DG->edge[*v][vertice[i]]) {
                        fail = 1;
                        break;
                    }                 
                    TmpNode2 = TmpNode2->next;
		}
                /* 
                 * If all the vertices with that color do not have
                 * arcs to vertice[i], it is used that color.
                 */
                if (!fail) {
	            G->point[vertice[i]].color = 
			 G->point[*v].color;
                    /* Now, inserts the vertex in the list */
                    v = (int *) malloc (sizeof(int));
		    *v = vertice[i];
                    SameColor = (List*) TmpNode1->data;
		    InsertNode(SameColor, MakeNode(v, sizeof(int)) );
                    break;
                }
                TmpNode1 = TmpNode1->next;
            }
            /* It is necessary a new color */
            if (TmpNode1 == NULL) {
                G->point[vertice[i]].color = GetNextColor(NEXT);

                /* Now, creates a new node in ColorSet */

                /* Firt creates the node, that is a list, and inserts
		 * in it the first vertex */
                SameColor = CreateList();
                InitList(SameColor);
		InsertNode(SameColor, 
			MakeNode((void*)&(vertice[i]), sizeof(int)));
		
                /* And than, insert this node in SameColor */
	        InsertNode(&ColorSet, MakeNode(SameColor, sizeof(List)));
			 
            }
        }
        sprintf(mess,"%s","Colored");
        return 1;
    
    /* If the graph is not chordal */
    } else {
        sprintf(mess,"%s","Not Chordal!");
        return 0;
    }
  
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

int GetNextColor(PalOpt opt) {
static int color=0;

    if (opt == RESET) {
        color = 0;
    } else {
        color++;
    }
    return color;
}


/* ******* */
/* * End * */
/* ******* */
