\name{Growing-Neural-Gas}
\alias{Growing-Neural-Gas}
\docType{package}
\title{
Efficient C++ implementation of online clustering algorithm Growing Neural Gas.
}
\description{
This package contains fast C++ implementation 
of online clustering algorithm Growing Neural Gas.
It produces topological graph, that you can easily convert to igraph, or you can
dump your model to optimized binary file and load it later on.
}
\details{
\tabular{ll}{
Package: \tab Growing-Neural-Gas\cr
Type: \tab Package\cr
Version: \tab 0.6\cr
Date: \tab 2014-06-03\cr
License: \tab MIT License\cr
}
For overview of usage please see exemplary code in demo/ folder.
}
\author{
Stanislaw Jastrzebski

Maintainer: Stanislaw Jastrzebski <staszek<at>gmail<dot>com>
}
\references{
~~ Literature or other references for background information ~~
}
~~ Optionally other standard keywords, one per line, from file KEYWORDS in ~~
~~ the R documentation directory ~~
\keyword{ package 
cluster
machine-learning
}
\seealso{
}
\examples{
gng <- GNG(dataset_type=gng.dataset.bagging.prob, max_nodes=300, dim=3)

insert_examples(gng, preset=gng.preset.sphere, N=10000, prob=0.8)

run(gng)

insert_examples(gng, preset=gng.preset.box, N=10000, prob=0.8)

plot(gng, mode=gng.plot.2d.errors, vertex.color=gng.plot.color.cluster, layout=gng.plot.layout.igraph.v2d)
}
