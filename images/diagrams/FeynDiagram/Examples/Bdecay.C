#include <FeynDiagram/fd.h>

  /* ANGLE is the angle for the W boson and pion */
#define ANGLE 70.0

/* A heavy quark symmetry diagram for B -> D,pi,l,nu  */

main()
	{
	page pg;
	FeynDiagram fd(pg,2,6.5,4.3);

	xy pt1(-10,-5), pt2(10,-5);    /* endpoints for heavy meson line */
	vertex_dot v1(fd,-4,-5), v2(fd,4,-5);
	  /* heavy meson lines, l1, l2, l3 */
	line_doubleplain l1(fd,pt1,v1), l2(fd,v1,v2), l3(fd,v2,pt2);
	  /* label the diagram */
	text tx1(fd,"\\overline{B}",l1.curve->f(.5)+xy(0,-1.5));
	text tx2(fd,"D^\\ast",l2.curve->f(.5)+xy(0,-1.5));
	text tx3(fd,"D",l3.curve->f(.5)+xy(0,-1.5));
	text tx4(fd,"v",l1.curve->f(.5)+xy(0,.5));
	text tx5(fd,"\\sim{}v^\\prime",l2.curve->f(.5)+xy(0,.5));
	text tx6(fd,"v^\\prime",l3.curve->f(.5)+xy(0,.5));

	   /* attach the Z, lepton & neutrino */
	vertex_dot v3(fd,v1+polar(4,ANGLE));
	line_wiggle Z(fd,v1,v3);
	   /* Flip the Z's line so that it doesn't overlap with heavy meson */
	Z.width.scale(-.8);
	line_plain electron(fd,v3,v3+polar(2.5,ANGLE+30));
	line_plain neutrino(fd,v3+polar(2.5,ANGLE-30),v3);
	text tx7(fd,"l",electron.curve->f(1)+xy(-.4,.3));
	   /* Use NewCenturySchlbk-Italic font - best we can do for script l */
	tx7.fontname.set("NewCenturySchlbk-Italic");
	text tx8(fd,"\\overline\\nu_l",neutrino.curve->f(0)+xy(.3,.3));
	tx8.fontname.set("NewCenturySchlbk-Italic");

	   /* Now the pion */
	line_plain pion(fd,v2,v2+polar(6,ANGLE));
	pion.dashon.settrue();
	pion.arrowon.setfalse();
	text tx9(fd,"\\pi",pion.curve->f(1)+xy(.3,.3));
	text tx10(fd,"q",pion.curve->f(.5)+xy(.5,0));

	pg.output();
	return 0;
	}
