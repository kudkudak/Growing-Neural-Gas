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
    double *org = new double[GNG_DIM];
    double *axis = new double[GNG_DIM];
    memcpy(org, m_origin, GNG_DIM * sizeof (double));
    memcpy(axis, m_axis, GNG_DIM * sizeof (double));
    purge(org, axis, l);
    delete[] org;
    delete[] axis;
}

template<class VectorContainer, class ListContainer, class T>
void UniformGrid<VectorContainer, ListContainer, T>::scanCell(int k, double* query) {
    double dist_candidate;
    FOREACH(node, m_grid[k]){
   
        dist_candidate = m_dist_fnc(*node, query);

        
        if (s_found_cells_dist[0] <0 || dist_candidate <= s_found_cells_dist[0]) {
            s_found_cells_dist[0] = dist_candidate;
            s_found_cells[0] = *node;

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
        if (current_dim >= GNG_DIM - 1) {					
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

            if (current_dim == GNG_DIM - 1) {					
                scanCell(getIndex(s_pos), s_query);
                //TMP[getIndex(pos)]=true;
            } else {
                crawl(current_dim + 1, fixed_dim);
            }
        }


    }


    return;
}

template<class VectorContainer, class ListContainer, class T>
bool UniformGrid<VectorContainer, ListContainer, T>::scanCorners() {

    int left, right;

    bool scanned = false;


    memcpy(s_pos, s_center, sizeof (int) *GNG_DIM);

    REP(i, GNG_DIM) {
        left = s_center[i] - s_radius;
        right = s_center[i] + s_radius;



        if (s_center[i] - s_radius >= 0) {
            s_pos[i] = left;
            //cout<<"crawler to "<<getIndex(pos)<<endl;
            //write_array(pos,pos+GNG_DIM);
            crawl(0, i);
            scanned = true;
        }
        if (s_center[i] + s_radius < m_dim[i]) {
            s_pos[i] = right;
            //cout<<"crawler to "<<getIndex(pos)<<endl;
            //write_array(pos,pos+GNG_DIM);
            crawl(0, i);
            scanned = true;
        }

        s_pos[i] = s_center[i];
    }

    return scanned;
}

template<class VectorContainer, class ListContainer, class T>
void UniformGrid<VectorContainer, ListContainer, T>::purge(double *origin, double *axis, double l) {
    int * dim = new int[GNG_DIM];
    memcpy(m_axis, axis, sizeof (double) *GNG_DIM);

    REP(i, GNG_DIM) {
        //REPORT(((axis[i] - origin[i]) / (l)));
        dim[i] = (int) ((axis[i] - origin[i]) / (l)) + 1;
    }
    purge(origin, dim, l);
    delete[] dim;
}

template<class VectorContainer, class ListContainer, class T>
void UniformGrid<VectorContainer, ListContainer, T>::purge(double *origin, int* dim, double l) {

    m_l = l;
    memcpy(&m_dim[0], dim, sizeof (int) *GNG_DIM);
    memcpy(&m_origin, origin, sizeof (double) *GNG_DIM);

    write_array(m_dim, m_dim + GNG_DIM); //to jest ten cout

    m_density = 0.0;
    m_density_threshold = 0.1;
    m_grow_factor = 1.5;
    m_nodes = 0;

    m_grid.clear();

    int new_size = 1;

    REP(i, GNG_DIM) {
        new_size *= m_dim[i];
    }

#ifdef DEBUG
    dbg.push_back(3, "UniformGrid::purge new size "+to_string<int>(new_size));
#endif
    
    m_grid.resize(new_size);
 
}


//mutates pos!

template<class VectorContainer, class ListContainer, class T>
int UniformGrid<VectorContainer, ListContainer, T>::insert(double *p, T x) {
    //memcpy(&m_copy[0],p,sizeof(double)*GNG_DIM);
    int * index = calculateCell(p);
    int k = getIndex(index);
    
    if(!_inside(k)) return 0;

    #ifdef DEBUG
    dbg.push_back(0,"UniformGrid:: "+to_string(k)+" inser");
    #endif
    m_grid[k].push_back(x);
    m_nodes++;
    m_density = (double) m_nodes / (double) SIZE(m_grid);
    return k;
}

template<class VectorContainer, class ListContainer, class T >
std::vector<T> UniformGrid<VectorContainer, ListContainer, T>::findNearest(const double *p, int n) { //returns indexes (values whatever)
    s_search_query = n;

    
    #ifdef DEBUG
    dbg.push_back(2,"UniformGird:: search for "+write_cnt_str(p,p+GNG_DIM));
    #endif

    int * center = calculateCell(p);
    
    REP(i,n){
        s_found_cells[i]=-1;
        s_found_cells_dist[i]=-1;
    }
    memcpy(s_center, center, sizeof (int) *GNG_DIM);

    memcpy(s_query, p, sizeof (double) *GNG_DIM);

    int center_id = getIndex(center);
    
    #ifdef DEBUG
    int size = SIZE(m_grid);
    dbg.push_back(2,"UniformGird:: search for "+to_string(center_id)+ "whereas size of the grid = "+to_string(size)+ "nodes = "+to_string(m_nodes));
    #endif

    if(!_inside(center_id)) {
        #ifdef DEBUG
                dbg.push_back(5, "UniformGird:: search for " + write_cnt_str(p, p + GNG_DIM));
        #endif
            #ifdef DEBUG
             dbg.push_back(5,"UniformGird:: search for "+to_string(center_id));
             #endif
        vector<int> returned_value;
        returned_value.push_back(-1);
        returned_value.push_back(-1);
        return returned_value;
    
    }
    double border, border_squared, tmp;
    s_found_cells_dist[0] = s_found_cells_dist[1] = -1;
    s_found_cells[0] = s_found_cells[1] = -1;

    //REP(i,10000) TMP[i]=false;
    #ifdef DEBUG
    dbg.push_back(0,"UniformGird:: initializing serach");
    #endif
    //init of the search
    scanCell(center_id, s_query);
    #ifdef DEBUG
    dbg.push_back(0,"UniformGird:: initializing serach");
    #endif

    for (int i = 0; i < GNG_DIM; ++i) {
        tmp = abs((p[i] - m_origin[i] - center[i] * m_l)) < abs((p[i] - m_origin[i] - (center[i] + 1) * m_l)) ?
                abs((p[i] - m_origin[i] - center[i] * m_l)) :
                abs((p[i] - m_origin[i] - (center[i] + 1) * m_l));
        // cout << tmp << endl;
        if (border > tmp || i == 0) border = tmp;
    }
    #ifdef DEBUG
    dbg.push_back(0,"UniformGird:: initializing serach");
    #endif

    border_squared = border*border;

    #ifdef DEBUG

    dbg.push_back(0,"UniformGird::search "+write_cnt_str(s_found_cells,s_found_cells+2));
    #endif

    s_radius = 0;
    #ifdef DEBUG
            dbg.push_back(2, "UniformGird:: scanned straightforward cell proceeding to crawling");
    #endif
    while
        (
            !searchSuccessful(border_squared)
            ) {
        
        ++s_radius;
        border += m_l;
        border_squared = border*border;


        if (!scanCorners()) break; //false if no cells to check (no cell checked)
    }
    #ifdef DEBUG
            dbg.push_back(2, "UniformGird:: scanning done");
    #endif


    std::vector<T> ret(2);
    ret[0] = s_found_cells[0];
    ret[1] = s_found_cells[1];
  
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
