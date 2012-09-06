
source("RcppInterface.r")
library("multicore")
library("rgl")
library("e1071")

iteration<-0

parallel(createGNGServer())
sv<-new("GNGClient")

