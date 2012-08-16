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


typedef  ExtGraphNodeManager<GNGNode, GNGEdge, GNGList> GNGGraphBase;

class GNGGraph : public GNGGraphBase{
    boost::mutex * m_mutex;
    
public:
    static ExtMemoryManager * mm;
    typedef ExtGraphNodeManager<GNGNode, GNGEdge, GNGList> super;
    
    GNGGraph(int start_number,boost::mutex * mutex):m_mutex(mutex),GNGGraphBase(start_number){
        
    }
    GNGGraph(boost::mutex * mutex):m_mutex(mutex),GNGGraphBase(){}
    GNGGraph(GNGNode * _g_pool, int _m_nodes, int _g_pool_nodes, int _m_first_free,boost::mutex * mutex):
    m_mutex(mutex),GNGGraphBase
       ( _g_pool, _m_nodes,  _g_pool_nodes, _m_first_free)
    {}
    
    void init(int start_number){
        super::init(start_number);
    }
   
    double getDist(int a, int b){
        double distance=0;
        for(int i=0;i<GNGExample::N;++i){
            distance += (super::operator[](a)->position[i]-super::operator[](b)->position[i])*(super::operator[](a)->position[i]-super::operator[](b)->position[i]);
        }
        return distance;
    }
    double getDist(double * pos_a, double * pos_b){
        double distance=0;
        for(int i=0;i<GNGExample::N;++i){
            distance += (pos_a[i]-pos_b[i])*(pos_a[i]-pos_b[i]);
        }
        return distance;
    }
        
    
    
    int newNode(double const *position){
        
        int i=super::newNode();
        
        //TO-DO: implementation depend !! (depre* after change to arma for example)
        memcpy(&(super::g_pool+i)->position[0],position,sizeof(double)*GNGExample::N);
       
  
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

