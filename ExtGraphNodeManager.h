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

#include "ExtMemoryManager.h"

#include "SHGraphDefs.h"

#include "Utils.h"
#include "DebugCollector.h"

#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>

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
template<class Node, class Edge, class EdgeStorage>
class ExtGraphNodeManager {
protected:  
   // ExtMemoryManager *m_mm;
    Node *  g_pool; 
    
    int m_first_free;
    
    int m_nodes;
    
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
        g_pool(_g_pool),m_nodes(_m_nodes),g_pool_nodes(_g_pool_nodes),m_first_free(_m_first_free)
    {}
    
    int newNode();
    
    int getNumberNodes() const{return m_nodes;}
    int getPoolNodes() const{return g_pool_nodes;}
    int getFirstFree() const{return m_first_free;}
    
    void init(int start_number);
    
    void addDEdge(int a, int b){
        //problem z krawedziami
        //ofset sie zmienia to nei jest bezwzgledny!
       // GNGEdge ed(b);
        ((g_pool+a)->edges)->push_back(b);
       
        (g_pool+a)->edgesCount++;
    }
    
    Edge * getFirstEdge(int a){
        if((g_pool+a)->edges==0) return 0;
        return reinterpret_cast<Edge*>(&(*(g_pool+a)->edges)[0]);
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
        
        dbg.push_back(3,"ExtGraphNodeManager::m_free="+to_string(m_first_free));
        
        #ifdef COMPILE_WITH_GROW_MUTEX        
        grow_mutex->unlock();
        #endif

        
        dbg.push_back(2,"ExtGraphNodeManager::completed");
        
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
        
        
        for (; it != end; ++it) {
            if (it->occupied) {
                ss << (*it) << ":";
             
                
               
                if(it->edgesCount!=0){
                    Edge * it2 =getFirstEdge(it->nr);
                    Edge * end = it2+ it->edgesCount;
               
                  
                   for(;it2!=end;++it2) ss << (it2->nr) << ",";
                }
                 
                //FOREACH(it2, *(it->edges)) ss << (it2->nr) << ",";
                
                ss << endl;
                
            }
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

                Edge * it2 = getFirstEdge(it->nr);
                Edge * end = it2+ it->edgesCount;
                for(;it2!=end;++it2) tmp = tmp + to_string(it2->nr) + ",";
                
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
        dbg.push_back(3,"ExtGraphNodeManager::zabraklo miejsca powiekszam z "+to_string(g_pool_nodes)); 

        growPool(); 
    }

   

    int createdNode = m_first_free;

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
    --m_nodes;
    g_pool[x-1].edgesCount=0;
    g_pool[x - 1].occupied = false;
    g_pool[x - 1].nextFree = m_first_free;
    delete &(*g_pool[x - 1].edges); //wazne zeby obiekt zostal bo jest alokowany na SH
    g_pool[x-1].edges=0; //nullpointer
    m_first_free = x - 1;
    return true;
}
#endif	/* EXTGRAPHNODEMANAGER_H */

