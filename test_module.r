library(devtools)
library(BH)

install.packages("BH")

load_all(".")

unif_module<-Module("GrowingNeuralGas",getDynLib(gng))

