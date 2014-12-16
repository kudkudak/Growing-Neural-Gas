#dev note: I have no idea how to document S4 methods using roxygen, I will have to assign someone to this task

library(igraph)
library(methods)

gng.dataset.bagging.prob <- 3
gng.dataset.bagging <- 2
gng.dataset.sequential <-1

gng.experimental.utility.option.off <- 0
gng.experimental.utility.option.basic <- 1

gng.plot.color.label <- 'label'
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
#' gng.plot.color.label(rounds to integer label if present), gng.plot.color.none(every node is white),
#' 
#' @note If you want to "power-use" plotting and plot for instance a subgraph, you might be interested in
#' exporting igraph with convertToGraph function 
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

#' @title meanError
#' 
#' @description Gets mean error of the graph (note: blocks the execution, O(n))
#' 
#' @usage
#' meanError(gng)
#' 
#' @export
#' 
#' @rdname meanError-methods
#' 
#' @docType methods
#'
#' @examples
#' meanError(gng)
#' 
#' @aliases meanError
#'
meanError.gng <- NULL

#' @title errorStatistics
#' 
#' @description Gets vector with errors for every second of execution
#' 
#' @usage
#' errorStatistics(gng)
#' 
#' @export
#' 
#' @rdname errorStatistics-methods
#' 
#' @docType methods
#'
#' @examples
#' errorStatistics(gng)
#' 
#' @aliases errorStatistics
#'
errorStatistics.gng <- NULL

#' @title Constructor of GrowingNeuralGas object. Can be used to train offline, or online.
#' 
#' @export 
#' 
#' @description Construct GNG object
#' 
#' @param beta coefficient. Decrease the error variables of all node nodes by this fraction. Forgetting rate. Default 0.99
#' 
#' @param alpha Alpha coefficient. Decrease the error variables of the nodes neighboring to the newly inserted node by this fraction. Default 0.5
#' 
#' @param lambda Every lambda iteration is added new vertex. Default 200
#' 
#' @param max.nodes Maximum number of nodes (after reaching this size it will continue running, but won't add new nodes)
#' 
#' @param eps_n Default 0.0006. How strongly adapt neighbour node
#' 
#' @param eps_w Default 0.05. How strongly adapt winning node
#' 
#' @param training Can be either gng.train.offline(max_iter, patience), or gng.train.online()
#' 
#' @param type We have 3 basic types: 
#' 		gng.type.default()
#'		gng.type.utility(k=1.3) - k is constant described for instance in http://sund.de/netze/applets/gng/full/tex/DemoGNG/node20.html
#'		gng.type.optimized(min=0, max=1) - each column should have values in the range [min,max]
#'
#' @examples
#' 
#' library(GrowingNeuralGas)
#' require(c("igraph", "rattle"))
#' data(wine, package="rattle")
#' scaled_wine <- scale(wine[-1])
#' # Train in an offline manner
#' gng <- GNG(scaled_wine, labels=wine$Type, max_nodes=20)
#'
#' # Train in an online manner optimized version
#' gng <- GNG(training = gng.train.online(), type=gng.type.optimized(min=-4, max=4), max_nodes=20)
#' insertExamples(gng, scaled_wine)
#' run(gng)
#' insertExamples(gng, scaled_wine)
#' Sys.sleep(10)
#' pause(gng)
GNG <- NULL

print.gng <- NULL

summary.gng <- NULL

#' @title convertToGraph
#' 
#' @description Converts to igraph (O(n) method, writing intermediately to disk)
#' 
#' @usage
#' convertToGraph(gng)
#' 
#' @export
#' 
#' @rdname convertToGraph-methods
#' 
#' @docType methods
#'
#' @examples
#' convertToGraph(gng)
#' 
#' @aliases convertToGraph
#'
convertToGraph.gng <- NULL


generateExamples <- NULL

#' @title insertExamples
#' 
#' @description Insert (inefficiently) examples to algorithm dataset. For
#' efficient insertion use gng$inser_examples
#' 
#' @usage
#' insertExamples(gng, M, L=c())
#' 
#' @export
#' 
#' @param examples Matrix with examples 
#' 
#' 
#' @rdname insertExamples-methods
#' 
#' @docType methods
#'
#' @examples
#' 
#' #Add preset examples
#' M = generateExamples(preset=gng.preset.sphere)
#' insertExamples(gng, M)
#' @aliases insertExamples
#'
insertExamples.gng <- NULL


loadModule('gng_module', TRUE)

