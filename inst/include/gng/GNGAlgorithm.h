/*
 * File: GNGAlgorithm.h
 * Author: Stanislaw "kudkudak" Jastrzebski <grimghil@gmail.com>
 *
 * Created on 11 sierpień 2012, 10:02
 */


#ifndef GNGALGORITHM_H
#define GNGALGORITHM_H

#include <memory>

#include "utils/threading.h"
#include "utils/circular_buffer.h"

#include "GNGGlobals.h"
#include "GNGGraph.h"
#include "GNGDataset.h"
#include "UniformGrid.h"
#include "GNGLazyErrorHeap.h"
#include <string>
#include <limits>
using namespace std;

namespace gmum {

/**
 * The main class of the implementation dealing with computations.
 * It should be agnostic of inner working (memory management etc.) of the graph and database.
 * Also should not be concerned with locking logic.
 *
 *
 * @note Many algorithm results are retrievable only when it is paused for performance reasons
 */
class GNGAlgorithm {
public:
	typedef std::list<int> Node;

	circular_buffer<double> m_mean_error; //error of the network
	int m_lambda; //lambda parameter
	double m_eps_w, m_eps_n; //epsilon of the winner and of the neighbour
	int m_max_age;
	int m_max_nodes;
	int m_iteration;

	bool m_toggle_uniformgrid, m_toggle_lazyheap;

	double m_utility_k;
	int m_utility_option;


	double m_alpha, m_betha;
	double * m_betha_powers;
	int m_betha_powers_to_n_length;
	double * m_betha_powers_to_n;
	int m_betha_powers_size;
	double m_accumulated_error;

	int dim;
	boost::shared_ptr<Logger> m_logger;

	std::map<std::string, long int> times;

	double m_density_threshold, m_grow_rate;

	/** Constants used by lazy heap implementation */
	int s, c;

	GNGGraph & m_g;
	GNGDataset * g_db;
	UniformGrid<std::vector<Node>, Node, int> * ug;
	GNGLazyErrorHeap errorHeap;

	enum GngStatus {
		GNG_PREPARING, GNG_RUNNING, GNG_PAUSED, GNG_TERMINATED
	};

	GngStatus m_gng_status;
	bool running;

	enum UtilityOptions {
		None, BasicUtility
	};


	gmum::recursive_mutex status_change_mutex;
	//rewrite to the same locking logic as get_clustering
	gmum::fast_mutex m_statistics_mutex;
	gmum::gmum_condition status_change_condition;

	GngStatus gng_status() {
		return m_gng_status;
	}

	vector<int> clustering_result;
public:


	/** Run main loop of the algorithm*/
	void runAlgorithm();

	/**Construct main algorithm object, that will hold mid-results
	 * @param alg_memory_lock When locked algorithm is not running anything that is memory dangerous
	 * @param g GNGGraph object implementing graph interface
	 * @param db GNGDataset object
	 * @param boundingbox_origin Starting point for reference system
	 * @param boundingbox_axis Axis lengths for reference system
	 * @param l Starting box size for uniform grid. Advised to be set to axis[0]/4 (TODO: move to the end of parameters list)
	 * @param max_nodes Maximum number of nodes
	 * @param max_age Maximum age of edge
	 * @param alpha See original paper(TODO: add description)
	 * @param betha See original paper (TODO: add description)
	 * @param lambda Every lambda new vertex is added
	 * @param eps_v See original paper(TODO: add description)
	 * @param eps_n See original paper (TODO: add description)
	 * @param dim Dimensionality
	 */
	GNGAlgorithm(GNGGraph * g, GNGDataset * db, double * boundingbox_origin,
			double * boundingbox_axis, double l, int max_nodes = 1000,
			int max_age = 200, double alpha = 0.95, double betha = 0.9995,
			double lambda = 200, double eps_w = 0.05, double eps_n = 0.0006,
			int dim = 3, bool uniformgrid_optimization = true,
			bool lazyheap_optimization = true, unsigned int utility_option =
					GNGConfiguration::UtilityOff, double utility_k = -1,
			boost::shared_ptr<Logger> logger = boost::shared_ptr<Logger>());

	///Retrieve closest node's gng_index to the example
	int predict(const std::vector<double> &);

	/** Start algorithm loop */
	void run() {
		this->m_gng_status = GNG_RUNNING;
		this->status_change_condition.notify_all();
	}

	bool isRunning() const{
		return this->m_gng_status == GNG_RUNNING;
	}

	/** Pause algorithm loop */
	void pause() {
		this->m_gng_status = GNG_PAUSED;
		this->status_change_condition.notify_all();
	}

