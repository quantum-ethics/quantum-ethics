#include <FeynDiagram/fd.h>

/* Operator product expansion diagrams */

main()
	{
	page pg;

	xy top(0,6.5), ll, lr, ml, mr;
	ll = top + polar(15,240);
	lr = top + polar(15,300);
	ml = .7*ll + .3*top;
	mr = .7*lr + .3*top;

	   /* The left diagram */
	FeynDiagram fdA(pg,1,4.5,5.5);
	fdA.line_plain_arrowon.setfalse();

	vertex_circlecross topA(fdA,top);
	vertex_dot leftA(fdA,ml), rightA(fdA,mr);
	line_plain f1A(fdA,ll,top), f2A(fdA,top,lr);
	line_wiggle wA(fdA,ml,mr);
	wA.arrowon.settrue();


	   /* The right diagram */
	FeynDiagram fdB(pg,4.5,8,5.5);
	fdB.line_plain_arrowon.setfalse();

	vertex_circlecross topB(fdB,top);
	vertex_dot leftB(fdB,ml);
	line_plain f1B(fdB,ll,top), f2B(fdB,top,lr);
	line_wiggle wB(fdB,ml,top);
	wB.arrowon.settrue();
	wB.arcthru(-1.2,1);

	  /* A separate diagram for the "+" */
	FeynDiagram fdC(pg,1.9,6.6,5.5);
	text plus(fdC,"+",xy(0,0));
	plus.fontsize.scale(1.5);

	pg.output();
	return 0;
	}

