/* 
 * File:   GNGConfiguration.h
 * Author: staszek
 *
 * Created on October 17, 2013, 8:11 PM
 */

#ifndef GNGCONFIGURATION_H
#define	 GNGCONFIGURATION_H

#ifdef RCPP_INTERFACE
using namespace Rcpp;
#endif

#include <vector>



	/**
	 *
	 * Configuration of GNG algorithm/server
	 * TODO: add detailed description for parameters
	 */
	class GNGConfiguration{
    public:
		enum GraphNodeStorage{
			NoneGraphNodeStorage,
			SharedMemory,
			RAMMemory
		} graph_storage;


		enum DatasetType{
			NoneDatasetType,
			DatasetSeq,
			DatasetSampling,
			DatasetSamplingProb
		};

		enum ExperimentalUtility{
			UtilityOff,
			UtilityBasicOn
		};

		///Utility constant
		double experimental_utility_k;

		///Utility option. Currently supported simples utility
		int experimental_utility_option;

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
		/**Epsilion v. How strongly move winning node*/
		double eps_w;//=0.05;
		/**Memory bound*/
		int graph_memory_bound;
		/**Epsilion n*/
		double eps_n;//=0.0006;


		/**Pseudodistance function used (might be non metric)*/
		int distance_function;

		/**Dimensionality of examples*/
		int dim;
		/**Type of used database, unsgined int for compabititlity with Rcpp**/
		unsigned int datasetType;
		/**Id of the server*/
		int serverId;

		/**Initial reserve memory for nodes */
		int starting_nodes;

		bool interprocess_communication; /**< Should server listen for incommin connection from other processes? Not possible in the current version */

		///Load serialized algorithm state from this file
		std::string load_graph_filename;


		///Dimensionality of vertex extra data (will be possible to vote on this data among vertices). Not suported yet.
		unsigned int dataset_vertex_extra_dim;

	public:


		GNGConfiguration(){
			//Not reusing code because of peculiar problem with Rcpp
			dataset_vertex_extra_dim = 0;

			starting_nodes = 100;

			message_bufor_size = 10000*sizeof(double);

			orig.push_back(0.0);
			orig.push_back(0.0);
			orig.push_back(0.0);

			axis.push_back(1.0);
			axis.push_back(1.0);
			axis.push_back(1.0);

			experimental_utility_option = (int)UtilityOff;
			experimental_utility_k = 1.5;

			graph_storage = RAMMemory;

			serverId = 0;
			dim = 3;
			datasetType = DatasetSampling;
			max_nodes=1000;
			uniformgrid_optimization=false;
			graph_memory_bound = 200000*sizeof(double);

			load_graph_filename = "";

			lazyheap_optimization=false;
			max_age=200;
			alpha=0.95;
			beta=0.9995;
			lambda=200;
			eps_w=0.05;
			eps_n=0.0006;

			distance_function = gmum::GNGGraph::Euclidean;


            interprocess_communication = false;
		}

#ifdef RCPP_INTERFACE
		void setUniformGridAxis(NumericVector v){
			axis.clear();
			for(int i=0;i<v.size();++i){
				axis.push_back(v(i));
			}
		}
		NumericVector getUniformGridAxis(){
			return NumericVector(axis.begin(), axis.end());
		}
		void setUniformGridOrigin(NumericVector v){
			orig.clear();
			for(int i=0;i<v.size();++i){
				orig.push_back(v(i));
			}
		}

		NumericVector getUniformGridOrigin(){
			return NumericVector(orig.begin(), orig.end());
		}
#endif

		/** Get default configuration of GNG Server */
		static GNGConfiguration getDefaultConfiguration(){
			GNGConfiguration default_configuration;
			return default_configuration;
		}

		/**Validate server configuration. *Not working now**/
		bool check_correctness(){
			if(experimental_utility_option != UtilityOff &&
					(uniformgrid_optimization || lazyheap_optimization)){
				cerr<<"ERROR: please turn OFF optimization when using experimental utility option\n";
				DBG(20, "ERROR: please turn OFF optimization when using experimental utility option\n");
				return false;
			}

			if(datasetType > 3 or datasetType <= 0){
				cerr<<"ERROR: wrong database specified\n";
				DBG(20, "ERROR: wrong database specified\n");
				return false;
			}
			if(! (dim < 20 || ! uniformgrid_optimization)){
				DBG(20, "ERROR: Too big dimensionality for uniformgrid_optimization");
				cerr<<"ERROR: Too big dimensionality for uniformgrid_optimization\n";
				return false;
			}
			if(! (distance_function!=gmum::GNGGraph::Euclidean || ! uniformgrid_optimization)){
				DBG(20, "ERROR: You can use only Euclidean distance function with uniformgrid optimization");
				cerr<<"ERROR: You can use only Euclidean distance function with uniformgrid optimization\n";
				return false;
			}
			if(! (!uniformgrid_optimization or (dim == axis.size() && dim == orig.size()))){
				DBG(20, "ERROR: dimensionality doesn't agree with axis and orig");
				cerr<<"ERROR: dimensionality doesn't agree with axis and orig"<<endl;
				return false;
			}

			return true;
		}
	};
#endif	/* GNGCONFIGURATION_H */

