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
#include "stats.h"
#include "strfriends.h"
#include "config.h"
#include <mutex>

vector<double> LOG(const vector<double>& v) {
	vector<double> retval(v.size());
	for(unsigned int i=0; i<v.size(); ++i) retval[i] = log(v[i]);
	return retval;
	}

double mean(const vector<double>& v) {
	double sum = 0.0;
	int n = 0;
	for(unsigned int i=0; i<v.size(); ++i) {
		if (isnanf(v[i])) continue;
		sum += v[i];
		n++;
		}
	return sum / double(n);
	}

double sd(const vector<double>& v) {
	double sum = 0.0;
	double mu = mean(v);
	int n=0;
	for(unsigned int i=0; i<v.size(); ++i) {
		if (isnanf(v[i])) continue;
		double e = v[i] - mu ;
		sum += e * e;
		n++;
		}
	return sqrt (sum / n );
	}

double median(const vector<double>& v) {
	vector<double> u;
	for(unsigned int i=0; i<v.size(); ++i) {
		if (isnanf(v[i])) continue;
		u.push_back(v[i]);
		}
	sort(u.begin(), u.end());
	int z = u.size();
	
	if (z & 1) return u[(z-1)/2];
	
	return 0.5 * ( u[(z-1)/2] + u[(z+1)/2] );
	}


#define SQRT2           1.41421356237309504880
#define HALF_SQRT_PI    0.88622692545275801364 

double pnorm (double x) { return 0.5 * ( 1 + erf( x / SQRT2 ) ); }
long double pnorml (long double x) { return 0.5 * ( 1 + erfl( x / sqrtl(2) ) ); }

double qnorm (double p) { 
	long double ql=-50, qh=50;
// 	long double pl=pnorml(ql), ph=pnorml(qh);
// 	long double pl=pnorml(ql), ph=pnorml(qh);
	
	do	{
		long double qm = (ql+qh) / 2.0;
		long double pm = pnorml(qm);
// 		printf("%g\t%g\t%g\t%g\t%g\t->\t%g\t%g\t|\t%g\n", p, ql, pl, qh, ph, qm, pm, p); 
		if (pm<p) {
			ql = qm;
// 			pl = pm;
			}
		else {
			qh = qm;
// 			ph = pm;
			}
		}
	while( (qh != ql) && ((qh - ql) > 1e-15) );
	
	return double( (ql + qh) / 2.0 );
	}


	
	
	

void two_class_stats::print_help(FILE* file)
	{
	fprintf(file,
	"Format flags: \n\
	'1' = dp (class positive)\n\
	'2' = dn (class negative)\n\
	'3' = zp (test positive)\n\
	'4' = zn (test positive)\n\
	'5' = tc (test correct)\n\
	'6' = tw (test wrong)\n\
	'a' = tp (true positive)\n\
	'b' = fp (false positive)\n\
	'c' = fn (false negative)\n\
	'd' = tn (true negative)\n\
	'e' = se (sensitivity, true positive rate = TPR)\n\
	'f' = sp (specificity, 1 = false positive rate = FPR)\n\
	'g' = pp (positive predictive value, PPV)\n\
	'h' = np (negative predictive value, NPV)\n\
	'F' = F-measure\n\
	'H' = H-measure\n\
	'x' = acc (accuracy)\n\
	'y' = la  (worst auc as estimated by binormal method)\n\
	'z' = ea  (auc as estimated by empirical method)\n\
	'<' = attributes\n\
	'-' = ->\n\
	'>' = class label\n\
	'$' = new line\n");
	}

