/*
* File: GNGAlgorithm.h
* Author: Stanislaw "kudkudak" Jastrzebski <grimghil@gmail.com>
*
* Created on 11 sierpień 2012, 10:02
*/

#ifndef GNGALGORITHM_H
#define GNGALGORITHM_H

#include <memory>

#include "GNGGlobals.h"
#include "GNGGraph.h"
#include "GNGDataset.h"
#include "Threading.h"
#include "UniformGrid.h"
#include "GNGLazyErrorHeap.h"
#include <string>
using namespace std;

namespace gmum{


	/**
	* The main class of the implementation dealing with computations.
	* It should be agnostic of inner working (memory management etc.) of the graph and database.
	* Also should not be concerned with locking logic.
	*
	* @note TODO: Implement GNG on GPU.
	*/
	class GNGAlgorithm {
	public:
		typedef std::list<int> Node;

		double m_error; //error of the network
		int m_lambda; //lambda parameter
		double m_eps_w, m_eps_n; //epsilon of the winner and of the neighbour
		int m_max_age;
		int m_max_nodes;

		bool m_toggle_uniformgrid, m_toggle_lazyheap;

		double m_utility_k;
		int m_utility_option;

		double m_alpha,m_betha;
		double * m_betha_powers;
		int m_betha_powers_to_n_length;
		double * m_betha_powers_to_n;
		int m_betha_powers_size;
		double m_accumulated_error;

		int dim;

		std::map<std::string, long int> times;

		double m_density_threshold,m_grow_rate;

		/** Constants used by lazy heap implementation */
		int s,c;

		GNGGraph & m_g;
		GNGDataset * g_db;
		UniformGrid< std::vector<Node>, Node, int> * ug;
		GNGLazyErrorHeap errorHeap;


		enum GngStatus{
			GNG_PREPARING,
			GNG_RUNNING,
			GNG_PAUSED,
			GNG_TERMINATED
		};

		GngStatus m_gng_status;
		bool running;


		enum UtilityOptions{
			None,
			BasicUtility
		};


	   gmum::gmum_recursive_mutex status_change_mutex;
	   gmum::gmum_condition status_change_condition;

	   GngStatus gng_status(){
		   return m_gng_status;
	   }


	public:
		/** Run main loop of the algorithm*/
		void runAlgorithm();

		/**Construct main algorithm object, that will hold mid-results
		 * @param alg_memory_lock When locked algorithm is not running anything that is memory dangerous
		* @param g GNGGraph object implementing graph interface
		* @param db GNGDataset object
		* @param boundingbox_origin Starting point for reference system
		* @param boundingbox_axis Axis lengths for reference system
		* @param l Starting box size for uniform grid. Advised to be set to axis[0]/4 (TODO: move to the end of parameters list)
		* @param max_nodes Maximum number of nodes
		* @param max_age Maximum age of edge
		* @param alpha See original paper(TODO: add description)
		* @param betha See original paper (TODO: add description)
		* @param lambda Every lambda new vertex is added
		* @param eps_v See original paper(TODO: add description)
		* @param eps_n See original paper (TODO: add description)
		* @param dim Dimensionality
		*/
		GNGAlgorithm(

				GNGGraph * g, GNGDataset * db,
			double * boundingbox_origin, double * boundingbox_axis, double l,int max_nodes=1000,
			int max_age=200, double alpha=0.95, double betha=0.9995, double lambda=200,
			double eps_w=0.05, double eps_n=0.0006, int dim=3,
			bool uniformgrid_optimization=true,
			bool lazyheap_optimization=true,
			unsigned int utility_option = GNGConfiguration::UtilityOff,
			double utility_k = -1
			);


		///Retrieve closest node's gng_index to the example
		int predict(const std::vector<double> &);

		/** Start algorithm loop */
		void run(){
			 this->m_gng_status = GNG_RUNNING;
			 this->status_change_condition.notify_all();
		}

		/** Pause algorithm loop */
		void pause(){
			 this->m_gng_status = GNG_PAUSED;
			 this->status_change_condition.notify_all();
		}

		/** Terminate the algorithm */
		void terminate(){
			this->m_gng_status = GNG_TERMINATED;
			this->status_change_condition.notify_all();
		}





		void setMaxNodes(int value){m_max_nodes = value;}


		double CalculateAccumulatedError() ;

		void TestAgeCorrectness();


		virtual ~GNGAlgorithm(){
			delete [] m_betha_powers_to_n;
			delete [] m_betha_powers;
		}

	private:

	   void resetUniformGrid(double * orig, double *axis, double l) {
			ug->purge(orig,axis,l);
			int maximum_index = m_g.getMaximumIndex();

			REP(i, maximum_index + 1) {
				if (m_g.existsNode(i)) ug->insert(m_g[i].position, m_g[i].nr);
			}
	   }




