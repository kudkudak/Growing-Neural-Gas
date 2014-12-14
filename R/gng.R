#dev note: I have no idea how to document S4 methods using roxygen, I will have to assign someone to this task

library(igraph)
library(methods)

gng.dataset.bagging.prob <- 3
gng.dataset.bagging <- 2
gng.dataset.sequential <-1

gng.experimental.utility.option.off <- 0
gng.experimental.utility.option.basic <- 1

gng.plot.color.extra <- 'extra'
gng.plot.color.fast_cluster <- 'fast_cluster'
gng.plot.color.cluster <- 'cluster'
gng.plot.color.none <- 'none'

gng.plot.layout.v2d <- function(g){
  cbind(V(g)$v0, V(g)$v1)
}
gng.plot.layout.igraph.fruchterman <- function(g){
  layout.fruchterman.reingold(g, niter=10000, area=4*vcount(g)^2)
}
gng.plot.layout.igraph.fruchterman.fast <- layout.fruchterman.reingold
gng.plot.layout.igraph.auto <- layout.auto

gng.plot.2d <- 1
gng.plot.rgl3d <- 2
gng.plot.2d.errors <- 3


.gng.type.optimized = 0
.gng.type.utility = 1
.gng.type.default = 2
.gng.train.online = 1
.gng.train.offline = 0

gng.type.default <- c(.gng.type.default)

gng.type.optimized <- function(minimum=0, maximum=10){
  c(.gng.type.optimized, minimum, maximum)
}

gng.type.utility<- function(k=1.3){
  c(.gng.type.utility, k)
}

gng.train.online <- function(dim){
  c(.gng.train.online,  dim)
}



gng.train.offline <- function(max_iter = 100, min_relative_dif = 1e-2){
  if(max_iter<7){
    gmum.error(ERROR, "Please pass at least 7 iterations")
  }
  c(.gng.train.offline, max_iter, min_relative_dif)
}





#' Plot GNG
#'
#' @title plot
#' 
#' @description Plot resulting graph using igraph plotting, or using rgl 3d engine.
#' 
#' @usage
#' plot(gng)
#' 
#' @export
#' 
#' @rdname plot-methods
#' 
#' @docType methods
#'
#' @param mode gng.plot.rgl3d (3d plot), gng.plot.2d (igraph plot) or
#' gng.plot.2d.errors (igraph plot with mean error log plot)
#' 
#' @param layout layout to be used when plotting. Possible values: gng.plot.layour.igraph.v2d (first two dimensions),
#' gng.plot.layout.igraph.auto (auto layout from igraph) gng.plot.layout.igraph.fruchterman.fast (fast fruchterman reingold layout),or any function accepting igraph graph and returning layout
#' 
#' @param vertex.color how to color vertexes. Possible values: gng.plot.color.cluster(vertex color is set to fastgreedy.community clustering),
#' gng.plot.color.extra(rounds to integer extra dim if present), gng.plot.color.none(every node is white),
#' 
#' @note If you want to "power-use" plotting and plot for instance a subgraph, you might be interested in
#' exporting igraph with convert_igraph function and plotting it using/reusing function from this package:
#' .visualizeIGraph2d
#' 
#' @examples
#' 
#' # Plots igraph using first 2 coordinates and colors according to clusters
#' plot(gng, mode=gng.plot.2d.errors, layout=gng.plot.layout.v2d, vertex.color=gng.plot.color.cluster)
#' 
#' # Plot rgl (make sure you have installed rgl library)
#' plot(gng, mode=gng.plot.rgl, layout=gng.plot.layout.v2d, vertex.color=gng.plot.color.cluster)
#' 
#' # For more possibilities see gng.plot.* constants
#' 
plot.gng <- NULL

#' Save model to binary format
#'
#' @title save.gng
#' 
#' @description Writes model to a disk space efficient binary format. 
#' 
#' @usage
#' save.gng(gng)
#' 
#' @export
#' 
#' @param
#' filename Dump destination
#' 
#' @rdname save.gng-methods
#' 
#' @docType methods
#'
#' @examples
#' save.gng(gng, 'graph.bin')
#' 
#' @aliases save.gng
#'
save.gng <- NULL



