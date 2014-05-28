.gng.plane_point<-function(r,center){
  if(!hasArg(r)) r<-1.0
  if(!hasArg(center)) center<-c(0,0,0)
  
  point<-center
  point[1]<-point[1]+r*runif(1.0)
  point[2]<-point[2]+r*runif(1.0)
  point[3]<-point[3]
  
  return(point)
}

.gng.sphere_point<-function(r,center){
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

gng.preset.plane<-function(N, side=0.5, center=c(0.5,0.5,0.5)){
  mat<-matrix(0,N,3)
  
  for(i in 1:N){
    mat[i,1:3] = .gng.plane_point(side, center)
    mat[i,3] = mat[i,1]
  }
  
  mat
}


gng.preset.sphere<-function(N, r=0.5, center=c(0.5,0.5,0.5)){
  mat<-matrix(0,N,3)
  
  for(i in 1:N){
    mat[i,1:3] = .gng.sphere_point(r, center)
  }
  
  mat
}


.sigmoid <- function(x){
   1./(1.+exp(-x))
}

gng.preset_potential<-function(N){
  mat <- matrix(rnorm(20,mean=1), N,3)
  
  for(j in 1:N){
    t<-rnorm(1,mean=0,sd=1)
    u<-rnorm(1,mean=0,sd=1)
    val<-.sigmoid(t^2+u^2);
    mat[j,1] = t
    mat[j,2] = u
    mat[j,3] = val 	
    
  }
  
  mat
}







