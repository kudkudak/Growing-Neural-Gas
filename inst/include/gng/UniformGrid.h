
/* 
 * File:   UniformGrid.h
 * Author: staszek
 *
 * Created on August 19, 2012, 7:02 AM
 */

#ifndef UNIFORM_GRID_H
#define UNIFORM_GRID_H

#include "Utils.h"
#include <vector>
#include "GNGGlobals.h"
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <cmath>
#include <algorithm>
using namespace std;

namespace gmum {
/**
 * Is not thread safe !! Very important (speed purposes)
 */
template<class VectorContainer, class ListContainer, class T = int>
class UniformGrid {
	typedef ListContainer Node;
	typedef VectorContainer NodeArray;

public:
	/*** Maximum size pass which UG won't grow */
	static const int MAX_SIZE = 1000000;

	NodeArray m_grid;

	//global variables for search query
	int s_found_cells[4];
	double s_found_cells_dist[4];
	int s_search_query;

	int *s_center;
	int s_radius;
	int *s_pos;
	double *s_query;

	double (*m_dist_fnc)(T, double*); //distance function;

	double m_l;
	double m_h;

	double m_density;
	double m_density_threshold;
	double m_density_threshold_min;
	double m_grow_factor;

	int neighbourhood_size; //=3^d

	int gng_dim;

	double m_axis[GNG_MAX_DIM];

	int m_nodes;

	boost::shared_ptr<Logger> m_logger;

	int* m_dim; //number of uniform cells along certain axis

	int* m_tmp_int; //avoid alloc on heap all the time in calccell <- one thread!

	vector<int> m_neigh;

	double m_origin[GNG_MAX_DIM];

	int getIndex(int *p) {
		int value = p[0];
		double mul = m_dim[0];
		for (int i = 1; i < this->gng_dim; ++i) {
			value += p[i] * mul;
			mul *= m_dim[i];
		}

		return value;
	}
	int * calculateCell(const double *p) {
		//int * m_tmp_int = new int[this->gng_dim];
		for (int i = 0; i < this->gng_dim; ++i) {
			m_tmp_int[i] = (int) ((p[i] - m_origin[i]) / m_l);
		}
		return &m_tmp_int[0];
	}

	bool _inside(int x) {
		return (x) >= 0 && (x) < m_grid.size();
	}

	bool isZero(double x) {
		return x > -EPS && x < EPS;
	}
	void purge(double *origin, int* dim, double l);

	unsigned int calculate_cell_side(double axis, double l, int old_dim) {
		return max(old_dim + 1, (int) ((axis) / (l)) + 1);
	}
public:
	~UniformGrid() {
		delete[] s_center;
		delete[] s_pos;
		delete[] s_query;
		delete[] m_dim;
		delete[] m_tmp_int;
	}

	//TODO: extract constructor base
	UniformGrid(double * origin, int *dim, int gng_dim, double m_grow_factor =
			1.5, double m_density_threshold = 2.0,
			double m_density_threshold_min = 0.4,
			boost::shared_ptr<Logger> logger = boost::shared_ptr<Logger>()) :
			m_dist_fnc(0), gng_dim(gng_dim), m_density_threshold(
					m_density_threshold), m_density_threshold_min(
					m_density_threshold_min), m_grow_factor(m_grow_factor), m_logger(
					logger) {
		neighbourhood_size = int(pow(3.0, (double) gng_dim));

		this->m_density_threshold = m_density_threshold;
		this->m_density_threshold_min = m_density_threshold_min;
		this->m_grow_factor = m_grow_factor;

		s_center = new int[this->gng_dim];

		s_pos = new int[this->gng_dim];
		s_query = new double[this->gng_dim];
		m_dim = new int[this->gng_dim]; //number of uniform cells along certain axis
		m_tmp_int = new int[this->gng_dim]; //avoid alloc on heap all the time in calccell <- one thread!

		//Zero
		for (int i = 0; i < this->gng_dim; ++i)
			s_center[i] = s_pos[i] = s_query[i] = m_dim[i] = m_tmp_int[i] = 0;

		purge(origin, dim, -1.0);
	}

	UniformGrid(double * origin, double *axis, double l, int gng_dim,
			double m_grow_factor = 1.5, double m_density_threshold = 2.0,
			double m_density_threshold_min = 0.4,
			boost::shared_ptr<Logger> logger = boost::shared_ptr<Logger>()) :
			m_dist_fnc(0), gng_dim(gng_dim), m_density_threshold(
					m_density_threshold), m_density_threshold_min(
					m_density_threshold_min), m_grow_factor(m_grow_factor), m_logger(
					logger) {
		neighbourhood_size = int(pow(3.0, (double) gng_dim));

		s_center = new int[this->gng_dim];

		s_pos = new int[this->gng_dim];
		s_query = new double[this->gng_dim];
		m_dim = new int[this->gng_dim]; //number of uniform cells along certain axis
		m_tmp_int = new int[this->gng_dim]; //avoid alloc on heap all the time in calccell <- one thread!

		//Zero
		for (int i = 0; i < this->gng_dim; ++i)
			s_center[i] = s_pos[i] = s_query[i] = m_dim[i] = m_tmp_int[i] = 0;

		purge(origin, axis, l);

	}

	/** Calculates new size given growth factor */
	long int calculate_new_size(double *origin, double *axis, double l) {
		unsigned long int result = 1;

		REP(i, this->gng_dim)
		{
			result *= calculate_cell_side(axis[i], l, m_dim[i]);
			if (result > UniformGrid::MAX_SIZE)
				return -1;
		}
		return result;
	}

	/** Calculates if growing the grid will payoff*/
	bool check_grow() {
		unsigned long int result = this->calculate_new_size(m_origin, m_axis,
				m_l / m_grow_factor);

		if (result == -1)
			return false;

		double avg_density = m_nodes / (double) m_grid.size();
		double new_avg_density = m_nodes
				/ (double) (this->calculate_new_size(m_origin, m_axis,
						m_l / m_grow_factor));

		DBG(m_logger,2, "avg_desnity = "+to_str(avg_density)); DBG(m_logger,2, "new_avg_density = "+to_str(new_avg_density));

		return avg_density > m_density_threshold
				&& new_avg_density > m_density_threshold_min;
	}

	void print3d();

	bool searchSuccessful(double min_dist = -1) {
		REP(i, s_search_query)
		{
			if (s_found_cells[i] == -1 || s_found_cells_dist[i] > min_dist)
				return false;
		}
		return true;
	}

	double getCellLength() const {
		return m_l;
	}

	double getDensity() const {
		return m_density;
	}

	int getCapacity() const {
		return SIZE(m_grid);
	}

	int getNodes() const {
		return m_nodes;
	}

	int getDimension(int axis) const {
		return m_dim[axis];
	}

	void new_l(double l);

	void purge(double *origin, double *axis, double l);

	//mutates pos!

	int insert(double *p, T x);
	bool remove(double *p);
	T find(double *p);

	vector<T> findNearest(const double *p, int n = 2);

	void setDistFunction(double (*dist_fnc)(T, double*)) {
		m_dist_fnc = dist_fnc;
	}

	//jedna funkcja uzywa m_tmp_int

	void scanCell(int k, double* query);
	void crawl(int current_dim, int fixed_dim);
	bool scanCorners();

};

#include "UniformGrid.hpp"

}

#endif
