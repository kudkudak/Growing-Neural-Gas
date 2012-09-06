

iteration<-iteration+1

tmp<-sv$updateBuffer()

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
k<-1
m<-1

faces<-matrix(0,1000000,3)

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
		

		has_a<-0
		has_b<-0		

		for(h in 1:(length(node)-4)){
			neigh = sv$getNode(node[h+4])
			if( length(neigh)>4){
			  for(m in 1:(length(neigh)-4)){
				if((neigh[m+4]+1) == a) has_a<-1
				if((neigh[m+4]+1) == b) has_b<-1			
			  }
			}
			if(has_a==1 & has_b==1){
				
				faces[k,1] = a
				faces[k,2] = b
				faces[k,3] = node[h+4]+1
				k=k+1
			}
			has_a<-0
			has_b<-0
        	
		
		}
	 }	
	}
	
}


cat("",file="output2.obj")
for(i in 1:(nodes))
{
	cat(paste("v",x[i],y[i],z[i],"\n",sep=" "), file="output2.obj",append=TRUE)
}
for(i in 1:(k-1))
{

	cat(paste("f",faces[i,1],faces[i,2],faces[i,3],"\n",sep=" "), file="output2.obj",append=TRUE)

	++i
}



