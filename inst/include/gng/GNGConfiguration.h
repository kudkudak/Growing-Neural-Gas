/* 
 * File:   GNGConfiguration.h
 * Author: staszek
 *
 * Created on October 17, 2013, 8:11 PM
 */

#ifndef GNGCONFIGURATION_H
#define	 GNGCONFIGURATION_H

#ifdef RCPP_INTERFACE
#include <RcppArmadillo.h>
using namespace Rcpp;
#endif

#include "utils/utils.h"
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
			NoneDatasetTypeinit,
			DatasetSeq,
			DatasetSampling,
			DatasetSamplingProb
		};

		enum ExperimentalUtility{
			UtilityOff,
			UtilityBasicOn
		};


		/**Maximum number of nodes*/
		int max_nodes;//=1000;
		/**Uniform grid optimization*/
		bool uniformgrid_optimization;//=true,lazyheap=true;
		/**Lazy heap optimization*/
		bool lazyheap_optimization;
		/**Bounding box specification*/


		/**Dimensionality of examples*/
		int dim;


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

		int verbosity;

		/**Pseudodistance function used (might be non metric)*/
		int distance_function;

		/**Type of used database, unsgined int for compabititlity with Rcpp**/
		unsigned int datasetType;

		/**Initial reserve memory for nodes */
		int starting_nodes;

		///Utility constant
		double experimental_utility_k;

		///Utility option. Currently supported simples utility
		int experimental_utility_option;

	public:


		GNGConfiguration(){

			verbosity = 10;

			starting_nodes = 100;

			experimental_utility_option = (int)UtilityOff;
			experimental_utility_k = 1.5;

			graph_storage = RAMMemory;

			dim = 3;
			setBoundingBox(0, 1);

			datasetType = DatasetSampling;
			max_nodes=1000;
			uniformgrid_optimization=false;
			graph_memory_bound = 200000*sizeof(double);

			lazyheap_optimization=false;
			max_age=200;
			alpha=0.95;
			beta=0.9995;
			lambda=200;
			eps_w=0.05;
			eps_n=0.0006;

			distance_function = gmum::GNGGraph::Euclidean;


		}


		void deserialize(std::istream & in){
			///Utility constant
			in >> experimental_utility_k;

			///Utility option. Currently supported simples utility
			in >> experimental_utility_option;

			/**Maximum number of nodes*/
			in >>  max_nodes;//=1000;
			/**Uniform grid optimization*/
			in >>  uniformgrid_optimization;//=true,lazyheap=true;
			/**Lazy heap optimization*/
			in >>  lazyheap_optimization;
			/**Bounding box specification*/

			/**Dimensionality of examples*/
			in >>  dim;

			REPORT(dim);

			orig = vector<double>(dim, 0);
			axis = vector<double>(dim, 0);

			for(int i=0;i<dim;++i){
				in >>axis[i]>>orig[i];
			}
			/**Max edge age*/
			in >>  max_age;//=200;
			/**Alpha coefficient*/
			in >>  alpha;//=0.95;
			/**Beta coefficient*/
			in >> beta;//=0.9995;
			/**Lambda coefficient*/
			in >>  lambda;//=200;
			/**Epsilion v. How strongly move winning node*/
			in >>  eps_w;//=0.05;
			/**Memory bound*/
			in >>  graph_memory_bound;
			/**Epsilion n*/
			in >> eps_n;//=0.0006;

			in >>  verbosity;

			/**Pseudodistance function used (might be non metric)*/
			in >> distance_function;


			/**Type of used database, unsgined int for compabititlity with Rcpp**/
			in >> datasetType;

			/**Initial reserve memory for nodes */
			in >> starting_nodes;
		}

		void serialize(std::ostream & out){
			///Utility constant
			out << experimental_utility_k << endl;

			///Utility option. Currently supported simples utility
			out <<  experimental_utility_option<< endl;

			/**Maximum number of nodes*/
			out <<  max_nodes<< endl;//=1000;
			/**Uniform grid optimization*/
			out <<  uniformgrid_optimization<< endl;//=true,lazyheap=true;
			/**Lazy heap optimization*/
			out <<  lazyheap_optimization<< endl;
			/**Bounding box specification*/

			/**Dimensionality of examples*/
			out <<  dim<< endl;

			REPORT(dim);

			for(int i=0;i<dim;++i){
				out<<axis[i]<< endl<<orig[i]<<endl;
			}
			/**Max edge age*/
			out <<  max_age<< endl;//=200;
			/**Alpha coefficient*/
			out <<  alpha<< endl;//=0.95;
			/**Beta coefficient*/
			out <<  beta<< endl;//=0.9995;
			/**Lambda coefficient*/
			out <<  lambda<< endl;//=200;
			/**Epsilion v. How strongly move winning node*/
			out <<  eps_w<< endl;//=0.05;
			/**Memory bound*/
			out <<  graph_memory_bound<< endl;
			/**Epsilion n*/
			out <<  eps_n<< endl;//=0.0006;

			out <<  verbosity<< endl;

			/**Pseudodistance function used (might be non metric)*/
			out <<  distance_function<< endl;


			/**Type of used database, unsgined int for compabititlity with Rcpp**/
			out <<  datasetType<< endl;

			/**Initial reserve memory for nodes */
			out <<  starting_nodes; //imporant not to add endl for binary correctness
		}

		//This is a simplification - we assume square box
		void setBoundingBox(double min, double max){
			orig = vector<double>();
			axis = vector<double>();
			for(int i=0;i<dim;++i){
				orig.push_back(min);
				axis.push_back(max - min);
			}
		}


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

				return false;
			}

			if(datasetType > 3 or datasetType <= 0){
				cerr<<"ERROR: wrong database specified\n";

				return false;
			}
			if(! (dim < 20 || ! uniformgrid_optimization)){

				cerr<<"WARNING: It might be too big dimensionality for OptimizedGNG."
						"OptimizedGNG works best for smaller dimensionality dataset"
						"Consider using PCA or other dim. reduction technique"
						"\n";

			}
			if(! (distance_function==gmum::GNGGraph::Euclidean || ! uniformgrid_optimization)){

				cerr<<"ERROR: You can use only Euclidean distance function with uniformgrid optimization\n";
				return false;
			}
			if(! (!uniformgrid_optimization or (dim == axis.size() && dim == orig.size()))){

				cerr<<"ERROR: dimensionality doesn't agree with axis and orig"<<endl;
				return false;
			}

			return true;
		}
	};
#endif	/* GNGCONFIGURATION_H */

