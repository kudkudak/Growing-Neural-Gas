
source("RcppInterface.r")
library("multicore")
library("rgl")
library("e1071")

sv<-new("GNGClient")


mat <- matrix(rnorm(20,mean=1), 10000,3)

for(j in 1:10000){
	t<-rnorm(1,mean=0,sd=2)
	u<-rnorm(1,mean=0,sd=2)
	val<-3*sigmoid(t^2+u^2);
	mat[j,1] = t
	mat[j,2] = u
	mat[j,3] = val 	

}

sv$addExamples(mat)

Sys.sleep(7)

nodes <- sv$getNumberNodes()
print(nodes)

nodes_matrix <- matrix(1,nodes,3)

for(i in 1:(nodes-1)){
	nodes_matrix[i,]=(sv$getNode(i-1))[1:3]
}

print(nodes_matrix)

x <- c(1:(nodes-1))
y <- c(1:(nodes-1))
z <- c(1:(nodes-1))

x_lines <- c(0,0)
y_lines <- c(0,0)
z_lines <- c(0,0)



k<-3

for(i in 1:(nodes-1))
{
	node = sv$getNode(i-1)
	cat("read ",node," ",length(node),"\n")
	x[i] = nodes_matrix[i,1]
	y[i] = nodes_matrix[i,2]
	z[i] = nodes_matrix[i,3]
	if( length(node)>3){
	for(j in 1:(length(node)-3)){
		x_lines[k] = x[i]
		y_lines[k] = y[i]
		z_lines[k] = z[i]
		print(node[j+3])
		x_lines[k+1]=nodes_matrix[node[j+3]+1,1]
		y_lines[k+1]=nodes_matrix[node[j+3]+1,2]
		z_lines[k+1]=nodes_matrix[node[j+3]+1,3]
		k=k+2
		print(cat("adding node between ",node[j+3],i))
	}
	}
	
}
print(x_lines)

zscale <- 1
 
# clear scene:
clear3d("all")
 
# setup env:
bg3d(color="white")
light3d()
cx <- c(1:(nodes-1))
cy <- c(1:(nodes-1))
cz <- c(1:(nodes-1))

cx <- abs(x)/max(abs(x))
cy <- abs(y)/max(abs(y))
cz <- abs(z)/max(abs(z))

cx
cy
cz

spheres3d(x, y, z, radius=0.05 ,rgb(cx,cy,cz))
rgl.lines(x_lines,y_lines,z_lines,color=rgb(0,0,0))
