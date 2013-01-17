#include <FeynDiagram/fd.h>

main() {
	page pg;
	
	const double xmax = 3.5;
	
	FeynDiagram fd( pg, 4.25 - 32.5 / xmax, 4.25 + 32.5 / xmax, 5.5 );
	
	//fd.gridon.settrue();
	
	vertex_dot v1( fd, 2, -5 ), v2( fd, -2 * .67, 5 * .67 ), v3( fd, -2.2, 5.5 );
	
	line_wiggle photon( fd, v1, v2 );
	line_plain arrow( fd, v2, v3 );
	
	v1.hide();
	v2.hide();
	v3.hide();
	arrow.arrowposition.set(1);
	
	photon.width.set(photon.width.get() * 2.6);
	arrow.arrowlen.set(arrow.arrowlen.get() * 4);
	arrow.arrowangle.set(33);
	photon.thickness.set(photon.thickness.get()*3);
	arrow.thickness.set(arrow.thickness.get()*3);
	
	pg.output();
	return 0;
}
