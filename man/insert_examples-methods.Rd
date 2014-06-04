\docType{methods}
\name{insert_examples.gng}
\alias{insert_examples}
\alias{insert_examples.gng}
\title{insert_examples}
\format{\preformatted{ NULL
}}
\usage{
insert_examples(gng, M)
}
\arguments{
  \item{examples}{Matrix with examples of dimensionality N
  rows x C columns, where C columns = dim (passed as
  parameter to GNG object) + 1 or 0 (1 if vertex_extra_data
  is TRUE) + 1 or 0 (1 if
  dataset_type=gng.dataset.bagging.prob).}

  \item{preset}{Use only if you are adding exemplary
  dataset. Possibilities: gng.preset.sphere,
  gng.preset.box. You can specify preset params: N=1000,
  center=c(0.5,0.5,0.5), prob=-1.}
}
\description{
Insert (inefficiently) examples to algorithm dataset. For
efficient insertion use gng$inser_examples, and for setting
memory pointer use gng$set_memory_move_examples
}
\note{
Complicated memory layout of the matrix is due to need for
memory efficiency. In the future versions you can expect
wrapper simplifying addition and also streaming from disk
file
}
\examples{
#Add preset examples with probability of being sampled (this assumed GNG was created with gng.dataset.bagging.prob dataset)
insert_examples(gng, preset=gng.preset.sphere)

#Insert efficiently examples
M <- matrix(0, ncol=3, nrow=10)
M[1,] = c(4,5,6)
gng$insert_examples(M)

#Set memory of the algorithm to point in memory. Note: you cannot remove this matrix while
in execution or you will experience memory error
gng$set_memory_move_examples(M)
}
\keyword{datasets}

