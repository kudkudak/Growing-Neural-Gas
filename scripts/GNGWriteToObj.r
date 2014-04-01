GNGWriteToObj<-function(sv, output){
sv$pauseServer()
tmp<-sv$updateBuffer()

nodes <- sv$getBufferSize()-1

x_lines <- c(0,0)
y_lines <- c(0,0)
z_lines <- c(0,0)
k<-1
m<-1

faces<-matrix(0,1000000,2)

x<-c(1:nodes)
y<-c(1:nodes)
z<-c(1:nodes)

for(i in 1:(nodes))
{
	node = sv$getNode(i-1)
	x[i]=node[2]
	y[i]=node[3]
	z[i]=node[4]
	
	if( length(node)>4){
 	 for(j in 1:(length(node)-4) ){
		
		a<-i	
		b<-node[j+4]+1
		
		

		faces[k,1] = a
		faces[k,2] = b
		
		
		
		k<-k+1
	 }	
	}
	
}


cat("",file=output)
for(i in 1:(nodes))
{
	cat(paste("v",x[i],y[i],z[i],"\n",sep=" "), file=output,append=TRUE)
}
for(i in 1:(k-1))
{

	cat(paste("l",faces[i,1],faces[i,2],"\n",sep=" "), file=output,append=TRUE)
}
sv$runServer()
}

