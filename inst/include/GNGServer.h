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
#include "GNGDataset.h"
#include "GNGAlgorithm.h"

#include "Utils.h"


//#ifdef RCPP_INTERFACE

#include <Rcpp.h>
#include <armadillo>


using namespace Rcpp;
using namespace arma;

//#endif



/** Holds together all logic and objects.*/
class GNGServer{
public:
    /**Construct GNGServer using configuration*/
    GNGServer(GNGConfiguration * configuration_ptr);


    void run() {
        boost::thread workerThread(boost::bind(&GNGServer::_run, this));
    }

    
    static GNGServer * constructTestServer(GNGConfiguration config){
        return new GNGServer(&config);
    }
    

    ///Export graph to GraphML format
    void exportsToGraphML(std::string filename){
    	assert(filename != "");
    	writeToGraphML(getGraph(), filename);
    }

    ///Insert examples
    void insertExamples(double * examples, unsigned int count, unsigned int size){
        this->_handle_InsertExamples(examples, count, size);
    }
    
//#ifdef RCPP_INTERFACE
	void RinsertExamples(Rcpp::NumericMatrix & ex){
		arma::mat points(ex.begin(), ex.nrow(), ex.ncol(), false);
		arma::inplace_trans( points, "lowmem");
		cout<<"Examples "<<points.n_rows<<endl;
		this->insertExamples(points.memptr(),(unsigned int)points.n_cols, (unsigned int)points.n_rows*points.n_cols);
	}
//#endif

    ///Calculate error per node
    double calculateAvgErrorNode() {
    	return this->getAlgorithm().CalculateAccumulatedError()/(this->getGraph().getNumberNodes()+0.0f);
    }
    
    ///Pause algorithm
    void pause(){
    	getAlgorithm().pause();
    }

    ///Terminate algorithm
    void terminate(){
    	getAlgorithm().terminate();
    }

    GNGConfiguration getConfiguration(){
    	//TODO: don't use singleton here
    	return GNGServer::current_configuration;
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
        static GNGServer gngServer(&current_configuration);
        return gngServer;
    }




    //TODO: const reference not dandling pointer
    GNGAlgorithm & getAlgorithm(){
        return *this->gngAlgorithm.get();
    }
     GNGGraph & getGraph(){
        return *this->gngGraph.get();
    }
     GNGDataset & getDatabase(){
        return *this->gngDataset.get();
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

private:
    /** Mutex used for synchronization of algorithm with other modules*/
    boost::mutex alg_memory_lock;
    /** Mutex used for synchronization of graph access*/
    boost::mutex grow_mutex;



    /** Singleton mutex*/
    static boost::mutex static_lock;

    std::auto_ptr<GNGAlgorithm> gngAlgorithm;
    std::auto_ptr<GNGGraph > gngGraph;
    std::auto_ptr<GNGDataset> gngDataset;

    static GNGConfiguration current_configuration;



    /** Run GNG Server - runs in separate thread and returns control
    * @note Runs one extra threads for communication.
    */
    void _run() {

        DBG(10, "GNGServer::run::proceeding to algorithm");
        gngAlgorithm->run();
        gngAlgorithm->runAlgorithm();

    }


    /**Section : protocol handling messages regardless of the source*/

    vector<double> _handle_getNode(int index){
        return vector<double>();
    }

    void _handle_InsertExamples(double * examples,unsigned int count, unsigned int size){
    	DBG(5, "GNGServer::Adding examples with "+to_string(gngDataset->getDataDim())+" dimensionality");

    	if(count*gngDataset->getDataDim() != size)
    		throw BasicException("Wrong dimensionality. "
    				"Check if you have added all field to "
    				"position (for instance probability)");




        //Handle coding
        DBG(1, "GNGServer::_handle_AddExamples adding examples");
        gngDataset->insertExamples(examples, count, size);
        int tmp = gngDataset->getSize();
        DBG(7, "GNGServer::Database size "+to_string(tmp));
    }

};


#endif
        /* GNGSERVER_H */
