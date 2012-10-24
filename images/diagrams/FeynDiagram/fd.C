/*
 *------------------------------------------------------------------------
 * FeynDiagram
 *    This file is part of the FeynDiagram C++ library for creation
 *    of Feynman diagrams.
 *
 * Version $Revision: 2.6 $,  $Date: 2003/08/13 06:23:38 $
 *
 * by Bill Dimm,  bdimm@hotneuron.com
 *
 * Copyright (c) 1993, 2003 Bill Dimm
 *    This code may be freely distributed.  No fee beyond a reasonable
 *    copying fee may be charged for this software.
 *    This code may not be included in commercial software without the
 *    author's permission.  If you modify this code and decide to
 *    distribute it, you MUST make it clear that it is a modified version.
 *------------------------------------------------------------------------
 */

#include <iostream.h>
#include <fstream.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#ifdef NOOSFCN
	extern "C" { void exit(int); }
#else
#  include <osfcn.h>
#endif

#include <FeynDiagram/fd.h>
#include <FeynDiagram/text2PS.h>

static char sccsid_FeynDiagram[] = "$Id: fd.C,v 2.6 2003/08/13 06:23:38 billd Exp $";

#ifdef DEBUG
#  define PSCOMMENTS
#endif

/* #define NOSAVERESTORE in order to have it not bracket each page with
 * a "save" and "restore" (dvips manual says you shouldn't, but it seems
 * to work fine for me).
 */

#define TESTVERSIONCOMMENT "Test version - not official release."

#ifndef SQR
#  define SQR(_x) ((_x)*(_x))
#endif

#define ISTINY(_x) ( fabs(_x) < 1.e-10 )

#define XYOUT(_xypair) (fd->coord2pt(_xypair)).x() \
	<< " " << (fd->coord2pt(_xypair)).y() << " "
#define CURVETO_PTS(_out) (_out) << XYOUT(pts[1]) << XYOUT(pts[2]) << XYOUT(pts[3]) << " C2\n"

#ifdef M_PI
#  define PI M_PI
#else
#  define PI 3.14159265358979323846
#endif

/* BBOX_NUMPTSONCURVE is the number of points on the curve that it
 * samples to determine the bounding box for the curve
 */
#define BBOX_NUMPTSONCURVE 10

#define ERRSTR "   ** "

#define AUTHOR "Bill Dimm"

/* If we do an arcthru() and get a radius bigger than BIGRADIUS, warn
 * the user.
 */
#define BIGRADIUS 1.0e6

// Page boundaries for 8.5x11" paper in points
#define PORTRAIT_MAXRIGHT 612
#define PORTRAIT_MAXTOP   792

// Min width or height of grid for it to decide to round edges to be integers
#define GRID_MINDISTTOROUND 3.0

// Amount of border to add to each side as fraction of width or height
#define GRID_HOR_BORDERFRACT 0.03
#define GRID_VERT_BORDERFRACT 0.15

// Number of steps to use when not using one step for each unit of coord space
#define GRID_DEFAULTSTEPS 20

#define MAX_FILENAME_LEN 2048

// Flags to tell which prolog files are needed
#define PROLOG_NEEDS_GRID 1
#define PROLOG_NEEDS_ELLIPSE 2
#define PROLOG_NEEDS_FILL 4
#define PROLOG_NEEDS_ARROW 8
#define PROLOG_NEEDS_VERTEX 16
#define PROLOG_NEEDS_TEXT 32
#define PROLOG_NEEDS_TILDE 64


drawable_list page::pagelist;
int page::currentpagenum = 0;
void (*page::adjustparams)(FeynDiagram &fd, double rescale) = NULL;
char *page::prologdir = PROLOGDIR;

const double FeynDiagram::xmin_inches_default = 1.0;
const double FeynDiagram::xmax_inches_default = 7.5;
const double FeynDiagram::yref_inches_default = 5.5;

const double FeynDiagram::xmin_coord_default = -10.0;
const double FeynDiagram::xmax_coord_default = 10.0;
const double FeynDiagram::yref_coord_default = 0.0;

const double FeynDiagram::pad_bbox_default = 0.006 * (FeynDiagram::xmax_coord_default - FeynDiagram::xmin_coord_default);;

const double line_plain::positionprec = 0.00001;
const double line_plain::derivprec = 0.001;

const double line_doubleplain::positionprec = 0.00001;
const double line_doubleplain::derivprec = 0.001;

const double line_wiggle::positionprec = 0.00001;
const double line_wiggle::derivprec = 0.001;

const double line_spring::positionprec = 0.00001;
const double line_spring::derivprec = 0.001;

const double line_zigzag::positionprec = 0.00001;
const double line_zigzag::derivprec = 0.001;

deflt_boolean FeynDiagram::global_gridon(NULL,0);


xy polar(double r, double theta)
	{
	double xval, yval;
	if (r < 0)
		cerr << ERRSTR << "Warning: polar() called with negative radius\n";
	xval = r * cos(theta * PI/180.);
	yval = r * sin(theta * PI/180.);
	return xy(xval,yval);
	}


xy operator+(xy p)
	{
	return p;
	}


xy operator-(xy p)
	{
	return xy(-p.x(),-p.y());
	}


xy operator+(xy p1, xy p2)
	{
	xy result(p1.x()+p2.x(), p1.y()+p2.y());
	return result;
	}


xy operator-(xy p1, xy p2)
	{
	xy result(p1.x()-p2.x(), p1.y()-p2.y());
	return result;
	}


xy operator*(double s1, xy p2)
	{
	xy result(s1*p2.x(), s1*p2.y());
	return result;
	}


xy operator*(xy p1, double s2)
	{
	xy result(s2*p1.x(), s2*p1.y());
	return result;
	}


xy operator/(xy p1, double s2)
	{
	xy result(p1.x()/s2, p1.y()/s2);
	return result;
	}


double operator*(xy p1, xy p2)
	{
	double result;

	result = p1.x() * p2.x() + p1.y() * p2.y();
	return result;
	}


xy::xy()
	{
	isdefinedflag = 0;
	}


xy::xy(double x, double y)
	{
	xval = x;
	yval = y;
	isdefinedflag = 1;
	}


double xy::x() const
	{
	if (!isdefinedflag)
		{
		cerr << ERRSTR << "Attempt to get x value from undefined xy\n";
		exit(1);
		}
	return xval;
	}


double xy::y() const
	{
	if (!isdefinedflag)
		{
		cerr << ERRSTR << "Attempt to get y value from undefined xy\n";
		exit(1);
		}
	return yval;
	}


/* Return a unit vector which is normal to this one.  We choose
 * the normal vector such that the cross product:
 *      vect x vect.perp() 
 * is out of the screen.
 */
xy xy::perp()
	{
	double len;

	len = sqrt(SQR(x()) + SQR(y()));
	if (len <= 0.0)
		{
		cerr << ERRSTR << "xy::perp() - cannot find vector perpendicular to ";
		cerr << ERRSTR << "null vector\n";
		exit(1);
		}
	return xy(-y()/len,x()/len);
	}


double xy::angle()
	{
	double result;

	if (ISTINY(x()))
		{
		if (y() > 0.0)
			result = PI / 2.0;
		else
			result = -PI / 2.0;
		}
	else
		{
		result = atan(y() / x());
		if (x() < 0.0)
			result += PI;
		}
	return result * 180. / PI;
	}


xy &xy::rotate(double angle)
	{
	double xnew, ynew;
	double cos_a, sin_a;

	angle *= PI / 180.;
	cos_a = cos(angle);
	sin_a = sin(angle);
	xnew = x()*cos_a - y()*sin_a;
	ynew = y()*cos_a + x()*sin_a;
	xval = xnew;
	yval = ynew;
	return *this;
	}


xy xy::operator*=(double s)
	{
	xval *= s;
	yval *= s;
	return *this;
	}


xy xy::operator/=(double s)
	{
	xval /= s;
	yval /= s;
	return *this;
	}


xy xy::operator+=(xy p)
	{
	xval += p.x();
	yval += p.y();
	return *this;
	}


xy xy::operator-=(xy p)
	{
	xval -= p.x();
	yval -= p.y();
	return *this;
	}


boundingbox::boundingbox(const xy &ll, const xy &ur)
	{
	isdefinedflag = 1;
	lowerleft = ll;
	upperright = ur;
	}


void boundingbox::include(boundingbox box)
	{
	if (!box.isdefined())
		return;
	if (!isdefined())
		{
		upperright = box.upperright;
		lowerleft = box.lowerleft;
		isdefinedflag = 1;
		}
	else
		{
		double xmin, xmax, ymin, ymax;

		xmax = (upperright.x() > box.upperright.x()) ? upperright.x() 
		   : box.upperright.x();
		ymax = (upperright.y() > box.upperright.y()) ? upperright.y() 
		   : box.upperright.y();
		xmin = (lowerleft.x() < box.lowerleft.x()) ? lowerleft.x() 
		   : box.lowerleft.x();
		ymin = (lowerleft.y() < box.lowerleft.y()) ? lowerleft.y() 
		   : box.lowerleft.y();
		upperright = xy(xmax,ymax);
		lowerleft = xy(xmin,ymin);
		}
	}


void boundingbox::expand(double top, double right, double bottom, double left)
	{
	lowerleft -= xy(left, bottom);
	upperright += xy(right, top);
	}


drawable::drawable(int priorityval,drawable_list *dlist)
	{
	drawlist = dlist;
	nextptr = prevptr = NULL;
	priority = priorityval;
	ishiddenflag = 0;
	if (dlist != NULL)
		dlist->add(this);
	}


drawable::~drawable()
	{
	drawlist->remove(this);
	}


void drawable::draw(ostream &out)
	{
	if (!ishidden())
		dodraw(out);
#ifdef DEBUG2
	cerr << "   Output with priority " << priority << "\n";
#endif
	}


void drawable_list::remove(drawable *ptr)
	{
	if (headptr == ptr)
		headptr = headptr->nextptr;
	if (ptr->prevptr != NULL)
		ptr->prevptr->nextptr = ptr->nextptr;
	if (ptr->nextptr != NULL)
		ptr->nextptr->prevptr = ptr->prevptr;
	}


void drawable::dodraw(ostream &out)
	{
	cerr << ERRSTR << "Missing virtual function drawable::dodraw\n";
	cerr << ERRSTR << "  contact " << AUTHOR << "\n";
	exit(1);
	}


boundingbox drawable::bbox()
	{
	if (ishidden())
		return boundingbox();
	else
		return findbbox();
	}


boundingbox drawable::findbbox()
	{
	cerr << ERRSTR << "Missing virtual function drawable::findbbox\n";
	cerr << ERRSTR << "  contact " << AUTHOR << "\n";
	exit(1);
	return boundingbox();
	}


int drawable::find_prolog_needs()  // virtual
	{
	return 0;  // default - flags that say we need nothing
	}


int drawable::prolog_needs()  
	{
	if (ishidden())
		return 0;
	else
		return find_prolog_needs();
	}


