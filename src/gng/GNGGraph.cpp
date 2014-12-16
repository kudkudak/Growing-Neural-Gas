#include "GNGGraph.h"

#include <iostream>
#include <fstream>

using namespace std;

namespace gmum {

void writeToGraphML(GNGGraph &g, std::ostream & out) {

	g.lock();

	out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	out << "<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd\">\n";
	out << "<key id=\"key0\" for=\"edge\" attr.name=\"dist\" attr.type=\"double\" />\n";
	out << "<key id=\"key1\" for=\"node\" attr.name=\"error\" attr.type=\"double\" />\n";
	out << "<key id=\"key2\" for=\"node\" attr.name=\"label\" attr.type=\"double\" />\n";
	out << "<key id=\"key3\" for=\"node\" attr.name=\"gng_index\" attr.type=\"int\" />\n";
	out << "<key id=\"key4\" for=\"node\" attr.name=\"utility\" attr.type=\"double\" />\n";
	out << "<key id=\"key5\" for=\"node\" attr.name=\"v0\" attr.type=\"double\" />\n";
	out << "<key id=\"key6\" for=\"node\" attr.name=\"v1\" attr.type=\"double\" />\n";
	out << "<key id=\"key7\" for=\"node\" attr.name=\"v2\" attr.type=\"double\" />\n";
	out << "<graph id=\"G\" edgedefault=\"undirected\" parse.nodeids=\"canonical\" parse.edgeids=\"canonical\" parse.order=\"nodesfirst\">\n";

	std::map<int, int> gng_index_to_graph_index;

	unsigned int k = 0;
	for (int i = 0; i <= g.get_maximum_index(); ++i) {

		if (g.existsNode(i)) {
			gng_index_to_graph_index[g[i].nr] = k; //TODO:To be honest I dnt remember purpose of g[i].nr..

			out << "<node id=\"n" << k++ << "\">\n";
			out << "<data key=\"key1\">" << g[i].error << "</data>\n";
			out << "<data key=\"key2\">" << g[i].extra_data << "</data>\n";
			out << "<data key=\"key3\">" << g[i].nr << "</data>\n";
			out << "<data key=\"key4\">" << g[i].utility << "</data>\n";
			out << "<data key=\"key5\">" << g[i].position[0] << "</data>\n";
			out << "<data key=\"key6\">" << g[i].position[1] << "</data>\n";
			out << "<data key=\"key7\">" << g[i].position[2] << "</data>\n";
			out << "</node>\n";
		}
	}

	unsigned int l = 0;

	for (unsigned int i = 0; i <= g.get_maximum_index(); ++i)
		if (g.existsNode(i)) {
			FOREACH(edg, g[i])
			{
				if (g[i].nr > (*edg)->nr) { //directed!
					out << "<edge id=\"e" << l++ << "\" source=\"n"
							<< gng_index_to_graph_index[(*edg)->nr]
							<< "\" target=\"n"
							<< gng_index_to_graph_index[g[i].nr] << "\">\n";
					out << "<data key=\"key0\">" << g.get_dist(i, (*edg)->nr) << "</data>";
					out << "</edge>\n";
				}
			}

		}

	out << "</graph>\n</graphml>\n";
	g.unlock();

}

std::string writeToGraphML(GNGGraph &g, string filename) {

	if (filename == "") {
		std::stringstream ss;
		writeToGraphML(g, ss);
		return ss.str();
	} else {
		ofstream myfile(filename.c_str());
		writeToGraphML(g, myfile);
		myfile.close();
		return "";
	}

}
//
//  std::string writeToGraphMLBoost(GNGGraph &g, string filename){
//
//      DBG(4, "writeToGraphML::locking");
//	  g.lock();
//
//	  DBG(4, "writeToGraphML::locked");
//
//	  typedef adjacency_list<vecS, vecS, undirectedS,
//		boost_vertex_desc , boost_edge_desc> Graph;
//
//
//	  DBG(4, "writeToGraphML::boost_g defining");
//
//	  Graph boost_g(0);
//
//	  DBG(4, "writeToGraphML::defined boost_g");
//
//	  boost::graph_traits<Graph>::vertex_descriptor u, v;
//	  typedef boost::property<boost::vertex_index_t, int> VID_prop;
//
//	  map<int, boost::graph_traits<Graph>::vertex_descriptor> map_desc;
//
//	  DBG(4, "writeToGraphML::adding verticeS");
//	  //Add vertexes
//	  for(int i=0;i<=g.getMaximumIndex();++i)
//		if(g.existsNode(i))
//		{
//			DBG(1, gmum::to_string(i));
//			map_desc[g[i].nr] = add_vertex(boost_g);
//			boost_g[map_desc[g[i].nr]].index = g[i].nr;
//			boost_g[map_desc[g[i].nr]].error = g[i].error;
//			boost_g[map_desc[g[i].nr]].extra_data = g[i].extra_data;
//            boost_g[map_desc[g[i].nr]].utility = g[i].utility;
//
////			std::string dump = "";
////			//TODO: string builder
////			for(int j=0;j<g.getDim()-1;++j){
////				dump+=to_string(g[i].position[j])+ " ";
////
////			}
////			dump+=to_string(g[i].position[g.getDim()-1]);
////			boost_g[map_desc[g[i].nr]].position_dump = dump;
//			if(g.getDim() >= 3){
//				boost_g[map_desc[g[i].nr]].v0 = g[i].position[0];
//				boost_g[map_desc[g[i].nr]].v1 = g[i].position[1];
//				boost_g[map_desc[g[i].nr]].v2 = g[i].position[2];
//			}
//		}
//
//	  boost::property_map< Graph, double boost_edge_desc::* >::type edge_dist( boost::get(&boost_edge_desc::dist, boost_g) );
//	  DBG(4, "writeToGraphML::adding edges");
//	  //Add edges
//	  for (unsigned int i = 0; i <= g.getMaximumIndex(); ++i)
//		  if(g.existsNode(i))
//		  {
//				typedef boost::graph_traits<Graph>::edge_descriptor desc;
//				desc e;
//				bool b;
//				FOREACH(edg, g[i]){
//					if(g[i].nr > (*edg)->nr){ //directed!
//						add_edge(map_desc[g[i].nr], map_desc[(*edg)->nr],  boost_g);
//						boost::tie(e, b) = edge(map_desc[g[i].nr], map_desc[(*edg)->nr],  boost_g);
//						edge_dist[e] = g.getDist(i, (*edg)->nr);
//					}
//				}
//
//		  }
//
//
//
//	  //Construct properties getters
//	  dynamic_properties p;
//	  p.property("gng_index", boost::get(&boost_vertex_desc::index, boost_g));
//	  p.property("v0", boost::get(&boost_vertex_desc::v0, boost_g));
//	  p.property("v1", boost::get(&boost_vertex_desc::v1, boost_g));
//	  p.property("v2", boost::get(&boost_vertex_desc::v2, boost_g));
//	  p.property("error", boost::get(&boost_vertex_desc::error, boost_g));
//	  p.property("extra_data", boost::get(&boost_vertex_desc::extra_data, boost_g));
//	  p.property("dist", boost::get(&boost_edge_desc::dist, boost_g));
//	  p.property("utility", boost::get(&boost_vertex_desc::utility, boost_g));
//
//
//	  g.unlock();
//
//	  if(filename==""){
//
//		std::stringstream ss;
//		write_graphml(ss, boost_g, p, true);
//
//		return ss.str();
//	  }
//	  else{
//		 DBG(4, "writeToGraphML::writing to file");
//		ofstream myfile;
//		myfile.open (filename.c_str());
//		std::stringstream ss;
//		write_graphml(ss, boost_g, p, true) ;
//
//		myfile << ss.str();
//		myfile.close();
//
//		  return "";
//	  }
//
//	}
}

