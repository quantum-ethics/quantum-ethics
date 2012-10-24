#include <FeynDiagram/fd.h>

main() {
	page pg;
	
	const double xmax = 6;
	
	FeynDiagram fd( pg, 4.25 - 32.5 / xmax, 4.25 + 32.5 / xmax, 5.5 );
	
	//fd.gridon.settrue();
	
	xy e1( 0, -10 ), e2( 0, 10 );
	vertex_dot v1( fd, 0, -5 ), v2( fd, 0, 5 );
	
	line_plain f1( fd, e1, v1 ), f2( fd, v1, v2 ), f3( fd, v2, e2 );
	line_wiggle photon( fd, v1, v2 );
	
	photon.arcthru( -5, 0 );
	f2.arcthru( 5, 0 );
	
	text t1( fd, "f_{q_f, \\lambda_f}", f1.curve->f( 0.5 ) + f1.curve->tang( 0.5 ).perp() + xy( 0, -1 ), 90 );
	text t2( fd, "f_{q_f - q_\\gamma, \\lambda}", f2.curve->f( 0.5 ) - f2.curve->tang( 0.5 ).perp() + xy( 0.5, -1 ), 90 );
	text t3( fd, "\\gamma_{q_\\gamma, \\lambda_\\gamma}", photon.curve->f( 0.5 ) + photon.curve->tang( 0.5 ).perp() + xy( 0, -1 ), 90 );
	text t4( fd, "f_{q_f, \\lambda_f}", f3.curve->f( 0.5 ) + f3.curve->tang( 0.5 ).perp() + xy( 0, -1 ), 90 );
	
	pg.output();
	return 0;
}