void drawable_list::add(drawable *ptr)
	{
	drawable *curr;

	ptr->drawlist = this;
	if (headptr == NULL)
		headptr = ptr;
	else if (headptr->priority < ptr->priority)
		{
#ifdef DEBUG
cerr << "Adding with priority = " << ptr->priority << " head = " << (int)headptr;
#endif
		ptr->nextptr = headptr;
		headptr->prevptr = ptr;
		headptr = ptr;
#ifdef DEBUG
cerr << ERRSTR << "  new head = " << (int)headptr << "\n";
#endif
		}
	else
		{
		curr = headptr;
		while (curr->nextptr != NULL && ptr->priority <= curr->nextptr->priority)
			curr = curr->nextptr;
		ptr->nextptr = curr->nextptr;
		if (curr->nextptr != NULL)
			curr->nextptr->prevptr = ptr;
		curr->nextptr = ptr;
		ptr->prevptr = curr;
		}
	}


FDpart::FDpart(FeynDiagram &feyndiag,int priorityval,drawable_list *dlist)
   : drawable(priorityval,dlist)
	{
	fd = &feyndiag;
	if (dlist == NULL)
		feyndiag.drawlist.add(this);
	}


char *vertex_raw::procedurestr()
	{
	cerr << ERRSTR << "Missing virtual function vertex_raw::procedurestr\n";
	cerr << ERRSTR << "  contact " << AUTHOR << "\n";
	exit(1);
	return NULL;
	}


void vertex_raw::dodraw(ostream &out)
	{
	out << fd->lencoord2pt(thickness.get()) << " SLW\n";
	out << procedurestr() << " " << fd->xcoord2pt(loc.x()) << " " << fd->ycoord2pt(loc.y());
	out << " " << fd->lencoord2pt(radius.get()) << " ";
	out << orient_angle;
	out << " vtx_create\n";
	}


boundingbox vertex_raw::findbbox()
	{
	return boundingbox(loc - (fabs(radius)+thickness.get()/2)*xy(1,1), loc + (fabs(radius)+thickness.get()/2)*xy(1,1));
	}


vertex_raw::vertex_raw(FeynDiagram &fd,const xy &location,double angle)
   : FDpart(fd,2) , loc(location), thickness(&fd.vertex_thickness)
	{
	orient_angle = angle;
	}



int vertex_raw::find_prolog_needs()
	{
	return PROLOG_NEEDS_VERTEX;
	}


char *vertex::procedurestr()
	{
	return NULL;
	}


vertex::vertex(FeynDiagram &fd, const xy &location)
   : vertex_raw(fd,location,0.0)
	{
	hide();
	}


vertex::vertex(FeynDiagram &fd,double x, double y)
   : vertex_raw(fd,xy(x,y),0.0)
	{
	hide();
	}


char *vertex_dot::procedurestr()
	{
	return "{vtx_dot}";
	}


vertex_dot::vertex_dot(FeynDiagram &fd, const xy &location)
   : vertex_raw(fd,location,0.0)
	{
	radius.setparent(&fd.vertex_radius_small);
	}


vertex_dot::vertex_dot(FeynDiagram &fd, double x, double y)
   : vertex_raw(fd,xy(x,y),0.0)
	{
	radius.setparent(&fd.vertex_radius_small);
	}


char *vertex_cross::procedurestr()
	{
	return "{vtx_cross}";
	}


vertex_cross::vertex_cross(FeynDiagram &fd, const xy &location, double angle)
   : vertex_raw(fd,location,angle)
	{
	radius.setparent(&fd.vertex_radius_large);
	}


vertex_cross::vertex_cross(FeynDiagram &fd, double x, double y, double angle)
   : vertex_raw(fd,xy(x,y),angle)
	{
	radius.setparent(&fd.vertex_radius_large);
	}


char *vertex_circlecross::procedurestr()
	{
	return "{vtx_circlecross}";
	}


vertex_circlecross::vertex_circlecross(FeynDiagram &fd, const xy &location, double angle)
   : vertex_raw(fd,location,angle)
	{
	radius.setparent(&fd.vertex_radius_large);
	}


vertex_circlecross::vertex_circlecross(FeynDiagram &fd, double x, double y, double angle)
   : vertex_raw(fd,xy(x,y),angle)
	{
	radius.setparent(&fd.vertex_radius_large);
	}


char *vertex_box::procedurestr()
	{
	if (fill.get())
		return "{vtx_box}";
	else
		return "{vtx_open_box}";
	}


vertex_box::vertex_box(FeynDiagram &fd, const xy &location, double angle)
   : vertex_raw(fd,location,angle)
	{
	radius.setparent(&fd.vertex_radius_large);
	fill.setparent(&fd.vertex_box_fill);
	}


vertex_box::vertex_box(FeynDiagram &fd, double x, double y, double angle)
   : vertex_raw(fd,xy(x,y),angle)
	{
	radius.setparent(&fd.vertex_radius_large);
	fill.setparent(&fd.vertex_box_fill);
	}


void curve_raw::func(double t, double &x, double &y)
	{
	cerr << ERRSTR << "Missing virtual function curve_raw::func\n";
	cerr << ERRSTR << "  contact " << AUTHOR << "\n";
	exit(1);
	}


void curve_raw::deriv(double t, double &dt, double &dx, double &dy, double prec)
	{
	cerr << ERRSTR << "Missing virtual function curve_raw::deriv\n";
	cerr << ERRSTR << "  contact " << AUTHOR << "\n";
	exit(1);
	}


double curve_raw::dist(double t, double prec, double ref)
	{
	cerr << ERRSTR << "Missing virtual function curve_raw::dist\n";
	cerr << ERRSTR << "  contact " << AUTHOR << "\n";
	exit(1);
	return 0.0;
	}


double curve_raw::tfromdist(double dist, double tmin, double tmax, double prec)
	{
	cerr << ERRSTR << "Missing virtual function curve_raw::tfromdist\n";
	cerr << ERRSTR << "  contact " << AUTHOR << "\n";
	exit(1);
	return 0.0;
	}


xy curve_raw::f(double t)
	{
	double x,y;
	func(t,x,y);
	return xy(x,y);
	}


// Computes unit tangent vector
xy curve_raw::tang(double t, double prec)
	{
	double dt, dx, dy, len;
	dt = 0.001;
	deriv(t,dt,dx,dy,prec);
	len = sqrt(dx*dx + dy*dy);
	if (len <= 0.0)
		{
		cerr << ERRSTR << "curve_raw::tang - tangent vector has zero length so the\n";
		cerr << ERRSTR << "  curve must be badly parameterized.\n";
		exit(1);
		}
	return xy(dx/len,dy/len);
	}


void curve_straightline::init()
	{
	typeval = straightline;
	length = sqrt( SQR( loc2.x() - loc1.x() ) + SQR( loc2.y() - loc1.y() ) );
	}


curve_straightline::curve_straightline(const xy &loc1val, const xy &loc2val)
	 : loc1(loc1val), loc2(loc2val)
	{
	init();
	}


void curve_straightline::func(double t, double &x, double &y)
	{
	x = loc1.x() + t * ( loc2.x() - loc1.x() );
	y = loc1.y() + t * ( loc2.y() - loc1.y() );
	}


void curve_straightline::deriv(double t, double &dt, double &dx, double &dy, double prec)
	{
	dx = dt * ( loc2.x() - loc1.x() );
	dy = dt * ( loc2.y() - loc1.y() );

	if (ISTINY(dx) && ISTINY(dy))
		{
		cerr << ERRSTR << "curve_straighline::deriv - both dx & dy are zero for t=" << t << " - bad curve\n";
		exit(1);
		}
	}


double curve_straightline::dist(double t, double prec, double ref)
	{
	return t * length;
	}
	

double curve_straightline::tfromdist(double dist, double tmin, double tmax, double prec)
	{
	if (ISTINY(dist))
		return 0.0;
	return dist / length;
	}


curve_arc::curve_arc(const xy &centerval, double r, double theta1val, double theta2val)
   : center(centerval)
	{
	typeval = arc;
	radius = r;
	theta1 = theta1val;
	theta2 = theta2val;
	}


void curve_arc::func(double t, double &x, double &y)
	{
	double angle;

	angle = (theta1 + t * (theta2 - theta1)) * PI / 180.0;
	x = center.x() + radius * cos(angle);
	y = center.y() + radius * sin(angle);
	}


void curve_arc::deriv(double t, double &dt, double &dx, double &dy, double prec)
	{
	double angle;

	angle = (theta1 + t * (theta2 - theta1)) * PI / 180.0;
	dx = dt * radius * (-sin(angle));
	dy = dt * radius * cos(angle);
	if ( (theta2 - theta1) < 0.0 )
		{
		dx *= -1.0;
		dy *= -1.0;
		}
	}


double curve_arc::dist(double t, double prec, double ref)
	{
	double result;

	result = t * radius * (theta2 - theta1) * PI / 180.0;
	result = fabs(result);
	return result;
	}


double curve_arc::tfromdist(double dist, double tmin, double tmax, double prec)
	{
	return dist / ( radius * fabs(theta2 - theta1) * PI / 180.0 );
	}


line_raw::line_raw(FeynDiagram &fd, const xy &loc1, const xy &loc2)
   : FDpart(fd,20)
	{
	do_whiteline = 0;
	curve = new curve_straightline(loc1,loc2);
	}


line_raw::~line_raw()
	{
	delete curve;
	}


void line_raw::dodraw(ostream &out)
	{
	if (do_whiteline)
		{
		out << "1 setgray\n";
		drawline(out,1,thickness.get()*4);
		out << "0 setgray\n";
		}
	drawline(out,0,thickness.get());
	}


void line_raw::drawline(ostream &out, int whiteline, double thick)
	{
	cerr << ERRSTR << "Missing virtual function line_raw::drawline\n";
	cerr << ERRSTR << "  contact " << AUTHOR << "\n";
	exit(1);
	}


/* This finds the bounding box for the path the curve goes through.
 * findbbox() must then supplement this result with information about 
 * the thickness of the curve to get the actual bounding box
 */
boundingbox line_raw::pathbbox()
	{
	int n;
	double dt;
	boundingbox result;

	if (curve->type() == curve_raw::straightline)
		{
		result.include(curve->f(0.0));
		result.include(curve->f(1.0));
		}
	else
		{
		dt = 1.0 / (BBOX_NUMPTSONCURVE - 1);
		for (n = BBOX_NUMPTSONCURVE - 1; n >= 0; --n)
			result.include(curve->f(n*dt));
		}
	return result;
	}


   // Finds angle in degrees between 0 and 360
double line_raw::findangle(double dx, double dy)
	{
	double ds;
	double result;

	ds = sqrt(SQR(dx) + SQR(dy));
	result = acos(dx/ds);
	result *= 180 / PI;
	if (dy < 0.0)
		result = 360 - result;

	return result;
	}


void line_raw::arcthru(double x, double y, int ccw)
	{
	arcthru(xy(x,y),ccw);
	}


