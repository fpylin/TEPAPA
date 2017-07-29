/* align.h
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

#ifndef __align_h
#define __align_h 1

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <vector>
#include <string>

#define max3(a,b,c)   (max((a),max((b),(c))))
#define max4(a,b,c,d) (max(max((a),(b)),max((c),(d))))


using namespace std;


/******************************************************/
template <class T> vector<T>  to_vector(const T* s, const T eos=0) {
	vector<T>  v;
	for(int i=0; s[i] != eos; ++i) v.push_back(s[i]);
	return v;
	}

/******************************************************/
class nw_mat {
	int rows, cols;
	vector<int> scores;
	
	public:
	nw_mat() { rows=0; cols=0; }
	nw_mat(int pr, int pc) {
		rows=pr; 
		cols=pc; 
		scores.resize(rows*cols);
		}
	
	void set(int v) {
		for(unsigned int i=0; i<scores.size(); ++i) scores[i]=v;
		}
	
	int& s(int i, int j) { return scores[i*cols+j]; }
	const int s(int i, int j) const { return scores[i*cols+j]; }
	
	int fill(const char* s1, const char* s2, int i, int j);
	
	inline void submat_max_at(int& p_i, int& p_j) const;
	};


/******************************************************/
inline void nw_mat::submat_max_at(int& p_i, int& p_j) const
	{
	int max_s=INT_MIN, max_at_i=0, max_at_j=0;

	for(int i=0; i<=p_i; ++i) {
		for(int j=0; j<=p_j; ++j) {
		
			int v = s(i,j);
			if (v > max_s) {
				max_s = v;
				max_at_i = i;
				max_at_j = j;
				}
			}
		}
	p_i = max_at_i;
	p_j = max_at_j;
	}



/******************************************************/
/******************************************************/
/******************************************************/
template <class T> 
	struct alignment: public vector<pair<int, int> >
	{
	string to_str(const vector<T>& s1, const vector<T>& s2, 
		const string& gap_str    = "-", 
		const string& bound_str  = "|",
		bool  f_horiz            = true,
		const string& delim_char = "",
		const string& delim_str  = "\n"
		) const;
		
	void print(const vector<T>& s1, const vector<T>& s2, 
		const string& gap_str    = "-", 
		const string& bound_str  = "|",
		bool  f_horiz            = true,
		const string& delim_char = "",
		const string& delim_str  = "\n") const
		{
		printf("%s", to_str(s1,s2,gap_str,bound_str,f_horiz,delim_char,delim_str).c_str());
		}
	};

inline string to_str(const char ch) 
	{
	char buf[2] = {ch, 0};
	return string(buf);
	}

/******************************************************/
template <class T> 
	string alignment<T>::to_str(const vector<T>& s1, const vector<T>& s2, 
	const string& gap_str,
	const string& bound_str,
	bool  f_horiz,
	const string& delim_char,
	const string& delim_str
	) const
	{
	int i=0, j=0;
	vector<pair<int, int> >::const_iterator k=begin();
	
	vector<string>  s1a, s2a;
	
	do	{
		if (k != end()) {
			while(i<k->first) {
				s1a.push_back( ::to_str(s1[i]) ); 
				s2a.push_back( gap_str );   
				++i;
				}
				
			while(j<k->second) {
				s1a.push_back( gap_str );  
				s2a.push_back( ::to_str(s2[j]) );  
				++j;
				}
			
			if (k == begin()) {
				s1a.push_back( bound_str );
				s2a.push_back( bound_str );
				}
				
			s1a.push_back( ::to_str(s1[i]) ); ++i;
			s2a.push_back( ::to_str(s2[j]) ); ++j;
			++k;
			}
		else
			{
			s1a.push_back( bound_str );
			s2a.push_back( bound_str );
			while(s1[i])  {
				s1a.push_back( ::to_str(s1[i]) ); 
				s2a.push_back( gap_str );
				++i;
				}
				
			while(s2[j])  {
				s1a.push_back( gap_str );
				s2a.push_back( ::to_str(s2[j]) );
				++j;
				}
			}
		}
	while(s1[i] || s2[j]);
	
	string  s;
	if (f_horiz) {
		for(int x=0; x<s1a.size(); ++x) {
			if (x) s += delim_char;
			s += s1a[x]; 
			}
		s += delim_str;
		
		for(int x=0; x<s2a.size(); ++x) {
			if (x) s += delim_char;
			s += s2a[x]; 
			}
		s += delim_str;
		}
	else
		{
		for(int x=0; x<s1a.size(); ++x) {
			s += s1a[x];
			s += delim_str;
			s += s2a[x];
			s += delim_char;
			}
		}
	return s;
	}



