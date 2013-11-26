/* 
 * File:   RcppInterfejs.h
 * Author: staszek
 *
 * Created on 11 sierpień 2012, 19:07
 */

#ifndef RCPPINTERFEJS_H
#define	RCPPINTERFEJS_H
#undef DEBUG



#include <RcppArmadillo.h>

#include "GNG.h"
#include "Utils.h"



struct GNGClient {
    /** @note All dangling pointers on purpose ! We do not own these !*/
    
    MyMutex * grow_mutex;
    GNGDatabase * g_database; 
    GNGAlgorithmControl *control; 
    GNGGraph * graph;
    vector<GNGNodeOffline> buffer;      
};



RcppExport SEXP GNGClient__new(SEXP);
RcppExport SEXP GNGClient__getNumberNodes(SEXP _xp);
RcppExport SEXP GNGClient__getNode(SEXP _xp,SEXP _nr);
RcppExport SEXP GNGClient__addExamples(SEXP _xp, SEXP _examples);

#endif	/* RCPPINTERFEJS_H */
