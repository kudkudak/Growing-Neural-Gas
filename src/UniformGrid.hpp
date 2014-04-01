/* 
 * File:   UniformGrid.hpp
 * Author: staszek
 *
 * Created on September 3, 2012, 9:03 AM
 */


template<class VectorContainer, class ListContainer, class T>
void UniformGrid<VectorContainer, ListContainer, T>::print3d() {
        using namespace std;
        int index[3];

        REP(k, m_dim[2]) {
            
            REP(j, m_dim[1]) {

                REP(i, m_dim[0]) {
                    index[0] = i;
                    index[1] = j;
                    index[2] = k;

                    int inner_index = getIndex(index);

                    cout << inner_index;
                    //if(TMP[inner_index]) cout<<"::"; 
                    cout << ":";
                    FOREACH(x, m_grid[inner_index]) cout << *x << ",";
                    cout << "\t";


                }
                cout << endl;
            }
            cout << "\n\n\n";
        }


}

template<class VectorContainer, class ListContainer, class T>
void  UniformGrid<VectorContainer, ListContainer, T>::new_l(double l) {
    double *org = new double[this->gng_dim];
    double *axis = new double[this->gng_dim];
    memcpy(org, m_origin, this->gng_dim * sizeof (double));
    memcpy(axis, m_axis, this->gng_dim * sizeof (double));
    purge(org, axis, l);
    delete[] org;
    delete[] axis;
}
template<class VectorContainer, class ListContainer, class T>
void UniformGrid<VectorContainer, ListContainer, T>::purge(double *origin, double *axis_array, double l) {
    int * dim = new int[this->gng_dim];
    memcpy(m_axis, axis_array,  sizeof (double) *this->gng_dim);

    // If length has not been specified, it means that we want to have minimum number of cells possible
    // Which amounts to picking the smallest axis
    if(l == -1.0){
        double l_min = axis_array[0];
        REP(i, this->gng_dim)
                l_min = min(l_min, axis_array[0]);
        l = l_min * 1.01; // For numerical stability
    }
    
    REP(i, this->gng_dim){ 
        dim[i] = calculate_cell_side(axis_array[i], l, m_dim[i]);
        
        DBG(6, "New dimension "+to_string(dim[i]));
    }
    
    purge(origin, dim, l);
    delete[] dim;
}
template<class VectorContainer, class ListContainer, class T>
void UniformGrid<VectorContainer, ListContainer, T>::purge(double *origin, int* dim, double l) {
    m_l = l;
    memcpy(&m_dim[0], dim, sizeof (int) *this->gng_dim);
    memcpy(&m_origin, origin, sizeof (double) *this->gng_dim);

    m_density = 0.0;
    m_density_threshold = 0.1;
    m_grow_factor = 1.5;
    m_nodes = 0;

    m_grid.clear();

    int new_size = 1;

    REP(i, this->gng_dim) {
        new_size *= m_dim[i];
        REPORT(new_size);
        REPORT(m_dim[i]);
    }

    DBG(3, "UniformGrid::purge new size "+to_string<int>(new_size));
 
    m_grid.resize(new_size);
}



template<class VectorContainer, class ListContainer, class T>
void UniformGrid<VectorContainer, ListContainer, T>::scanCell(int k, double* query) {
    double dist_candidate;
    
    // Not implemented search routine for > 2
    if(s_search_query != 2) throw "Not implemented for >2 search query..";
    
    FOREACH(node, m_grid[k]){
   
        dist_candidate = m_dist_fnc(*node, query);

        
        //
        if (*node != s_found_cells[1] &&
                (s_found_cells_dist[0] < 0 || dist_candidate <= s_found_cells_dist[0]) ){

            //Overwrite worst 
            s_found_cells_dist[0] = dist_candidate;
            s_found_cells[0] = *node;           
                   
            //Swap it to the right place
            for (int j = 1; j < s_search_query; ++j) {
                if (s_found_cells_dist[j] <0 || dist_candidate <= s_found_cells_dist[j]) {
                    std::swap(s_found_cells[j], s_found_cells[j - 1]);
                    std::swap(s_found_cells_dist[j], s_found_cells_dist[j - 1]);
                    
                }
            }

            
        }


    }
}

template<class VectorContainer, class ListContainer, class T>
void UniformGrid<VectorContainer, ListContainer, T>::crawl(int current_dim, int fixed_dim) {

    if (current_dim == fixed_dim) {
        if (current_dim >= this->gng_dim - 1) {					
            scanCell(getIndex(s_pos), s_query);		
        }            //skip current dimension
        else crawl(current_dim + 1, fixed_dim);
    }
    else {
        int from, to;

        //skip corners
        if (current_dim < fixed_dim) {
            from = std::max(s_center[current_dim] - s_radius + 1, 0);
            to = std::min(s_center[current_dim] + s_radius - 1, m_dim[current_dim] - 1);
        } else {
            from = std::max(s_center[current_dim] - s_radius, 0);
            to = std::min(s_center[current_dim] + s_radius, m_dim[current_dim] - 1);
        }

        for (int i = from; i <= to; ++i) {


            s_pos[current_dim] = i;

            if (current_dim == this->gng_dim - 1) {					
                scanCell(getIndex(s_pos), s_query);
            } else {
                crawl(current_dim + 1, fixed_dim);
            }
        }


    }


    return;
}

