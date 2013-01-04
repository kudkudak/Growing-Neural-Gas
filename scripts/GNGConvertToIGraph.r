GNGConvertToIGraph<-function(sv){
	sv$pauseServer()
	sv$updateBuffer()
	
	adjlist<-list()
	for(i in (1:(sv$getBufferSize()))){
		node<-sv$getNode(i-1) #indexing
		if(length(node)>3) adjlist[[i]]=node[5:length(node)]+1
		else adjlist[[i]]=c()

lapply(node, function(x) if(x<0) print("ERROR"))
	}
	sv$runServer()
	#adjlist
	graph.adjlist(adjlist)
}
