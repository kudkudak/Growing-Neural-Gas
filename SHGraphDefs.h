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

#include <list>

#include "SHMemoryManager.h"
#include "GNGDatabase.h"


/*
struct Edge;

class Node{ public:
    static ExtMemoryManager * mm;
    
     void* operator new[](std::size_t size){
         return mm->allocate(size);
         
     }
     void * operator new(std::size_t size){
         return mm->allocate(size);
          
     }
     void operator delete(void * ptr){
         mm->deallocate(ptr);
     }
     
    int nr;
    int edgesCount;
    bool occupied;
    int nextFree;
    std::vector<Edge> * edges;
};


   
class Edge{ public:
    int nr;
    Edge(int nr):nr(nr){}
};
   */

class SHEdge{ public:
    int nr;
    SHEdge(){}
    SHEdge(int nr):nr(nr){}
};

typedef boost::interprocess::allocator<SHEdge, boost::interprocess::managed_shared_memory::segment_manager>  ShmemAllocator;
typedef boost::interprocess::vector<SHEdge, ShmemAllocator> SHVectorTempl;


class SHVector;

class SHNode{ public:
    static ExtMemoryManager * mm;
    
     void* operator new[](std::size_t size){
         return mm->allocate(size);
         
     }
     void * operator new(std::size_t size){
         return mm->allocate(size);
          
     }
     void operator delete(void * ptr){
         mm->deallocate(ptr);
     }
     
    int edgesCount; 
    int nr;
    bool occupied;
    int nextFree;
    
    boost::interprocess::offset_ptr<SHVector> edges; //jest w shm mam nadzieje, chyba tak. do sprawdzenia
};

class SHVector: public SHVectorTempl{ public:
        static ShmemAllocator * alloc_inst;
	static ExtMemoryManager * mm;

     SHVector():SHVectorTempl(*alloc_inst){}

     

     void* operator new[](std::size_t size){
         return mm->allocate(size);
         
     }
     void * operator new(std::size_t size){
         return mm->allocate(size);
          
     }
     void operator delete(void * ptr){
         mm->deallocate(ptr);
     }
    
};

class GNGEdge;

typedef boost::interprocess::allocator<GNGEdge, boost::interprocess::managed_shared_memory::segment_manager>  ShmemAllocatorGNG;
typedef boost::interprocess::vector<GNGEdge, ShmemAllocatorGNG> GNGVectorTempl;
typedef boost::interprocess::list<GNGEdge, ShmemAllocatorGNG> GNGListTempl;

class GNGList;

typedef typename GNGListTempl::iterator GNGListTemplIterator;

class GNGEdge{ public:
    
    int nr;
    double error;
    int age;
    GNGListTemplIterator rev;
    GNGEdge():error(0),age(0){}
    GNGEdge(int nr):nr(nr),error(0.0),age(0){}
    
};


//how to implement iterators on this list? my own list rewrite? lets try it! but have to write iterator, that sucks.

class GNGList: public GNGListTempl{ public:

    
    static ShmemAllocatorGNG * alloc_inst;
    static ExtMemoryManager * mm;

 
     GNGList():GNGListTempl(*alloc_inst){}

     

     void* operator new[](std::size_t size){
         return mm->allocate(size);
         
     }
     void * operator new(std::size_t size){
         return mm->allocate(size);
          
     }
     void operator delete(void * ptr){
         mm->deallocate(ptr);
     }
    
};








































//TODO: hack with GNGNode repair

class GNGNode{ public:
    static ExtMemoryManager * mm;
    
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
         for(int i=0;i<GNGExample::N;++i){
             out<<node.position[i]<<",";
         }
         out<<")";

        if (node.edgesCount != 0) {
            FOREACH(it2, *(node.edges)) out << (it2->nr) << ",";
        }
        
         return out;
     }
     
     GNGNode():error(0.0),occupied(false),nr(-1),nextFree(-1),edgesCount(0){
     
     }
     
    double position[GNG_MAX_DIMENSION]; //wymiar//
    
    double error; 
    int edgesCount; 
    int nr;
    bool occupied;
    int nextFree;
    
    boost::interprocess::offset_ptr<GNGList> edges; //jest w shm mam nadzieje, chyba tak. do sprawdzenia
};   



class GNGNodeOffline{ public:
     
     friend std::ostream& operator<<(std::ostream& out, const GNGNodeOffline & node){
         out<<node.nr<<"(";
         for(int i=0;i<GNGExample::N;++i){
             out<<node.position[i]<<",";
         }
         
         out<<")";
         return out;
     }

     
     //TODO: workaround with std::copy failure
     bool operator=(const GNGNode & rhs){
         this->edgesCount = rhs.edgesCount;
         this->nr = rhs.nr;
         this->occupied = rhs.occupied;
         this->nextFree = rhs.nextFree;
         memcpy(&position[0],&rhs.position[0],GNGExample::N*sizeof(double));
         
         if(rhs.edgesCount==0 || rhs.edges==0) return true; //nie kopiuj
         
         int i=0;
         FOREACH(edg, *rhs.edges){
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


//typedef GNGNodeTempl<GNGVectorPointerOnline> GNGNode;


#endif	/* SHGRAPHDEFS_H */

