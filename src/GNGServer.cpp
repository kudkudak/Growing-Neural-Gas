#include "GNGServer.h"



GNGServer::GNGServer(GNGConfiguration * configuration_ptr):
error_statistics_end(0), error_statistics_start(0), error_statistics_size(10000), error_statistics_delay_ms(1000),
error_statistics(error_statistics_size, 0.0)
{
    m_current_dataset_memory_was_set = false;
    m_running_thread_created = false;

	GNGConfiguration configuration = *configuration_ptr;


    DBG(10, "GNGServer()::constructing GNGServer");



    if(!configuration.check_correctness())
        throw BasicException("Invalid configuration passed to GNGServer");

    if(configuration.interprocess_communication)
        throw BasicException("Current version doesn't allow for crossprocess communication");

    this->current_configuration = configuration; //assign configuration


    if(current_configuration.graph_storage == GNGConfiguration::RAMMemory){
        //Nothing to do here
    }else{
        throw BasicException("Not supported GNGConfiguration type");
    }

//        if(configuration.interprocess_communication){
//            this->shm->new_named_segment("MessageBufor",current_configuration.message_bufor_size);
//            this->message_bufor_mutex = this->shm->get_named_segment("MessageBufor")->construct<
//                    boost::interprocess::interprocess_mutex>("MessageBuforMutex")();
//        }


    /** Construct database **/
    if(current_configuration.datasetType == GNGConfiguration::DatasetSampling){
    	DBG(11, "GNGServer::Constructing Normal Sampling Prob Dataset");
        this->gngDataset = std::auto_ptr<GNGDataset>(
                new GNGDatasetSimple<GNGDatasetStorageRAM>
                (&database_mutex, current_configuration.dim, current_configuration.
                		dataset_vertex_extra_dim, 0));
    }
    else if(current_configuration.datasetType == GNGConfiguration::DatasetSamplingProb){
    	    //Add probability to layout
    		DBG(11, "GNGServer::Constructing Sampling Prob Dataset");
            this->gngDataset = std::auto_ptr<GNGDataset>(
                    new GNGDatasetSimple<GNGDatasetStorageRAM>
                    (&database_mutex, current_configuration.dim, current_configuration.dataset_vertex_extra_dim, 1, 0));
    }
    else if(current_configuration.datasetType == GNGConfiguration::DatasetSeq){
    	DBG(11, "GNGServer::Constructing Normal Seq Dataset");
            this->gngDataset = std::auto_ptr<GNGDataset>(
                    new GNGDatasetSimple<GNGDatasetStorageRAM>
                    (&database_mutex, current_configuration.dim, current_configuration.dataset_vertex_extra_dim, 0, -1, false));
    }
    else{
    	cerr<<"Passed dataset type "<<current_configuration.datasetType<<endl;
    	cerr<<GNGConfiguration::DatasetSampling<<endl;
    	cerr<<GNGConfiguration::DatasetSamplingProb<<endl;
    	DBG(11, "GNGServer::Not recognized dataset");
        throw BasicException("Database type not supported "+to_string(current_configuration.datasetType));
    }


    DBG(10, "GNGServer()::gngDatabase constructed");

    /** Construct graph **/
    if(current_configuration.graph_storage == GNGConfiguration::SharedMemory){
        throw BasicException("Not supported SharedMemory configuration");
//            SharedMemoryGraphStorage * storage
//                    = this->shm->get_named_segment("GraphStorage")->
//                    construct<SharedMemoryGraphStorage >("storage")(GNGServer::START_NODES);
//            this->gngGraph = std::auto_ptr<ExtGNGGraph<SharedMemoryGraphStorage> >(
//                    new SHGNGGraph<SharedMemoryGraphStorage>(&this->gngAlgorithmControl->grow_mutex, storage,
//                    configuration.dim));
//            DBG(10, "GNGServer()::constructed shared graph");
    }
    else if(current_configuration.graph_storage == GNGConfiguration::RAMMemory){

        this->gngGraph = std::auto_ptr<RAMGNGGraph<GNGNode, GNGEdge> >(
                new RAMGNGGraph<GNGNode, GNGEdge>(&grow_mutex,
                current_configuration.dim, current_configuration.starting_nodes));
    }else{
        throw BasicException("Not supported GNGConfiguration type");
    }


    if(current_configuration.load_graph_filename != "")
    {
    	DBG(10, "GNGServer():::loading serialized graph");
    	gngGraph->load(current_configuration.load_graph_filename);
    }


    DBG(10, "GNGServer()::constructing algorithm object");

    /** Initiliaze main computing object **/
    this->gngAlgorithm = std::auto_ptr<GNGAlgorithm>(new GNGAlgorithm
    (
    		this->gngGraph.get(), //I do not want algorithm to depend on boost
            this->gngDataset.get(),
            &current_configuration.orig[0],
            &current_configuration.axis[0],
            current_configuration.axis[0]*1.1, //only 2^dim //TODO: min
            current_configuration.max_nodes,
            current_configuration.max_age,
            current_configuration.alpha,
            current_configuration.beta,
            current_configuration.lambda,
            current_configuration.eps_v,
            current_configuration.eps_n,
            current_configuration.dim,
            current_configuration.uniformgrid_optimization,
            current_configuration.lazyheap_optimization,
            current_configuration.experimental_utility_option,
            current_configuration.experimental_utility_k
    ));


    DBG(10, "GNGServer()::constructed algorithm object");


}

GNGConfiguration GNGServer::current_configuration = GNGConfiguration::getDefaultConfiguration();
boost::mutex GNGServer::static_lock;

