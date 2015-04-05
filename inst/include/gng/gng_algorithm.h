/*
 * File: GNGAlgorithm.h
 * Author: Stanislaw "kudkudak" Jastrzebski <grimghil@gmail.com>
 *
 * Created on 11 sierpień 2012, 10:02
 */

#ifndef GNGALGORITHM_H
#define GNGALGORITHM_H

#include <globals.h>
#include <gng_dataset.h>
#include <gng_graph.h>
#include <gng_lazy_error_heap.h>
#include <uniform_grid.h>
#include <memory>

#include "utils/threading.h"
#include "utils/circular_buffer.h"

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

	/** Run main loop of the algorithm*/
	void runAlgorithm();

	///Retrieve closest node's gng_index to the example
	int predict(const std::vector<double> &);

	//Updates clustering field on the dataset kept in memory
	void updateClustering();

	void run(bool synchronized=true);
	void pause(bool synchronized=true);
	bool isRunning();
	void terminate(bool synchronized=true);

	unsigned getErrorIndex() const;
	void setMaxNodes(int value);
	int getIteration() const;
	double getMeanError();
	vector<pair<double, double> > getMeanErrorStatistics();

	//Retrieve clustering result.
	//@note pauses algorithm as many
	const vector<int> & get_clustering();

	virtual ~GNGAlgorithm();
public:
	//TODO: don't use list in UniformGrid
	typedef std::list<int> Node;

	int calculated_errors; //for convergence checking
	circular_buffer<pair<double, double> > m_mean_error; //error of the network
	int m_lambda; //lambda parameter
	double m_eps_w, m_eps_n; //epsilon of the winner and of the neighbour
	int m_max_age, m_max_nodes, m_iteration;

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

	GngStatus m_gng_status, m_gng_status_request;
	bool running;

	enum UtilityOptions {
		None, BasicUtility
	};

	gmum::recursive_mutex status_change_mutex;
	//rewrite to the same locking logic as get_clustering
	gmum::fast_mutex m_statistics_mutex;
	gmum::gmum_condition status_change_condition;

	vector<int> clustering_result;
private:
	//Main algorithm methods

	//@return error and closest node index
	std::pair<double, int> adapt(const double * ex, const double * extra);
	std::pair<int, int> _getNearestNeurons(const double *ex);

	void randomInit();
	void addNewNode();

	GNGNode ** LargestErrorNodesLazy();
	GNGNode ** LargestErrorNodes();

	bool stoppingCriterion();

	//Utility functions

	double calculateAccumulatedError();
	void testAgeCorrectness();
	void resetUniformGrid(double * orig, double *axis, double l);
	void resizeUniformGrid();

	//sets clustering assignment of given node
	void set_clustering(unsigned int ex, unsigned int node_idx);

	void increaseErrorNew(GNGNode * node, double error);
	void fixErrorNew(GNGNode * node);
	double getMaximumError() const;
	void decreaseAllErrorsNew();
	void decreaseErrorNew(GNGNode * node);
	void setErrorNew(GNGNode * node, double error);
	void increaseError(GNGNode * node, double error);
	void decreaseAllErrors();
	void decreaseError(GNGNode * node);
	void setError(GNGNode * node, double error);

	// Note: this code is not optimal and is inserted only for research purposes
	double getUtility(int i);
	void setUtility(int i, double u);
	void utilityCriterionCheck();
	void decreaseAllUtility();

	GngStatus gng_status() {
		return m_gng_status;
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