void line_raw::arcthru(const xy &location, int ccw)
	{
	double xc, yc;
	double x1, y1, x2, y2, x3, y3;
	double denom;
	double radius, thetatmp, t, theta1, theta2;
	int tadpole = 0;

	x3 = location.x();
	y3 = location.y();

	curve->func(0.0,x1,y1);
	curve->func(1.0,x2,y2);

	denom = (x1-x2)*(y1-y3) - (x1-x3)*(y1-y2);
	if (x1 == x2 && y1 == y2 && (x1 != x3 || y1 != y3))
		tadpole = 1;
	else if (ISTINY(denom))
		{
		cerr << ERRSTR << "Illegal arcthru - the point lies on the line connecting the endpoints\n";
		exit(1);
		}

	if (tadpole)
		{
		yc = 1./2. * (y1 + y3);
		xc = 1./2. * (x1 + x3);
		}
	else
		{
		yc = 1/2.0 * ( (x1-x3)*(SQR(x2)-SQR(x1)+SQR(y2)-SQR(y1)) 
			 - (x1-x2)*(SQR(x3)-SQR(x1)+SQR(y3)-SQR(y1)) ) / denom;
		if ( ISTINY(x1-x2) )
			xc = - 1/2.0 * ( SQR(x3) - SQR(x1) + SQR(y3) - SQR(y1) + 2*yc*(y1-y3)) / (x1-x3);
		else
			xc = - 1/2.0 * ( SQR(x2) - SQR(x1) + SQR(y2) - SQR(y1) + 2*yc*(y1-y2)) / (x1-x2);
		}

	radius = sqrt( SQR(x1-xc) + SQR(y1-yc) );
	if (radius > BIGRADIUS)
		{
		cerr << ERRSTR << "warning - radius for arcthru() is huge.\n";
		cerr << ERRSTR << "          Are the three points colinear?\n";
		}

	theta1 = findangle(x1-xc,y1-yc);
	if (tadpole)
		if (ccw)
			theta2 = theta1 + 360;
		else
			theta2 = theta1 - 360;
	else
		{
		theta2 = findangle(x2-xc,y2-yc);
		thetatmp = findangle(x3-xc,y3-yc);
		t = (thetatmp - theta1) / (theta2 - theta1);  // See what t value it gives - should be in [0,1]
		if (t < 0.0 || t > 1.0)
			{  // Must readjust one of the angles to get the arc to go around the right way
			if (theta2 > theta1)
				theta1 += 360;
			else
				theta2 += 360;
			}

		if (fabs(theta1-theta2) > 360)
			{
			cerr << ERRSTR << "Error - difference between starting & ending angles is\n";
			cerr << ERRSTR << "        greater than 360 degress - notify " << AUTHOR << "\n";
			exit(1);
			}
		}

	delete curve;
	curve = new curve_arc(xy(xc,yc),radius,theta1,theta2);
	}


int line_raw::find_prolog_needs()
	{
	if (arrowon.get())
		return PROLOG_NEEDS_ARROW;
	return 0;
	}


void line_raw::ontop(int level)
	{
	if (level < 1 || level > 9)
		{
		cerr << ERRSTR << "Attempt to call ontop() with a level number not\n";
		cerr << ERRSTR << "in the range 1-9\n";
		exit(1);
		}
	do_whiteline = 1;
	drawlist->remove(this);
	priority = 20 - level;
	drawlist->add(this);
	}


line_plain::line_plain(FeynDiagram &fd, const xy &loc1, const xy &loc2)
   : line_raw(fd,loc1,loc2)
	{
	init();
	}


void line_plain::init()
	{
	thickness.setparent(&fd->line_plain_thickness);
	dashon.setparent(&fd->line_plain_dashon);
	spacelen.setparent(&fd->line_plain_spacelen);
	dsratio.setparent(&fd->line_plain_dsratio);
	arrowon.setparent(&fd->line_plain_arrowon);
	arrowposition.setparent(&fd->line_plain_arrowposition);
	arrowlen.setparent(&fd->line_plain_arrowlen);
	arrowangle.setparent(&fd->line_plain_arrowangle);
	}


void line_plain::drawline(ostream &out, int whiteline, double thick)
	{
	double tmpx, tmpy;
	double total_len;
	double arrowt1, arrowt2;
	double lenval1, lenval2;
	curve_arc *carc;

#ifdef PSCOMMENTS
	out << "% Start of line_plain\n";
#endif
	out << "NP\n";
	out << fd->lencoord2pt(thick) << " SLW\n";
	out << "1 SLC\n";
	if (dashon.get())
		{
		if (spacelen.get() <= 0.0)
			{
			cerr << ERRSTR << "'spacelen' must be greater than zero, it is: ";
			cerr << spacelen.get() << '\n';
			exit(1);
			}
		out << "[ " << fd->lencoord2pt(spacelen.get() * dsratio.get());
		out << " " << fd->lencoord2pt(spacelen.get()) << " ] 0 SD\n";
		}

	switch (curve->type())
		{
		case curve_raw::straightline:
			curve->func(0.0,tmpx,tmpy);
			out << fd->xcoord2pt(tmpx) << " " << fd->ycoord2pt(tmpy);
			out << " M2\n";
			curve->func(1.0,tmpx,tmpy);
			out << fd->xcoord2pt(tmpx) << " " << fd->ycoord2pt(tmpy);
			out << " L2\n";
			break;
		case curve_raw::arc:
			carc = (curve_arc *)curve;  // A little naughty...
			out << fd->xcoord2pt(carc->centerx()) << " ";
			out << fd->ycoord2pt(carc->centery()) << " ";
			out << fd->lencoord2pt(carc->rad()) << " ";
			if (carc->angle1() < carc->angle2())
				out << carc->angle1() << " " << carc->angle2() << " arc\n";
			else
				out << carc->angle1() << " " << carc->angle2() << " arcn\n";
			break;
		default:
			cerr << ERRSTR << "Unknown curve type encountered in line_plain::drawline()\n";
			exit(1);
		}
	out << "ST\n";
	if (dashon.get())
		out << "[] 0 SD\n";
	if (arrowon.get())
		{
		if (arrowposition.get() < 0.0 || arrowposition.get() > 1.0)
			{
			cerr << ERRSTR << "line_plain::drawline arrow position must be in range [0,1]\n";
			exit(1);
			}
		else
			{
			total_len = curve->dist(1.0,positionprec,0.0);
			lenval1 = total_len * arrowposition.get() - arrowlen.get() / 2.0;
			lenval2 = total_len * arrowposition.get() + arrowlen.get() / 2.0;
			if (lenval1 < 0.0)
				{  // Shift both ends of the arrow
				lenval2 += -lenval1;
				lenval1 = 0.0;
				}
			if (lenval2 > total_len)
				{
				lenval1 -= lenval2 - total_len;
				lenval2 = total_len;
				}
			arrowt1 = curve->tfromdist(lenval1,0.0,1.0,positionprec);
			arrowt2 = curve->tfromdist(lenval2,arrowt1,1.0,positionprec);
#ifdef PSCOMMENTS
			out << "%  Start of arrow\n";
#endif
			out << "1 SLW\n";
			out << "0 SLC\n";
			curve->func(arrowt1,tmpx,tmpy);
			out << fd->xcoord2pt(tmpx) << " " << fd->ycoord2pt(tmpy) << " ";
			curve->func(arrowt2,tmpx,tmpy);
			out << fd->xcoord2pt(tmpx) << " " << fd->ycoord2pt(tmpy) << " ";
			out << arrowangle.get();
			out << " arrow\nfill\n";
#ifdef PSCOMMENTS
			out << "%  End of arrow\n";
#endif
			}
		}
#ifdef PSCOMMENTS
	out << "% End of line_plain\n\n";
#endif
	}


boundingbox line_plain::findbbox()
	{
	boundingbox result = pathbbox();
	result.expand(thickness.get() / 2);

	 /* Make sure bounding box has space for the arrow.  Do this by
	  * finding the (approximate) location of the arrow, and moving
	  * the appropriate distance perpendicular to the curve at that
	  * point toward either side of the line.  This might be a little
	  * off in some odd cases.
	  */
	if (arrowon.get())
		{
		double sin_angle = sin(arrowangle.get() * PI / 180.0);
		 /* w = max dist from line to edge of the arrow */
		double w = arrowlen.get() * sin_angle * sqrt((1. + sin_angle) / (1. - sin_angle));
		xy pos = curve->f(arrowposition.get());
		xy perp = curve->tang(arrowposition.get()).perp();
		result.include(pos + w * perp);
		result.include(pos - w * perp);
		}

	return result;
	}


line_doubleplain::line_doubleplain(FeynDiagram &fd, const xy &loc1, const xy &loc2)
   : line_raw(fd,loc1,loc2)
	{
	init();
	}


void line_doubleplain::init()
	{
	thickness.setparent(&fd->line_doubleplain_thickness);
	dashon.setparent(&fd->line_doubleplain_dashon);
	spacelen.setparent(&fd->line_doubleplain_spacelen);
	dsratio.setparent(&fd->line_doubleplain_dsratio);
	arrowon.setparent(&fd->line_doubleplain_arrowon);
	arrowposition.setparent(&fd->line_doubleplain_arrowposition);
	arrowlen.setparent(&fd->line_doubleplain_arrowlen);
	arrowangle.setparent(&fd->line_doubleplain_arrowangle);
	lineseparation.setparent(&fd->line_doubleplain_lineseparation);
	}


void line_doubleplain::drawline(ostream &out,int whiteline,double thick)
	{
	double tmpx, tmpy;
	double total_len;
	double arrowt1, arrowt2;
	double lenval1, lenval2;
	curve_arc *carc;
	xy pos;
	int sign;
	double tmprad;

#ifdef PSCOMMENTS
	out << "% Start of line_doubleplain\n";
#endif
	out << "NP\n";
	out << fd->lencoord2pt(thick) << " SLW\n";
	out << "1 SLC\n";
	if (dashon.get())
		{
		if (spacelen.get() <= 0.0)
			{
			cerr << ERRSTR << "'spacelen' must be greater than zero, it is: ";
			cerr << spacelen.get() << '\n';
			exit(1);
			}
		out << "[ " << fd->lencoord2pt(spacelen.get() * dsratio.get());
		out << " " << fd->lencoord2pt(spacelen.get()) << " ] 0 SD\n";
		}

	switch (curve->type())
		{
		case curve_raw::straightline:
			for (sign = 1; sign >= -1; sign -= 2)
				{
				pos = curve->f(0.0);
				pos += (sign * lineseparation.get() / 2.) * curve->tang(0.0).perp();
				out << fd->xcoord2pt(pos.x()) << " " << fd->ycoord2pt(pos.y());
				out << " M2\n";
				pos = curve->f(1.0);
				pos += (sign * lineseparation.get() / 2.) * curve->tang(0.0).perp();
				out << fd->xcoord2pt(pos.x()) << " " << fd->ycoord2pt(pos.y());
				out << " L2\n";
				out << "ST\n";
				}
			break;
		case curve_raw::arc:
			carc = (curve_arc *)curve;  // A little naughty...
			for (sign = 1; sign >= -1; sign -= 2)
				{
				tmprad = carc->rad() + sign * lineseparation.get() / 2.;
				out << fd->xcoord2pt(carc->centerx()) << " ";
				out << fd->ycoord2pt(carc->centery()) << " ";
				out << fd->lencoord2pt(tmprad) << " ";
				if (carc->angle1() < carc->angle2())
					out << carc->angle1() << " " << carc->angle2() << " arc\n";
				else
					out << carc->angle1() << " " << carc->angle2() << " arcn\n";
				out << "ST\n";
				}
			break;
		default:
			cerr << ERRSTR << "Unknown curve type encountered in line_doubleplain::drawline()\n";
			exit(1);
		}
	if (dashon.get())
		out << "[] 0 SD\n";
	if (arrowon.get())
		{
		if (arrowposition.get() < 0.0 || arrowposition.get() > 1.0)
			{
			cerr << ERRSTR << "line_doubleplain::drawline arrow position must be in range [0,1]\n";
			exit(1);
			}
		else
			{
			total_len = curve->dist(1.0,positionprec,0.0);
			lenval1 = total_len * arrowposition.get() - arrowlen.get() / 2.0;
			lenval2 = total_len * arrowposition.get() + arrowlen.get() / 2.0;
			if (lenval1 < 0.0)
				{  // Shift both ends of the arrow
				lenval2 += -lenval1;
				lenval1 = 0.0;
				}
			if (lenval2 > total_len)
				{
				lenval1 -= lenval2 - total_len;
				lenval2 = total_len;
				}
			arrowt1 = curve->tfromdist(lenval1,0.0,1.0,positionprec);
			arrowt2 = curve->tfromdist(lenval2,arrowt1,1.0,positionprec);
#ifdef PSCOMMENTS
			out << "%  Start of arrow\n";
#endif
			out << "1 SLW\n";
			out << "0 SLC\n";
			curve->func(arrowt1,tmpx,tmpy);
			out << fd->xcoord2pt(tmpx) << " " << fd->ycoord2pt(tmpy) << " ";
			curve->func(arrowt2,tmpx,tmpy);
			out << fd->xcoord2pt(tmpx) << " " << fd->ycoord2pt(tmpy) << " ";
			out << arrowangle.get();
			out << " arrow\nfill\n";
#ifdef PSCOMMENTS
			out << "%  End of arrow\n";
#endif
			}
		}
#ifdef PSCOMMENTS
	out << "% End of line_doubleplain\n\n";
#endif
	}