void two_class_stats::print(char fmt, FILE* file)  const
	{
	switch(fmt)
		{
		case '1': fprintf(file,"dp %05d ",  dp()); break;
		case '2': fprintf(file,"dn %05d ",  dn()); break;
		case '3': fprintf(file,"zp %05d ",  sp()); break;
		case '4': fprintf(file,"zn %05d ",  sn()); break;
		case '5': fprintf(file,"tc %05d ",  sc()); break;
		case '6': fprintf(file,"tw %05d ",  sw()); break;
		
		case '#': fprintf(file,"zpr %.3f ",   spr()); break;
		
		case 'a': fprintf(file,"tp %05d ",  tp); break;
		case 'b': fprintf(file,"fp %05d ",  fp); break;
		case 'c': fprintf(file,"fn %05d ",  fn); break;
		case 'd': fprintf(file,"tn %05d ",  tn); break;
		case 'e': fprintf(file,"se %5.3f ", sens()); break;
		case 'f': fprintf(file,"sp %5.3f ", spec()); break;
		case 'g': fprintf(file,"pp %5.3f ", ppv()); break;
		case 'h': fprintf(file,"np %5.3f ", npv()); break;
		case 'i': fprintf(file,"pr %5.3f ", prev()); break; // to add
		case 'j': fprintf(file,"or %8.2g ", o_r()); break; // to add to help
		case 'J': fprintf(file,"[%8.2g--%8.2g]  ", ci95_l_or(), ci95_h_or()); break; // to add to help
		case 'k': fprintf(file,"rr %8.2f ", rr()); break;  // to add to help
		case 'l': fprintf(file,"lr+ %8.2g ", lrp()); break;  // to add to help
		case 'L': fprintf(file,"[%8.2g--%8.2g]  ", ci95_l_lrp(), ci95_h_lrp()); break; // to add to help
		case 'm': fprintf(file,"lr- %6.2g ", lrn()); break;  // to add to help
		case 'x': fprintf(file,"ac %5.4f ", acc()); break;
		case 'y': fprintf(file,"la %5.3f ", mauc()); break;
		case 'z': fprintf(file,"ea %5.3f ", eauc()); break;
		case 'F': fprintf(file,"Fm %5.3f ", f_measure()); break;
		case 'H': fprintf(file,"Hm %5.3f ", h_measure()); break;
		case '$': fprintf(file,"\n"); break;
		default: fprintf(file,"%c", fmt);
		}
	}

string two_class_stats::format(char fmt)  const
	{
	switch(fmt)
		{
		case '1': return strfmt("dp %05d ",  dp()); break;
		case '2': return strfmt("dn %05d ",  dn()); break;
		case '3': return strfmt("zp %05d ",  sp()); break;
		case '4': return strfmt("zn %05d ",  sn()); break;
		case '5': return strfmt("tc %05d ",  sc()); break;
		case '6': return strfmt("tw %05d ",  sw()); break;
		
		case '#': return strfmt("zpr %.3f ",   spr()); break;
		
		case 'a': return strfmt("tp %05d ",  tp); break;
		case 'b': return strfmt("fp %05d ",  fp); break;
		case 'c': return strfmt("fn %05d ",  fn); break;
		case 'd': return strfmt("tn %05d ",  tn); break;
		case 'e': return strfmt("se %5.3f ", sens()); break;
		case 'f': return strfmt("sp %5.3f ", spec()); break;
		case 'g': return strfmt("pp %5.3f ", ppv()); break;
		case 'h': return strfmt("np %5.3f ", npv()); break;
		case 'i': return strfmt("pr %5.3f ", prev()); break; // to add
		case 'j': return strfmt("or %7.2f ", o_r()); break; // to add to help
		case 'J': return strfmt("[%7.2f--%7.2f]  ", ci95_l_or(), ci95_h_or()); break; // to add to help
		case 'k': return strfmt("rr %7.2f ", rr()); break;  // to add to help
		case 'l': return strfmt("lr+ %7.2f ", lrp()); break;  // to add to help
		case 'L': return strfmt("[%7.2f--%7.2f]  ", ci95_l_lrp(), ci95_h_lrp()); break; // to add to help
		case 'm': return strfmt("lr- %7.2f ", lrn()); break;  // to add to help
		case 'x': return strfmt("ac %5.4f ", acc()); break;
		case 'y': return strfmt("la %5.3f ", mauc()); break;
		case 'z': return strfmt("ea %5.3f ", eauc()); break;
		case 'F': return strfmt("Fm %5.3f ", f_measure()); break;
		case 'H': return strfmt("Hm %5.3f ", h_measure()); break;
		case '$': return strfmt("\n"); break;
		default: return strfmt("%c", fmt);
		}
	}


