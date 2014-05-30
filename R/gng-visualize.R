library(igraph)



if("rgl" %in% rownames(installed.packages()) == TRUE){
  .gng.plot3d<-function(gngServer){
    tmp_name <- paste("tmp",sample(1:1000, 1),".graphml", sep="")
    gngServer$export_to_graphml(tmp_name)
    print("Reading GraphML dumped")
    .visualizeIGraphRGL(.readFromGraphML(tmp_name))
    file.remove(tmp_name)
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
  }
}
.gng.plot2d.errors<-function(gngServer, cluster, layout_2d){
  tmp_name <- paste("tmp",sample(1:1000, 1),".graphml", sep="")
  gngServer$export_to_graphml(tmp_name)
  .visualizeIGraph2dWithErrors(.readFromGraphML(tmp_name ), cluster, layout_2d)
  file.remove(tmp_name)
}

.gng.plot2d<-function(gngServer, cluster, layout_2d){
  tmp_name <- paste("tmp",sample(1:1000, 1),".graphml", sep="")
  gngServer$export_to_graphml(tmp_name)
  .visualizeIGraph2d(.readFromGraphML(tmp_name ), cluster, layout_2d)
  file.remove(tmp_name)
}
#' Visualize igraph using igraph plot
#' It will layout graph using v0 and v1 coordinates
#' @note It is quite slow, works for graphs < 2000 nodes, and for graphs <400 when using layout
.visualizeIGraph2d<-function(g, cluster, layout_2d){
  #g<-as.undirected(g)
  L<-NULL
  
  if(layout_2d){
    L<-cbind(V(g)$v0, V(g)$v1)
  }else{
      L <- layout.auto(g)
    #     L<-layout.fruchterman.reingold(g, niter=10000, area=4*vcount(g)^2)
  }
  
  if(cluster){
    l = fastgreedy.community(g)#as.undirected(g))
    col<-rainbow(length(l))
    plot.igraph(g,vertex.size=3.0,vertex.label=NA,vertex.color=col[membership(l)],layout=L)
  }else{
    plot.igraph(g,vertex.size=3.0,vertex.label=NA,layout=L)
  }
}

.visualizeIGraph2dWithErrors<-function(ig, cluster, layout_2d){
    plot.new()
    par(mfrow=c(1,2))
    .visualizeIGraph2d(ig, cluster, layout_2d)
    title("Graph visualization")
    errors_raw = gng$get_error_statistics()
    errors = log((errors_raw+1)/min(errors_raw+1))[2:length(errors_raw)]
    plot(errors, type="l", lty=2, lwd=2, xlab="Time  [s]", ylab="Mean error (log)", frame.plot=F)
    title("Mean error (log)")
}
