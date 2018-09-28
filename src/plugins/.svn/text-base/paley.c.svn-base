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
/* * paley.c * */
/* *********** */


#include <stdio.h>
#include <stdlib.h>
#include "../pgin.h"
#include "../graph.h"


Pgin *pgin_info(void);
int paley(Graph *G, char *mess);


Pgin *pgin_info(void){
Pgin *pgin;

    pgin = (Pgin *) malloc(2 * sizeof(Pgin));
    
    pgin[0].type = PGIN_TRANSF_ALG;
    pgin[0].label = "Generate Paley Graph";
    pgin[0].name = "Paley";
    pgin[0].flags = 0;

    pgin[1].type = PGIN_LIST_END;
    pgin[1].label = 0;
    pgin[1].name = 0;
    pgin[1].flags = 0;

    return pgin;
}


int Paley(Graph *G, char *mess){
int n, i, j, k, f, v;
int *sq;

    /* Get number of vertices */
    n = pgin_read_int("Paley", "Number of vertices",
                5, 1, 2000000, 1);
    
    /* Generate field squares */
    sq = (int *) malloc(n * sizeof(int));
    for (i = 0; i < n; i++)
        sq[i] = (i*i)%n;

    /* Generate graph */
    InitGraph(G, G->type);
    
    for (i = 0; i < n; i++)
        AddPoint(G, 0, 0);

    for (i = 0; i < n; i++) { 
        for (j = 0; j < n; j++) {
            if (i != j){
                v = i-j;
              
                while (v < 0)
                    v += n; 
                        
                for (k = 0, f = 0; (k < n) && (!f); k++){
                    if (v == sq[k])
                        f = 1;
                }

                if (f)
                    AddArc(G, i, j);
            }
        }
    }

    free(sq);

    return 1;
}


/* ******* */
/* * End * */
/* ******* */
