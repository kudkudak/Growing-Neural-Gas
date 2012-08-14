#include "RcppInterface.h"

#include "Utils.cpp"

using namespace boost::interprocess;
using namespace std;
using namespace arma;
using namespace Rcpp;


DebugCollector dbg;
GNGGraphInfo * ggi;
int GNGExample::N=3; 
RcppExport SEXP GNGClient__new(){
   //GNGExample::N=3; g
    
    
    
   managed_shared_memory *  mshm1= new managed_shared_memory(open_only,"SHMemoryPool_Segment1");
   managed_shared_memory *  mshm2= new managed_shared_memory(open_only,"SHMemoryPool_Segment2");
   
   MyMutex * grow_mutex = (mshm1->find< MyMutex >("grow_mutex")).first;		
   MyMutex * database_mutex = (mshm2->find< MyMutex >("database_mutex")).first;	
   SHGNGExampleDatabase * database_vec = (mshm2->find< SHGNGExampleDatabase >("database_vec")).first;	
   
   grow_mutex->lock();

   GNGGraphInfo * ggi2 = (mshm1->find<GNGGraphInfo >("SHGraphInfo")).first; //musi byc stworzony : kolejne miejsce na blad !
   
   
   GNGClient * gngClient = new GNGClient();

   
   gngClient->ggi = ggi2;
   gngClient->g_database = new GNGDatabaseSimple(database_mutex, database_vec);
   

   gngClient->grow_mutex = grow_mutex;

   Rcpp::XPtr<GNGClient> ptr(gngClient,true);


   
   grow_mutex->unlock();

   return wrap(ptr); 
}

//czyta 3 node

RcppExport SEXP GNGClient__updateBuffer(SEXP _xp){
    
    
    Rcpp::XPtr<GNGClient> ptr(_xp);

    ptr->grow_mutex->lock();
  
    int a = ptr->ggi->nodes;
  
    ptr->grow_mutex->unlock();     
    
    return wrap(0);    
}

RcppExport SEXP GNGClient__getNumberNodes(SEXP _xp){
    
    Rcpp::XPtr<GNGClient> ptr(_xp);

    ptr->grow_mutex->lock();
  
    int a = ptr->ggi->nodes;
  
    ptr->grow_mutex->unlock();     
    
   
    
    return wrap(a);
}


RcppExport SEXP GNGClient__getNode(SEXP _xp, SEXP _nr){

    Rcpp::XPtr<GNGClient> ptr(_xp);
    int nr = as<int>(_nr);
    
    ptr->grow_mutex->lock();

  
    int a = 1;
 
    GraphAccess & graph = *(ptr->readGraph());
    
 
    GNGNode * requested_node = graph[nr];
 
    int edg = requested_node->edgesCount;
    
    Rcpp::NumericVector node(edg+3);
    
    for(int i=0;i<3;++i) {
        node[i]=requested_node->position[i];
    }

    if(edg>0){
      GNGEdge * first = graph.getFirstEdge(nr);
    
      for(int i=0;i<edg;++i){
          node[i+3] = first->nr;
          ++first;
      }  
    }
    

   // memcpy(pos,&(graph[2]->position[0]),3*sizeof(double));

    ptr->grow_mutex->unlock();     
    
   
    
    return wrap(node);
}

RcppExport SEXP GNGClient__addExamples(SEXP _xp, SEXP _examples){
    Rcpp::XPtr<GNGClient> ptr(_xp);
    Rcpp::NumericMatrix examples(_examples);
    cout<<examples.nrow()<<","<<examples.ncol()<<endl;

    ptr->grow_mutex->lock();
    double * pos = new double[3];
    for(int i=0;i<examples.nrow();++i){
        for(int j=0;j<3;++j) pos[j] = (double)(examples(i,j));
        GNGExample ex(&pos[0]);
        ptr->g_database->addExample(&ex);
    }

    
    ptr->grow_mutex->unlock();   
    
    return wrap(1);
}
