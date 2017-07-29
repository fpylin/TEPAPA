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

#ifndef  __IPTR_H
#define  __IPTR_H 1

#include <stdio.h>
#include <set>

class __iptr_member
	{
	std::set<class __iptr_base*> spectators;
	unsigned int __ref_cnt;
	
	public:
	__iptr_member() {__ref_cnt=0; }
 	inline virtual ~__iptr_member();
	
	bool __can_delete() const {return __ref_cnt==0;}
	void __inc_ref_cnt() {__ref_cnt++;}
	bool __dec_ref_cnt() {return ((--__ref_cnt) > 0);}
	unsigned int __get_ref_cnt() const {return __ref_cnt;}
	
 	void __add_spectator(class __iptr_base* t){if(t) spectators.insert(t);}
 	void __del_spectator(class __iptr_base* t){if(t) spectators.erase(t);}
	};



class __iptr_base
	{
	friend class __iptr_member;
	protected:
	union
		{
		mutable       __iptr_member* o;
		mutable const __iptr_member* o_const;
		};
	
	virtual void set(const __iptr_member* po)=0;
	virtual void reset()=0;
	
	friend inline void __iptr_set_work_around (__iptr_base* ib, const __iptr_member* po) {ib->set(po);}
	friend inline void __iptr_reset_work_around (__iptr_base* ib) {ib->reset();}

	public:
	const __iptr_member* __get() const  {return o_const;}
	__iptr_base()                       {}
	__iptr_base(const __iptr_member* p) {}
	__iptr_base(const __iptr_base& pb)  {}
	virtual ~__iptr_base()              {}
	
	__iptr_base& operator=(const __iptr_base& u)
		{
		if (this == &u) return *this;
		reset();
		set(u.o);
		return *this;
		}
	
	__iptr_base& operator=(const __iptr_member* u)
		{
		if (o && o == u) return *this;
		reset();
		set(u);
		return *this;
		}

 	operator bool ()    const  {return o;}
 	bool operator !()   const  {return !o;}
	
	bool operator <  (const __iptr_base& ep) {return o < ep.o;}
	bool operator == (const __iptr_base& ep) {return o == ep.o;}
	bool operator != (const __iptr_base& ep) {return o != ep.o;}
	};


 inline __iptr_member::~__iptr_member()
 	{
 	for(std::set<class __iptr_base*>::iterator i=spectators.begin(); i!=spectators.end(); ++i)
 		{
 		if(*i) (*i)->set(0);
 		}
 	}


template <class T> class iptr: public __iptr_base
	{
	protected:	
	virtual void set(const __iptr_member* po)
		{
		if( (o_const = po) ) o->__inc_ref_cnt();
		}
	
	virtual void reset()
		{
		if(o && !o->__dec_ref_cnt())
			{
			delete o;
			o=0;
			}
		}
	
	public:
	iptr():__iptr_base() {set(0);}
	iptr(const __iptr_member* p): __iptr_base(p) {set(p);}
	iptr(const iptr& pb): __iptr_base(pb) {set(pb.__get());}
	iptr(const __iptr_base& pb): __iptr_base(pb) {set(pb.__get());}
	virtual ~iptr(){reset();}
	
 	operator T*()       const  {return dynamic_cast<T*>(o);}
 	operator const T*() const  {return dynamic_cast<const T*>(o_const);}
 	T& operator *()     const  {return dynamic_cast<T&>(*o);}
 	T* operator ->()    const  {return dynamic_cast<T*>(o);}

	__iptr_base& operator=(const __iptr_base& u)
		{return __iptr_base::operator=(u);}
	
	__iptr_base& operator=(const iptr& u)
		{return __iptr_base::operator=(u);}
	
	__iptr_base& operator=(const __iptr_member* u)
		{return __iptr_base::operator=(u);}
	};


template <class T> class iptr_ref: public __iptr_base
	{
	virtual void set(const __iptr_member* po)
		{
		o_const = po;
		if(o) o->__add_spectator(this);
		}
	
	virtual void reset()
		{
		if(o) o->__del_spectator(this);
		o=0;
		}
	
	public:
	iptr_ref():__iptr_base() {set(0);}
	iptr_ref(const __iptr_member* p): __iptr_base(p) {set(p);}
	iptr_ref(const iptr_ref& pb): __iptr_base(pb) {set(pb.__get());}
	iptr_ref(const __iptr_base& pb): __iptr_base(pb) {set(pb.__get());}
	virtual ~iptr_ref(){reset();}
	
 	operator T*()       const  {return dynamic_cast<T*>(o);}
 	operator const T*() const  {return dynamic_cast<const T*>(o_const);}
 	T& operator *()     const  {return dynamic_cast<T&>(*o);}
 	T* operator ->()    const  {return dynamic_cast<T*>(o);}

	__iptr_base& operator=(const __iptr_base& u)
		{return __iptr_base::operator=(u);}
	
	__iptr_base& operator=(const iptr_ref& u)
		{return __iptr_base::operator=(u);}
	
	__iptr_base& operator=(const __iptr_member* u)
		{return __iptr_base::operator=(u);}
		
	__iptr_base& operator=(const iptr<T>& u)
		{return __iptr_base::operator=(u);}
	};



#endif
