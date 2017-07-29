/* 
 * Copyright (C) 2015-2016, Frank Lin - All Rights Reserved
 * Written by Frank P.Y. Lin 
 *
 * This file is part of TEPAPA, a Text mining-based Exploratory Pattern
 * Analyser for Prognostic and Associative factor discovery
 * 
 * TEPAPA is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TEPAPA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TEPAPA.  If not, see <http://www.gnu.org/licenses/>
 */
#include <math.h>
#include <stdio.h>
#include "matrix.h"



mat& mat::operator += (const mat& m2)
	{
	check_dimension(m2.rows, m2.cols);
	for (register int i=0; i<rows*cols; ++i) v[i] += m2.v[i];
	return *this;
	}
	
mat& mat::operator -= (const mat& m2)
	{
	check_dimension(m2.rows, m2.cols);
	for (register int i=0; i<rows*cols; ++i) v[i] -= m2.v[i];
	return *this;
	}

mat& mat::operator *= (mat_prec f)
	{
	for (register int i=0; i<rows*cols; ++i) v[i] *= f;
	return *this;
	}

mat& mat::operator /= (mat_prec f)
	{
	for (register int i=0; i<rows*cols; ++i) v[i] /= f;
	return *this;
	}


mat operator - (const mat& m1)
	{
	mat m(m1);
	for (register int i=0; i<m.rows * m.cols; ++i) m.v[i] = -m1.v[i];
	return m;
	}


mat operator + (const mat& m1, const mat& m2)
	{
	mat m(m1);
	m.check_dimension(m2.rows, m2.cols);
	for (register int i=0; i<m1.rows*m1.cols; ++i) m.v[i] += m2.v[i];
	return m;
	}
	
mat operator - (const mat& m1, const mat& m2)
	{
	mat m(m1);
	m.check_dimension(m2.rows, m2.cols);
	for (register int i=0; i<m1.rows*m1.cols; ++i) m.v[i] -= m2.v[i];
	return m;
	}

mat operator * (const mat& m1, mat_prec f)
	{
	mat m(m1);
	for (register int i=0; i<m1.rows*m1.cols; ++i) m.v[i] *= f;
	return m;
	}
	
mat operator / (const mat& m1, mat_prec f)
	{
	mat m(m1);
	for (register int i=0; i<m1.rows*m1.cols; ++i) m.v[i] /= f;
	return m;
	}


mat operator * (const mat& m1, const mat& m2)
	{
	mat m(m1.rows, m2.cols);
	assert(m1.cols == m2.rows);
	for(register int i=0; i<m1.rows; ++i)
		{
		for(int j=0; j<m2.cols; ++j)
			{
			mat_prec sum = 0;
			for(int k=0; k<m1.cols; ++k) sum += m1(i,k) * m2(k,j);
			m(i,j) = sum;
			}
		}
	return m;
	}




void mat::dump(const char* name) const
	{
	if(name[0]) printf("%s = (%d x %d)\n", name, rows, cols);
	for(register int i=0; i<rows; ++i) 
		{
		printf("[ ");
		for(int j=0; j<cols; ++j)
			{
			printf("%6.3g ", operator()(i,j));
			}
		printf("]\n");
		}
	}


mat mat::identity(int dim)
	{
	mat  r(dim,dim);
	for(register int i=0;i<dim;++i) r(i,i) = 1;
	return r;
	}


inline mat_prec bound(mat_prec x) 
	{
	if (x > 1e+150) x = 1e+300;
	else if (x < -1e+150) x = -1e+300;
	return x;
	}

mat& mat::row_op_add(int row_op1, int row_op2)
	{
	for(register int c=0; c<cols; ++c) { __v(row_op1, c) += __v(row_op2, c); }
	return *this;
	}

mat& mat::row_op_add(int row_op1, int row_op2, mat_prec f)
	{
	for(register int c=0; c<cols; ++c) 
		{
		register mat_prec x = bound( __v(row_op1, c) ) ;
		x += bound( __v(row_op2, c) ) * f;
		__v(row_op1, c) = bound( x );
		}
	return *this;
	}

mat& mat::row_op_mul(int row_op1, mat_prec f)
	{
	for(register int c=0; c<cols; ++c) 
		{
		register mat_prec x = bound( __v(row_op1, c) );
		x *= f;
		__v(row_op1, c) = bound( x );
		}
	return *this;
	}

