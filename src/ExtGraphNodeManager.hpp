/* 
 * File:   ExtGraphNodeManager.hpp
 * Author: staszek
 *
 * Created on September 3, 2012, 8:07 AM
 */


template<class Node, class Edge, class EdgeStorage >
bool  ExtGraphNodeManager<Node,Edge,EdgeStorage>::isEdge(int a, int b){
        FOREACH(edg,(g_pool[a].edges)){
            if(edg->nr == b) return true;
        }
        return false;
    }
    
    
template<class Node, class Edge, class EdgeStorage >
void  ExtGraphNodeManager<Node,Edge,EdgeStorage>::removeRevEdge(int a,EdgeIterator it){      
        int b=it->nr;       
        EdgeIterator rev = it->rev; 
        DBG(-1,"ExtGraphNodeManager::removing rev edge");
        g_pool[b].edges.erase(rev);       
        DBG(-1,"ExtGraphNodeManager::edge pretty much erased");  
        g_pool[b].edgesCount--;      
        
    }
     
template<class Node, class Edge, class EdgeStorage >
bool
ExtGraphNodeManager<Node,Edge,EdgeStorage>::removeEdge(int a, int b){
        DBG(-1, "ExtGraphNodeManager::removing edge, searching for it");

        FOREACH(edg, g_pool[a].edges){
            if(edg->nr==b) {
                EdgeIterator rev = edg->rev;
                DBG(-1, "ExtGraphNodeManager::removing edge");
                EdgeIterator ret= g_pool[a].edges.erase(edg);   
                DBG(-5, "ExtGraphNodeManager::removed edge from "+to_string(a));  
                g_pool[b].edges.erase(rev);
                DBG(-1, "ExtGraphNodeManager::edges pretty much erased also from "+to_string(b));

                g_pool[a].edgesCount--;
                g_pool[b].edgesCount--;    
                return true;
//                return ret;
            }
        }
        
        return false;
//        return g_pool[a].edges.end();
    }   
    
    
   template<class Node, class Edge, class EdgeStorage >
typename ExtGraphNodeManager<Node,Edge,EdgeStorage>::EdgeIterator  ExtGraphNodeManager<Node,Edge,EdgeStorage>::removeEdge(int a,EdgeIterator it){

        
        int b=it->nr;
        
        DBG(-1,"ExtGraphNodeManager::removing edge getting reverse iterator");
            
        
        EdgeIterator rev = it->rev; 
        

        EdgeIterator ret=g_pool[a].edges.erase(it);
        g_pool[b].edges.erase(rev);
        
        
        DBG(-1,"ExtGraphNodeManager::edges pretty much erased");
        
        g_pool[a].edgesCount--;       
        g_pool[b].edgesCount--;      
        
        return ret;
    }
template<class Node, class Edge, class EdgeStorage >
void  ExtGraphNodeManager<Node,Edge,EdgeStorage>:: addUDEdge(int a, int b){
        g_pool[a].edges.push_back(Edge(b));
        g_pool[b].edges.push_back(Edge(a));
        
        
        
        g_pool[a].edges.back().rev = --g_pool[b].edges.end();
        g_pool[b].edges.back().rev = --g_pool[a].edges.end();
                
        
        
        g_pool[a].edgesCount++;       
        g_pool[b].edgesCount++;
        
    }
 template<class Node, class Edge, class EdgeStorage >
void  ExtGraphNodeManager<Node,Edge,EdgeStorage>::addDEdge(int a, int b){
        //problem z krawedziami
        //ofset sie zmienia to nei jest bezwzgledny!
       // GNGEdge ed(b);
        

        g_pool[a].edges.push_back(Edge(b));
        g_pool[a].edgesCount++;

    }




template<class Node, class Edge, class EdgeStorage >
bool  ExtGraphNodeManager<Node,Edge,EdgeStorage>::growPool(){
      
        
    
    DBG(8,"ExtGraphNodeManager::growing");
    DBG(8,to_string(g_pool_nodes)+" from to 2*this size");
 
        g_pool_nodes*=2;   
        Node * tmp = g_pool;
        g_pool = new Node[g_pool_nodes];
        m_first_free=g_pool_nodes/2;
        

        
        DBG(8,"ExtGraphNodeManager::copied old nodes");

        for(int i=0;i<(g_pool_nodes/2);++i){
            g_pool[i] = tmp[i];
            if(g_pool[i].occupied){
                g_pool[i].nr = tmp[i].nr;
  
                FOREACH(edg,tmp[i].edges){
                    this->addUDEdge(i, edg->nr);
                }
                
            }
        }

        
        DBG(8,"ExtGraphNodeManager::copy fine");

        for(int i=m_first_free;i<g_pool_nodes;++i) {
            g_pool[i].occupied = false;
            g_pool[i].edgesCount = 0;
            g_pool[i].edges.clear();
            
            
            if (i != g_pool_nodes - 1) g_pool[i].nextFree = i + 1;
            else g_pool[i].nextFree = -1;           
        }
        

        DBG(8,"ExtGraphNodeManager::all pointer corrected (untested)");
        delete[] tmp;
        DBG(8,"ExtGraphNodeManager::m_free="+to_string(m_first_free));
        DBG(8,"ExtGraphNodeManager::completed");
        

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

                tmp = tmp + "\n";
            }
            ss.push_back(tmp);
        }

        sort(ss.begin(), ss.end());

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

    for (int i = 0; i < g_pool_nodes; ++i) {
        g_pool[i].occupied = false;
        g_pool[i].edges.clear();
        g_pool[i].edgesCount = 0;
        if (i != g_pool_nodes - 1) g_pool[i].nextFree = i + 1;
        else g_pool[i].nextFree = -1;
    }

}

template<class Node, class Edge, class EdgeStorage >
ExtGraphNodeManager<Node,Edge,EdgeStorage>::ExtGraphNodeManager(int start_number)  {
    this->init(start_number);
//    delete[] g_pool;
//    this->init(start_number);
}
template<class Node, class Edge, class EdgeStorage >
int ExtGraphNodeManager<Node,Edge,EdgeStorage>::newNode() {

    int createdNode = m_first_free; //taki sam jak w g_node_pool
    
    //not accurate but that is not essential of course to have always correct value as it is a matter of few nodes at most
    m_maximum_index = createdNode>m_maximum_index ? createdNode : m_maximum_index;
    
    g_pool[createdNode].occupied = true;
    g_pool[createdNode].nr = createdNode;
    m_first_free = g_pool[createdNode].nextFree;
    g_pool[createdNode].nextFree = -1;
    g_pool[createdNode].edgesCount = 0;
    
   

  //  g_pool[createdNode].edges = new EdgeStorage();

    
     ++m_nodes; //zwiekszam licznik wierzcholkow //na koncu zeby sie nie wywalil przypadkowo
    
    
    return createdNode;
}
template<class Node, class Edge, class EdgeStorage >
bool ExtGraphNodeManager<Node,Edge,EdgeStorage>::deleteNode(int x) {

   if(g_pool[x].occupied) {
        --m_nodes;
        g_pool[x].edgesCount = 0;
        g_pool[x].occupied = false;
        g_pool[x].nextFree = m_first_free;

        //deletion is unfortunately linear - see to it?

        //delete &(*g_pool[x ].edges); //wazne zeby obiekt zostal bo jest alokowany na SH
        g_pool[x].edges.clear();
        m_first_free = x;
    }
    return true;
}
