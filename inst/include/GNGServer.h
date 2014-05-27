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
#include <boost/thread/recursive_mutex.hpp>
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
    bool m_current_dataset_memory_was_set;
    bool m_running_thread_created;

    boost::thread collect_statistics_thread;
    boost::thread algorithm_thread;

    /** Mutex used for synchronization of graph access*/
    boost::recursive_mutex grow_mutex;
    /** Mutex used for synchronization of graph access*/
    boost::recursive_mutex database_mutex;
    /** Mutex used for synchronization of graph access*/
    boost::mutex stat_mutex;


    /** Singleton mutex*/
    static boost::mutex static_lock;

    std::auto_ptr<GNGAlgorithm> gngAlgorithm;
    std::auto_ptr<GNGGraph > gngGraph;
    std::auto_ptr<GNGDataset> gngDataset;

    static GNGConfiguration current_configuration;
public:
    /**Construct GNGServer using configuration*/
    GNGServer(GNGConfiguration * configuration_ptr);



    void run() {
    	if(!m_running_thread_created){
    		DBG(10, "GNGServer::runing algorithm thread");
    		algorithm_thread = boost::thread(boost::bind(&GNGServer::_run, this));
    		DBG(10, "GNGServer::runing collect_statistics thread");
    		collect_statistics_thread = boost::thread(boost::bind(&GNGServer::_collect_statics, this));

    		m_running_thread_created = true;
    	}else{
    		gngAlgorithm->run();
    	}
    }

    void setDebugLevel(int level){
    	dbg.set_debug_level(level);
    }

    
    static GNGServer * constructTestServer(GNGConfiguration config){
        return new GNGServer(&config);
    }
    


    ///Export graph to GraphML format
    void exportsToGraphML(std::string filename){
    	gngGraph->lock();
    	assert(filename != "");
    	writeToGraphML(getGraph(), filename);
    	gngGraph->unlock();
    }

    ///Insert examples
    void insertExamples(double * examples, unsigned int count, unsigned int size){
        this->_handle_addExamples(examples, count, size);
    }
    
    unsigned int getNumberNodes() const{
    	int nr = this->gngGraph->getNumberNodes();
    	return nr;
    }

    double getMeanError() const{
    	if(this->getNumberNodes() == 0) return 0.0;
    	double error = this->gngAlgorithm->CalculateAccumulatedError();
    	return error/(0.0+this->getNumberNodes());
    }

    //Error statistics cyclic queue

    unsigned int error_statistics_size;
    vector<double> error_statistics;
    unsigned int error_statistics_end;
    unsigned int error_statistics_start;
    unsigned int error_statistics_delay_ms;
    vector<double> getErrorStatistics(){
    	stat_mutex.lock();

        	vector<double> x;
        	x.reserve(error_statistics_size);

        	if(error_statistics_start!=error_statistics_end){
        		int idx = error_statistics_start;
        		while(idx != error_statistics_end){
        			x.push_back(error_statistics[idx]);
        			idx = (idx + 1)%error_statistics_size;
        		}
        	}


        stat_mutex.unlock();

        	return x;
        }