		GNGNode ** LargestErrorNodesLazy();

		GNGNode ** LargestErrorNodes();

		/**
		* @brief Return two closest nodes (neurons) to the given example
		* @param[in] position Vector of coordinates of the example
		*/
		GNGNode ** TwoNearestNodes(const double * position);

		void RandomInit();

		void AddNewNode();

		void Adapt(const double * ex, const double * extra);

		void ResizeUniformGrid();

		bool stoppingCriterion(){ return m_g.getNumberNodes()>m_max_nodes; }

		void IncreaseErrorNew(GNGNode * node, double error){
			FixErrorNew(node);
			node->error+=m_betha_powers[m_lambda-s]*error;
			errorHeap.updateLazy(node->nr);
		}

		void FixErrorNew(GNGNode * node){
			if(node->error_cycle==c) return;

	//        if(c - node->error_cycle >= m_betha_powers_to_n_length){
	//            delete[] m_betha_powers_to_n;
	//            m_betha_powers_to_n_length *= 2;
	//            m_betha_powers_to_n = new double[m_betha_powers_to_n_length];
	//            REP(i, m_betha_powers_to_n_length)
	//            m_betha_powers_to_n[i] = std::pow(m_betha, m_lambda * (double) (i));
	//        }
	//
			node->error = m_betha_powers_to_n[c - node->error_cycle] * node->error;
			node->error_cycle = c;
		}

		double GetMaximumError() const{
			double max_error = 0;
			int maximumIndex = m_g.getMaximumIndex();
			REP(i,maximumIndex+1){
				if(m_g.existsNode(i)){
					max_error = std::max(max_error, m_g[i].error);
				}
			}
			return max_error;
		}


		void DecreaseAllErrorsNew(){
			return;
		}

		void DecreaseErrorNew(GNGNode * node){
			FixErrorNew(node);
			node->error = m_alpha*node->error;
			errorHeap.updateLazy(node->nr);
		}

		void SetErrorNew(GNGNode * node, double error){
			node->error = error;
			node->error_cycle = c;
			errorHeap.insertLazy(node->nr);
		}


		void IncreaseError(GNGNode * node, double error){
			node->error+=error;
		}

		void DecreaseAllErrors(){
			int maximumIndex = m_g.getMaximumIndex();
			REP(i,maximumIndex+1){
				if(m_g.existsNode(i)){
					m_g[i].error = m_betha*m_g[i].error;
				}
			}
		}

		void DecreaseError(GNGNode * node){
			node->error = m_alpha*node->error;
		}

		void SetError(GNGNode * node, double error){
			node->error = error;
		}



		// Note: this code is not optimal and is inserted only for research purposes

		double get_utility(int i){
            return m_g[i].utility;
		}

		void set_utility(int i, double u){
            m_g[i].utility = u;
		}

		void utility_criterion_check(){

			if(m_g.getNumberNodes()<10) return; //just in case

			double max_error = this->GetMaximumError();
			int maximumIndex = m_g.getMaximumIndex();

			double min_utility = 100000000;
			int min_utility_index = -1;
			for(int i=0;i<=maximumIndex;++i){
				if(min_utility > get_utility(i)){
					min_utility = get_utility(i);
					min_utility_index = i;
				}
			}

			if (m_g.existsNode(min_utility_index) &&
						max_error/get_utility(min_utility_index) > m_utility_k){
					DBG(2, "GNGAlgorithm:: removing node with utility "+gmum::to_string(get_utility(min_utility_index)) + " max error "+gmum::to_string(max_error));

					DBG(2,gmum::to_string<double>(max_error));


					GNGNode::EdgeIterator edg = m_g[min_utility_index].begin();
					while(edg != m_g[min_utility_index].end()){
							int nr = (*edg)->nr;
							edg = m_g.removeUDEdge(min_utility_index, nr);
					}


					m_g.deleteNode(min_utility_index);
					set_utility(min_utility_index,0);
			}


		}
		 void decrease_all_utility(){
			int maximumIndex = m_g.getMaximumIndex();
			for(int i=0;i<=maximumIndex;++i){
				if (m_g.existsNode(i)) {
					set_utility(i, get_utility(i)*(m_betha));
				}
			}

		}
	};




	/**Design hack for passing distance function dist(index, position)*/
	struct GNGGraphAccessHack{
		static GNGGraph * pool;
		static double dist(int index, double *position){
			return pool->getDist((*pool)[index].position, position);
		}
	};


//	typedef boost::posix_time::ptime Time;
//	typedef boost::posix_time::time_duration TimeDuration;

}

#endif
