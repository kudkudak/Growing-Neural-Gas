source("gng.r")
source("GNGConvertToIGraph.r")

unix_processes_before = system("ps", intern=TRUE)

# Construct graph


for (k in 1:200){

    GNGSetParams(debug_level=8, max_nodes=600,
                orig=c(-2, -2, -2), axis=c(2, 2, 2),
                database_type=2, memory_bound = 5000000)
    server = GNGCreateServer()
    Sys.sleep(1.0)
    print("Server created")
    print(server$server_id)
    sv<-new("GNGClient", server)
    print("Successful client creation")



    spherePoint<-function(){
        phi<-runif(1)*2*pi
        theta<-runif(1)*pi
        c(cos(theta)*cos(phi), cos(theta)*sin(phi), sin(theta)) 
    }	

    mat<-matrix(0,9,4)

    for(i in 1:9){
        mat[i,1:3] = spherePoint()+1.0
        mat[i,4]=mat[i,1]
    }


    sv$addExamples(mat)
    sv$runServer()


# Construct graph and get buffer (no pausing deliberately)
    Sys.sleep(1.4)

    sv$pauseServer() #will it change anything?

    #Sys.sleep(1.0) #Erasing and not paused yet so update is caught when erasing ?? Possible!!

    sv$updateBuffer()
    print(sv$getBufferSize())
    if(sv$getBufferSize() > 10){
        g<-GNGConvertToIGraph(sv, 3) # test for current version
    }

# Running unit tests (almost)
    stopifnot(any(degree(g)==0)==FALSE)
    print("Test::No isolated vertexes")
    sv$pauseServer()
    print(paste("Terminating server",k))
    Sys.sleep(0.3)
    sv$terminateServer()

    print("Terminated server..")
    
}
Sys.sleep(2.0)    
unix_processes_after = system("ps", intern=TRUE)
stopifnot(length(unix_processes_before) - length(unix_processes_after) > -10)
print("Test::Termination successful")
