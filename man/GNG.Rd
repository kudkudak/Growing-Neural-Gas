\docType{data}
\name{GNG}
\alias{GNG}
\title{Constructor of GrowingNeuralGas object}
\format{\preformatted{ NULL
}}
\usage{
GNG
}
\arguments{
  \item{beta}{coefficient. Decrease the error variables of
  all node nodes by this fraction. Forgetting rate. Default
  0.99}

  \item{alpha}{Alpha coefficient. Decrease the error
  variables of the nodes neighboring to the newly inserted
  node by this fraction. Default 0.5}

  \item{uniformgrid.optimization}{TRUE/FALSE. You cannot
  use utility option with this, so please pass FALSE here
  then.}

  \item{lazyheap.optimization}{TRUE/FALSE. You cannot use
  utility option with this, so please pass FALSE here
  then.}

  \item{max.nodes}{Maximum number of nodes (after reaching
  this size it will continue running, but won't add new
  nodes)}

  \item{eps_w}{Default 0.05}

  \item{eps_n}{Default 0.0006}

  \item{dataset.type}{Dataset type. Possibilities
  gng.dataset.bagging, gng.dataset.bagging.prob,
  gng.dataset.sequential}

  \item{experimental_utility_option}{EXPERIMENTAL Utility
  option gng.experimental.utility.option.off /
  gng.experimental.utility.option.basic}

  \item{experimental_utility_k}{EXPERIMENTAL Utility option
  constant}

  \item{load_model_filename}{Set to path to file from which
  load serialized model}

  \item{experimental_vertex_extra_data}{if TRUE each
  example should have additional coordinate, that will be
  voted in graph. Each node (that you can get using node
  function) will have extra_data field that will be equal
  to mean of samples around given node. If used with
  probability dataset example layout is <vertex position>
  <vertex_extra_data> <sampling_probability>, for example
  c(0.3, 0.6, 0.7, 100, 0.7)}
}
\description{
Construct GNG object
}
\examples{
# Default GNG instance, without optimitzations and vertex dimensionality 3

gng <- GNG(dataset_type=gng.dataset.bagging.prob, max_nodes=max_nodes, dim=3)

# Construct GNG loaded from file with uniform grid

gng <- GNG(dataset_type=gng.dataset.bagging.prob, max_nodes=max_nodes, dim=3,
uniformgrid_optimization=TRUE,  lazyheap_optimization=FALSE,
uniformgrid_boundingbox_sides=c(3,3,3), uniformgrid_boundingbox_origin=c(-0.5,-0.5,-0.5),
load_model_filename="sphere_simple.bin")
}
\keyword{datasets}

