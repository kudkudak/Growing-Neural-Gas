library(igraph)
.gng.construct_igraph<-function(object){
  tmp_name <- paste("tmp",sample(1:1000, 1),".graphml", sep="")
  if(file.exists(tmp_name))
      file.remove(tmp_name)
  object$export_to_graphml(tmp_name)
  constructed_graph <- .readFromGraphML(tmp_name)
  file.remove(tmp_name)
  constructed_graph
}
#' Reads igraph from GraphML file
.readFromGraphML<-function(filename){
   if(is.null(filename) || length(filename)==0 ){
      warning("Wrong filename returning empty graph")
      graph.empty()
   }
   g = read.graph(filename, format=c("graphml"))
}


