library(igraph)

gng.plot3d<-function(gngServer){
  tmp_name <- paste("tmp",sample(1:1000, 1),".graphml", sep="")
  gngServer$export_to_graphml(tmp_name)
  print("Reading GraphML dumped")
  .visualizeIGraphRGL(.readFromGraphML(tmp_name))
}
gng.plot2d<-function(gngServer){
  print("plot2d")
  tmp_name <- paste("tmp",sample(1:1000, 1),".graphml", sep="")
  gngServer$export_to_graphml(tmp_name)
  print("Reading GraphML dumped")
  .visualizeIGraph2d(.readFromGraphML(tmp_name))
}

#' Draw igraph using rgl - assumes >=3 dimensions and draws 3 first
.visualizeIGraphRGL<-function(g, radius=NULL){
  library(multicore)
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
        radius <- 5.0*(0.3333* (abs(max(x) - min(x))+abs(max(y) - min(y))+abs(max(z) - min(z)))/(nodes+0.0))
    }

    ### Draw graph ###
    rgl.clear()
    rgl.light()
    rgl.bg(color="black")
    axes3d(edges="bbox")
    rgl.spheres(x,y,z, radius = radius) #,radius=cx*error_scale, color=rgb(cx,cy,cz))

    rgl.lines(x_lines[1:k-1],y_lines[1:k-1],z_lines[1:k-1],color="bisque")
}


#' Visualize igraph using igraph plot
#' It will layout graph using v0 and v1 coordinates
#' @note It is quite slow, works for graphs < 2000 nodes
.visualizeIGraph2d<-function(g){
  #g<-as.undirected(g)
  L<-cbind(V(g)$v0, V(g)$v1)
  l = fastgreedy.community(g)#as.undirected(g))
  col<-rainbow(length(l))
  plot.igraph(g,vertex.size=3.0,vertex.label=NA,vertex.color=col[membership(l)],layout=L)
}

