/* 
 * File:   GNGAlgorithm.cpp
 * Author: staszek
 * 
 * Created on 11 sierpień 2012, 10:02
 */

#include "GNGAlgorithm.h"

GNGNode ** GNGAlgorithm::LargestErrorNodesLazy() {
    GNGNode ** largest = new GNGNode*[2];

    GNGNode * gngnode;

    FOREACH(it, errorHeap.getLazyList()) {
        gngnode = m_g[*it];
        //dbg.push_back(1,"GNGAlgorithm::LargestErrorLazy::inserting node "+to_string(*node)+" "+to_string(m_g[*node]->error_cycle));
        errorHeap.insert(gngnode->nr, gngnode->error_new);
    }
    errorHeap.getLazyList().clear();


    ErrorNode max;
    do {
        max = errorHeap.extractMax();
        //dbg.push_back(1,"GNGAlgorithm::LargestErrorLazy::found max "+to_string(max.i));
        GNGNode * gngnode = m_g[max.i];

        if (gngnode->error_cycle != c) {
            FixErrorNew(gngnode);
            errorHeap.update(gngnode->nr, gngnode->error_new);
        } else {
            largest[0] = gngnode;
            int j = 0;
            double error = 0.0;

            FOREACH(edg, *(largest[0]->edges)) {
                ++j;
                FixErrorNew(m_g[edg->nr]);
                if (j == 1) {
                    largest[1] = m_g[edg->nr];
                    error = largest[1]->error_new;
                    ;
                    continue;
                }


                double new_error = m_g[edg->nr]->error_new;
                if (error < new_error) {
                    error = new_error;
                    largest[1] = m_g[edg->nr];
                }
            }
            break;
        }
    } while (1);

    return largest;
}


GNGNode * GNGGraphAccessHack::pool = 0;

GNGAlgorithm::GNGAlgorithm(GNGDatabase* db, GNGAlgorithmControl * control, int start_number, double * boundingbox_origin, double * boundingbox_axis, double l, int max_nodes) :
m_g(), g_db(db), m_control(control), c(0), s(0),
m_max_nodes(max_nodes), m_max_age(200),
m_alpha(0.95), m_betha(0.9995), m_lambda(200),
m_eps_v(0.05), m_eps_n(0.0006), ug(boundingbox_origin, boundingbox_axis, l),
m_density_threshold(0.1), m_grow_rate(1.5),
errorHeap(m_g) {
    m_g.init(start_number);
    GNGGraphAccessHack::pool = m_g.getPool();
    ug.setDistFunction(GNGGraphAccessHack::dist);
    m_betha_powers_size = m_lambda * 10;
    m_betha_powers = new double[m_betha_powers_size];
    REP(i, m_betha_powers_size) m_betha_powers[i] = std::pow(m_betha, (double) (i));

    m_betha_powers_to_n = new double[m_max_nodes * 2];
    REP(i, m_max_nodes * 2) m_betha_powers_to_n[i] = std::pow(m_betha, m_lambda * (double) (i));

}

void GNGAlgorithm::RandomInit() {
    GNGExample ex1 = g_db -> drawExample();
    GNGExample ex2 = g_db -> drawExample();

    while (ex2 == ex1) ex2 = g_db -> drawExample();

    m_g.newNode(ex1.position);
    m_g.newNode(ex2.position);
    ug.insert(m_g[0]->position, 0);
    ug.insert(m_g[1]->position, 1);

    SetErrorNew(m_g[0], 0.0);
    SetErrorNew(m_g[1], 0.0);

}

