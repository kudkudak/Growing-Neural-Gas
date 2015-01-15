/*
 * File: GNGServer.h
 * Author: staszek
 *
 * Created on October 17, 2013, 8:12 PM
 */
#ifndef GNGSERVER_H
#define GNGSERVER_H


#include <cstdlib>
#include <cstddef>
#include <map>
#include <exception>
#include <memory>

#include "utils/threading.h"
#include "utils/utils.h"

#include "GNGDefines.h"
#include "GNGConfiguration.h"
#include "GNGGlobals.h"
#include "GNGGraph.h"
#include "GNGDataset.h"
#include "GNGAlgorithm.h"


#ifdef RCPP_INTERFACE
#include <RcppArmadillo.h>
using namespace Rcpp;
using namespace arma;
#endif

using namespace gmum;

/** Holds together all logic and objects.*/
class GNGServer {
	bool m_current_dataset_memory_was_set;
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

public:
	boost::shared_ptr<Logger> m_logger;

	/**Construct GNGServer using configuration*/
	GNGServer(GNGConfiguration configuration, std::istream * input_graph);

	GNGServer(std::string filename){
		std::ifstream input;
		input.open(filename.c_str(), ios::in | ios::binary);

		GNGConfiguration conf;
		conf.deserialize(input);

		init(conf, &input);
	}




	void run() {
		DBG(m_logger,10, "GNGServer::runing algorithm thread");
		algorithm_thread = new gmum::gmum_thread(&GNGServer::_run,
				(void*) this);
		DBG(m_logger,10, "GNGServer::runing collect_statistics thread");

		m_running_thread_created = true;
	}


	GNGConfiguration getConfiguration(){
		return current_configuration;
	}

	double nodeDistance(unsigned int id1, unsigned int id2) const{
		if(gngAlgorithm->isRunning()){
			cerr<<"Please pause algorithm before calling nodeDistance function\n";
			return -1.0;
		}
		return gngGraph->get_dist(id1, id2);
	}

	static GNGServer * constructTestServer(GNGConfiguration config) {
		return new GNGServer(config, 0 /*input_graph*/);
	}

	void save(std::string filename){
		if(current_configuration.experimental_utility_option == GNGConfiguration::UtilityBasicOn){
			cerr<<"Saving is not supported for gng.type.utility\n";
			return;
		}


		std::ofstream output;
		output.open(filename.c_str(), ios::out | ios::binary);

		current_configuration.serialize(output);

		try{
			gngGraph->lock();
			assert(filename != "");
			gngGraph->serialize(output);
		}catch(...){
			cerr<<"Failed exporting to GraphML\n";
			#ifdef DEBUG_GMUM
				throw BasicException("Failed exporting to GraphML\n");
			#endif
			gngGraph->unlock(); //No RAII, yes..
			return;
		}
		gngGraph->unlock();
	}

	unsigned int getCurrentIteration() const{
		return gngAlgorithm->getIteration();
	}

	//This is tricky - used only by convertToIGraph in R, because
	//it might happen that we delete nodes and have bigger index of the last node
	//than actual nodes (especially in the case of utility version of GNG)
	unsigned int _getLastNodeIndex() const{
		return gngGraph->get_maximum_index() + 1;
	}

	///Exports GNG state to file
	void exportToGraphML(std::string filename) {
		try{
			gngGraph->lock();
			assert(filename != "");
			writeToGraphML(getGraph(), filename);
		}catch(...){
			cerr<<"Failed exporting to GraphML\n";
			#ifdef DEBUG_GMUM
				throw BasicException("Failed exporting to GraphML\n");
			#endif
			gngGraph->unlock(); //No RAII, yes..
			return;
		}
		gngGraph->unlock();
	}

	///Insert examples
	void insertExamples(double * positions, double * extra, double * probability,
			unsigned int count, unsigned int dim) {
		gmum::scoped_lock<GNGDataset> lock(gngDataset.get());


		if (dim != current_configuration.dim) {
			DBG(m_logger,10, "Wrong dimensionality is "+gmum::to_string(count*dim)+" expected "+
					gmum::to_string(count*gngDataset->getDataDim()) + \
					" data dim " + gmum::to_string(gngDataset->size()));
			throw BasicException("Wrong dimensionality. "
					"Check if you have added all field to "
					"position (for instance probability)");
		}



		gngDataset->insertExamples(positions, extra, probability, count);
		DBG(m_logger,7, "GNGServer::Database size "+gmum::to_string(gngDataset->size()));

	}

	unsigned int getNumberNodes() const {
		int nr = this->gngGraph->get_number_nodes();
		return nr;
	}

	double getMeanError(){
		return gngAlgorithm->getMeanError();
	}

