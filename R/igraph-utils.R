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
   g = graph.empty()
   tryCatch({
   		g = read.graph(filename, format=c("graphml"))
	    }
     , 
       error= 
       
       function(error){
  	  warning(error$message)
	  warning("Please install igraph with support for GraphML! You are probably on Linux. This package won't plot graphs without
	     graphml support in igraph")	   
	   
       }
   )
   g
}


