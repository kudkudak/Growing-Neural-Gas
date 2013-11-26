/*
* File: SHGraphDefs.h
* Author: staszek
*
* Created on 11 sierpień 2012, 08:18
*/

#ifndef SHGRAPHDEFS_H
#define        SHGRAPHDEFS_H



#include <boost/interprocess/containers/list.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/offset_ptr.hpp>

#include "SHMemoryManager.h"
#include "GNGDatabase.h"
#include "GNGGlobals.h"

#include <cmath>



class SHGNGEdge;

typedef boost::interprocess::allocator<SHGNGEdge, boost::interprocess::managed_shared_memory::segment_manager> ShmemAllocatorGNG;
typedef boost::interprocess::vector<SHGNGEdge, ShmemAllocatorGNG> GNGVectorTempl;
typedef boost::interprocess::list<SHGNGEdge, ShmemAllocatorGNG> GNGListTempl;

typedef boost::interprocess::list<SHGNGEdge, ShmemAllocatorGNG> GNGList;
typedef typename GNGListTempl::iterator GNGListTemplIterator;


/**
* Basic interface for Node in GNGGraph.
* For information only, because actual GNGNode implementation doesn't
* inherit after this class (vtable perfomance issue).
 * 
 * 
 * Every inheriting class should implement = operator, but note, that = operator
 * doesn't copy edges
*/
class GNGNode{
public:
    int error_cycle;
    double error;
    double error_new;
    int edgesCount;
    int nr;
    bool occupied;
    
        
    GNGNode():error(0.0),error_cycle(0),occupied(false),nr(-1),edgesCount(0){
        
    }
    
    virtual double dist(GNGNode *) const = 0;
    
    static int size(){
        return 0;
    }
};

/**
* GNG Edge kept in shared memory.
*/
class SHGNGEdge{ public:
    
    int nr;
    double error;
    int age;
    GNGListTemplIterator rev;
    
    static int size(){
        return sizeof(int)*2 + sizeof(double)*1 + sizeof(GNGListTemplIterator)*1;
   }
    
    SHGNGEdge():error(0),age(0){}
    SHGNGEdge(int nr):nr(nr),error(0.0),age(0){}
};



/**
* GNG Node kept in shared memory.
*/
class SHGNGNode: public GNGNode
{
public:
    static MemoryAllocator * mm;
    static ShmemAllocatorGNG * alloc_inst;
    int nextFree;
    double position[GNG_MAX_DIMENSION]; //wymiar//
    GNGList edges; //jest w shm mam nadzieje, chyba tak. do sprawdzenia
    
    bool operator=(const  SHGNGNode & r){
        GNGNode::operator=(r);
        memcpy(position, r.position, sizeof(double)*GNG_MAX_DIMENSION); //TODO: get rid of this 
    }
    
     /**
     * @note Hack for memory be allocated to in SHM space (g_pool)
     */
     void* operator new[](std::size_t size){
         return mm->allocate(size);
         
     }
     void * operator new(std::size_t size){
         return mm->allocate(size);
          
     }
     void operator delete(void * ptr){
         mm->deallocate(ptr);
     }
     void operator delete[](void * ptr){
         mm->deallocate(ptr);
     }
     
     double dist(GNGNode * gnode) const{ //dist doesnt account for param
             using namespace std;
         SHGNGNode * node = reinterpret_cast<SHGNGNode*>(gnode); //no error handling for performance

             double ret=0;
             REP(i,GNG_DIM){
                     ret+=(this->position[i]-node->position[i])*
                         (this->position[i]-node->position[i]);
             }
             return sqrt(ret);
     }

     friend std::ostream& operator<<(std::ostream& out, const SHGNGNode & node){
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
     
     SHGNGNode():nextFree(-1),edges(*alloc_inst){
     
     }
     
     
     static int size(){
         //Note : of course non-static data can be measured by sizeof(), but
         //the point is that some GNGNode implementations can need more space
         //(by dynamic allocation on shared memory for instance).
         return sizeof(double)*GNG_MAX_DIMENSION + sizeof(double)*2 +
                 sizeof(int)*4 + sizeof(bool)*1;
     }
     

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

     

     bool operator=(const SHGNGNode & rhs){
         if(SIZE(edges)) edges.clear();
         
         this->edgesCount = rhs.edgesCount;
         this->nr = rhs.nr;
         this->occupied = rhs.occupied;
         this->nextFree = rhs.nextFree;
         memcpy(&position[0],&rhs.position[0],(GNG_DIM+1)*sizeof(double)); //param copy
         
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
    
    std::list<SHGNGEdge> edges; //jest w shm mam nadzieje, chyba tak. do sprawdzenia
};


#endif        /* SHGRAPHDEFS_H */