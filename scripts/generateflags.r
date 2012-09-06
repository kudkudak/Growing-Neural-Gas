require(RcppArmadillo)
system( sprintf("echo %s %s %s ", RcppArmadillo:::RcppArmadilloCxxFlags(), Rcpp:::RcppLdFlags(),Rcpp:::RcppCxxFlags()) )
