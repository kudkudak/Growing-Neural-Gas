/* 
 * File:   GNGGraphInfo.h
 * Author: staszek
 *
 * Created on 12 sierpień 2012, 10:56
 */

#ifndef GNGGRAPHINFO_H
#define	GNGGRAPHINFO_H

#include "GNGAlgorithm.h"


//zmienic design na update z get_info
struct GNGGraphInfo{
    typedef boost::interprocess::offset_ptr< GNGNode  > PoolPtr;
    
    
    int nodes;
    int pool_nodes;
    int first_free;
    double accumulated_error;
    int maximum_index;
    
    
    PoolPtr ptr;
    GNGAlgorithm * alg;

    
    GNGGraphInfo(){}
    GNGGraphInfo(GNGAlgorithm * alg, SHMemoryManager *shm): alg(alg){
        ptr = alg->get_graph().getPool();       
    }
    

    void update(){

        (ptr) = alg->get_graph().getPool();
        nodes = alg->get_graph().getNumberNodes();
        pool_nodes = alg->get_graph().getPoolNodes();
        first_free =alg->get_graph().getFirstFree();
        maximum_index = alg->get_graph().getMaximumIndex();   
        accumulated_error = alg->getAccumulatedError();

    }
    
    
    
};

#endif	/* GNGGRAPHINFO_H */

