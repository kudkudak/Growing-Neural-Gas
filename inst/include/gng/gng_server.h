/*
 * File: GNGServer.h
 * Author: staszek
 *
 * Created on October 17, 2013, 8:12 PM
 */
#ifndef GNGSERVER_H
#define GNGSERVER_H

#include <globals.h>
#include <gng_algorithm.h>
#include <gng_configuration.h>
#include <gng_dataset.h>
#include <gng_defines.h>
#include <gng_graph.h>
#include <cstdlib>
#include <cstddef>
#include <map>
#include <exception>
#include <memory>

#include "utils/threading.h"
#include "utils/utils.h"


#ifdef RCPP_INTERFACE
#include <RcppArmadillo.h>
using namespace Rcpp;
using namespace arma;
#endif

static int gng_server_count=0;

using namespace gmum;

/** Holds together all logic and objects.*/
class GNGServer {
public:
	boost::shared_ptr<Logger> m_logger;

	/**Construct GNGServer using configuration*/
	GNGServer(GNGConfiguration configuration, std::istream * input_graph);

	GNGServer(std::string filename);

	void run();

	void pause();

	bool hasStarted() const;

	void terminate();

	double nodeDistance(int id1, int id2) const;

	void save(std::string filename);

	unsigned int getCurrentIteration() const;

	///Exports GNG state to file
	void exportToGraphML(std::string filename);

	///Insert examples
	void insertExamples(double * positions, double * extra,
			double * probability, unsigned int count, unsigned int dim);


    unsigned getDatasetSize() const;
	unsigned getGNGErrorIndex() const;
	bool isRunning() const;
	vector<double> getMeanErrorStatistics();
	unsigned int getNumberNodes() const;
	double getMeanError();
	GNGConfiguration getConfiguration();
	GNGAlgorithm & getAlgorithm();
	GNGGraph & getGraph();
	GNGDataset & getDatabase();

	~GNGServer();

#ifdef RCPP_INTERFACE
	//Constructor needed for RCPPInterface
	GNGServer(GNGConfiguration * configuration);

	///Moderately slow function returning node descriptors
	Rcpp::List getNode(int index);

	int Rpredict(Rcpp::NumericVector & r_ex);

	Rcpp::NumericVector RgetClustering();

	Rcpp::NumericVector RgetErrorStatistics();

	void RinsertExamples(Rcpp::NumericMatrix & r_points);

	void RinsertLabeledExamples(Rcpp::NumericMatrix & r_points,
			Rcpp::NumericVector r_extra);

	//This is tricky - used only by convertToIGraph in R, because
	//it might happen that we delete nodes and have bigger index of the last node
	//than actual nodes (especially in the case of utility version of GNG)
	unsigned int _getLastNodeIndex() const;

	///Calls updateClustering on the GNGAlgorithm object
	void _updateClustering();
#endif

private:
	int m_index;

	bool m_running_thread_created;

	gmum::gmum_thread * algorithm_thread;

	/** Mutex used for synchronization of graph access*/
	gmum::recursive_mutex grow_mutex;
	/** Mutex used for synchronization of graph access*/
	gmum::recursive_mutex database_mutex;
	/** Mutex used for synchronization of graph access*/
	gmum::recursive_mutex stat_mutex;

	GNGConfiguration current_configuration;

	std::auto_ptr<gmum::GNGAlgorithm> gngAlgorithm;
	std::auto_ptr<gmum::GNGGraph> gngGraph;
	std::auto_ptr<gmum::GNGDataset> gngDataset;

	//Called from constructors
	void init(GNGConfiguration configuration, std::istream * input_graph = 0);

private:
	/** Run GNG Server - runs in separate thread and returns control
	 * @note Runs one extra threads for communication.
	 */
	static void _run(void * server) {
		GNGServer * gng_server = (GNGServer*) server;
		try {
			DBG(gng_server->m_logger,10, "GNGServer::run::proceeding to algorithm");
			gng_server->getAlgorithm().runAlgorithm();
		} catch (std::exception & e) {
			cerr << "GNGServer::failed _run with " << e.what() << endl;
			DBG(gng_server->m_logger,10, e.what());
		}
	}

public:
	//legacy code
	static GNGServer * constructTestServer(GNGConfiguration config) {
		return new GNGServer(config, 0 /*input_graph*/);
	}
};

#endif
/* GNGSERVER_H */
