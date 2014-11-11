/*
 * File constructs R Interface, exports necessary classes and functions using Rcpp package
 */

#ifdef RCPP_INTERFACE

#include <RcppCommon.h>
using namespace Rcpp;

class GNGConfiguration;
class GNGServer;

RCPP_EXPOSED_CLASS(GNGConfiguration);
RCPP_EXPOSED_CLASS(GNGServer);



#include "GNG.h"
#include "GNGServer.h"
#include "GNGConfiguration.h"
using namespace gmum;




RCPP_MODULE(gng_module){
	class_<GNGConfiguration>("GNGConfiguration" )
	.constructor()

	.field("uniformgrid_optimization", &GNGConfiguration::uniformgrid_optimization, "Uniform grid optimization" )
	.field("lazyheap_optimization", &GNGConfiguration::lazyheap_optimization )
	.method("get_uniform_grid_axis", &GNGConfiguration::getUniformGridAxis)
	.method("set_uniform_grid_axis", &GNGConfiguration::setUniformGridAxis)
	.method("get_uniform_grid_origin", &GNGConfiguration::getUniformGridOrigin)
	.method("set_uniform_grid_origin", &GNGConfiguration::setUniformGridOrigin)

	.field("alpha", &GNGConfiguration::alpha, "Alpha coefficient. "
			"Decrease the error variables of the nodes neighboring to the newly inserted node by this fraction. Default 0.5")
	.field("beta", &GNGConfiguration::beta, "Beta coefficient. "
			"Decrease the error variables of all node nodes by this fraction. Forgetting rate. Default 0.99")

	.field("eps_n", &GNGConfiguration::eps_n, "How strongly move neighbour node. Default 0.0006")
	.field("load_graph_filename", &GNGConfiguration::load_graph_filename, "Default empty. Load serialized graph by serialize_graph method.")
	.field("experimental_utility_option", &GNGConfiguration::experimental_utility_option, "Default 0 (off). You can turn it on to 1, but remember to turn off optimizations. Likely will change in the future.")
	.field("experimental_utility_k",
			&GNGConfiguration::experimental_utility_k, "Default 1.3 (note: option is off by default). ")

	.field("eps_w", &GNGConfiguration::eps_w, "How strongly move winner node. Default 0.05")
	.field("max_edge_age", &GNGConfiguration::max_age, "Max edge age")
	.field("dim", &GNGConfiguration::dim, "Vertex position dimensionality")
	.field("lambda", &GNGConfiguration::lambda, "Every lambda iteration is added new vertex. Default 200")
	.field("dataset_type", &GNGConfiguration::datasetType, "Dataset type. Currently supported:"
			"2: DatasetBagging - examples are sampled from dataset with equal probability, "
			"3: DatasetBaggingProbability - examples are sampled with probability equal to pos_dim+vertex_dim coordinate (last number in vector)")
	.field("max_nodes", &GNGConfiguration::max_nodes)
	.field("vertex_extra_data_dim", &GNGConfiguration::dataset_vertex_extra_dim)

	.field("distance_function", &GNGConfiguration::distance_function,
			"Distance function used: Euclidean, or Cosine. Note that "
			"cosine isn't a metric and all theoretical guarantees of GNG won't work"
			"Default Euclidean.")

	.method("check_correctness", &GNGConfiguration::check_correctness);

	class_<GNGServer>("GNGServer").
			 constructor<GNGConfiguration*>()
			.method("dump_memory", &GNGServer::dumpMemory)
			.method("set_memory_move_examples", &GNGServer::RsetExamples, "This function is tricky. It will use"
					"your memory to store examples, so you shouldn't modify the matrix afterwards")
			.method("run", &GNGServer::run)
			.method("pause", &GNGServer::pause)
			.method("dump_graph", &GNGServer::serializeGraph)
			.method("set_debug_level", &GNGServer::setDebugLevel)
			.method("get_configuration", &GNGServer::getConfiguration)
			.method("terminate", &GNGServer::terminate)

			.method("get_mean_error", &GNGServer::getMeanError)
			.method("get_number_nodes", &GNGServer::getNumberNodes)
			.method("export_to_graphml", &GNGServer::exportsToGraphML)
			.method("get_node", &GNGServer::getNode)
			.method("insert_examples", &GNGServer::RinsertExamples)
			.method("get_error_statistics", &GNGServer::getErrorStatistics)
			.method("predict", &GNGServer::Rpredict);
}

#include <RcppArmadillo.h>

#endif
