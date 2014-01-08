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
#include <iostream>
using namespace std;

//typedef ExtGraphNodeManager<SHGNGNode, SHGNGEdge> SharedMemoryGraphStorage;
typedef ExtGraphNodeManager<RAMGNGNode, RAMGNGEdge> RAMMemoryGraphStorage;

#include <iterator>
/** Graph interface for GNGAlgorithm. 
 * @note: The next step will be implementing this interface using igraph and external storage
 * such as disk space (with cache?)
 */

class GNGGraph{
public: 
    virtual double getErrorNodeShare(int i ) const = 0;

    virtual double getAccumulatedErrorShare() const= 0;

    virtual GNGNode & operator[](int i) = 0;

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
    
    virtual typename GNGNode::EdgeIterator removeEdge(int a, int b)= 0;
    
    virtual void addUDEdge(int a, int b)= 0;
    
    virtual void addDEdge(int a, int b)= 0;
    
    virtual void print() = 0;
};
//TODO: rewrite to use as a composite


/* Shared GNG Graph class - thread safe graph using as storage template class
 Storage class should be a specialization of ExtGraphNodeManager
 */
template<class Storage, class Mutex = boost::mutex>
class ExtGNGGraph: public GNGGraph {
    typedef typename Storage::NodeClass Node;

    int dim;
    Mutex * m_mutex;
    
    
public:
    //Note: currently assumes single threaded access to the elements
    
    void print(){
        cout<<storage.reportPool()<<endl;
    }
    Storage & storage;
    bool isEdge(int a, int b) const{
//        boost::interprocess::scoped_lock<Mutex>(*m_mutex);
        return storage.isEdge(a,b);
    }
    const double * getPosition(int nr) const{
//        boost::interprocess::scoped_lock<Mutex>(*m_mutex);
        return storage[nr].position;
    }
    
    double getErrorNodeShare(int i ) const{
//        boost::interprocess::scoped_lock<Mutex>(*m_mutex);
        return storage[i].error_new;
    }

    int getNumberNodes() const{
//        boost::interprocess::scoped_lock<Mutex>(*m_mutex);
        return storage.getNumberNodes();
    }
    
    int getMaximumIndex() const{
//        boost::interprocess::scoped_lock<Mutex>(*m_mutex);
        return storage.m_maximum_index;
    }
    
    //TODO: problem when growing ! Should be called *ONLY* by GNGAlgorithm
    Node & operator[](int i){ 
//        boost::interprocess::scoped_lock<Mutex>(*m_mutex);
        return storage[i]; 
    }
    
    double getAccumulatedErrorShare() const{
//       boost::interprocess::scoped_lock<Mutex>(*m_mutex);
       double error=0.0;
       
       REP(i,storage.m_maximum_index+1){
            error+=storage[i].error_new;
        }
        return error;
    }
    
    ExtGNGGraph(Mutex * mutex,  Storage * s, int dim) : dim(dim), m_mutex(mutex), storage(*s) {
    }

    void init(int start_number) {
//        boost::interprocess::scoped_lock<Mutex>(*m_mutex);
        storage.init(start_number);
    }

    double getDist(int a, int b) {
//        boost::interprocess::scoped_lock<Mutex>(*m_mutex);
        double distance = 0;
        for (int i = 0; i < this->dim; ++i) {
            
            distance += (storage[a].position[i] - storage[b].position[i])*(storage[a].position[i] - storage[b].position[i]);
        }
        return distance;
    }

    double getDist(const double * pos_a, const double * pos_b) const {
//        boost::interprocess::scoped_lock<Mutex>(*m_mutex);
        double distance = 0;
        for (int i = 0; i < this->dim; ++i) {
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
        memcpy(&storage[i].position[0], position, sizeof(double) *(this->dim)); //param

        storage[i].error = 0.0;
        storage[i].error_cycle = 0;
        storage[i].error_new = 0.0;
        return i;
    }
    bool deleteNode(int x){
//        boost::interprocess::scoped_lock<Mutex>(*m_mutex);
        return storage.deleteNode(x);
    }
    typename GNGNode::EdgeIterator removeEdge(int a, int b){
//        boost::interprocess::scoped_lock<Mutex>(*m_mutex);
        
        return storage.removeEdge(a,b);
    }

  
    void addUDEdge(int a, int b){
//        boost::interprocess::scoped_lock<Mutex>(*m_mutex);
        return storage.addUDEdge(a,b);
    }
    void addDEdge(int a, int b){
//        boost::interprocess::scoped_lock<Mutex>(*m_mutex);
        return storage.addDEdge(a,b);
    }


private:

};




#endif        /* SHGRAPH_H */