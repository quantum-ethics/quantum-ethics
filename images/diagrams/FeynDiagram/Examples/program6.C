#include <FeynDiagram/fd.h>

main()
	{
	page pg;
	FeynDiagram fd(pg);

	xy pos1(-5,2);
	text t1(fd,"Simple Text",pos1);

	text t2(fd,"Large Text",xy(-5,0));
	t2.fontsize.scale(1.5);


	vertex_dot v(fd,-5,-2);
	text t3(fd,"F^{\\mu\\nu}_{a_1}",v + xy(0,-.5),0.5,1);

	text t4(fd,"Rotated Helvetica!",xy(3,2),-40);
	t4.fontname.set("Helvetica");

	text t5(fd,"Change \\font{Courier}{font}",xy(-5,-6));

	text t6(fd,"\\tildebar{c} \\overrightarrow{p}",xy(3,-5));

	pg.output();
	return 0;
	}
