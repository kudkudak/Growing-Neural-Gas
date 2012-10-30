**Growing Neural Gas in R**
====================

General
------

Implementation of GNG algorithm in C++/Rcpp/R. This version should be stable, please e-mail me 
provided something bad occured :)
Alpha build, which means unfriendly building, and not fully implemented interface for the end user, 
but I reckon that it is already consitutes a practical tool for data analysis.

#Installation
-----------
Because it is a very early version, building isnt straightforward. You need to have installed libboost libraries and necessary packages in R (i.e. rgl, parallel). Now go to the reposity directory and enter:
```bash
    make rcpp
```

#Usage
-----
Please consult scripts/ files, simple sanity-check (first run R in scripts directory)
```R
    source("gng.r") #load package
    GNGCreateServer() 
    #wait at least 2 seconds (booting)
    sv<-new("GNGClient")
    sv$addExamples(preset="sphere",preset_size=100000) #load sphere point distribution
    sv$runServer()
    #wait at least 2 seconds
    GNGVisualise() #plot points
```
You should get something like
![screen](http://img405.imageshack.us/img405/2727/v15a.png)