double two_class_stats::operator()(char code) const
	{
	switch(code)
		{
		case '1': return dp();
		case '2': return dn();
		case '3': return sp();
		case '4': return sn();
		case '5': return sc();
		case '6': return sw();
		case 'a': return tp;
		case 'b': return fp;
		case 'c': return fn;
		case 'd': return tn;
		case 'e': return sens();
		case 'f': return spec();
		case 'g': return ppv();
		case 'h': return npv();
		case 'x': return acc();
		case 'y': return mauc();
		case 'z': return eauc();
		case 'F': return f_measure();
		case 'H': return h_measure();
		default: break;
		};
	return NAN;
	}



void two_class_stats::print(const string& output_format, FILE* file)  const
	{
	for(unsigned int f=0; f<output_format.length(); ++f)
		print(output_format[f],file);
	}


string two_class_stats::format(const string& output_format)  const
	{
	string s;
	for(unsigned int f=0; f<output_format.length(); ++f) s += format(output_format[f]);
	return s;
	}

/** Compute two class stats by comparing logical double in two vectors/arrays
  */
int two_class_stats::compute( two_class_stats& st, const vector<double>& vd , const vector<double>& vp )
	{
	assert( vd.size() == vp.size() );
	for(unsigned int i=0; i<vd.size(); ++i) st.incr( (vd[i]!=0.0), (vp[i]!=0.0) );
	
	return vd.size();
	}

int two_class_stats::compute( two_class_stats& st, const vector<bool>& vd , const vector<double>& vp )
	{
	assert( vd.size() == vp.size() );
	for(unsigned int i=0; i<vd.size(); ++i) st.incr( vd[i], (vp[i]!=0.0) );
	
	return vd.size();
	}

int two_class_stats::compute( two_class_stats& st, const vector<bool>& vd , const vector<bool>& vp )
	{
	assert( vd.size() == vp.size() );
	for(unsigned int i=0; i<vd.size(); ++i) st.incr( vd[i], vp[i] );
	
	return vd.size();
	}

#include <map>

using namespace std;

#include <set>

int n_uniq(const vector<double>& x) {
	set<double> xs;
	for(unsigned int i=0; i<x.size(); ++i) xs.insert(x[i]);
	return xs.size();
	}

vector<double> get_rank(const vector<double>& x) {
	map<double, int>     score_cnt;
	map<double, double>  score_rank;
	vector<double>       retval ;
	
	int n = x.size();
	for(int i=0; i<n; ++i) {
		double z = x[i]; 
		if ( score_cnt.find(z) != score_cnt.end() ) 
			score_cnt[ z ] ++;
		else 
			score_cnt[ z ] = 1;
		}
	
	int cnt = 0;
	for (map<double,int>::const_reverse_iterator j=score_cnt.rbegin(); j != score_cnt.rend(); ++j) {
		double rank = 0.5 + ( double(j->second) / 2.0 ); 
		score_rank[ j->first ] = cnt + rank;
		cnt += j->second;
		}
	
	for(int i=0; i<n; ++i) retval.push_back( score_rank[ x[i] ] );
	
// 	for(int i=0; i<n; ++i) printf ("get_rank\t%d\t%f\t%f\n", i, x[i], retval[i] );
	
	return retval;
	}

// spearman(@x, @y): calculates Spearman's correlation (Rho) of @x and @y

#include <assert.h>
#include <gsl/gsl_cdf.h>

