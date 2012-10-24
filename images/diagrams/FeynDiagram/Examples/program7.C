#include <FeynDiagram/fd.h>

main()
	{
	page pg;
	FeynDiagram fd(pg);

	vertex_dot v1(fd,-3,4), v2(fd,polar(5,-30));
	line_wiggle L(fd,v1,v2);

	xy midpt, arcpt;
	midpt = 0.5 * (v1 + v2);
	arcpt = midpt + 4.0 * (v2-v1).perp();
	L.arcthru(arcpt);

	xy refpt, tanvect;
	refpt = L.curve->f(0.6);
	tanvect = L.curve->tang(0.6);
	refpt += 0.8 * tanvect.perp();
	text txt(fd,"k_1",refpt,tanvect.angle());

	pg.output();
	return 0;
	}
