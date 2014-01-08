/*
* File: GNGServer.h
* Author: staszek
*
* Created on October 17, 2013, 8:12 PM
*/

#ifndef GNGSERVER_H
#define GNGSERVER_H

#include "GNGDefines.h"
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
#include "GNGGraph.h"
#include "GNGDatabase.h"
#include "GNGAlgorithm.h"

#include "Utils.h"


/** Holds together all logic and objects.*/
class GNGServer{
    /**Construct GNGServer using configuration*/
    GNGServer(GNGConfiguration configuration): listening_daemon_sleep_time(50){
        
        if(configuration.interprocess_communication) throw BasicException("Current version doesn't allow for crossprocess communication")
        
        this->current_configuration = configuration; //assign configuration
        
        DBG(10, "GNGServer()::constructing GNGServer");
        DBG(1, "GNGServer() dim = "+to_string(GNGNode::dim));

        /** Set up dimensionality **/
        GNGNode tmp;
        tmp.dim = current_configuration.dim;

        DBG(1, "GNGServer() dim = "+to_string(GNGNode::dim));


        
        /** Construct Shared Memory if allowed **/
//        if(crossprocess_communication)
//            this->shm = std::auto_ptr<SHMemoryManager>(new SHMemoryManager("Server"+to_string<int>(current_configuration.serverId)));
        if(current_configuration.graph_storage == GNGConfiguration::SharedMemory){
            throw BasicException("Not supported SharedMemory configuration");     
//            if(!crossprocess_communication) throw BasicException("SharedMemory configuration with switched off crossproces_communication");
//            this->shm->new_named_segment("GraphStorage",current_configuration.graph_memory_bound)
//            BoostSHMemoryAllocator * boostSHMemoryAllocator = new BoostSHMemoryAllocator(this->shm->get_named_segment("GraphStorage"));
//            SHGNGNode::mm = boostSHMemoryAllocator;
//            SHGNGNode::alloc_inst = new ShmemAllocatorGNG(this->shm->get_named_segment("GraphStorage")->get_segment_manager());
        }
        else if(current_configuration.graph_storage == GNGConfiguration::RAMMemory){
            //Nothing to do here
        }else{
            throw BasicException("Not supported GNGConfiguration type");
        }

//        if(configuration.interprocess_communication){
//            this->shm->new_named_segment("MessageBufor",current_configuration.message_bufor_size);
//            this->message_bufor_mutex = this->shm->get_named_segment("MessageBufor")->construct<
//                    boost::interprocess::interprocess_mutex>("MessageBuforMutex")();
//        }


        /** Construct database **/
        if(current_configuration.databaseType == GNGConfiguration::DatabaseProbabilistic){
                boost::shared_ptr<std::vector<GNGExampleProbabilistic> > g_database(new std::vector<GNGExampleProbabilistic>());
                this->gngDatabase = std::auto_ptr<GNGDatabase>(new GNGDatabaseProbabilistic<std::vector<GNGExampleProbabilistic, boost::mutex>
                        (&this->gngAlgorithmControl->database_mutex_thread, g_database, current_configuration.dim));
        }else if(current_configuration.databaseType == GNGConfiguration::DatabaseSimple){
            throw BasicException("Database type not supported");
        }


        DBG(10, "GNGServer()::gngDatabase and gngAlgorithmicControl constructed");

        /** Construct graph **/
        if(current_configuration.graph_storage == GNGConfiguration::SharedMemory){
            throw BasicException("Not supported SharedMemory configuration");
//            SharedMemoryGraphStorage * storage
//                    = this->shm->get_named_segment("GraphStorage")->
//                    construct<SharedMemoryGraphStorage >("storage")(GNGServer::START_NODES);
//            this->gngGraph = std::auto_ptr<ExtGNGGraph<SharedMemoryGraphStorage> >(
//                    new SHGNGGraph<SharedMemoryGraphStorage>(&this->gngAlgorithmControl->grow_mutex, storage,
//                    configuration.dim));
//            DBG(10, "GNGServer()::constructed shared graph");
        }
        else if(current_configuration.graph_storage == GNGConfiguration::RAMMemory){
            RAMMemoryGraphStorage * storage
                    = new RAMMemoryGraphStorage(GNGServer::START_NODES);
            this->gngGraph = std::auto_ptr<ExtGNGGraph<RAMMemoryGraphStorage> >(
                    new ExtGNGGraph<RAMMemoryGraphStorage>(&this->gngAlgorithmControl->grow_mutex, storage,
                    configuration.dim));           
        }else{
            throw BasicException("Not supported GNGConfiguration type");
        } 
        
        
        
        /** Initiliaze main computing object **/
        this->gngAlgorithm = std::auto_ptr<GNGAlgorithm>(new GNGAlgorithm
        ( this->gngGraph.get(), //I do not want algorithm to depend on boost
                this->gngDatabase.get(),
                &current_configuration.orig[0],
                &current_configuration.axis[0],
                current_configuration.axis[0]/4.0,
                current_configuration.max_nodes,
                current_configuration.max_age,
                current_configuration.alpha,
                current_configuration.beta,
                current_configuration.lambda,
                current_configuration.eps_v,
                current_configuration.eps_n,
                current_configuration.dim
        ));

        
             DBG(10, "GNGServer()::constructed algorithm object");
        


        //TODO: this should be moved to constructor
        this->gngAlgorithm->setToggleLazyHeap(current_configuration.lazyheap_optimization);
        this->gngAlgorithm->setToggleUniformGrid(current_configuration.uniformgrid_optimization);
    }


