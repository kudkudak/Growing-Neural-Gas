#include "GNGServer.h"

GNGServer::GNGServer(GNGConfiguration configuration, std::istream * input_graph){
	init(configuration, input_graph);
}

void GNGServer::init(GNGConfiguration configuration, std::istream * input_graph){

	algorithm_thread = 0;
	m_current_dataset_memory_was_set = false;
	m_running_thread_created = false;

	m_logger = boost::shared_ptr<Logger>(new Logger(configuration.verbosity));

	DBG(m_logger,10, "GNGServer()::constructing GNGServer");

	if (!configuration.check_correctness())
		throw BasicException("Invalid configuration passed to GNGServer");

	this->current_configuration = configuration; //assign configuration

	if (current_configuration.graph_storage == GNGConfiguration::RAMMemory) {
		//Nothing to do here
	} else {
		throw BasicException("Not supported GNGConfiguration type");
	}

	/** Construct database **/
	if (current_configuration.datasetType
			== GNGConfiguration::DatasetSampling) {
		DBG(m_logger,11, "GNGServer::Constructing Normal Sampling Prob Dataset");
		this->gngDataset = std::auto_ptr<GNGDataset>(
				new GNGDatasetSimple<double>(&database_mutex,
						current_configuration.dim,
						true /* store_extra */, GNGDatasetSimple<double>::Sampling,
						m_logger));
	} else if (current_configuration.datasetType
			== GNGConfiguration::DatasetSamplingProb) {
		//Add probability to layout
		DBG(m_logger,11, "GNGServer::Constructing Sampling Prob Dataset");
		this->gngDataset = std::auto_ptr<GNGDataset>(
				new GNGDatasetSimple<double>(&database_mutex,
						current_configuration.dim,
						true /* store_extra */, GNGDatasetSimple<double>::SamplingProbability,
						m_logger));
	} else if (current_configuration.datasetType
			== GNGConfiguration::DatasetSeq) {
		DBG(m_logger,11, "GNGServer::Constructing Normal Seq Dataset");
		this->gngDataset = std::auto_ptr<GNGDataset>(
				new GNGDatasetSimple<double>(&database_mutex,
						current_configuration.dim,
						true /* store_extra */, GNGDatasetSimple<double>::Sequential,
						m_logger));
	} else {
		cerr << "Passed dataset type " << current_configuration.datasetType
				<< endl;
		cerr << GNGConfiguration::DatasetSampling << endl;
		cerr << GNGConfiguration::DatasetSamplingProb << endl;
		DBG(m_logger,11, "GNGServer::Not recognized dataset");
		throw BasicException(
				"Database type not supported "
						+ to_string(current_configuration.datasetType));
	}

	DBG(m_logger,10, "GNGServer()::gngDatabase constructed");

	/** Construct graph **/
	if (current_configuration.graph_storage == GNGConfiguration::SharedMemory) {
		throw BasicException("Not supported SharedMemory configuration");
	} else if (current_configuration.graph_storage
			== GNGConfiguration::RAMMemory) {
		REPORT(current_configuration.starting_nodes);

		this->gngGraph =
				std::auto_ptr<RAMGNGGraph<GNGNode, GNGEdge> >(
						new RAMGNGGraph<GNGNode, GNGEdge>(&grow_mutex,
								current_configuration.dim,
								current_configuration.starting_nodes,
								(gmum::GNGGraph::GNGDistanceFunction) current_configuration.distance_function,
								m_logger));
	} else {
		throw BasicException("Not supported GNGConfiguration type");
	}


	if(input_graph){
		this->gngGraph->load(*input_graph);
	}

	DBG(m_logger,10, "GNGServer()::constructing algorithm object");

	/** Initiliaze main computing object **/
	this->gngAlgorithm = std::auto_ptr<GNGAlgorithm>(
			new GNGAlgorithm(
					this->gngGraph.get(), //I do not want algorithm to depend on boost
					this->gngDataset.get(), &current_configuration.orig[0],
					&current_configuration.axis[0],
					current_configuration.axis[0] * 1.1, //only 2^dim //TODO: min
					current_configuration.max_nodes,
					current_configuration.max_age, current_configuration.alpha,
					current_configuration.beta, current_configuration.lambda,
					current_configuration.eps_w, current_configuration.eps_n,
					current_configuration.dim,
					current_configuration.uniformgrid_optimization,
					current_configuration.lazyheap_optimization,
					current_configuration.experimental_utility_option,
					current_configuration.experimental_utility_k, m_logger));

	DBG(m_logger,10, "GNGServer()::constructed algorithm object");

}