/******************************************************/
/******************************************************/
/******************************************************/

template <class T> class needleman_wunsch
	{
	protected:
	int s_match, s_mismatch, s_gap;
	
	void create_alignment(alignment<T>& a,
		const nw_mat& m, 
		int i, int j) const;
	
	virtual int fill_matrix(nw_mat& m, 
		const vector<T>& s1, const vector<T>& s2, 
		int i, int j) const;
	
	public:
	needleman_wunsch(
		int p_s_match=1, 
		int p_s_mismatch=0,
		int p_s_gap=0
		) {
		s_match = p_s_match;
		s_mismatch = p_s_mismatch;
		s_gap = p_s_gap;
		}
		
	alignment<T> operator()(const vector<T>& s1, const vector<T>& s2) const;
	};

template <class T> bool mnw_eq(const T& s1, const T& s2) { return s1 == s2; }


/**************************************/
template <class T> 
	int needleman_wunsch<T>::fill_matrix(nw_mat& m, 
	const vector<T>& s1, const vector<T>& s2, int i, int j) const
	{
// Mi-1, j-1 + Si,j (match/mismatch in the diagonal),
// Mi,j-1 + w (gap in sequence #1),
// Mi-1,j + w (gap in sequence #2)]
	if ( !i || !j) return (m.s(i,j) = 0);
	
	if ( m.s(i, j) != INT_MIN ) return m.s(i, j);
	
	int diag  = fill_matrix(m, s1, s2, i-1, j-1) ;
	int match = (nw_eq(s1[i-1], s2[j-1]) ? s_match : s_mismatch);
	int here  = diag + match;
	int hgap  = fill_matrix(m, s1, s2, i-1, j) + s_gap;
	int vgap  = fill_matrix(m, s1, s2, i, j-1) + s_gap;
	
// 	printf("(%d,%d)\t%d [%d,%d,%d]\n", i, j, match, here, hgap, vgap);
	
	return ( m.s(i, j) = max3(here,hgap,vgap) );
	}

/***************************************************************************************/
template <class T> alignment<T>
	needleman_wunsch<T>::operator()(const vector<T>& s1, const vector<T>& s2) const
	{
	int n1 = s1.size() + 1;
	int n2 = s2.size() + 1;
	
	nw_mat  m( n1, n2 );
	
	m.set(INT_MIN);

	fill_matrix(m, s1, s2, n1-1, n2-1);
	
// 	int i=n1-1, j=n2-1;
	
	alignment<T>  r_retval;

	create_alignment(r_retval, m, n1-1, n2-1);
	
	return r_retval;
	}

/******************************************************/
template <class T> void 
	needleman_wunsch<T>::create_alignment(alignment<T>& a, const nw_mat& m, int i, int j) const
	{
	if (i>0 && j>0) {
		m.submat_max_at(i, j);

		create_alignment(a, m, i-1, j-1);

		if (i && j) a.push_back( pair<int,int>(i-1, j-1) );
		}
	}


/******************************************************/
template <class T> 
	class smith_waterman: public needleman_wunsch<T>
	{
	virtual int fill_matrix(nw_mat& m, 
		const char* s1, const char* s2, 
		int i, int j) const;
		
	public:
	smith_waterman(
		int p_s_match=1, 
		int p_s_mismatch=0,
		int p_s_gap=0
		) : needleman_wunsch<T>(p_s_match, p_s_mismatch, p_s_gap) { }
	};


/**************************************/
template <class T> 
	int smith_waterman<T>::fill_matrix(nw_mat& m, 
	const char* s1, const char* s2, int i, int j) const
	{
// Mi-1, j-1 + Si,j (match/mismatch in the diagonal),
// Mi,j-1 + w (gap in sequence #1),
// Mi-1,j + w (gap in sequence #2)]
// 0 ; no negative values allowed
	if ( !i || !j) return (m.s(i,j) = 0);
	
	if ( m.s(i, j) != INT_MIN ) return m.s(i, j);
	
	int diag  = fill_matrix(m, s1, s2, i-1, j-1) ;
	int match = (nw_eq(s1[i-1], s2[j-1]) ? 
		needleman_wunsch<T>::s_match : 
		needleman_wunsch<T>::s_mismatch);
	int here  = diag + match;
	int hgap  = fill_matrix(m, s1, s2, i-1, j) + needleman_wunsch<T>::s_gap;
	int vgap  = fill_matrix(m, s1, s2, i, j-1) + needleman_wunsch<T>::s_gap;
	
	return ( m.s(i, j) = max4(here,hgap,vgap,0) );
	}




#endif // #ifndef __align_h
