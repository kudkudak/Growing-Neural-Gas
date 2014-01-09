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


    /** Run GNG Server - runs in separate thread and returns control
* @note Runs one extra threads for communication.
*/
    void _run() {
//      boost::thread workerThread3(boost::bind(&GNGServer::runSHListeningDaemon, this));
        DBG(10, "GNGServer::run::waiting for database");
        while (gngDatabase->getSize() < 200);
        gngAlgorithm->run();
        DBG(10, "GNGServer::run::proceeding to algorithm");
        gngAlgorithm->runAlgorithm();
    }


    /** Mutex used for synchronization in database*/
    boost::mutex database_mutex_thread;
    /** Mutex used for synchronization of graph access*/
    boost::mutex grow_mutex;
    
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
                gngDatabase->addExample(&ex);
            }
        else if(current_configuration.databaseType==GNGConfiguration::DatabaseSimple)
             for(int i=0;i<count;++i){
                GNGExampleProbabilistic ex(&examples[i*current_configuration.dim],current_configuration.dim); //decoding
                gngDatabase->addExample(&ex);
            }
        else{
            DBG(100,"Not supported database type" );
            throw BasicException("Not supported database type");
        }
    }
    
public:
    
    static GNGServer * constructTestServer(GNGConfiguration config){
        return new GNGServer(config);
    }
    
    
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
    GNGAlgorithm & getAlgorithm(){
        return *this->gngAlgorithm.get();
    }
     GNGGraph & getGraph(){
        return *this->gngGraph.get();
    }
     GNGDatabase & getDatabase(){
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