
library("rgl")

rgl.light()
rgl.clear("all")



iteration<-iteration+1
#print(cat("iteration nr ",iteration,sep=""))
tmp<-sv$updateBuffer()

print(cat(":: update of buffer successful, iteration nr ",iteration,sep=""))

if(iteration>1){
rm(x)
rm(y)
rm(z)
rm(x_lines)
rm(y_lines)
rm(z_lines)
rm(nodes)
}
#not that good........
nodes <- sv$getBufferSize()

k<-1
m<-1

x<-c(0)
y<-c(0)
z<-c(0)
x_lines<-c(0)
y_lines<-c(0)
z_lines<-c(0)

errors<-c(0)

for(i in 1:(nodes-1))
{
	errors[i]=sv$getErrorNode(i-1)
	node = sv$getNode(i-1) #from node matrix?
	if(i==1){
		print(node)
		print(sv$getNodeNumberEdges(0))	
	}
	x[i]=node[2]
	y[i]=node[3]
	z[i]=node[4]

	if(node[1]<1){
		print("empty")
		print(sv$getNumberNodesOnline())
		print(sv$getBufferSize())
	}
	if( length(node)>4 & node[1]==1){
	
	

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
print(cat(nodes," of nodes",cat=""))<
print(sv$getAccumulatedError())

zscale <- 1


# clear scene:
#clear3d("all")
 
# setup env:
  clear3d(type="all")
  rgl.bg(color="white")

 

cx <- c(1:(nodes-1))
cy <- c(1:(nodes-1))
cz <- c(1:(nodes-1))

print(max(abs(errors)))

cx <- abs(errors)/max(abs(errors)) #abs(x)/max(abs(x))
print(cx)
cy[1:nodes-1] <- 0.1 #abs(y)/max(abs(y)
cz[1:nodes-1] <- 0.1 #abs(z)/max(abs(z))

print(cz[1])

print(length(cz))
print(length(x))

errortext<-cat("Error = ",sv$getAccumulatedError(),cat="")

axes3d(edges="bbox")

rgl.spheres(x,y,z,radius=cx/20, color=rgb(cx,cy,cz))
rgl.lines(x_lines,y_lines,z_lines,color=rgb(0.0,0.0,0.0))


