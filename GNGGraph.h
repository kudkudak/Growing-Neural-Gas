/* 
 * File:   SHGraph.h
 * Author: staszek
 *
 * Created on 11 sierpień 2012, 09:07
 */

#ifndef GNGGraph_H
#define	GNGGraph_H

#include "SHMemoryManager.h"
#include "ExtGraphNodeManager.h"
#include "SHGraphDefs.h"

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>




class GNGGraph : public ExtGraphNodeManager<GNGNode, GNGEdge, GNGVector>{
    boost::mutex * m_mutex;
    
public:
    static ExtMemoryManager * mm;
    typedef ExtGraphNodeManager<GNGNode, GNGEdge, GNGVector> super;
    
    GNGGraph(int start_number,boost::mutex * mutex):m_mutex(mutex),ExtGraphNodeManager<GNGNode, GNGEdge, GNGVector>(start_number){
        
    }
    GNGGraph(boost::mutex * mutex):m_mutex(mutex),ExtGraphNodeManager<GNGNode, GNGEdge, GNGVector>(){}
    GNGGraph(GNGNode * _g_pool, int _m_nodes, int _g_pool_nodes, int _m_first_free,boost::mutex * mutex):
    m_mutex(mutex),ExtGraphNodeManager<GNGNode, GNGEdge, GNGVector>
       ( _g_pool, _m_nodes,  _g_pool_nodes, _m_first_free)
    {}
    
    void init(int start_number){
        super::init(start_number);
    }
   
    GNGNode * operator[](int i){ return super::g_pool +i; }
    
    int newNode(double const *position){
       //m_mutex->lock();  
        
        int i=super::newNode();
        memcpy(&(super::g_pool+i)->position[0],position,sizeof(double)*GNGExample::N);
       
        
        //m_mutex->unlock();
        
        return i;
    }
    
    int newNode(GNGNode const *node){
        //m_mutex->lock();  
        
       // m_mutex->unlock();
        //not implemented
        return -1;
    }   
    
    void deleteNode(int i){
       //m_mutex->lock();  
         
        
        super::deleteNode(i);
        
       //m_mutex->unlock();
    }
    
    
    virtual ~GNGGraph(){
    
    }
    
    
     /*void* operator new[](std::size_t size){
         return mm->allocate(size);
         
     }
     void * operator new(std::size_t size){
         return mm->allocate(size);
          
     }
     void operator delete(void * ptr){
         mm->deallocate(ptr);
     } */
    
private:
  
};




#endif	/* SHGRAPH_H */

