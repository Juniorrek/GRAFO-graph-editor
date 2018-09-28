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
/* * conected.c * */
/* ************** */


/* Plugin example: test the graph conectivity */

#include <stdio.h>
#include <stdlib.h>
/* Here the GRAFO headers */
#include "../pgin.h"
#include "../graph.h"
#include "../alghandler.h"

Pgin pgin[2] = {
    { PGIN_TEST_ALG, "Conectivity", "Conected" },
    { PGIN_LIST_END, NULL, NULL },
};


Pgin *pgin_info(void);
int Conected(Graph *G, char *mess);


Pgin *pgin_info(void){

    return pgin;
}


int Conected(Graph *G, char *mess){
List E, V;
	
    if (G->size < 1) {
        sprintf(mess, "%s", "Empty graph!");
        return 1;
    }

    InitList(&E);
    InitList(&V);
    
    if (RunAlgorithm("Span. Tree", G,mess, 0, &E,&V)) {
        if ((E.size+1) == G->size) {
            sprintf(mess,"%s","Conected");
        } else {
            sprintf(mess,"%s","Disconected");
        }
    } else {
        printf("Erro: problemas chamando algoritmo (built-in) Span. Tree!\n");
        sprintf(mess,"%s","Erro!");
    }

    CleanList(&E); 
    CleanList(&V); 
    
    return 1;
}


/* ******* */
/* * End * */
/* ******* */
