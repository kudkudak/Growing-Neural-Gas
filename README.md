#Growing Neural Gas in R

Package to be included in GMuM.R project. Currently under heavy rewritting.

###General
------

Fast implementation of the GNG algorithm in C++/Rcpp/R (plot compares its performance with the original implementation from Fisher's paper). This algorithms is widely used for dynamic clustering problems, often occuring for example in robotics. This version should be stable.
Alpha build, which means unfriendly building, and not fully implemented interface for the end user, 
but I reckon that it is already consitutes a practical tool for data analysis.



<center><img src="https://raw.github.com/kudkudak/Growing-Neural-Gas/master/screens/plot_speed.png" width="50%"></img></center>


###Installation
-----------
Because it is a very early version, building isnt straightforward. You need to have installed libboost libraries and necessary packages in R (i.e. rgl, parallel). Now go to the reposity directory and enter:

```bash
    make rcpp
```

<br>

##Examples
-----

Growing Neural Gas is a clustering algorithm. For theoretical details please see (in polish) this file [growing-neural-gas.pdf](https://www.dropbox.com/s/glol7j54qg2vnuh/praca_roczna_draft.pdf). Algorithm in action:

<small>Plane triangulation</small>:
<center><img src="https://raw.github.com/kudkudak/Growing-Neural-Gas/master/screens/fig11.png" width="70%" height="70%"></img></center>

Bone reconstruction from a MRI scan:
<center><img src="https://raw.github.com/kudkudak/Growing-Neural-Gas/master/screens/1.png" width="60%"></img></center>

Reconstruction of the Buddha figure from Standford Repositories
<center><img src="https://raw.github.com/kudkudak/Growing-Neural-Gas/master/screens/fig9_both.png" width="70%"></img></center>





##Scripts
------

* Load sample data
```Matlab
    source("gng.r") #load package
    GNGCreateServer() 
    #wait at least 2 seconds (booting)
    sv<-new("GNGClient")
    sv$addExamples(preset="sphere",preset_size=100000) #load sphere point distribution
    sv$runServer()
    #wait at least 2 seconds
    GNGVisualise() #plot points
```

And you should get something like this:
<center><img src="http://img405.imageshack.us/img405/2727/v15a.png" width="70%" height="70%"></img></center>

* Load obj file
```R
source("gng.r")
GNGSetParams(max_nodes=25000)
GNGCreateServer() #notice that servers runs in a parallel manner
Sys.sleep(2.0)
sv<-new("GNGClient")
sv$loadObj("data/models/buddha2.obj") #provided you have got model here
sv$run()
iteration<-0
GNGVisualise()
sv$getAccumulatedError() #retrieve error value
sv$getNumberNodes() #get nodes
```

For more scripts, please check the 'scripts' directory. 


#Testing
-----
To run tests you will need Google C++ Test Framework
installed to your search paths.

See instructions:
http://code.google.com/p/tonatiuh/wiki/InstallingGoogleTestForLinux

#Contributors
---------
Feel free to contribute to the code. Contributions should be posted as pull requests. For now
there is not automatic testing, so make sure that tests located in tests/ folder are passing. 

Please use Google C++ naming convention: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml


