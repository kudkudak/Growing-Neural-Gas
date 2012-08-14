library("methods")

setGenericVerif <- function(x,y){if(!isGeneric(x)){setGeneric(x,y)}else{}}

dyn.load("RcppInterface.so")

setClass("GNGClient",representation(pointer="externalptr"))

GNGClient_method <- function(name){
	paste("GNGClient",name,sep="__");
}

setMethod("initialize",
"GNGClient",function(.Object,...) {
	.Object@pointer <- .Call(GNGClient_method("new"),...)
	.Object
}
)

setMethod( "$", "GNGClient", function(x, name ) {
	function(...) .Call( GNGClient_method(name) , x@pointer , ... )
} )

