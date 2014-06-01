####################################################################
# Clustering MNIST dataset with GNG algorithm and running RF on it #
# note: make sure you have in data mnist dataset                   #
 ####################################################################


# Load the MNIST digit recognition dataset into R
# http://yann.lecun.com/exdb/mnist/
# assume you have all 4 files and gunzip'd them
# creates train$n, train$x, train$y and test$n, test$x, test$y
# e.g. train$x is a 60000 x 784 matrix, each row is one digit (28x28)
# call: show_digit(train$x[5,]) to see a digit.
# brendan o'connor - gist.github.com/39760 - anyall.org

load_mnist <- function() {
  load_image_file <- function(filename) {
    ret = list()
    f = file(filename,'rb')
    readBin(f,'integer',n=1,size=4,endian='big')
    ret$n = readBin(f,'integer',n=1,size=4,endian='big')
    nrow = readBin(f,'integer',n=1,size=4,endian='big')
    ncol = readBin(f,'integer',n=1,size=4,endian='big')
    x = readBin(f,'integer',n=ret$n*nrow*ncol,size=1,signed=F)
    ret = matrix(x, ncol=nrow*ncol, byrow=T)
    close(f)
    ret
  }
  load_label_file <- function(filename) {
    f = file(filename,'rb')
    readBin(f,'integer',n=1,size=4,endian='big')
    n = readBin(f,'integer',n=1,size=4,endian='big')
    y = readBin(f,'integer',n=n,size=1,signed=F)
    ret = list()
    close(f)
    ret = y
    ret
  }
  train <<- load_image_file('./data/train-images.idx3-ubyte')
  test <<- load_image_file('./data/t10k-images.idx3-ubyte')
  
  train <- train/255.0
  test <- test/255.0
  
  data = list()
  data$train = cbind(train, as.matrix(load_label_file('./data/train-labels.idx1-ubyte')))
  data$test = cbind(test, as.matrix(load_label_file('./data/t10k-labels.idx1-ubyte')))
  
  
  data
}
show_digit <- function(arr784, col=gray(12:1/12), ...) {
  print(matrix(arr784, nrow=28, ncol=28)[1,])
  image(matrix(arr784, nrow=28, ncol=28)[,28:1], col=col, ...)
}

library("GrowingNeuralGas")
library(igraph)
library(testthat)

max_nodes <- 1500

# Construct gng object, NOTE: adding last column (target) as extra_data - this data won't be used
# in training, but will be assigned to close vertex in the graph (technically speaking it WILL be used in training,
# but will bear no effect on convergence)
gng <- GNG(dataset_type=gng.dataset.bagging, max_nodes=max_nodes, dim=784, lazyheap_optimization=TRUE,
           experimental_vertex_extra_data=TRUE, load_model_filename="mnist.trained.1500.bin"
           )

data <- load_mnist()
data0 <- data$train
data0[data0[,785]!=0.0,785] = 1.0
gng$insert_examples(data0)

### Run algorithm ###
run(gng)
number_nodes(gng)
mean_error(gng)

### Pause and dump ###
pause(gng)
GrowingNeuralGas::dump_model(gng, "mnist.trained.1500.bin")

### Plot using igraph layout and coloring from extra vertex ###
plot(gng, mode=gng.plot.2d.errors, vertex.color=gng.plot.color.extra, layout=layout.fruchterman.reingold)


### Test prediction of 0 ###
for(i in 1:nrow(data$test)){
  if(data$test[i, 785]==0){
    predict(gng, data$test[i,-1])
    node(gng, predict(gng, data$test[i,-1]))
    print(node(gng, predict(gng, data$test[i,-1]))$extra_data)
    break
  }
}

### Test infomap community ###
plot(gng, vertex.color=gng.plot.color.cluster, 
     mode=gng.plot.2d, layout=igraph::layout.fruchterman.reingold)

centr <- centroids2.gng(gng)
### Plot centroids ###
centroids_pos = lapply(centr, function(x){ node(gng, x)$pos})
par(mfrow=c(2,2))
show_digit(node(gng, centr[9])$pos)
show_digit(node(gng, centr[3])$pos)
show_digit(node(gng, centr[2])$pos)
show_digit(node(gng, centr[4])$pos)


#####################################################################
# Code training classifier, not pertaining to Growing-Neural-Gas API#
#####################################################################

function train_classifier(){
  ### Transform data ###
  data <-load_mnist()
  data_transformed_train <- matrix(0, ncol=(length(centroids_pos) + 1), nrow=nrow(data$train))
  data_transformed_test <- matrix(0, ncol=(length(centroids_pos) + 1), nrow=nrow(data$test))
  
  for(i in 1:nrow(data$train)){
      data_transformed_train[i, 1:length(centroids_pos)] = unlist(lapply(centroids_pos, 
                                                                  function(x){ sqrt(sum((x - data$train[i, 1:784]) ^ 2)) } ))                                         
      data_transformed_train[i, length(centroids_pos)+1 ] = data$train[i, 785]
  }
  for(i in 1:nrow(data$test)){
    data_transformed_test[i, 1:length(centroids_pos)] = unlist(lapply(centroids_pos, 
                                                                       function(x){ sqrt(sum((x - data$test[i, 1:784]) ^ 2)) } ))                                         
    data_transformed_test[i, length(centroids_pos)+1 ] = data$test[i, 785]
  }
  
  write.csv(data_transformed_train, file='mnist.transformed.train.csv')
  write.csv(data_transformed_test, file='mnist.transformed.test.csv')
  
  ### Construct formula for nnet ###
  data_transformed_train = read.csv(file='mnist.transformed.train.csv')
  data_transformed_test = read.csv(file='mnist.transformed.test.csv')
  
  colnames(data_transformed_train) <- paste0("V", seq_len(ncol(data_transformed_train)))
  colnames(data_transformed_test) <- paste0("V", seq_len(ncol(data_transformed_test)))
  n <- colnames(data_transformed_train)
  last_col = paste("V", ncol(data_transformed_train), sep="")
  f <- as.formula(paste(paste(last_col, " ~", sep=""), paste(n[!n %in% last_col], collapse = " + ")))
  print(f)
  
  
  ### Train nnet ###
  install.packages("randomForest")
  library(randomForest)
  library(kernlab)
  library(klaR)
  library("nnet")
  
  rf <- randomForest(x=data_transformed_train[,1:26], 
                     y=as.factor(data_transformed_train[,27]), ntree=50)
  
  as.double(predict(rf, data_transformed_test[1,1:26]))-1
  
  k <- 0
  cor <- 0
  for(i in 1:nrow(data_transformed_test)){
    
    if(i%%100 == 0){
      print((cor+0.0)/k)
       print(i)
    }
    
    if((as.double(predict(rf, data_transformed_test[i,1:(ncol(data_transformed_test)-1)]))-1)
       == data_transformed_test[i,ncol(data_transformed_test)]){
       cor <- cor + 1
    }
  
    k <- k + 1
  }

}



