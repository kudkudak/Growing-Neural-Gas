/* 
 * File:   ExtGraphNodeManager.h
 * Author: staszek
 *
 * Created on 10 sierpień 2012, 15:48
 */
#ifndef EXTGRAPHNODEMANAGER_H
#define	EXTGRAPHNODEMANAGER_H
#define COMPILE_WITH_GROW_MUTEX

#include "MemoryAllocator.h"
#include "SHGraphDefs.h"
#include "Utils.h"
#include "DebugCollector.h"
#include "GNGDefines.h"

extern DebugCollector dbg;

/*!
 * @brief This class implements graph stored in pool memory.
 * What's unique, is that this pool memory can be implemented
 * afterwards in an arbitrary way. 
 * Node class has to implement new operators if it wants to change
 * memory management. 
 * 
 * @note EdgeStorage has to implement iterator (EdgeStorage::iterator)
 * 
 * 
 */
template<class Node, class Edge, class EdgeStorage>
class ExtGraphNodeManager {
    
protected:  
    typedef typename EdgeStorage::iterator EdgeIterator;

    Node *  g_pool; 
    

    /**
     * @brief This will grow (populate) the pool, but is called by the encapsulating function. 
     * 
     * @return Has the operation succeeded
     */

    
    ExtGraphNodeManager(const ExtGraphNodeManager& orig){}
public:
    bool growPool();
    bool poolIsFull(){
        return m_first_free==-1;
    }   
    /** For debugging purposes, prints pool
     */
    std::string reportPool(bool sorted=false);
    int m_first_free,m_nodes,m_maximum_index,g_pool_nodes;
    
    ExtGraphNodeManager(int start_number);
    ExtGraphNodeManager(){}
    ExtGraphNodeManager(Node * _g_pool, int _m_nodes, 
            int _g_pool_nodes, int _m_first_free):
        g_pool(_g_pool),m_nodes(_m_nodes),g_pool_nodes(_g_pool_nodes),
        m_first_free(_m_first_free),m_maximum_index(0)
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

    bool removeEdge(int a, int b);
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

