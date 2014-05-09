library(igraph)

#' Reads igraph from GraphML file
readFromGraphML<-function(filename){
   if(is.null(filename) || length(filename)==0 ){
      warning("Wrong filename returning empty graph")
      graph.empty()
   }
   g = read.graph(filename, format=c("graphml"))
}

