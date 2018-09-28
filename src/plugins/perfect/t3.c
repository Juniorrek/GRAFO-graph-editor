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
/* * t3.c * */
/* ******** */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../pgin.h"
#include "../../graph.h"
#include "../../list.h"
#include "../../alghandler.h"
#include "../../algorithms.h"


Pgin pgin[2] = {
    { PGIN_RESULT_ALG, "Find T3", "T3" },
    { PGIN_LIST_END, NULL, NULL },
};


Pgin *pgin_info(void);
int T3(Graph *G, char *mess, List *arc_edges, List *verts);
int checkP4(Graph *G, int v[]);
int **getpermut(int n, int *v);
int factorial(int n);

Pgin *pgin_info(void){

    return pgin;
}


int T3(Graph *G, char *mess, List *arc_edges, List *verts){
List *Yset; 
Vertex *V;
Node N, AUX;
Graph *H, *I;
int f4, /* 4! */
    bin_n_4,  /* n escolhe 4 */
    found=0,
    FoundT3=0,
    i,j,k,l,m,n,o,
    P4[4],
    tmp[4],
    **Xset,
    **v,
    **tuples, /* 4-paths */
    count=0, /* Number of tuples*/
    XComplete,
    Ivertex1,
    Ivertex2;

    if (G->size < 4) {
        sprintf(mess, "%s", "No T3 Structure! (|V(G)|<4)");
        return 1;
    }

    
    CleanList(verts);
    CleanList(arc_edges);
	
    /* "Enumerate all paths v1-v2-v3-v4 of G" */
	
    v = getpermut(4, &f4);

    bin_n_4 = (G->size) * (G->size - 1) * (G->size - 2) * (G->size - 3) * f4;
	
    tuples = (int **) malloc(4 * sizeof(int *));
    for (i = 0; i < 4; i++) {
        tuples[i] = (int *) malloc(bin_n_4 * sizeof(int));
    }


    for (i=0; i<G->size; i++) {
        for (j=i+1; j<G->size; j++) {
            for (k=j+1; k<G->size; k++) {
                for (l=k+1; l<G->size; l++) {
                    tmp[0] = i;
                    tmp[1] = j;
                    tmp[2] = k;
                    tmp[3] = l;
                    for (m=0; m<f4; m++) {
                        for (n=0; n<4; n++) {
                            P4[n] = tmp[v[n][m]-1];
                        }
                        if (found = checkP4(G,P4)) {
                            for (o=0; o<4; o++) {
                                tuples[o][count] = P4[o];
                            }
                            count++;
                        }
                    }
                }
            }
        }
    }

    for (j=0; j<4; j++) {
        free(v[j]);
    }
    free(v);

    V = (Vertex *) malloc (sizeof(Vertex));

    Yset = (List *) malloc (count * sizeof(List));
    for (i=0; i < count; i++) {
        InitList(&(Yset[i]));
    }
printf("All 3-tuples generated\n");

    /* "For each one, find the set Yset of all {v1,v2,v4}-complete vertices." */
    for (i=0; i<count; i++) {
        for (j=0; j<G->size; j++) {
            /* "... find the set F of all vertices nonadjacent to each of
			 * v2, v3 and v5 and ..." */
//            if ((j != tuples[0][i])&&
//                 (j != tuples[1][i])&&
//                 (j != tuples[3][i]) ){
                if ((G->edge[j][tuples[0][i]]) &&
                    (G->edge[j][tuples[1][i]]) &&
                    (G->edge[j][tuples[3][i]])) {
                    *V = j;
                    /* Marca o primeiro vertice */
                    N = MakeNode(V, sizeof(int));
                    InsertNode(&(Yset[i]), N);
                }
//            }
        }
    }
    free(V);

printf("Number of 3-tuplas:%d\n",count);
    
    /* ... find all its anticomponents. */
    for (i=0; i < count; i++) {

//if (!(i % 500))
//printf("Examinadas %d tuplas\n",i);
printf("[%d] ",i);
fflush(stdout);
        
        H = (Graph *) malloc (sizeof (Graph));
        memcpy(H, G, sizeof(Graph));
        V = (Vertex *) malloc (sizeof(Vertex));
        
        N = Yset[i].head;
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
 
        Complement(H, mess);
        RunAlgorithm("Conected Components", H, mess, 0, arc_edges, verts);
        CleanList(verts);
        CleanList(arc_edges);

        /* Xset is a table that records in what component of the complement of 
         * the induced subgraph by Y some vertex lies */
        Xset = (int **) malloc (G->size * sizeof(int *));
        for (j=0; j<G->size; j++) {
            Xset[j] = (int *) malloc (H->ncomp * sizeof(int));
            memset(Xset[j], 0, H->ncomp * sizeof(int));
        }
        /* recording */
        for (j=0; j<H->size; j++) {
            Xset[H->point[j].id][H->point[j].concomp] = 1;
        }

        /* "For each anticomponent X, test if there is a path P between
         * v1, v4 in G\{v2,v3}, such that no internal vertex of P is
         * adjacent to v2 or v3, and no internal vertex of P 
         * is X-complete." */
        
        /* for each anticomponent */
        for (j=0; j<H->ncomp; j++) {
            I = (Graph *) malloc (sizeof (Graph));
            memcpy(I, G, sizeof(Graph));

            for (k=0; k<I->size; k++) {
                /* Such that no internal vertex of P is djacent to v2 or v3 */
                if ((I->edge[k][tuples[1][i]])||
                    (I->edge[k][tuples[2][i]])) {

                    I->point[k].mark = 1;
                }
                /* No internal vertex of P is X-complete */
                XComplete = 1;
                for (l=0; l<G->size; l++) {
                    if ((!I->edge[k][l]) && 
                        (Xset[j][l]) ) {
                        XComplete = 0;
                    }
                }
                if (XComplete) {
                    I->point[k].mark = 1;
                }
            }
            
            /* Between v1, v4 in G\{v2,v3} */
            I->point[tuples[0][i]].mark = 0;
            I->point[tuples[1][i]].mark = 1; //redundant, since v2 is X-compl.
            I->point[tuples[2][i]].mark = 1;
            I->point[tuples[3][i]].mark = 0;

            k=0;
            while (k<I->size) {
                if (!I->point[k].mark) {
                    k++;
                } else {
//printf("Removendo%d\n",I->point[k].id);
                    RemovePoint(I, k);
                }
            }

            RunAlgorithm("Conected Components", I, mess, 0, arc_edges, verts);


              /* If the graph is one component */
              if (I->ncomp == 1) {

                FoundT3 = 1;

                /* Mark the vertices */
                V = (Vertex *) malloc (sizeof(Vertex));
                for (l=0;l<4;l++) {
                    *V = tuples[l][i];
                    N = MakeNode(V, sizeof(int));
                    InsertNode(verts, N);
                }

printf("p4: ");
for (l=0;l<4;l++) {
printf("%d-",tuples[l][i]);
}
printf("\n");

AUX = Yset[i].head;
while (AUX != NULL) {
 memcpy(V, AUX->data, sizeof(Vertex));
 printf("em X:%d\n", *V);
 AUX = AUX->next;
}

for (l=0; l<I->size; l++) {
printf("%d - %d\n", I->point[l].id, I->point[l].concomp);
}


                //*V = *V1;
                //N = MakeNode(V, sizeof(int));
                //InsertNode(verts, N);
                //*V = *V2;
                //N = MakeNode(V, sizeof(int));
                //InsertNode(verts, N);
                goto SKIP;
            }

            free(I);
        }
    }
 
SKIP: 

    if (FoundT3) {
        sprintf(mess, "T3 structure found!");
printf("SIM\n");
    } else {
        sprintf(mess, "T3 structure not found!");
printf("NAO\n");
    }
 
    return 0;

}