boundingbox line_doubleplain::findbbox()
	{
	boundingbox result = pathbbox();
	result.expand((lineseparation.get() + thickness.get()) / 2);

	 /* Make sure bounding box has space for the arrow.  Do this by
	  * finding the (approximate) location of the arrow, and moving
	  * the appropriate distance perpendicular to the curve at that
	  * point toward either side of the line.  This might be a little
	  * off in some odd cases.
	  */
	if (arrowon.get())
		{
		double sin_angle = sin(arrowangle.get() * PI / 180.0);
		 /* w = max dist from line to edge of the arrow */
		double w = arrowlen.get() * sin_angle * sqrt((1. + sin_angle) / (1. - sin_angle));
		xy pos = curve->f(arrowposition.get());
		xy perp = curve->tang(arrowposition.get()).perp();
		result.include(pos + w * perp);
		result.include(pos - w * perp);
		}

	return result;
	}


line_wiggle::line_wiggle(FeynDiagram &fd, const xy &loc1, const xy &loc2)
   : line_raw(fd,loc1,loc2)
	{
	init();
	}


void line_wiggle::init()
	{
	width.setparent(&fd->line_wiggle_width);
	lwratio.setparent(&fd->line_wiggle_lwratio);
	thickness.setparent(&fd->line_wiggle_thickness);
	dashon.setparent(&fd->line_wiggle_dashon);
	spacelen.setparent(&fd->line_wiggle_spacelen);
	dsratio.setparent(&fd->line_wiggle_dsratio);
	fract.setparent(&fd->line_wiggle_fract);
	arrowon.setparent(&fd->line_wiggle_arrowon);
	arrowposition.setparent(&fd->line_wiggle_arrowposition);
	arrowlen.setparent(&fd->line_wiggle_arrowlen);
	arrowangle.setparent(&fd->line_wiggle_arrowangle);
	}


void line_wiggle::drawline(ostream &out, int whiteline, double thick)
	{
	double total_len;
	double dx, dy, dt=.0001, ds;
	double len_val, width_val, fract_val;
	int cnt, cntmax;
	double x[5], y[5], t[5];
	double tmpx, tmpy;
	int cnt_putarrow;  // which line segment to put the arrow on
	xy middle, delta;  // to compute arrow position
	xy tail, tip;  // Where the arrow goes if there is one

	width_val = width.get();
	len_val = fabs(width_val * lwratio.get());
	fract_val = fract.get();

#ifdef PSCOMMENTS
	out << "% Start of line_wiggle\n";
#endif
	out << "NP\n";
	out << fd->lencoord2pt(thick) << " SLW\n";
	out << "1 SLC\n";
	if (dashon.get())
		{
		if (spacelen.get() <= 0.0)
			{
			cerr << ERRSTR << "'spacelen' must be greater than zero, it is: ";
			cerr << spacelen.get() << '\n';
			exit(1);
			}
		out << "[ " << fd->lencoord2pt(spacelen.get() * dsratio.get());
		out << " " << fd->lencoord2pt(spacelen.get()) << " ] 0 SD\n";
		}

	total_len = curve->dist(1.0,positionprec/100.0,0.0);
	cntmax = (int) (total_len / len_val + 0.5);
	if (cntmax < 2)
		{
		cerr << ERRSTR << "Warning: line_wiggle is very short\n";
		cntmax = 2;
		}
	len_val = total_len / cntmax;  /* re-adjust to get int num of wiggles */
	if (arrowon.get())
		{
		if (arrowposition.get() < 0.0 || arrowposition.get() > 1.0)
			{
			cerr << ERRSTR << "line_wiggle::drawline arrow position must be in range [0,1]\n";
			exit(1);
			}
		cnt_putarrow = (int) (arrowposition.get() * cntmax + 0.5);
		}
	else
		cnt_putarrow = -1;  // This will never match cnt, so won't output arrow
	curve->func(0.0,x[3],y[3]);
	out << fd->xcoord2pt(x[3]) << " " << fd->ycoord2pt(y[3]) << " M2\n";
	for (cnt = 0; cnt < cntmax; ++cnt)
		{
		if (cnt == 0)
			{
			t[1] = curve->tfromdist(len_val*fract_val,0.0,1.0,positionprec);
			curve->deriv(t[1],dt,dx,dy,derivprec);
			ds = sqrt(SQR(dx) + SQR(dy));
			curve->func(t[1],tmpx,tmpy);
			x[1] = tmpx + width_val * dy / ds;
			y[1] = tmpy - width_val * dx / ds;
			}
		else
			{
			x[1] = x[4];
			y[1] = y[4];
			}
		if (cnt == cnt_putarrow)  // Must do this before x[3] & y[3] are updated
			{
			middle = xy(x[3],y[3]);
			delta = xy(x[1]-x[3],y[1]-y[3]);
			}

		t[2] = curve->tfromdist(len_val*(cnt + (1.0-fract_val)),t[1],1.0,positionprec);
		curve->deriv(t[2],dt,dx,dy,derivprec);
		ds = sqrt(SQR(dx) + SQR(dy));
		curve->func(t[2],tmpx,tmpy);
		x[2] = tmpx + width_val * dy / ds;
		y[2] = tmpy - width_val * dx / ds;
		if (cnt == cntmax - 1)
			{
			curve->func(1.0,x[3],y[3]);
			if (cnt_putarrow == cntmax)
				{
				middle = xy(x[3],y[3]);
				delta = xy(x[3]-x[2],y[3]-y[2]);
				}
			}
		else
			{
			t[4] = curve->tfromdist(len_val*(cnt+1+fract_val),t[2],1.0,positionprec);
			curve->deriv(t[4],dt,dx,dy,derivprec);
			ds = sqrt(SQR(dx) + SQR(dy));
			curve->func(t[4],tmpx,tmpy);
			x[4] = tmpx - width_val * dy / ds;
			y[4] = tmpy + width_val * dx / ds;
			x[3] = (x[2] + x[4]) / 2.0;
			y[3] = (y[2] + y[4]) / 2.0;
			}
		out << fd->xcoord2pt(x[1]) << " " << fd->ycoord2pt(y[1]) << " ";
		out << fd->xcoord2pt(x[2]) << " " << fd->ycoord2pt(y[2]) << " ";
		out << fd->xcoord2pt(x[3]) << " " << fd->ycoord2pt(y[3]) << " ";
		out << "C2\n";
		width_val *= -1.0;
		}
	out << "ST\n";
	if (dashon.get())
		out << "[] 0 SD\n";
	if (cnt_putarrow >= 0)  // If we were to output an arrow, do it now
		{
		   // Put the tip slightly forward
		tip = middle + delta / delta.len() * arrowlen.get() * 0.7;
		tail = middle - delta / delta.len() * arrowlen.get() * 0.3;
#ifdef PSCOMMENTS
		out << "%  Start of arrow\n";
#endif
		out << "1 SLW\n";
		out << "0 SLC\n";
		out << fd->xcoord2pt(tail.x()) << " " << fd->ycoord2pt(tail.y()) << " ";
		out << fd->xcoord2pt(tip.x()) << " " << fd->ycoord2pt(tip.y()) << " ";
		out << arrowangle.get();
		out << " arrow\nfill\n";
#ifdef PSCOMMENTS
		out << "%  End of arrow\n";
#endif
		}
#ifdef PSCOMMENTS
	out << "% End of line_wiggle\n\n";
#endif
	}


boundingbox line_wiggle::findbbox()
	{
	 /* no need to worry about arrow - always occurs around center of the wiggle */
	boundingbox result = pathbbox();
	result.expand(fabs(width.get()) + thickness.get() / 2);
	return result;
	}


line_spring::line_spring(FeynDiagram &fd, const xy &loc1, const xy &loc2)
   : line_raw(fd,loc1,loc2)
	{
	init();
	}


void line_spring::init()
	{
	width.setparent(&fd->line_spring_width);
	lwratio.setparent(&fd->line_spring_lwratio);
	thickness.setparent(&fd->line_spring_thickness);
	dashon.setparent(&fd->line_spring_dashon);
	spacelen.setparent(&fd->line_spring_spacelen);
	dsratio.setparent(&fd->line_spring_dsratio);
	arrowon.setparent(&fd->line_spring_arrowon);
	arrowposition.setparent(&fd->line_spring_arrowposition);
	arrowlen.setparent(&fd->line_spring_arrowlen);
	arrowangle.setparent(&fd->line_spring_arrowangle);
	fract1.setparent(&fd->line_spring_fract1);
	fract2.setparent(&fd->line_spring_fract2);
	fract3.setparent(&fd->line_spring_fract3);
	fract4.setparent(&fd->line_spring_fract4);
	threeD.setparent(&fd->line_spring_threeD);
	threeDgap.setparent(&fd->line_spring_threeDgap);
	}


