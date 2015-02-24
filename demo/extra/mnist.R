library(gmum.r)
library(igraph)

####################################################################
#       Clustering MNIST dataset with GNG algorithm                #
####################################################################


### Helper functions ###
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
  train <<- load_image_file('./data/train-images-idx3-ubyte')
  test <<- load_image_file('./data/t10k-images-idx3-ubyte')
  
  train <- train/255.0
  test <- test/255.0
  
  data = list()
  data$train = cbind(train, as.matrix(load_label_file('./data/train-labels-idx1-ubyte')))
  data$test = cbind(test, as.matrix(load_label_file('./data/t10k-labels-idx1-ubyte')))
  
  data
}

show_digit <- function(arr784, col=gray(12:1/12), ...) {
  print(matrix(arr784, nrow=28, ncol=28)[1,])
  image(matrix(arr784, nrow=28, ncol=28)[,28:1], col=col, ...)
}

### Configure and load examples ###
train.examples <- 10000
max.nodes <- 100
max.iter = 500
data <- load_mnist() 
X = data$train[1:train.examples,-785]
Y = data$train[1:train.examples,785]
X.test = data$test[,-785]
Y.test = data$test[,785]

### Train Optimized GNG ###
gng <- OptimizedGNG(max.nodes=max.nodes, x=X, value.range=c(0,1),
                    labels=Y, training = gng.train.offline(max.iter, 1e-2))


### Print some variables and save ###
numberNodes(gng)
meanError(gng)
save.gng(gng, "mnist.trained.100.bin")

### Plot using igraph layout and coloring from extra vertex ###
plot(gng, mode=gng.plot.2d.errors, 
     vertex.color=gng.plot.color.label, layout=gng.plot.layout.igraph.fruchterman.fast)


### Show closest to some examples ###
id=200
show_digit(X.test[id,])
show_digit(node(gng, predict(gng, X.test[id,])+1)$pos)

id=300
show_digit(X.test[id,])
show_digit(node(gng, predict(gng, X.test[id,])+1)$pos)

id=400
show_digit(X.test[id,])
show_digit(node(gng, predict(gng, X.test[id,])+1)$pos)


### Plot centroids ###
centr <- centroids.gng(gng)
centroids_pos = lapply(centr, function(x){ node(gng, x)$pos})
par(mfrow=c(2,2))
show_digit(node(gng, centr[1])$pos)
show_digit(node(gng, centr[2])$pos)
show_digit(node(gng, centr[3])$pos)
show_digit(node(gng, centr[4])$pos)