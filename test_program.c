#include "epslib.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>


Figure create_fig(void);

void insert_branch(Tree *root,int branch_no,int number)
{
	Tree *branch=(Tree*)malloc(sizeof(Tree));
	branch->number=number;

	if(branch_no==1)	root->branch1=branch;
	else  				root->branch2=branch;
}

int main()
{

	int size=9;

	Figure *fig_list=(Figure*)malloc(size*sizeof(Figure));

	for(int i=0;i<size;i++)
	{
		fig_list[i]=create_fig();
	}

	
	/* draw fx */
	draw_fx(&fig_list[0],sin,0.0,10.0); 
	scale_figure(&fig_list[0],30.0,30.0);
	

	/* poly line */
	Point2D *arr_poly= (Point2D*)malloc(20*sizeof(Point2D));
	for(int i=0;i<size;i++){
		arr_poly[i].x=i;
		arr_poly[i].y=i*i;
	}
	draw_polyline(&fig_list[1],arr_poly,size);


	/* circle */
	Point2D *center=(Point2D*)malloc(sizeof(Point2D));
	center->x=0.0; 			center->y=0.0;
	draw_circle(&fig_list[2],center,50.0);

	/*ellipse */
	draw_ellipse(&fig_list[3],center,80.0,40.0);

	/* binary tree */
	Tree *root = (Tree*)malloc(sizeof(Tree));
	insert_branch(root,0,10);
	insert_branch(root,1,20);
	insert_branch(root->branch1,0,30);
	insert_branch(root->branch1,1,40);
	insert_branch(root->branch2,0,50);
	insert_branch(root->branch2,1,60);
	Point2D start,end;
	start.x=-100;
	start.y=-100;
	end.x=100;
	end.y=50;

	resize_figure(&fig_list[4],start,end);
	draw_binary_tree(&fig_list[4],root,center->x,center->y);

	/* append */
	draw_circle(&fig_list[5],center,50.0);
	append_figures(&fig_list[5],&fig_list[3]);


	/* koch snow flake */
	draw_koch_snowflake(&fig_list[6],center,100,6);

	/* fractal tree */

	Figure fig=create_fig();

	double center_x=0.0,center_y=-250.0;

	double size1=70.0;

	int iteration1=10;

	draw_fractal_tree(&fig,center_x,center_y,size1,iteration1);

	Figure fig2=create_fig();

	draw_fractal_atree(&fig2,center_x,center_y,1.5*size1,iteration1);
	

	




	printf("\nStarting to test...\n");

	export_eps(&fig_list[0],"draw_fx.eps");
	export_eps(&fig_list[1],"polyline.eps");
	export_eps(&fig_list[2],"circle.eps");
	export_eps(&fig_list[3],"ellipse.eps");
	export_eps(&fig_list[4],"binary_tree.eps");
	export_eps(&fig_list[5],"append.eps");
	export_eps(&fig_list[6],"koch_snowflake.eps");
	export_eps(&fig,"fractal_tree.eps");
	export_eps(&fig2,"fractal_atree.eps");



	printf("\nDone! You can check the EPS files..");
	

	return 0;
}

Figure create_fig(void)
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
