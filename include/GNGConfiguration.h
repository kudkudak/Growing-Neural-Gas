/* 
 * File:   GNGConfiguration.h
 * Author: staszek
 *
 * Created on October 17, 2013, 8:11 PM
 */

#ifndef GNGCONFIGURATION_H
#define	GNGCONFIGURATION_H

#include <vector>

/**
 * 
 * Configuration of GNG algorithm/server
 * TODO: add detailed description for parameters
 */
class GNGConfiguration{
public:
    GNGConfiguration(): dim(-1), serverId(-1){}


    enum GraphNodeStorage{
        NoneGraphNodeStorage,
        SharedMemory,
        RAMMemory
    } graph_storage;
    
    enum DatabaseType{
        NoneDatabaseType,
        DatabaseSimple,
        DatabaseProbabilistic
    };
    int message_bufor_size;
    /**Maximum number of nodes*/
    int max_nodes;//=1000;
    /**Uniform grid optimization*/
    bool uniformgrid_optimization;//=true,lazyheap=true;
    /**Lazy heap optimization*/
    bool lazyheap_optimization;
    /**Bounding box specification*/
    std::vector<double> orig;
    std::vector<double> axis;
    /**Max edge age*/
    int max_age;//=200;
    /**Alpha coefficient*/
    double alpha;//=0.95;
    /**Beta coefficient*/
    double beta;//=0.9995;
    /**Lambda coefficient*/
    double lambda;//=200;
    /**Epsilion v*/
    double eps_v;//=0.05;
    /**Memory bound*/
    int graph_memory_bound;
    /**Epsilion n*/
    double eps_n;//=0.0006;   
    
    /**Dimensionality of examples*/
    int dim;
    /**Type of used database*/
    DatabaseType databaseType;
    /**Id of the server*/
    int serverId;
    
    /**Initial reserve memory for nodes */
    int starting_nodes;
    
    bool interprocess_communication; /**< Should server listen for incommin connection from other processes? Not possible in the current version */
   
    
    /** Get default configuration of GNG Server */
    static GNGConfiguration getDefaultConfiguration(){
        GNGConfiguration default_configuration;
     
        default_configuration.starting_nodes = 100;
        
        default_configuration.message_bufor_size = 10000*sizeof(double);
        
        default_configuration.orig.push_back(0.0);
        default_configuration.orig.push_back(0.0);
        default_configuration.orig.push_back(0.0);
        
        default_configuration.axis.push_back(1.0);
        default_configuration.axis.push_back(1.0);
        default_configuration.axis.push_back(1.0);  
            
        default_configuration.graph_storage = RAMMemory;
        
        default_configuration.serverId = 0;
        default_configuration.dim = 3;
        default_configuration.databaseType = DatabaseSimple;
        default_configuration.max_nodes=1000;       
        default_configuration.uniformgrid_optimization=false; 
        default_configuration.graph_memory_bound = 200000*sizeof(double);
        
        default_configuration.lazyheap_optimization=false;       
        default_configuration.max_age=200;        
        default_configuration.alpha=0.95;     
        default_configuration.beta=0.9995;      
        default_configuration.lambda=200;
        default_configuration.eps_v=0.05;
        default_configuration.eps_n=0.0006; 
        
        default_configuration.interprocess_communication = false;
        
        return default_configuration;
    }
    
//    /**Validate server configuration. *Not working now**/
//    bool checkCorrectness(){
//        GNGConfiguration empty_configuration;
//        return gngDim!=empty_configuration.gngDim && 
//               serverId!=empty_configuration.serverId && 
//                databaseType != empty_configuration.databaseType;
//    }
//    
//    /**Python like update of configuration*/
//    void updateConfiguration(GNGConfiguration update){
//        //python dictionary is missing! 
//        GNGConfiguration empty_configuration;
//        
//        if(update.gngDim != empty_configuration.gngDim)
//            gngDim = update.gngDim;
//        
//        if(update.serverId != empty_configuration.serverId)
//            serverId = update.serverId;
//        
//        if(update.databaseType != empty_configuration.databaseType)
//            serverId = update.serverId; 
//    }
    
};

#endif	/* GNGCONFIGURATION_H */

