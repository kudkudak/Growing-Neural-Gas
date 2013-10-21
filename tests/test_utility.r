source("gng.r")
source("GNGConvertToIGraph.r")



# Construct graph

    GNGSetParams(debug_level=100, max_nodes=600,
                orig=c(-2, -2, -2), axis=c(2, 2, 2),
                database_type=2, memory_bound = 500000000)
    server = GNGCreateServer()
    Sys.sleep(2.0)
    print("Server created")
    print(server$server_id)
    sv<-new("GNGClient", server[["server_id"]])
    print("Successful client creation")



    spherePoint<-function(){
        phi<-runif(1)*2*pi
        theta<-runif(1)*pi
        c(cos(theta)*cos(phi), cos(theta)*sin(phi), sin(theta)) 
    }	

    mat<-matrix(0,9000,4)

    for(i in 1:9000){
        mat[i,1:3] = spherePoint()+1.0
        mat[i,4]=mat[i,1]
    }


    sv$addExamples(mat)
    sv$runServer()


source("GNGVisualisePoints.r")
parallel(LearningCurve(sv, 100))


# Construct graph and get buffer (no pausing deliberately)
    Sys.sleep(20.0)
    sv$updateBuffer()
    g<-GNGConvertToIGraph(sv)



# Add jumped distribution 

    boxPoint<-function(){
    point<-c(0,0,0,0)
    point[1] = runif(1)
    point[2] = runif(1)
    point[3] = runif(1)
    point[4]=point[1]
    return(point)
    }

    mat<-matrix(0,90000,4)

    for(i in 1:90000){
        mat[i,1:3] = boxPoint()[1:3]-1
        mat[i,4]=0.5
    }


    sv$addExamples(mat)



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



