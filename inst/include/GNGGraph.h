/*
 * File: SHGraph.h
 * Author: staszek
 *
 * Created on 11 sierpień 2012, 09:07
 */

#ifndef GNGGraph_H
#define GNGGraph_H

#include <string>
#include <sstream>

#include "SHMemoryManager.h"
#include "GNGNode.h"
#include "GNGGlobals.h"

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include <boost/interprocess/offset_ptr.hpp>
#include <iostream>
using namespace std;

#include <iterator>

/** Graph interface for GNGAlgorithm. 
 * 
 * 
 */
class GNGGraph {
public:

    virtual ~GNGGraph() {
    }

    /** Lock from unsafe operations 
     * @note It ensures that operations won't fail (in worst case block)
     * Mostly used for blocking regrowing 
     */
    virtual void lock() {
        //Do nothing by default
    }

    /** Unlock for unsafe operations */
    virtual void unlock() {
        //Do nothing by default
    }

    /** This is specific for GNG Graph - e
     * each node is assigned index. It fetches maximum node index
     */
    virtual unsigned int getMaximumIndex() const = 0;
    
    
    /*
     * @return True if exists node in the graph
     */
    virtual bool existsNode(unsigned int) const = 0;
    
    virtual GNGNode & operator[](int i) = 0;

    virtual unsigned int getNumberNodes() const = 0;

    //TODO: move it to GNGNode
    virtual double getDist(int a, int b) = 0;

    //TODO: move it to GNGNode
    virtual double getDist(const double * pos_a, const double * pos_b) const = 0;

    /* Initialize node with position attribute */
    virtual int newNode(double const *position) = 0;

    virtual bool deleteNode(int x) = 0;

    virtual bool isEdge(int a, int b) const = 0;

    virtual typename GNGNode::EdgeIterator removeEdge(int a, int b) = 0;

    virtual void addUDEdge(int a, int b) = 0;

    virtual void addDEdge(int a, int b) = 0;

    virtual std::string reportPool() {
        return "";
    }
};

/* @note: Not thread safe
 * Can be used by external thread by has to be locked. All operations moving
 * whole memory sectory will unlock. Elegant solution: locking interface
 * 
 * It allows for easy erasing of nodes.
 * 
 * Node: implements GNGNode interface
 * Edge: implements GNGEdge interface
 * Mutex: implements lock and unlock interface
 *
 * TODO: change from pointer to keeping whole objects: Deleting
 * is inefficient in memory sense (fragmenting etc.)
 */
template<class Node, class Edge, class Mutex = boost::mutex>
class RAMGNGGraph : public GNGGraph {
    /** Mutex provided externally for synchronization*/
    Mutex * mutex;
    std::vector<Node*> g;
    /** Indicates next free vertex */
    std::vector<unsigned int> next_free;

    unsigned int maximum_index;
    unsigned int nodes;
    unsigned int gng_dim;
    unsigned int firstFree;
public:
    typedef typename Node::EdgeIterator EdgeIterator;

    RAMGNGGraph(Mutex* mutex, unsigned int dim, int starting_nodes) :
    maximum_index(starting_nodes-1), mutex(mutex), gng_dim(dim), 
    firstFree(-1), nodes(starting_nodes) {
        //Initialize graph data structures
        g.resize(starting_nodes);
        for (int i = 0; i < starting_nodes; ++i) g[i] = 0;
        next_free.resize(starting_nodes);
        for (int i = 0; i < starting_nodes - 1; ++i) next_free[i] = i + 1;
        next_free[starting_nodes - 1] = -1;
    }

    virtual void lock() {
        mutex->lock();
    }

    virtual void unlock() {
        mutex->unlock();
    }
    /** This is specific for GNG Graph - e
     * each node is assigned index. It fetches maximum node index
     */
    virtual unsigned int getMaximumIndex() const
    {
        return this->maximum_index;
    }
    
    
    /*
     * @return True if exists node in the graph
     */
    virtual bool existsNode(unsigned  i) const{
        return i < nodes && g[i];
    }


    bool isEdge(int a, int b) const {

        FOREACH(edg, *g[a]) {
            if ((*edg)->nr == b) return true;
        }
        return false;
    }

    const double * getPosition(int nr) const {
        return g[nr].position;
    }

    unsigned int getNumberNodes() const {
        return g.size();
    }



    //TODO: problem when growing ! Should be called *ONLY* by GNGAlgorithm
    /** @note Should be called *ONLY* by GNGAlgorithm */
    Node & operator[](int i) {
        return *g[i];
    }

