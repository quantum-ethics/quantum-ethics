#include <FeynDiagram/fd.h>

#define EG3_RADX 2.5
#define EG3_RADY 1.2

/* from B&D fig 19.14 */

main()
	{
	page pg;
	FeynDiagram fd(pg);

	xy e1(-10,-5), e2(10,-5);

	   // blobs 
	xy c1(-3,-5), c2(5,-5), c3(-1,.5);
	blob b1(fd,c1,EG3_RADX,EG3_RADY);
	blob b2(fd,c2,EG3_RADX,EG3_RADY);
	blob b3(fd,c3,EG3_RADX,EG3_RADY);
	b1.addshading_lines();
	b2.addshading_dots(30);
	b3.addshading_lines();

	vertex_dot v1(fd,xy(-8,-5)), v2(fd,b1.location(180));
	vertex_dot v3(fd,b1.location(0)), v4(fd,b2.location(180));
	vertex_dot v5(fd,b2.location(0));
	vertex_dot v6(fd,b3.location(180)), v7(fd,b3.location(0));
	vertex_dot v8(fd,b2.location(90));

	   /* fermions */
	line_plain f1(fd,e1,v1), f2(fd,v1,v2), f3(fd,v3,v4), f4(fd,v5,e2);

	   /* photons */	 
	line_wiggle p1(fd,v1,v6), p2(fd,v7,v8);
	xy delta;
	delta = v6 - v1;
	p1.arcthru(v1 + 0.5*delta + 1.3 * delta.perp());
	delta = v8 - v7;
	p2.arcthru(v7 + 0.5*delta + 1.0 * delta.perp());

	   /* labels */
	text t1(fd,"p",f1.curve->f(0.5) + xy(-.2,.7));
	text t2(fd,"p-k",f2.curve->f(0.5) + xy(-.5,-1));
	text t3(fd,"p-k",f3.curve->f(0.5) + xy(-.65,-1));
	text t4(fd,"p",f4.curve->f(0.5) + xy(-.2,.7));
	text t5(fd,"\\nu",v1 + xy(-.2,-.9));
	text t6(fd,"\\mu",v5 + polar(0.9,-80));
	text t7(fd,"k",p1.curve->f(0.5) + 1.1 * p1.curve->tang(0.5).perp());
	text t8(fd,"k",p2.curve->f(0.5) + 0.8 * p2.curve->tang(0.5).perp());

	pg.output();
	return 0;
	}

