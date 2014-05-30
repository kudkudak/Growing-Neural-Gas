#Growing Neural Gas in R

Part of gmum.R project.

##General
------

This package contains fast C++ implementation 
of **on-line** clustering algorithm Growing Neural Gas wrapped as R package using Rcpp.
It produces topological graph, that you can easily convert to igraph.

This algorithms is widely used for dynamic clustering problem. Package is designed with
focus on big datasets. It is already possible to cluster dataset without making its
copy, with different dataset types (bagging, sequential or probability
sampling). In the near future it will be possible to stream dataset from csv file.

<small>Comparision of performance of original implementation and with
improvements suggested by D. Fiser, J. Faigl, M. Kulich </small>
<center><img src="https://raw.github.com/kudkudak/Growing-Neural-Gas/dev/doc/img/plot_speed.png" width="50%"></img></center>

Daniel Fiser, Jan Faigl, Miroslav Kulich optimization paper [FGNG: A fast multi-dimensional growing neural gas implementation](http://www.sciencedirect.com/science/article/pii/S0925231213009259)

##Installation
-----------

You can install it as an R package, it is easiest to do it using devtools (of
course make sure you download devtools package in the first place).

```Matlab
devtools::install_github("kudkudak/Growing-Neural-Gas")
```


##Usage

For more detailed usage see code in demo folder, or in tests/testthat folder. I
am currently working on documentation.

### Adding sphere data and clustering it.

```Matlab
library("GrowingNeuralGas")

# Create main GNG object
gng <- GNG(dataset_type=gng.dataset.bagging.prob, max_nodes=max_nodes, dim=3,
            uniformgrid_optimization=FALSE,  lazyheap_optimization=FALSE)

# Add examples (note: you can avoid here copy using set_memory_move_examples)
gng$insert_examples(preset=gng.preset.sphere, N=10000)

# Run algorithm in parallel
run(gng)

# Wait for the graph to converge
n <- 0
print("Waiting to converge")
while(number_nodes(gng) != gng$get_configuration()$max_nodes && n < 100) {
    Sys.sleep(1.0)
    n <- n + 1
}

# Find closest node
predict(gng, c(1,1,1))

# Plot 
plot(gng, mode=gng.plot.2derrors)

# Terminate GNG, to free memory you should call rm(gng)
terminate(gng)
```


##List of functions

I will add regular documentation after having finished adding last functionality
(dumping to file and clustering helper functions)

* GNG(...) - constructor for GNG object

* run(gng), pause(gng), terminate(gng) - execution control

* node(gng, gng_index) - returns node given index

* mean_error(gng) - mean error in the graph

* number_nodes(gng) - returns number of nodes

* error_statistics(gng) - vector of errors every second

* plot(gng, mode) - plots gng using one of the presets (gng.plot.rgl3d,
  gng.plot.2d, gng.plot.2derrors) 

* convert_igraph(gng) - converts GNG to igraph

* predict(gng, x) - return gng_index of the closest node in the graph to given
  example

* insert_examples(gng, M) - inefficient adding examples to the graph

* gng$insert_examples() - add examples to gng object. **Note**: this will
  perform a copy to gng object dataset. To set memory pointer to your matrix use
  set_memory_move_examples function (you cannot change it to other memory
pointer later on, only add new examples). You can also use function
insert_examples(gng, ...), but this will perform 2 copies in RAM due to R design.


##Examples
-----

<small>Plane triangulation</small>:
<center><img src="https://raw.github.com/kudkudak/Growing-Neural-Gas/dev/doc/img/ex1.png" width="60%" height="70%"></img></center>

<small>Bone reconstruction from a MRI scan</small>:
<center><img src="https://raw.github.com/kudkudak/Growing-Neural-Gas/doc/img/ex2.png" width="60%"></img></center>

<small>Reconstruction of the Buddha figure from Standford Repositories</small>
<center><img src="https://raw.github.com/kudkudak/Growing-Neural-Gas/doc/img/ex3.png" width="60%"></img></center>


##Testing
-----
To run tests you will need Google C++ Test Framework
installed to your search paths.

To run R tests type `devtools::test(".")` in the package folder.

##Contributors
---------
Feel free to contribute to the code. Contributions should be posted as pull requests. For now
there is not automatic testing, so make sure that tests located in tests/ folder are passing. 

Please use Google C++ naming convention: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml


