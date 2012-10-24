#include <FeynDiagram/fd.h>

void adjust(FeynDiagram &fd, double scalefactor)
	{
	fd.line_arrowon.settrue();
	fd.line_arrowposition.set(.25);
	fd.line_wiggle_arrowposition.set(.5);
	fd.line_wiggle_width.set(.7*scalefactor);
	}

main()
	{
	page pg;
	pg.adjustparams = adjust;
	xy pt1(-7,4), pt2(0,0), pt3(-7,-4), pt4(8,0);
	FeynDiagram fdA(pg,1,4,5.5);
	line_plain L1A(fdA,pt1,pt2), L2A(fdA,pt2,pt3);
	line_zigzag L3A(fdA,pt2,pt4);
	
	FeynDiagram fdB(pg,4.5,7.5,5.5);
	line_plain L1B(fdB,pt1,pt2), L2B(fdB,pt2,pt3);
	line_wiggle L3B(fdB,pt2,pt4);

	pg.output();
	return 0;
	}