	vector<double> getMeanErrorStatistics() {
		return gngAlgorithm->getMeanErrorStatistics();
	}

#ifdef RCPP_INTERFACE
	//Constructor needed for RCPPInterface
	GNGServer(GNGConfiguration * configuration){
		init(*configuration, 0 /*input_graph*/);
	}
	SEXP m_current_dataset_memory; //will be deleted in ~delete
	///Moderately slow function returning node descriptors
	Rcpp::List getNode(int index) {
		int gng_index = index - 1; //1 based

		if(index <= 0){
			cerr<<"Indexing of nodes starts from 1 (R convention)\n";
			List ret;
			return ret;
		}

		gngGraph->lock();

		if(!gngGraph->existsNode(gng_index)) {
			List ret;
			return ret;
		}

		GNGNode & n = getGraph()[gng_index];
		NumericVector pos(n.position, n.position + gngDataset->getGNGDim());

		List ret;
		ret["pos"] = pos;
		ret["error"] = n.error;
		ret["label"] = n.extra_data;

		if(getConfiguration().experimental_utility_option != GNGConfiguration::UtilityOff){
			ret["utility"] = n.utility;
		}

		vector<unsigned int> neigh(n.size());
		GNGNode::EdgeIterator edg = n.begin();
		unsigned i = 0;
		while(edg!=n.end()) {
			neigh[i++] = (*edg)->nr + 1;
			++edg;
		}

		ret["neighbours"] = IntegerVector(neigh.begin(), neigh.end());

		gngGraph->unlock();

		return ret;
	}

	int Rpredict(Rcpp::NumericVector & r_ex) {
		return gngAlgorithm->predict(std::vector<double>(r_ex.begin(), r_ex.end()) );
	}

	Rcpp::NumericVector RgetClustering() {
		const vector<int> & x = gngAlgorithm->get_clustering();
		return NumericVector(x.begin(), x.end());
	}

	Rcpp::NumericVector RgetErrorStatistics() {
		vector<double> x = getMeanErrorStatistics();
		return NumericVector(x.begin(), x.end());
	}
	void RinsertExamples(Rcpp::NumericMatrix & r_points,
			Rcpp::NumericVector  r_extra =  Rcpp::NumericVector()) {
		std::vector<double> extra(r_extra.begin(), r_extra.end());
		arma::mat * points = new arma::mat(r_points.begin(), r_points.nrow(), r_points.ncol(), false);



		arma::Row<double> mean_colwise = arma::mean(*points, 0 /*dim*/);
		arma::Row<double> std_colwise = arma::stddev(*points, 0 /*dim*/);
		arma::Row<double> diff_std = arma::abs(std_colwise - 1.0);
		float max_diff_std = arma::max(diff_std), max_mean = arma::max(mean_colwise);
		if(max_diff_std > 0.1 || max_mean > 0.1){
			cerr<<max_diff_std<<" "<<max_mean<<endl;
			cerr<<"Warning: it is advised to scale data for optimal algorithm behavior to mean=1 std=0 \n";
		}

		//Check if data fits in bounding box
		if(current_configuration.uniformgrid_optimization){
			arma::Row<double> max_colwise = arma::max(*points, 0 /*dim*/);
			arma::Row<double> min_colwise = arma::min(*points, 0 /*dim*/);
			arma::Row<double> diff = max_colwise - min_colwise;
			float max = arma::max(diff), min = arma::min(diff);

			for(int i=0;i<current_configuration.dim; ++i){
				if(current_configuration.orig[i] > min_colwise[i] || current_configuration.orig[i]+current_configuration.axis[i] < max_colwise[i]){
					cerr<<"Error: each feature has to be in range <min, max> passed to gng.type.optimized \n";
					cerr<<"Error: returning, did not insert examples\n";
					return;
				}
			}
		}


		arma::inplace_trans( *points, "lowmem");

		if(extra.size()){
			insertExamples(points->memptr(), &extra[0], 0 /*probabilty vector*/,
					(unsigned int)points->n_cols, (unsigned int)points->n_rows);
		}else{
			insertExamples(points->memptr(), 0 /* extra vector */, 0 /*probabilty vector*/,
					(unsigned int)points->n_cols, (unsigned int)points->n_rows);
		}

		arma::inplace_trans( *points, "lowmem");
	}



#endif

	///Pause algorithm
	void pause() {
		gngAlgorithm->pause();
	}

	///Terminate algorithm
	void terminate() {
		getAlgorithm().terminate();
		DBG(m_logger,20, "GNGServer::getAlgorithm terminated, joining algorithm thread");
		if (algorithm_thread)
			algorithm_thread->join();
		DBG(m_logger,20, "GNGServer::algorithm thread terminated, joining statistic thread");
		gmum::sleep(100);
	}

	GNGAlgorithm & getAlgorithm() {
		return *this->gngAlgorithm.get();
	}
	GNGGraph & getGraph() {
		return *this->gngGraph.get();
	}
	GNGDataset & getDatabase() {
		return *this->gngDataset.get();
	}

	~GNGServer() {
		DBG(m_logger,10, "GNGServer::destructor called");
#ifdef RcppInterface

		if(m_current_dataset_memory_was_set) {
			R_ReleaseObject(m_current_dataset_memory);

		}
		//R Matrix will be deleted from R level
#endif
	}

private:

	/** Run GNG Server - runs in separate thread and returns control
	 * @note Runs one extra threads for communication.
	 */
	static void _run(void * server) {
		GNGServer * gng_server = (GNGServer*) server;
		try {
			DBG(gng_server->m_logger,10, "GNGServer::run::proceeding to algorithm");
			gng_server->getAlgorithm().run();
			gng_server->getAlgorithm().runAlgorithm();
		} catch (std::exception & e) {
			cerr << "GNGServer::failed _run with " << e.what() << endl;
			DBG(gng_server->m_logger,10, e.what());
		}
	}

};

#endif
/* GNGSERVER_H */
