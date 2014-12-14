
devtools::install(".")
devtools::load_all(".")


.gng.type.optimized = 0
.gng.type.utility = 1
.gng.type.default = 2
.gng.train.online = 1
.gng.train.offline = 0

gng.type.default <- c(.gng.type.default)

gng.type.optimized <- function(minimum=0, maximum=10){
  c(.gng.type.optimized, minimum, maximum)
}

gng.type.utility<- function(k=1.3){
  c(.gng.type.utility, k)
}

gng.train.online <- c(.gng.train.online)



gng.train.offline <- function(max_iter = 100, min_relative_dif = 1e-2){
  if(max_iter<7){
    gmum.error(ERROR, "Please pass at least 7 iterations")
  }
  c(.gng.train.offline, max_iter, min_relative_dif)
}

x<- list(1,"2",3)
x[1]/2

GNG2 <- function(x=NULL, 
                 beta=0.99, 
                 alpha=0.5, 
                 max_nodes=1000, 
                 eps_n=0.0006, 
                 eps_w= 0.05, 
                 max_edge_age = 200, 
                 type = gng.type.default,
                 training = gng.train.offline(),
                 lambda=200,
                 verbosity=0
                 
){

  config <- new(GNGConfiguration)
  
  # Fill in configuration
  if(training[1] == .gng.train.offline){
    config$dim = ncol(x)
  }else{
    config$dim = training(2)
  }

  if(type[1] == .gng.type.optimized){
    config$uniformgrid_optimization = TRUE
    config$lazyheap_optimization = TRUE  
    config$setBoundingBox(type(2), type(3))
    
    if(training[1] == .gng.train.offline){
      if(!max(df) <= type(3) && !min(df) >= type(2)){
        gmum.error("Passed incorrect parameters. The dataset is not in the defined range")
      }
    }
    
  }else{
    config$uniformgrid_optimization = FALSE
    config$lazyheap_optimization = FALSE
  }
  
  if(type[1] == .gng.type.utility){
    config$experimental_utility_k = type(2)
    config$experimental_utility_option = 1
  }
  else{
    config$experimental_utility_option = 0
  }

  config$dataset_type=gng.dataset.sequential
  config$beta = beta
  config$max_edge_age = max_edge_age
  config$alpha = alpha  
  config$max_nodes = max_nodes
  config$eps_n = eps_n
  config$eps_w = eps_w

  config$lambda = lambda
  config$verbosity = verbosity
  
  if(!config$check_correctness()){
    gmum.error(ERROR_BAD_PARAMS, "Passed incorrect parameters.")
  }
 
  # Construct server
  server = new(GNGServer, config)
  
  # Perform training on passed dataset
  if(training[1] == .gng.train.offline){
    print("Training offline")
    if(is.null(x)){
        gmum.error(ERROR, "Passed null data and requested training offline")
    }else{
        insert_examples(server, x)
        run(server)
      
        max_iter = training[2]
        print(max_iter)
        min_relative_dif = training[3]
        iter = 0
        errors_calculated = 0
        while(iter < max_iter || errors_calculated == 0){
          Sys.sleep(0.1)
          iter = server$get_current_iteration()

          if(iter %% (max_iter/100) == 0){    
            print(paste("Iteration", iter))
          }

          # Iter 5 = 5 times passed whole dataset. 
          if(iter > 5){
              errors_calculated = 1
              errors = server$get_error_statistics()
              best_previously = min(errors[(length(errors)-5):length(errors)-1])
              current = errors[length(errors)]
              if(best_previously != 0){
                change = 1.0 - current/best_previously
                if(change < min_relative_dif){
                  break
                }
              }
          }
        }
      
    }
  }
  
  server
}
x = c(1,2,3,4,5)
g$get_current_iteration()


min(x[(length(x)-4):length(x)])

data(wine, package="rattle")
head(wine)
df <- scale(wine[-1])

summary(df)

?GrowingNeuralGas::GNG
# check that we get mean of 0 and sd of 1

dferrors = apply(as.matrix(df), MARGIN = 2, FUN = function(X) (X - min(X))/diff(range(X)))

max(df)

colMeans(df )  # faster version of apply(scaled.dat, 2, mean)
apply(df, 2, sd)
apply(df, 2, mean)
summary(df)

summary(df)


g <- GNG2(as.matrix(df), max_nodes=20)

insert_examples(g, df)

nrow(df)

df <- scale(df)

colMeans(df)

run(g)
summary(g)


summary(df)

colMeans(df)

number_nodes(g)

error_statistics(g)

ncol(df)



pause(g)

plot(g, mode = gng.plot.2d, vertex.color=gng.plot.color.cluster, layout=igraph::layout.fruchterman.reingold)
