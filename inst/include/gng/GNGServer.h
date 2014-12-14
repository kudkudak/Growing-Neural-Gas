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
		if (!m_running_thread_created) {
			DBG(m_logger,10, "GNGServer::runing algorithm thread");
			algorithm_thread = new gmum::gmum_thread(&GNGServer::_run,
					(void*) this);
			DBG(m_logger,10, "GNGServer::runing collect_statistics thread");


			m_running_thread_created = true;
		} else {
			gngAlgorithm->run();
		}
	}


	GNGConfiguration getConfiguration(){
		return current_configuration;
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
	void insertExamples(double * examples, unsigned int count,
			unsigned int size) {
		this->_handle_addExamples(examples, count, size);
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
	Rcpp::List getNode(unsigned int gng_index) {
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
		ret["extra_data"] = n.extra_data;

		vector<unsigned int> neigh(n.size());
		GNGNode::EdgeIterator edg = n.begin();
		unsigned i = 0;
		while(edg!=n.end()) {
			neigh[i++] = (*edg)->nr;
			++edg;
		}

		ret["neighbours"] = IntegerVector(neigh.begin(), neigh.end());

		gngGraph->unlock();

		return ret;
	}

	int Rpredict(Rcpp::NumericVector & ex) {
		std::vector<double> x(ex.begin(), ex.end());
		return gngAlgorithm->predict(x);
	}

	Rcpp::NumericVector RgetErrorStatistics() {
		vector<double> x = getMeanErrorStatistics();
		return NumericVector(x.begin(), x.end());
	}
	void RinsertExamples(Rcpp::NumericMatrix & ex) {
		arma::mat * points = new arma::mat(ex.begin(), ex.nrow(), ex.ncol(), false);

		//Check if normalised
		arma::Row<double> max_colwise = arma::max(*points, 0 /*dim*/);
		arma::Row<double> min_colwise = arma::min(*points, 0 /*dim*/);
		arma::Row<double> diff = max_colwise - min_colwise;
		float max = arma::max(diff), min = arma::min(diff);
		if(abs(max - min) > 2.0){
			cerr<<"Warning: it is advised to scale data to a constant range for optimal algorithm behavior \n";
		}

		//Check if data fits in bounding box
		if(current_configuration.uniformgrid_optimization){
			for(int i=0;i<current_configuration.dim; ++i){
				if(current_configuration.orig[i] > min_colwise[i] || current_configuration.orig[i]+current_configuration.axis[i] < max_colwise[i]){
					cerr<<"Error: please scale your data to match range passed to gng.optimized\n";
					cerr<<"Error: returning, did not insert examples\n";
					return;
				}
			}
		}


		arma::inplace_trans( *points, "lowmem");
		this->_handle_addExamples(points->memptr(),(unsigned int)points->n_cols,
				(unsigned int)points->n_rows*points->n_cols);
		arma::inplace_trans( *points, "lowmem");
	}
	//Not used in API but left for reference
	void RsetExamples(Rcpp::NumericMatrix & ex) {
		throw 1;

		//Release previous if was present
		if(m_current_dataset_memory_was_set) {
			throw "You cannot set example memory pool more than once!";
		}
		m_current_dataset_memory = wrap(ex);
		//We have to fix the object
		R_PreserveObject(wrap(ex));

		arma::mat * points = new arma::mat(ex.begin(), ex.nrow(), ex.ncol(), false);
		arma::inplace_trans( *points, "lowmem");
		this->_handle_addExamples(points->memptr(),(unsigned int)points->n_cols,
				(unsigned int)points->n_rows*points->n_cols, true);
		//Doesn't set the back
	}

	void dumpMemory() {
		gngDataset->lock();
		for(int i=0; i<gngDataset->getSize()*gngDataset->getDataDim();++i) {
			cout<<gngDataset->getMemoryPtr()[i]<<" ";
		}
		cout<<endl;
		gngDataset->unlock();
	}

#endif

	///Calculate error per node
	double calculateAvgErrorNode() {
		return this->getAlgorithm().calculateAccumulatedError()
				/ (this->getGraph().get_number_nodes() + 0.0f);
	}

	///Pause algorithm
	void pause() {
		getAlgorithm().pause();
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


	/**Section : protocol handling messages regardless of the source*/

	vector<double> _handle_getNode(int index) {
		return vector<double>();
	}

	void _handle_addExamples(double * examples, unsigned int count,
			unsigned int size, bool set = false) {
		gngDataset->lock();

		DBG(m_logger,5, "GNGServer::Adding examples with "+gmum::to_string(gngDataset->getDataDim())+" dimensionality");

		if (count * gngDataset->getDataDim() != size) {
			DBG(m_logger,10, "Wrong dimensionality is "+gmum::to_string(size)+" expected "+
					gmum::to_string(count*gngDataset->getDataDim()) + " data dim " + gmum::to_string(gngDataset->getDataDim()));
			throw BasicException("Wrong dimensionality. "
					"Check if you have added all field to "
					"position (for instance probability)");
		}

		//Handle coding
		DBG(m_logger,1, "GNGServer::_handle_AddExamples adding examples");
		if (!set)
			gngDataset->insertExamples(examples, count, size);
		else
			gngDataset->setExamples(examples, count, size);

		DBG(m_logger,7, "GNGServer::Database size "+gmum::to_string(gngDataset->getSize()));

		gngDataset->unlock();
	}

};

#endif
/* GNGSERVER_H */
