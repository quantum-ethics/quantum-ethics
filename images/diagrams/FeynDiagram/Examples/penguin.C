#include <FeynDiagram/fd.h>

#define Y1 -3.5
#define Y2 3.5
#define Y3 6.5
#define X1 3.5
#define YGAP 0.7

/* penguin diagram */

main()	
	{
	page pg;
	FeynDiagram fd(pg);

	/* incoming lines: db_in (d-bar), s_in (s)
	 * outgoing lines: db_out (d-bar), d_out (d), ub_out (u-bar), u_out (u)
	 */
	xy db_in(-8,Y1), s_in(-8,Y2), db_out(8,Y1), d_out(8,Y2);
	xy ub_out(d_out-xy(0,YGAP)), u_out(db_out+xy(0,YGAP));

	vertex v1(fd,0,Y1), v2(fd,-X1,Y3), v3(fd,X1,Y3), v4(fd,0,Y2);

	line_plain d1(fd,v1,db_in), d2(fd,db_out,v1);
	line_plain s1(fd,s_in,v2), intermediate(fd,v2,v3), d3(fd,v3,d_out);
	s1.arcthru( (s_in + v2)/2. + .6*xy(1,-1) );
	d3.arcthru( (d_out + v3)/2. + .6*xy(-1,-1) );
	intermediate.arrowon.setfalse();
	intermediate.arcthru(v4);
	line_plain u1(fd,ub_out,u_out);
	u1.arcthru(5,(Y1+Y2)/2.);

	line_spring gluon(fd,v1,v4);
	gluon.threeD.settrue();

	line_wiggle W(fd,v2,v3);
	W.arcthru(0,Y3+(Y3-Y2));

	// Elementary particle labels
	xy hgap(.2,0);
	text s_in_txt(fd,"s",s_in-hgap,1,.5);
	text db_in_txt(fd,"\\overline{d}",db_in-hgap,1,.5);
	text gluon_txt(fd,"g",gluon.curve->f(.5)+xy(.8,0));
	text intermediat_txt(fd,"u, c, t", v4+xy(0,.5), .5, 0);
	text W_txt(fd, "W", W.curve->f(.5) + xy(0,.5), .5, 0);
	text d_out_txt(fd, "d", d_out+hgap, 0, .2);
	text ub_out_txt(fd, "\\overline{u}",ub_out+hgap, 0, .8);
	text u_out_txt(fd, "u", u_out+hgap, 0, .2);
	text db_out_txt(fd, "\\overline{d}", db_out+hgap, 0, .8);

	pg.output();
	return 0;
	}
