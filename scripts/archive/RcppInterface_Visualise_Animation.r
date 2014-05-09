
source("RcppInterface.r")
library("multicore")
library("rgl")
library("e1071")

delay<-0.5

iteration<-0

sv<-new("GNGClient")
rgl.clear("all")
rgl.light()


target_number<-2

print("Server paused")
while(1){

sv$runServer()
while(sv$getNumberNodesOnline()<target_number || sv$getNumberNodesOnline()>999){
	
}
target_number<-target_number+1
sv$pauseServer()
iteration<-iteration+1
if(sv$getNumberNodesOnline()!=0){


#print(cat("iteration nr ",iteration,sep=""))
tmp<-sv$updateBuffer()

print(cat(":: update of buffer successful, iteration nr ",iteration,sep=""))

#not that good........
x_lines <- c(0,0)
y_lines <- c(0,0)

z_lines <- c(0,0)
k<-3
m<-1

x<-c(1:nodes)
y<-c(1:nodes)
z<-c(1:nodes)

if(iteration>1){
rm(x)
rm(y)
rm(z)
rm(x_lines)
rm(y_lines)
rm(z_lines)
rm(nodes)
}
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
  rgl.bg(color="white")

 
cx <- c(1:(nodes-1))
cy <- c(1:(nodes-1))
cz <- c(1:(nodes-1))

cx <- abs(x)/max(abs(x))
cy <- abs(y)/max(abs(y))
cz <- abs(z)/max(abs(z))

errortext<-cat("Error = ",sv$getAccumulatedError(),cat="")

axes3d(edges="bbox")
rgl.spheres(x,y,z,radius=0.06,color=rgb(0.2,0.2,0.2))
rgl.lines(x_lines,y_lines,z_lines,color=rgb(0.5,0.4,0.5))
if(iteration<10) nmbr<-paste("00",iteration,sep="")
else if(iteration<100) nmbr<-paste("0",iteration,sep="")
else nmbr<-iteration

filename<- paste("formation1_",nmbr,".png",sep="")
snapshot3d(filename)
print(filename)
if(iteration==1) scan()

}
}