void GNGAlgorithm::AddNewNode() {
    using namespace std;



    GNGNode ** error_nodes_new = LargestErrorNodesLazy();
    /*  
      double max_error=-1;
        
      REP(i,m_g.getMaximumIndex()){
            
          if(m_g[i]->occupied){
              FixErrorNew(m_g[i]);
              cout<<m_g[i]->error_new<<",";
              max_error = std::max(max_error, m_g[i]->error_new);
          }
      }
      cout<<endl;
      if(max_error != error_nodes_new[0]->error_new) {
          REPORT(max_error);REPORT(error_nodes_new[0]->nr);
          REPORT(error_nodes_new[0]->error_new);
      }
         
        
      max_error=-1;
      GNGNode ** error_nodes_new2=LargestErrorNodes();
     */
    /*
    REP(i,m_g.getMaximumIndex()+1){
        //cout<<m_g[i]->error<<",";
        if(m_g[i]->occupied){
              
            max_error = std::max(max_error, m_g[i]->error);
        }
    }
    //cout<<endl;
    if(max_error != error_nodes_new[0]->error) {
        REPORT(max_error)
            ;REPORT(error_nodes_new[0]->nr);
        REPORT(error_nodes_new[0]->error);
    }
     */
    if (m_max_nodes <= m_g.getNumberNodes()) {
        delete[] error_nodes_new;
        return;
    }

    double position[GNG_DIM];
    for (int i = 0; i < GNG_DIM; ++i)
        position[i] = (error_nodes_new[0]->position[i] + error_nodes_new[1]->position[i]) / 2;



    int new_node_index = m_g.newNode(&position[0]);

    ug.insert(m_g[new_node_index]->position, new_node_index);

    //dbg.push_back(4,"GNGAlgorith::AddNewNode::added "+to_string(*m_g[new_node_index]));

    m_g.removeEdge(error_nodes_new[0]->nr, error_nodes_new[1]->nr);

    //dbg.push_back(3,"GNGAlgorith::AddNewNode::removed edge beetwen "+to_string(error_nodes_new[0]->nr)+" and"+to_string( error_nodes_new[1]->nr));
    //dbg.push_back(2,"GNGAlgorithm::AddNewNode::largest error node after removing edge : "+to_string(*error_nodes_new[0]));

    m_g.addUDEdge(error_nodes_new[0]->nr, new_node_index);
    m_g.addUDEdge(new_node_index, error_nodes_new[1]->nr);

    //dbg.push_back(3,"GNGAlgorith::AddNewNode::add edge beetwen "+to_string(error_nodes_new[0]->nr)+" and"+to_string(new_node_index));
    //dbg.push_back(3,"GNGAlgorith::AddNewNode::add edge beetwen "+to_string(error_nodes_new[1]->nr)+" and"+to_string(new_node_index));

    //DecreaseError(error_nodes_new[0]);
    //DecreaseError(error_nodes_new[1]);

    DecreaseErrorNew(error_nodes_new[0]);
    DecreaseErrorNew(error_nodes_new[1]);

    //SetError(m_g[new_node_index],(error_nodes_new[0]->error + error_nodes_new[1]->error)/2);
    SetErrorNew(m_g[new_node_index], (error_nodes_new[0]->error + error_nodes_new[1]->error) / 2);

    delete[] error_nodes_new;
}

