/*
 * File: GNGAlgorithm.cpp
 * Author: staszek "kudkudak" jastrzebski <grimghil<at>gmail.com>
 *
 * Created on 11 sierpień 2012, 10:02
 */

#include "GNGAlgorithm.h"
#include <cstdlib>

using namespace gmum;
using namespace std;

namespace gmum {

GNGNode ** GNGAlgorithm::LargestErrorNodesLazy() {
	GNGNode ** largest = new GNGNode*[2];
	GNGNode * gng_node;

	FOREACH(it, errorHeap.getLazyList()){
		gng_node = &m_g[*it];
		errorHeap.insert(gng_node->nr, gng_node->error);
	}

	errorHeap.getLazyList().clear();

	ErrorNode max;
	//Extract max until you get correct one (that is not lazy)
	do {
		max = errorHeap.extractMax();

		DBG_2(m_logger, 4, "GNGAlgorithm::LargestErrorLazy::found max " + to_string(max.i));

		GNGNode * gng_node = &m_g[max.i];
		if (gng_node->error_cycle != c) {
			FixErrorNew(gng_node);
			errorHeap.update(gng_node->nr, gng_node->error);
		} else {
			largest[0] = gng_node;
			int j = 0;
			double error = 0.0;
			DBG_2(m_logger, 4, "GNGAlgorithm::LargestErrorLazy::found max " + to_string(max.i));

			FOREACH(edg, *largest[0]) {
				++j;
				FixErrorNew(&m_g[(*edg)->nr]);

				if (j == 1) {
					largest[1] = &m_g[(*edg)->nr];
					error = largest[1]->error;
					;
					continue;
				}

				double new_error = m_g[(*edg)->nr].error;

				if (error < new_error) {
					error = new_error;
					largest[1] = &m_g[(*edg)->nr];
				}
			}
			break;
		}



	} while (1);
	DBG_2(m_logger, 3, "GNGAlgorithm::LargestErrorLazy::returning");
	return largest;

}

GNGGraph* GNGGraphAccessHack::pool = 0;

GNGAlgorithm::GNGAlgorithm(GNGGraph * g, GNGDataset* db,
		double * boundingbox_origin, double * boundingbox_axis, double l,
		int max_nodes, int max_age, double alpha, double betha, double lambda,
		double eps_w, double eps_n, int dim, bool uniformgrid_optimization,
		bool lazyheap_optimization, unsigned int utility_option,
		double utility_k, boost::shared_ptr<Logger> logger) :
		m_g(*g), g_db(db), c(0), s(0), m_max_nodes(max_nodes), m_max_age(
				max_age), m_alpha(alpha), m_betha(betha), m_lambda(lambda), m_eps_w(
				eps_w), m_eps_n(eps_n), m_density_threshold(0.1), m_grow_rate(
				1.5), errorHeap(), dim(dim), m_toggle_uniformgrid(
				uniformgrid_optimization), m_toggle_lazyheap(
				lazyheap_optimization), running(false), m_utility_option(
				utility_option), m_error(0.0), m_utility_k(utility_k), m_logger(
				logger), m_iteration(0) {

	DBG(m_logger, 1, "GNGAlgorithm:: Constructing object");DBG(m_logger, 10, "GNGAlgorithm::Constructed object with utility "+to_string(utility_option)+" "+to_string(utility_k));

	if (m_toggle_uniformgrid) {
		ug = new UniformGrid<std::vector<Node>, Node, int>(boundingbox_origin, boundingbox_axis, l, dim, m_grow_rate, m_density_threshold, 0.4, m_logger);

		GNGGraphAccessHack::pool = &m_g;

		ug->setDistFunction(GNGGraphAccessHack::dist);

		// Restore uniform grid state
		int maximum_index = m_g.get_maximum_index();
		REP(i, maximum_index + 1)
			if (m_g.existsNode(i))
				ug->insert(m_g[i].position, m_g[i].nr);

	}

	if (m_toggle_lazyheap) {
		int maximum_index = m_g.get_maximum_index();
		REP(i, maximum_index + 1)
			if (m_g.existsNode(i))
				set_error_new(&m_g[i], m_g[i].error);
	}

	m_betha_powers_size = m_lambda * 10;
	m_betha_powers = new double[m_betha_powers_size];

	REP(i, m_betha_powers_size)
		m_betha_powers[i] = std::pow(m_betha, (double) (i));

	m_betha_powers_to_n_length = m_max_nodes * 2;
	m_betha_powers_to_n = new double[m_max_nodes * 2];

	REP(i, m_max_nodes * 2)
		m_betha_powers_to_n[i] = std::pow(m_betha, m_lambda * (double) (i));
	DBG(m_logger, 1, "GNGAlgorithm:: Constructed object");
}

void GNGAlgorithm::RandomInit() {

	DBG(m_logger, 3, "RandomInit::Drawing examples");

	int ex1 = g_db->drawExample();
	int ex2 = g_db->drawExample();

	DBG(m_logger, 3, "RandomInit::Drawn 2");
	int index = 0;
	while (ex2 == ex1 && index < 100) {
		++index;
		ex2 = g_db->drawExample();
	}
	DBG(m_logger, 3, "RandomInit::database_size = "+to_string(g_db->getSize()));DBG(m_logger, 3, "RandomInit::drawn "+to_string(ex1)+" "+to_string(ex2));

	const double * ex1_ptr = g_db->getPosition(ex1);
	const double * ex1_extra_ptr = g_db->getVertexData(ex1);
	const double * ex2_ptr = g_db->getPosition(ex2);
	const double * ex2_extra_ptr = g_db->getVertexData(ex2);

	m_g.newNode(ex1_ptr);
	m_g.newNode(ex2_ptr);

	if (ex1_extra_ptr)
		m_g[0].extra_data = ex1_extra_ptr[0];
	if (ex2_extra_ptr)
		m_g[1].extra_data = ex2_extra_ptr[0];

	DBG(m_logger, 3, "RandomInit::created nodes graph size="+to_string(m_g.get_number_nodes()));

#ifdef GMUM_DEBUG_2
	assert(m_g.get_number_nodes()==2);
#endif

	if (m_toggle_uniformgrid) {
		ug->insert(m_g[0].position, 0);
		ug->insert(m_g[1].position, 1);
	}

	if (m_toggle_lazyheap) {
		set_error_new(&m_g[0], 0.0);
		set_error_new(&m_g[1], 0.0);
	}
	if (this->m_utility_option == BasicUtility) {
		set_utility(0, 0.001);
		set_utility(1, 0.001);
	}
}

void GNGAlgorithm::AddNewNode() {
	using namespace std;

	DBG(m_logger, 4, "GNGAlgorith::AddNewNode::start search");

	if (m_toggle_lazyheap)
		DBG(m_logger, 4, "GNGAlgorithm::AddNewNode:: "+to_string(m_toggle_lazyheap)+" : )= toggle_lazyheap");

	GNGNode ** error_nodes_new;

	if (m_toggle_lazyheap)
		error_nodes_new = LargestErrorNodesLazy();
	else
		error_nodes_new = LargestErrorNodes();

	DBG_2(m_logger, 4, "GNGAlgorith::AddNewNode::search completed");

	if (!error_nodes_new[0] || !error_nodes_new[1])
		return;

	DBG_2(m_logger, 4, "GNGAlgorith::AddNewNode::search completed and successful");

	if (m_max_nodes <= m_g.get_number_nodes()) {
		DBG(m_logger, 4, "GNGAlgorith::AddNewNode:: achieved maximum number of nodes");
		delete[] error_nodes_new;
		return;
	}

	double position[this->dim]; //param

	//TODO: < GNG_DIM?
	for (int i = 0; i < this->dim; ++i) //param
		position[i] = (error_nodes_new[0]->position[i] + error_nodes_new[1]->position[i]) / 2;

	//In case pool has been reallocated
	int er_nr1 = error_nodes_new[0]->nr, er_nr2 = error_nodes_new[1]->nr;
	int new_node_index = m_g.newNode(&position[0]);
	error_nodes_new[0] = &m_g[er_nr1];
	error_nodes_new[1] = &m_g[er_nr2];

	//Vote for extra data
	m_g[new_node_index].extra_data = (error_nodes_new[0]->extra_data
			+ error_nodes_new[1]->extra_data) / 2.0;

	if (m_toggle_uniformgrid)
		ug->insert(m_g[new_node_index].position, new_node_index);

	DBG_2(m_logger, 4, "GNGAlgorith::AddNewNode::added " + to_string(m_g[new_node_index]));

	m_g.removeUDEdge(error_nodes_new[0]->nr, error_nodes_new[1]->nr);

	DBG_2(m_logger, 3, "GNGAlgorith::AddNewNode::removed edge beetwen " + to_string(error_nodes_new[0]->nr) + " and" + to_string(error_nodes_new[1]->nr));DBG(m_logger, 2, "GNGAlgorithm::AddNewNode::largest error node after removing edge : " + to_string(*error_nodes_new[0]));

	m_g.addUDEdge(error_nodes_new[0]->nr, new_node_index);

	m_g.addUDEdge(new_node_index, error_nodes_new[1]->nr);

	DBG_2(m_logger, 3, "GNGAlgorith::AddNewNode::add edge beetwen " + to_string(error_nodes_new[0]->nr) + " and" + to_string(new_node_index));DBG(m_logger, 3, "GNGAlgorith::AddNewNode::add edge beetwen " + to_string(error_nodes_new[1]->nr) + " and" + to_string(new_node_index));

	if (!m_toggle_lazyheap) {
		DecreaseError(error_nodes_new[0]);
		DecreaseError(error_nodes_new[1]);
		set_error(&m_g[new_node_index],
				(error_nodes_new[0]->error + error_nodes_new[1]->error) / 2);
	} else {
		DecreaseErrorNew(error_nodes_new[0]);
		DecreaseErrorNew(error_nodes_new[1]);
		set_error_new(&m_g[new_node_index],
				(error_nodes_new[0]->error + error_nodes_new[1]->error) / 2);
	}

	if (this->m_utility_option == BasicUtility)
		this->set_utility(new_node_index,0.5 * (get_utility(error_nodes_new[0]->nr) + get_utility(error_nodes_new[1]->nr)));

	;
	delete[] error_nodes_new;
}

int GNGAlgorithm::predict(const std::vector<double> & ex) {

	if (m_g.get_number_nodes() == 0)
		return -1; //No node

	if (ex.size() != g_db->getGNGDim())
		throw BasicException("Wrong example dimensionality");


	if (m_toggle_uniformgrid) {
		std::vector<int> nearest_index = ug->findNearest(&ex[0], 2); //TwoNearestNodes(ex->position);
		return nearest_index[0];
	} else {
		GNGNode ** tmp = TwoNearestNodes(&ex[0]);
		int ret_index = tmp[0]->nr;
		delete[] tmp;
		return ret_index;
	}
}

void GNGAlgorithm::Adapt(const double * ex, const double * extra) {
//		Time t1(boost::posix_time::microsec_clock::local_time());

	DBG_2(m_logger, 4, "GNGAlgorith::Adapt::commence search");

	GNGNode * nearest[2];

	if (m_toggle_uniformgrid) {

		DBG(m_logger, 1, "GNGAlgorithm::Adapt::Graph size "+to_string(m_g.get_number_nodes()));

		std::vector<int> nearest_index = ug->findNearest(ex, 2); //TwoNearestNodes(ex->position);

		DBG(m_logger, 1, "GNGAlgorithm::Adapt::Found nearest");

//			Time t2(boost::posix_time::microsec_clock::local_time());
//			TimeDuration dt = t2 A
//			times["uniform_grid_search"] += dt.total_microseconds();

#ifdef GMUM_DEBUG_2
		if (nearest_index[0] == nearest_index[1]) {
			cerr<<"Adapt::Found same nearest_indexes!~! "+to_string(nearest_index[0])<<endl;
			throw BasicException("Found same nearest_indexes");
			return; //something went wrong (-1==-1 też)
		}
#endif

		nearest[0] = &m_g[nearest_index[0]];
		nearest[1] = &m_g[nearest_index[1]];

#ifdef GMUM_DEBUG_2

		double error0 = m_g.get_dist(nearest[0]->position, ex);
		double error1 = m_g.get_dist(nearest[1]->position, ex);

		if(error1 < error0)
			throw BasicException("Failed UG - returned nodes in reversed order ?");

#endif

	} else {
		DBG(m_logger, 1, "GNGAlgorith::Adapt::calling TwoNearestNodes");

		GNGNode ** tmp = TwoNearestNodes(ex);
		DBG(m_logger, 1, "GNGAlgorith::Adapt::found TwoNearestNodes");

		nearest[0] = tmp[0];
		nearest[1] = tmp[1];
		delete[] tmp;
	}
//		Time t2(boost::posix_time::microsec_clock::local_time());

//		TimeDuration dt = t2 - t1;

//		times["adapt1"] += dt.total_microseconds();

//		t1 = boost::posix_time::microsec_clock::local_time();

	DBG_2(m_logger, 4, "GNGAlgorith::Adapt::found nearest nodes to the drawn example " + to_string(*nearest[0]) + " " + to_string(*nearest[1]));

	double error = m_g.get_dist(nearest[0]->position, ex);
	if (this->m_utility_option == BasicUtility) {

		DBG(m_logger, 4, "GNGAlgorithm::Adapt::setting utility");

		double error_2 = m_g.get_dist(nearest[1]->position, ex);

		this->set_utility(nearest[0]->nr, this->get_utility(nearest[0]->nr) + error_2 - error);
	}

	DBG_2(m_logger, 3, "GNGAlgorith::Adapt::increasing error");

	if (!m_toggle_lazyheap)
		IncreaseError(nearest[0], error);
	else
		IncreaseErrorNew(nearest[0], error);

	DBG_2(m_logger, 3, "GNGAlgorith::Adapt::accounted for the error");

	if (m_toggle_uniformgrid)
		ug->remove(nearest[0]->position);
	for (int i = 0; i < this->dim; ++i)
		nearest[0]->position[i] += m_eps_w * (ex[i] - nearest[0]->position[i]);

	//Adapt to extra dimensionality if present (TODO: refactor)
	if (extra)
		nearest[0]->extra_data = (nearest[0]->extra_data + extra[0]) / 2.0;

	if (m_toggle_uniformgrid)
		ug->insert(nearest[0]->position, nearest[0]->nr);

	if (nearest[0]->edgesCount) {
		FOREACH(edg, *nearest[0]){
			if (m_toggle_uniformgrid)
				ug->remove(m_g[(*edg)->nr].position);

			for (int i = 0; i < this->dim; ++i) { //param accounting
				m_g[(*edg)->nr].position[i] += m_eps_n
						* (ex[i] - m_g[(*edg)->nr].position[i]);
			}

			//Adapt to extra dimensionality if present (TODO: refactor)
			if (extra) {
				m_g[(*edg)->nr].extra_data = (0.9 * m_g[(*edg)->nr].extra_data
						+ extra[0] * 0.1);
			}

			if (m_toggle_uniformgrid)
				ug->insert(m_g[(*edg)->nr].position, (*edg)->nr);
		}
	}

	DBG(m_logger, 4, "GNGAlgorith::Adapt::position of the winner and neighbour mutated");

	if (!m_g.isEdge(nearest[0]->nr, nearest[1]->nr)) {
		m_g.addUDEdge(nearest[0]->nr, nearest[1]->nr);
		DBG(m_logger, 4, "GNGAlgorith::Adapt::added edge beetwen " + to_string(nearest[0]->nr) + " and " + to_string(nearest[1]->nr));
	}

	bool BYPASS = false;

	DBG_2(m_logger, 4, "GNGAlgorith::Adapt::commence scan of edges");

	//TODO: assuming here GNGNode not any arbitrary node :/
	GNGNode::EdgeIterator edg = nearest[0]->begin();
	while (edg != nearest[0]->end()) {
		DBG(m_logger, 2, "Currently on edge to"+to_string((*edg)->nr));

		(*edg)->age++;
		(((*edg)->rev))->age++;

		if ((*edg)->nr == nearest[1]->nr) {
			(*edg)->age = 0;
			(((*edg)->rev))->age = 0;
		}

		if ((*edg)->age > m_max_age) {

			DBG(m_logger, 3, "GNGAlgorith::Adapt::Removing aged edge " + to_string(nearest[0]->nr) + " - " + to_string((*edg)->nr));

			int nr = (*edg)->nr;

			//Note that this is O(E), but average number of edges is very small, so it is OK
			edg = m_g.removeUDEdge(nearest[0]->nr, nr);

			if (m_g[nr].edgesCount == 0 && this->m_utility_option == None) {

				DBG(m_logger, 8, "GNGAlgorith:: remove node because no edges");



#ifdef DEBUG_GMUM_2
				FOREACH(edg, m_g[nr])
				{
					cerr<<"WARNING: GNGAlgorithm:: edges count of neighbours of erased node, shouldn't happen! " + to_string(m_g[(*edg)->nr].edgesCount)<<endl;
				}
#endif

				if (m_toggle_uniformgrid)
					ug->remove(m_g[nr].position);

				DBG(m_logger, 8, "GNGAlgorithm::Adapt() Erasing node " + to_string<int>(nr));

				DBG(m_logger, 8, "GNGAlgorithm::Adapt() First coordinate " + to_string<double>(m_g[nr].position[0]));

				m_g.deleteNode(nr);
			}
			if (m_g[nearest[0]->nr].edgesCount == 0
					&& this->m_utility_option == None) {

				DBG(m_logger, 49, "WARNING: GNGAlgorithm::Adapt() remove node because no edges, shouldn't happen"); //Shouldn't happen

				if (m_toggle_uniformgrid)
					ug->remove(m_g[nearest[0]->nr].position);
				m_g.deleteNode(nearest[0]->nr);
				break;
			}
			if (edg != nearest[0]->end())
				--edg;
			else
				break;DBG(m_logger, 3, "GNGAlgorith::Adapt::Removal completed");
		}
		++edg;
	}

	DBG(m_logger, 3, "GNGAlgorith::Adapt::Scan completed");

	//erase nodes
	if (this->m_utility_option == BasicUtility)
		this->utility_criterion_check();

//		t2 = boost::posix_time::microsec_clock::local_time();
//		dt = t2 - t1;

//		times["adapt2"] += dt.total_microseconds();

	DBG(m_logger, 3, "GNGAlgorith::Adapt::leaving");

}

double GNGAlgorithm::CalculateAccumulatedError() {
	//@note: this function can be called from outside so it has to synchronize

	//TODO: Why does it lock?

	int maximum_index = m_g.get_maximum_index();
	m_accumulated_error = 0.0;

	if (this->m_toggle_lazyheap) {

		m_g.lock();
		int maximum_index = m_g.get_maximum_index();
		m_accumulated_error = 0.0;

		REP(i, maximum_index + 1)
			if (m_g.existsNode(i))
				m_accumulated_error += m_g[i].error;

		m_g.unlock();
		return m_accumulated_error;
	} else {
		m_g.lock();
		m_accumulated_error = 0.0;
		REP(i, maximum_index + 1)
			if (m_g.existsNode(i))
				m_accumulated_error += m_g[i].error;

		m_g.unlock();
		return m_accumulated_error;
	}

}

void GNGAlgorithm::TestAgeCorrectness() {

	int maximum_index = m_g.get_maximum_index();

	REP(i, maximum_index + 1)
		if (m_g.existsNode(i) && m_g[i].edgesCount)
			FOREACH(edg, m_g[i])
				if ((*edg)->age > m_max_age) {
					//cout << "XXXXXXXXXXXXXXXXX\n";
					//cout << (m_g[i]) << endl;
				}

}

void GNGAlgorithm::ResizeUniformGrid() {

	DBG_2(m_logger, 6, "GNGAlgorithm::Resize Uniform Grid");DBG(m_logger, 6, "GNGAlgorithm::Resize Uniform Grid old_l=" + to_string(ug->getCellLength()));DBG(m_logger, 6, "GNGAlgorithm::Resize Uniform Grid new_l=" + to_string(ug->getCellLength() / m_grow_rate));

	ug->new_l(ug->getCellLength() / m_grow_rate);

	int maximum_index = m_g.get_maximum_index();

	REP(i, maximum_index + 1)
		if (m_g.existsNode(i))
			ug->insert(m_g[i].position, m_g[i].nr);

}

GNGNode ** GNGAlgorithm::LargestErrorNodes() {

	DBG(m_logger, 2, "LargestErrorNodes::started procedure");

	GNGNode ** largest = new GNGNode*[2];

	largest[0] = 0;
	largest[1] = 0;
	double error = -1.0;

	REP(i, m_g.get_maximum_index() + 1)
		if (m_g.existsNode(i))
			error = std::max(error, m_g[i].error);

	DBG(m_logger, 2, "LargestErrorNodes::found maximum error");

	REP(i, m_g.get_maximum_index() + 1)
		if (m_g.existsNode(i))
			if (m_g[i].error == error)
				largest[0] = &m_g[i];


	DBG(m_logger, 2, "LargestErrorNodes::largest picked");

	if (largest[0]->edgesCount == 0) { //{largest[0]->error=0; return largest;} //error?
		m_g.deleteNode(largest[0]->nr);
		return largest;
	}

	int j = 0;

	FOREACH(edg, *largest[0]) {
		++j;

		if (j == 1) {
			largest[1] = &m_g[(*edg)->nr];
			error = largest[1]->error;
			continue;
		}

		double new_error = m_g[(*edg)->nr].error;

		if (error < new_error) {
			error = new_error;
			largest[1] = &m_g[(*edg)->nr];
		}
	}

	return largest;

}

GNGNode ** GNGAlgorithm::TwoNearestNodes(const double * position) { //to the example

	DBG(m_logger, 1, "GNGAlgorithm::just called TwoNearestNodes");
	//no unique_ptr in this C++, nearest is returned!
	DBG(m_logger, 1, "GNGAlgorithm::just called TwoNearestNodes");

	DBG(m_logger, 1, "GNGAlgorithm::just called TwoNearestNodes");

	GNGNode** nearest = new GNGNode*[2];
	nearest[0] = 0;
	nearest[1] = 0;

	DBG(m_logger, 1, "GNGAlgorithm::just called TwoNearestNodes");
	int start_index = 0;
	DBG(m_logger, 1, "GNGAlgorithm::just called TwoNearestNodes");DBG(m_logger, 1, "GNGAlgorithm::starting TwoNearestNodes");
	while (!m_g.existsNode(start_index))
		++start_index;

	DBG(m_logger, 1, "GNGAlgorithm::just called TwoNearestNodes");
	double dist = m_g.get_dist(position, m_g[start_index].position);
	nearest[0] = &m_g[start_index];

	DBG(m_logger, 1, "GNGAlgorithm::starting search of nearest node from " + to_string(start_index));

	for (int i = start_index + 1; i <= m_g.get_maximum_index(); ++i) { //another idea for storing list of actual nodes?

		if (m_g.existsNode(i)) {
			DBG(m_logger, 1, "calculating new_dist\n");

			double new_dist = m_g.get_dist(position, m_g[i].position);

			if (dist > new_dist) {
				dist = new_dist;
				nearest[0] = &m_g[i];
			}
		}
	}

	DBG(m_logger, 1, "finding next\n");

	start_index = 0;

	while (!m_g.existsNode(start_index) || start_index == nearest[0]->nr)
		++start_index;
	dist = m_g.get_dist(position, m_g[start_index].position);
	nearest[1] = &m_g[start_index];

	for (int i = start_index + 1; i <= m_g.get_maximum_index(); ++i) { //another idea for storing list of actual nodes?
		if (m_g.existsNode(i) && i != nearest[0]->nr) {
			double new_dist = m_g.get_dist(position, m_g[i].position);

			if (dist > new_dist) {
				dist = new_dist;
				nearest[1] = &m_g[i];
			}
		}
	}
	DBG(m_logger, 1, "search successful");DBG(m_logger, 1, "search successful and nearest[1]= " + to_string(nearest[1]));

	return nearest;
}

void GNGAlgorithm::runAlgorithm() { //1 thread needed to do it (the one that computes)

	this->running = true;
	int size = g_db->getSize();

	DBG_2(m_logger, 3, "GNGAlgorithm::runAlgorithm()");

	int counter = 0;
	s = 0;

//		Time t1, t2, t3, t4;
//		TimeDuration dt;

	DBG_2(m_logger, 3, "GNGAlgorithm::check size of the db " + to_string(size));

	while (g_db->getSize() < 2) {

		++counter;
		gmum::sleep(100);

		//TODO: change to interruption pattern
		while (this->m_gng_status != GNG_RUNNING) {
			DBG(m_logger, 1, "GNGAlgorithm::status in main loop = "+to_string(this->m_gng_status));
			if (this->m_gng_status == GNG_TERMINATED)
				break;
			this->status_change_condition.wait(this->status_change_mutex);
		}

		if (counter % 10 != 0)
			continue;

		int size = g_db->getSize();
		DBG(m_logger, 2, "GNGAlgorithm::check size of the db " + to_string(size));

		while (this->m_gng_status != GNG_RUNNING) {
			DBG(m_logger, 1, "GNGAlgorithm::status in database loop = "+to_string(this->m_gng_status));
			if (this->m_gng_status == GNG_TERMINATED)
				break;
			this->status_change_condition.wait(this->status_change_mutex);
		}
	}

	if (m_g.get_number_nodes() == 0) {
		g_db->lock();
		m_g.lock();
		RandomInit();
		m_g.unlock();
		g_db->unlock();
	} else if (m_g.get_number_nodes() == 1) {
		cerr << "Incorrect passed graph to GNGAlgorithm. Aborting\n";
		throw BasicException("Incorrect passed graph to GNGAlgorithm");
	}

	c = 0; // cycle variable for lazyheap optimization

	DBG(m_logger, 3, "GNGAlgorithm::init successful, starting the loop");

//		t3 = boost::posix_time::microsec_clock::local_time();
	DBG_2(m_logger, 1, "GNGAlgorithm::gng_status="+to_string(this->m_gng_status));
	while (this->m_gng_status != GNG_TERMINATED) {

		while (this->m_gng_status != GNG_RUNNING) {
			DBG(m_logger, 1, "GNGAlgorithm::status in main loop = "+to_string(this->m_gng_status));
			if (this->m_gng_status == GNG_TERMINATED)
				break;
			this->status_change_condition.wait(this->status_change_mutex);
		}

		for (s = 0; s < m_lambda; ++s) { //global counter!!
			g_db->lock();
			m_g.lock();


			DBG(m_logger, 0, "GNGAlgorithm::draw example");

			unsigned int ex = g_db->drawExample();

			Adapt(g_db->getPosition(ex), g_db->getVertexData(ex));

			m_g.unlock();
			g_db->unlock();
		}

#ifdef GMUM_DEBUG_2
		for (int i = 0; i <= m_g.get_maximum_index(); ++i) { //another idea for storing list of actual nodes?
			if (m_g.existsNode(i) && m_g[i].edgesCount == 0 && m_utility_option == None) {
				cerr<<"Error at " + to_string<int>(i))<<endl;
			}
		}
#endif
		g_db->lock();
		m_g.lock();

		DBG_2(m_logger, 4, "GNGAlgorithm::add new node");

//			t1 = boost::posix_time::microsec_clock::local_time();

		AddNewNode();
		if (m_toggle_uniformgrid && ug->check_grow()) {
			DBG_2(m_logger, 10, "GNGAlgorithm:: resizing uniform grid");
			ResizeUniformGrid();
		}

//			t2 = boost::posix_time::microsec_clock::local_time();
//			dt = t2 - t1;
//			times["resize"] += dt.total_microseconds();

		++c; //epoch
		if (!m_toggle_lazyheap)
			DecreaseAllErrors();
		if (this->m_utility_option == BasicUtility)
			decrease_all_utility();
		m_g.unlock();
		g_db->unlock();

		++m_iteration;
	}
	m_g.lock();
	g_db->lock();
	m_g.unlock();
	g_db->unlock();

	DBG(m_logger, 30, "GNGAlgorithm::Terminated server");
	this->running = false;

//		t4 = boost::posix_time::microsec_clock::local_time();
//		dt = t4 - t3;
//		REPORT(times["adapt1"]);
//		REPORT(times["adapt2"]);
//		REPORT(times["uniform_grid_search"]);
//		REPORT(times["resize"]);
//		REPORT(dt.total_microseconds() / (double) 1000000);

}

}
