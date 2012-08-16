
source("RcppInterface.r")
library("multicore")
library("rgl")
library("e1071")
library("scatterplot3d")

iteration<-0

sv<-new("GNGClient")

rgl.light()

errors<-c(0)

iteration<-0

while(1){
iteration<-iteration+1
Sys.sleep(1)
print(sv$getAccumulatedError())
errors[iteration] <- sv$getAccumulatedError()
plot(errors,type="l")
}
