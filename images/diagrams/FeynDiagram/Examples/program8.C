#include <FeynDiagram/fd.h>

main()
	{
	page pg;
	xy pt[6];

	pt[0] = xy(-10,5);
	pt[1] = xy(-10,-5);
	pt[2] = -pt[1];
	pt[3] = -pt[0];

	pt[4] = xy(-5,0);
	pt[5] = -pt[4];

	FeynDiagram fdA(pg,1,4,4);
	line_plain fA1(fdA,pt[0],pt[4]), fA2(fdA,pt[4],pt[1]);
	line_plain fA3(fdA,pt[5],pt[2]), fA4(fdA,pt[3],pt[5]);
	line_wiggle pA(fdA,pt[4],pt[5]);

	int i;
	for (i = 0; i < 6; ++i)
		{
		pt[i].rotate(90);
		pt[i] = xy(pt[i].x(),-pt[i].y());
		}

	FeynDiagram fdB(pg,4.5,6.2,4);
	line_plain fB1(fdB,pt[0],pt[4]), fB2(fdB,pt[4],pt[1]);
	line_plain fB3(fdB,pt[5],pt[2]), fB4(fdB,pt[3],pt[5]);
	line_wiggle pB(fdB,pt[4],pt[5]);

	pg.output();
	return 0;
	}
