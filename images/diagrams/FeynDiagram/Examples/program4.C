#include <FeynDiagram/fd.h>

main()
	{
	page pg;
	FeynDiagram fd(pg);

	line_spring gluon(fd,xy(-10,0),xy(-2,0));
	gluon.arcthru(-6,3);
	line_spring gluonflip(fd,xy(2,0),xy(10,0));
	gluonflip.arcthru(6,3);
	gluonflip.width.scale(-1);
	line_plain fermion(fd,xy(-6,0),xy(-2,3));
	fermion.ontop(1);
	pg.output();
	return 0;
	}
