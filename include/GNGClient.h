/*
* File: GNGClient.h
* Author: Stanislaw "kudkudak" Jastrzebski <grimghil[at]gmail[dot]com>
 * 
 * @note: This file is currently not developed, until there is a need for communication from other processes
*
* Created on October 17, 2013, 8:14 PM
*/

#ifndef GNGCLIENT_H
#define        GNGCLIENT_H

#include "Utils.h"
#include "GNGConfiguration.h"
#include "GNGDefines.h"
#include <boost/interprocess/managed_shared_memory.hpp>
#include <memory>
#include "SHMemoryManager.h"



/** Handles communication with server through different communication channels */
class GNGClient{
    
    
    GNGClient(){}
    std::string server_identifier;
    boost::interprocess::managed_shared_memory * message_buffer; //dangling pointer is fine here, it has no ownership
    GNGConfiguration server_configuration;
    boost::interprocess::interprocess_mutex * message_buffer_mutex;
public:
    enum GNGServerClientConnection{
        SharedMemory
    } connection_type;
    
    /**
* Create instance of GNGClient
* @param server_identifier It is server identifier, that is enough information to connect to connect
* @param server_configuration Current configuration of server (has to be updated in case of severe changes)
*/
    GNGClient(std::string server_identifier, GNGConfiguration server_configuration):
                server_identifier(server_identifier),
                server_configuration(server_configuration)
    {
         message_buffer = //create shared memory segment reference
                 new boost::interprocess::managed_shared_memory(boost::interprocess::open_only,
                        SHMemoryManager::generate_name(server_identifier,"MessageBufor").c_str())
                ;
         
         if(!message_buffer) throw BasicException("GNGClient::GNGClient FATAL not found message_buffer shared memory segment!!");
         
         message_buffer_mutex = this->message_buffer->find<boost::interprocess::interprocess_mutex>("MessageBuforMutex").first;
         
        #ifdef DEBUG
         dbg.push_back(9,"GNGClient::Successfuly attached GNGClient");
        #endif
    }
    
    
    /**
*
* @param databaseType
* @param examples_encoding concatenation of casted GNGExamples to double array. Double array can be stored in shared memory. Should be
* acquired using GNGExample->getDoubleEncoding()
* @param examples_count
* @param encoding_length How many doubles per example. GNGExample->getDoubleEncodingLenght()
*/
    void addExamples(const double * examples, int examples_count, int encoding_length){
        
        std::size_t bytes = sizeof(double)*examples_count*encoding_length;
        
        if(this->message_buffer->get_size()-100*sizeof(double) < bytes){
            throw BasicException("GNGClient::addExamples examples do not fit into shared memory segment");
        }
        
        double * ptr = message_buffer->construct<double>("GNGExamples")[examples_count*encoding_length]();
        
        #ifdef DEBUG
        dbg.push_back(4, "GNGClient::addExamples constructed array in shared memory");
        #endif

        memcpy(ptr, examples, bytes);
        
       
        if(server_configuration.databaseType == GNGConfiguration::DatabaseProbabilistic){
            
        }else if(server_configuration.databaseType == GNGConfiguration::DatabaseSimple){
            
        }else{
            throw BasicException("GNGClient::addExamples Not supported database type");
        }
        
        
        SHGNGMessageAddExamples message_params;
        message_params.count = examples_count;
        message_params.pointer_reference_name = "GNGExamples";
        
        //post message
        this->message_buffer_mutex->lock();

        SHGNGMessage * current_message = this->message_buffer->find_or_construct<SHGNGMessage>("current_message")();

        if(current_message->state == SHGNGMessage::Processed || current_message->state == SHGNGMessage::NoState){
            current_message->state = SHGNGMessage::Waiting;
            current_message->type = SHGNGMessage::AddExamples;
            
            SHGNGMessageAddExamples * params=this->message_buffer->find_or_construct<SHGNGMessageAddExamples>("current_message_params")();
            *params = message_params;

        }

        message_buffer_mutex->unlock();
        
        #ifdef DEBUG
        dbg.push_back(2, "GNGClient::addExamples finished successfully");
        #endif
        
    }
};


#endif        /* GNGCLIENT_H */