    double getDist(int a, int b) {
        //        boost::interprocess::scoped_lock<Mutex>(*m_mutex);
        double distance = 0;
        for (int i = 0; i < this->gng_dim; ++i) {

            distance += (g[a]->position[i] - g[b]->position[i])*(g[a]->position[i] - g[b]->position[i]);
        }
        return distance;
    }

    double getDist(const double * pos_a, const double * pos_b) const {
        //        boost::interprocess::scoped_lock<Mutex>(*m_mutex);
        double distance = 0;
        for (int i = 0; i < this->gng_dim; ++i) {
            distance += (pos_a[i] - pos_b[i])*(pos_a[i] - pos_b[i]);
        }
        return distance;
    }


    
    int newNode(const double *position) {
        if (firstFree == -1) {
            DBG(10, "RAMGNGGraph::newNode() growing pool");
            boost::interprocess::scoped_lock<Mutex>(*mutex);
            this->resizeGraph();

        }

        int createdNode = firstFree; //taki sam jak w g_node_pool

        maximum_index = createdNode > maximum_index 
                ? createdNode : maximum_index;

        g[createdNode] = new Node();
        g[createdNode]->occupied = true;
        g[createdNode]->nr = createdNode;
        firstFree = next_free[createdNode];
        g[createdNode]->edgesCount = 0;

        //  g_pool[createdNode].edges = new EdgeStorage();


        ++this->nodes; //zwiekszam licznik wierzcholkow //na koncu zeby sie nie wywalil przypadkowo


        memcpy(&(g[createdNode]->position[0]), position, sizeof (double) *(this->gng_dim)); //param

        //TODO: this should be tracked by GNGAlgorithm
        g[createdNode]->error = 0.0;
        g[createdNode]->error_cycle = 0;
        g[createdNode]->error_new = 0.0;
        return createdNode;
    }

    bool deleteNode(int x) {
        if (existsNode(x)) {
            --nodes;
            if(maximum_index==x) 
                maximum_index = maximum_index - 1;
            
            next_free[x] = firstFree;
            firstFree = x;
            delete g[x];
        }
        return true;
    }

    EdgeIterator removeEdge(int a, int b) {

        FOREACH(edg, *g[a]) {
            if ((*edg)->nr == b) {
                Edge * ptr_rev = (Edge*) ((**edg).rev);
                Edge * ptr = (Edge*) (&(**edg));

                g[b]->erase(find(g[b]->begin(), g[b]->end(), (*edg)->rev));
                edg = g[a]->erase(edg);


                delete ptr;
                delete ptr_rev;

                g[a]->edgesCount--;
                g[b]->edgesCount--;

                return edg;
            }
        }
        DBG(10, "ExtGraphNodeManager()::removeEdge Not found edge!");
        return g[a]->end();
    }

    void addUDEdge(int a, int b) {
        if (a == b) throw "Added loop to the graph";

        g[a]->push_back(new Edge(b));
        g[b]->push_back(new Edge(a));

        g[a]->back()->rev = g[b]->back();
        g[b]->back()->rev = g[a]->back();

        g[a]->edgesCount++;
        g[b]->edgesCount++;
    }

    void addDEdge(int a, int b) {
        throw "Not implemented";

        g[a]->push_back(new Edge(b));
        g[a]->edgesCount++;
    }


    
    std::string reportPool() {
        std::stringstream ss;
        for (unsigned int i = 0; i < g.size(); ++i) {
            string tmp = "";
            if (g[i]) {
                tmp = tmp + to_string(*g[i]) + ":";
                FOREACH(it2, *(g[i]))
                tmp = tmp + to_string((*it2)->nr) + "[" + to_string((((*it2)->rev))->nr) + "],";
                tmp = tmp + "\n";
            }
            ss << tmp;
        }
        return ss.str();
    }

    ~RAMGNGGraph() {
        for (int i = 0; i < g.size(); ++i) {
            if (g[i]) {
                FOREACH(edg, *g[i]) delete *edg;
                delete g[i];
            }
        }
    }
private:
    void resizeGraph() {
        unsigned int previous_size = g.size();
        g.resize(2 * previous_size);
        for (int i = previous_size; i < g.size(); ++i) {
            g[i] = 0;
        }
        next_free.resize(2 * previous_size);
        for (int i = previous_size - 1; i < g.size() - 1; ++i) {
            next_free[i] = i + 1;
        }
        next_free[g.size() - 1] = -1;
    }
};









#endif        /* SHGRAPH_H */