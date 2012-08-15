
source("RcppInterface.r")
library("multicore")
library("rgl")
library("e1071")

iteration<-0

sv<-new("GNGClient")

rgl.light()

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

while(1){

Sys.sleep(0.6)

if(sv$getNumberNodesOnline()!=0){

iteration<-iteration+1
#print(cat("iteration nr ",iteration,sep=""))
tmp<-sv$updateBuffer()
print(cat("iteration nr ",iteration,sep=""))
nodes <- sv$getNumberNodes()
nodes_matrix <- sv$getNodeMatrix()

x <- nodes_matrix[,1]
y <- nodes_matrix[,2]
z <- nodes_matrix[,3]

x_lines <- c(0,0)
y_lines <- c(0,0)
z_lines <- c(0,0)

nodes_matrix

k<-3

for(i in 1:(nodes-1))
{
	node = sv$getNode(i-1) #from node matrix?
	#print(node)
	#print(cat(length(node)-3," edges"))
	if( length(node)>3){
	
 	for(j in 1:(length(node)-3) ){
		
		x_lines[k] = x[i]
		y_lines[k] = y[i]
		z_lines[k] = z[i]
		#print(cat("accessing", node[j+3]+1, " at node ",i,sep=""))
		#print(dim(nodes_matrix))
		x_lines[k+1]=nodes_matrix[node[j+3]+1,1]
		y_lines[k+1]=nodes_matrix[node[j+3]+1,2]
		z_lines[k+1]=nodes_matrix[node[j+3]+1,3]
		k=k+2		
	 }	

	

	}
	
}
print("reading was succesful")
print(cat(nodes," of nodes",cat=""))
print(dim(nodes_matrix))


zscale <- 1
 
# clear scene:
#clear3d("all")
 
# setup env:
  clear3d(type="shapes")
  rgl.bg(color="black")

  
#light3d()
cx <- c(1:(nodes-1))
cy <- c(1:(nodes-1))
cz <- c(1:(nodes-1))

cx <- abs(x)/max(abs(x))
cy <- abs(y)/max(abs(y))
cz <- abs(z)/max(abs(z))

spheres3d(x, y, z, radius=0.1 , rgb(cx,cy,cz))
rgl.lines(x_lines,y_lines,z_lines)#,color=rgb(1,1,1))
}
}
