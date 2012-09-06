/* 
 * File:   RcppInterfejs.h
 * Author: staszek
 *
 * Created on 11 sierpień 2012, 19:07
 */

#ifndef RCPPINTERFEJS_H
#define	RCPPINTERFEJS_H



#include <Rcpp.h>
#include <RcppArmadillo.h>

#include "GNG.h"


struct GNGClient {
    MyMutex * grow_mutex;
    GNGDatabase* g_database;
    GNGAlgorithmControl *control;
    GNGGraph * graph;
    vector<GNGNodeOffline> buffer;      
};



RcppExport SEXP GNGClient__new();
RcppExport SEXP GNGClient__getNumberNodes(SEXP _xp);
RcppExport SEXP GNGClient__getNode(SEXP _xp,SEXP _nr);
RcppExport SEXP GNGClient__addExamples(SEXP _xp, SEXP _examples);

#endif	/* RCPPINTERFEJS_H */
