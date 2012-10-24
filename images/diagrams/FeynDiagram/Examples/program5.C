#include <FeynDiagram/fd.h>

main()
	{
	page pg;
	FeynDiagram fd(pg);

	xy e1(-10,0), e2(10,0);
	blob b(fd,xy(0,0),3,2);
	b.addshading_lines(45);
	b.addshading_lines(-45);
	vertex_dot v1(fd,b.location(180)), v2(fd,b.location(0));
	line_zigzag l1(fd,e1,v1), l2(fd,v2,e2);

	pg.output();
	return 0;
	}
