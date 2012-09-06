Growing Neural Gas in R
----------------------
Implementation of GNG algorithm in C++/Rcpp/R. This version should be stable, please e-mail me 
provided something bad occured :)
Alpha build, which means unfriendly building, and not fully implemented interface for the end user, 
but I reckon that it is already a practical tool for data analysis.

Installation
-----------
Because it is a very early version, building isnt straightforward. First of all you boost library installed
(for now - in default directory), of course R (with installed packages "multicore" "rgl" "e1071"). Having done that"
```bash
    make rcpp
```

Usage
-----
Please consult scripts/ files, yet i have to admit some are outdated, simple sanity-check:
```R
    source("gng.r") #load package
    GNGCreateServer() 
    #wait at least 1 second (booting)
    client<-new("GNGClient")
    client$addExamples(preset="sphere",preset_size=100000) #load sphere point distribution
    sv$runServer()
    #wait at least second
    GNGVisualise() #plot points
```

Further Work
------
Clearly all the code calls for robust documentation, so it will be the next step. Also I think i should include 
more examples