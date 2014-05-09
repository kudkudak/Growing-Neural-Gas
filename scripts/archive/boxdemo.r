source("gng.r")

 GNGSetParams(debug_level=100, max_nodes=600,
               orig=c(-1, -1, +1), axis=c(2, 2, 2),
               database_type=2)
GNGCreateServer()
Sys.sleep(2.0)
sv<-new("GNGClient")

boxPoint<-function(){
point<-c(0,0,0,0)
point[1] = runif(1)
point[2] = runif(1)
point[3] = runif(1)
point[4]=point[1]
return(point)
}

linePoint<-function(){
point<-c(0,0,0,0)
t<-runif(1)
point[1] = t
point[2] = t
point[3] = 1.0
point[4] = 2.0
return(point)
}

mat<-matrix(0,20000,4)

for(i in 1:10000){
mat[2*i-1,1:4]=boxPoint()
mat[2*i,1:4]=linePoint()
}

sv$addExamples(mat)
sv$runServer()

