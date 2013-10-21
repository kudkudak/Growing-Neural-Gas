GNGConvertToIGraphAnyDim<-function(sv, dim){
	sv$pauseServer()
	sv$updateBuffer()
	
	adjlist<-list()
        if(sv$getBufferSize() > 0) {
          for(i in (1:(sv$getBufferSize()))){
            node<-sv$getNode(i-1) #indexing
            if(length(node) > (dim+1)) adjlist[[i]] <- node[(dim+2):length(node)] + 1
            else adjlist[[i]]=c()

            lapply(node, function(x) if(x<0) print("ERROR"))
          }
        }
	sv$runServer()
	#adjlist
	graph.adjlist(adjlist)
}

GNGConvertToIGraph<-function(sv){
	sv$pauseServer()
	sv$updateBuffer()
	count_edges<-0
	for(i in (1:(sv$getBufferSize()))){
		count_edges<-count_edges+length(sv$getNode(i-1))-6
	}
	edglist<-matrix(0,10*count_edges,2)
	weightlist<-matrix(0,10*count_edges)
	x_pos<-matrix(0,sv$getBufferSize(),1)
	y_pos<-matrix(0,sv$getBufferSize(),1)
	z_pos<-matrix(0,sv$getBufferSize(),1)
	density<-matrix(0,sv$getBufferSize(),1)
	k<-0
	for(i in (1:(sv$getBufferSize()))){
		
		node<-sv$getNodeWithParam(i-1) #indexing
		if(node[1]==0) print("ERRRRRRRRRRRROR")		
		x_pos[i,1]<-node[2]
		y_pos[i,1]<-node[3]
		z_pos[i,1]<-node[4]
		density[i,1]<-node[5]
		if(length(node)>6){
			for(j in  (6: (length(node)-1)  )     ){ #-1 bo error jes
				if(node[j]==-1) {
					print(node[j])
					print(i)				
				}				
				k<-k+1
				edglist[k,1:2]=c(i,node[j]+1)
				weightlist[k]<-sqrt(sum((node[2:4]-sv$getNode(node[j])[2:4])^2)) #jjjj
			}
		}
	}	
	g<-graph.edgelist(edglist[1:k,])
	E(g)$weight = weightlist[1:k]
	V(g)$x=x_pos
	V(g)$y=y_pos
	V(g)$z=z_pos
	V(g)$density=density
	g
}

GNGConvertToSpGraph<-function(sv){
	sv$pauseServer()
	sv$updateBuffer()
	
	adjlist<-list()
	for(i in (1:(sv$getBufferSize()))){
		node<-sv$getNode(i-1) #indexing
		if(node[1]==1){
			adjlist[[i]]=node[2:length(node)]+1
		}else{
			adjlist[[i]]=c(0,0,0)	
		}
	}
	sv$runServer()
	adjlist
}
