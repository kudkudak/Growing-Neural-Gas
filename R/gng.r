# Documenting S4: https://github.com/variani/pckdev/wiki/Documenting-with-roxygen2
# 
# 
# devtools::clean_dll(".")

library(devtools)

gng.dataset.bagging.prob <- 3
gng.dataset.bagging <- 2
gng.dataset.sequential <-1

gng.plot.2d <- 1
gng.plot.rgl3d <- 2


loadModule('gng_module', TRUE)

# # Load dynamic library
# delayedAssign(".server_example", local({
#   .conf <- new(GNGConfiguration)
#   .server_example <- new(GNGServer, .conf )
# }))


#' Constructor of GrowingNeuralGas object
#' 
#' @param beta coefficient. Decrease the error variables of all node nodes by this fraction. Forgetting rate. Default 0.99
#' @param alpha Alpha coefficient. Decrease the error variables of the nodes neighboring to the newly inserted node by this fraction. Default 0.5
#' @param uniformgrid.optimization TRUE/FALSE. You cannot use utility option with this, so please pass FALSE here then.
#' @param lazyheap.optimization TRUE/FALSE. You cannot use utility option with this, so please pass FALSE here then.
#' @param max.nodes Maximum number of nodes (after reaching this size it will continue running, but won't add new nodes)
#' @param eps_n Default 0.05
#' @param eps_v Default 0.0006
#' @param dataset.type Dataset type. Possibilities gng.dataset.bagging, gng.dataset.bagging.prob, gng.dataset.sequential
#' 
#' @name GrowingNeuralGas_initialize
GNG <- function(dataset_type=gng.dataset.sequential, beta=0.99, 
                alpha=0.5, uniformgrid_optimization=TRUE, 
                lazyheap_optimization=TRUE, max_nodes=1000, eps_n=0.05, 
                eps_v = 0.0006, dim=-1, uniformgrid_boundingbox_sides=c(), uniformgrid_boundingbox_origin=c()){
  
  
  if((length(uniformgrid_boundingbox_sides)==0 || length(uniformgrid_boundingbox_origin)==0) && uniformgrid_optimization==TRUE){
    stop("Please define bounding box for your data if you are using uniform grid. uniformgrid.boundingbox.sides is a
         dim sized vector defining lengths of the sides of the box, whereas uniformgrid.boundingbox.origin defines 
         origin of the box. Note that uniform grid optimization will give significant speedup, but only for low-dim data.
         ")  
    
  }
  
  config <- new(GNGConfiguration)

  # Fill in configuration
  config$dataset_type=dataset_type
  config$beta = beta
  config$alpha = alpha
  config$uniformgrid_optimization = uniformgrid_optimization
  config$lazyheap_optimization = lazyheap_optimization
  config$max_nodes = max_nodes
  config$eps_n = eps_n
  config$eps_v = eps_v
  config$dim = dim
  
  if(config$uniformgrid_optimization){
    config$set_uniform_grid_axis(uniformgrid_boundingbox_sides)
    config$set_uniform_grid_origin(uniformgrid_boundingbox_orig)
  }

  if(config$uniformgrid_optimization &&
       (length(uniformgrid_boundingbox_sides)!=length(uniformgrid_boundingbox_orig)
        || length(uniformgrid_boundingbox_orig) != dim)){
    
    stop("Make sure that dimensions of bounding box and vertex position match")
  }
  
  if(!config$check_correctness()){
    stop("Passed incorrect parameters.")
  }
  
  # Construct server
  new(GNGServer, config)
}

# Construct necessary generics
if (!isGeneric("node"))
  setGeneric("node", 
             function(x, gng_id, ...) standardGeneric("node"))

if (!isGeneric("convert_igraph"))
  setGeneric("convert_igraph", 
             function(object, ...) standardGeneric("convert_igraph"))

if (!isGeneric("run"))
  setGeneric("run", 
             function(object, ...) standardGeneric("run"))

if (!isGeneric("pause"))
  setGeneric("pause", 
             function(object, ...) standardGeneric("pause"))

if (!isGeneric("terminate"))
  setGeneric("terminate", 
             function(object, ...) standardGeneric("terminate"))


if (!isGeneric("insert_examples"))
  setGeneric("insert_examples", 
             function(object, ...) standardGeneric("insert_examples"))

if (!isGeneric("number_nodes"))
  setGeneric("number_nodes", 
             function(object, ...) standardGeneric("number_nodes"))

#' Plot GNG
#'
#' @param mode = gng.plot.rgl3d  (rgl plot, requires rgl library) or gng.plot.2d (igraph plot)
setMethod("plot", 
          
          signature(x="Rcpp_GNGServer"),
          function(x, mode){
              print(x)
              print(mode)
                  if(x$get_number_nodes() > 4000){
                    warning("Trying to plot very large graph (>4000 nodes). It might take a while.")
                  }
                  if(x$get_number_nodes() == 0){
                     stop("Empty graph")
                  }
                  
                  if(mode == gng.plot.rgl3d && !("rgl" %in% rownames(installed.packages()))){
                    stop("Please install rgl and reload the package to plot 3d")
                  }
                  
                  if(mode == gng.plot.rgl3d){
                    .gng.plot3d(x)
                  }
                  else if(mode == gng.plot.2d){
                    .gng.plot2d(x)
                  }
}
)


setMethod("print", 
          
          signature(x="Rcpp_GNGServer"),
          function(x){
              sprintf("Growing Neural Gas, nodes %d with mean error %f", 
                      x$get_number_nodes(), x$get_mean_error())
          }
)

setMethod("summary", 
          
          signature(object="Rcpp_GNGServer"),
          function(object){
            sprintf("Growing Neural Gas, nodes %d with mean error %f", 
                    object$get_number_nodes(), object$get_mean_error())
            print("Mean errors[s]: ")
            print(object$get_error_statistics())
          }
)


#' Get node descriptor from graph
#'
#' @param gng_id gng id of the node NOTE: might differ from one in exported igraph
setMethod("node" ,
          signature(x="Rcpp_GNGServer", gng_id="numeric"),
          function(x, gng_id){
                x$get_node(gng_id)
          })

#' Run algorithm
setMethod("run" ,
          signature(object="Rcpp_GNGServer"),
          function(object){
             object$run()
          })


#' Pause algorithm
setMethod("pause" ,
          signature(object="Rcpp_GNGServer"),
          function(object){
            object$pause()
          })

#' Terminate algorithm
setMethod("terminate" ,
          signature(object="Rcpp_GNGServer"),
          function(object){
            object$terminate()
          })


#' Get number of nodes
setMethod("number_nodes" ,
          signature(object="Rcpp_GNGServer"),
          function(object){
            object$get_number_nodes()
          })

source("R/igraph-utils.r")

#' Get node descriptor from graph
#'
#' @note This function will dump graph to .graphml file on this first and then will remove
#' the file. Be cautious with huge graphs!
#' 
#' @param gng_id gng id of the node NOTE: nmight differ from one in exported igraph
setMethod("convert_igraph" ,
          signature(object="Rcpp_GNGServer"),
          .gng.construct_igraph)


#' Find closest example
#' @param x Vector of dimensionality of vertex
#' @return gng_index of the closest example
setMethod("predict" ,
          signature(object="Rcpp_GNGServer"),
          function(object, x){
               object$predict(x)
          })

#' Insert examples
setMethod("insert_examples" ,
          signature(object="Rcpp_GNGServer"),
          function(object, examples, preset, N){
            warning("This function is copying examples to RAM. If your data is big,
you can use more efficient object$insert_examples function, or you can set pointer without
copying data at all using object$set_memory_move_examples. See documentation for more information
                    ")
            if(hasArg(preset)){
                if(object$get_configuration()$dim != 3){
                    stop("Presets work only for dimensionality 3")
                }
                object$insert_examples(preset(N))
            }
          })