void line_spring::drawline(ostream &out, int whiteline, double thick)
	{
	double total_len;
	double dx, dy, dt=.0001, ds;
	double len_val, width_val, f1, f2, f3, f4;
	double separation, gap;
	int cnt, cntmax;
	double t[5];
	xy pts[4], tang_vect, tmp_pt;
	double tmpx, tmpy;
	int cnt_putarrow;  // which line segment to put the arrow on
	xy middle, delta;  // to compute arrow position
	xy tail, tip;  // Where the arrow goes if there is one
	int do3D;
	double sw;  // Sign of width_val

	width_val = width.get();
	sw = (width_val > 0) ? 1 : -1;
	len_val = fabs(width_val * lwratio.get());
	f1 = fract1.get();
	f2 = fract2.get();
	f3 = fract3.get();
	f4 = fract4.get();
	do3D = threeD.get();
	gap = threeDgap.get();

#ifdef PSCOMMENTS
	out << "% Start of line_spring\n";
#endif
	out << "NP\n";
	out << fd->lencoord2pt(thick) << " SLW\n";
	out << "1 SLC\n";
	if (dashon.get())
		{
		if (spacelen.get() <= 0.0)
			{
			cerr << ERRSTR << "'spacelen' must be greater than zero, it is: ";
			cerr << spacelen.get() << '\n';
			exit(1);
			}
		out << "[ " << fd->lencoord2pt(spacelen.get() * dsratio.get());
		out << " " << fd->lencoord2pt(spacelen.get()) << " ] 0 SD\n";
		}

	total_len = curve->dist(1.0,positionprec/100.0,0.0);
	cntmax = (int) ((total_len - len_val) / (len_val * (1. - f1)) + 0.5) + 1;
	if (cntmax < 2)
		{
		cerr << ERRSTR << "Warning: line_spring is very short\n";
		cntmax = 2;
		}
	   /* re-adjust to get int num of springs */
	len_val = total_len / (1. + (cntmax - 1.) * (1. - f1));  
	separation = len_val * (1. - f1);
	if (arrowon.get())
		{
		if (arrowposition.get() < 0.0 || arrowposition.get() > 1.0)
			{
			cerr << ERRSTR << "line_spring::dodraw arrow position must be in range [0,1]\n";
			exit(1);
			}
		cnt_putarrow = (int) (arrowposition.get() * cntmax);  // Not quite right...
		if (cnt_putarrow >= cntmax)
			cnt_putarrow = cntmax - 1;
		}
	else
		cnt_putarrow = -1;  // This will never match cnt, so won't output arrow
	for (cnt = 0; cnt < cntmax; ++cnt)
		{
		if (cnt != 0)
			{  // previously defined: pts[3], tang_vect, and some t[i]'s
			   // This is the little arc
			pts[1] = pts[3] + f3 * width_val * (-tang_vect.perp() + f4*sw*tang_vect);
			t[1] = curve->tfromdist(len_val + separation*(cnt-1) - len_val*f1/2.
			   , t[2], t[4], positionprec);
			tang_vect = curve->tang(t[1],derivprec);
			pts[3] = curve->f(t[1]) - width_val * tang_vect.perp();
			pts[2] = pts[3] + f3 * len_val * f1 / 2. * tang_vect;
			CURVETO_PTS(out);

			pts[1] = pts[3] - f3 * len_val * f1 / 2. * tang_vect;
			t[2] = curve->tfromdist(len_val + separation*(cnt-1) - len_val*f1
			   , t[2], t[1], positionprec);
			pts[3] = curve->f(t[2]);
			tang_vect = curve->tang(t[2],derivprec);
			pts[2] = pts[3] + f3 * width_val * (-tang_vect.perp() - f4*sw*tang_vect);
			CURVETO_PTS(out);
			}
		else
			{  // preparation
			pts[3] = curve->f(0.0);
			tang_vect = curve->tang(0.0,derivprec);
			out << XYOUT(pts[3]) << "M2\n";
			}

		   // This is the big arc
		tmp_pt = pts[3];  // Save first point in case needed for 3D
		pts[1] = pts[3] + f2 * width_val * (tang_vect.perp() + f4*sw*tang_vect);
		if (cnt == 0)
			t[3] = curve->tfromdist(len_val/2., 0., 1., positionprec);
		else
			t[3] = curve->tfromdist(len_val + separation*(cnt-1) - len_val*f1 
			   + len_val/2., t[2], 1., positionprec);
		tang_vect = curve->tang(t[3],derivprec);
		pts[3] = curve->f(t[3]) + width_val * tang_vect.perp();
		pts[2] = pts[3] - f2 * len_val / 2. * tang_vect;
		if (do3D && cnt != 0 && !whiteline)
			{
			out << "ST\n";
			out << "1 setgray\n";
			out << fd->lencoord2pt(gap) << " SLW\n";
			out << "0 SLC\n";
			out << "[] 0 SD\n";
			out << XYOUT(tmp_pt) << "M2\n";
			CURVETO_PTS(out);
			out << "ST\n";
			out << "0 setgray\n";
			out << fd->lencoord2pt(thick) << " SLW\n";
			out << "1 SLC\n";
			if (dashon.get())
				{
				if (spacelen.get() <= 0.0)
					{
					cerr << ERRSTR << "'spacelen' must be greater than zero, it is: ";
					cerr << spacelen.get() << '\n';
					exit(1);
					}
				out << "[ " << fd->lencoord2pt(spacelen.get() * dsratio.get());
				out << " " << fd->lencoord2pt(spacelen.get()) << " ] 0 SD\n";
				}
			out << XYOUT(tmp_pt) << "M2\n";
			}
		CURVETO_PTS(out);
		if (cnt == cnt_putarrow) // record the arrow position
			{
			middle = pts[3];
			delta = tang_vect;
			}

		pts[1] = pts[3] + f2 * len_val / 2. * tang_vect;
		if (cnt == cntmax - 1)
			t[4] = 1.0;
		else if (cnt == 0)
			t[4] = curve->tfromdist(len_val, t[3], 1., positionprec);
		else
			t[4] = curve->tfromdist(len_val + separation*(cnt-1) - len_val*f1 + len_val
			   , t[3], 1., positionprec);
		tang_vect = curve->tang(t[4],derivprec);
		pts[3] = curve->f(t[4]);
		pts[2] = pts[3] + f2 * width_val * (tang_vect.perp() - f4*sw*tang_vect);
		CURVETO_PTS(out);
		}
	out << "ST\n";
	if (dashon.get())
		out << "[] 0 SD\n";
	if (cnt_putarrow >= 0)  // If we were to output an arrow, do it now
		{
		   // Put the tip slightly forward
		tip = middle + delta * arrowlen.get() * 0.6;
		tail = middle - delta * arrowlen.get() * 0.4;
#ifdef PSCOMMENTS
		out << "%  Start of arrow\n";
#endif
		out << "1 SLW\n";
		out << "0 SLC\n";
		out << fd->xcoord2pt(tail.x()) << " " << fd->ycoord2pt(tail.y()) << " ";
		out << fd->xcoord2pt(tip.x()) << " " << fd->ycoord2pt(tip.y()) << " ";
		out << arrowangle.get();
		out << " arrow\nfill\n";
#ifdef PSCOMMENTS
		out << "%  End of arrow\n";
#endif
		}
#ifdef PSCOMMENTS
	out << "% End of line_spring\n\n";
#endif
	}


boundingbox line_spring::findbbox()
	{
	boundingbox result = pathbbox();
	result.expand(fabs(width.get()) + thickness.get() / 2);

	 /* Make sure bounding box has space for the arrow.  Do this by
	  * finding the (approximate) location of the arrow, and moving
	  * the appropriate distance perpendicular to the curve at that
	  * point toward either side of the line.  This might be a little
	  * off in some odd cases.
	  */
	if (arrowon.get())
		{
		double sin_angle = sin(arrowangle.get() * PI / 180.0);
		 /* w = max dist from line to edge of the arrow */
		double w = arrowlen.get() * sin_angle * sqrt((1. + sin_angle) / (1. - sin_angle));
		xy pos = curve->f(arrowposition.get());
		xy perp = curve->tang(arrowposition.get()).perp();
		result.include(pos + (width.get() + w) * perp);
		result.include(pos + (width.get() - w) * perp);
		}

	return result;
	}



line_zigzag::line_zigzag(FeynDiagram &fd, const xy &loc1, const xy &loc2)
   : line_raw(fd,loc1,loc2)
	{
	init();
	}


void line_zigzag::init()
	{
	width.setparent(&fd->line_zigzag_width);
	lwratio.setparent(&fd->line_zigzag_lwratio);
	thickness.setparent(&fd->line_zigzag_thickness);
	dashon.setparent(&fd->line_zigzag_dashon);
	spacelen.setparent(&fd->line_zigzag_spacelen);
	dsratio.setparent(&fd->line_zigzag_dsratio);
	arrowon.setparent(&fd->line_zigzag_arrowon);
	arrowposition.setparent(&fd->line_zigzag_arrowposition);
	arrowlen.setparent(&fd->line_zigzag_arrowlen);
	arrowangle.setparent(&fd->line_zigzag_arrowangle);
	}


void line_zigzag::drawline(ostream &out, int whiteline, double thick)
	{
	double len_val, width_val;
	double dx, dy, dt=.0001, ds;
	double total_len;
	int cnt, cntmax;
	double t,x,y;
	double tmpx, tmpy, oldx, oldy;
	int cnt_putarrow;  // which line segment to put the arrow on
	xy middle, delta;  // to compute arrow position
	xy tail, tip;  // Where the arrow goes if there is one

	width_val = width.get();
	len_val = fabs(width_val * lwratio.get());

#ifdef PSCOMMENTS
	out << "% Start of line_zigzag\n";
#endif
	out << "NP\n";
	out << fd->lencoord2pt(thick) << " SLW\n";
	out << "1 SLC\n";
	if (dashon.get())
		{
		if (spacelen.get() <= 0.0)
			{
			cerr << ERRSTR << "'spacelen' must be greater than zero, it is: ";
			cerr << spacelen.get() << '\n';
			exit(1);
			}
		out << "[ " << fd->lencoord2pt(spacelen.get() * dsratio.get());
		out << " " << fd->lencoord2pt(spacelen.get()) << " ] 0 SD\n";
		}

	total_len = curve->dist(1.0,positionprec/100.0,0.0);
	cntmax = (int) (total_len / len_val + 0.5);
	if (cntmax < 2)
		{
		cerr << ERRSTR << "Warning: line_zigzag is very short\n";
		cntmax = 2;
		}
	len_val = total_len / cntmax;  /* re-adjust to get int num of wiggles */
	if (arrowon.get())
		{
		if (arrowposition.get() < 0.0 || arrowposition.get() > 1.0)
			{
			cerr << ERRSTR << "line_zigzag::drawline arrow position must be in range [0,1]\n";
			exit(1);
			}
		cnt_putarrow = (int) (arrowposition.get() * cntmax + 0.5);
		}
	else
		cnt_putarrow = -1;  // This will never match cnt, so won't output arrow
	curve->func(0.0,x,y);
	oldx = x;
	oldy = y;
	t = 0.0;
	out << fd->xcoord2pt(x) << " " << fd->ycoord2pt(y) << " M2\n";
	for (cnt = 0; cnt < cntmax; ++cnt)
		{
		t = curve->tfromdist(len_val*(cnt + 0.5),t,1.0,positionprec);
		curve->deriv(t,dt,dx,dy,derivprec);
		ds = sqrt(SQR(dx) + SQR(dy));
		curve->func(t,tmpx,tmpy);
		x = tmpx + width_val * dy / ds;
		y = tmpy - width_val * dx / ds;
		out << fd->xcoord2pt(x) << " " << fd->ycoord2pt(y) << " L2\n";
		if (cnt == cnt_putarrow)
			{
			middle = xy((x+oldx)/2,(y+oldy)/2);
			delta = xy(x-oldx,y-oldy);
			}
		width_val *= -1.0;
		oldx = x;
		oldy = y;
		}
	curve->func(1.0,x,y);
	out << fd->xcoord2pt(x) << " " << fd->ycoord2pt(y) << " L2\n";
	if (cnt == cnt_putarrow)  // does the arrow go on the last segment?
		{
		middle = xy((x+oldx)/2,(y+oldy)/2);
		delta = xy(x-oldx,y-oldy);
		}
	out << "ST\n";
	if (dashon.get())
		out << "[] 0 SD\n";
	if (cnt_putarrow >= 0)  // If we were to output an arrow, do it now
		{
		   // Put the tip slightly forward
		tip = middle + delta / delta.len() * arrowlen.get() * 0.7;
		tail = middle - delta / delta.len() * arrowlen.get() * 0.3;
#ifdef PSCOMMENTS
		out << "%  beggining of arrow\n";
#endif
		out << "1 SLW\n";
		out << "0 SLC\n";
		out << fd->xcoord2pt(tail.x()) << " " << fd->ycoord2pt(tail.y()) << " ";
		out << fd->xcoord2pt(tip.x()) << " " << fd->ycoord2pt(tip.y()) << " ";
		out << arrowangle.get();
		out << " arrow\nfill\n";
#ifdef PSCOMMENTS
		out << "%  End of arrow\n";
#endif
		}
#ifdef PSCOMMENTS
	out << "% End of line_zigzag\n\n";
#endif
	}


