/* 
 * File:   GNGServer.h
 * Author: staszek
 *
 * Created on October 17, 2013, 8:12 PM
 */

#ifndef GNGSERVER_H
#define	GNGSERVER_H

#include "GNGConfiguration.h"

#include <cstdlib> //std::system
#include <cstddef>
#include <map>
#include <exception>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>

#include <memory>

#include "GNGGlobals.h"
#include "GNGAlgorithmControl.h"
#include "GNGGraph.h"
#include "GNGDatabase.h"
#include "GNGAlgorithm.h"

#include "Utils.h"

/** Holds together all logic and objects.*/
class GNGServer{
    int listening_daemon_sleep_time;
    static GNGConfiguration current_configuration;
    static boost::mutex static_lock;
    static const int DEFAULT_MESSAGE_BUFOR = 1000000;

    /**Construct GNGServer using configuration*/
    GNGServer(GNGConfiguration configuration): listening_daemon_sleep_time(50){
        this->current_configuration = configuration; //assign configuration
        
        #ifdef DEBUG
             dbg.push_back(10, "GNGServer()::constructing GNGServer");
        #endif
        GNG_DIM = current_configuration.dim;

        boost::shared_ptr<std::vector<GNGExample> > g_database(new std::vector<GNGExample>());

        this->shm = std::auto_ptr<SHMemoryManager>(new SHMemoryManager("Server"+to_string<int>(current_configuration.serverId))); 
              
        if(current_configuration.graph_storage == GNGConfiguration::SharedMemory){
         
            #ifdef DEBUG
                 dbg.push_back(10, "GNGServer()::constructing graph storage segment");
            #endif           
            
            this->shm->new_named_segment("GraphStorage",current_configuration.graph_memory_bound);
            
            BoostSHMemoryAllocator * boostSHMemoryAllocator = new BoostSHMemoryAllocator(this->shm->get_named_segment("GraphStorage"));
                      
            SHGNGNode::mm =  boostSHMemoryAllocator;        
            SHGNGNode::alloc_inst = new ShmemAllocatorGNG(this->shm->get_named_segment("GraphStorage")->get_segment_manager());
        }
       
        this->shm->new_named_segment("MessageBufor",DEFAULT_MESSAGE_BUFOR * sizeof (double)); 

        this->gngAlgorithmControl = std::auto_ptr<GNGAlgorithmControl>(
                this->shm->get_named_segment("MessageBufor")->construct<GNGAlgorithmControl >("gngAlgorithmControl")()
                );
        
        if(current_configuration.databaseType == GNGConfiguration::DatabaseProbabilistic){
                this->gngDatabase = std::auto_ptr<GNGDatabase>(new GNGDatabaseProbabilistic<std::vector<GNGExample> ,boost::mutex>
                        (&this->gngAlgorithmControl->database_mutex_thread, g_database, current_configuration.dim));
        }
        
        #ifdef DEBUG
             dbg.push_back(10, "GNGServer()::gngDatabase and gngAlgorithmicControl constructed");
        #endif       
        if(current_configuration.graph_storage == GNGConfiguration::SharedMemory){
            this->gngGraph = std::auto_ptr<SHGNGGraph>(this->shm->get_named_segment("GraphStorage")->
                    construct<SHGNGGraph>("gngGraph")(&this->gngAlgorithmControl->grow_mutex,250));
            #ifdef DEBUG
                 dbg.push_back(10, "GNGServer()::constructed shared graph");
            #endif
        }
        /* Initiliaze main computing object */
        this->gngAlgorithm = std::auto_ptr<GNGAlgorithm>(new GNGAlgorithm
        (       *this->gngGraph.get(),  //I do not want algorithm to depend on boost
                this->gngDatabase.get(),  
                this->gngAlgorithmControl.get(),
                &current_configuration.orig[0], 
                &current_configuration.axis[0],
                current_configuration.axis[0]/4.0,
                current_configuration.max_nodes,
                current_configuration.max_age,
                current_configuration.alpha,
                current_configuration.beta,
                current_configuration.lambda,
                current_configuration.eps_v,
                current_configuration.eps_n
        ));
        
        #ifdef DEBUG
             dbg.push_back(10, "GNGServer()::constructed algorithm object");
        #endif
        
       
        //TODO: this should be moved to constructor
        this->gngAlgorithm->setToggleLazyHeap(current_configuration.lazyheap_optimization);
        this->gngAlgorithm->setToggleUniformGrid(current_configuration.uniformgrid_optimization);      
    }    
    
    
    /** Run GNG Server - runs in separate thread and returns control
     * @note Runs one extra threads for communication.
     */
    void _run() {
        //boost::thread workerThread3(boost::bind(&GNGServer::runSHListeningDaemon, gngServer));

        this->gngAlgorithmControl->setRunningStatus(true); //skrypt w R inicjalizuje

        #ifdef DEBUG
        dbg.push_back(10, "GNGServer::run::waiting for database");
        #endif       
        while (this->getDatabase()->getSize() < 200);
        #ifdef DEBUG
        dbg.push_back(10, "GNGServer::run::proceeding to algorithm");
        #endif
        this->getAlgorithm()->runAlgorithm();
        
    }
    
