#Growing Neural Gas in R

Part of gmum.R project. See gmum research group page [here](http://gmum.ii.uj.edu.pl) .

##General
------

This package contains **fast** C++ implementation 
of **online** clustering algorithm Growing Neural Gas wrapped as R package using Rcpp.
It produces topological graph, that you can easily convert to igraph, or you can
dump your model to optimized binary file and load it later on.

This algorithms is widely used for dynamic clustering problem. Package is designed with
focus on **big datasets**. It is already possible to cluster dataset without making its
copy, with different dataset types (bagging, sequential or probability
sampling). In the near future it will be possible to stream dataset from csv file.

<small>Comparision of performance of original implementation and with
improvements suggested by D. Fiser, J. Faigl, M. Kulich </small>
<center><img src="https://raw.github.com/kudkudak/Growing-Neural-Gas/dev/doc/img/plot_speed.png" width="50%"></img></center>

Daniel Fiser, Jan Faigl, Miroslav Kulich optimization paper [FGNG: A fast multi-dimensional growing neural gas implementation](http://www.sciencedirect.com/science/article/pii/S0925231213009259)

##Examples
-----

<small>MNIST dataset clustering (raw images)</small>
<center><img src="https://raw.github.com/kudkudak/Growing-Neural-Gas/dev/doc/img/mnist1.png" width="60%"></img></center>

<small>Reconstruction of the Buddha figure from Standford Repositories</small>
<center><img src="https://raw.github.com/kudkudak/Growing-Neural-Gas/dev/doc/img/ex3.png" width="60%"></img></center>


##Installation
-----------

You can install it as an R package, it is easiest to do it using devtools (of
course make sure you download devtools package in the first place).

Make sure you have boost if you are unix/mac user. It is packaged in most distributions.
For instance in ubuntu you can download it using `sudo apt-get install
libboost-all-dev`

```Matlab
devtools::install_github("kudkudak/Growing-Neural-Gas")
```


##Usage

For more detailed usage see code in demo folder, or in tests/testthat folder.
You can also refer to R package documentation (pdf version 
[gng.pdf](https://github.com/kudkudak/Growing-Neural-Gas/blob/master/doc/gng.pdf)).

### Adding sphere data and clustering

```Matlab
library("GrowingNeuralGas")

max_nodes <- 600

# Create main GNG object (without optimization) with dataset sampling according
#to probability passed as last coordinate
gng <- GNG(dataset_type=gng.dataset.bagging.prob, max_nodes=max_nodes, dim=3)

# Add examples (note: you can avoid here copy using set_memory_move_examples)
gng$insert_examples(preset=gng.preset.sphere, N=10000, prob=0.8)

# Run algorithm in parallel
run(gng)

# Wait for the graph to converge
print("Waiting to converge")
while(number_nodes(gng) != max_nodes) 
    Sys.sleep(1.0)

# Find closest node to vector [1,1,1]
predict(gng, c(1,1,1))

# Plot with first 2 coordinates
plot(gng, mode=gng.plot.2d.errors, vertex.color=gng.plot.color.cluster, layout=gng.plot.layout.igraph.v2d)

# Terminate GNG, to free memory you should call rm(gng)
terminate(gng)
```


##List of functions

This is not a full documentation. Please refer to R package documentation (pdf version 
[gng.pdf](https://github.com/kudkudak/Growing-Neural-Gas/blob/master/doc/gng.pdf) ).

* GNG(...) - constructor for GNG object. Parameters:

    *  beta - Decrease the error variables of all node nodes by this fraction. Forgetting rate. Default 0.99

    *  alpha - Alpha coefficient. Decrease the error variables of the nodes neighboring to the newly inserted node by this fraction. Default 0.5

    *  uniformgrid_optimization - TRUE/FALSE. If TRUE please pass bounding box
       parameters also.
 
    *  lazyheap_optimization - TRUE/FALSE. 

    *  max.node - Maximum number of nodes (after reaching this size it will continue running, but won't add new nodes)

    *  eps_n - Default 0.05

    *  eps_v - Default 0.0006

    *  dataset_type - Dataset type. Possibilities gng.dataset.bagging, gng.dataset.bagging.prob (sampling according to dim+1 coordinate probability), gng.dataset.sequential (loop through examples - default option)

    *  experimental_utility_option - EXPERIMENTAL Utility option (try using it
       for quickly changing distributions). Value: gng.experimental.utility.option.off / gng.experimental.utility.option.basic

    *  experimental_utility_k - EXPERIMENTAL Utility option constant. Default
       to 1.5.

    *  experimental_vertex_extra_data

    *  load_model_filename - Set to path to file from which load serialized model

    *  uniformgrid_boundingbox_sides - Required for uniformgrid_optimization.
      You will need to define bounding box for your data, that will remain the
      same throughout the execution. To change it you should dump model and
      recreate from dumped file
    
    *  uniformgrid_boundingbox_origin - Origin of the bounding box    

    *  max_edge_age - Maximum age of edge, after which it is deleted. Decrease
       if your graph is not following changes of the dataset (you can also try
       experimental utility option)

* run(gng), pause(gng), terminate(gng) - execution control

* node(gng, gng_index) - returns node given index

* dump_model(gng, filename) - dump model to file

* mean_error(gng) - mean error in the graph

* number_nodes(gng) - returns number of nodes

* error_statistics(gng) - vector of errors every second

* plot(gng, mode, layout, vertex.color start_s) - plots gng using one of the presets (gng.plot.rgl3d,
  gng.plot.2d, gng.plot.2derrors). If plotting erros you can specify second from
  it will plot the errors. 

* centroids(gng) - using igraph algorithm GNG will write centroids of found
  clusters (community centers)

* convert_igraph(gng) - converts GNG to igraph

* predict(gng, x) - return gng_index of the closest node in the graph to given
  example

* insert_examples(gng, M) - inefficient adding examples to the graph

* gng$insert_examples() - add examples to gng object. **Note**: this will
  perform a copy to gng object dataset. To set memory pointer to your matrix use
  set_memory_move_examples function (you cannot change it to other memory
pointer later on, only add new examples). You can also use function
insert_examples(gng, ...), but this will perform 2 copies in RAM due to R design.



##Testing
-----
To run tests you will need Google C++ Test Framework
installed to your search paths.

To run R tests type `devtools::test(".")` in the package folder.

##Contributors
---------
Feel free to contribute to the code. Contributions should be posted as pull requests. 
