
source("RcppInterface.r")
library("multicore")
library("rgl")
library("e1071")
library("scatterplot3d")

iteration<-0

sv<-new("GNGClient")

rgl.light()

mat <- matrix(rnorm(20,mean=1), 10000,3)

for(j in 1:10000){
	t<-rnorm(1,mean=0,sd=1)
	u<-rnorm(1,mean=0,sd=1)
	val<-3*sigmoid(t^2+u^2);
	mat[j,1] = t
	mat[j,2] = u
	mat[j,3] = val 	

}

#sv$addExamples(mat)


if(sv$getNumberNodesOnline()!=0){

iteration<-iteration+1
#print(cat("iteration nr ",iteration,sep=""))
tmp<-sv$updateBuffer()

print(cat(":: update of buffer successful, iteration nr ",iteration,sep=""))

#not that good........
nodes <- sv$getBufferSize()
print(nodes)

x_lines <- c(0,0)
y_lines <- c(0,0)
z_lines <- c(0,0)

k<-3
m<-1

x<-c(1:nodes)
y<-c(1:nodes)
z<-c(1:nodes)

for(i in 1:(nodes))
{
	node = sv$getNode(i-1) #from node matrix?
	#print(node)
	x[i]=node[2]
	y[i]=node[3]
	z[i]=node[4]
	
	#print(node)
	#print(cat(length(node)-3," edges"))
	if( length(node)>4){
	
	

 	for(j in 1:(length(node)-4) ){
		
		x_lines[k] = x[i]
		y_lines[k] = y[i]
		z_lines[k] = z[i]
		#print(cat("accessing", node[j+3]+1, " at node ",i,sep=""))
		#print(dim(nodes_matrix))
		
		#idea - find edges later when have all x,y,z read in - better?

		connected_node = (sv$getNode(node[j+4]))

		x_lines[k+1]=connected_node[2]
		y_lines[k+1]=connected_node[3]
		z_lines[k+1]=connected_node[4]
		k=k+2		
	 }	

	

	}
	
}
print("::reading was succesful")
print(cat(nodes," of nodes",cat=""))

zscale <- 1


# clear scene:
#clear3d("all")
 
# setup env:
  clear3d(type="shapes")
  rgl.bg(color="white")

  
#light3d()
cx <- c(1:(nodes-1))
cy <- c(1:(nodes-1))
cz <- c(1:(nodes-1))

cx <- abs(x)/max(abs(x))
cy <- abs(y)/max(abs(y))
cz <- abs(z)/max(abs(z))

#rgl.lines(x_lines,y_lines,z_lines,color=rgb(0,0,0))
points3d(x, y, z,  color=rgb(cx,cy,cz))

}

