#include <FeynDiagram/fd.h>

main()
	{
	page pg;
	FeynDiagram fd(pg);

	xy e1(-10,5), e2(-10,-5), e3(10,5), e4(10,-5);
	vertex_dot v1(fd,-5,0), v2(fd,5,0);

	line_plain f1(fd,e1,v1);
	line_plain f2(fd,v1,e2);
	line_wiggle photon(fd,v1,v2);
	line_plain f3(fd,v2,e3), f4(fd,e4,v2);

	pg.output();
	return 0;
	}
