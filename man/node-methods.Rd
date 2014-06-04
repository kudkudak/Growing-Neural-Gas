\docType{methods}
\name{node.gng}
\alias{node}
\alias{node.gng}
\title{node}
\format{\preformatted{ NULL
}}
\usage{
node(gng, 10)
}
\arguments{
  \item{gng_id}{Id of the node to retrieve. This is the id
  returned by functions like predict, or centroids}
}
\description{
Retrieves node from resulting graph
}
\details{
Get GNG node
}
\examples{
print(node(gng, 10)$pos)
}
\keyword{datasets}

