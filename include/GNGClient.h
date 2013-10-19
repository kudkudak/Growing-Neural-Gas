/* 
 * File:   GNGClient.h
 * Author: staszek
 *
 * Created on October 17, 2013, 8:14 PM
 */

#ifndef GNGCLIENT_H
#define	GNGCLIENT_H

#include "GNGConfiguration.h"


/** Handles communication with server through different communication channels */
class GNGClient{
    GNGClient(){}
    std::string process_identifier;
public:
    GNGClient(std::string process_identifier): process_identifier(process_identifier){
    
    
    }
    
    
    void addExamples(GNGConfiguration::DatabaseType databaseType, double * examples_encoding){
        
    }
};


#endif	/* GNGCLIENT_H */