boundingbox line_zigzag::findbbox()
	{
	 /* no need to worry about arrow - always occurs around center of the wiggle */
	boundingbox result = pathbbox();
	result.expand(fabs(width.get()) + thickness.get() / 2);
	return result;
	}


FeynDiagram::FeynDiagram(page &pg)
   : drawable(0), scalefactor(&defaultscalefactor)
	{
	init(pg,xmin_inches_default,xmax_inches_default,yref_inches_default);
	}


FeynDiagram::FeynDiagram(page &pg, double xmin_in, double xmax_in, double yref_in)
   : drawable(0), scalefactor(&defaultscalefactor)
	{
	init(pg,xmin_in,xmax_in,yref_in);
	}


void FeynDiagram::prt_begin(ostream &out)
	{
	}


void FeynDiagram::setwithmin(deflt_double &param, double val, double min)
	{
	double tmp;
	if (val < min)
		val = min;
	param.set(val);
	}



void FeynDiagram::init(page &pg, double xmin, double xmax, double yref)
	{
	pg.fdlist.add(this);

	parentpage = &pg;

	xmin_inches = xmin;
	xmax_inches = xmax;
	yref_inches = yref;

	xmin_coord = xmin_coord_default;
	xmax_coord = xmax_coord_default;
	yref_coord = yref_coord_default;

	line_dashon.setfalse();
	line_dsratio.set(0.3/0.2);
	line_arrowon.setfalse();
	line_arrowposition.set(0.5);
	line_arrowangle.set(22.0);
	line_lwratio.set(0.8 / 0.68);

	line_plain_thickness.setparent(&line_thickness);
	line_plain_dashon.setparent(&line_dashon);
	line_plain_spacelen.setparent(&line_spacelen);
	line_plain_dsratio.setparent(&line_dsratio);
	//line_plain_arrowon.setparent(&line_arrowon);
	line_plain_arrowon.settrue();
	line_plain_arrowposition.setparent(&line_arrowposition);
	line_plain_arrowlen.setparent(&line_arrowlen);
	line_plain_arrowangle.setparent(&line_arrowangle);

	line_doubleplain_thickness.setparent(&line_thickness);
	line_doubleplain_dashon.setparent(&line_dashon);
	line_doubleplain_spacelen.setparent(&line_spacelen);
	line_doubleplain_dsratio.setparent(&line_dsratio);
	line_doubleplain_arrowon.setparent(&line_arrowon);
	line_doubleplain_arrowposition.setparent(&line_arrowposition);
	//line_doubleplain_arrowlen.setparent(&line_arrowlen);
	line_doubleplain_arrowangle.setparent(&line_arrowangle);
	line_doubleplain_arrowon.settrue();

	line_wiggle_width.setparent(&line_width);
	line_wiggle_lwratio.setparent(&line_lwratio);
	line_wiggle_thickness.setparent(&line_thickness);
	line_wiggle_dashon.setparent(&line_dashon);
	line_wiggle_spacelen.setparent(&line_spacelen);
	line_wiggle_dsratio.setparent(&line_dsratio);
	line_wiggle_arrowon.setparent(&line_arrowon);
	line_wiggle_arrowposition.setparent(&line_arrowposition);
	line_wiggle_arrowlen.setparent(&line_arrowlen);
	line_wiggle_arrowangle.setparent(&line_arrowangle);
	line_wiggle_fract.set(1/3.0);

	line_spring_width.setparent(&line_width);
	line_spring_lwratio.set(20. / 9.);
	line_spring_thickness.setparent(&line_thickness);
	line_spring_dashon.setparent(&line_dashon);
	line_spring_spacelen.setparent(&line_spacelen);
	line_spring_dsratio.setparent(&line_dsratio);
	line_spring_arrowon.setparent(&line_arrowon);
	line_spring_arrowposition.setparent(&line_arrowposition);
	line_spring_arrowlen.setparent(&line_arrowlen);
	line_spring_arrowangle.setparent(&line_arrowangle);
	line_spring_fract1.set(.32);
	line_spring_fract2.set(.56);
	line_spring_fract3.set(.73);
	line_spring_fract4.set(.1);
	line_spring_threeD.settrue();


	line_zigzag_width.setparent(&line_width);
	line_zigzag_lwratio.setparent(&line_lwratio);
	line_zigzag_thickness.setparent(&line_thickness);
	line_zigzag_dashon.setparent(&line_dashon);
	line_zigzag_spacelen.setparent(&line_spacelen);
	line_zigzag_dsratio.setparent(&line_dsratio);
	line_zigzag_arrowon.setparent(&line_arrowon);
	line_zigzag_arrowposition.setparent(&line_arrowposition);
	line_zigzag_arrowlen.setparent(&line_arrowlen);
	line_zigzag_arrowangle.setparent(&line_arrowangle);

	blob_thickness.setparent(&line_thickness);

	shading_lines_angle.set(45.0);
	shading_dots_angle.set(0.0);

	fontname.set("Times-Roman");

	gridon.setparent(&global_gridon);
	grid_xmin.setparent(&default_grid_xmin);
	grid_xmax.setparent(&default_grid_xmax);
	grid_ymin.setparent(&default_grid_ymin);
	grid_ymax.setparent(&default_grid_ymax);

	updatedefaults();
	}


/* This is needed to change the defaults whenever the user 
 * changes the definition of his coordinate system.  When this is done,
 * we must rescale the defaults so that objects appear the same on the
 * page.  For example, if we have a line_wiggle which connects two points
 * on the page, we don't want the number of wiggles to change just because
 * the user changes the way he labels his coordinate system.
 */
void FeynDiagram::updatedefaults()
	{
	double tmp;
	double rescale;

	tmp = (xmax_coord - xmin_coord) / (xmax_coord_default - xmin_coord_default);
	defaultscalefactor.set(tmp);
	rescale = scalefactor.get();

	pad_bbox_top = pad_bbox_right = pad_bbox_bottom = pad_bbox_left = tmp * pad_bbox_default;

	setwithmin(line_width, rescale * 0.476, pt2lencoord(3.0));
	setwithmin(line_thickness, rescale * 0.07, pt2lencoord(0.5));
	setwithmin(line_spacelen, rescale * 2./3. * 0.4, 2./3.* 6. * line_thickness.get());
	setwithmin(line_arrowlen, rescale * 0.4, 6.6 * line_thickness.get());

	setwithmin(line_doubleplain_lineseparation, rescale * 0.14, line_thickness.get() + pt2lencoord(1.4));
	setwithmin(line_doubleplain_arrowlen, rescale * 0.6
	   , 2.2 * (line_thickness.get() + line_doubleplain_lineseparation.get()));

	setwithmin(line_spring_threeDgap, 3*line_thickness.get(), pt2lencoord(2.5));

	vertex_thickness.set(1.5 * line_thickness.get());
	setwithmin(vertex_radius_small, rescale * 0.18, 1.5/2. 
	   * (line_thickness.get() + line_doubleplain_lineseparation.get()));
	vertex_radius_large.set(2.8 * vertex_radius_small.get());
	vertex_box_fill.settrue();

	shading_lines_thickness.set(0.75 * line_thickness.get());
	shading_lines_spacing.set(7.8 * shading_lines_thickness.get());

	shading_dots_radius.set(1.3 * line_thickness.get());
	shading_dots_spacing.set(4.3 * shading_dots_radius.get());

	setwithmin(fontsize, rescale * 1.2, pt2lencoord(8.5));

	if (parentpage->adjustparams != NULL)
		(*parentpage->adjustparams)(*this,rescale);
	}


// Get bounding box in user coords
boundingbox FeynDiagram::findbbox_usercoords()
	{
	boundingbox result;
	drawable *drawptr;

	drawptr = drawlist.head();
	while (drawptr != NULL)
		{
		result.include(drawptr->bbox());
		drawptr = drawptr->next();
		}
	result.expand(pad_bbox_top, pad_bbox_right, pad_bbox_bottom, pad_bbox_left);
	return result;
	}


// Unlike FDpart's which return bbox in the users coords, we give in points
boundingbox FeynDiagram::findbbox()
	{
	boundingbox result;
	drawable *drawptr;

	result = findbbox_usercoords();
	if (result.isdefined())
		return boundingbox(coord2pt(result.lowerleft),coord2pt(result.upperright));
	return result;
	}


