/* 
 * File:   ExtGraphNodeManager.h
 * Author: staszek
 *
 * Created on 10 sierpień 2012, 15:48
 */

#ifndef EXTGRAPHNODEMANAGER_H
#define	EXTGRAPHNODEMANAGER_H

#define COMPILE_WITH_GROW_MUTEX

#include "ExtMemoryManager.h"
#include "SHGraphDefs.h"
#include "Utils.h"
#include "DebugCollector.h"
#include "GNGDefines.h"

extern DebugCollector dbg;

/*
 * Node musi miec pole nr (int),bool,int i edges (klasa z operatorem[], generalnie vector)
 * Node musi miec domyslny konstruktor
 * Node musi dac sie wypisac
 * 
 * 
 * Idea: niezalezne od implementacji dzielonej pamieci - moze byc w koncu inna (np do pliku)
 */

template<class Node, class Edge, class EdgeStorage>
class ExtGraphNodeManager {
    
protected:  
    typedef typename EdgeStorage::iterator EdgeIterator;

    Node *  g_pool; 
    
    int m_first_free,m_nodes,m_maximum_index,g_pool_nodes;

    /**
     * @brief this will grow (populate) the pool, but ! is called by encapsulating class !
     * @return 
     */
    bool growPool();
    bool poolIsFull(){
        return m_first_free==-1;
    }
    
    ExtGraphNodeManager(const ExtGraphNodeManager& orig){}
public:
    std::string reportPool(bool sorted=false);
 
    
    ExtGraphNodeManager(int start_number);
    ExtGraphNodeManager(){}
    ExtGraphNodeManager(Node * _g_pool, int _m_nodes, int _g_pool_nodes, int _m_first_free):
        g_pool(_g_pool),m_nodes(_m_nodes),g_pool_nodes(_g_pool_nodes),m_first_free(_m_first_free),m_maximum_index(0)
    {}
    
    int newNode();

    
    int getNumberNodes() const{return m_nodes;}
    int getPoolNodes() const{return g_pool_nodes;}
    int getFirstFree() const{return m_first_free;}
    
    void init(int start_number);
    
    int getMaximumIndex() const{       
        return m_maximum_index;
    }
    
    
    bool isEdge(int a, int b);
    

    void removeRevEdge(int a,EdgeIterator it);

    EdgeIterator removeEdge(int a, int b);
    EdgeIterator removeEdge(int a,EdgeIterator it);
    
    void addUDEdge(int a, int b);
    void addDEdge(int a, int b);
 
    bool deleteNode(int x);    
    
    Node & operator[](int i){ return g_pool[i]; }
 
    Node * getPool() const{ return g_pool; }
    int getPoolSize() const{ return g_pool_nodes; }
    
    virtual ~ExtGraphNodeManager(){
        delete[] g_pool;
    }
private:

};


#include "ExtGraphNodeManager.hpp"

#endif	/* EXTGRAPHNODEMANAGER_H */

