/*
 * File constructs R Interface, exports necessary classes and functions using Rcpp package
 */

#define RCPP_INTERFACE
#include <Rcpp.h>
//#include <Rcpp/Module.h>
//#include <armadillo>

#include "GNG.h"
#include "GNGServer.h"

using namespace Rcpp;

DebugCollector dbg;
int GNGNode::dim =0;
RCPP_EXPOSED_CLASS(GNGConfiguration)
RCPP_EXPOSED_CLASS(GNGServer)




RCPP_MODULE(gng_module333){
	/// GNGConfiguration class
	class_<GNGConfiguration>("GNGConfiguration" )
	.constructor()

	.field( "message_bufor_size", &GNGConfiguration::message_bufor_size );

	class_<GNGServer>("GNGServer").
      constructor<GNGConfiguration*>()
			.method("getConfiguration", &GNGServer::getConfiguration);
		//	.method("insertExamples", &GNGServer::RinsertExamples);

}