double spearman(const vector<double>& x, const vector<double>& y, double* ppval) {
	assert( x.size() == y.size() );

	if ( n_uniq(x) <= 1) return NAN;
	if ( n_uniq(y) <= 1) return NAN;
	
	int n = x.size();
	if (n<=2) return NAN;
	
	vector<double>  x_rank = get_rank(x);
	vector<double>  y_rank = get_rank(y);
	

	double sumd2 = 0;
	for(int i=0; i<n; ++i) {
		double d = ( x_rank[i] - y_rank[i] );
		sumd2 += d * d;
		}
	double rho = 1.0 - ( 6 * sumd2) / (n * ( n * n - 1.0 ) );
	
	if (ppval) {
		double t = rho * sqrt( (n - 2.0) / ( 1.0 - (rho * rho) + 1e-20 ) ) ;
// 		fprintf(stderr, "%g\t%g\t%d\n", t, rho, n);
		double p = 1 - fabs( 0.5 - gsl_cdf_tdist_P ( t, n-2 ) ) * 2;
		if (p == 0.0) p = gsl_cdf_tdist_Q( fabs(t), n-2 ) * 2;
		*ppval = p;
		}
	
	return rho;
	}



vector<double> hypergeometric_log_i_cache;

void hypergeometric_log_i_cache_init(void) {
	const int init_size = 1048576;
	hypergeometric_log_i_cache.resize(init_size);
	for (register int i=0; i<init_size; ++i) {
		if (i < 2) continue;
		hypergeometric_log_i_cache[i] = log( (double)(i) );
		}
	}

std::mutex  hypergeometric_mutex;


double hypergeometric(int a, int b, int c, int d) {
	double p = 0;
	int n = a+b+c+d;
	
// #if TEPAPA_MULTITHREAD
// 	hypergeometric_mutex.lock();
// #endif // TEPAPA_MULTITHREAD
	register int n0 = hypergeometric_log_i_cache.size();
	if (n0 < n) {
		hypergeometric_log_i_cache.resize(n+1);
		}

	for (register int i=n0; i<=n; ++i) {
		if (i < 2) continue;
		hypergeometric_log_i_cache[i] = log( (double)(i) );
		}
		
	for(register int i=2; i<=n; ++i) {
		double logi = hypergeometric_log_i_cache[i];
// 		double logi = log( (double)(i) );
		if (i>a && i<=a+b) p += logi ;
		if (i>d && i<=c+d) p += logi ;
		if (i>c && i<=a+c) p += logi ;
		if (i>b && i<=b+d) p += logi ;
		p -= logi ;
		}
// #if TEPAPA_MULTITHREAD
// 	hypergeometric_mutex.unlock();
// #endif // TEPAPA_MULTITHREAD
	return exp(p);
	}


double fisher_exact_test(int a, int b, int c, int d) {
	int ab = a+b;
// 	int cd = c+d;
	int ac = a+c;
	int bd = b+d;
	double p = 0;
	double ref = hypergeometric(a,b,c,d);
	for(int i=0; i<=ab; ++i) {
		int A = i;
		int B = ab - A; if (B<0) continue;
		int C = ac - A; if (C<0) continue;
		int D = bd - B; if (D<0) continue;
		double p1 = hypergeometric(A,B,C,D);
		if (p1 <= ref) p += p1;
		}
	return p;
	}

inline double cc(double x) {if (x < 1) return x + 0.5; return x; } 

double odds_ratio(int a, int b, int c, int d) {
	double p = 1;
	bool f_do_cc = ( (uint)(a) & (uint)(b) & (uint)(c) & (uint)(d) );
	if (f_do_cc) {
		p *= double(a);
		p *= double(d);
		p /= double(b);
		p /= double(c);
		}
	else { // Haldane correction
		p *= double(a) + 0.5;
		p *= double(d) + 0.5;
		p /= double(b) + 0.5;
		p /= double(c) + 0.5;
		}
	return p;
	}
