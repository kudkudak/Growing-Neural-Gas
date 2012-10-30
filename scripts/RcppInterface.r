library("methods")

setGenericVerif <- function(x,y){if(!isGeneric(x)){setGeneric(x,y)}else{}}

dyn.load("RcppInterface.so")

setClass("GNGClient",representation(pointer="externalptr"))

GNGClient_method <- function(name){
	paste("GNGClient",name,sep="__");
}
setClass("GNGClient",representation(pointer="externalptr"))
setMethod("initialize",
"GNGClient",function(.Object,...) {
	.Object@pointer <- .Call(GNGClient_method("new"),...)
	.Object
}
)


planePoint<-function(r,center){
	if(!hasArg(r)) r<-1.0
	if(!hasArg(center)) center<-c(0,0,0)
	
	point<-center
	point[1]<-point[1]+r*runif(1.0)
	point[2]<-point[2]+r*runif(1.0)
	point[3]<-point[3]

	return(point)
}

spherePoint<-function(r,center){
	if(!hasArg(r)) r<-1.0
	if(!hasArg(center)) center<-c(0,0,0)

	alpha<-runif(1)*2*pi
	beta<-runif(1)*pi
	
	point<-center
	point[1]<-point[1]+r*cos(alpha)*sin(beta)
	point[2]<-point[2]+r*sin(alpha)*sin(beta)
	point[3]<-point[3]+r*cos(beta)

	return(point)
}

GNGAddExamples<-function(x,preset,preset_size,...){
	.Call(GNGClient_method("addExamples"),x@pointer,...)			
	#if(hasArg(preset)){
	#	examples<-matrix(0,preset_size,3)		
	#	if(preset=="sphere"){
	#		for(i in 1:preset_size) examples[i,]=spherePoint()		
	#	}		
	#	else if(preset=="plane"){
	#		for(i in 1:preset_size) examples[i,]=planePoint()		
	#	}
	#	.Call(GNGClient_method("addExamples"),x@pointer,examples)	
	#}
	#else{
	#	.Call(GNGClient_method("addExamples"),x@pointer,...)	
	#}
}

setMethod( "$", "GNGClient", function(x, name ) { 
	#if(name=="addExamples") function(...) GNGAddExamples(x,...)
	function(...) .Call( GNGClient_method(name) , x@pointer , ... )
} )


createGNGServer <- function(... ){
	.Call("GNGRunServer",...)
	return(0)
}

GNGCreateServer <- function(... ){
	parallel(.Call("GNGRunServer",...))
	return(0)
}

GNGVisualiseStepAnimation <- function(step,pause,... ){
	iteration<-1
	source("VisualisePointsSingle.r")
	return(0)
}
GNGVisualise <- function(... ){
	iteration<-1
	source("VisualisePointsSingle.r")
	return(0)
}

GNGSetParams<- function(max_nodes,orig,axis,dim,uniformgrid,lazyheap,debug_level){
	if(hasArg(max_nodes)) .Call("GNGSet__max_nodes",max_nodes)
	if(hasArg(dim)) .Call("GNGSet__dim",dim)
	if(hasArg(orig) & hasArg(axis)) .Call("GNGSet__boundingbox",orig,axis)
	if(hasArg(uniformgrid)) .Call("GNGSet__uniformgrid",uniformgrid)
	if(hasArg(lazyheap)) .Call("GNGSet__lazyheap",lazyheap)
	if(hasArg(debug_level)) .Call("GNGSet__debug_level",debug_level)
	return(0)
}


