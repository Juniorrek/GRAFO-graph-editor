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


/* *********** */
/* * jewel.c * */
/* *********** */


/* Plugin example: finds maximum clique */
/* Warning: this plugin is in development! */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../pgin.h"
#include "../../graph.h"
#include "../../list.h"
#include "../../alghandler.h"

Pgin pgin[2] = {
    { PGIN_RESULT_ALG, "Find Jewel", "Jewel" },
    { PGIN_LIST_END, NULL, NULL },
};


Pgin *pgin_info(void);
int Jewel(Graph *G, char *mess, List *arc_edges, List *verts);
int check3(Graph *G, int v[]);
int **getpermut(int n, int *v);
int factorial(int n);

Pgin *pgin_info(void){

    return pgin;
}


/* Implementation of the algorithm of Chudnovsky and Seymour [XX] to
 * find jewels */
int Jewel(Graph *G, char *mess, List *arc_edges, List *verts){
List *F, X1, X2;
Vertex *V,*V1,*V2;
Node N,N1,N2;
Graph *H;
int f3, /* 3! */
    bin_n_3,  /* n escolhe 3 */
    found=0,
    FoundJewel=0,
    xpos,xpos1,xpos2,
    i,j,k,l,m,n,
    h3[3],
    tmp[3],
    **v,
    **tuples, /* 3-tuples */
    count=0, /* Number of tuples*/
    **recordX1, **recordX2,
	Hvertex=-1;

    if (G->size < 5) {
        sprintf(mess, "%s", "No T2 Structure! (|V(G)|<5)");
        return 1;
    }

    
    CleanList(verts);
    CleanList(arc_edges);
	
    /* "Enumerate all 3-tuples v2, v3, v5 of distinct vertices 
	 * such that v2v3 is an edge." */
	
    v = getpermut(3, &f3);

    bin_n_3 = (G->size) * (G->size - 1) * (G->size -2) * f3;
	
    tuples = (int **) malloc(3 * sizeof(int *));
    for (i = 0; i < 3; i++) {
        tuples[i] = (int *) malloc(bin_n_3 * sizeof(int));
    }

    for (i=0; i<G->size; i++) {
        for (j=i+1; j<G->size; j++) {
            for (k=j+1; k<G->size; k++) {
                tmp[0] = i;
                tmp[1] = j;
                tmp[2] = k;
                for (l=0;l<f3;l++) {
                    for (m=0; m<3; m++) {
                        h3[m] = tmp[v[m][l]-1];
                    }
                    if (found = check3(G,h3)) {
                        for (n=0; n<3; n++) {
                            tuples[n][count] = h3[n];
                        }
                        count++;
                    }
                }
            }
        }
    }

    for (j=0; j<3; j++) {
        free(v[j]);
    }
    free(v);

    V = (Vertex *) malloc (sizeof(Vertex));

    F = (List *) malloc (count * sizeof(List));
    for (i=0; i < count; i++) {
        InitList(&(F[i]));
    }

    /* "For each choice of v2, v3 and v5, find the set of all
	 * vertices nonadjacent to each of v2, v3, v5, ..." */
    for (i=0; i<count; i++) {
        for (j=0; j<G->size; j++) {
            /* "... find the set F of all vertices nonadjacent to each of
			 * v2, v3 and v5 and ..." */
            if ((j != tuples[0][i])&&
                 (j != tuples[1][i])&&
                 (j != tuples[2][i]) ){
                if ((!G->edge[j][tuples[0][i]]) &&
                    (!G->edge[j][tuples[1][i]]) &&
                    (!G->edge[j][tuples[2][i]])) {
                    *V = j;
                    /* Marca o primeiro vertice */
                    N = MakeNode(V, sizeof(int));
                    InsertNode(&(F[i]), N);
                }
            }
        }
    }
    free(V);

    /* ... find all its components. */
    for (i=0; i < count; i++) {
        /* The graph H will be the induced subgraph by F (that will be 
        defined bellow) */
        H = (Graph *) malloc (sizeof (Graph));
        memcpy(H, G, sizeof(Graph));
        V = (Vertex *) malloc (sizeof(Vertex));
        
        N = F[i].head;
        while (N != NULL) {
            memcpy(V, N->data, sizeof(Vertex));
            H->point[(int)*V].mark = 1;
            N = N->next;
        }
        
        j=0;
        while (j<H->size) {
            if (H->point[j].mark) {
                j++;
            } else {
                RemovePoint(H, j);
            }
        }

        RunAlgorithm("Conected Components", H, mess, 0, arc_edges, verts);
        CleanList(verts);
        CleanList(arc_edges);

        InitList(&X1);
        /* */
        for (j=0; j<G->size; j++) {
            if ((j != tuples[0][i])&&
                 (j != tuples[1][i])&&
                 (j != tuples[2][i]) ){
                if ((G->edge[j][tuples[0][i]]) &&
                    (!G->edge[j][tuples[1][i]]) &&
                    (G->edge[j][tuples[2][i]])) {
                    *V = j;
                    N = MakeNode(V, sizeof(int));
                    InsertNode(&X1, N);
                }
            }
        }
 
        /* "... record whether v1 has a neighbour in F'." */
        recordX1 = (int **) malloc (X1.size * sizeof(int *));
        for (j=0; j<X1.size; j++) {
            recordX1[j] = (int *) malloc (H->ncomp * sizeof(int));
            memset(recordX1[j], 0, H->ncomp * sizeof(int));
        }

        N = X1.head;
        xpos = 0;
        while (N != NULL) {
            memcpy(V, N->data, sizeof(Vertex));
            for (j=0; j<G->size; j++) {
                if ((G->edge[(int)*V][j]) && 
                    ((Hvertex = IDinG(H,G->point[j].id)) != -1) ) {
                    recordX1[xpos][H->point[Hvertex].concomp]= 1;
                }
            }
            N = N->next;
            xpos++;
        }

        /* Do the same for the set X2...*/
        InitList(&X2);
        for (j=0; j<G->size; j++) {
            if ((j != tuples[0][i])&&
                 (j != tuples[1][i])&&
                 (j != tuples[2][i]) ){
                if ((!G->edge[j][tuples[0][i]]) &&
                    (G->edge[j][tuples[1][i]]) &&
                    (G->edge[j][tuples[2][i]])) {
                    *V = j;
                    N = MakeNode(V, sizeof(int));
                    InsertNode(&X2, N);
                }
            }
        }
        
        /* "... record whether v1 has a neighbour in F'." */
        recordX2 = (int **) malloc (X2.size * sizeof(int *));
        for (j=0; j<X2.size; j++) {
            recordX2[j] = (int *) malloc (H->ncomp * sizeof(int));
            memset(recordX2[j], 0, H->ncomp * sizeof(int));
        }

        N = X2.head;
        xpos = 0;
        while (N != NULL) {
            memcpy(V, N->data, sizeof(Vertex));
            for (j=0; j<G->size; j++) {
                if ((G->edge[(int)*V][j]) &&
                    ((Hvertex = IDinG(H,G->point[j].id)) != -1) ){
                    recordX2[xpos][H->point[Hvertex].concomp]= 1;
                }
            }
            N = N->next;
            xpos++;
        }

        V1 = (Vertex *) malloc (sizeof(Vertex));
        V2 = (Vertex *) malloc (sizeof(Vertex));
    
        N1 = X1.head;
        xpos1=0;
        while (N1 != NULL) {
            memcpy(V1, N1->data, sizeof(Vertex));
            N2 = X2.head;
            xpos2=0;
            while (N2 != NULL) {
                memcpy(V2, N2->data, sizeof(Vertex));
                if (!G->edge[*V1][*V2]){
                    for (j=0; j<H->ncomp; j++) {
                        if ((recordX1[xpos1][j])&&
                            (recordX2[xpos2][j]) ){
                            FoundJewel = 1;
                            
                            /* Mark the vertices */
                            V = (Vertex *) malloc (sizeof(Vertex));
							for (k=0;k<3;k++) {
                                *V = tuples[k][i];
                                N = MakeNode(V, sizeof(int));
                                InsertNode(verts, N);
							}
                            *V = *V1;
                            N = MakeNode(V, sizeof(int));
                            InsertNode(verts, N);
                            *V = *V2;
                            N = MakeNode(V, sizeof(int));
                            InsertNode(verts, N);
                            printf("3-tupla: (%d,%d,%d), v1:%d, v4:%d.\n",
                              tuples[0][i],tuples[1][i],tuples[2][i],*V1,*V2);
							
                     free(V); free(V1); free(V2); free(H);
                     for(j=0;j<3;j++){free(tuples[j]);}free(tuples);
                     for(j=0;j<X1.size;j++){free(recordX1[j]);}free(recordX1);
                     for(j=0;j<X2.size;j++){free(recordX2[j]);}free(recordX2);
                     for(j=0;j<count;j++){CleanList(&(F[j]));}free(F);
                     CleanList(&X1); CleanList(&X2);
 
                            goto BYE;
                        }
                    }
                }
                N2 = N2->next;
                xpos2++;
            }
            N1 = N1->next;
            xpos1++;
        }

        free(V); free(V1); free(V2); free(H);
        for(j=0;j<X1.size;j++){free(recordX1[j]);}free(recordX1);
        for(j=0;j<X2.size;j++){free(recordX2[j]);}free(recordX2);
        CleanList(&X1); CleanList(&X2);
    }
    for(j=0;j<3;j++){free(tuples[j]);}free(tuples); 
    for(j=0;j<count;j++){CleanList(&(F[j]));}free(F);
	
BYE:
    

    if (FoundJewel) {
        sprintf(mess, "T2 structure found!");
printf("SIM\n");
    } else {
        sprintf(mess, "T2 structure not found!");
printf("NAO\n");
    }
    return 0;

}


int check3(Graph *G, int v[]) {

    if (G->edge[v[0]][v[1]]){
        return 1;
    } else {
        return 0;
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