#if 0

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
        
        /* "... record whether v4 (v2 in the paper) has a neighbour in F'." */
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
                if ((!G->edge[*V1][*V2]) &&
                   (*V1 != tuples[0][i]) && 
                   (*V1 != tuples[1][i]) && 
                   (*V1 != tuples[2][i]) &&
                   (*V2 != tuples[0][i]) && 
                   (*V2 != tuples[1][i]) && 
                   (*V2 != tuples[2][i])){
                    for (j=0; j<H->ncomp; j++) {
                        if ((recordX1[xpos1][j])&&
                            (recordX2[xpos2][j]) ){
                            FoundJewel = 1;
printf("Ncomp:%d, H->size:%d,vert:%d\n", 
H->ncomp, H->size,H->point[0].id);  

                            /* Mark the vertices */
                            V = (Vertex *) malloc (sizeof(Vertex));
                            for (l=0;l<3;l++) {
                                *V = tuples[l][i];
                                N = MakeNode(V, sizeof(int));
                                InsertNode(verts, N);
                            }
                            *V = *V1;
                            N = MakeNode(V, sizeof(int));
                            InsertNode(verts, N);
                            *V = *V2;
                            N = MakeNode(V, sizeof(int));
                            InsertNode(verts, N);

AUX = X1.head;
while (AUX != NULL) {
 memcpy(V, AUX->data, sizeof(Vertex));
 printf("X1:%d\n", *V);
 AUX = AUX->next;
}
AUX = X2.head;
while (AUX != NULL) {
 memcpy(V, AUX->data, sizeof(Vertex));
 printf("X2:%d\n", *V);
 AUX = AUX->next;
}


printf("3-tupla: (%d,%d,%d), v1:%d, v4:%d.\n",
   tuples[0][i],tuples[1][i],tuples[2][i],*V1,*V2);
AUX = Y[i].head;
while (AUX != NULL) {
 memcpy(V, AUX->data, sizeof(Vertex));
 printf("Nao adj:%d\n", *V);
 AUX = AUX->next;
}
							
                     free(V); free(V1); free(V2); free(H);
                     for(j=0;j<3;j++){free(tuples[j]);}free(tuples);
                     for(j=0;j<X1.size;j++){free(recordX1[j]);}free(recordX1);
                     for(j=0;j<X2.size;j++){free(recordX2[j]);}free(recordX2);
                     for(j=0;j<count;j++){CleanList(&(Y[j]));}free(Y);
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
    for(j=0;j<count;j++){CleanList(&(Y[j]));}free(Y);
	
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

#endif

int checkP4(Graph *G, int v[]) {

    if ((G->edge[v[0]][v[1]]) &&
        (G->edge[v[1]][v[2]]) &&
        (G->edge[v[2]][v[3]]) &&
        (!G->edge[v[0]][v[2]]) &&
        (!G->edge[v[0]][v[3]]) &&
        (!G->edge[v[1]][v[3]])){
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
