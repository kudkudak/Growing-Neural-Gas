#include <iostream>
#include <fstream>
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
using namespace gmum;

template<class Node, class Edge, class Mutex>
RAMGNGGraph<Node, Edge, Mutex>::RAMGNGGraph(Mutex * mutex, unsigned int dim,
		int initial_pool_size, GNGDistanceFunction dist_fnc,
		boost::shared_ptr<Logger> logger) :
		maximum_index(-1), mutex(mutex), gng_dim(dim), first_free(-1), nodes(0), dist_fnc(
				dist_fnc), m_logger(logger) {

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

template<class Node, class Edge, class Mutex>
RAMGNGGraph<Node, Edge, Mutex>::~RAMGNGGraph() {
	for (int i = 0; i < g.size(); ++i) {
		if (occupied[i]) {
			BOOST_FOREACH(GNGEdge * edg, g[i])
				delete edg;
		}
	}

}

template<class Node, class Edge, class Mutex>
unsigned int RAMGNGGraph<Node, Edge, Mutex>::get_maximum_index() const {
	return this->maximum_index;
}

template<class Node, class Edge, class Mutex>
bool RAMGNGGraph<Node, Edge, Mutex>::existsNode(unsigned i) const {
	return i < nodes && occupied[i];
}

template<class Node, class Edge, class Mutex>
bool RAMGNGGraph<Node, Edge, Mutex>::isEdge(int a, int b) const {

	BOOST_FOREACH(GNGEdge * edg, g[a]) {
		if ((edg)->nr == b)
			return true;
	}
	return false;
}

template<class Node, class Edge, class Mutex>
const double *RAMGNGGraph<Node, Edge, Mutex>::getPosition(int nr) const {
	return g[nr].position;
}

template<class Node, class Edge, class Mutex>
unsigned int RAMGNGGraph<Node, Edge, Mutex>::get_number_nodes() const {
	return this->nodes;
}

template<class Node, class Edge, class Mutex>
Node &
RAMGNGGraph<Node, Edge, Mutex>::operator[](int i) {
	return g[i];
}

template<class Node, class Edge, class Mutex>
double RAMGNGGraph<Node, Edge, Mutex>::get_dist(int a, int b) {
	return get_dist(g[a].position, g[b].position);
}

template<class Node, class Edge, class Mutex>
double RAMGNGGraph<Node, Edge, Mutex>::get_euclidean_dist(const double *pos_a,
		const double *pos_b) const {
	double distance = 0;
	for (int i = 0; i < this->gng_dim; ++i)
		distance += (pos_a[i] - pos_b[i]) * (pos_a[i] - pos_b[i]);

	return distance;
}

template<class Node, class Edge, class Mutex>
double RAMGNGGraph<Node, Edge, Mutex>::get_dist(const double *pos_a,
		const double *pos_b) const {
	if (dist_fnc == Euclidean) {
		double distance = 0;
		for (size_t i = 0; i < this->gng_dim; ++i)
			distance += (pos_a[i] - pos_b[i]) * (pos_a[i] - pos_b[i]);

		return distance;
	} else if (dist_fnc == Cosine) {
		double norm_1 = 0, norm_2 = 0, distance = 0;

		for (size_t i = 0; i < this->gng_dim; ++i) {
			norm_1 += (pos_a[i]) * (pos_a[i]);
			norm_2 += (pos_b[i]) * (pos_b[i]);
			distance += pos_a[i] * pos_b[i];
		}

		norm_1 = sqrt(norm_1);
		norm_2 = sqrt(norm_2);
		return 1.0 - distance / (norm_1 * norm_2);
	}
}

template<class Node, class Edge, class Mutex>
int RAMGNGGraph<Node, Edge, Mutex>::newNode(const double *position) {
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

template<class Node, class Edge, class Mutex>
bool RAMGNGGraph<Node, Edge, Mutex>::deleteNode(int x) {

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

template<class Node, class Edge, class Mutex>
typename RAMGNGGraph<Node, Edge, Mutex>::EdgeIterator RAMGNGGraph<Node, Edge,
		Mutex>::removeUDEdge(int a, int b) {

	this->lock();

	for (typename Node::iterator edg = g[a].begin(); edg != g[a].end(); ++edg) {
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

template<class Node, class Edge, class Mutex>
void RAMGNGGraph<Node, Edge, Mutex>::addUDEdge(int a, int b) {

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

template<class Node, class Edge, class Mutex>
void RAMGNGGraph<Node, Edge, Mutex>::addDEdge(int a, int b) {
	throw BasicException("Not implemented");
}

template<class Node, class Edge, class Mutex>
std::string RAMGNGGraph<Node, Edge, Mutex>::reportPool() {
	std::stringstream ss;
	for (unsigned int i = 0; i < g.size(); ++i) {
		string tmp = "";
		if (occupied[i]) {
			tmp = tmp + to_str(g[i]) + ":";
			BOOST_FOREACH(GNGEdge * it2, g[i]) {
				tmp += to_str((it2)->nr) + "[" + to_str((((it2)->rev))->nr)
						+ "],";
			}
			tmp = tmp + "\n";
		}
		ss << tmp;
	}
	return ss.str();
}

template<class Node, class Edge, class Mutex>
int RAMGNGGraph<Node, Edge, Mutex>::get_dim() const {
	return gng_dim;
}
template<class Node, class Edge, class Mutex>
void RAMGNGGraph<Node, Edge, Mutex>::lock() {
	mutex->lock();
}

template<class Node, class Edge, class Mutex>
void RAMGNGGraph<Node, Edge, Mutex>::unlock() {
	mutex->unlock();
}

template<class Node, class Edge, class Mutex>
void RAMGNGGraph<Node, Edge, Mutex>::serialize(std::ostream & output) {
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
	}DBG(m_logger,7, "GNGGraph::Serializing edges");
	//Edges
	for (int i = 0; i < g.size(); ++i) {
		if (existsNode(i)) {
			vector<double> serialized_node = g[i].dumpEdges();
			std::copy(serialized_node.begin(), serialized_node.end(),
					std::back_inserter(S));
		} else {
			S.push_back((double) 0);
		}
	}DBG(m_logger,7, "GNGGraph::Serializing nextFree");
	//NextFree
	for (int i = 0; i < g.size(); ++i) {
		S.push_back((double) next_free[i]);
	}DBG(m_logger,7, "GNGGraph::Serialize;:writing out");

	_write_bin_vect(output, S);

	this->unlock();
}

template<class Node, class Edge, class Mutex>
void RAMGNGGraph<Node, Edge, Mutex>::load(std::istream & input) {
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

	for (size_t i = 0; i < bufor_size; ++i) {
		occupied[i] = false;
		g[i].reserve(gng_dim + 2);
	}

	//Deserialize nodes
	for (size_t i = 0; i < g.size(); ++i) {
		int tmp = (int) *(++itr);
		occupied[i] = (bool) tmp;
		if (occupied[i])
			g[i].loadVertexData(itr, gng_dim, &positions[i * gng_dim]);

	}

	//Deserialize edges
	for (size_t i = 0; i < g.size(); ++i) {
		int edges_length = (int) *(++itr);

		for (int j = 0; j < edges_length; ++j) {
			int gng_endpoint_index = (int) *(++itr);
			if (gng_endpoint_index > i)
				this->addUDEdge(i, gng_endpoint_index);
		}
	}

	//Deserialize nextFree
	for (size_t i = 0; i < g.size(); ++i) {
		next_free[i] = (int) *(++itr);
	}

	this->unlock();
}

template<class Node, class Edge, class Mutex>
void RAMGNGGraph<Node, Edge, Mutex>::resizeGraph() {
	//DBG(m_logger,5, "GNGGraph::resizing graph from "+to_string(g.size()));
	DBG_2(m_logger,5, "GNGGraph::resizing");
	unsigned int previous_size = g.size();
	//Grow positions pool

	positions.resize(2 * previous_size * gng_dim);

	//Reassign memory pointers
	for (size_t i = 0; i < previous_size; ++i) {
		g[i].position = &positions[i * gng_dim];

	}

	g.resize(2 * previous_size);

	for (size_t i = 0; i < previous_size; ++i) {
		g[i].position = &positions[i * gng_dim];
	}

	occupied.resize(2 * previous_size);
	for (int i = previous_size; i < 2 * previous_size; ++i) {
		//                        g[i].reset();
		//                        g[i].reserve(gng_dim); //for speed purposes
		occupied[i] = false;
	}

	next_free.resize(2 * previous_size);
	for (size_t i = previous_size - 1; i < 2 * previous_size - 1; ++i) {
		next_free[i] = i + 1;
	}
	next_free[g.size() - 1] = -1;
	first_free = previous_size;

	DBG_2(m_logger,5, "GNGGraph::resizing done");DBG(m_logger,5, to_str(first_free));DBG(m_logger,5, to_str(next_free[previous_size]));
}

static void writeToGraphML(GNGGraph &g, std::ostream & out) {

	g.lock();

	out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	out
			<< "<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd\">\n";
	out
			<< "<key id=\"key0\" for=\"edge\" attr.name=\"dist\" attr.type=\"double\" />\n";
	out
			<< "<key id=\"key1\" for=\"node\" attr.name=\"error\" attr.type=\"double\" />\n";
	out
			<< "<key id=\"key2\" for=\"node\" attr.name=\"label\" attr.type=\"double\" />\n";
	out
			<< "<key id=\"key3\" for=\"node\" attr.name=\"gng_index\" attr.type=\"int\" />\n";
	out
			<< "<key id=\"key4\" for=\"node\" attr.name=\"utility\" attr.type=\"double\" />\n";
	out
			<< "<key id=\"key5\" for=\"node\" attr.name=\"v0\" attr.type=\"double\" />\n";
	out
			<< "<key id=\"key6\" for=\"node\" attr.name=\"v1\" attr.type=\"double\" />\n";
	out
			<< "<key id=\"key7\" for=\"node\" attr.name=\"v2\" attr.type=\"double\" />\n";
	out
			<< "<graph id=\"G\" edgedefault=\"undirected\" parse.nodeids=\"canonical\" parse.edgeids=\"canonical\" parse.order=\"nodesfirst\">\n";

	std::map<int, int> gng_index_to_graph_index;

	unsigned int k = 0;
	for (int i = 0; i <= g.get_maximum_index(); ++i) {

		if (g.existsNode(i)) {
			gng_index_to_graph_index[g[i].nr] = k; //TODO:To be honest I dnt remember purpose of g[i].nr..

			out << "<node id=\"n" << k++ << "\">\n";
			out << "<data key=\"key1\">" << g[i].error << "</data>\n";
			out << "<data key=\"key2\">" << g[i].extra_data << "</data>\n";
			out << "<data key=\"key3\">" << g[i].nr << "</data>\n";
			out << "<data key=\"key4\">" << g[i].utility << "</data>\n";
			out << "<data key=\"key5\">" << g[i].position[0] << "</data>\n";
			out << "<data key=\"key6\">" << g[i].position[1] << "</data>\n";
			out << "<data key=\"key7\">" << g[i].position[2] << "</data>\n";
			out << "</node>\n";
		}
	}

	unsigned int l = 0;

	for (unsigned int i = 0; i <= g.get_maximum_index(); ++i)
		if (g.existsNode(i)) {
			FOREACH(GNGEdge * edg, g[i]) {
				if (g[i].nr > (edg)->nr) { //directed!
					out << "<edge id=\"e" << l++ << "\" source=\"n"
							<< gng_index_to_graph_index[(edg)->nr]
							<< "\" target=\"n"
							<< gng_index_to_graph_index[g[i].nr] << "\">\n";
					out << "<data key=\"key0\">" << g.get_dist(i, (edg)->nr)
							<< "</data>";
					out << "</edge>\n";
				}
			}

		}

	out << "</graph>\n</graphml>\n";
	g.unlock();

}

static std::string writeToGraphML(GNGGraph &g, string filename) {

	if (filename == "") {
		std::stringstream ss;
		writeToGraphML(g, ss);
		return ss.str();
	} else {
		ofstream myfile(filename.c_str());
		writeToGraphML(g, myfile);
		myfile.close();
		return "";
	}

}