	/** Terminate the algorithm */
	void terminate() {
		this->m_gng_status = GNG_TERMINATED;
		this->status_change_condition.notify_all();
	}

	void setMaxNodes(int value) {
		m_max_nodes = value;
	}

	int getIteration() const{
		return m_iteration;
	}

	double getMeanError() {

		gmum::scoped_lock<gmum::fast_mutex> alg_lock(m_statistics_mutex);
		DBG(m_logger, 3, gmum::to_string(m_mean_error.size()));
		if(m_mean_error.size() == 0){

			return std::numeric_limits<double>::max();
		}else{

			return m_mean_error[m_mean_error.size()-1];
		}
	}

	vector<double> getMeanErrorStatistics() {
		gmum::scoped_lock<gmum::fast_mutex> alg_lock(m_statistics_mutex);
		if(m_mean_error.size() == 0){
			return vector<double>(1, std::numeric_limits<double>::max());
		}else{
			return vector<double>(m_mean_error.begin(), m_mean_error.end());
		}
	}

	//Retrieve clustering result.
	//@note pauses algorithm as many
	const vector<int> & get_clustering(){
		bool was_running = false;
		if(isRunning()){
			was_running = true;
			pause();
			while(isRunning()){
				DBG(m_logger, 2, "get_clustering waiting for pause");
				this->status_change_condition.wait(this->status_change_mutex);
			}
		}
		vector<int> & result = clustering_result;
		if(was_running)
			run();

		return result;
	}

	virtual ~GNGAlgorithm() {
		delete[] m_betha_powers_to_n;
		delete[] m_betha_powers;
	}


private:


	std::pair<int, int> _getNearestNeurons(const double *ex){
		if (m_toggle_uniformgrid) {
				DBG(m_logger, 1, "GNGAlgorithm::Adapt::Graph size " + to_string(m_g.get_number_nodes()));
				std::vector<int> nearest_index = ug->findNearest(ex, 2); //TwoNearestNodes(ex->position);
				DBG(m_logger, 1, "GNGAlgorithm::Adapt::Found nearest");

				#ifdef GMUM_DEBUG_2
						if (nearest_index[0] == nearest_index[1]) {
							cerr<<"Adapt::Found same nearest_indexes!~! "+to_string(nearest_index[0])<<endl;
							throw BasicException("Found same nearest_indexes");  //something went wrong (-1==-1 też)
						}
				#endif


				#ifdef GMUM_DEBUG_2
						assert(m_g[nearest_index[1]].position > m_g.get_dist(m_g[nearest_index[0]].position, ex));
				#endif

				return std::pair<int, int>(nearest_index[0], nearest_index[1]);
			} else {
				DBG(m_logger, 1, "GNGAlgorithm::just called TwoNearestNodes");
				int start_index = 0;
				while (!m_g.existsNode(start_index))
					++start_index;

				double dist0 = m_g.get_dist(ex, m_g[start_index].position);
				int best_0 = start_index, best_1 = -1;
				for (int i = start_index + 1; i <= m_g.get_maximum_index(); ++i) {
					if (m_g.existsNode(i)) {
						double new_dist = m_g.get_dist(ex, m_g[i].position);
						if (dist0 > new_dist) {
							dist0 = new_dist;
							best_0 = i;
						}
					}
				}

				DBG(m_logger, 1, "finding next\n");

				start_index = 0;
				while (!m_g.existsNode(start_index) || start_index == best_0)
					++start_index;
				double dist1 = m_g.get_dist(ex, m_g[start_index].position);
				best_1 = start_index;

				for (int i = start_index + 1; i <= m_g.get_maximum_index(); ++i) { //another idea for storing list of actual nodes?
					if (m_g.existsNode(i) && i != best_0) {
						double new_dist = m_g.get_dist(ex, m_g[i].position);
						if (dist1 > new_dist) {
							dist1 = new_dist;
							best_1 = i;
						}
					}
				}


				#ifdef GMUM_DEBUG_2
					assert(dist1 > dist0);
				#endif

				return std::pair<int, int>(best_0, best_1);
			}
	}

	double calculateAccumulatedError();

	void testAgeCorrectness();

	void resetUniformGrid(double * orig, double *axis, double l) {
		ug->purge(orig, axis, l);
		int maximum_index = m_g.get_maximum_index();

		REP(i, maximum_index + 1)
		{
			if (m_g.existsNode(i))
				ug->insert(m_g[i].position, m_g[i].nr);
		}
	}

	GNGNode ** LargestErrorNodesLazy();

	GNGNode ** LargestErrorNodes();


	void randomInit();

	void addNewNode();

	//@return error and closest node index
	std::pair<double, int> adapt(const double * ex, const double * extra);

	void resizeUniformGrid();

