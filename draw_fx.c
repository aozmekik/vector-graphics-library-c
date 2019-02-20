#include "epslib.h"
#include <math.h>
#include <stdio.h>

Figure *create_fig(void);

int main()
{
	Figure *fig1,*fig2,*fig3;
	fig1=create_fig();
	fig2=create_fig();
	fig3=create_fig();


	draw_fractal_tree(fig1,0,0,10,15);

	export_eps(fig1,"draw_fx_01.eps");
	export_eps(fig2,"draw_fx_02.eps");
	export_eps(fig3,"draw_fx_03.eps");
	return 0;

}


Figure* create_fig(void)
{
	Figure *fig=start_figure(500,500);
	set_thickness_resolution(fig,20,0.8);
	Color c;
	c.red=0;
	c.green=0;
	c.blue=0;
	set_color(fig,c);

	return *fig;
}