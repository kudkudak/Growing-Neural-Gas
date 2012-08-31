
source("RcppInterface.r")
library("multicore")
library("rgl")
library("e1071")

iteration<-0

sv<-new("GNGClient")

rgl.light()
rgl.clear("all")


while(1){

Sys.sleep(3.6)

if(sv$getNumberNodesOnline()!=0){

iteration<-iteration+1
#print(cat("iteration nr ",iteration,sep=""))
tmp<-sv$updateBuffer()

print(cat(":: update of buffer successful, iteration nr ",iteration,sep=""))

#not that good........
nodes <- sv$getBufferSize()

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
		
		#print(connected_node)
		

		dist = sqrt((x[i] - connected_node[2])^2 + (y[i] - connected_node[3])^2 + (z[i] - connected_node[4])^2)
		#print(dist)

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
  clear3d(type="shapes")
  rgl.bg(color="black")

 
cx <- c(1:(nodes-1))
cy <- c(1:(nodes-1))
cz <- c(1:(nodes-1))

cx <- abs(x)/max(abs(x))
cy <- abs(y)/max(abs(y))
cz <- abs(z)/max(abs(z))

errortext<-cat("Error = ",sv$getAccumulatedError(),cat="")

axes3d(edges="bbox")
rgl.points(x,y,z,radius=0.009,color=rgb(cx,cy,cz))
rgl.lines(x_lines,y_lines,z_lines,color=rgb(cx,cy,cz))


}
}
