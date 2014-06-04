\docType{methods}
\name{dump_model.gng}
\alias{dump_model}
\alias{dump_model.gng}
\title{dump_model}
\format{\preformatted{ NULL
}}
\usage{
dump_model(gng)
}
\arguments{
  \item{filename}{Dump destination}
}
\description{
Writes graph to a disk space efficient binary format. It
can be used in GNG constructor to reconstruct graph from
file.
}
\details{
Dump model to binary
}
\examples{
dump_model(gng, 'graph.bin')
}
\keyword{datasets}