#' Load model from binary format
#'
#' @title load.gng
#' 
#' @description Writes model to a disk space efficient binary format. 
#' 
#' @usage
#' load.gng(gng)
#' 
#' @export
#' 
#' @param
#' filename Dump location
#' 
#' @rdname load.gng-methods
#' 
#' @docType methods
#'
#' @examples
#' load.gng('model.bin')
#' 
#' @aliases load.gng
#'
load.gng <- NULL

#' Get centroids
#'
#' @title centroids
#' 
#' @description Using infomap.communities finds communities and for each community pick node with biggest betweenness score
#' 
#' @usage
#' centroids(gng)
#' 
#' @export
#' 
#' @rdname centroids-methods
#' 
#' @docType methods
#'
#' @examples
#' # Print position of the first centroid
#' print(node(gng, centroids(gng)[1])$pos)
#' 
#' @aliases centroids
#'  
centroids.gng <- NULL

#' Get GNG node
#'
#' @title node
#' 
#' @description Retrieves node from resulting graph
#' 
#' @usage
#' node(gng, 10)
#' 
#' @export
#' 
#' @rdname node-methods
#' 
#' @docType methods
#'
#' @param gng_id Id of the node to retrieve. This is the id returned by functions like predict, or centroids
#' 
#' @examples
#' print(node(gng, 10)$pos)
#' 
#' @aliases node
#' 
node.gng <- NULL


#' @title run
#' 
#' @description Run algorithm (in parallel)
#' 
#' @usage
#' run(gng)
#' 
#' @export
#' 
#' 
#' @rdname run-methods
#' 
#' @docType methods
#'
#' @examples
#' run(gng)
#' 
#' @aliases run
#'
run.gng <- NULL

#' @title pause
#' 
#' @description Pause algorithm
#' 
#' @usage
#' pause(gng)
#' 
#' @export
#' 
#' 
#' @rdname pause-methods
#' 
#' @docType methods
#'
#' @examples
#' pause(gng)
#' 
#' @aliases pause
#'
pause.gng <- NULL

#' @title pause
#' 
#' @description Terminate algorithm
#' 
#' @usage
#' terminate(gng)
#' 
#' @export
#' 
#' 
#' @rdname terminate-methods
#' 
#' @docType methods
#'
#' @examples
#' terminate(gng)
#' 
#' @aliases terminate
#'
terminate.gng <- NULL

#' @title mean_error
#' 
#' @description Gets mean error of the graph (note: blocks the execution, O(n))
#' 
#' @usage
#' mean_error(gng)
#' 
#' @export
#' 
#' @rdname mean_error-methods
#' 
#' @docType methods
#'
#' @examples
#' mean_error(gng)
#' 
#' @aliases mean_error
#'
mean_error.gng <- NULL

#' @title error_statistics
#' 
#' @description Gets vector with errors for every second of execution
#' 
#' @usage
#' error_statistics(gng)
#' 
#' @export
#' 
#' @rdname error_statistics-methods
#' 
#' @docType methods
#'
#' @examples
#' error_statistics(gng)
#' 
#' @aliases error_statistics
#'
error_statistics.gng <- NULL

