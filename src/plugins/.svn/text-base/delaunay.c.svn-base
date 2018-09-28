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


/* ************** */
/* * delaunay.c * */
/* ************** */


/* Plugin: create a Delaunay triangulation from a set of vertices */

#include <stdio.h>
#include <stdlib.h>
#include "../pgin.h"
#include "../graph.h"


Pgin *pgin_info(void);
int Delaunay(Graph *G, char *mess);


Pgin *pgin_info(void){
Pgin *pgin;

    pgin = (Pgin *) malloc(2 * sizeof(Pgin));
    
    pgin[0].type = PGIN_TRANSF_ALG;
    pgin[0].label = "Delaunay triangulation";
    pgin[0].name = "Delaunay";
    pgin[0].flags = PGIN_FLAG_NOREDRAW;

    pgin[1].type = PGIN_LIST_END;
    pgin[1].label = 0;
    pgin[1].name = 0;
    pgin[1].flags = 0;

    return pgin;
}


int Delaunay(Graph *G, char *mess){
int *x, *y, *z;  /* Point coordinates */
int n;           /* Number of points */
int i, j, k, m;  /* Indices of four points */
int xn, yn, zn;  /* Outward vector normal to (i,j,k) */
int flag;        /* True if m above of (i,j,k) */
int f = 0;       /* Number of lower faces */

    /* Return if fewer than 3 vertices */
    if (G->size < 3) {
        return 1;
    }

    /* Reset graph edges */
    InitEdges(G);

    /* Alocate point arrays */
    n = G->size;
    x = (int *) malloc(n * sizeof(int));
    y = (int *) malloc(n * sizeof(int));
    z = (int *) malloc(n * sizeof(int));

    /* Input points and compute z = x^2 + y^2. */
    for (i = 0; i < n; i++){
        x[i] = G->vertex[i].x;
        y[i] = G->vertex[i].y;
        z[i] = x[i] * x[i] + y[i] * y[i];
    }

    /* For each triple (i,j,k) */
    for (i = 0; i < n - 2; i++)
    for (j = i + 1; j < n; j++)
    for (k = i + 1; k < n; k++) 
    if (j != k) {
        /* Compute normal to triangle (i,j,k). */
        xn = (y[j]-y[i])*(z[k]-z[i]) - (y[k]-y[i])*(z[j]-z[i]);
        yn = (x[k]-x[i])*(z[j]-z[i]) - (x[j]-x[i])*(z[k]-z[i]);
        zn = (x[j]-x[i])*(y[k]-y[i]) - (x[k]-x[i])*(y[j]-y[i]);

        /* Only examine faces on bottom of paraboloid: zn < 0. */
        if ((flag = (zn < 0)))
            /* For each other point m */
            for (m = 0; m < n; m++)
                /* Check if m above (i,j,k). */
                flag = flag && 
                        ((x[m]-x[i])*xn +
                         (y[m]-y[i])*yn +
                         (z[m]-z[i])*zn <= 0);
        if (flag) {
            G->edge[i][j] = 1;
            G->edge[j][i] = 1;
            G->edge[i][k] = 1;
            G->edge[k][i] = 1;
            G->edge[j][k] = 1;
            G->edge[k][i] = 1;
            f++;
        }
    }

    /* Free memory */
    free(x);
    free(y);
    free(z);

    sprintf(mess, "A total of %d lower faces found", f);

    return 1;
}


/* ******* */
/* * End * */
/* ******* */
