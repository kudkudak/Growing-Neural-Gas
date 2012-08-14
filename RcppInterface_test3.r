
source("RcppInterface.r")
library("multicore")
library("rgl")


sv<-new("GNGClient")

nodes <- sv$getNumberNodes()
print(nodes)


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

zscale <- 1
 
# clear scene:
clear3d("all")
 
# setup env:
bg3d(color="white")
light3d()
  
spheres3d(x, y, z, radius=2.15,color=rgb(abs(x)/max(abs(x)),abs(y)/max(abs(y)),abs(z)/max(abs(z))))
