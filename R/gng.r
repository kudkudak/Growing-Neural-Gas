# Documenting S4: https://github.com/variani/pckdev/wiki/Documenting-with-roxygen2

gng.dataset.bagging.prob <- 3
gng.dataset.bagging <- 2
gng.dataset.sequential <-1

gng.plot.2d <- 1
gng.plot.rgl3d <- 2


# Load dynamic library
x = library("GrowingNeuralGas")
mo<-Module("gng_module", PACKAGE=x)
g = mo$GNGConfiguration
s = mo$GNGServer

gngConfig = new(g)
gngServer = new(s, gngConfig)

#' Class GrowingNeuralGas
#' 
#' @name GrowingNeuralGas
#' @rdname GrowingNeuralGas
#' @exportClass GrowingNeuralGas
#' 
setClass("GrowingNeuralGas",representation(server="Rcpp_GNGServer"))



#' Constructor of GrowingNeuralGas object
#' 
#' @param beta coefficient. Decrease the error variables of all node nodes by this fraction. Forgetting rate. Default 0.99
#' @param alpha Alpha coefficient. Decrease the error variables of the nodes neighboring to the newly inserted node by this fraction. Default 0.5
#' @param uniformgrid.optimization TRUE/FALSE. You cannot use utility option with this, so please pass FALSE here then.
#' @param lazyheap.optimization TRUE/FALSE. You cannot use utility option with this, so please pass FALSE here then.
#' @param max.nodes Maximum number of nodes (after reaching this size it will continue running, but won't add new nodes)
#' @param eps_n Default 0.05
#' @param eps_v Default 0.0006
#' @param dim Vertex position dimensionality.
#' @param dataset.type Dataset type. Possibilities gng.dataset.bagging, gng.dataset.bagging.prob, gng.dataset.sequential
#' 
#' @name GrowingNeuralGas_initialize
setMethod("initialize", "GrowingNeuralGas",
          function(.Object, dim=-1, dataset.type=gng.dataset.gng.dataset.sequential, beta=0.99, 
                      alpha=0.5, uniformgrid.optimization=TRUE, 
                      lazyheap.optimization=TRUE, max.nodes=1000, eps_n=0.05, 
                      eps_v = 0.0006){
  
})


#' Plot GNG
#' 
#' @param mode = gng.plot.rgl3d  (rgl plot, requires rgl library) or gng.plot.2d (igraph plot)
setMethod("plot", "GrowingNeuralGas", function(.Object, mode){
    if(.Object@server$get_number_nodes() > 4000){
        warning("Trying to plot very large graph (>4000 nodes). It might take a while.")
    }
    
    if(mode == gng.plot.rgl && !("rgl" %in% rownames(installed.packages()))){
         stop("Please install rgl and reload the package to plot 3d")
    }
    
    if(mode == gng.plot.rgl3d){
      .gng.plot3d(.Object@server)
    }
    else if(mode == gng.plot.2d){
      .gng.plot2d(.Object@server)
    }
})

gng <- new("GrowingNeuralGas", server=gngServer)



gng$server
gng$increaseServer()
