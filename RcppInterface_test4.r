
source("RcppInterface.r")
library("multicore")

library("e1071")

sv<-new("GNGClient")


mat <- matrix(rnorm(20,mean=1), 1000,3)

for(j in 1:1000){
	val<-sigmoid(rnorm(1,mean=0,sd=2));
	mat[j,1] = val
	mat[j,2] = val
	mat[j,3] = val 	
}

sv$addExamples(mat)

Sys.sleep(4)

nodes <- sv$getNumberNodes()
print(nodes)


x <- array(dim=nodes)
y <- array(dim=nodes)
z <- array(dim=nodes)

for(i in 1:(nodes-1))
{
	node = sv$getNode(i-1)
	x[i] = node[1]
	y[i] = node[2]
	z[i] = node[3]
}

print("read examples")


# setup env:


