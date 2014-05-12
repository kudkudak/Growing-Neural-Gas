library(Rcpp)
library(devtools)
library(BH)
library(inline)

build(".")
load_all(".")


unif_module<-Module("gng_module",getDynLib("GrowingNeuralGas"))

unif_module

gngConfig <- unif_module$GNGConfiguration

u <- new(gngConfig)

u$message_bufor_size = 0


