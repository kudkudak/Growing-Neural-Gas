
.plane.point<-function(r,center){
  if(!hasArg(r)) r<-1.0
  if(!hasArg(center)) center<-c(0,0,0)
  
  point<-center
  point[1]<-point[1]+r*runif(1.0)
  point[2]<-point[2]+r*runif(1.0)
  point[3]<-point[3]
  
  return(point)
}

.sphere.point<-function(r,center){
  if(!hasArg(r)) r<-1.0
  if(!hasArg(center)) center<-c(0,0,0)
  
  alpha<-runif(1)*2*pi
  beta<-runif(1)*pi
  
  point<-center
  point[1]<-point[1]+r*cos(alpha)*sin(beta)
  point[2]<-point[2]+r*sin(alpha)*sin(beta)
  point[3]<-point[3]+r*cos(beta)
  
  return(point)
}
