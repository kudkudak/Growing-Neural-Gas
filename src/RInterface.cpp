/*
 * File constructs R Interface, exports necessary classes and functions using Rcpp package
 */

#include <RcppArmadillo.h>
#include "GNG.h"

using namespace Rcpp;

//
RCPP_MODULE(gng_module){
	/// GNGConfiguration class
	class_<GNGConfiguration>("GNGConfiguration" )
	.constructor()
	.field( "message_bufor_size", &GNGConfiguration::message_bufor_size )
	;
}



