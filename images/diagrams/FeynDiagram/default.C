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
 *    author's permission.  If you modify this code and decide to
 *    distribute it, you MUST make it clear that it is a modified version.
 *------------------------------------------------------------------------
 */

#include <iostream>

#ifdef NOOSFCN
	extern "C" { void exit(int); }
#else
#  include <osfcn.h>
#endif

#include <FeynDiagram/default.h>

static char sccsid_default[] = "$Id: default.C,v 2.5 2000/04/18 17:03:05 billd Exp $";

// values for "flags"
#define F_ISSET 01
#define F_SCALE  02

#define AUTHOR "Bill Dimm"

#define ERRSTR "   ** "

void deflt_double::init(deflt_double *parentptr)
	{
	flags = 0;
	parent = parentptr;
	}


void deflt_double::set(double newval)
	{
	clear(); 
	val = newval; 
	flags |= F_ISSET;
	}


void deflt_double::scale(double scalefactor)
	{
	clear(); 
	val = scalefactor; 
	flags |= F_ISSET | F_SCALE;
	}


double deflt_double::get()
	{
	double result;

	if (flags & F_ISSET)
		{
		if (flags & F_SCALE)
			{
			if (parent == NULL)
				{
				std::cerr << ERRSTR << "Attempt to rescale a variable with no parent\n";
				exit(1);
				}
			result = val * parent->get();  // Oh joy, recursion!
			}
		else
			result = val;
		}
	else
		{
		if (parent == NULL)
			{
			std::cerr << ERRSTR << "Fatal error - measurement has no value and no parent\n";
			std::cerr << ERRSTR << "  Please notify " << AUTHOR << "\n";
			exit(1);
			}
		else
			result = parent->get();  // Oh joy, recursion!
		}
	return result;
	}


void deflt_boolean::init(deflt_boolean *parentptr)
	{
	flags = 0;
	parent = parentptr;
	}


void deflt_boolean::settrue()
	{
	clear(); 
	val = 1;
	flags |= F_ISSET;
	}


void deflt_boolean::setfalse()
	{
	clear(); 
	val = 0;
	flags |= F_ISSET;
	}


int deflt_boolean::get()
	{
	int result;

	if (flags & F_ISSET)
		result = val;
	else
		{
		if (parent == NULL)
			{
			std::cerr << ERRSTR << "Fatal error - measurement has no value and no parent\n";
			std::cerr << ERRSTR << "  Please notify " << AUTHOR << "\n";
			exit(1);
			}
		else
			result = parent->get();  // Oh joy, recursion!
		}
	return result;
	}


void deflt_str::init(deflt_str *parentptr)
	{
	flags = 0;
	parent = parentptr;
	}


void deflt_str::set(char *newval)
	{
	clear();
	val = newval;
	flags |= F_ISSET;
	}


char *deflt_str::get()
	{
	char *result;

	if (flags & F_ISSET)
		result = val;
	else
		{
		if (parent == NULL)
			{
			std::cerr << ERRSTR << "Fatal error - measurement has no value and no parent\n";
			std::cerr << ERRSTR << "  Please notify " << AUTHOR << "\n";
			exit(1);
			}
		else
			result = parent->get();  // Oh joy, recursion!
		}
	return result;
	}

