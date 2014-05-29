#dev note: I have no idea how to document S4 methods using roxygen, I will have to assign someone to this task

library(igraph)

gng.dataset.bagging.prob <- 3
gng.dataset.bagging <- 2
gng.dataset.sequential <-1

gng.plot.2d <- 1
gng.plot.rgl3d <- 2
gng.plot.2d.errors <- 3

loadModule('gng_module', TRUE)



#' Plot GNG
#'
#' @param mode = gng.plot.rgl3d  (rgl plot, requires rgl library) or gng.plot.2d (igraph plot) or
#' gng.plot.2d.errors (igraph plot with mean error log plot)
#' @param layout_2d = if TRUE it will draw vertex at position x,y. if FALSE it will adapt vertex position using
#' layout from igraph (waring: will take noticeably longer for bigger graphs)
#' @param cluster = if TRUE it will color vertexes according to cluster found by fastgreedy.community algorithm
#' from igraph package
#' 
#' @note if you want to "power-use" plotting and plot for instance a subgraph, you might be interested in
#' exporting igraph with convert_igraph function and plotting it using/reusing function from this package:
#' .visualizeIGraph2d
plot.gng <- NULL


#' Get node descriptor from graph
#'
#' @param gng_id gng id of the node NOTE: might differ from one in exported igraph
node.gng <- NULL


#' Run the algorithm (in parallel)
run.gng <- NULL

#' Pause the algorithm (in parallel)
pause.gng <- NULL

#' Terminate the algorithm (in parallel)
terminate.gng <- NULL

#' Mean node error
mean_error.gng <- NULL

#' Error statistics
error_statistics.gng <- NULL

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
  
  
  if(dim == -1){
     stop("Please pass vertex dimensionality (dim argument)")
  }
  
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
    config$set_uniform_grid_origin(uniformgrid_boundingbox_origin)
  }
  
  if(config$uniformgrid_optimization &&
       (length(uniformgrid_boundingbox_sides)!=length(uniformgrid_boundingbox_origin)
        || length(uniformgrid_boundingbox_origin) != dim)){
    
    stop("Make sure that dimensions of bounding box and vertex position match")
  }
  
  if(!config$check_correctness()){
    stop("Passed incorrect parameters.")
  }
  
  # Construct server
  server = new(GNGServer, config)
  server$set_debug_level(10)
  server
}


print.gng <- NULL

summary.gng <- NULL


# Lazy loading to allow for discovery of all files
evalqOnLoad({


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

  if (!isGeneric("mean_error"))
    setGeneric("mean_error", 
               function(object, ...) standardGeneric("mean_error"))

  if (!isGeneric("error_statistics"))
    setGeneric("error_statistics", 
               function(object, ...) standardGeneric("error_statistics"))
  
  
  if (!isGeneric("number_nodes"))
    setGeneric("number_nodes", 
               function(object, ...) standardGeneric("number_nodes"))
  

  plot.gng <<- function(x, cluster=TRUE, layout_2d=TRUE, mode){
    
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
      .gng.plot2d(x, cluster, layout_2d)
    }
    else if(mode == gng.plot.2d.errors){
      .gng.plot2d.errors(x, cluster, layout_2d)
    }
  }
  
  print.gng <<- function(x){
    print(sprintf("Growing Neural Gas, nodes %d with mean error %f", 
                  x$get_number_nodes(), x$get_mean_error()))
  }
  
  summary.gng <<- function(object){
    print(sprintf("Growing Neural Gas, nodes %d with mean error %f", 
                  object$get_number_nodes(), object$get_mean_error()))
    print("Mean errors[s]: ")
    print(object$get_error_statistics())
  }
  
  
  setMethod("plot",  signature(x="Rcpp_GNGServer"), plot.gng)
  setMethod("print",  signature(x="Rcpp_GNGServer"), print.gng)
  setMethod("summary", signature(object="Rcpp_GNGServer"), summary.gng)
  
  node.gng <<- function(x, gng_id){
    x$get_node(gng_id)
  }
  
  run.gng <<- function(object){
    object$run()
  }

  pause.gng <<- function(object){
    object$pause()
  }
  
  terminate.gng <<- function(object){
    object$terminate()
  }

  mean_error.gng <<- function(object){
    object$get_mean_error()
  }  

  error_statistics.gng <<- function(object){
    object$get_error_statistics()
  }  
  
  setMethod("node", signature(x="Rcpp_GNGServer", gng_id="numeric"), node.gng)
  setMethod("run", signature(object="Rcpp_GNGServer"), run.gng)
  setMethod("pause", signature(object="Rcpp_GNGServer"), pause.gng)
  setMethod("terminate", signature(object="Rcpp_GNGServer"), terminate.gng)
  setMethod("mean_error", signature(object="Rcpp_GNGServer"), mean_error.gng) 
  setMethod("error_statistics", signature(object="Rcpp_GNGServer"), error_statistics.gng) 
  
  #' Get number of nodes
  setMethod("number_nodes" ,
            signature(object="Rcpp_GNGServer"),
            function(object){
              object$get_number_nodes()
            })
  
  
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
  #' 
  #' @note It copies your examples twice in RAM. You might want to use object$insert_examples, or
  #' not to copy at all set_memory_move_examples (when using this function, remember not to modify the matrix
  #' and after removing the object delete it aswell)
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

})


