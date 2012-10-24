/*
 *------------------------------------------------------------------------
 * FeynDiagram
 *    This file is part of the FeynDiagram C++ library for creation
 *    of Feynman diagrams.
 *
 * Version $Revision: 2.5 $,  $Date: 2000/04/18 17:03:05 $
 *
 * by Bill Dimm,  bdimm@hotneuron.com
 *
 * Copyright 1993 Bill Dimm
 *    This code may be freely distributed.  No fee beyond a reasonable
 *    copying fee may be charged for this software.
 *    This code may not be included in commercial software without the
 *    author's permission.
 *------------------------------------------------------------------------
 */

#ifndef DEFAULT_H
#  define DEFAULT_H

// $Id: default.h,v 2.5 2000/04/18 17:03:05 billd Exp $

class deflt_double
	{
	private:
		int          flags;
		double       val;
		void         init(deflt_double *parent);
		deflt_double *parent;

	public:
		             deflt_double(deflt_double *parent = NULL)
		                { init(parent); }
		             deflt_double(deflt_double *parent, double value)
		                { init(parent); set(value); }
		void         clear() { flags = 0; }
		void         set(double newval);
		void         scale(double scalefactor);
		double       get();
		void         setparent(deflt_double *parentptr) { parent = parentptr; }
		             operator double() { return get(); }
	};


class deflt_boolean
	{
	private:
		int          flags;
		int          val;
		void         init(deflt_boolean *parent);
		deflt_boolean    *parent;

	public:
		             deflt_boolean(deflt_boolean *parent = NULL)
		                { init(parent); }
		             deflt_boolean(deflt_boolean *parent, int value)
		                { init(parent); if (value) settrue(); else setfalse(); }
		void         clear() { flags = 0; }
		void         settrue();
		void         setfalse();
		int          get();
		void         setparent(deflt_boolean *parentptr) { parent = parentptr; }
		             operator int() { return get(); }
	};


class deflt_str
	{
	private:
		int          flags;
		char         *val;
		void         init(deflt_str *parent);
		deflt_str    *parent;

	public:
		             deflt_str(deflt_str *parent = NULL)
		                { init(parent); }
		             deflt_str(deflt_str *parent, char *value)
		                { init(parent); set(value); }
		void         clear() { flags = 0; }
		void         set(char *newval);
		char         *get();
		void         setparent(deflt_str *parentptr) { parent = parentptr; }
								 operator char*() { return get(); }
	};


#endif  // ifndef DEFAULT_H
