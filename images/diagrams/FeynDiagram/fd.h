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
 *    author's permission.
 *------------------------------------------------------------------------
 */

#ifndef FD_H
#  define FD_H

// $Id: fd.h,v 2.6 2003/08/13 06:23:38 billd Exp $

#include <iostream.h>
#include <math.h>
#include <FeynDiagram/default.h>

class drawable;
class drawable_list;
class FeynDiagram;
class page;


class xy    
	{
	private:
		double        xval, yval;
		int           isdefinedflag;

	public:
		              xy    ();
		              xy    (double x, double y);
		double        x() const;
		double        y() const;
		double        len() { return sqrt(x()*x() + y()*y()); }
		double        angle();
		xy            perp();
		xy            &rotate(double angle);
		xy            operator*=(double s);
		xy            operator/=(double s);
		xy            operator+=(xy p);
		xy            operator-=(xy p);
		              //operator double() { return angle(); }
	};

extern xy polar(double r, double theta);

xy operator+(xy p);
xy operator-(xy p);
xy operator+(xy p1, xy p2);
xy operator-(xy p1, xy p2);
xy operator*(double s1, xy p2);
xy operator*(xy p1, double s2);
xy operator/(xy p1, double s2);
double operator*(xy p1, xy p2);


class boundingbox
	{
	private:
		int           isdefinedflag;

	public:
		              boundingbox() { isdefinedflag = 0; }
		              boundingbox(const xy &ll, const xy &ur);
		int           isdefined() { return isdefinedflag; }
		xy            lowerleft;
		xy            upperright;
		void          include(boundingbox box);
		void          include(const xy &point) { include(boundingbox(point,point)); }
		void          expand(double top, double right, double bottom, double left);
		void          expand(double amt) { expand(amt, amt, amt, amt); }
	};


class drawable_list
	{
	private:
		drawable      *headptr;

	public:
		              drawable_list() { headptr = NULL; }
		void          add(drawable *ptr);
		void          remove(drawable *ptr);
		drawable      *head() { return headptr; }
	};


/* Note: bbox() returns a result in user's coord space for FDpart's and
 * it returns a result in points (ie. 72th of an inch) for FeynDiagram's
 */
class drawable
	{
	private:
		drawable      *nextptr;
		drawable      *prevptr;
		int           ishiddenflag;
		friend void   drawable_list::add(drawable *newlink);
		friend void   drawable_list::remove(drawable *link);

	protected:
		int           priority;  // Highest priority is drawn first
		drawable_list *drawlist;  // name of list this is linked into
		virtual void  dodraw(ostream &out);
		virtual boundingbox findbbox();

	public:
		              drawable(int priority, drawable_list *dlist = NULL);
		virtual       ~drawable();
		drawable      *next() { return nextptr; }
		int           ishidden() { return ishiddenflag; }
		void          hide() { ishiddenflag = 1; }
		void          draw(ostream &out = cout);
		boundingbox   bbox();
		virtual int   find_prolog_needs();
		int           prolog_needs();  // gives flags for what prologs are needed
	};


class FDpart : public drawable
	{
	protected:
		FeynDiagram   *fd;

	public:
		              FDpart(FeynDiagram &fd, int priority, drawable_list *dlist = NULL);
	};


class curve_raw
	{
	protected:
		int           typeval;

	public:
		int           type() { return typeval; }
		enum          curvetypes { straightline, arc };
		virtual void  func(double t, double &x, double &y);
		virtual void  deriv(double t, double &dt, double &dx, double &dy, double prec);
		virtual double dist(double t, double prec, double ref);
		virtual double tfromdist(double dist, double tmin, double tmax, double prec);
		xy            f(double t);
		xy            tang(double t, double prec = .0001);
	};


class curve_straightline : public curve_raw
	{
	private:
		xy            loc1, loc2;
		double        length;
		void          init();

	public:
		              curve_straightline(const xy &loc1, const xy &loc2);
		void          func(double t, double &x, double &y);
		void          deriv(double t, double &dt, double &dx, double &dy, double prec = .001);
		double        dist(double t, double prec, double ref = 0.0);
		double        tfromdist(double dist, double tmin = 0.0, double tmax = 1.0, double prec = 0.0001);
	};


class curve_arc : public curve_raw
	{
	private:
		double        radius;
		double        theta1;  // angles in degrees
		double        theta2;

	protected:
		xy            center;

	public:
		              curve_arc(const xy &center, double r, double theta1, double theta2);
		void          func(double t, double &x, double &y);
		void          deriv(double t, double &dt, double &dx, double &dy, double prec = .001);
		double        dist(double t, double prec, double ref = 0.0);
		double        tfromdist(double dist, double tmin = 0.0, double tmax = 1.0, double prec = 0.0001);
		double        rad() { return radius; }
		double        centerx() { return center.x(); }
		double        centery() { return center.y(); }
		double        angle1() { return theta1; }
		double        angle2() { return theta2; }
	};