#' @title Constructor of GrowingNeuralGas object
#' 
#' @export 
#' 
#' @description Construct GNG object
#' 
#' @param beta coefficient. Decrease the error variables of all node nodes by this fraction. Forgetting rate. Default 0.99
#' 
#' @param alpha Alpha coefficient. Decrease the error variables of the nodes neighboring to the newly inserted node by this fraction. Default 0.5
#' 
#' @param uniformgrid.optimization TRUE/FALSE. You cannot use utility option with this, so please pass FALSE here then.
#' 
#' @param lazyheap.optimization TRUE/FALSE. You cannot use utility option with this, so please pass FALSE here then.
#' 
#' @param lambda Every lambda iteration is added new vertex. Default 200
#' 
#' @param max.nodes Maximum number of nodes (after reaching this size it will continue running, but won't add new nodes)
#' 
#' @param eps_n Default 0.0006. How strongly adapt neighbour node
#' 
#' @param eps_w Default 0.05. How strongly adapt winning node
#' 
#' @param dataset.type Dataset type. Possibilities gng.dataset.bagging, gng.dataset.bagging.prob, gng.dataset.sequential
#' 
#' @param experimental_utility_option EXPERIMENTAL Utility option gng.experimental.utility.option.off / gng.experimental.utility.option.basic
#'
#' @param experimental_utility_k EXPERIMENTAL Utility option constant
#' 
#' @param load_model_filename Set to path to file from which load serialized model
#'
#' @param experimental_vertex_extra_data if TRUE each example should have additional coordinate, that will be
#' voted in graph. Each node (that you can get using node function) will have extra_data field that will be
#' equal to mean of samples around given node. If used with probability dataset example layout is 
#' <vertex position> <vertex_extra_data> <sampling_probability>, for example c(0.3, 0.6, 0.7, 100, 0.7)
#' 
#'
#' @examples
#' 
#' # Default GNG instance, without optimitzations and vertex dimensionality 3
#' 
#' gng <- GNG(dataset_type=gng.dataset.bagging.prob, max_nodes=max_nodes, dim=3)
#' 
#' # Construct GNG loaded from file with uniform grid
#' 
#' gng <- GNG(dataset_type=gng.dataset.bagging.prob, max_nodes=max_nodes, dim=3,
#' uniformgrid_optimization=TRUE,  lazyheap_optimization=FALSE,
#' uniformgrid_boundingbox_sides=c(3,3,3), uniformgrid_boundingbox_origin=c(-0.5,-0.5,-0.5), 
#' load_model_filename="sphere_simple.bin")
GNG <- NULL

print.gng <- NULL

summary.gng <- NULL

#' @title convert_igraph
#' 
#' @description Converts to igraph (O(n) method, writing intermediately to disk)
#' 
#' @usage
#' convert_igraph(gng)
#' 
#' @export
#' 
#' @rdname convert_igraph-methods
#' 
#' @docType methods
#'
#' @examples
#' convert_igraph(gng)
#' 
#' @aliases convert_igraph
#'
convert_igraph.gng <- NULL



#' @title insert_examples
#' 
#' @description Insert (inefficiently) examples to algorithm dataset. For
#' efficient insertion use gng$inser_examples, and for setting memory pointer
#' use gng$set_memory_move_examples
#' 
#' @usage
#' insert_examples(gng, M)
#' 
#' @export
#' 
#' @param examples Matrix with examples of dimensionality N rows x C columns, where
#' C columns = dim (passed as parameter to GNG object) + 1 or 0 (1 if vertex_extra_data is TRUE)
#' + 1 or 0 (1 if dataset_type=gng.dataset.bagging.prob). 
#' 
#' @param preset Use only if you are adding exemplary dataset. Possibilities: gng.preset.sphere,
#' gng.preset.box. You can specify preset params: N=1000, center=c(0.5,0.5,0.5), prob=-1. 
#' 
#' @note Complicated memory layout of the matrix is due to need for memory efficiency.
#' In the future versions you can expect wrapper simplifying addition and also
#' streaming from disk file
#' 
#' @rdname insert_examples-methods
#' 
#' @docType methods
#'
#' @examples
#' 
#' #Add preset examples with probability of being sampled (this assumed GNG was created with gng.dataset.bagging.prob dataset)
#' insert_examples(gng, preset=gng.preset.sphere)
#' 
#' #Insert efficiently examples
#' M <- matrix(0, ncol=3, nrow=10)
#' M[1,] = c(4,5,6)
#' gng$insert_examples(M)
#' 
#' #Set memory of the algorithm to point in memory. Note: you cannot remove this matrix while
#' in execution or you will experience memory error
#' gng$set_memory_move_examples(M)
#' 
#' @aliases insert_examples
#'
insert_examples.gng <- NULL


loadModule('gng_module', TRUE)

