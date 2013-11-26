/* 
 * File:   GNGGlobals.h
 * Author: staszek
 *
 * Created on August 19, 2012, 2:30 PM
 */

#ifndef GNGGLOBALS_H
#define	GNGGLOBALS_H



#define GNGNODEPOOL_MODIFIED 0 //??
#define GNG_MAX_DIM 20
#define GNG_MAX_DIMENSION 100 /**< Maximum dimensionality of GNG - important for speed issues 
if SH based model is used (whole graph in SH) */


const double EPS = 1E-9;



//TODO: remove those!! (move to GNGConfiguration)
extern int GNG_DIM; /**< Dimensionality of GNG instance (first GNG_DIM from GNG_EX_DIM is taken for calculation)*/
extern int GNG_EX_DIM; /**< Dimensionality of GNG example (can have more dimensions)*/


#endif	/* GNGGLOBALS_H */

