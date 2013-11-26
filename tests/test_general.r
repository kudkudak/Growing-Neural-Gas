source("gng.r")
source("GNGConvertToIGraph.r")

    unix_processes_before = system("ps")

# Construct graph

    GNGSetParams(debug_level=100, max_nodes=600,
                orig=c(-2, -2, -2), axis=c(2, 2, 2),
                database_type=2, memory_bound = 500000000)
    server = GNGCreateServer()
    Sys.sleep(2.0)
    print("Server created")
    print(server$server_id)
    sv<-new("GNGClient", server)
    print("Successful client creation")



    spherePoint<-function(){
        phi<-runif(1)*2*pi
        theta<-runif(1)*pi
        c(cos(theta)*cos(phi), cos(theta)*sin(phi), sin(theta)) 
    }	

    mat<-matrix(0,90000,4)

    for(i in 1:90000){
        mat[i,1:3] = spherePoint()+1.0
        mat[i,4]=mat[i,1]
    }


    sv$addExamples(mat)
    sv$runServer()




# Construct graph and get buffer (no pausing deliberately)
    Sys.sleep(20.0)
    sv$updateBuffer()
    g<-GNGConvertToIGraph(sv)


# Running unit tests (almost)
    stopifnot(any(degree(g)==0)==FALSE)
    print("Test::No isolated vertexes")
    stopifnot(sv$getNumberNodesOnline()==600)
    print("Test::Correct number of vertexes")
    error_before = sv$getAccumulatedError()
    stopifnot(error_before<20.0)
    print("Test::Converged")
    sv$runServer()
    Sys.sleep(1)
    sv$pauseServer()
    stopifnot(error_before!=sv$getAccumulatedError())
    print("Test::Functional communication")
    sv$terminateServer()
    #TODO: Add checking if termination was successful :)
    Sys.sleep(2)    
    stopifnot(system("ps") == unix_processes_before)
    print("Test::Termination successful")