class FeynDiagram : public drawable
	{
	private:
		static const double  xmin_inches_default;
		static const double  xmax_inches_default;
		static const double  yref_inches_default;

		static const double  xmin_coord_default;
		static const double  xmax_coord_default;
		static const double  yref_coord_default;

		double        xmin_inches;
		double        xmax_inches;
		double        yref_inches;

		double        xmin_coord;
		double        xmax_coord;
		double        yref_coord;

		deflt_double  defaultscalefactor;
		deflt_double  default_grid_xmin;
		deflt_double  default_grid_xmax;
		deflt_double  default_grid_ymin;
		deflt_double  default_grid_ymax;

		page          *parentpage;

		static const double  pad_bbox_default;

		double        pad_bbox_top;
		double        pad_bbox_right;
		double        pad_bbox_bottom;
		double        pad_bbox_left;

		void          prt_begin(ostream &out);
		double        inch2pt(double x) { return x * 72.0; }
		void          setwithmin(deflt_double &param, double val, double min);
		friend        FDpart::FDpart(FeynDiagram &fd,int priority,drawable_list *);
		drawable_list drawlist;
		void          dodraw(ostream &out);
		boundingbox   findbbox_usercoords();
		boundingbox   findbbox();
		void          showbbox(ostream &out, boundingbox bb);

	public:
		void          init(page &pg,double xmin_in, double xmax_in, double yref_in);
		deflt_double  scalefactor;

		deflt_double  vertex_thickness;
		deflt_double  vertex_radius_small;
		deflt_double  vertex_radius_large;
		deflt_boolean vertex_box_fill;

		deflt_double  line_width;
		deflt_double  line_lwratio;
		deflt_double  line_thickness;
		deflt_boolean line_dashon;
		deflt_double  line_spacelen;
		deflt_double  line_dsratio;
		deflt_boolean line_arrowon;
		deflt_double  line_arrowposition;
		deflt_double  line_arrowlen;
		deflt_double  line_arrowangle;

		deflt_double  line_plain_thickness;
		deflt_boolean line_plain_dashon;
		deflt_double  line_plain_spacelen;
		deflt_double  line_plain_dsratio;
		deflt_boolean line_plain_arrowon;
		deflt_double  line_plain_arrowposition;
		deflt_double  line_plain_arrowlen;
		deflt_double  line_plain_arrowangle;

		deflt_double  line_doubleplain_thickness;
		deflt_boolean line_doubleplain_dashon;
		deflt_double  line_doubleplain_spacelen;
		deflt_double  line_doubleplain_dsratio;
		deflt_boolean line_doubleplain_arrowon;
		deflt_double  line_doubleplain_arrowposition;
		deflt_double  line_doubleplain_arrowlen;
		deflt_double  line_doubleplain_arrowangle;
		deflt_double  line_doubleplain_lineseparation;

		deflt_double  line_wiggle_width;
		deflt_double  line_wiggle_lwratio;
		deflt_double  line_wiggle_thickness;
		deflt_boolean line_wiggle_dashon;
		deflt_double  line_wiggle_spacelen;
		deflt_double  line_wiggle_dsratio;
		deflt_boolean line_wiggle_arrowon;
		deflt_double  line_wiggle_arrowposition;
		deflt_double  line_wiggle_arrowlen;
		deflt_double  line_wiggle_arrowangle;
		deflt_double  line_wiggle_fract;

		deflt_double  line_spring_width;
		deflt_double  line_spring_lwratio;
		deflt_double  line_spring_thickness;
		deflt_boolean line_spring_dashon;
		deflt_double  line_spring_spacelen;
		deflt_double  line_spring_dsratio;
		deflt_boolean line_spring_arrowon;
		deflt_double  line_spring_arrowposition;
		deflt_double  line_spring_arrowlen;
		deflt_double  line_spring_arrowangle;
		deflt_double  line_spring_fract1;
		deflt_double  line_spring_fract2;
		deflt_double  line_spring_fract3;
		deflt_double  line_spring_fract4;
		deflt_boolean line_spring_threeD;
		deflt_double  line_spring_threeDgap;

		deflt_double  line_zigzag_width;
		deflt_double  line_zigzag_lwratio;
		deflt_double  line_zigzag_thickness;
		deflt_boolean line_zigzag_dashon;
		deflt_double  line_zigzag_spacelen;
		deflt_double  line_zigzag_dsratio;
		deflt_boolean line_zigzag_arrowon;
		deflt_double  line_zigzag_arrowposition;
		deflt_double  line_zigzag_arrowlen;
		deflt_double  line_zigzag_arrowangle;

		deflt_double  shading_lines_angle;
		deflt_double  shading_lines_thickness;
		deflt_double  shading_lines_spacing;

		deflt_double  shading_dots_angle;
		deflt_double  shading_dots_radius;
		deflt_double  shading_dots_spacing;

		deflt_double  blob_thickness;

		deflt_str     fontname;
		deflt_double  fontsize;

		static deflt_boolean global_gridon;
		deflt_boolean gridon;
		deflt_double  grid_xmin;
		deflt_double  grid_xmax;
		deflt_double  grid_ymin;
		deflt_double  grid_ymax;

		              FeynDiagram(page &pg);
		              FeynDiagram(page &pg, double xmin_in, double xmax_in, double yref_in);
		void          updatedefaults();
		void          coordlimits(double xmin, double xmax, double yref);
		double        xcoord2pt(double x);
		double        ycoord2pt(double y);
		xy            coord2pt(const xy &pair);
		double        lencoord2pt(double w);
		double        pt2lencoord(double w);
		double        inch2lencoord(double w) { return pt2lencoord(72.*w); }
		int           find_prolog_needs();  // virtual
		void          pad_bbox(double top, double right, double bottom, double left);
	};


