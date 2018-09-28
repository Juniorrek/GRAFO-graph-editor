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


/* ******** */
/* * t1.c * */
/* ******** */


/* Plugin example: finds maximum clique */
/* Warning: this plugin is in development! */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../pgin.h"
#include "../../graph.h"
#include "../../list.h"

Pgin pgin[2] = {
    { PGIN_RESULT_ALG, "Find T1 Structure", "T1" },
    { PGIN_LIST_END, NULL, NULL },
};


Pgin *pgin_info(void);
int T1(Graph *G, char *mess, List *arc_edges, List *verts);
int checkH5(Graph *G, int v[]);
int **getpermut(int n, int *v);
int factorial(int n);

Pgin *pgin_info(void){

    return pgin;
}


int T1(Graph *G, char *mess, List *arc_edges, List *verts){
Vertex *V;
Edge *E;
Node N;
int f5,     /* n! */
    found=0,
    i,j,k,l,m,n,o,
    h5[5],
    tmp[5],
    **v;

    if (G->size < 5) {
        sprintf(mess, "%s", "No T1 Structure!");
        return 1;
    }

    v = getpermut(5, &f5);

    for (i=0; i<G->size; i++) {
        for (j=i+1; j<G->size; j++) {
            for (k=j+1; k<G->size; k++) {
                for (l=k+1; l<G->size; l++) {
                    for (m=l+1; m<G->size; m++) {

                        tmp[0] = i;
                        tmp[1] = j;
                        tmp[2] = k;
                        tmp[3] = l;
                        tmp[4] = m;
                        for (n=0;n<f5;n++) {
                            for (o=0; o<5; o++) {
                                h5[o] = tmp[v[o][n]-1];
                            }
                            if (found = checkH5(G,h5)) {
                                goto SKIP;
                            }
                        }
//printf("\n\n");
                    }
                }
            }
        }
    }

SKIP:
//printf("\n");

    if (found) {
#if 0
		V = (Vertex *) malloc (sizeof(Vertex));
	   
		for (i=0; i<5; i++) {
			*V = h5[i];
			/* Marca o primeiro vertice */
			N = MakeNode(V, sizeof(int));
			InsertNode(verts, N);
		}
		free(V);
#endif

        E = (Edge *) malloc (sizeof(Edge));

        for (i=0; i<4; i++) {
            E->u = h5[i];
            E->v = h5[i+1];
            N = MakeNode(E, sizeof(Edge));
            InsertNode(arc_edges, N);
        }
    
        E->u = h5[4];
        E->v = h5[0];
        N = MakeNode(E, sizeof(Edge));
        InsertNode(arc_edges, N);

        free(E);
        sprintf(mess, "T1 structure found!");
        return 1;
    }

    sprintf(mess, "T1 structure not found!");
    return 0;

}


int checkH5(Graph *G, int v[]) {
int i;
int fault = 0;

#if 0
    for (i=0;i<5;i++) {
        printf("%d", v[i]+1);
    }
#endif
	
	i = 0;
	while (i < 4) {
        if (!G->edge[v[i]][v[i+1]]) {
            fault = 1;
        }
        i++;
	}

	if (!G->edge[v[4]][v[0]]) {
        fault = 1;
    }
	
	
	if (!fault) {
        if (G->edge[v[0]][v[2]] ||
            G->edge[v[0]][v[3]] ||
            G->edge[v[1]][v[3]] ||
            G->edge[v[1]][v[4]] ||
            G->edge[v[2]][v[4]])  {
            fault = 1;
		}
	}
#if  0
    if (fault) {
        printf("  ");
        return 0;
	} else {
        printf("X ");
        return 1;
	}
#endif
	if (fault) {
        return 0;
	} else {
        return 1;
	}

}


int **getpermut(int n, int *fn){
int i, /* loop variable */
    m, /* find the mth one to swap, 1st decrease from right*/
    k, /* right-most element with s[m] < s[k]*/
    q, /* interchange index */
    p, /* other interchange index */
    c, /* used for interchange of elements*/
    j,
    *s, /* the string for which to find all the permutations */
    **v, /* all permutations */
    facn;

    if (n <= 0)  /* quit if enter 0 or less */
        exit(0);
    
    facn = factorial(n);
    
    s = (int *) malloc(n * sizeof(int));

    v = (int **) malloc(n * sizeof(int *));
    for (i = 0; i < n; i++) {
        v[i] = (int *) malloc(facn * sizeof(int));
    }
		
    for (i = 0; i < n; i++){
        s[i] = i+1;
    }

    //printf("All the permutations on %d elements:\n", n);
    
    for (i = 0; i < n; i++){
        //printf("%d", s[i]);
        v[i][0] = s[i];
    }
    //printf(" ");


    
    for (i = 0; i < facn-1; i++){

        m = n - 2;  /* first decrease from right to left */

        while(s[m] > s[m+1])
            m--;

        k = n-1;  /* rightmost element s[k] with m < k */

        while(s[m] > s[k])
            k--;

        c = s[m];  /* swap(s[k], s[m]) */
        s[m] = s[k];
        s[k] = c;
        p = m+1;  /* swap(s[m+1] and s[n], s[m+2] and s[n-1], etc. */
        q = n-1;

        while (p < q){
            c = s[p]; s[p] = s[q]; s[q] = c;
            p++; q--;
        }  

        for (j = 0; j < n; j++){
            //printf("%d", s[j]);
            v[j][i+1] = s[j];
        }
        //printf(" ");

    }

    printf("\n"); /* no more to print */

    *fn = facn;
    return v;
    
}


int factorial(int n){

    if (n == 0)
        return 1;

    return n * factorial(n - 1);
}



/* ******* */
/* * End * */
/* ******* */
