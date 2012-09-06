source("gng.r")

parallel(createGNGServer())
Sys.sleep(2.0)
sv<-new("GNGClient")

iteration<-0

