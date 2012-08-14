/* 
 * File:   SHGraphDefs.h
 * Author: staszek
 *
 * Created on 11 sierpień 2012, 08:18
 */

#ifndef SHGRAPHDEFS_H
#define	SHGRAPHDEFS_H

#define GNG_MAX_DIMENSION 100 //wazne dla szybkosci

#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/offset_ptr.hpp>

#include "SHMemoryManager.h"
#include "GNGDatabase.h"

#include <iostream>
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

class GNGVector;
class GNGEdge{ public:
    int nr;
    GNGEdge(){}
    GNGEdge(int nr):nr(nr){}
};

typedef boost::interprocess::allocator<GNGEdge, boost::interprocess::managed_shared_memory::segment_manager>  ShmemAllocatorGNG;
typedef boost::interprocess::vector<GNGEdge, ShmemAllocatorGNG> GNGVectorVectorTempl;



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
         out<<node.nr<<"(";
         for(int i=0;i<GNGExample::N;++i){
             out<<node.position[i]<<",";
         }
         out<<")";
         return out;
     }
     

     
    double position[GNG_MAX_DIMENSION]; //wymiar//
    
     
    int edgesCount; 
    int nr;
    bool occupied;
    int nextFree;
    
    boost::interprocess::offset_ptr<GNGVector> edges; //jest w shm mam nadzieje, chyba tak. do sprawdzenia
};

class GNGVector: public SHVectorTempl{ public:
        static ShmemAllocator * alloc_inst;
	static ExtMemoryManager * mm;

     GNGVector():SHVectorTempl(*alloc_inst){}

     

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

   

#endif	/* SHGRAPHDEFS_H */

