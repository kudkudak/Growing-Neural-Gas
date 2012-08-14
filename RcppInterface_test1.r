source("RcppInterface.r")
library("multicore")

sv<-new("GNGClient")

sv$getNode(3)

sv$getNode(1)

sv$getNode(0)