    //TODO: use unique_ptr here
    std::auto_ptr<GNGAlgorithmControl>  gngAlgorithmControl;
    std::auto_ptr<GNGAlgorithm> gngAlgorithm;
    std::auto_ptr<SHGNGGraph> gngGraph;
    std::auto_ptr<GNGDatabase> gngDatabase;
    std::auto_ptr<SHMemoryManager> shm;
    
public:
    void run() {
        REPORT("run");
        boost::thread workerThread(boost::bind(&GNGServer::_run, this));     
    }    
    static void setConfiguration(GNGConfiguration configuration){
        GNGServer::current_configuration = configuration;
    }   
    
    /** Get singleton of GNGServer (thread safe) */
    static GNGServer& getInstance(){
        boost::mutex::scoped_lock sc(static_lock);
        static GNGServer gngServer(current_configuration);
        return gngServer;
    }
    

    
    boost::interprocess::interprocess_mutex communication_bufor_mutex; /** Locking communication bufor in interprocess communication */

    //TODO: const reference not dandling pointer
    
    GNGAlgorithmControl * getAlgorithmControl(){
        return this->gngAlgorithmControl.get();
    }
    GNGAlgorithm * getAlgorithm(){
        return this->gngAlgorithm.get();
    }
    SHGNGGraph * getGraph(){
        return this->gngGraph.get();
    }
    GNGDatabase * getDatabase(){
        return this->gngDatabase.get();
    }
 
    SHMemoryManager * getSHM(){
        return this->shm.get();
    }    
    
    /**Run main processing loop for shared memory communication channel*/
    void runSHListeningDaemon(){
        while(true){
           communication_bufor_mutex.lock();
           
           int status = *this->getSHM()->get_named_segment("MessageBufor")->find<int>("GNGMessageState").first;
           if(status == SHGNGMessage::Waiting){
                int type = *this->getSHM()->get_named_segment("MessageBufor")->find<int>("GNGMessageType").first;
                #ifdef DEBUG
                dbg.push_back(12, "GNGServer::runListeningDaemon caught message of type "+to_string<int>(type));
                #endif

                *this->getSHM()->get_named_segment("MessageBufor")->find<int>("GNGMessageState").first = SHGNGMessage::Processed;
           }
          
           communication_bufor_mutex.unlock();
            
           boost::this_thread::sleep(boost::posix_time::millisec(this->listening_daemon_sleep_time)); 
        }
    }
    
    ~GNGServer(){
        //Nothing to do here
    }
    
};

#endif	/* GNGSERVER_H */

