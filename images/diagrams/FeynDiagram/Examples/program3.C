#include <FeynDiagram/fd.h>

main()
	{
	page pg;
	FeynDiagram fd(pg);

	vertex_dot vtx(fd,0,0);
	line_spring gluon(fd,xy(-5,0),vtx);
	gluon.arrowon.settrue();
	line_plain ghost(fd,vtx,vtx);
	ghost.arcthru(3,0,1);
	ghost.dashon.settrue();
	ghost.dsratio.set(0);
	ghost.thickness.scale(1.8);
	pg.output();
	return 0;
	}
