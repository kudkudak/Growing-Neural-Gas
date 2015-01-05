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

	.field("alpha", &GNGConfiguration::alpha, "Alpha coefficient. "
			"Decrease the error variables of the nodes neighboring to the newly inserted node by this fraction. Default 0.5")
	.field("beta", &GNGConfiguration::beta, "Beta coefficient. "
			"Decrease the error variables of all node nodes by this fraction. Forgetting rate. Default 0.99")

	.field("eps_n", &GNGConfiguration::eps_n, "How strongly move neighbour node. Default 0.0006")
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
	.field("verbosity", &GNGConfiguration::verbosity)
	.method("check_correctness", &GNGConfiguration::check_correctness)
	.method("set_bounding_box", &GNGConfiguration::setBoundingBox);

	class_<GNGServer>("GNGServer")
			 .constructor<GNGConfiguration*>()
			 .constructor<std::string>()
			.method("save", &GNGServer::save)
			.method("run", &GNGServer::run)
			.method("getCurrentIteration", &GNGServer::getCurrentIteration)
			.method("pause", &GNGServer::pause)
			.method("terminate", &GNGServer::terminate)
			.method("getMeanError", &GNGServer::getMeanError)
			.method("nodeDistance", &GNGServer::nodeDistance)
			.method("getConfiguration", &GNGServer::getConfiguration)
			.method("getNumberNodes", &GNGServer::getNumberNodes)
			.method("exportToGraphML", &GNGServer::exportToGraphML)
			.method("getNode", &GNGServer::getNode)
			.method("insertExamples", &GNGServer::RinsertExamples)
			.method("getErrorStatistics", &GNGServer::RgetErrorStatistics)
			.method("predict", &GNGServer::Rpredict)
			.method("getLastNodeIndex", &GNGServer::_getLastNodeIndex);
}

#include <RcppArmadillo.h>

#endif
