/*
 * File: SHGraph.h
 * Author: staszek
 *
 * Created on 11 sierpień 2012, 09:07
 */

#ifndef GNGGraph_H
#define GNGGraph_H

#include <boost/smart_ptr/shared_ptr.hpp>
#include <gng_node.h>
#include <logger.h>

#include <threading.h>
#include <utils.h>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstring>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

namespace gmum {

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

	virtual double get_dist(int a, int b) = 0;

	virtual double get_euclidean_dist(const double * pos_1,
			const double * pos_2) const= 0;

	virtual double get_dist(const double *pos_a, const double *pos_b) const = 0;

	/* Initialize node with position attribute */
	virtual int newNode(const double *position) = 0;

	virtual bool deleteNode(int x) = 0;

	virtual bool isEdge(int a, int b) const = 0;

	virtual GNGNode::EdgeIterator removeUDEdge(int a, int b) = 0;

	virtual void addUDEdge(int a, int b) = 0;

	virtual void addDEdge(int a, int b) = 0;

	virtual std::string reportPool() {
		return "";
	}

	virtual void load(std::istream & in) = 0;
	virtual void serialize(std::ostream & out) = 0;

};

/*
 * Node: implements GNGNode interface
 * Edge: implements GNGEdge interface
 * Mutex: implements lock and unlock interface
 */
template<class Node, class Edge, class Mutex = gmum::recursive_mutex>
class RAMGNGGraph: public GNGGraph {
public:
	// Indicates next free vertex
	std::vector<int> next_free;
	int first_free;
	GNGDistanceFunction dist_fnc;

	typedef typename Node::EdgeIterator EdgeIterator;

	RAMGNGGraph(Mutex * mutex, unsigned int dim, int initial_pool_size,
			GNGDistanceFunction dist_fnc = Euclidean,
			boost::shared_ptr<Logger> logger = boost::shared_ptr<Logger>());

	Node & operator[](int i);

	int newNode(const double *position);
	bool deleteNode(int x);
	EdgeIterator removeUDEdge(int a, int b);
	void addUDEdge(int a, int b);
	void addDEdge(int a, int b);

	std::string reportPool();

	virtual int get_dim() const;
	double get_dist(int a, int b);
	double get_euclidean_dist(const double *pos_a, const double *pos_b) const;
	double get_dist(const double *pos_a, const double *pos_b) const;
	virtual unsigned int get_maximum_index() const;
	virtual bool existsNode(unsigned i) const;
	bool isEdge(int a, int b) const;
	const double *getPosition(int nr) const;
	unsigned int get_number_nodes() const;

	virtual void lock();
	virtual void unlock();

	/*
	 * format is [N] [gng_dim] N* [0/1 + vertex] N*[ [l] l*[gng_idx]]
	 */
	void serialize(std::ostream & output);
	void load(std::istream & input);

	~RAMGNGGraph();
private:
	Mutex * mutex;

	std::vector<Node> g;
	std::vector<bool> occupied;
	std::vector<double> positions; //as continuous array for speed/caching purposes, could be vector

	int maximum_index;
	unsigned int nodes, gng_dim;

	boost::shared_ptr<Logger> m_logger;

private:
	void resizeGraph();
};

#include <gng_graph.hpp>

static std::string writeToGraphML(GNGGraph &g, string filename = "");

}
#endif