//#ifdef RCPP_INTERFACE

    SEXP m_current_dataset_memory; //will be deleted in ~delete
    ///Moderately slow function returning node descriptors
    Rcpp::List getNode(unsigned int gng_index){
    	gngGraph->lock();

    	if(!gngGraph->existsNode(gng_index)){
    		List ret;
    		return ret;
    	}

    	GNGNode & n = getGraph()[gng_index];
    	NumericVector pos(n.position, n.position + gngDataset->getGNGDim());

    	List ret;
    	ret["pos"] = pos;
    	ret["error"] = n.error;

    	vector<unsigned int> neigh(n.size());
    	GNGNode::EdgeIterator edg = n.begin();
    	unsigned i = 0;
    	while(edg!=n.end()){
    		neigh[i++] = (*edg)->nr;
    	    ++edg;
    	}

    	ret["neighbours"] = IntegerVector(neigh.begin(), neigh.end());

    	gngGraph->unlock();

    	return ret;
    }


    Rcpp::NumericVector RgetErrorStatistics(){
    	vector<double> x = getErrorStatistics();
    	return NumericVector(x.begin(), x.end());
    }
	void RinsertExamples(Rcpp::NumericMatrix &  ex){


		if(m_current_dataset_memory_was_set){
			throw "You cannot set example memory pool more than once!";
		}

		arma::mat * points = new arma::mat(ex.begin(), ex.nrow(), ex.ncol(), false);
		arma::inplace_trans( *points, "lowmem");
		this->_handle_addExamples(points->memptr(),(unsigned int)points->n_cols,
				(unsigned int)points->n_rows*points->n_cols);
	}
	void RsetExamples(Rcpp::NumericMatrix & ex){
		//Release previous if was present
		if(m_current_dataset_memory_was_set){
			throw "You cannot set example memory pool more than once!";
		}
		m_current_dataset_memory = wrap(ex);
		//We have to fix the object
		R_PreserveObject(wrap(ex));



		arma::mat * points = new arma::mat(ex.begin(), ex.nrow(), ex.ncol(), false);
		arma::inplace_trans( *points, "lowmem");
		this->_handle_addExamples(points->memptr(),(unsigned int)points->n_cols,
				(unsigned int)points->n_rows*points->n_cols, true);
	}

	void dumpMemory(){
		gngDataset->lock();
		for(int i=0; i<gngDataset->getSize()*gngDataset->getDataDim();++i){
			cout<<gngDataset->getMemoryPtr()[i]<<" ";
		}
		cout<<endl;
		gngDataset->unlock();
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
    	DBG(20, "GNGServer::collect_statistics interrupting");
    	collect_statistics_thread.interrupt();
//    	DBG(20, "GNGServer::collect_statistics joining");
//    	collect_statistics_thread.join();


    	DBG(20, "GNGServer::collect_statistics finished");
    	getAlgorithm().terminate();
    	DBG(20, "GNGServer::getAlgorithm terminated");
//    	algorithm_thread.join();
//    	DBG(20, "GNGServer::getAlgorithm joined");
    	//TODO: fix problems with joining interrupted thread ..
    	boost::this_thread::sleep(boost::posix_time::millisec(100));
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
    	DBG(10, "GNGServer::destructor called");
#ifdef RcppInterface

    	if(m_current_dataset_memory_was_set){
    		R_ReleaseObject(m_current_dataset_memory);

    	}
    	//R Matrix will be deleted from R level
#endif
//        this->shm->get_named_segment("MessageBufor")->destroy_ptr(this->message_bufor_mutex);
    }

private:

    /** Run GNG Server - runs in separate thread and returns control
    * @note Runs one extra threads for communication.
    */
    void _run() {
    	try{
			DBG(10, "GNGServer::run::proceeding to algorithm");
			gngAlgorithm->run();
			gngAlgorithm->runAlgorithm();
    	}
    	catch(std::exception & e){
    		cerr<<"GNGServer::failed _run with "<<e.what()<<endl;
    		DBG(10, e.what());
    	}
    }

    void _collect_statics(){
    	try{
    		while(!gngAlgorithm->running)
    			boost::this_thread::sleep(boost::posix_time::millisec(10));

    		DBG(10, "GNGServer::run::proceeding to collect_statistics");
			while(true){
				while(!stat_mutex.try_lock()){ //just to ensure no livelocks
					boost::this_thread::sleep(boost::posix_time::millisec(10));
				}

				unsigned int insert_place = (error_statistics_end+1) % error_statistics_size;
				error_statistics[insert_place] = this->getMeanError();
				error_statistics_end = insert_place;
				if(error_statistics_end == error_statistics_start)
					error_statistics_start = (error_statistics_start+1) % error_statistics_size;
				stat_mutex.unlock();

				boost::this_thread::sleep(boost::posix_time::millisec(error_statistics_delay_ms));
			}
    	}
    	catch(...){
    		//interrupted probably
    		cerr<<"GNGServer::stop collecting statistics\n";
    		DBG(10, "GNGServer::stop collecting statistics");
    	}

    }


    /**Section : protocol handling messages regardless of the source*/

    vector<double> _handle_getNode(int index){
        return vector<double>();
    }

    void _handle_addExamples(double * examples,unsigned int count, unsigned int size, bool set=false){
    	gngDataset->lock();

    	DBG(5, "GNGServer::Adding examples with "+to_string(gngDataset->getDataDim())+" dimensionality");

    	if(count*gngDataset->getDataDim() != size){
    		DBG(10, "Wrong dimensionality is "+to_string(size)+" expected "+
    				to_string(count*gngDataset->getDataDim()) + " data dim " + to_string(gngDataset->getDataDim()));
    		throw BasicException("Wrong dimensionality. "
    				"Check if you have added all field to "
    				"position (for instance probability)");
    	}



        //Handle coding
        DBG(1, "GNGServer::_handle_AddExamples adding examples");
        if(!set)
        	gngDataset->insertExamples(examples, count, size);
        else
        	gngDataset->setExamples(examples, count, size);

        int tmp = gngDataset->getSize();
        DBG(7, "GNGServer::Database size "+to_string(tmp));

        gngDataset->unlock();
    }

};


#endif
        /* GNGSERVER_H */
