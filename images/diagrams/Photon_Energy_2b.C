#include <FeynDiagram/fd.h>

main() {
	page pg;
	
	const double xmax = 6;
	
	FeynDiagram fd( pg, 4.25 - 32.5 / xmax, 4.25 + 32.5 / xmax, 5.5 );
	
	//fd.gridon.settrue();
	
	xy e1( 0, -10 ), e2( 0, 10 );
	vertex_dot v1( fd, 0, -5 ), v2( fd, 0, 5 );
	
	line_wiggle photon1( fd, e1, v1 ), photon2( fd, v2, e2 );
	line_plain f1( fd, v1, v2 ), f2( fd, v2, v1 );
	
	f1.arcthru( -5, 0 );
	f2.arcthru( 5, 0 );
	
	text t1( fd, "\\gamma_{q_\\gamma, \\lambda_\\gamma}", photon1.curve->f( 0.5 ) + photon1.curve->tang( 0.5 ).perp() + xy( 0, -1 ), 90 );
	text t2( fd, "\\phi_{q + q_\\gamma, \\lambda'}", f1.curve->f( 0.5 ) + f1.curve->tang( 0.5 ).perp() + xy( 0, -1 ), 90 );
	text t3( fd, "\\overline{\\phi}_{- q, \\lambda}", f2.curve->f( 0.5 ) + f2.curve->tang( 0.5 ).perp() + xy( 0.5, -1 ), 90 );
	text t4( fd, "\\gamma_{q_\\gamma, \\lambda_\\gamma}", photon2.curve->f( 0.5 ) + photon2.curve->tang( 0.5 ).perp() + xy( 0, -1 ), 90 );
	
	pg.output();
	return 0;
}
