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
class GNGNode : public vector<GNGEdge*>{
public:
    typedef vector<GNGEdge*>::iterator EdgeIterator;
    
    
    
    static int dim;
    
    int error_cycle;
    double error;
    double error_new;
    int edgesCount;
    double * position;
    int nr;
    bool occupied;
    
    virtual void init(){
        
    }
    GNGNode():error(0.0),error_cycle(0),occupied(false),nr(-1),edgesCount(0){
        
    }
    
   
    double dist(GNGNode * gnode) const{ //dist doesnt account for param
             using namespace std;
             double ret=0;
             REP(i,dim){
                     ret+=(this->position[i]-gnode->position[i])*
                         (this->position[i]-gnode->position[i]);
             }
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


/**
* GNG Edge kept in RAM memory.
*/
class RAMGNGEdge: public GNGEdge { public:
    RAMGNGEdge():GNGEdge(){}
    RAMGNGEdge(int nr):GNGEdge(nr){
    }
};
#include <iostream>
/**
* GNG Node kept in RAM memory.
*/
class RAMGNGNode: public GNGNode
{ 
public:
    
    void operator=(RAMGNGNode & other){
        GNGNode::operator=(other);
        other.position = 0; //basically it is a move operator so cheating a little bit
    }
    
    void init(){
        this->position = new double[dim];
    }
    
    int nextFree;
     RAMGNGNode(): nextFree(-1), GNGNode(){
         this->position = 0;
     }
     ~RAMGNGNode(){
         if(position) delete [] position;
     }
};



/* Implementations of graph primitives in shared memory. Obsolete*/
//class SHGNGEdge;
//
//typedef boost::interprocess::allocator<SHGNGEdge, boost::interprocess::managed_shared_memory::segment_manager> ShmemAllocatorGNG;
//typedef boost::interprocess::vector<SHGNGEdge, ShmemAllocatorGNG> GNGVectorTempl;
//typedef boost::interprocess::list<SHGNGEdge, ShmemAllocatorGNG> GNGListTempl;
//
//typedef boost::interprocess::list<SHGNGEdge, ShmemAllocatorGNG> GNGList;
//typedef typename GNGListTempl::iterator GNGListTemplIterator;
//
//
///**
//* GNG Edge kept in shared memory.
//*/
//class SHGNGEdge: public GNGEdge{ public:
//    GNGListTemplIterator rev;
//    
//    SHGNGEdge():GNGEdge(){}
//    SHGNGEdge(int nr):GNGEdge(nr){
//    }
//};
//
///**
//* GNG Node kept in shared memory.
//*/
//class SHGNGNode: public GNGNode
//{ 
//public:
//    typedef GNGList EdgeStorage;
//    
//    
//    static MemoryAllocator * mm;
//    static ShmemAllocatorGNG * alloc_inst;
//    int nextFree;
//    double position_storage[GNG_MAX_DIMENSION]; //wymiar//
//    GNGList edges; //jest w shm mam nadzieje, chyba tak. do sprawdzenia
//    
//    //temporarily moved to private
//    bool operator=(const  SHGNGNode & r){
//        double * position = this->position;
//        GNGNode::operator=(r);
//        this->position = position;
//        memcpy(position, r.position, sizeof(double)*dim); //TODO: get rid of this 
//    }
//    
//    
//     /**
//     * @note Hack for memory be allocated to in SHM space (g_pool)
//     */
//     void* operator new[](std::size_t size){
//         return mm->allocate(size);
//         
//     }
//     void * operator new(std::size_t size){
//         return mm->allocate(size);
//          
//     }
//     void operator delete(void * ptr){
//         mm->deallocate(ptr);
//     }
//     void operator delete[](void * ptr){
//         mm->deallocate(ptr);
//     }
//
//     
//     SHGNGNode():nextFree(-1), edges(*alloc_inst){
//         this->position = &position_storage[0];
//     }
//     
//
//
//};


//
//class GNGNodeOffline{ public:
//     
//     friend std::ostream& operator<<(std::ostream& out, const GNGNodeOffline & node){
//         out<<node.nr<<"(";
//         for(int i=0;i<GNG_DIM;++i){
//             out<<node.position[i]<<",";
//         }
//         
//         out<<")";
//         return out;
//     }
//
//     
//
//     bool operator=(const SHGNGNode & rhs){
//         if(SIZE(edges)) edges.clear();
//         
//         this->edgesCount = rhs.edgesCount;
//         this->nr = rhs.nr;
//         this->occupied = rhs.occupied;
//         this->nextFree = rhs.nextFree;
//         memcpy(&position[0],&rhs.position[0],(GNG_DIM+1)*sizeof(double)); //param copy
//         
//         if(rhs.edgesCount==0 || SIZE(rhs.edges)==0) return true; //nie kopiuj
//         
//         int i=0;
//         FOREACH(edg, rhs.edges){
//             if((i++)==edgesCount) break;
//             edges.push_back(*edg); //rev nieistotne
//             
//         }
//         //*(this->edges) = *(rhs.edges);
//     }
//
//     
//    double position[GNG_MAX_DIMENSION]; //wymiar//
//    
//     
//    int edgesCount;
//    int nr;
//    bool occupied;
//    int nextFree;
//    
//    std::list<SHGNGEdge> edges; //jest w shm mam nadzieje, chyba tak. do sprawdzenia
//};


#endif        /* SHGRAPHDEFS_H */