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

#include "Utils.h"



typedef boost::interprocess::interprocess_mutex MyMutex;
typedef boost::interprocess::offset_ptr< GNGNode  > PoolPtr;
typedef ExtGraphNodeManager<GNGNode, GNGEdge, GNGVector> GraphAccess;


struct GNGClient{
    GNGGraphInfo * ggi; //czyta z shared memory w new
    MyMutex * grow_mutex;
    GNGDatabase * g_database;
     GraphAccess * readGraph(){
        return new GraphAccess((ggi->ptr).get(),ggi->nodes,ggi->pool_nodes,ggi->first_free);
    }
    
    vector<GNGNode> buffer;  
     
};


/*
 * makes a copy of graph structure (as armadillo matrix)
 */


RcppExport SEXP GNGClient__new();
RcppExport SEXP GNGClient__getNumberNodes(SEXP _xp);
RcppExport SEXP GNGClient__getNode(SEXP _xp,SEXP _nr);
RcppExport SEXP GNGClient__addExamples(SEXP _xp, SEXP _examples);

#endif	/* RCPPINTERFEJS_H */
