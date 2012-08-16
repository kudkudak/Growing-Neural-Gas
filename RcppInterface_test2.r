source("RcppInterface.r")
library("multicore")
library("scatterplot3d")

sv<-new("GNGClient")

nodes <- sv$getNumberNodes()

x <- array(dim=nodes)
y <- array(dim=nodes)
z <- array(dim=nodes)

for(i in 1:nodes)
{
	node = sv$getNode(i-1)
	x[i] = node[1]
	y[i] = node[2]
	z[i] = node[3]
}


scatterplot3d(x,y,z, pch=16, cex.symbols=0.25, axis=FALSE )
