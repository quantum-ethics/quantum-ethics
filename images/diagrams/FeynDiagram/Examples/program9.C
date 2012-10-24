#include <FeynDiagram/fd.h>

main()
	{
	page pg;
	FeynDiagram fd(pg);
	fd.line_plain_arrowon.setfalse();
	fd.line_plain_thickness.scale(3.0);
	fd.vertex_radius_small.set(2*fd.vertex_radius_small.get());

	xy e1(-10,5), e2(-10,-5), e3(10,5), e4(10,-5);
	vertex_dot v1(fd,-6,0), v2(fd,6,0), v3(fd,0,6), v4(fd,0,-6);
	line_plain L1(fd,e1,v1), L2(fd,e2,v1), L3(fd,e3,v2), L4(fd,e4,v2);
	line_plain L5(fd,v1,v2), L6(fd,v2,v1);
	L5.arcthru(v3);
	L6.arcthru(v4);
	line_plain L7(fd,v3,v4), L8(fd,v4,v3);
	L7.arcthru(-2,0);
	L8.arcthru(2,0);

	pg.output();
	return 0;
	}
