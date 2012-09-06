require(RcppArmadillo)
system( sprintf(Rcpp:::RcppLdFlags()) )


 PKG_LIBS <- sprintf( "build/performance/SHGraphDefs.o build/performance/GNGAlgorithm.o build/performance/SHMemoryManager.o build/performance/Utils.o %s -I./include -I./src $(LAPACK_LIBS) $(BLAS_LIBS) $(FLIBS) -L/home/staszek/usr/lib -lboost_thread -lboost_system",
 Rcpp:::RcppLdFlags() )
 PKG_CPPFLAGS <- sprintf( "-I./include -I./src -O4 %s %s", Rcpp:::RcppCxxFlags(),
 RcppArmadillo:::RcppArmadilloCxxFlags() )

 Sys.setenv(
 PKG_LIBS = PKG_LIBS ,
 PKG_CPPFLAGS = PKG_CPPFLAGS
 )

 R <- file.path( R.home(component = "bin"), "R" )

 system( sprintf( "%s  CMD SHLIB %s", R, paste( commandArgs(TRUE),
	 collapse = " ") ) )