# Lazy loading to allow for discovery of all files
evalqOnLoad({
    
    
  
  GNG <<- function(x=NULL, labels=c(),
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
        insertExamples(server, x, labels)
        run(server)
        
        max_iter = training[2]
        print(max_iter)
        min_relative_dif = training[3]
        iter = 0
        errors_calculated = 0
        while(iter < max_iter || errors_calculated == 0){
          Sys.sleep(0.1)
          iter = server$getCurrentIteration()
          
          if(iter %% (max_iter/100) == 0){    
            print(paste("Iteration", iter))
          }
          
          # Iter 5 = 5 times passed whole dataset. 
          if(iter > 5){
            errors_calculated = 1
            errors = server$getErrorStatistics()
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
 
     setGeneric("convertToGraph", 
                function(object, ...) standardGeneric("convertToGraph"))
     
     setGeneric("run", 
                function(object, ...) standardGeneric("run"))
     
     setGeneric("pause", 
                function(object, ...) standardGeneric("pause"))
     
     setGeneric("terminate", 
                function(object, ...) standardGeneric("terminate"))
     
     
     setGeneric("insertExamples", 
                function(object, ...) standardGeneric("insertExamples"))
     
     setGeneric("meanError", 
                function(object, ...) standardGeneric("meanError"))
     
     setGeneric("centroids", 
                function(object, ...) standardGeneric("centroids"))  

     
     setGeneric("errorStatistics", 
                function(object, ...) standardGeneric("errorStatistics"))
     
     
     setGeneric("numberNodes", 
                function(object, ...) standardGeneric("numberNodes"))
     
  
  plot.gng <<- function(x, vertex.color=gng.plot.color.cluster, layout=gng.plot.layout.v2d, mode){
    
    if(x$getNumberNodes() > 4000){
      warning("Trying to plot very large graph (>4000 nodes). It might take a while.")
    }
    if(x$getNumberNodes() == 0){
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
      .gng.plot2d.errors(x, vertex.color, layout)
    }
  }
  
  print.gng <<- function(x){
    print(sprintf("Growing Neural Gas, nodes %d with mean error %f", 
                  x$getNumberNodes(), x$getMeanError()))
  }
  
  summary.gng <<- function(object){
    print(sprintf("Growing Neural Gas, nodes %d with mean error %f", 
                  object$getNumberNodes(), object$getMeanError()))
    print("Mean errors[s]: ")
    print(object$getErrorStatistics())
  }
  

  setMethod("plot",  "Rcpp_GNGServer", plot.gng)
  setMethod("print",  "Rcpp_GNGServer", print.gng)
  setMethod("summary", "Rcpp_GNGServer", summary.gng)
  
  node.gng <<- function(x, gng_id){
    x$getNode(gng_id)
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
  
  meanError.gng <<- function(object){
    object$getMeanError()
  }  
  
  errorStatistics.gng <<- function(object){
    object$getErrorStatistics()
  }  
  
  save.gng <<- function(object, filename){
    object$save(filename)
  }
  
  load.gng <<- function(filename){
    new(GNGServer, filename)
  }
 
  
  centroids.gng <<- function(object){
    ig <- convertToGraph(object)
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
  setMethod("meanError", "Rcpp_GNGServer", meanError.gng) 
  setMethod("errorStatistics", "Rcpp_GNGServer", errorStatistics.gng) 
  
  #' Get number of nodes
  setMethod("numberNodes" ,
            "Rcpp_GNGServer",
            function(object){
              object$getNumberNodes()
            })
  
  
  
  convertToGraph.gng <- function(object){
    .gng.construct_igraph(object)
  }
  
  
  #' Get node descriptor from graph
  #'
  #' @note This function will dump graph to .graphml file on this first and then will remove
  #' the file. Be cautious with huge graphs!
  #' 
  #' @param gng_id gng id of the node NOTE: nmight differ from one in exported igraph
  setMethod("convertToGraph" ,
            "Rcpp_GNGServer",
            convertToGraph.gng)
  
  
  #' Find closest example
  #' @param x Vector of dimensionality of vertex
  #' @return gng_index of the closest example
  setMethod("predict" ,
            "Rcpp_GNGServer",
            function(object, x){
              object$predict(x)
            })
  
  
  insertExamples.gng <<- function(object, examples, labels=c()){   
	  if(length(labels) == 0){
      	object$insertExamples(examples, vector(mode="numeric", length=0))
	  }else if(typeof(labels) == "character"){
		if(typeof(labels) == "list"){
			if(is.null(examples$labels)){
				gmum.error(ERROR_BAD_PARAMS, "Empty labels column")
			}else{
				label.column <- examples$labels
				examples$labels <- NULL
				object$insertExamples(examples, label.column)
			}
		}else{
	  		gmum.error(ERROR_BAD_PARAMS, "Please pass data frame")
		}
	  }else{
        object$insertExamples(examples, labels)
	  }    
	
  }
  

	generateExamples <<- function(preset, N, r=1.0, center=c(0.5,0.5,0.5)){
		preset(N, center=center, r=r, prob=-1)
	}

  #' Insert examples
  #' 
  #' @note It copies your examples twice in RAM. You might want to use object$insertExamples, or
  #' not to copy at all set_memory_move_examples (when using this function, remember not to modify the matrix
  #' and after removing the object delete it aswell)
  setMethod("insertExamples" ,
            "Rcpp_GNGServer",
            insertExamples.gng)
  
})
