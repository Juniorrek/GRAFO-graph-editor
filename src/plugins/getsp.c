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
/* * getsp.c * */
/* *********** */


/* Plugin example: get some input parameters */

#include <stdio.h>
#include <stdlib.h>
/* Here the GRAFO headers */
#include "../pgin.h"
#include "../graph.h"


Pgin *pgin_info(void);
int getsp(Graph *G, char *mess);


Pgin *pgin_info(void){
Pgin *pgin;

    pgin = (Pgin *) malloc(2 * sizeof(Pgin));

    pgin[0].type = PGIN_TEST_ALG;
    pgin[0].label = "Get some parameters";
    pgin[0].name = "getsp";

    pgin[1].type = PGIN_LIST_END;
    pgin[1].label = 0;
    pgin[1].name = 0;

    return pgin;
}


int getsp(Graph *G, char *mess){
int i;
double d;
char *s;

    i = pgin_read_int("Get some parameters", "Give me an int",
                10, 1, 2000000, 1);
    d = pgin_read_double("Get some parameters", "Give me a double",
                0.0, -10.0, 10.0, 0.1, 1);
    s = pgin_read_string("Get some parameters", "Give me a string",
                "I'm a string", 40);

    sprintf(mess, "I got: %d, %f and '%s'. Thanks!", i, d, s);
    free(s);

    return 1;
}


/* ******* */
/* * End * */
/* ******* */
