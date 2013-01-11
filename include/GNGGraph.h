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
#include "GNGGlobals.h"

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include <boost/interprocess/offset_ptr.hpp>


typedef ExtGraphNodeManager<GNGNode, GNGEdge, GNGList> GNGGraphBase;


//w GNGClient mozna wywolywac tylko czesc funkcji, w szczegolnosci te opatrzone koncowka Share (moze nakladka jakas potem?)
class GNGGraph : public GNGGraphBase {
    typedef ExtGraphNodeManager<GNGNode, GNGEdge, GNGList > super;
    typedef boost::interprocess::interprocess_mutex Mutex;
    Mutex * m_mutex;
    boost::interprocess::offset_ptr<GNGNode> g_pool_share;    
public:

    double getErrorNodeShare(int i ) const{
        return g_pool_share[i].error_new;
    }
    
    GNGNode* getPoolShare(){
        return g_pool_share.get();
    }
    
    double getAccumulatedErrorShare(){
        GNGNode * nodes = getPoolShare();
        double error=0.0;
        REP(i,m_maximum_index+1){
            error+=nodes[i].error_new;
        }
        return error;
    }
    
    GNGGraph(Mutex * mutex, int start_number) : m_mutex(mutex), GNGGraphBase(start_number) {
        g_pool_share = super::getPool();
    }

    GNGGraph(Mutex * mutex) :m_mutex(mutex), GNGGraphBase() {
        g_pool_share = super::getPool();
    }

    GNGGraph(Mutex * mutex,GNGNode * _g_pool, int _m_nodes, int _g_pool_nodes, int _m_first_free) :
    m_mutex(mutex),GNGGraphBase(_g_pool, _m_nodes, _g_pool_nodes, _m_first_free) {
        g_pool_share = super::getPool();
    }

    void init(int start_number) {
        super::init(start_number);
    }

    double getDist(int a, int b) {
        double distance = 0;
        for (int i = 0; i < GNG_DIM; ++i) {
            distance += (super::operator[](a).position[i] - super::operator[](b).position[i])*(super::operator[](a).position[i] - super::operator[](b).position[i]);
        }
        return distance;
    }

    double getDist(double * pos_a, double * pos_b) {
        double distance = 0;
        for (int i = 0; i < GNG_DIM; ++i) {
            distance += (pos_a[i] - pos_b[i])*(pos_a[i] - pos_b[i]);
        }
        return distance;
    }

    double getDistEdge(int a, super::EdgeIterator it) {
        return std::sqrt(getDist(super::operator[](a).position, super::operator[](it->nr).position));
    }

    int newNode(double const *position) {
         if (super::poolIsFull()) {
                 boost::interprocess::scoped_lock<Mutex>(*m_mutex);
                 super::growPool(); 
                 g_pool_share = super::getPool();
         }
         
        int i = super::newNode();

        memcpy(&(super::g_pool + i)->position[0], position, sizeof (double) *(GNG_DIM+1)); //param
        g_pool[i].error = 0.0;
        g_pool[i].error_cycle = 0;
        g_pool[i].error_new = 0.0;

        return i;
    }
    bool deleteNode(int x){
        boost::interprocess::scoped_lock<Mutex>(*m_mutex);
        return super::deleteNode(x);
    }    
    super::EdgeIterator removeEdge(int a, int b){
        boost::interprocess::scoped_lock<Mutex>(*m_mutex);
        return super::removeEdge(a,b);
    }
    super::EdgeIterator removeEdge(int a,super::EdgeIterator it){
        boost::interprocess::scoped_lock<Mutex>(*m_mutex);
        return super::removeEdge(a,it);
    }
     void removeRevEdge(int a,super::EdgeIterator it){
         boost::interprocess::scoped_lock<Mutex>(*m_mutex);
         return super::removeRevEdge(a,it);
     }
    void addUDEdge(int a, int b){
        boost::interprocess::scoped_lock<Mutex>(*m_mutex);
        return super::addUDEdge(a,b);
    }
    void addDEdge(int a, int b){
        boost::interprocess::scoped_lock<Mutex>(*m_mutex);
        return super::addDEdge(a,b);
    }


    virtual ~GNGGraph() {

    }


private:

};




#endif	/* SHGRAPH_H */

