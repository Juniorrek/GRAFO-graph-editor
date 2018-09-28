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
/* * euler.c * */
/* *********** */


/* Plugin example: test if the graph have an Euler Path */

#include <stdio.h>
#include <stdlib.h>
/* Here the GRAFO headers */
#include "../pgin.h"
#include "../graph.h"
#include "../alghandler.h"

/* You have to fill the pgin struct with information about your plugins.
 * Each position of the pgin struct is related with some function you
 * have programed. Below one example where you programmed five functions:
 * 
 * Pgin pgin[6] = {
 *     { PGIN_RESULT_ALG, "Find a Hamilton Path", "HPath", 0 },
 *     { PGIN_TEST_ALG, "Test Planarity", "PlanarGraph", 0 },
 *     { PGIN_TEST_ALG, "Test Bergeness", "BergeGraph", 0 },
 *     { PGIN_TRANSF, "Compute the complement", "Compl", PGIN_FLAG_NOREDRAW },
 *     { PGIN_TRANSF, "Line Graph", "LineGraph", 0 },
 *     { PGIN_LIST_END, NULL, NULL, 0 },
 * };
 *
 * In the last position you should put { PGIN_LIST_END, NULL, NULL }, to
 * end the list of plugins.
 * You have to put four pieces of information in the pgin struct:
 * -> The first is the class of your algorithm. The first entry
 * above is a Result Algorithm, so you have to put PGIN_RESULT_ALG
 * -> The second (in this case "Some Edge") is the name that will 
 * appear in the menu of GRAFO. 
 * -> The third is the name of the function in the code.
 * -> The fourth is a set of plugin flags. In the example of the 
 * fouth funtion the flag prevents that GRAFO redraws the resulting graph.
 */


/* Plugin Information */
Pgin pgin[2] = {
    { PGIN_TEST_ALG, "Euler Graph", "Euler" },
    { PGIN_LIST_END, NULL, NULL },
};


/* The pgin_info() function should always be present. GRAFO will call
 * it to get the plugin information.
 */
Pgin *pgin_info(void);

Pgin *pgin_info(void){

    return pgin;
}


/* The plugin example:  This is a example of a "test algorithm". In this 
 * case you take a pointer to graph G and a string *mess and you have 
 * to test some propertie of the graph. Then you can return an int
 * and put something in the string *mess to be showed in the GRAFO
 * interface.
 * 
 * Here you will test if the graph has an Euler Path. 
 * We know from our first graph theory class that a graph has an 
 * Euler Path if and only if all vertices have even degree. So, we
 * only have to test it.
 * */

int Euler(Graph *G, char *mess);

int Euler(Graph *G, char *mess){
int i, flag = 0;
    
    if (G->size < 1) {
        sprintf(mess, "%s", "Empty graph!");
        return 1;
    }

    /* Test if there is some vertex with odd degree */
    for (i=0; i < G->size; i++) {
        if ((G->vertex[i].degree) % 2) {
            flag = 1;
        }
    }

    /* If there is such a vertex then the graph 
     * does not have an Euler Path */
    if (flag) {
        sprintf(mess,"%s","Not Eulerian");
        return 0;
    } else {
        sprintf(mess,"%s","Eulerian");
        return 1;
    }
        

}


/* ******* */
/* * End * */
/* ******* */
