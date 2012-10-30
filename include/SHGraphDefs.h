/* 
 * File:   SHGraphDefs.h
 * Author: staszek
 *
 * Created on 11 sierpień 2012, 08:18
 */

#ifndef SHGRAPHDEFS_H
#define	SHGRAPHDEFS_H

#define GNG_MAX_DIMENSION 100 //wazne dla szybkosci

#include <boost/interprocess/containers/list.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/offset_ptr.hpp>

#include "SHMemoryManager.h"
#include "GNGDatabase.h"
#include "GNGGlobals.h"



class GNGEdge;

typedef boost::interprocess::allocator<GNGEdge, boost::interprocess::managed_shared_memory::segment_manager>  ShmemAllocatorGNG;
typedef boost::interprocess::vector<GNGEdge, ShmemAllocatorGNG> GNGVectorTempl;
typedef boost::interprocess::list<GNGEdge, ShmemAllocatorGNG> GNGListTempl;

typedef  boost::interprocess::list<GNGEdge, ShmemAllocatorGNG> GNGList;
typedef typename GNGListTempl::iterator GNGListTemplIterator;

class GNGEdge{ public:
    
    int nr;
    double error;
    int age;
    //std::list<GNGEdge>::iterator rev;
    GNGListTemplIterator rev;
    GNGEdge():error(0),age(0){}
    GNGEdge(int nr):nr(nr),error(0.0),age(0){}
    
};




class GNGNode{ public:
    static ExtMemoryManager * mm;
    static ShmemAllocatorGNG * alloc_inst;
     void* operator new[](std::size_t size){
         return mm->allocate(size);
         
     }
     void * operator new(std::size_t size){
         return mm->allocate(size);
          
     }
     void operator delete(void * ptr){
         
         mm->deallocate(ptr);

     }


     friend std::ostream& operator<<(std::ostream& out, const GNGNode & node){
         out<<node.nr<<"("<<node.error<<")(";
         for(int i=0;i<GNG_DIM;++i){
             out<<node.position[i]<<",";
         }
         out<<")";

        if (node.edgesCount != 0) {
            FOREACH(it2, (node.edges)) out << (it2->nr)<<"|"<<it2->age << ",";
        }
        
         return out;
     }
     
     GNGNode():error(0.0),error_cycle(0),occupied(false),nr(-1),nextFree(-1),edgesCount(0),edges(*alloc_inst){
     
     }
     
    double position[GNG_MAX_DIMENSION]; //wymiar//
    
    int error_cycle;
    double error; 
    double error_new;
    int edgesCount; 
    int nr;
    bool occupied;
    int nextFree;
    
    //std::list<GNGEdge> * edges;
    GNGList edges; //jest w shm mam nadzieje, chyba tak. do sprawdzenia
};   



class GNGNodeOffline{ public:
     
     friend std::ostream& operator<<(std::ostream& out, const GNGNodeOffline & node){
         out<<node.nr<<"(";
         for(int i=0;i<GNG_DIM;++i){
             out<<node.position[i]<<",";
         }
         
         out<<")";
         return out;
     }

     
     //TODO: workaround with std::copy failure
     bool operator=(const GNGNode & rhs){
         if(SIZE(edges)) edges.clear();
         
         this->edgesCount = rhs.edgesCount;
         this->nr = rhs.nr;
         this->occupied = rhs.occupied;
         this->nextFree = rhs.nextFree;
         memcpy(&position[0],&rhs.position[0],GNG_DIM*sizeof(double));
         
         if(rhs.edgesCount==0 || SIZE(rhs.edges)==0) return true; //nie kopiuj
         
         int i=0;
         FOREACH(edg, rhs.edges){
             if((i++)==edgesCount) break;
             edges.push_back(*edg); //rev nieistotne
             
         }
         //*(this->edges) = *(rhs.edges);
     }

     
    double position[GNG_MAX_DIMENSION]; //wymiar//
    
     
    int edgesCount; 
    int nr;
    bool occupied;
    int nextFree;
    
    std::list<GNGEdge> edges; //jest w shm mam nadzieje, chyba tak. do sprawdzenia
};   


#endif	/* SHGRAPHDEFS_H */

