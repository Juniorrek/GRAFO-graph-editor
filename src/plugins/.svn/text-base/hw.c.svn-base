/*
 *  grafo - graph editor
 *  Copyright (c) 2003
 *      Murilo Vicente Goncalves da Silva <murilo@pet.inf.ufpr.br>
 *      Oliver Matias van Kaick <oliver@pet.inf.ufpr.br>
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


/* ******** */
/* * hw.c * */
/* ******** */


/* Plugin example: hello world */

/* C language headers */
#include <stdio.h>
#include <stdlib.h>

/* Here the GRAFO headers */
/* Add plugins information */
#include "../pgin.h"

/* Add graph struct information */
#include "../graph.h"

/* Mandatory function pgin_info */
Pgin *pgin_info(void);

/* algorithm function */
int HelloWorld(Graph *G, char *mess);

/* Others functions */


/* The pgin_info() function should always be present. GRAFO will call
 * it to get the plugin information.
 */
Pgin *pgin_info(void){
Pgin *pgin;

    /* Alocate space for HelloWorld and PGIN_LIST_END */
    pgin = (Pgin *) malloc(2 * sizeof(Pgin));

    /* Includes HelloWorld as the first algorithm of plugin */
    pgin[0].type = PGIN_TEST_ALG;
    pgin[0].label = "Hello World!";
    pgin[0].name = "HelloWorld";
    pgin[0].flags = 0;

    /* It indicates the end of the list of algorithms */
    pgin[1].type = PGIN_LIST_END;
    pgin[1].label = 0;
    pgin[1].name = 0;
    pgin[1].flags = 0;

    return pgin;
}

/* This is the plugin's main function */
int HelloWorld(Graph *G, char *mess){

    sprintf(mess, "Hello World!");

    return 1;
}

/* ******* */
/* * End * */
/* ******* */
