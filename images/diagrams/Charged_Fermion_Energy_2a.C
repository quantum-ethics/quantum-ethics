#include <FeynDiagram/fd.h>

main() {
	page pg;
	
	const double xmax = 5;
	
	FeynDiagram fd( pg, 4.25 - 32.5 / xmax, 4.25 + 32.5 / xmax, 5.5 );
	
	//fd.gridon.settrue();
	
	vertex_dot v1( fd, -5, 0 ), v2( fd, 5, 0 );
	
	line_plain f1( fd, v1, v2 ), f2( fd, v2, v1 );
	line_wiggle photon( fd, v1, v2 );
	
	f1.arcthru( 0, 5 );
	f2.arcthru( 0, -5 );
	
	text t1( fd, "f_{q_f, \\lambda_f}", f1.curve->f( 0.5 ) + f1.curve->tang( 0.5 ).perp() + xy( -1, 0 ) );
	text t2( fd, "\\overline{f}_{- q_f - q_\\gamma, \\lambda}", f2.curve->f( 0.5 ) + f2.curve->tang( 0.5 ).perp() + xy( -1, -0.5 ) );
	text t3( fd, "\\gamma_{q_\\gamma, \\lambda_\\gamma}", photon.curve->f( 0.5 ) + photon.curve->tang( 0.5 ).perp() + xy( -1, 0 ) );
	
	pg.output();
	return 0;
}