void FeynDiagram::dodraw(ostream &out)
	{
	drawable *drawptr;

#ifdef DEBUG
	cerr << "outputting with head = " << (int)drawlist.head() << "\n";
#endif

	prt_begin(out);
	drawptr = drawlist.head();
	while (drawptr != NULL)
		{
		drawptr->draw(out);
#ifdef SHOWBBOX
		showbbox(out,drawptr->bbox());
#endif
		drawptr = drawptr->next();
		}
#ifdef SHOWBBOX
	showbbox(out,bbox());
#endif

	   // Now, lay a grid over the whole thing if we were requested to	
	if (gridon.get())
		{
		boundingbox bounds;
		double xmin, ymin, delta, xminpts, yminpts, deltapts;
		double xmax, ymax, tmp;
		int xsteps, ysteps;

#ifdef PSCOMMENTS
		out << "% Start of Grid\n";
#endif
		bounds = findbbox_usercoords();  // Probably shouldn't call directly...
		if (bounds.isdefined())
			{  
			   // Compute default xmin, xmax, ymin, ymax
			xmin = bounds.lowerleft.x();
			ymin = bounds.lowerleft.y();
			xmax = bounds.upperright.x();
			ymax = bounds.upperright.y();
			   /* Add a little border space & if grid is large enough,
			    * round the edges to be integers
			    */
			tmp = xmax - xmin;
			xmin -= GRID_HOR_BORDERFRACT * tmp;
			xmax += GRID_HOR_BORDERFRACT * tmp;
			if (tmp > GRID_MINDISTTOROUND)
				{
				xmin = (int)xmin - 1.0;
				xmax = (int)xmax + 1.0;
				}
			tmp = ymax - ymin;
			ymin -= GRID_VERT_BORDERFRACT * tmp;
			ymax += GRID_VERT_BORDERFRACT * tmp;
			if (tmp > GRID_MINDISTTOROUND)
				{
				ymin = (int)ymin - 1.0;
				ymax = (int)ymax + 1.0;
				}
			default_grid_xmin.set(xmin);
			default_grid_xmax.set(xmax);
			default_grid_ymin.set(ymin);
			default_grid_ymax.set(ymax);
			xmin = grid_xmin.get();
			xmax = grid_xmax.get();
			ymin = grid_ymin.get();
			ymax = grid_ymax.get();
			xsteps = (int)(xmax - xmin);
			if (xsteps < 5 || xsteps > 50)
				xsteps = GRID_DEFAULTSTEPS;
			delta = (xmax - xmin) / xsteps;
			ysteps = (int) ((ymax - ymin) / delta);
			xminpts = xcoord2pt(xmin);
			yminpts = ycoord2pt(ymin);
			deltapts = lencoord2pt(delta);
			out << xmin << " " << ymin << " " << delta << " ";
			out << xminpts << " " << yminpts << " " << deltapts << " ";
			out << xsteps << " " << ysteps << " grid\n";
			}
#ifdef PSCOMMENTS
		out << "% End of Grid\n\n";
#endif
		}
	}


void FeynDiagram::coordlimits(double xmin, double xmax, double yref)
	{
	xmin_coord = xmin;
	xmax_coord = xmax;
	yref_coord = yref;

	updatedefaults();
	}


double FeynDiagram::xcoord2pt(double x)
	{
	double result;

#ifdef DEBUG
//cerr << "xcoord2pt called with " << x ;
#endif
	result = (x - xmin_coord) / (xmax_coord - xmin_coord);
	result *= (xmax_inches - xmin_inches);
	result += xmin_inches;
	result = inch2pt(result);
#ifdef DEBUG
//cerr << " returning " << result << "\n";
//cerr << "xmin_coord xmax_coord " << xmin_coord << " " << xmax_coord << "\n";
#endif
	return result;
	}


double FeynDiagram::ycoord2pt(double y)
	{
	double result;

	result = (y - yref_coord) / (xmax_coord - xmin_coord);
	result *= (xmax_inches - xmin_inches);
	result += yref_inches;
	result = inch2pt(result);
	return result;
	}


xy FeynDiagram::coord2pt(const xy &pair)
	{
	return xy(xcoord2pt(pair.x()),ycoord2pt(pair.y()));
	}


double FeynDiagram::lencoord2pt(double w)
	{
	double result;

	result = w * fabs(xmax_inches - xmin_inches) / fabs(xmax_coord - xmin_coord);
	return inch2pt(result);
	}


double FeynDiagram::pt2lencoord(double w)
	{
	double result;

	result = w / 72.0;  // convert to inches
	result *= fabs(xmax_coord - xmin_coord) / fabs(xmax_inches - xmin_inches);
	return result;
	}


void FeynDiagram::pad_bbox(double top, double right, double bottom, double left)
	{
	pad_bbox_top = top;
	pad_bbox_right = right;
	pad_bbox_bottom = bottom;
	pad_bbox_left = left;
	}


void FeynDiagram::showbbox(ostream &out, boundingbox bb)
	{
	if (!bb.isdefined())
		return;
	out <<"% Start of bounding box\n";
	out << xcoord2pt(bb.lowerleft.x()) 
	   << " " << ycoord2pt(bb.lowerleft.y()) << " M2\n";
	out << xcoord2pt(bb.upperright.x()) 
	   << " " << ycoord2pt(bb.lowerleft.y()) << " L2\n";
	out << xcoord2pt(bb.upperright.x()) 
	   << " " << ycoord2pt(bb.upperright.y()) << " L2\n";
	out << xcoord2pt(bb.lowerleft.x()) 
	   << " " << ycoord2pt(bb.upperright.y()) << " L2\n";
	out << xcoord2pt(bb.lowerleft.x()) 
	   << " " << ycoord2pt(bb.lowerleft.y()) << " L2\n";
	out << "ST\n% End of bounding box\n\n";
	}


int FeynDiagram::find_prolog_needs()
	{
	int result = 0;
	drawable *drawptr;

	drawptr = drawlist.head();
	while (drawptr != NULL)
		{
		result |= drawptr->prolog_needs();
		drawptr = drawptr->next();
		}
	if (gridon.get())
		result |= PROLOG_NEEDS_GRID;
	return result;
	}


void page::insert_prolog_file(ostream &out, char *prefix, char *suffix)
	{
	char buf[1000];
	char fname[MAX_FILENAME_LEN];
	sprintf(fname,"%s%s",prefix,suffix);  
	ifstream fin(fname);

	if (!fin)
		{
		cerr << ERRSTR << "Cannot open prolog file: " << fname << "\n";
		exit(1);
		}

	while (fin)
		{
		fin.read(buf,sizeof(buf));
		out.write(buf,fin.gcount());
		   /* Next line is needed for some VMS compilers or they write the 
		    * last block twice.
		    */
		if (fin.gcount() < sizeof(buf))
			break;  
		}
	}


void page::prt_begin(ostream &out, int numpages)
	{
	char buf[1000];
	boundingbox bb;
	int prolog_flags;
	int blx, bly, bux, buy;

	cerr << "FeynDiagram by Bill Dimm, bdimm@hotneuron.com\n";
	cerr << "Version $Revision: 2.6 $   $Date: 2003/08/13 06:23:38 $\n";
#ifdef TESTVERSION
	cerr << TESTVERSIONCOMMENT << "\n";
#endif
	cerr << "\n";

	out << "%!\n";
	out << "%%Creator: FeynDiagram $Revision: 2.6 $  by Bill Dimm\n";
	bb = total_bbox(); 
		/* Convert bounding box to integer values - fractions screw-up some
		 * versions of xdvi.
		 */
	blx = (int)bb.lowerleft.x();
	bly = (int)bb.lowerleft.y();
	bux = (int)bb.upperright.x();
	buy = (int)bb.upperright.y();
	if (blx > bb.lowerleft.x())
		--blx;
	if (bly > bb.lowerleft.y())
		--bly;
	if (bux < bb.upperright.x())
		++bux;
	if (buy < bb.upperright.y())
		++buy;
	if (bb.isdefined())
		{
		out << "%%BoundingBox: " << blx << " " << bly;
		out << " " << bux << " " << buy << "\n";
		}
	out << "%%LanguageLevel: 1\n";
	out << "%%Pages: " << numpages << "\n";
	out << "%%EndComments\n";

	out << "%%BeginProlog\n";
	insert_prolog_file(out,prologdir,"abbrev.ps");
	insert_prolog_file(out,prologdir,"max.ps");
	prolog_flags = total_prolog_needs();
	if (prolog_flags & PROLOG_NEEDS_VERTEX)
		insert_prolog_file(out,prologdir,"vertex.ps");
	if (prolog_flags & PROLOG_NEEDS_ARROW)
		insert_prolog_file(out,prologdir,"arrow.ps");
	if (prolog_flags & PROLOG_NEEDS_FILL)
		insert_prolog_file(out,prologdir,"fill.ps");
	if (prolog_flags & PROLOG_NEEDS_ELLIPSE)
		insert_prolog_file(out,prologdir,"ellipse.ps");
	if (prolog_flags & PROLOG_NEEDS_TEXT)
		insert_prolog_file(out,prologdir,"text.ps");
	if (prolog_flags & PROLOG_NEEDS_TILDE)
		insert_prolog_file(out,prologdir,"tilde.ps");
	if (prolog_flags & PROLOG_NEEDS_GRID)
		insert_prolog_file(out,prologdir,"grid.ps");
	out << "%%EndProlog\n";
	}


void page::prt_end(ostream &out)
	{
	out << "%%EOF\n";
	}


page::page() : drawable(-1,&pagelist)
	{
	}


page::~page()
	{
	}


boundingbox page::findbbox()
	{
	boundingbox result;
	drawable *drawptr;

	drawptr = fdlist.head();
	while (drawptr != NULL)
		{
		result.include(drawptr->bbox());
		drawptr = drawptr->next();
		}
	return result;
	}


boundingbox page::total_bbox()
	{
	boundingbox result;
	drawable *drawptr;

	drawptr = pagelist.head();
	while (drawptr != NULL)
		{
		result.include(drawptr->bbox());
		drawptr = drawptr->next();
		}
	return result;
	}


void page::dodraw(ostream &out)
	{
	drawable *drawptr;
	boundingbox bb;
	int fdcnt = 0;

	out << "%%Page: " << currentpagenum << " " << currentpagenum << "\n";
	bb = bbox();
	if (bb.isdefined())
		{
		out << "%%PageBoundingBox: " << bb.lowerleft.x() << " " << bb.lowerleft.y();
		out << " " << bb.upperright.x() << " " << bb.upperright.y() << "\n";
		if (bb.lowerleft.x() < 0 || bb.lowerleft.y() < 0 
		   || bb.upperright.x() > PORTRAIT_MAXRIGHT 
		   || bb.upperright.y() > PORTRAIT_MAXTOP)
			cerr << ERRSTR << "Warning - this page may not fit on 8.5x11\" paper\n";
		}
	out << "%%PageOrientation: Portrait\n";
#ifndef NOSAVERESTORE
	out << "save\n";
#endif
	out << "\n";

	drawptr = fdlist.head();
	while (drawptr != NULL)
		{
		if (!drawptr->ishidden())
			{
			++fdcnt;
			cerr << "  FeynDiagram: " << fdcnt << "\n";
			drawptr->draw(out);
			}
		drawptr = drawptr->next();
		}

#ifndef NOSAVERESTORE
	out << "restore\n";
#endif
	out << "showpage\n";
	}




// Outputs ALL non-hidden pages
void page::output(ostream &out)
	{
	int pgnum = 0;
	drawable *drawptr;
	int numpages = 0;

	/* Count the pages (can't keep track as we go along since they might
	 * get hidden).
	 */
	drawptr = pagelist.head();
	while (drawptr != NULL)
		{
		if (!drawptr->ishidden())
			++numpages;
		drawptr = drawptr->next();
		}

	// Do the output
	prt_begin(out,numpages);
	drawptr = pagelist.head();
	while (drawptr != NULL)
		{
		if (!drawptr->ishidden())
			{
			++pgnum;
			cerr << " Page: " << pgnum << "\n";
			currentpagenum = pgnum;
			drawptr->draw(out);
			}
		drawptr = drawptr->next();
		}
	prt_end(out);
	currentpagenum = 0;
	}


