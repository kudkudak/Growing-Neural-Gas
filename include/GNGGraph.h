/*
* File: SHGraph.h
* Author: staszek
*
* Created on 11 sierpień 2012, 09:07
*/

#ifndef GNGGraph_H
#define GNGGraph_H

#include "SHMemoryManager.h"
#include "ExtGraphNodeManager.h"
#include "SHGraphDefs.h"
#include "GNGGlobals.h"

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include <boost/interprocess/offset_ptr.hpp>


typedef ExtGraphNodeManager<SHGNGNode, SHGNGEdge, GNGList> SharedMemoryGraphStorage;


/** Graph interface for GNGAlgorithm. 
 * @note: The next step will be implementing this interface using igraph and external storage
 * such as disk space (with cache?)
 */
template <class Node>
class GNGGraph{
public:
    virtual double getErrorNodeShare(int i ) const = 0;

    virtual double getAccumulatedErrorShare() const= 0;

    virtual Node & operator[](int i) = 0;

    virtual int getNumberNodes() const = 0;
    
    virtual void init(int start_number) = 0;

    /* Get maximum index in the node pool */
    virtual int getMaximumIndex() const = 0;
    
    //TODO: move it to GNGNode
    virtual double getDist(int a, int b) = 0;

    virtual const double* getPosition(int nr) const = 0; 
    
    //TODO: move it to GNGNode
    virtual double getDist(const double * pos_a, const double * pos_b) const = 0;

    /* Initialize node with position attribute */
    virtual int newNode(double const *position)= 0;
    
    virtual bool deleteNode(int x)= 0;
    
    virtual bool isEdge(int a, int b) const= 0;
    
    virtual bool removeEdge(int a, int b)= 0;
    
    virtual void addUDEdge(int a, int b)= 0;
    
    virtual void addDEdge(int a, int b)= 0;
};
//TODO: rewrite to use as a composite


/* Shared GNG Graph class - thread safe graph using as storage template class
 Storage class should be a specialization of ExtGraphNodeManager
 */
template<class Storage>
class SHGNGGraph: public GNGGraph<SHGNGNode> {
    typedef boost::interprocess::interprocess_mutex Mutex;
    
    Mutex * m_mutex;
    
    Storage & storage;
public:

    bool isEdge(int a, int b) const{
        
        return storage.isEdge(a,b);
    }
    const double * getPosition(int nr) const{
        return storage[nr].position;
    }
    
    double getErrorNodeShare(int i ) const{
        return storage[i].error_new;
    }

    int getNumberNodes() const{
        return storage.getNumberNodes();
    }
    
    int getMaximumIndex() const{
        return storage.m_maximum_index;
    }
    
    //TODO: problem when growing ! Should be called *ONLY* by GNGAlgorithm
    SHGNGNode & operator[](int i){ return storage[i]; }
    
    double getAccumulatedErrorShare() const{
       double error=0.0;
       
       REP(i,storage.m_maximum_index+1){
            error+=storage[i].error_new;
        }
        return error;
    }
    
    SHGNGGraph(Mutex * mutex,  Storage * s) : m_mutex(mutex), storage(*s) {
    }




    void init(int start_number) {
        storage.init(start_number);
    }

    double getDist(int a, int b) {
        double distance = 0;
        for (int i = 0; i < GNG_DIM; ++i) {
            distance += (storage.operator[](a).position[i] -
            
            storage.operator[](b).position[i])*(storage.operator[](a).position[i] - storage.operator[](b).position[i]);
        }
        return distance;
    }

    double getDist(const double * pos_a, const double * pos_b) const {
        double distance = 0;
        for (int i = 0; i < GNG_DIM; ++i) {
            distance += (pos_a[i] - pos_b[i])*(pos_a[i] - pos_b[i]);
        }
        return distance;
    }


    int newNode(const double  *position) {

         if (storage.poolIsFull()) {
             DBG(10, "SHGNGGraph::newNode() growing pool");
            boost::interprocess::scoped_lock<Mutex>(*m_mutex);
            storage.growPool();
            
         }
        int i = storage.newNode();

        //TODO: get rid of GNG_DIM
        memcpy(&storage[i].position[0], position, sizeof (double) *(GNG_DIM)); //param

        storage[i].error = 0.0;
        storage[i].error_cycle = 0;
        storage[i].error_new = 0.0;
        return i;
    }
    bool deleteNode(int x){
        boost::interprocess::scoped_lock<Mutex>(*m_mutex);
        return storage.deleteNode(x);
    }
    bool removeEdge(int a, int b){
        boost::interprocess::scoped_lock<Mutex>(*m_mutex);
        return storage.removeEdge(a,b);
    }

  
    void addUDEdge(int a, int b){
        boost::interprocess::scoped_lock<Mutex>(*m_mutex);
        return storage.addUDEdge(a,b);
    }
    void addDEdge(int a, int b){
        boost::interprocess::scoped_lock<Mutex>(*m_mutex);
        return storage.addDEdge(a,b);
    }


    virtual ~SHGNGGraph() {

    }


private:

};




#endif        /* SHGRAPH_H */