/*
 * File: SHGraph.h
 * Author: staszek
 *
 * Created on 11 sierpień 2012, 09:07
 */

#ifndef GNGGraph_H
#define GNGGraph_H

#include <string>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <iostream>
#include <fstream>
#include <cassert>
#include <boost/shared_ptr.hpp>

#include "utils/threading.h"
#include "utils/utils.h"

#include "GNGNode.h"
#include "GNGGlobals.h"

using namespace std;

namespace gmum {
/** Graph interface for GNGAlgorithm.
 *
 *
 */
class GNGGraph {
public:

	enum GNGDistanceFunction {
		Euclidean, Cosine
	};

	virtual ~ GNGGraph() {
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
	virtual unsigned int get_maximum_index() const = 0;

	/*
	 * @return True if exists node in the graph
	 */
	virtual bool existsNode(unsigned int) const = 0;

	virtual int get_dim() const = 0;

	virtual GNGNode & operator[](int i) = 0;

	virtual unsigned int get_number_nodes() const = 0;

	//TODO: move it to GNGNode
	virtual double get_dist(int a, int b) = 0;

	//TODO: move it to GNGNode
	virtual double get_euclidean_dist(const double * pos_1, const double * pos_2) const= 0;

	//TODO: move it to GNGNode
	virtual double get_dist(const double *pos_a, const double *pos_b) const = 0;

	/* Initialize node with position attribute */
	virtual int newNode(const double *position) = 0;

	virtual bool deleteNode(int x) = 0;

	virtual bool isEdge(int a, int b) const = 0;

	//Again:dependency on GNGNode::
	virtual typename GNGNode::EdgeIterator removeUDEdge(int a, int b) = 0;

	virtual void addUDEdge(int a, int b) = 0;

	virtual void addDEdge(int a, int b) = 0;

	virtual std::string reportPool() {
		return "";
	}

	virtual void load(std::istream & in) = 0;
	virtual void serialize(std::ostream & out) = 0;


};

/* @note: Not thread safe. To be used from one thread only!
 *
 * Can be used by external thread by has to be locked. All operations moving
 * whole memory sectory will unlock. Elegant solution: locking interface
 *
 * It allows for easy erasing of nodes.
 *
 * Node: implements GNGNode interface
 * Edge: implements GNGEdge interface
 * Mutex: implements lock and unlock interface
 *
 * TODO: change GNGEdge* to GNGEdge (problems with rev)
 * TODO: edges ~ gng_dim - maybe use this for better efficiency?
 */
template<class Node, class Edge, class Mutex = gmum::recursive_mutex> class RAMGNGGraph: public GNGGraph {
	/** Mutex provided externally for synchronization*/
	Mutex * mutex;

	std::vector<Node> g;
	std::vector<bool> occupied;

	//TODO: change to vector
	std::vector<double> positions; //as continuous array for speed/caching purposes, could be vector

	int maximum_index;
	unsigned int nodes;

	unsigned int gng_dim;

	boost::shared_ptr<Logger> m_logger;

public:
	/** Indicates next free vertex */
	std::vector<int> next_free; //TODO: has to be public : /
	int first_free;

	GNGDistanceFunction dist_fnc;

	typedef typename Node::EdgeIterator EdgeIterator;

	RAMGNGGraph(Mutex * mutex, unsigned int dim, int initial_pool_size, GNGDistanceFunction dist_fnc = Euclidean,
			boost::shared_ptr<Logger> logger = boost::shared_ptr<Logger>()) :
			maximum_index(-1), mutex(mutex), gng_dim(dim), first_free(-1), nodes(0), dist_fnc(dist_fnc), m_logger(logger) {

		positions.resize(initial_pool_size * gng_dim);

		//Initialize graph data structures
		g.resize(initial_pool_size);

		for (int i = 0; i < initial_pool_size; ++i)
			g[i].reserve(gng_dim);

		occupied.resize(initial_pool_size);

		for (int i = 0; i < initial_pool_size; ++i)
			occupied[i] = false;
		next_free.resize(initial_pool_size);

		for (int i = 0; i < initial_pool_size - 1; ++i)
			next_free[i] = i + 1;
		next_free[initial_pool_size - 1] = -1;
		first_free = 0;

	}

	/** This is specific for GNG Graph - e
	 * each node is assigned index. It fetches maximum node index
	 */

	virtual unsigned int get_maximum_index() const {
		return this->maximum_index;
	}

	/* @note NOT THREAD SAFE - USE ONLY FROM ALGORITHM THREAD OR LOCK
	 * @return True if exists node in the graph
	 */
	virtual bool existsNode(unsigned i) const {
		return i < nodes && occupied[i];
	}

	///NOT THREAD SAFE - USE ONLY FROM ALGORITHM THREAD OR LOCK
	bool isEdge(int a, int b) const {

		FOREACH(edg, g[a])
		{
			if ((*edg)->nr == b)
				return true;
		}
		return false;
	}

	///NOT THREAD SAFE - USE ONLY FROM ALGORITHM THREAD OR LOCK
	const double *getPosition(int nr) const {
		return g[nr].position;
	}

	unsigned int get_number_nodes() const {
		return this->nodes;
	}

	///NOT THREAD SAFE - USE ONLY FROM ALGORITHM THREAD OR LOCK
	Node &
	operator[](int i) {
		return g[i];
	}

	///NOT THREAD SAFE - USE ONLY FROM ALGORITHM THREAD OR LOCK
	double get_dist(int a, int b) {
		return get_dist(g[a].position, g[b].position);
	}

	double get_euclidean_dist(const double *pos_a, const double *pos_b) const {
		double distance = 0;
		for (int i = 0; i < this->gng_dim; ++i)
			distance += (pos_a[i] - pos_b[i]) * (pos_a[i] - pos_b[i]);

		return distance;
	}

	///NOT THREAD SAFE - USE ONLY FROM ALGORITHM THREAD OR LOCK
	double get_dist(const double *pos_a, const double *pos_b) const {
		if (dist_fnc == Euclidean) {
			double distance = 0;
			for (int i = 0; i < this->gng_dim; ++i)
				distance += (pos_a[i] - pos_b[i]) * (pos_a[i] - pos_b[i]);

			return distance;
		} else if (dist_fnc == Cosine) {
			double norm_1 = 0, norm_2 = 0, distance = 0;

			for (int i = 0; i < this->gng_dim; ++i) {
				norm_1 += (pos_a[i]) * (pos_a[i]);
				norm_2 += (pos_b[i]) * (pos_b[i]);
				distance += pos_a[i] * pos_b[i];
			}

			norm_1 = sqrt(norm_1);
			norm_2 = sqrt(norm_2);
			return 1.0 - distance / (norm_1 * norm_2);
		}
	}

	///NOT THREAD SAFE - USE ONLY FROM ALGORITHM THREAD OR LOCK
	int newNode(const double *position) {
		if (first_free == -1) {
			DBG(m_logger,10, "RAMGNGGraph::newNode() growing pool");
			this->resizeGraph();

		}

		int createdNode = first_free; //taki sam jak w g_node_pool

		maximum_index = createdNode > maximum_index ? createdNode : maximum_index;

		//Assuming it is clear here
#ifdef GMUM_DEBUG
		assert(g[createdNode].size() == 0);
#endif

		// Initialize node
		g[createdNode].position = &positions[createdNode * gng_dim];
		occupied[createdNode] = true;
		g[createdNode].nr = createdNode;
		g[createdNode].edgesCount = 0;
		g[createdNode].utility = 0.0;
		g[createdNode]._position_owner = false;
		g[createdNode].dim = gng_dim;
		g[createdNode].extra_data = 0.0;

		first_free = next_free[createdNode];

		//zwiekszam licznik wierzcholkow //na koncu zeby sie nie wywalil przypadkowo
		++this->nodes;
		memcpy(&(g[createdNode].position[0]), position,
				sizeof(double) * (this->gng_dim));        //param

		//TODO: this should be tracked by GNGAlgorithm
		g[createdNode].error = 0.0;
		g[createdNode].error_cycle = 0;

		return createdNode;

	}

	///NOT THREAD SAFE - USE ONLY FROM ALGORITHM THREAD OR LOCK
	bool deleteNode(int x) {

		this->lock();
		if (existsNode(x)) {
			//TODO: add automatic erasing edges
			assert(g[x].size() == 0);

			--nodes;
			if (maximum_index == x)
				maximum_index = maximum_index - 1;

			occupied[x] = false;
			next_free[x] = first_free;
			first_free = x;
			this->unlock();
			return true;

		}

		this->unlock();
		return false;

	}

	///NOT THREAD SAFE - USE ONLY FROM ALGORITHM THREAD OR LOCK
	EdgeIterator removeUDEdge(int a, int b) {

		this->lock();

		FOREACH(edg, g[a])
		{
			if ((*edg)->nr == b) {
				Edge *ptr_rev = (Edge *) ((**edg).rev);
				Edge *ptr = (Edge *) (&(**edg));

				g[b].erase(find(g[b].begin(), g[b].end(), (*edg)->rev));
				edg = g[a].erase(edg);

				delete ptr;
				delete ptr_rev;

				g[a].edgesCount--;
				g[b].edgesCount--;
				this->unlock();
				return edg;
			}
		}

		this->unlock();
		DBG(m_logger,10, "ExtGraphNodeManager()::removeEdge Not found edge!");
		return g[a].end();

	}

	///NOT THREAD SAFE - USE ONLY FROM ALGORITHM THREAD OR LOCK
	void addUDEdge(int a, int b) {

		this->lock();

		if (a == b)
			throw "Added loop to the graph";

		g[a].push_back(new Edge(b));
		g[b].push_back(new Edge(a));

		g[a].back()->rev = g[b].back();
		g[b].back()->rev = g[a].back();

		g[a].edgesCount++;
		g[b].edgesCount++;
		this->unlock();

	}

	///NOT THREAD SAFE - USE ONLY FROM ALGORITHM THREAD OR LOCK

	void addDEdge(int a, int b) {
		throw BasicException("Not implemented");
	}

	///NOT THREAD SAFE - USE ONLY FROM ALGORITHM THREAD OR LOCK
	std::string reportPool() {
		std::stringstream ss;
		for (unsigned int i = 0; i < g.size(); ++i) {
			string tmp = "";
			if (occupied[i]) {
				tmp = tmp + to_str(g[i]) + ":";
				FOREACH(it2, g[i])
				{
					tmp += to_str((*it2)->nr) + "["
							+ to_str((((*it2)->rev))->nr) + "],";
				}
				tmp = tmp + "\n";
			}
			ss << tmp;
		}
		return ss.str();
	}

	~RAMGNGGraph() {
		for (int i = 0; i < g.size(); ++i) {
			if (occupied[i]) {
				FOREACH(edg, g[i])
					delete *edg;
			}
		}

	}

	virtual int get_dim() const {
		return gng_dim;
	}

	virtual void lock() {
		mutex->lock();
	}

	virtual void unlock() {
		mutex->unlock();
	}


	/*
	 * format is [N] [gng_dim] N* [0/1 + vertex] N*[ [l] l*[gng_idx]]
	 */
	void serialize(std::ostream &  output) {
		this->lock();


		vector<double> S;
		S.reserve(10000);

		//Header
		S.push_back((double) (g.size()));
		S.push_back((double) (maximum_index + 1));
		S.push_back((double) gng_dim);
		S.push_back((double) first_free);
		S.push_back((double) nodes);

		DBG(m_logger,7, "GNGGraph::Serializing nodes");
		//Nodes
		for (int i = 0; i < g.size(); ++i) {
			if (existsNode(i)) {
				S.push_back((double) 1);
				vector<double> serialized_node = g[i].dumpVertexData();

				std::copy(serialized_node.begin(), serialized_node.end(),
						std::back_inserter(S));
			} else {
				S.push_back((double) 0);
			}
		} DBG(m_logger,7, "GNGGraph::Serializing edges");
		//Edges
		for (int i = 0; i < g.size(); ++i) {
			if (existsNode(i)) {
				vector<double> serialized_node = g[i].dumpEdges();
				std::copy(serialized_node.begin(), serialized_node.end(),
						std::back_inserter(S));
			} else {
				S.push_back((double) 0);
			}
		} DBG(m_logger,7, "GNGGraph::Serializing nextFree");
		//NextFree
		for (int i = 0; i < g.size(); ++i) {
			S.push_back((double) next_free[i]);
		} DBG(m_logger,7, "GNGGraph::Serialize;:writing out");

		_write_bin_vect(output, S);


		this->unlock();
	}
	void load(std::istream &  input) {
		this->lock();

		DBG(m_logger,7, "GNGGraph:: loading ");

		vector<double> S = _load_bin_vector(input);
		vector<double>::iterator itr = S.begin();
		//Header
		unsigned int bufor_size = (int) *itr;
		maximum_index = (int) *(++itr) - 1;
		gng_dim = (int) *(++itr);
		first_free = (int) *(++itr);
		nodes = (int) *(++itr);

		DBG(m_logger,5, "Read in "+to_str(bufor_size) +" sized graph with "+
				" max_index="+to_str(maximum_index)+" gng_dim="+to_str(gng_dim)+" "+
				"first_free="+to_str(first_free)+" nodes="+to_str(nodes)
		);

		positions.clear();
		g.clear();
		next_free.clear();
		occupied.clear();

		occupied.resize(bufor_size);
		g.resize(bufor_size);
		next_free.resize(bufor_size);
		positions.resize((bufor_size + 1) * gng_dim);

		for (int i = 0; i < bufor_size; ++i) {
			occupied[i] = false;
			g[i].reserve(gng_dim + 2);
		}

		//Deserialize nodes
		for (int i = 0; i < g.size(); ++i) {
			int tmp = (int) *(++itr);
			occupied[i] = (bool) tmp;
			if (occupied[i])
				g[i].loadVertexData(itr, gng_dim, &positions[i * gng_dim]);

		}

		//Deserialize edges
		for (int i = 0; i < g.size(); ++i) {
			int edges_length = (int) *(++itr);

			for (int j = 0; j < edges_length; ++j) {
				int gng_endpoint_index = (int) *(++itr);
				if (gng_endpoint_index > i)
					this->addUDEdge(i, gng_endpoint_index);
			}
		}

		//Deserialize nextFree
		for (int i = 0; i < g.size(); ++i) {
			next_free[i] = (int) *(++itr);
		}


		this->unlock();
	}

private:
	///NOT THREAD SAFE - USE ONLY FROM ALGORITHM THREAD OR LOCK
	void resizeGraph() {
		//DBG(m_logger,5, "GNGGraph::resizing graph from "+to_string(g.size()));
		DBG_2(m_logger,5, "GNGGraph::resizing");
		unsigned int previous_size = g.size();
		//Grow positions pool

		positions.resize(2 * previous_size * gng_dim);

		//Reassign memory pointers
		for (int i = 0; i < previous_size; ++i) {
			g[i].position = &positions[i * gng_dim];

		}

		g.resize(2 * previous_size);

		for (int i = 0; i < previous_size; ++i) {
			g[i].position = &positions[i * gng_dim];
		}

		occupied.resize(2 * previous_size);
		for (int i = previous_size; i < 2 * previous_size; ++i) {
			//                        g[i].reset();
			//                        g[i].reserve(gng_dim); //for speed purposes
			occupied[i] = false;
		}

		next_free.resize(2 * previous_size);
		for (int i = previous_size - 1; i < 2 * previous_size - 1; ++i) {
			next_free[i] = i + 1;
		}
		next_free[g.size() - 1] = -1;
		first_free = previous_size;

		DBG_2(m_logger,5, "GNGGraph::resizing done"); DBG(m_logger,5, to_str(first_free)); DBG(m_logger,5, to_str(next_free[previous_size]));
		//DBG(m_logger,5, "GNGGraph::resizing graph from "+to_string(g.size())+" done");
	}
};


std::string writeToGraphML(GNGGraph &g, string filename = "");

}
#endif
