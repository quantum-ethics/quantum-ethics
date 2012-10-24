#include <FeynDiagram/fd.h>

main() {
	page pg;
	
	const double xmax = 5;
	
	FeynDiagram fd( pg, 4.25 - 32.5 / xmax, 4.25 + 32.5 / xmax, 5.5 );
	
	//fd.gridon.settrue();
	
	xy e1( -5, -7 ), e2( 5, -7 );
	vertex_dot v1( fd, 0, -2 );
	vertex_circlecross v2( fd, 0, 5 );
	
	line_plain f1( fd, e1, v1 ), f2( fd, v1, e2 );
	line_wiggle photon( fd, v2, v1 );
	
	text t1( fd, "e_{q_i, \\lambda_i}", f1.curve->f( 0.5 ) + f1.curve->tang( 0.5 ).perp() + xy( -2, 0 ) );
	text t2( fd, "e_{q_f, \\lambda_f}", f2.curve->f( 0.5 ) + f2.curve->tang( 0.5 ).perp() );
	text t3( fd, "\\widetilde{V}^{cl}_{q_f - q_i}", photon.curve->f( 0.5 ) + photon.curve->tang( 0.5 ).perp() + xy( 0, -0.25 ) );
	text t4( fd, "Ze", v2 + xy( -0.625, 1.25 ) );
	
	pg.output();
	return 0;
}