/** Uses cached values (s_pos, s_center, s_radius) to search cells */
template<class VectorContainer, class ListContainer, class T>
bool UniformGrid<VectorContainer, ListContainer, T>::scanCorners() {
    
    
    int left, right;

    bool scanned = false;


    memcpy(s_pos, s_center, sizeof (int) *this->gng_dim);

    REP(i, this->gng_dim) {
        left = s_center[i] - s_radius;
        right = s_center[i] + s_radius;



        if (s_center[i] - s_radius >= 0) {
            s_pos[i] = left;
            crawl(0, i);
            scanned = true;
        }
        if (s_center[i] + s_radius < m_dim[i]) {
            s_pos[i] = right;
            crawl(0, i);
            scanned = true;
        }

        s_pos[i] = s_center[i];
    }

    return scanned;
}




//mutates pos!

template<class VectorContainer, class ListContainer, class T>
int UniformGrid<VectorContainer, ListContainer, T>::insert(double *p, T x) {
    //memcpy(&m_copy[0],p,sizeof(double)*this->gng_dim);
    int * index = calculateCell(p);
    int k = getIndex(index);
    
    if(!_inside(k)) return 0;

    
    DBG(0,"UniformGrid:: "+to_string(k)+" inser");
    
    m_grid[k].push_back(x);
    m_nodes++;
    m_density = (double) m_nodes / (double) SIZE(m_grid);
    return k;
}

template<class VectorContainer, class ListContainer, class T >
std::vector<T> UniformGrid<VectorContainer, ListContainer, T>::findNearest(const double *p, int n) { //returns indexes (values whatever)
    s_search_query = n;

    
    
    DBG(2,"UniformGird:: search for "+write_cnt_str(p,p+this->gng_dim));
    

    int * center = calculateCell(p);
    
    REP(i,n){
        s_found_cells[i] = -1;
        s_found_cells_dist[i] = -1;
    }
    memcpy(s_center, center, sizeof (int) *this->gng_dim);

    memcpy(s_query, p, sizeof (double) *this->gng_dim);

    int center_id = getIndex(center);
    
    
    int size = SIZE(m_grid);
    DBG(2,"UniformGird:: search for "+to_string(center_id)+ "whereas size of the grid = "+to_string(size)+ "nodes = "+to_string(m_nodes));
    
    //Check if inside uniform grid
    if(!_inside(center_id)) {
        DBG(6, "UniformGird:: search for " + write_cnt_str(p, p + this->gng_dim));  
        DBG(6,"UniformGird:: search for "+to_string(center_id));
        DBG(6, "UniformGrid:: size="+to_string(m_grid.size()));
        vector<int> returned_value;
        returned_value.push_back(-1);
        returned_value.push_back(-1);
        return returned_value;
    
    }
    double border, border_squared, tmp;
    s_found_cells_dist[0] = s_found_cells_dist[1] = -1;
    s_found_cells[0] = s_found_cells[1] = -1;

    //init of the search
    scanCell(center_id, s_query);

    
    if(s_found_cells[0] == s_found_cells[1] && s_found_cells[0] != -1){
        DBG(10, "UniformGrid:: Found two same nodes in one cell!!");
    }

    DBG(2, "UniformGrid:: Found 0: "+to_string(s_found_cells[0]));
    DBG(2, "UniformGrid:: Found 1: "+to_string(s_found_cells[1]));
    
    for (int i = 0; i < this->gng_dim; ++i) {
        tmp = abs((p[i] - m_origin[i] - center[i] * m_l)) < abs((p[i] - m_origin[i] - (center[i] + 1) * m_l)) ?
                abs((p[i] - m_origin[i] - center[i] * m_l)) :
                abs((p[i] - m_origin[i] - (center[i] + 1) * m_l));
        // cout << tmp << endl;
        if (border > tmp || i == 0) border = tmp;
    }
    
    DBG(0,"UniformGird:: initializing serach");
    

    border_squared = border*border;

    

    DBG(0,"UniformGird::search "+write_cnt_str(s_found_cells,s_found_cells+2));
    

    s_radius = 0;
    
            DBG(2, "UniformGird:: scanned straightforward cell proceeding to crawling");
     Time t1(boost::posix_time::microsec_clock::local_time());

    
     //No more cells to search
    if(m_grid.size()==1){
            std::vector<T> ret(2);
            ret[1] = s_found_cells[0];
            ret[0] = s_found_cells[1];

            return ret;
     }
     
     
    while
        (
            !searchSuccessful(border_squared)
            ) {
        DBG(3, "UniformGird:: scanning radius "+to_string(s_radius));
        ++s_radius;
        border += m_l;
        border_squared = border*border;


        if (!scanCorners()) break; //false if no cells to check (no cell checked)
    }
    
            DBG(2, "UniformGird:: scanning done");
    
        Time t2(boost::posix_time::microsec_clock::local_time());
        TimeDuration dt = t2 - t1;
        
        

//        cout<<"Uniform grid search time "<< dt.total_microseconds()<<endl;

    std::vector<T> ret(2);
    
    //Reversed array - see scanCell
    ret[1] = s_found_cells[0];
    ret[0] = s_found_cells[1];
  
    return ret;
}

template<class VectorContainer, class ListContainer, class T >
bool UniformGrid<VectorContainer, ListContainer, T>::remove(double *p) { //returns indexes (values whatever)
    int * cell = calculateCell(p);
    int index = getIndex(cell);

    FOREACH(node, m_grid[index]) {
        if (isZero(m_dist_fnc(*node, p))) {
            m_grid[index].erase(node);
            --m_nodes;
            return true;
        }
    }
    return false;
}
