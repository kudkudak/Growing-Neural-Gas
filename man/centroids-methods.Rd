\docType{methods}
\name{centroids.gng}
\alias{centroids}
\alias{centroids.gng}
\title{centroids}
\format{\preformatted{ NULL
}}
\usage{
centroids(gng)
}
\description{
Using infomap.communities finds communities and for each
community pick node with biggest betweenness score
}
\details{
Get centroids
}
\examples{
# Print position of the first centroid
print(node(gng, centroids(gng)[1])$pos)
}
\keyword{datasets}