mat& mat::row_op_swp(int row_op1, int row_op2)
	{
	for(register int c=0; c<cols; ++c) 
		{
		register mat_prec z =  __v(row_op1, c);
		__v(row_op1, c) = __v(row_op2, c) ;
		__v(row_op2, c) = z;
		}
	return *this;
	}


#define MATRIX_INVERT_CUTOFF (1e-300)

mat invert(const mat& src)
	{
	assert(src.rows == src.cols);
	int n = src.rows ;
	mat s = src;
	mat r = mat::identity(n);
	
	/* code stole from wikipedia, thank you for your generosity! */
	int lead = 0;
	for(int row = 0; row < n; row++)
		{
		int i = row;

		if(lead >= n) return r;

		while( fabs(s(i,lead)) <= MATRIX_INVERT_CUTOFF )
			{
			if(++i >= n)
				{
				i = row;
				if(++lead >= n) return r;
				}
			}
			
		s.row_op_swp(i, row); 
		r.row_op_swp(i, row);
		
		mat_prec v = s(row,lead);
// 		if (v == 0.0) v = MATRIX_INVERT_CUTOFF ;
		mat_prec f1 = (1.0/v);
		
		s.row_op_mul(i, f1);
		r.row_op_mul(i, f1);
		
		for(i = 0; i < n; i++) if(i != row)
			{
			mat_prec f2 = -s(i,lead);
			s.row_op_add(i, row, f2);
			r.row_op_add(i, row, f2);
			}
// 		s.dump("** s");
// 		r.dump("** r");
		lead++;
		}
	
	return r;
	}





mvec::mvec(const mat& m) : mat(m) { assert(m.mat_spec::c() == 1); }


mat_prec dot(const mvec& v1, const mvec& v2)
	{
	assert(v1.rows == v2.rows);
	mat_prec sum = 0.0;
	for(int i=0; i<v1.rows; ++i) sum += v1.v[i] * v2.v[i];
	return sum;
	}
	

mat_prec mvec::euclidean2() const
	{
	mat_prec tot = 0.0;
	for (int i=0; i<rows; ++i) tot += v[i] * v[i];
	return tot;
	}
	
mat_prec mvec::euclidean() const
	{
	return sqrt(euclidean2());
	}

mat_prec mvec::mean() const
	{
	mat_prec tot = 0.0;
	for (int i=0; i<rows; ++i) tot += v[i];
	return tot / mat_prec(rows);
	}

mat_prec mvec::var() const 
	{
	mat_prec d2 = 0.0;
	mat_prec mu = mean();
	
	for (int i=0; i<rows; ++i)
		{
		mat_prec x = v[i] - mu;
		d2 += x * x;
		}
	
	return (d2/rows);
	}

mat_prec mvec::ssq() const 
	{
	mat_prec d2 = 0.0;
	mat_prec mu = mean();
	
	for (int i=0; i<rows; ++i)
		{
		mat_prec x = v[i] - mu;
		d2 += x * x;
		}
	
	return d2;
	}

/*

///////////////////////////////////////////////////////

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_permutation.h>
#include <gsl/gsl_linalg.h>

mat invert_gsl(const mat& src)
	{
	int n = src.rows;
	gsl_matrix*  m         = gsl_matrix_calloc(n, n);
    gsl_matrix*  inverse   = gsl_matrix_alloc(n, n);
    gsl_permutation*  perm = gsl_permutation_alloc(n);
    
	for(int i=0; i<n; ++i)
		{
		for(int j=0; j<n; ++j)
			{
			gsl_matrix_set (m, i, j, src(i,j)) ;
			}
		}

	int s=0;
	
    gsl_linalg_LU_decomp(m, perm, &s);
	
    gsl_linalg_LU_invert(m, perm, inverse);

	mat retval(n,n);
	
	for(int i=0; i<n; ++i)
		{
		for(int j=0; j<n; ++j)
			{
			retval(i,j) = gsl_matrix_get (inverse, i, j);
			}
		}
	

	gsl_permutation_free(perm);
	gsl_matrix_free(inverse);
	gsl_matrix_free (m);
	
	return retval;
	}*/

/*
int main(void)
	{
	mat m(2,2);
	m(0,0) = 3;
	m(1,0) = 5;
	m(0,1) = 1;
	m(1,1) = 2;
	
	m.dump("m");
	
	invert_gsl(m).dump("m^-1");
	}*/
