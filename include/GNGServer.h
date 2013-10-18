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

#include "GNGGlobals.h"
#include "GNGAlgorithmControl.h"
#include "GNGGraph.h"
#include "GNGDatabase.h"
#include "GNGAlgorithm.h"

/** Holds together all logic and objects.*/
class GNGServer{
    int listening_daemon_sleep_time;
    static GNGConfiguration current_configuration;
    static boost::mutex static_lock;
    
public:
    
    static void setConfiguration(GNGConfiguration configuration){
        GNGServer::current_configuration = configuration;
    }    
    static GNGServer& getInstance(){
        boost::mutex::scoped_lock sc(static_lock);
        static GNGServer gngServer(current_configuration);
        return gngServer;
    }
    
    /**OBSOLETE: Create GNGServer (and fill it manually after that)*/
    GNGServer(): listening_daemon_sleep_time(50){}
    /**Construct GNGServer using configuration*/
    GNGServer(GNGConfiguration configuration): listening_daemon_sleep_time(50){
        
        
    }
    
    boost::interprocess::interprocess_mutex communication_bufor_mutex; /** Locking communication bufor in interprocess communication */
    
    GNGAlgorithmControl * gngAlgorithmControl;
    GNGAlgorithm * gngAlgorithm;
    SHGNGGraph * gngGraph;
    GNGDatabase* gngDatabase;
    SHMemoryManager *shm;
    
    GNGAlgorithmControl * getAlgorithmControl(){
        return this->gngAlgorithmControl;
    }
    GNGAlgorithm * getAlgorithm(){
        return this->gngAlgorithm;
    }
    SHGNGGraph * getGraph(){
        return this->gngGraph;
    }
    GNGDatabase * getDatabase(){
        return this->gngDatabase;
    }
 
    SHMemoryManager * getSHM(){
        return this->shm;
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
    
    
};

#endif	/* GNGSERVER_H */

