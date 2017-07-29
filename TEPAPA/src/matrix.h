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
#ifndef __MATRIX_H
#define __MATRIX_H 1

#include <vector>
#include <assert.h>

using namespace std;


template <typename T> class mat_spec {
	protected:
	int rows, cols;
	vector<T> v;

	T& __v(int r, int c) { return v[r*cols+c]; }
	void check_dimension(int r, int c) const { 
		assert (r == rows && c == cols); 
		}
	
	public:
	mat_spec(int pr=0, int pc=0) {
		rows = pr;
		cols = pc;
		if (rows*cols) {
			v.resize(rows*cols);
			for(int i=0; i<rows*cols; ++i) v[i] =0;
			}
		}
		
	T& operator() (int index) {return v[index];}
	const T& operator() (int index) const {return v[index];}
	T& operator() (int r, int c) { 
		if ( r>= rows || c>= cols ) fprintf(stderr, "Accessing (%d,%d) is out of bounds (%d x %d)", r, c, rows, cols);
		assert(r<rows && c<cols);
		return v[r*cols+c]; 
		}
	const T& operator() (int r, int c) const { 
		if ( r>= rows || c>= cols ) fprintf(stderr, "Accessing (%d,%d) is out of bounds (%d x %d)", r, c, rows, cols);
		assert(r<rows && c<cols);
		return v[r*cols+c]; 
		}

	T& e(int index) {return v[index];}
	const T& e_const(int index) const {return v[index];}
	T& e(int r, int c) { return v[r*cols+c]; }
	const T& e_const(int r, int c) const { return v[r*cols+c]; }

	int r() const {return rows;}
	int c() const {return cols;}
	
	template <typename U> friend mat_spec<U> sub(const mat_spec<U> & m, int r0, int c0, int nr, int nc) ;
	template <typename U> friend mat_spec<U> rbind(const mat_spec<U> & m1, const mat_spec<U> & m2);
	template <typename U> friend mat_spec<U> cbind(const mat_spec<U> & m1, const mat_spec<U> & m2);
	template <typename U> friend mat_spec<U> rbind(const mat_spec<U> & m1, T& constant=0);
	template <typename U> friend mat_spec<U> cbind(const mat_spec<U> & m1, T& constant=0);
	template <typename U> friend mat_spec<U> rdel(const mat_spec<U> & m1, int r0);
	template <typename U> friend mat_spec<U> cdel(const mat_spec<U> & m1, int c0);
	
	template <typename U> friend mat_spec<U> transpose(const mat_spec<U>& m1);
	template <typename U> friend mat_spec<U> print(const mat_spec<U>& m1, const char* name);
	
	mat_spec<T> r(int r_s, int r_e)  const { return sub(*this, r_s, 0, r_e-r_s+1, cols); }
	mat_spec<T> r(int r_s) const { return sub(*this, r_s, 0, 1, cols); }
	mat_spec<T> c(int c_s, int c_e) const { return sub(*this, 0, c_s, rows, c_e-c_s+1); }
	mat_spec<T> c(int c_s) const  { return sub(*this, 0, c_s, rows, 1); }

	void apply(T(*func)(T)) { for (register int i=0; i<rows*cols; ++i) v[i] = func(v[i]); }
	
	vector<T> cvec(int c_s) const {
		vector<T> retval(rows);
		for(int i=0; i<rows; ++i) retval[i] = e_const(i, c_s);
		return retval;
		}
	vector<T> rvec(int r_s) const {
		vector<T> retval(cols);
		for(int j=0; j<cols; ++j) retval[j] = e_const(r_s, j);
		return retval;
		}
	};



template <typename T> mat_spec<T> transpose(const mat_spec<T>& m1)
	{
	mat_spec<T> m2(m1.cols, m1.rows);
	
	for(register int i=0; i<m1.rows; ++i) 
		for(int j=0; j<m1.cols; ++j)
			m2(j,i) = m1(i,j);
			
	return m2;
	}



template <typename T> mat_spec<T> sub(const mat_spec<T>& m, int r0, int c0, int nr, int nc)
	{
	mat_spec<T> r(nr, nc);
	for(register int i=0; i<nr; ++i) 
		for(register int j=0; j<nc; ++j) 
			r(i,j) = m(r0+i,c0+j);
	return r;
	}


template <typename T> mat_spec<T> rbind(const mat_spec<T>& m1, const mat_spec<T>& m2)
	{
	assert(m1.cols == m2.cols);
	int new_cols = m1.cols;
	int new_rows = m1.rows+m2.rows;
	mat_spec<T> r(new_rows, new_cols);
	for(register int i=0; i<new_rows; ++i) for(register int j=0; j<new_cols; ++j)
		{
		r(i,j) = ( (i<m1.rows) ? m1(i,j) : m2(i-m1.rows,j));
		}
	return r;
	}

