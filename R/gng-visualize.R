library(igraph)

  .gng.plot3d<-function(gngServer){
	if("rgl" %in% rownames(installed.packages()) == TRUE){
	    g <- convertToGraph(gngServer)
	    .visualizeIGraphRGL(g)
	}else{
	    warning("Please install rgl package to plot 3d graphs")
	}
  }
  #' Draw igraph using rgl - assumes >=3 dimensions and draws 3 first
  .visualizeIGraphRGL<-function(g, radius=NULL){
    if("rgl" %in% rownames(installed.packages()) == TRUE){
	    library(rgl)
	    library(igraph)
	    
	    if(length(V(g))==0) return
	    
	    iteration<-0
	    nodes <- length(V(g))
	    
	    # Init 3d data
	    x_lines <- c(1:2*length(E(g)))
	    y_lines <- c(1:2*length(E(g)))
	    z_lines <- c(1:2*length(E(g)))
	    k<-1
	    m<-1
	    x<-c(1:nodes)
	    y<-c(1:nodes)
	    z<-c(1:nodes)
	    
	    # Write 3d positions
	    for(i in 1:nodes){
	      x[i]=V(g)[i]$v0
	      y[i]=V(g)[i]$v1
	      z[i]=V(g)[i]$v2
	    }
	    
	    # TODO: edges might be huge..
	    for(edg_idx in 1:length(E(g)))
	    {
	      edg <- get.edges(g, E(g)[edg_idx])
	      x_lines[k] = V(g)[edg[1]]$v0
	      y_lines[k] = V(g)[edg[1]]$v1
	      z_lines[k] = V(g)[edg[1]]$v2
	      
	      x_lines[k+1] = V(g)[edg[2]]$v0
	      y_lines[k+1] = V(g)[edg[2]]$v1
	      z_lines[k+1] = V(g)[edg[2]]$v2
	      k = k + 2
	    }  
	    
	    if(is.null(radius)){
	      radius <- 8.0*(0.3333* (abs(max(x) - min(x))+abs(max(y) - min(y))+abs(max(z) - min(z)))/(nodes+0.0))
	    }
	    
	    cx <- V(g)$error
	    cx <- abs(cx)/max(abs(cx)) 
	    cy <- c(1:(nodes))
	    cz <- c(1:(nodes))
	    
	    cy <- 0.1
	    cz <- 0.1
	    print(cx)
	    
	    
	    ### Draw graph ###
	    rgl.clear()
	    rgl.light()
	    rgl.bg(color="white")
	    axes3d(edges="bbox")
	    
	    
	    rgl.spheres(x,y,z, 
			radius = rep(radius, length(cx)), 
			col=rgb(cx,cy, cz))
	    
	    rgl.lines(x_lines[1:k-1],y_lines[1:k-1],z_lines[1:k-1],color="bisque")
	}else{
    		warning("Please install rgl package to plot 3d graph")
    	}
  }
.gng.plot2d.errors<-function(gngServer, vertex.color, layout){
  ig <- convertToGraph(gngServer)
  
  if(length(V(ig))==0) return
  
  if(vertex.color == 'label'){
    vertex.color = c(1:length(V(ig)))
    max_col = 0
    for(label in V(ig)$label)
        max_col = max(max_col, round(label))
    cols = rainbow(max_col+1)
    vertex.color = cols[as.double(lapply(V(ig)$label, round))]
  }
  
  .visualizeIGraph2dWithErrors(ig, vertex.color, layout, gngServer)
}

.gng.plot2d<-function(gngServer, vertex.color, layout){
  tmp_name <- paste("tmp",sample(1:1000, 1),".graphml", sep="")

  ig <- convertToGraph(gngServer)
  
  if(length(V(ig))==0) return

  if(vertex.color == 'label'){
    vertex.color = c(1:length(V(ig)))
    max_col = 0
    for(label in V(ig)$label)
      max_col = max(max_col, round(label))
    cols = rainbow(max_col+1)
    vertex.color = cols[as.double(lapply(V(ig)$label, round))]
  }
  
  .visualizeIGraph2d(ig, vertex.color, layout)
}

#' Visualize igraph using igraph plot
#' It will layout graph using v0 and v1 coordinates
#' @note It is quite slow, works for graphs < 2000 nodes, and for graphs <400 when using layout
.visualizeIGraph2d<-function(g, vertex.color, layout){
  L<-layout(g)
  
  if(vertex.color == 'cluster'){   
    communities <- infomap.community(g)
    communities
    col <- rainbow(length(communities))
    vertex.color <- col[membership(communities)]
  }
  if(vertex.color == 'fast_cluster'){
    l = fastgreedy.community(g)#as.undirected(g))
    col <- rainbow(length(l))
    print(membership(l))
    vertex.color <- col[membership(l)]
  }
  else if(vertex.color == 'none'){
    vertex.color = NA
  }else{
    # Passed something else as vector
  }
    
  plot.igraph(g,vertex.size=3.0,vertex.label=NA,vertex.color=vertex.color,layout=L)
}

.visualizeIGraph2dWithErrors<-function(ig, vertex.color, layout_2d, gng){
  plot.new()
  par(mfrow=c(1,2))
  .visualizeIGraph2d(ig, vertex.color, layout_2d)
  title("Graph visualization")
  errors_raw = gng$getErrorStatistics()
  errors_raw = errors_raw[5:length(errors_raw)]
  errors = errors_raw
  #errors = log((errors_raw)/min(errors_raw+1e-4))
  plot(errors, type="l", lty=2, lwd=2, xlab="Batch", ylab="Mean batch error", frame.plot=F)
  title("Mean error (log)")
}