    /** Run GNG Server - runs in separate thread and returns control
* @note Runs one extra threads for communication.
*/
    void _run() {
//      boost::thread workerThread3(boost::bind(&GNGServer::runSHListeningDaemon, this));
        this->gngAlgorithmControl->setRunningStatus(true); //skrypt w R inicjalizuje
        DBG(10, "GNGServer::run::waiting for database");
        while (this->getDatabase()->getSize() < 200);
        DBG(10, "GNGServer::run::proceeding to algorithm");
        this->getAlgorithm()->runAlgorithm();
    }


    std::auto_ptr<GNGAlgorithm> gngAlgorithm;
    std::auto_ptr<GNGGraph > gngGraph;
    std::auto_ptr<GNGDatabase> gngDatabase;
    
    
//    int listening_daemon_sleep_time;
    static GNGConfiguration current_configuration;
    static boost::mutex static_lock;
//    boost::mutex message_bufor_mutex;
//    
//    /**
//    * Locking communication bufor in interprocess communication
//    * Deallocated in destructor
//    **/
//    boost::interprocess::interprocess_mutex * message_bufor_mutex;
//
//


    /**Section : protocol handling messages regardless of the source*/

    vector<double> _handle_getNode(int index){
        return vector<double>();
    }
    
    void _handle_AddExamples(double * examples, int count){
        //Handle coding
        if(current_configuration.databaseType==GNGConfiguration::DatabaseProbabilistic)
            for(int i=0;i<count;++i){
                GNGExampleProbabilistic ex(&examples[i*(current_configuration.dim+1)],current_configuration.dim); //decoding
                this->getDatabase()->addExample(&ex);
            }
        else if(current_configuration.databaseType==GNGConfiguration::DatabaseSimple)
             for(int i=0;i<count;++i){
                GNGExampleProbabilistic ex(&examples[i*current_configuration.dim],current_configuration.dim); //decoding
                this->getDatabase()->addExample(&ex);
            }
        else{
            DBG(100,"Not supported database type" );
            throw BasicException("Not supported database type");
        }
    }
    
public:
    void run() {
        boost::thread workerThread(boost::bind(&GNGServer::_run, this));
    }


    
    void addExamples(double * examples, int count){
        this->_handle_AddExamples(examples, count);
    }
    
    
    

    static void setConfiguration(GNGConfiguration configuration){
        GNGServer::current_configuration = configuration;
    }
    static GNGConfiguration getCurrentConfiguration(){
        return GNGServer::current_configuration;
    }

    /** Get singleton of GNGServer (thread safe) */
    static GNGServer& getInstance(){
        boost::mutex::scoped_lock sc(static_lock);
        static GNGServer gngServer(current_configuration);
        return gngServer;
    }




    //TODO: const reference not dandling pointer
    const GNGAlgorithm & getAlgorithm(){
        return *this->gngAlgorithm.get();
    }
    const GNGGraph & getGraph(){
        return *this->gngGraph.get();
    }
    const GNGDatabase & getDatabase(){
        return *this->gngDatabase.get();
    }
    
    
    
//
//    SHMemoryManager * getSHM(){
//        return this->shm.get();
//    }

    /**Run main processing loop for shared memory communication channel
     * @note: Not developed for now, until there is a need for crossprocess communication
     */
//    void runSHListeningDaemon(){
//        
//        DBG(12, "GNGServer:: runSHListeningDaemon");
//        
//        //TODO: add pause checking
//        while(true){
//           message_bufor_mutex->lock();
//
//           SHGNGMessage * current_message = this->getSHM()->get_named_segment("MessageBufor")->find_or_construct<SHGNGMessage>("current_message")();
//           int state = current_message->state;
//           if(state == SHGNGMessage::Waiting){
//                int type = current_message->type;
//
//                
//                DBG(12, "GNGServer::runListeningDaemon caught message of type "+to_string<int>(type));
//                
//
//                if(type == SHGNGMessage::AddExamples){
//                    SHGNGMessageAddExamples * message_params = this->
//                            getSHM()->get_named_segment("MessageBufor")->find<SHGNGMessageAddExamples>("current_message_params").first;
//
//
//                    if(!message_params){
//                        DBG(100, "GNGServer::runSHListeningDaemon not found message" );
//                        throw BasicException("GNGServer::runSHListeningDaemon not found message");
//                    }
//
//                    //note - this is quite specific coding
//                    double * examples = this->
//                            getSHM()->get_named_segment("MessageBufor")->find<double>(message_params->pointer_reference_name.c_str()).first;
//
//                    if(!examples){
//                        DBG(100, "GNGServer::runSHListeningDaemon not found examples to add" );
//                        throw BasicException("GNGServer::runSHListeningDaemon not found examples to add");
//                    }
//
//                    _handle_AddExamples(examples, message_params->count);
//
//                    this-> getSHM()->get_named_segment("MessageBufor")->destroy_ptr(examples);
//                    this-> getSHM()->get_named_segment("MessageBufor")->destroy_ptr(message_params);
//
//                }
//
//
//                current_message->state = SHGNGMessage::Processed;
//           }
//
//           message_bufor_mutex->unlock();
//
//           boost::this_thread::sleep(boost::posix_time::millisec(this->listening_daemon_sleep_time));
//        }
//    }

    ~GNGServer(){
//        this->shm->get_named_segment("MessageBufor")->destroy_ptr(this->message_bufor_mutex);
    }

};


#endif
        /* GNGSERVER_H */