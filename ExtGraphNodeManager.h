/* 
 * File:   ExtGraphNodeManager.h
 * Author: staszek
 *
 * Created on 10 sierpień 2012, 15:48
 */

#ifndef EXTGRAPHNODEMANAGER_H
#define	EXTGRAPHNODEMANAGER_H

#define COMPILE_WITH_GROW_MUTEX

#include <cstddef>

#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <list>

#include "ExtMemoryManager.h"

#include "SHGraphDefs.h"

#include "Utils.h"
#include "DebugCollector.h"

#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>

#include "GNGDefines.h"

extern DebugCollector dbg;

#ifdef COMPILE_WITH_GROW_MUTEX
 typedef boost::interprocess::interprocess_mutex MyMutex;
extern MyMutex * grow_mutex;
#endif

/*
 * Node musi miec pole nr (int),bool,int i edges (klasa z operatorem[], generalnie vector)
 * Node musi miec domyslny konstruktor
 * Node musi dac sie wypisac
 * 
 * 
 * Idea: niezalezne od implementacji dzielonej pamieci - moze byc w koncu inna (np do pliku)
 */

/*
 * O alokacje w SHM dba oczywiscie vector z boost'a
 */

// Node -> nr ==== numer w tablicy wierzcholkow (dokladnie jak w pamieci!)
// Do funkcji podajemy /Node ->nr, co one z nimi zrobia - nie wazne
template<class Node, class Edge, class EdgeStorage>
class ExtGraphNodeManager {
    
    
    
protected:  
    typedef typename EdgeStorage::iterator EdgeIterator;
    
   // ExtMemoryManager *m_mm;
    Node *  g_pool; 
    
    int m_first_free;
    
    int m_nodes;
    int m_maximum_index;
    
    int g_pool_nodes;
    
   
    
    //for error checking
    bool growPool();
    bool poolIsFull(){
        return m_first_free==-1;
    }
    
    ExtGraphNodeManager(const ExtGraphNodeManager& orig){}
public:
    
    std::string reportPool(bool sorted=false);
    std::string reportMemory(){
        
    }
    
    ExtGraphNodeManager(int start_number);
    ExtGraphNodeManager(){}
    ExtGraphNodeManager(Node * _g_pool, int _m_nodes, int _g_pool_nodes, int _m_first_free):
        g_pool(_g_pool),m_nodes(_m_nodes),g_pool_nodes(_g_pool_nodes),m_first_free(_m_first_free),m_maximum_index(0)
    {}
    
    int newNode();
    
    void checkNodeEgdeStorage(){
        
    }
    
    int getNumberNodes() const{return m_nodes;}
    int getPoolNodes() const{return g_pool_nodes;}
    int getFirstFree() const{return m_first_free;}
    
    void init(int start_number);
    
    //return part of pool occupied -> past it is nothing
    int getMaximumIndex() const{
       
        return m_maximum_index;
    }
    
    //linear complexity but gng graph is sparse because it is delanuay triangulation inducted graph which is O(N), where N is the dimension of GNG network
    bool isEdge(int a, int b){
        
        FOREACH(edg,*(g_pool[a].edges)){
            if(edg->nr == b) return true;
        }
        
        return false;
    }
    
    
    //constant complexity
     void removeRevEdge(int a,EdgeIterator it){
        #ifdef COMPILE_WITH_GROW_MUTEX
        ScopedLock sc(*grow_mutex);
        #endif
        
        int b=it->nr;
        
        EdgeIterator rev = it->rev; 
        
        //dbg.push_back(-1,"ExtGraphNodeManager::removing rev edge");

        g_pool[b].edges->erase(rev);
        
        //dbg.push_back(-1,"ExtGraphNodeManager::edge pretty much erased");
  

        g_pool[b].edgesCount--;      
        
    }
     
     //return false if edge doesnt exist//
    EdgeIterator removeEdge(int a, int b){
        #ifdef COMPILE_WITH_GROW_MUTEX
        ScopedLock sc(*grow_mutex);
        #endif
        
        FOREACH(edg,*(g_pool[a].edges)){
            if(edg->nr==b) {
                EdgeIterator rev = edg->rev;
                
                //dbg.push_back(-1, "ExtGraphNodeManager::removing edge");

                EdgeIterator ret= g_pool[a].edges->erase(edg);
                g_pool[b].edges->erase(rev);

                //dbg.push_back(-1, "ExtGraphNodeManager::edges pretty much erased");

                g_pool[a].edgesCount--;
                g_pool[b].edgesCount--;    
                
                return ret;
            }
        }
        
        return g_pool[a].edges->end();
    }   
    
    
    