# Lazy loading to allow for discovery of all files
evalqOnLoad({
    
    
  
  GNG <<- function(x=NULL, 
                   beta=0.99, 
                   alpha=0.5, 
                   max_nodes=1000, 
                   eps_n=0.0006, 
                   eps_w= 0.05, 
                   max_edge_age = 200, 
                   type = gng.type.default,
                   training = gng.train.offline(),
                   lambda=200,
                   verbosity=0
                   
  ){
    
    config <- new(GNGConfiguration)
    
    # Fill in configuration
    if(training[1] == .gng.train.offline){
      config$dim = ncol(x)
    }else{
	  
       config$dim = training[2]
		print(config$dim)    
	}

    
    if(type[1] == .gng.type.optimized){
      config$uniformgrid_optimization = TRUE
      config$lazyheap_optimization = TRUE  
      config$setBoundingBox(type[2], type[3])
      
      if(training[1] == .gng.train.offline){
        if(!max(df) <= type[3] && !min(df) >= type[2]){
          gmum.error("Passed incorrect parameters. The dataset is not in the defined range")
        }
      }
      
    }else{
      config$uniformgrid_optimization = FALSE
      config$lazyheap_optimization = FALSE
    }
    
    if(type[1] == .gng.type.utility){
      config$experimental_utility_k = type[2]
      config$experimental_utility_option = 1
    }
    else{
      config$experimental_utility_option = 0
    }
    
    config$dataset_type=gng.dataset.sequential
    config$beta = beta
    config$max_edge_age = max_edge_age
    config$alpha = alpha  
    config$max_nodes = max_nodes
    config$eps_n = eps_n
    config$eps_w = eps_w
    
    config$lambda = lambda
    config$verbosity = verbosity
    
    if(!config$check_correctness()){
      gmum.error(ERROR_BAD_PARAMS, "Passed incorrect parameters.")
    }
    
    # Construct server
    server = new(GNGServer, config)
    
    # Perform training on passed dataset
    if(training[1] == .gng.train.offline){
      print("Training offline")
      if(is.null(x)){
        gmum.error(ERROR, "Passed null data and requested training offline")
      }else{
        insert_examples(server, x)
        run(server)
        
        max_iter = training[2]
        print(max_iter)
        min_relative_dif = training[3]
        iter = 0
        errors_calculated = 0
        while(iter < max_iter || errors_calculated == 0){
          Sys.sleep(0.1)
          iter = server$get_current_iteration()
          
          if(iter %% (max_iter/100) == 0){    
            print(paste("Iteration", iter))
          }
          
          # Iter 5 = 5 times passed whole dataset. 
          if(iter > 5){
            errors_calculated = 1
            errors = server$get_error_statistics()
            best_previously = min(errors[(length(errors)-5):length(errors)-1])
            current = errors[length(errors)]
            if(best_previously != 0){
              change = 1.0 - current/best_previously
              if(change < min_relative_dif){
                break
              }
            }
          }
        }
        
        terminate(server)
        
      }
    }
    
    
    
    server
  }
    

     setGeneric("node", 
                function(x, gng_id, ...) standardGeneric("node"))
 
     setGeneric("convert_igraph", 
                function(object, ...) standardGeneric("convert_igraph"))
     
     setGeneric("run", 
                function(object, ...) standardGeneric("run"))
     
     setGeneric("pause", 
                function(object, ...) standardGeneric("pause"))
     
     setGeneric("terminate", 
                function(object, ...) standardGeneric("terminate"))
     
     
     setGeneric("insert_examples", 
                function(object, ...) standardGeneric("insert_examples"))
     
     setGeneric("mean_error", 
                function(object, ...) standardGeneric("mean_error"))
     
     setGeneric("centroids", 
                function(object, ...) standardGeneric("centroids"))  

     
     setGeneric("error_statistics", 
                function(object, ...) standardGeneric("error_statistics"))
     
     
     setGeneric("number_nodes", 
                function(object, ...) standardGeneric("number_nodes"))
     
  
  plot.gng <<- function(x, vertex.color=gng.plot.color.cluster, layout=gng.plot.layout.v2d, start_s=2, mode){
    
    if(x$get_number_nodes() > 4000){
      warning("Trying to plot very large graph (>4000 nodes). It might take a while.")
    }
    if(x$get_number_nodes() == 0){
      warning("Empty graph")
      return
    }
    
    if(mode == gng.plot.rgl3d && !("rgl" %in% rownames(installed.packages()))){
      warning("Please install rgl and reload the package to plot 3d")
      return
    }
    
    if(mode == gng.plot.rgl3d){
      .gng.plot3d(x)
    }
    else if(mode == gng.plot.2d){
      .gng.plot2d(x, vertex.color, layout)
    }
    else if(mode == gng.plot.2d.errors){
      .gng.plot2d.errors(x, vertex.color, layout, start_s)
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
  

  setMethod("plot",  "Rcpp_GNGServer", plot.gng)
  setMethod("print",  "Rcpp_GNGServer", print.gng)
  setMethod("summary", "Rcpp_GNGServer", summary.gng)
  
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
  
  save.gng <<- function(object, filename){
    object$save(filename)
  }
  
  load.gng <<- function(filename){
    new(GNGServer, filename)
  }
 
  
  centroids.gng <<- function(object){
    ig <- convert_igraph(object)
    communities <- spinglass.community(ig)
    centroids <- c()
    for(i in 1:length(communities)){
      ig_test <- induced.subgraph(ig, which(membership(communities)==i))
      centroids<- c(centroids, (order(betweenness(ig_test))[1]))
    }
    centroids
  }
  setMethod("centroids", signature("Rcpp_GNGServer"), centroids.gng)
  
  setMethod("node", signature("Rcpp_GNGServer","numeric"), node.gng)
  setMethod("run", "Rcpp_GNGServer", run.gng)
  setMethod("pause", "Rcpp_GNGServer", pause.gng)
  setMethod("terminate", "Rcpp_GNGServer", terminate.gng)
  setMethod("mean_error", "Rcpp_GNGServer", mean_error.gng) 
  setMethod("error_statistics", "Rcpp_GNGServer", error_statistics.gng) 
  
  #' Get number of nodes
  setMethod("number_nodes" ,
            "Rcpp_GNGServer",
            function(object){
              object$get_number_nodes()
            })
  
  
  
  convert_igraph.gng <- function(object){
    .gng.construct_igraph(object)
  }
  
  
  #' Get node descriptor from graph
  #'
  #' @note This function will dump graph to .graphml file on this first and then will remove
  #' the file. Be cautious with huge graphs!
  #' 
  #' @param gng_id gng id of the node NOTE: nmight differ from one in exported igraph
  setMethod("convert_igraph" ,
            "Rcpp_GNGServer",
            convert_igraph.gng)
  
  
  #' Find closest example
  #' @param x Vector of dimensionality of vertex
  #' @return gng_index of the closest example
  setMethod("predict" ,
            "Rcpp_GNGServer",
            function(object, x){
              object$predict(x)
            })
  
  
  insert_examples.gng <<- function(object, examples, preset, N, r=1.0, center=c(0.5,0.5,0.5), prob=-1){
    warning("This function is copying examples to RAM. If your data is big,
  you can use more efficient object$insert_examples function, or you can set pointer without
            copying data at all using object$set_memory_move_examples. See documentation for more information
            ")
    if(hasArg(preset)){
      if(object$get_configuration()$dim != 3){
        stop("Presets work only for dimensionality 3")
      }
      object$insert_examples(preset(N, center=center, r=r, prob=prob))
    }else{ 
      object$insert_examples(examples)
    }
  }
  
  #' Insert examples
  #' 
  #' @note It copies your examples twice in RAM. You might want to use object$insert_examples, or
  #' not to copy at all set_memory_move_examples (when using this function, remember not to modify the matrix
  #' and after removing the object delete it aswell)
  setMethod("insert_examples" ,
            "Rcpp_GNGServer",
            insert_examples.gng)
  
})
