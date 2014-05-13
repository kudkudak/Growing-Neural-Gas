# Always clear workspace before installing new version of module
# Because Module function is caching 
rm(list=ls(all=TRUE))

library(Rcpp)
library(devtools)
library(devtools)
library(BH)
library(inline)

# Clean and install (load_all could have caused some problems - not sure here)
devtools::clean_dll(".")
devtools::install(".")

# Install module
x = library("GrowingNeuralGas")
mo<-Module("gng_module333", PACKAGE=x)


# Test loading
mo
g = mo$GNGConfiguration
s = mo$GNGServer
s
g
mo$GNGConfiguration

gngConfig = new(g)
gngConfig$message_bufor_size


gngServer = new(s, gngConfig)
gngServer$getConfiguration()
