#Growing Neural Gas in R

Easy to install and use package for fast **online** GNG algorithm.

Part of gmum.R project. See gmum research group page [here](http://gmum.ii.uj.edu.pl).

##General
------

This package contains **fast** C++ implementation 
of **online** clustering algorithm Growing Neural Gas wrapped as R package using Rcpp.
It produces topological graph, that you can easily **convert to igraph**, or save to 
efficient binary file.

This algorithms is widely used for dynamic clustering problem. Package is designed with
focus on **big datasets**. It is already possible to cluster dataset without making its
copy, with different dataset types (bagging, sequential or probability
sampling). In the near future it will be possible to stream dataset from csv file.

<small>Comparision of performance of original implementation and with
improvements suggested by D. Fiser, J. Faigl, M. Kulich </small>
<center><img src="https://raw.github.com/kudkudak/Growing-Neural-Gas/dev/doc/img/plot_speed.png" width="50%"></img></center>

Daniel Fiser, Jan Faigl, Miroslav Kulich optimization paper [Growing neural gas efficiently](http://www.sciencedirect.com/science/article/pii/S0925231212008351)

##Examples
-----

<small>MNIST dataset clustering (raw images)</small>
<center><img src="https://raw.github.com/kudkudak/Growing-Neural-Gas/dev/doc/img/mnist1.png" width="60%"></img></center>



##Installation
-----------

You can install it as an R package, it is easiest to do it using devtools (make sure you download devtools and RCurl packages in the first place).

```R
devtools::install_github("kudkudak/Growing-Neural-Gas")
```

I recommend using RStudio, especially for plotting.

See **Known issues** if your installation fails. There are few problems with
R/igraph that are platform dependent.

##Usage

For more detailed usage see code in demo folder, or in tests/testthat folder.
You can also refer to R package documentation (pdf version 
[gng.pdf](https://github.com/kudkudak/Growing-Neural-Gas/blob/master/doc/gng.pdf)).

### Cluster wine dataset

In this example we will construct a clustering of UCI wine dataset using offline GNG.

```R
library("GrowingNeuralGas")

# Load data
data(wine, package="rattle")
scaled_wine <- scale(wine[-1])

# Train in an offline manner
gng <- GNG(scaled_wine, labels=wine$Type, max_nodes=20)

# Find closest node to vector [1,1,1]
predict(gng, c(1,1,1))

# Convert to igraph and print mean degree of the network
ig = convertToGraph(gng)
mean(degree(ig))

# Find mean error
meanError(gng)

# Plot with first 2 coordinates as position
plot(gng, mode=gng.plot.2d.errors, vertex.color=gng.plot.color.cluster, 
     layout=gng.plot.layout.v2d)
```

<small>Reconstruction of the Buddha figure from Standford Repositories</small>
<center><img src="https://raw.github.com/kudkudak/Growing-Neural-Gas/dev/doc/img/ex3.png" width="60%"></img></center>


##List of functions

This is not a full documentation. Please refer to R package documentation (pdf version 
[gng.pdf](https://github.com/kudkudak/Growing-Neural-Gas/blob/master/doc/gng.pdf) ).

* GNG(...) - constructor for GNG object. Parameters:

    *  beta - Decrease the error variables of all node nodes by this fraction. Forgetting rate. Default 0.99

    *  alpha - Alpha coefficient. Decrease the error variables of the nodes neighboring to the newly inserted node by this fraction. Default 0.5

    *  lambda - how often to add new vertices. Default is 200

    *  max.node - Maximum number of nodes (after reaching this size it will continue running, but won't add new nodes)

    *  eps.n - Default 0.05

    *  eps.v - Default 0.0006

    *  max.edge.age - Maximum age of edge, after which it is deleted. Decrease
       if your graph is not following changes of the dataset.

	*  k - Utility parameter. Defaults to NULL. Controls speed of removal of obsolete nodes, a common value is 1.3.

* OptimizedGNG(...) - constructor for simplified and optimized (linear) GNG object. Parameters:

    *  beta - Decrease the error variables of all node nodes by this fraction. Forgetting rate. Default 0.99

    *  alpha - Alpha coefficient. Decrease the error variables of the nodes neighboring to the newly inserted node by this fraction. Default 0.5

    *  lambda - how often to add new vertices. Default is 200

    *  max.node - Maximum number of nodes (after reaching this size it will continue running, but won't add new nodes)

    *  eps.n - Default 0.05

    *  eps.v - Default 0.0006

    *  max.edge.age - Maximum age of edge, after which it is deleted. Decrease
       if your graph is not following changes of the dataset.

	*  value.range - Defaults to c(0,1). All example features have to be in this range.

* run(gng), pause(gng), terminate(gng) - execution control

* node(gng, gng_index) - returns node given index

* save(gng, filename) - save gng

* meanError(gng) - mean error in the graph

* numberNodes(gng) - returns number of nodes

* errorStatistics(gng) - vector of errors every second

* plot(gng, mode, layout, vertex.color start_s) - plots gng using one of the presets (gng.plot.rgl3d,
  gng.plot.2d, gng.plot.2derrors). If plotting erros you can specify second from
  it will plot the errors. 

* centroids(gng) - using igraph algorithm GNG will write centroids of found
  clusters (community centers)

* convertToGraph(gng) - converts GNG to igraph

* predict(gng, x) - return gng_index of the closest node in the graph to given
  example

* insertExamples(gng, M) - inefficient adding examples to the graph


##Contributors
---------
Feel free to contribute to the code. Contributions should be posted as pull requests. For testing please refer to tests folder. 

##Known issues
---------
* Package is not released for Windows yet.

* Igraph plotting issues

	* Due to bug in R (https://bugs.r-project.org/bugzilla/show_bug.cgi?id=15327)
	  on some OS you have to install liblzma-dev additionally. 

	* Sometimes after installation of igraph you might have disabled graphml support 
	  (http://lists.gnu.org/archive/html/igraph-help/2011-03/msg00101.html). Try
	installing libxml2-dev package and reinstalling igraph.a