class page : public drawable
	{
	private:
		friend        void FeynDiagram::init(page &, double, double, double);
		drawable_list fdlist;
		void          prt_begin(ostream &out, int numpages);
		void          prt_end(ostream &out);
		static drawable_list pagelist;
		static int    currentpagenum;  // Which one it is currently outputting
		void          dodraw(ostream &out);
		boundingbox   findbbox();
		void          insert_prolog_file(ostream &out, char *prefix, char *suffix);

	public:
		static void   (*adjustparams)(FeynDiagram &fd, double rescale);
		static char   *prologdir;

		              page();
		virtual       ~page();
		void          output(ostream &out = cout);
		boundingbox   total_bbox();  // Gets bounding box for all pages
		int           find_prolog_needs();
		int           total_prolog_needs();  // Gets prolog needs for all pages
	};


class vertex_raw : public FDpart 
	{
	private:
		xy            loc;
		double        orient_angle;
		void          init(FeynDiagram &fd, double dx, double dy);
		void          dodraw(ostream &out);
		boundingbox   findbbox();

	protected:
		virtual char  *procedurestr();

	public:
		deflt_double  radius;
		deflt_double  thickness;

		              vertex_raw(FeynDiagram &fd,const xy &location,double angle);
		operator xy () { return loc; }
		int           find_prolog_needs();  // virtual
	};


  // This vertex doesn't do any output
class vertex : public vertex_raw
	{
	private:
		char          *procedurestr();
	
	public:
		              vertex(FeynDiagram &fd,const xy &location);
		              vertex(FeynDiagram &fd,double x,double y);
	};


class vertex_dot : public vertex_raw
	{
	private:
		char          *procedurestr();

	public:
		              vertex_dot(FeynDiagram &fd,const xy &location);
		              vertex_dot(FeynDiagram &fd,double x,double y);
	};

		
class vertex_cross : public vertex_raw
	{
	private:
		char          *procedurestr();

	public:
		              vertex_cross(FeynDiagram &fd,const xy &location
		                 ,double angle = 0.0);
		              vertex_cross(FeynDiagram &fd,double x,double y
		                 ,double angle = 0.0);
	};


class vertex_circlecross : public vertex_raw
	{
	private:
		char          *procedurestr();

	public:
		              vertex_circlecross(FeynDiagram &fd,const xy &location
		                 ,double angle = 0.0);
		              vertex_circlecross(FeynDiagram &fd,double x,double y
		                 ,double angle = 0.0);
	};


class vertex_box : public vertex_raw
	{
	private:
		char          *procedurestr();

	public:
		              vertex_box(FeynDiagram &fd,const xy &location
		                 ,double angle = 0.0);
		              vertex_box(FeynDiagram &fd,double x,double y
		                 ,double angle = 0.0);
		deflt_boolean fill;
	};


class line_raw : public FDpart
	{
	private:
		int           do_whiteline;

	protected:
		void          dodraw(ostream &out);
		virtual void  drawline(ostream &out,int whiteline, double thick);
		boundingbox   pathbbox();
		double        findangle(double dx,double dy);

	public:
		curve_raw     *curve;
		deflt_double  thickness;
		deflt_boolean dashon;
		deflt_double  spacelen;
		deflt_double  dsratio;
		deflt_boolean arrowon;
		deflt_double  arrowposition;
		deflt_double  arrowlen;
		deflt_double  arrowangle;
		              line_raw(FeynDiagram &fd, const xy &loc1, const xy &loc2);
		              ~line_raw();
		void          arcthru(const xy &location, int ccw = 0);
		void          arcthru(double x, double y, int ccw = 0);
		int           find_prolog_needs();  // virtual
		void          ontop(int level);
	};


