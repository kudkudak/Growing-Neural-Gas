/*
* File: SHGraphDefs.h
* Author: staszek
*
* Created on 11 sierpień 2012, 08:18
*/

#ifndef SHGRAPHDEFS_H
#define SHGRAPHDEFS_H



#include <boost/interprocess/containers/list.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/offset_ptr.hpp>
#include "SHMemoryManager.h"
#include "GNGDatabase.h"
#include "GNGGlobals.h"
#include <cmath>
#include <vector>
#include <iterator>

/**
* Basic interface for Edge in GNGGraph.
*/
class GNGEdge{ 
public:
    GNGEdge * rev;
    int nr;
    double error;
    int age;

    GNGEdge(): error(0),age(0){}
    GNGEdge(int nr): nr(nr),error(0.0),age(0){}
};


/**
* Basic interface for Node in GNGGraph.
 * 
 * Every inheriting class should implement = operator, but note, that = operator
 * doesn't copy edges
*/
class GNGNode : public std::vector<GNGEdge*>{
public:
    //TODO: change to GNGEdge, no need for dandling pointers
    typedef std::vector<GNGEdge*>::iterator EdgeIterator;

    static int dim;
    
    int error_cycle;
    double error;
    double error_new;
    
    
    int edgesCount;
    double * position;
    int nr;
    
    bool occupied;
 
    
    GNGNode():error(0.0),error_cycle(0),occupied(false),nr(-1),edgesCount(0){
    }
    
    ~GNGNode(){
        delete[] position;
    }
    
    double dist(GNGNode * gnode) const{ //dist doesnt account for param
             using namespace std;
             double ret=0;
             for(int i=0;i<dim;++i)
                ret+=(this->position[i]-gnode->position[i])*(this->position[i]-gnode->position[i]);
             return sqrt(ret);
     }

     friend std::ostream& operator<<(std::ostream& out, const GNGNode & node){
         out<<node.nr<<"("<<node.error<<")(";
         for(int i=0;i<dim;++i){
             out<<node.position[i]<<",";
         }
         out<<")";
        
         return out;
     }   
};




#endif        /* SHGRAPHDEFS_H */