void GNGAlgorithm::Adapt(GNGExample * ex) {

    Time t1(boost::posix_time::microsec_clock::local_time());


    int * nearest_index = ug.findNearest(ex->position, 2); //TwoNearestNodes(ex->position);

    //GNGNode ** nearest = TwoNearestNodes(ex->position);
    GNGNode * nearest[2] = {m_g[nearest_index[1]], m_g[nearest_index[0]]};

    Time t2(boost::posix_time::microsec_clock::local_time());

    TimeDuration dt = t2 - t1;

    times["adapt1"] += dt.total_microseconds();

    t1 = boost::posix_time::microsec_clock::local_time();



    //GNGNode ** nearest = TwoNearestNodes(ex->position);

    //if(m_g.getNumberNodes()>9000){ cout<<"ok\n";}

    //ug.print3d();
    /*
    REP(i,m_g.getMaximumIndex()+1){
        if(m_g.getDist(m_g[i]->position,ex->position)<m_g.getDist(nearest[0]->position,ex->position)){
            cout<<"XXXXXXXXXXXXXXXXXXXX\n";
            cout<<m_g.getDist(nearest[0]->position,ex->position)<<">"<<m_g.getDist(m_g[i]->position,ex->position)<<endl;
            //dbg.push_back(3,"GNGAlgorithm::Adapt:: found nodes "+to_string(nearest_index[0])+" "+to_string(nearest_index[1]));
            double dist2=m_g.getDist(nearest[1]->position,ex->position);
            //dbg.push_back(3,"GNGAlgorithm::Adapt:: found node in distance (second) "+to_string(dist2));      
           // ug.print3d();
           // ug.findTwoNearest(ex->position,true);
            cout<<"MISSED "<<i<<endl;
            cout<<"FOUND "<<nearest[0]->nr<<endl;
           // cout<<ug.find(m_g[i]->position)<<endl;
           // write_array(m_g[i]->position,m_g[i]->position+3);

        }
    }

        
    REP(i,m_g.getMaximumIndex()+1){
        if(m_g[i]->occupied && m_g.getDist(m_g[i]->position,ex->position)<m_g.getDist(nearest[1]->position,ex->position) && i!=nearest[0]->nr){
            cout<<"XXXXXXXXXXXXXXXXXXXX\n";
            cout<<nearest[1]->nr<<" "<<nearest[0]->nr<<" "<<i<<endl;
            cout<<m_g.getDist(nearest[1]->position,ex->position)<<">"<<m_g.getDist(m_g[i]->position,ex->position)<<endl;
        }
    }
     */
    //dbg.push_back(4,"GNGAlgorith::Adapt::found nearest nodes to the drawn example "+to_string(*nearest[0])+" "+to_string(*nearest[1]));


    double error = m_g.getDist(nearest[0]->position, ex->position);

    //dbg.push_back(3,"GNGAlgorith::Adapt::increasing error");
    //  IncreaseError(nearest[0],error);
    IncreaseErrorNew(nearest[0], error);

    //dbg.push_back(3,"GNGAlgorith::Adapt::accounted for the error");

    ug.remove(nearest[0]->position);
    for (int i = 0; i < GNG_DIM; ++i) {
        nearest[0]->position[i] += m_eps_v * (ex->position[i] - nearest[0]->position[i]);
    }
    ug.insert(nearest[0]->position, nearest[0]->nr);


    if (nearest[0]->edgesCount)
        FOREACH(edg, *(nearest[0]->edges)) {
        ug.remove(m_g[edg->nr]->position);
        for (int i = 0; i < GNG_DIM; ++i) {
            m_g[edg->nr]->position[i] += m_eps_n * (ex->position[i] - m_g[edg->nr]->position[i]);
        }
        ug.insert(m_g[edg->nr]->position, edg->nr);
    }

    //dbg.push_back(4,"GNGAlgorith::Adapt::position of the winner and neighbour mutated");
    if (!m_g.isEdge(nearest[0]->nr, nearest[1]->nr)) {
        m_g.addUDEdge(nearest[0]->nr, nearest[1]->nr);
        //dbg.push_back(4,"GNGAlgorith::Adapt::added edge beetwen "+to_string(nearest[0]->nr)+ " and " +to_string(nearest[1]->nr));
    }


    //cout<<(*nearest[0])<<endl;
    bool BYPASS = false;

    FOREACH(edg, *(nearest[0]->edges)) {
        //  cout<<edg->nr<<",";
        if (BYPASS) {
            --edg;
            BYPASS = false;
        }

        edg->age++;
        edg->rev->age++;

        if (edg->nr == nearest[1]->nr) {
            edg->age = 0;
            edg->rev->age = 0;
        }

        if (edg->age > m_max_age) {
            // cout<<"DELETION";
            //dbg.push_back(3,"GNGAlgorith::Adapt::Removing aged edge "+to_string(nearest[0]->nr)+" - "+to_string(edg->nr));
            int nr = edg->nr;
            edg = m_g.removeEdge(nearest[0]->nr, edg);
            if (edg == nearest[0]->edges->end()) break;
            BYPASS = true;
            if (m_g[nr]->edgesCount == 0) {
                ug.remove(m_g[nr]->position);
                m_g.deleteNode(nr);
            }
            if (m_g[nearest[0]->nr]->edgesCount == 0) {
                ug.remove(m_g[nearest[0]->nr]->position);
                m_g.deleteNode(nearest[0]->nr);
            }
            //dbg.push_back(3,"GNGAlgorith::Adapt::Removal completed");
            //dbg.push_back(2,to_string(*nearest[0]));

        }
    }

    // DecreaseAllErrors();
    DecreaseAllErrorsNew();

    t2 = boost::posix_time::microsec_clock::local_time();
    dt = t2 - t1;

    times["adapt2"] += dt.total_microseconds();
    delete[] nearest_index;
}



int GNGAlgorithm::CalculateAccumulatedError() {
    int maximumIndex = m_g.getMaximumIndex();
    m_accumulated_error = 0.0;

    REP(i, maximumIndex + 1) {

        if (m_g[i]->occupied) {
            m_accumulated_error += m_g[i]->error;
        }
    }
    return m_accumulated_error;
}

int GNGAlgorithm::CalculateAccumulatedErrorNew() {
    int maximumIndex = m_g.getMaximumIndex();
    m_accumulated_error = 0.0;

    REP(i, maximumIndex + 1) {

        if (m_g[i]->occupied) {
            m_accumulated_error += m_g[i]->error_new;
        }
    }
    return m_accumulated_error;
}

void GNGAlgorithm::TestAgeCorrectness() {
    int maximumIndex = m_g.getMaximumIndex();

    REP(i, maximumIndex + 1) {

        if (m_g[i]->occupied && m_g[i]->edgesCount) {

            FOREACH(edg, *(m_g[i]->edges)) {
                if (edg->age > m_max_age) {
                    cout << "XXXXXXXXXXXXXXXXX\n";
                    cout << *(m_g[i]) << endl;
                }
                //cout<<m_g.getDistEdge(i,edg)<<endl;
                if (m_g.getDistEdge(i, edg) > 0.2) {
                    cout << (*m_g[i]) << " TO " << edg->nr << endl;
                }
            }
        }
    }
}