	bool stoppingCriterion() {
		return m_g.get_number_nodes() > m_max_nodes;
	}

	void increaseErrorNew(GNGNode * node, double error) {
		fixErrorNew(node);
		assert(m_lambda - s <= m_betha_powers_size -1);
		node->error += m_betha_powers[m_lambda - s] * error;
		errorHeap.updateLazy(node->nr);
	}

	void fixErrorNew(GNGNode * node) {

		if (node->error_cycle == c)
			return;

		while(c - node->error_cycle > m_betha_powers_to_n_length - 1){
			DBG_2(m_logger, 5, "Recreating m_betha_powers_to_n");
			delete[] m_betha_powers_to_n;
			m_betha_powers_to_n_length *= 2;
			m_betha_powers_to_n = new double[m_betha_powers_to_n_length];
			REP(i, m_betha_powers_to_n_length)
			m_betha_powers_to_n[i] = std::pow(m_betha, m_lambda * (double) (i));
		}

		assert(c - node->error_cycle  <= m_betha_powers_to_n_length -1);

		node->error = m_betha_powers_to_n[c - node->error_cycle] * node->error;
		node->error_cycle = c;

	}


	void set_clustering(unsigned int ex, unsigned int node_idx){

		if(ex + 1 > clustering_result.size()){
			DBG(m_logger, 6, "Resizing clustering_result to "+to_string(g_db->size()));
			clustering_result.resize(g_db->size());
		}

		//Can potentially happen in case of shrinkage of dataset size
		if(ex + 1 > clustering_result.size()){
			cerr<<"g_db->size mismatch with ex index?\n";
			return;
		}


		clustering_result[ex] = node_idx;
	}

	double getMaximumError() const {
		double max_error = 0;
		int maximum_index = m_g.get_maximum_index();
		REP(i,maximum_index+1)
			if (m_g.existsNode(i))
				max_error = std::max(max_error, m_g[i].error);
		return max_error;
	}

	void decreaseAllErrorsNew() {
		return;
	}

	void decreaseErrorNew(GNGNode * node) {
		fixErrorNew(node);
		node->error = m_alpha * node->error;
		errorHeap.updateLazy(node->nr);
	}

	void setErrorNew(GNGNode * node, double error) {
		node->error = error;
		node->error_cycle = c;
		errorHeap.insertLazy(node->nr);
	}

	void increaseError(GNGNode * node, double error) {
		node->error += error;
	}

	void decreaseAllErrors() {
		int maximum_index = m_g.get_maximum_index();
		REP(i,maximum_index+1)
			if (m_g.existsNode(i))
				m_g[i].error = m_betha * m_g[i].error;
	}

	void decreaseError(GNGNode * node) {
		node->error = m_alpha * node->error;
	}

	void setError(GNGNode * node, double error) {
		node->error = error;
	}

	// Note: this code is not optimal and is inserted only for research purposes

	double getUtility(int i) {
		return m_g[i].utility;
	}

	void setUtility(int i, double u) {
		m_g[i].utility = u;
	}

	void utilityCriterionCheck() {

		if (m_g.get_number_nodes() < 10)
			return; //just in case

		double max_error = this->getMaximumError();
		int maximum_index = m_g.get_maximum_index();

		double min_utility = 100000000;
		int min_utility_index = -1;

		for (int i = 0; i <= maximum_index; ++i)
			if (min_utility > getUtility(i)) {
				min_utility = getUtility(i);
				min_utility_index = i;
			}

		if (m_g.existsNode(min_utility_index) && max_error / getUtility(min_utility_index) > m_utility_k) {

			DBG(m_logger,2, "GNGAlgorithm:: removing node with utility "+gmum::to_string(getUtility(min_utility_index)) + " max error "+gmum::to_string(max_error));

			DBG(m_logger,2,gmum::to_string<double>(max_error));

			GNGNode::EdgeIterator edg = m_g[min_utility_index].begin();
			while (edg != m_g[min_utility_index].end()) {
				int nr = (*edg)->nr;
				edg = m_g.removeUDEdge(min_utility_index, nr);
			}

			m_g.deleteNode(min_utility_index);
			setUtility(min_utility_index, 0);
		}

	}
	void decreaseAllUtility() {
		int maximum_index = m_g.get_maximum_index();
		for (int i = 0; i <= maximum_index; ++i)
			if (m_g.existsNode(i))
				setUtility(i, getUtility(i) * (m_betha));
	}
};

/**Design hack for passing distance function dist(index, position)*/
struct GNGGraphAccessHack {
	static GNGGraph * pool;
	static double dist(int index, double *position) {
		return pool->get_euclidean_dist((*pool)[index].position, position);
	}
};


}

#endif