template <typename T> mat_spec<T> cbind(const mat_spec<T>& m1, const mat_spec<T>& m2)
	{
	assert(m1.rows == m2.rows);
	int new_rows = m1.rows;
	int new_cols = m1.cols+m2.cols;
	mat_spec<T> r(new_rows, new_cols);
	for(register int i=0; i<new_rows; ++i) for(register int j=0; j<new_cols; ++j)
			r(i,j) = ( (j<m1.cols) ? m1(i,j) : m2(i,j-m2.cols));
	return r;
	}


template <typename T> mat_spec<T> rbind(const mat_spec<T>& m1, T& constant)
	{
	mat_spec<T> r(m1.rows+1, m1.cols);
	for(register int i=0; i<=m1.rows; ++i)
		for(register int j=0; j<m1.cols; ++j)
			{
			r(i,j) = ( (i == m1.rows) ? constant : m1(i,j) );
			}
	return r;
	}

	
template <typename T> mat_spec<T> cbind(const mat_spec<T>& m1, T& constant)
	{
	mat_spec<T> r(m1.rows, m1.cols+1);
	for(register int i=0; i<m1.rows; ++i)
		for(register int j=0; j<=m1.cols; ++j)
			{
			r(i,j) = ( (j == m1.cols) ? constant : m1(i,j) );
			}
	return r;
	}


template <typename T> mat_spec<T> rdel(const mat_spec<T>& m1, int r0)
	{
	mat_spec<T> z(m1.rows-1, m1.cols);
	for(register int i=0; i<m1.rows; ++i) {
		if (i == r0) continue;
		for(register int j=0; j<m1.cols; ++j) {
			z(i,j) = m1( ((i<r0) ? i : (i-1)) , j );
			}
		}
	return z;
	}

template <typename T> mat_spec<T> cdel(const mat_spec<T>& m1, int c0)
	{
	mat_spec<T> z(m1.rows, m1.cols-1);
	for(register int i=0; i<m1.rows; ++i) {
		for(register int j=0; j<m1.cols; ++j) {
			if (j == c0) continue;
			z(i,j) = m1( i, ((j<c0) ? j : j-1) );
			}
		}
	return z;
	}





/* ********************************************************************************** */

typedef double  mat_prec;

class mat: public mat_spec<double> {
	public:
	mat(int pr=0, int pc=0): mat_spec(pr, pc) { }
		
	virtual ~mat() { }

	mat& operator += (const mat& m2);
	mat& operator -= (const mat& m2);
	mat& operator *= (mat_prec f);
	mat& operator /= (mat_prec f);
	
	friend mat operator - (const mat& m1);
	friend mat operator + (const mat& m1, const mat& m2);
	friend mat operator - (const mat& m1, const mat& m2);
	friend mat operator * (const mat& m1, mat_prec f);
	friend mat operator / (const mat& m1, mat_prec f);
	
	friend mat operator * (const mat& m1, const mat& m2);

	virtual void dump(const char* name="") const;
	
	mat& row_op_add(int row_op1, int row_op2);
	mat& row_op_add(int row_op1, int row_op2, mat_prec f);
	mat& row_op_mul(int row_op1, mat_prec f);
	mat& row_op_swp(int row_op1, int row_op2);

	friend mat invert(const mat& src);
	friend mat invert_gsl(const mat& src);
	static mat identity(int dim);
	};



class mvec: public mat
	{
	public:
	mvec(int pr=0) : mat(pr, 1) {}
	mvec(const mat& m);
	mvec(const vector<mat_prec>& pv, int dim=0) : mat(dim ? dim : pv.size(), 1) { v = pv; }
	virtual ~mvec() {}
	
	mat_prec& operator ()(int i) {return mat::operator()(i,0);}
	const mat_prec& operator ()(int i) const {return mat::operator()(i,0);}
	
	mat_prec euclidean2() const ;
	mat_prec euclidean() const ;
	mat_prec mean() const ;
	mat_prec var() const ;
	mat_prec ssq() const ;
	
	friend mat_prec dot(const mvec& v1, const mvec& v2);
	};


typedef class mat_spec<int> imat ;

#include <stdio.h>

template <class T> void print(const mat_spec<T>& m, const char* name) {
	if(name[0]) printf("%s = (%d x %d)\n", name, m.rows, m.cols);
	for(register int i=0; i<m.rows; ++i) {
		printf("[ ");
		for(int j=0; j<m.cols; ++j) printf("%6d ", m(i,j));
		printf("]\n");
		}
	}


#endif