class line_plain : public line_raw
	{
	private:
		void          drawline(ostream &out,int whiteline, double thick);
		boundingbox   findbbox();
		static const double  positionprec;
		static const double  derivprec;
		void          init();

	public:
		              line_plain(FeynDiagram &fd, const xy &loc1, const xy &loc2);
	};


class line_doubleplain : public line_raw
	{
	private:
		void          drawline(ostream &out,int whiteline, double thick);
		boundingbox   findbbox();
		static const double  positionprec;
		static const double  derivprec;
		void          init();

	public:
		deflt_double  lineseparation;

		              line_doubleplain(FeynDiagram &fd, const xy &loc1, const xy &loc2);
	};


class line_wiggle : public line_raw
	{
	private:
		void          drawline(ostream &out,int whiteline, double thick);
		boundingbox   findbbox();
		static const double  positionprec;
		static const double  derivprec;
		void          init();

	public:
		deflt_double  width;
		deflt_double  lwratio;
		deflt_double  fract;

		              line_wiggle(FeynDiagram &fd, const xy &loc1, const xy &loc2);
	};


class line_spring : public line_raw
	{
	private:
		void          drawline(ostream &out,int whiteline, double thick);
		boundingbox   findbbox();
		static const double  positionprec;
		static const double  derivprec;
		void          init();

	public:
		deflt_double  width;
		deflt_double  lwratio;
		deflt_double  fract1;
		deflt_double  fract2;
		deflt_double  fract3;
		deflt_double  fract4;
		deflt_boolean threeD;
		deflt_double  threeDgap;

		              line_spring(FeynDiagram &fd, const xy &loc1, const xy &loc2);
	};



class line_zigzag : public line_raw
	{
	private:
		void          drawline(ostream &out,int whiteline, double thick);
		boundingbox   findbbox();
		static const double  positionprec;
		static const double  derivprec;
		void          init();

	public:
		deflt_double  width;
		deflt_double  lwratio;

		              line_zigzag(FeynDiagram &fd, const xy &loc1, const xy &loc2);
	};


class shading_raw : public FDpart
	{
	protected:
		xy            minpt, maxpt;

	public:
		deflt_double  angle;
		              shading_raw(FeynDiagram &feyndiag, double x1, double y1
		                 ,double x2, double y2, drawable_list *dlist = NULL);
		int           find_prolog_needs();  // virtual
	};


class shading_lines : public shading_raw
	{
	private:
		void          dodraw(ostream &out);

	public:
		deflt_double  thickness;
		deflt_double  spacing;

		              shading_lines(FeynDiagram &feyndiag, double x1, double y1
		                 ,double x2, double y2, drawable_list *dlist = NULL);
	};


class shading_dots : public shading_raw
	{
	private:
		void          dodraw(ostream &out);

	public:
		deflt_double  radius;
		deflt_double  spacing;

		              shading_dots(FeynDiagram &feyndiag, double x1, double y1
		                 ,double x2, double y2, drawable_list *dlist = NULL);
	};


class blob : public FDpart
	{
	private:
		drawable_list drawlist;
		xy            center;
		double        radiusx;
		double        radiusy;
		void          dodraw(ostream &out);
		boundingbox   findbbox();

	public:
		deflt_double  thickness;

		              blob(FeynDiagram &fd, const xy &centerval, double rx, double ry);
		xy            location(double angle);
		shading_lines *addshading_lines();
		shading_lines *addshading_lines(double angle);
		shading_dots  *addshading_dots();
		shading_dots  *addshading_dots(double angle);
		int           find_prolog_needs();  // virtual
	};
	

class text : public FDpart
	{
	private:
		xy            loc;
		double        orient_angle;
		char          *textstr;
		double        approx_strlen();
		void          dodraw(ostream &out);
		boundingbox   findbbox();
		double        horizfract;
		double        vertfract;
		int           specialpositioning;  // Tell if should use horizfract etc.
		int           iscontained(char *str);
	
	public:
		deflt_str     fontname;
		deflt_double  fontsize;

		              text(FeynDiagram &fd, char *str, const xy &location
		                 , double angle = 0.0);
		              text(FeynDiagram &fd, char *str, const xy &location
		                 , double xfract, double yfract, double angle = 0.0);
		int           find_prolog_needs();  // virtual
	};


extern void params4reduction(FeynDiagram &fd, double rescale);

#endif  // ifndef FD_H