int page::find_prolog_needs()
	{
	drawable *drawptr;
	int result = 0;

	drawptr = fdlist.head();
	while (drawptr != NULL)
		{
		result |= drawptr->prolog_needs();
		drawptr = drawptr->next();
		}
	return result;
	}


int page::total_prolog_needs()
	{
	drawable *drawptr;
	int result = 0;

	drawptr = pagelist.head();
	while (drawptr != NULL)
		{
		result |= drawptr->prolog_needs();
		drawptr = drawptr->next();
		}
	return result;
	}


shading_raw::shading_raw(FeynDiagram &feyndiag, double x1, double y1, double x2
   , double y2, drawable_list *dlist)
   : FDpart(feyndiag,0,dlist), minpt(x1,y1), maxpt(x2,y2)
	{
	}


int shading_raw::find_prolog_needs()
	{
	return PROLOG_NEEDS_FILL;
	}


shading_lines::shading_lines(FeynDiagram &feyndiag, double x1, double y1, double x2
   , double y2, drawable_list *dlist)
   : shading_raw(feyndiag,x1,y1,x2,y2,dlist)
	{
	angle.setparent(&fd->shading_lines_angle);
	thickness.setparent(&fd->shading_lines_thickness);
	spacing.setparent(&fd->shading_lines_spacing);
	}


void shading_lines::dodraw(ostream &out)
	{
	out << "NP\n";
	out << fd->lencoord2pt(thickness.get()) << " SLW\n";
	out << fd->lencoord2pt(spacing.get()) << " ";
	out << "{fillbox_lines} ";
	out << fd->xcoord2pt(minpt.x()) << " " << fd->ycoord2pt(minpt.y()) << " ";
	out << fd->xcoord2pt(maxpt.x()) << " " << fd->ycoord2pt(maxpt.y()) << " ";
	out << angle.get() << " fillbox_create\n";
	}


shading_dots::shading_dots(FeynDiagram &feyndiag, double x1, double y1, double x2
   , double y2, drawable_list *dlist)
   : shading_raw(feyndiag,x1,y1,x2,y2,dlist)
	{
	angle.setparent(&fd->shading_dots_angle);
	radius.setparent(&fd->shading_dots_radius);
	spacing.setparent(&fd->shading_dots_spacing);
	}


void shading_dots::dodraw(ostream &out)
	{
	out << "NP\n";
	out << fd->lencoord2pt(spacing.get()) << " ";
	out << fd->lencoord2pt(radius.get()) << " ";
	out << "{fillbox_dots} ";
	out << fd->xcoord2pt(minpt.x()) << " " << fd->ycoord2pt(minpt.y()) << " ";
	out << fd->xcoord2pt(maxpt.x()) << " " << fd->ycoord2pt(maxpt.y()) << " ";
	out << angle.get() << " fillbox_create\n";
	}


blob::blob(FeynDiagram &fd, const xy &centerval, double rx, double ry)
   : FDpart(fd,10), center(centerval), thickness(&fd.blob_thickness)
	{
	if (rx < 0)
		cerr << ERRSTR << "Warning: blob's x-radius is negative\n";
	if (ry < 0)
		cerr << ERRSTR << "Warning: blob's y-radius is negative\n";
	radiusx = rx;
	radiusy = ry;
	}


void blob::dodraw(ostream &out)
	{
	drawable *drawptr;

#ifdef PSCOMMENTS
	out << "% Start of blob\n";
#endif
	out << "gsave\nNP\n";
	out << fd->xcoord2pt(center.x()) << " " << fd->ycoord2pt(center.y()) << " ";
	out << fd->lencoord2pt(radiusx) << " " << fd->lencoord2pt(radiusy) << " ellipse\n";
	out << "clip\nNP\n";
	
	drawptr = drawlist.head();
	while (drawptr != NULL)	
		{
		drawptr->draw(out);
		drawptr = drawptr->next();
		}

	out << "grestore\n";
	out << "NP\n";
	out << fd->xcoord2pt(center.x()) << " " << fd->ycoord2pt(center.y()) << " ";
	out << fd->lencoord2pt(radiusx) << " " << fd->lencoord2pt(radiusy) << " ellipse\n";
	out << fd->lencoord2pt(thickness.get()) << " SLW\n";
	out << "ST\n";
#ifdef PSCOMMENTS
	out << "% End of blob\n\n";
#endif
	}


boundingbox blob::findbbox()
	{
	boundingbox result = boundingbox(center-xy(fabs(radiusx),fabs(radiusy))
	   ,center+xy(fabs(radiusx),fabs(radiusy)));
	result.expand(thickness.get() / 2);
	return result;
	}


xy blob::location(double angle)
	{
	double xpos, ypos;

	xpos = center.x() + radiusx * cos(angle * PI / 180);
	ypos = center.y() + radiusy * sin(angle * PI / 180);

	xy result(xpos,ypos);
	return result;
	}


shading_lines *blob::addshading_lines()
	{
	shading_lines *result;

	result = new shading_lines(*fd, center.x()-radiusx, center.y()-radiusy
	   , center.x()+radiusx, center.y()+radiusy, &drawlist);
	return result;
	}


shading_lines *blob::addshading_lines(double angleval)
	{
	shading_lines *result;

	result = addshading_lines();
	result->angle.set(angleval);
	return result;
	}

shading_dots *blob::addshading_dots()
	{
	shading_dots *result;

	result = new shading_dots(*fd, center.x()-radiusx, center.y()-radiusy
	   , center.x()+radiusx, center.y()+radiusy, &drawlist);
	return result;
	}


shading_dots *blob::addshading_dots(double angleval)
	{
	shading_dots *result;

	result = addshading_dots();
	result->angle.set(angleval);
	return result;
	}


int blob::find_prolog_needs()
	{
	int result = 0;
	drawable *drawptr;

	drawptr = drawlist.head();
	while (drawptr != NULL)	
		{
		result = drawptr->prolog_needs();
		drawptr = drawptr->next();
		}
	result |= PROLOG_NEEDS_ELLIPSE;
	return result;
	}
	

text::text(FeynDiagram &fd, char *str, const xy &location, double angle)
   : FDpart(fd,1), loc(location)
	{
	orient_angle = angle;
	fontname.setparent(&fd.fontname);
	fontsize.setparent(&fd.fontsize);
	textstr = str;
	specialpositioning = 0;
	}


text::text(FeynDiagram &fd, char *str, const xy &location, double hfract
   , double vfract, double angle)
   : FDpart(fd,1), loc(location)
	{
	orient_angle = angle;
	fontname.setparent(&fd.fontname);
	fontsize.setparent(&fd.fontsize);
	textstr = str;
	specialpositioning = 1;
	horizfract = hfract;
	vertfract = vfract;
	}


/* approx_strlen() tries to make a half-way-intelligent guess at the length
 * of the printed string.  It does this
 * by ignoring braces, underscores, ^'s, and counting "\TeXident" as
 * one character long (except for \font{fontname} which counts for zero).  
 * Also, lower case letters are counted less than upper
 * case ones since they are generally thinner (thus we return double instead
 * of int). Cleary this is only a very approximate approach.
 *
 * I have assumed that the font has proportional spacing - not a good
 * thing to do...
 */
double text::approx_strlen()
	{
	double result = 0;
	char *ptr;

	ptr = textstr;
	while (*ptr != '\0')
		{
		if (strchr("{}_^",*ptr) != NULL)
			{
			++ptr;
			continue;
			}
		if (*ptr == '\\')
			{
			++ptr;
			if (strncmp(ptr,"font",4) == 0 && ptr[4] == '{')
				{  // if font change, don't count it in length
				ptr += 4;
				while (*ptr != '}' && *ptr != '\0')
					++ptr;
				}
			else
				{
				while (isalpha(*ptr))
					++ptr;  // Skip TeX identifier
				result += 1.0;
				}
			}
		else
			{
			if (strchr(" ,.;:'`!|il1I",*ptr) != NULL)
				result += 0.4;  // Really thin characters
			else if (islower(*ptr) && (strchr("mw",*ptr) == NULL))
				result += 0.6;  // Medium width characters
			else
				result += 1.0;  // Potentially wide characters
			++ptr;
			}
		}
	return result;
	}


void text::dodraw(ostream &out)
	{
#ifdef PSCOMMENTS
	out << "% Start of text\n";
#endif
	out << fd->xcoord2pt(loc.x()) << " " << fd->ycoord2pt(loc.y()) << " ";
	out << orient_angle << " txtsta\n";
	out << "0 0 M2\n";
	if (specialpositioning)
		text2PS(out,textstr,horizfract,vertfract,fd->lencoord2pt(fontsize.get())
		   ,fontname.get());
	else
		text2PS(out,textstr,fd->lencoord2pt(fontsize.get()),fontname.get());
	out << "txtend\n";
#ifdef PSCOMMENTS
	out << "% End of text\n\n";
#endif
	}


/* Here, we make a REALLY pathetic attempt at guessing at the
 * size of the bounding box - pretty hard with all the TeX junk
 * and unknown font width.
 */
boundingbox text::findbbox()
	{
	boundingbox result;
	double width, height;
	xy ll, ur;

	width = approx_strlen() * fontsize.get() * 0.9;
	height = fontsize;
	if (!specialpositioning)
		{
		ll = xy(0,-.5*height);
		ur = xy(width,height);
		}
	else
		{
		xy center(horizfract*width, vertfract*1.5*height);
		ll = xy(0,-.5*height) - center;
		ur = xy(width,height) - center;
		}
	
	result.include(loc + ll.rotate(orient_angle));  // lower left
	result.include(loc + xy(ur.x(),ll.y()).rotate(orient_angle)); // lower right
	result.include(loc + xy(ll.x(),ur.y()).rotate(orient_angle)); // upper left
	result.include(loc + ur.rotate(orient_angle)); // upper right
	return result;
	}


int text::iscontained(char *str)
	{
	char *ptr;
	int len;

	ptr = textstr;
	len = strlen(str);
	while (*ptr != '\0')
		{
		if (*ptr == *str)
			if (strncmp(ptr,str,len) == 0)
				return 1;
		++ptr;
		}
	return 0;
	}


int text::find_prolog_needs()
	{
	int result;
	result = PROLOG_NEEDS_TEXT;
	if (iscontained("tilde"))
		result |= PROLOG_NEEDS_TILDE;
	if (iscontained("arrow"))
		result |= PROLOG_NEEDS_ARROW;
	return result;
	}


#define RESCALEFACT 1.4
#define RESCALE(_x) fd._x.set(RESCALEFACT * fd._x.get())
#define SMALLRESCALEFACT 1.2
#define SMALLRESCALE(_x) fd._x.set(SMALLRESCALEFACT * fd._x.get())

/* Adjusts parameters to be approprate for 50% photo reduction twice
 */
void params4reduction(FeynDiagram &fd, double rescale)
	{
	RESCALE(line_thickness);
	RESCALE(vertex_thickness);
	RESCALE(shading_lines_thickness);
	SMALLRESCALE(vertex_radius_small);
	SMALLRESCALE(line_arrowlen);
	SMALLRESCALE(line_doubleplain_lineseparation);
	SMALLRESCALE(line_doubleplain_arrowlen);
	fd.fontname.set("Helvetica-Bold");
	}