    EdgeIterator removeEdge(int a,EdgeIterator it){
        #ifdef COMPILE_WITH_GROW_MUTEX
        ScopedLock sc(*grow_mutex);
        #endif
        
        int b=it->nr;
        
        EdgeIterator rev = it->rev; 
        
        //dbg.push_back(-1,"ExtGraphNodeManager::removing edge");
        
        EdgeIterator ret=g_pool[a].edges->erase(it);
        g_pool[b].edges->erase(rev);
        
        //dbg.push_back(-1,"ExtGraphNodeManager::edges pretty much erased");
        
        g_pool[a].edgesCount--;       
        g_pool[b].edgesCount--;      
        
        return ret;
    }
    
    void addUDEdge(int a, int b){
        //no self loops !! - important for correctness//
        ScopedLock sc(*grow_mutex);
        
        
        //what is important is that you cant remove edge when buffering so grow_mutex lock is necessary
        

        ((g_pool + a)->edges)->push_back(b);
        ((g_pool+b)->edges)->push_back(a);
        
        
        
        (g_pool+a)->edges->back().rev = --((g_pool+b)->edges->end());
        (g_pool+b)->edges->back().rev = --((g_pool+a)->edges->end());
                
        
        
        (g_pool + a)->edgesCount++;       
        (g_pool + b)->edgesCount++;
        
    }
    
    void addDEdge(int a, int b){
        //problem z krawedziami
        //ofset sie zmienia to nei jest bezwzgledny!
       // GNGEdge ed(b);
        
        grow_mutex->lock();
        ((g_pool+a)->edges)->push_back(b);
        (g_pool+a)->edgesCount++;
        grow_mutex->unlock();
    }
    
 
    bool deleteNode(int x);    
    
    Node * operator[](int i){ return g_pool +i; }
    
    
    //TODO: dodac const
    Node * getPool() const{ return g_pool; }
    int getPoolSize() const{ return g_pool_nodes; }
    
    virtual ~ExtGraphNodeManager(){
        
        
    }
private:

};












template<class Node, class Edge, class EdgeStorage >
bool  ExtGraphNodeManager<Node,Edge,EdgeStorage>::growPool(){
        #ifdef COMPILE_WITH_GROW_MUTEX
        grow_mutex->lock();
        #endif
        
        dbg.push_back(2,"ExtGraphNodeManager::growing");
        g_pool_nodes*=2;   
        
        Node * tmp=g_pool;
       
        g_pool = new Node[g_pool_nodes];

    
        memcpy(g_pool,tmp, sizeof(Node)*(g_pool_nodes)/2);
       
        m_first_free=g_pool_nodes/2;
        
        
        //naprawa pointerow
        for(int i=0;i<(g_pool_nodes/2);++i){
            if(g_pool[i].occupied){
                g_pool[i].edges = tmp[i].edges; //operator == handles changing offset
            }
        }
        
        for(int i=m_first_free;i<g_pool_nodes;++i) {
            g_pool[i].occupied = false;
            g_pool[i].edgesCount = 0;
            g_pool[i].edges=0;
            
            
            if (i != g_pool_nodes - 1) g_pool[i].nextFree = i + 1;
            else g_pool[i].nextFree = -1;           
        }
        
        delete tmp;
        
        //dbg.push_back(3,"ExtGraphNodeManager::m_free="+to_string(m_first_free));
        
        #ifdef COMPILE_WITH_GROW_MUTEX        
        grow_mutex->unlock();
        #endif

        
        //dbg.push_back(2,"ExtGraphNodeManager::completed");
        
        return true;
    }




