#include <FeynDiagram/fd.h>

main()
	{
	page pg;
	FeynDiagram fd(pg);

	xy e1(-10,0), e2(10,0), arcpt(0,4);
	vertex_circlecross v1(fd,-4,0), v2(fd,4,0);
	line_plain f1(fd,e1,v1), f2(fd,v1,v2), f3(fd,v2,e2);
	line_wiggle photon1(fd,v1,v2), photon2(fd,v1,v2);

	photon1.arcthru(arcpt);
	photon1.dashon.settrue();
	photon2.arcthru(0,-2);
	photon2.width.scale(0.7);
	pg.output();
	return 0;
	}
