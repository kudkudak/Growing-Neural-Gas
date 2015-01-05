#include "gng/GNG.h"
#include "utils/utils.h"
#include "gng/GNGGraph.h"
#include "gng/GNGDataset.h"
#include "utils/threading.h"

#include <algorithm>
#include <utility>
#include <vector>

#include "gtest/gtest.h"
/** Include all tests */

using namespace std;
using namespace gmum;
/*
 * Basic test
 */
TEST(GraphTests, BasicGraphTest) {
	boost::shared_ptr<Logger> logger = boost::shared_ptr<Logger>(
			new Logger(10));
	cerr << "Testing GraphTest\n";

	int N_start = 30;

	unsigned int dim = 6;

	gmum::recursive_mutex grow_mutex;
	RAMGNGGraph<GNGNode, GNGEdge> g(&grow_mutex, dim, N_start, //Initial pool size
			GNGGraph::Euclidean, //Used metric
			logger); //Logger

	ASSERT_EQ(g.existsNode(0), false);
	ASSERT_EQ(g.get_number_nodes(), 0);

	double x[6] = { 0.1, 0.2, 0.3, 0.4, 0.5, 0.6 };

	//Just fill in
	for (int i = 0; i < N_start; ++i) {
		int idx = g.newNode(x);
		ASSERT_EQ(idx, i);
	}

	//Enforce growing
	x[0] *= 100;

	cerr << "Getting size before regrowing\n" << g[0].size() << " "
			<< g[0].capacity() << endl << flush;

	g.deleteNode(2);
	cerr << "Checking writing before growing\n";
	string graphml = writeToGraphML(g);

	int idx = g.newNode(x);
	ASSERT_EQ(idx, 2);

	ASSERT_EQ(g[N_start - 2].nr, N_start - 2); //Check memory consistency
	ASSERT_EQ(g.newNode(x), N_start);
	ASSERT_EQ(g.get_number_nodes(), N_start + 1);
	ASSERT_EQ(g.newNode(x), N_start + 1);
	ASSERT_EQ(g.get_number_nodes(), N_start + 2);

	ASSERT_EQ(g.get_maximum_index(), N_start + 1);

	g.deleteNode(10);
	g.deleteNode(15);
	g.deleteNode(20);

	ASSERT_EQ(g.get_number_nodes(), N_start - 1);

	/** WARNING: highly intrusive test ! Can change implementation*/
	ASSERT_EQ(g.first_free, 20);
	ASSERT_EQ(g.next_free[g.first_free], 15); //might fail if not doubling
	ASSERT_EQ(g.next_free[15], 10);

	g.addUDEdge(0, 1);
	g.addUDEdge(0, 2);
	g.addUDEdge(2, 5);

	ASSERT_EQ(g.isEdge(0, 2), true);
	g.removeUDEdge(0, 2);
	ASSERT_EQ(g.isEdge(0, 1), true);
	ASSERT_EQ(g.isEdge(0, 2), false);
	ASSERT_EQ(g[0].size(), 1);
	ASSERT_EQ(g[5].size(), 1);

	for (int i = 0; i < g.get_maximum_index(); ++i) {
		if (g.existsNode(i))
			ASSERT_EQ(g[i].position[1], 0.2);
	}

	graphml = writeToGraphML(g);

	//Check memory consistency
	ASSERT_EQ(g[0].position[3], 0.4);

	DBG(logger,10, "Test OK");

	//Check regrowing
	for (int i = 0; i < 20 * N_start; ++i) {
		int idx = g.newNode(x);
		int cn = __int_rnd(0, i);
		while (!g.existsNode(cn)) {
			cn = __int_rnd(0, i);
		}
		g.addUDEdge(idx, cn);
	}

	string pool_before = g.reportPool();

	set<int> edges_first_10_bef;
	for (int i = 0; i < 10; ++i) {
		if (g.existsNode(i))
			for (int k = 0; k < g[i].size(); ++k) {
				edges_first_10_bef.insert(g[i][k]->nr);
			}
	}
	vector<int> serialized_edges_first_10_bef;
	std::copy(edges_first_10_bef.begin(), edges_first_10_bef.end(),
			std::back_inserter(serialized_edges_first_10_bef));

	std::ofstream output;
	output.open("graph.bin", ios::out | ios::binary);

	g.serialize(output);

	RAMGNGGraph<GNGNode, GNGEdge> g2(&grow_mutex, dim, N_start,
			GNGGraph::Euclidean, logger);
	output.close();

	std::ifstream input;
	input.open("graph.bin", ios::in | ios::binary);

	g2.load(input);

	string pool_after = g2.reportPool();

	set<int> edges_first_10_aft;
	for (int i = 0; i < 10; ++i) {
		if (g2.existsNode(i))
			for (int k = 0; k < g2[i].size(); ++k) {
				edges_first_10_aft.insert(g2[i][k]->nr);
			}
	}
	vector<int> serialized_edges_first_10_aft;
	std::copy(edges_first_10_aft.begin(), edges_first_10_aft.end(),
			std::back_inserter(serialized_edges_first_10_aft));

	ASSERT_TRUE(
			std::equal(serialized_edges_first_10_aft.begin(),
					serialized_edges_first_10_aft.end(),
					serialized_edges_first_10_bef.begin()));

	for (int i = 0; i < 1000; ++i) {
		int id = g2.newNode(x);
		g2.addUDEdge(id, 0);
	}

	g2.reportPool();
}

TEST(DatabaseTests, BasicDatasetTest) {
	boost::shared_ptr<Logger> logger = boost::shared_ptr<Logger>(
			new Logger(10));
	int m_verbosity = 3;

	unsigned int dim = 6;
	gmum::recursive_mutex phase_2_lock;
	unsigned int num_examples = 100, meta_data_dim = 1;
	REPORT_PRODUCTION("Creating databse");
	GNGDatasetSimple<double> dataset2(&phase_2_lock, dim,
			true /* store_extra */,
			GNGDatasetSimple<double>::SamplingProbability, logger);

	double * x = new double[num_examples * (dim)];
	double * labels = new double[num_examples];
	double * probabilities = new double[num_examples];


	for (int i = 0; i < num_examples; ++i) {
		for (int j = 0; j < dim; ++j) {
			x[i * dim + j] = 0.2;
		}
		labels[i] = i;
		probabilities[i] = 0.3;
	}
	REPORT_PRODUCTION("Adding first batch");
	dataset2.insertExamples(x, labels, probabilities, num_examples);


	for (int i = 0; i < num_examples; ++i) {
		for (int j = 0; j < dim; ++j) {
			x[i * dim + j] = j / (2 * (float) dim);
		}
		labels[i] = i - 2;
		probabilities[i] = 0.9;
	}
	REPORT_PRODUCTION("Adding second batch");
	dataset2.insertExamples(x, labels, probabilities, num_examples);

	REPORT_PRODUCTION("Quering");
	for (int i = 0; i < 1000; ++i) {
		unsigned int a = dataset2.drawExample();
		unsigned int b = dataset2.drawExample();
		unsigned int c = dataset2.drawExample();

		ASSERT_LE(dataset2.getPosition(a)[0], 0.9);
		ASSERT_LE(dataset2.getPosition(b)[0], 0.9);
		ASSERT_LE(dataset2.getPosition(c)[0], 0.9);
	}
}
