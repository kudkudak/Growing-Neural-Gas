/* 
 * File:   ExtGraphNodeManager.hpp
 * Author: staszek
 *
 * Created on September 3, 2012, 8:07 AM
 */
#include <iostream>
#include <typeinfo>
#include <algorithm>
using namespace std;

template<class Node, class Edge >
bool ExtGraphNodeManager<Node,Edge>::isEdge(int a, int b){ 
        FOREACH(edg, g_pool[a]){
            if((*edg)->nr == b) return true;
        }
        return false;
}


template<class Node, class Edge >
typename Node::EdgeIterator ExtGraphNodeManager<Node,Edge >::removeEdge(int a, int b){
        FOREACH(edg, g_pool[a]){
            if((*edg)->nr==b) {
//                
//                cout<<"erasing\n";
//                cout<<this->reportPool()<<endl;
//                cout<<((*edg)->nr)<<"->"<<((*edg)->rev->nr)<<endl;
//                cout<<((long int)(*edg)->rev)<<endl;
//                cout<<"TYPE\n";
//                std::cout << typeid((**edg).rev).name() << '\n';
                RAMGNGEdge * ptr_rev = (RAMGNGEdge*)((**edg).rev);
                RAMGNGEdge * ptr = (RAMGNGEdge*)(&(**edg));
               
                
                
//                cout<<this->reportPool()<<endl;
                g_pool[b].erase(find(g_pool[b].begin(), g_pool[b].end(), (*edg)->rev));
//                cout<<this->reportPool()<<endl;
        
                edg = g_pool[a].erase(edg);
                
                
                delete ptr;
                delete ptr_rev;
         
                

                g_pool[a].edgesCount--;
                g_pool[b].edgesCount--;
  
                return edg;
            }
        }
        DBG(10, "ExtGraphNodeManager()::removeEdge Not found edge!");
        return g_pool[a].end();
} 

 
template<class Node, class Edge >
void  ExtGraphNodeManager<Node,Edge>:: addUDEdge(int a, int b){
    if(a==b) throw "Added loop to the graph";
        //not important what it is, TODO: change to Edge
        g_pool[a].push_back(new Edge(b));
        g_pool[b].push_back(new Edge(a));
        
        
        
        g_pool[a].back()->rev = g_pool[b].back();
        g_pool[b].back()->rev = g_pool[a].back();
                
        
        
        g_pool[a].edgesCount++;       
        g_pool[b].edgesCount++;

//#ifdef DEBUG
//        DBG(1,"ExtGraphNodeManager::addUDEdge "+to_string(a)+" edges");
//        FOREACH(edg, g_pool[a]){
//            DBG(1, to_string((*edg)->nr));
//            DBG(1, to_string((((*edg)->rev))->nr));
//        }
//        DBG(1,"ExtGraphNodeManager::addUDEdge "+to_string(b)+" edges");
//        FOREACH(edg, g_pool[b]){
//            DBG(1, to_string((*edg)->nr));
//             DBG(1, to_string((((*edg)->rev))->nr));
//        }    
//#endif        

        
    }
 template<class Node, class Edge >
void  ExtGraphNodeManager<Node,Edge>::addDEdge(int a, int b){
      throw "Not implemented";
     
        g_pool[a].push_back(new Edge(b));
        g_pool[a].edgesCount++;

    }




template<class Node, class Edge >
bool  ExtGraphNodeManager<Node,Edge>::growPool(){
    DBG(8,"ExtGraphNodeManager::growing");
    DBG(8,to_string(g_pool_nodes)+" from to 2*this size");
 
        g_pool_nodes*=2;   
        Node * tmp = g_pool;
        g_pool = new Node[g_pool_nodes];
        m_first_free=g_pool_nodes/2;
        
        DBG(8,"ExtGraphNodeManager::copied old nodes");

        for(int i=0;i<(g_pool_nodes/2);++i)
            g_pool[i] = tmp[i]; //Edges stay the same
//            if(g_pool[i].occupied){
//                g_pool[i].nr = tmp[i].nr;
//  
//                FOREACH(edg,tmp[i]){
//                    this->addUDEdge(i, (*edg)->nr);
//                }
//                
//            }
        

        DBG(8,"ExtGraphNodeManager::copy fine");

        
        for(int i=m_first_free;i<g_pool_nodes;++i) {
            g_pool[i].init();
            g_pool[i].occupied = false;
            g_pool[i].edgesCount = 0;
            g_pool[i].clear();
            
            if (i != g_pool_nodes - 1) g_pool[i].nextFree = i + 1;
            else g_pool[i].nextFree = -1;           
        }

        DBG(8,"ExtGraphNodeManager::all pointer corrected (untested)");
        
        delete[] tmp;

        
        DBG(8,"ExtGraphNodeManager::m_free="+to_string(m_first_free));
        DBG(8,"ExtGraphNodeManager::completed");
        
        return true;
    }




template<class Node, class Edge >
std::string ExtGraphNodeManager<Node,Edge>::reportPool(bool sorted) {
    using namespace std;

   
        vector<string> ss;


        Node * it = g_pool;
        Node * end = it + g_pool_nodes;
        for (; it != end; ++it) {
            string tmp = "";
            if (it->occupied) {
                tmp = tmp + to_string(*it) + ":";

                FOREACH(it2, *(it)) tmp = tmp + to_string((*it2)->nr) + "["+to_string((((*it2)->rev))->nr)+"],";

                tmp = tmp + "\n";
            }
            ss.push_back(tmp);
        }


        string result = "";

        FOREACH(it, ss) {
            result = result + (*it);
        }

        return result;


}


template<class Node, class Edge >
void ExtGraphNodeManager<Node,Edge>::init(int start_number)  {
    m_first_free=0;
    m_nodes=0;
    
    g_pool = new Node[start_number];
    g_pool_nodes = start_number;

    for (int i = 0; i < g_pool_nodes; ++i) {
        g_pool[i].init();
        g_pool[i].occupied = false;
        g_pool[i].clear();
        g_pool[i].edgesCount = 0;
        if (i != g_pool_nodes - 1) g_pool[i].nextFree = i + 1;
        else g_pool[i].nextFree = -1;
    }

}

template<class Node, class Edge >
int ExtGraphNodeManager<Node,Edge>::newNode() {
    int createdNode = m_first_free; //taki sam jak w g_node_pool
    
    //not accurate but that is not essential of course to have always correct value as it is a matter of few nodes at most
    m_maximum_index = createdNode>m_maximum_index ? createdNode : m_maximum_index;
    
    g_pool[createdNode].occupied = true;
    g_pool[createdNode].nr = createdNode;
    m_first_free = g_pool[createdNode].nextFree;
    g_pool[createdNode].nextFree = -1;
    g_pool[createdNode].edgesCount = 0;
    DBG(8, "New node");
    DBG(8, "Position pointer="+to_string(g_pool[createdNode].position[0]));
   

  //  g_pool[createdNode].edges = new EdgeStorage();

    
     ++m_nodes; //zwiekszam licznik wierzcholkow //na koncu zeby sie nie wywalil przypadkowo
    

    return createdNode;
}
template<class Node, class Edge>
bool ExtGraphNodeManager<Node, Edge>::deleteNode(int x) {

   if(g_pool[x].occupied) {
        --m_nodes;
        g_pool[x].edgesCount = 0;
        g_pool[x].occupied = false;
        g_pool[x].nextFree = m_first_free;
        g_pool[x].clear();
        m_first_free = x;
    }
    return true;
}
