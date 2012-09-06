source("gng.r")
parallel(createGNGServer())
Sys.sleep(2.0)
sv<-new("GNGClient")

boxPoint<-function(){
point<-c(0,0,0)
point[1] = runif(1)
point[2] = runif(1)
point[3] = runif(1)
return(point)
}

linePoint<-function(){
point<-c(0,0,0)
t<-runif(1)
point[1] = t
point[2] = t
point[3] = 1.0
return(point)
}

mat<-matrix(0,20000,3)

for(i in 1:10000){
mat[2*i-1,1:3]=boxPoint()
mat[2*i,1:3]=linePoint()
}

sv$addExamples(mat)
sv$runServer()