void GNGAlgorithm::ResizeUniformGrid() {
    //dbg.push_back(6,"GNGAlgorithm::Resize Uniform Grid");
    ug.new_l(ug.getCellLength() / m_grow_rate);
    int maximum_index = m_g.getMaximumIndex();

    REP(i, maximum_index + 1) {
        if (m_g[i]->occupied) ug.insert(m_g[i]->position, m_g[i]->nr);
    }
}

GNGNode ** GNGAlgorithm::LargestErrorNodes() {
    int j = 0;
    GNGNode ** largest = new GNGNode*[2];

    largest[0] = 0;
    largest[1] = 0;
    double error = -1.0;

    if (largest[0]->edgesCount == 0) {
        //removing the node
        m_g.deleteNode(largest[0]->nr);
        return largest;
    }

    FOREACH(edg, *(largest[0]->edges)) {
        ++j;

        if (j == 1) {
            largest[1] = m_g[edg->nr];
            error = largest[1]->error;
            continue;
        }


        double new_error = m_g[edg->nr]->error;
        if (error < new_error) {
            error = new_error;
            largest[1] = m_g[edg->nr];
        }
    }
    return largest;
}

GNGNode ** GNGAlgorithm::TwoNearestNodes(double * position) { //to the example

    GNGNode ** nearest = new GNGNode*[2];

    nearest[0] = 0;
    nearest[1] = 0;

    int start_index = 0;

    while (!m_g[start_index]->occupied) ++start_index;

    double dist = m_g.getDist(position, m_g[start_index]->position);
    nearest[0] = m_g[start_index];

    //dbg.push_back(1,"GNGAlgorithm::starting search of nearest node from "+to_string(start_index));

    for (int i = start_index + 1; i <= m_g.getMaximumIndex(); ++i) { //another idea for storing list of actual nodes?

        if (m_g[i]->occupied) {

            double new_dist = m_g.getDist(position, m_g[i]->position);

            if (dist > new_dist) {
                dist = new_dist;

                nearest[0] = m_g[i];
            }
        }
    }

    start_index = 0;

    while (!m_g[start_index]->occupied || start_index == nearest[0]->nr) ++start_index;
    dist = m_g.getDist(position, m_g[start_index]->position);
    nearest[1] = m_g[start_index];

    for (int i = start_index + 1; i <= m_g.getMaximumIndex(); ++i) { //another idea for storing list of actual nodes?

        if (m_g[i]->occupied && i != nearest[0]->nr) {

            double new_dist = m_g.getDist(position, m_g[i]->position);

            if (dist > new_dist) {
                dist = new_dist;

                nearest[1] = m_g[i];
            }
        }
    }

    //dbg.push_back(1,"search successful and nearest[1]= "+to_string(nearest[1]));
    return nearest;
}

void GNGAlgorithm::runAlgorithm() { //1 thread needed to do it (the one that computes)
    c = 0;
    s = 0;
    boost::posix_time::microseconds work(1000), waitsleep(40);
    bool waited = false;
    Time t1, t2, t3, t4;
    TimeDuration dt;
    RandomInit();

    t3 = boost::posix_time::microsec_clock::local_time();
    int iteration = 0;
    while (1) {//(m_g.getNumberNodes() < m_max_nodes) {
        m_control->checkPause();
        for (s = 0; s < m_lambda; ++s) { //global counter!!
            // if(iteration>3) return;
            ++iteration;
            GNGGraphAccessHack::pool = m_g.getPool(); //bad design
            //dbg.push_back(-3,"GNGAlgorithm::draw example");
            GNGExample ex = g_db->drawExample();
            Adapt(&ex);
            // ug.print3d();
        }
        t1 = boost::posix_time::microsec_clock::local_time();
        AddNewNode();
        if (ug.getDensity() > m_density_threshold) ResizeUniformGrid();
        t2 = boost::posix_time::microsec_clock::local_time();
        dt = t2 - t1;
        times["resize"] += dt.total_microseconds();
        ++c; //epoch
   

    }
    t4 = boost::posix_time::microsec_clock::local_time();
    dt = t4 - t3;
    REPORT(times["adapt1"]);
    REPORT(times["adapt2"]);
    REPORT(times["resize"]);
    REPORT(dt.total_microseconds() / (double) 1000000);
}