template<class Node, class Edge, class EdgeStorage >
std::string ExtGraphNodeManager<Node,Edge,EdgeStorage>::reportPool(bool sorted) {
    using namespace std;

    if (!sorted) {
        stringstream ss;
        
        ss<<"g_pool_nodes="<<g_pool_nodes<<" m_nodes="<<m_nodes<<" m_first_free="<<m_first_free<<endl;

        Node * it = g_pool;
        Node * end = it + g_pool_nodes;
        
        int k = getMaximumIndex();
        int j=0;
        
        for (; it != end; ++it) {
            ++j;
            if(j>getMaximumIndex()+1) break; //not current edge?
            
            if (it->occupied) {
                ss << (*it) << ":";
             
                
            
                
                //FOREACH(it2, *(it->edges)) ss << (it2->nr) << ",";
                
                ss << endl;
                
            }
            else ss<<"X"<<endl;
        }
        return ss.str();
    } else {
        vector<string> ss;


        Node * it = g_pool;
        Node * end = it + g_pool_nodes;
        for (; it != end; ++it) {
            string tmp = "";
            if (it->occupied) {
                tmp = tmp + to_string(*it) + ":";

       
                FOREACH(it2, *(it->edges)) tmp = tmp + to_string(it2->nr) + ",";
                
                //FOREACH(it2, *it->edges) tmp = tmp + to_string(it2->nr) + ",";

                tmp = tmp + "\n";
            }
            ss.push_back(tmp);
        }

        sort(ss.begin(), ss.end()); //domyslnie sortowanie leksykograficzne

        string result = "";

        FOREACH(it, ss) {
            result = result + (*it);
        }

        return result;
    }

}


template<class Node, class Edge, class EdgeStorage >
void ExtGraphNodeManager<Node,Edge,EdgeStorage>::init(int start_number)  {
    m_first_free=0;
    m_nodes=0;
    
    g_pool = new Node[start_number];
    g_pool_nodes = start_number;

    //zaznaczam wolne pola
    for (int i = 0; i < g_pool_nodes; ++i) {
        g_pool[i].occupied = false;
        g_pool[i].edges =0 ;
        g_pool[i].edgesCount = 0;
        if (i != g_pool_nodes - 1) g_pool[i].nextFree = i + 1;
        else g_pool[i].nextFree = -1;
    }

}

template<class Node, class Edge, class EdgeStorage >
ExtGraphNodeManager<Node,Edge,EdgeStorage>::ExtGraphNodeManager(int start_number)  {
    this->init(start_number);
}
template<class Node, class Edge, class EdgeStorage >
int ExtGraphNodeManager<Node,Edge,EdgeStorage>::newNode() {
    if (poolIsFull()) {
        //dbg.push_back(3,"ExtGraphNodeManager::zabraklo miejsca powiekszam z "+to_string(g_pool_nodes)); 

        growPool(); 
    }

   

    int createdNode = m_first_free; //taki sam jak w g_node_pool
    
    //not accurate but that is not essential of course to have always correct value as it is a matter of few nodes at most
    m_maximum_index = createdNode>m_maximum_index ? createdNode : m_maximum_index;
    
    g_pool[createdNode].occupied = true;
    g_pool[createdNode].nr = createdNode;
    m_first_free = g_pool[createdNode].nextFree;
    g_pool[createdNode].nextFree = -1;
    g_pool[createdNode].edgesCount = 0;
    
   

    g_pool[createdNode].edges = new EdgeStorage();

    
     ++m_nodes; //zwiekszam licznik wierzcholkow //na koncu zeby sie nie wywalil przypadkowo
    
    
    return createdNode;
}
template<class Node, class Edge, class EdgeStorage >
bool ExtGraphNodeManager<Node,Edge,EdgeStorage>::deleteNode(int x) {
   
        #ifdef COMPILE_WITH_GROW_MUTEX
        ScopedLock sc(*grow_mutex);
        #endif
       
   if(g_pool[x].occupied) {
        --m_nodes;
        g_pool[x].edgesCount = 0;
        g_pool[x ].occupied = false;
        g_pool[x ].nextFree = m_first_free;

        //deletion is unfortunately linear - see to it?

        delete &(*g_pool[x ].edges); //wazne zeby obiekt zostal bo jest alokowany na SH
        g_pool[x].edges = 0; //nullpointer
        m_first_free = x;
    }
    return true;
}
#endif	/* EXTGRAPHNODEMANAGER_H */

