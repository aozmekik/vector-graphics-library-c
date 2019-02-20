#include "epslib.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define TRUE 1
#define FALSE 0

#define PI 3.141592


/* for manage to point lists */
void insert_point(Point2D *headp,double x,double y,int jump); /* add new point to given list */
void merge(Point2D *p1,Point2D *p2); /* link the lists without jump */
void jump_and_merge(Point2D *p1,Point2D *p2); /* link the list with jump */

/* duplicates fig2's component to fig1, useful for avoid initialize temporary figures */ 
void duplicate_figure(Figure *fig1,Figure *fig2);

/* text list insertation */
void insert_text(Text *text,int number,double center_x, double center_y);

/* helper functions of draw_binary_tree */
Point2D* draw_node(Figure *fig2,Tree *branch,Point2D *point,double center_x,double center_y,int radius,double size,int node_number);
int find_node_number(Tree *branch); /* find branch number of given node */

/* helper functions of draw_koch_snowflake */
void koch_curve(Point2D *p1,Point2D *p2,double size,int num_iterations); /* draws only one edge of koch snowflake */
Point2D* find_third_vertex(Point2D *p1,Point2D *p2); /* find the unknown third vertex of equilateral triangle */

/* helper function of draw_fractal_tree and  draw_afractal_tree ,		  *
   recursively defined and finds all the points where lines start and end */
void frac_branch_points(Point2D *points,Point2D p,double angle,double size, int num_iterations,int symmetric);

Figure * start_figure(double width, double height)
{
	Figure *fig=(Figure*)malloc(sizeof(Figure)); /* defined new figure */

	fig->urx=width/2.0;
	fig->ury=height/2.0;
	fig->llx=-width/2.0; 
	fig->lly=-height/2.0;
	fig->text=NULL; /* defined as null in default */

	return fig;
}
void set_thickness_resolution(Figure *fig, double resolution, double thickness)
{
	fig->resolution=resolution;
	fig->thickness=thickness;
}
void set_color(Figure * fig, Color c)
{
	fig->color.red=c.red;
	fig->color.green=c.green;
	fig->color.blue=c.blue;
}

void draw_fx(Figure * fig, double f(double x), double start_x, double end_x)
{
	/* constructing a point list */ 

	Point2D *headp=(Point2D*)malloc(sizeof(Point2D));  /* head of our pointer list */

	headp->x=start_x;
	headp->y=f(start_x);
	headp->jump=FALSE;

	for(double x=start_x+(1.0/fig->resolution); /* continues  */
			    					  x<=end_x;	
			    					  x+=(1.0/fig->resolution))
	{
		insert_point(headp,x,f(x),FALSE);
	}
	insert_point(headp,end_x,f(end_x),FALSE); /* prevent to miss end_x */  

	fig->point=headp; /* make figure has the list */
}

void insert_point(Point2D *headp,double x,double y,int jump)
{
	Point2D *lastp,*newp;

	if(headp==NULL)
		return;

	lastp = headp; 
	while(lastp->nextp!=NULL)
		lastp=lastp->nextp; 

	
	/* insert */
	newp = (Point2D*)malloc(sizeof(Point2D));
	newp->x=x;
	newp->y=y;
	newp->jump=jump;
	newp->nextp=NULL; /* making newp, the last item */

	lastp->nextp=newp;
}


void jump_and_merge(Point2D *p1,Point2D *p2)
{
	Point2D *tailp=p1;

	insert_point(p1,p2->x,p2->y,TRUE); /* jump to p2's starting point */

	while(tailp->nextp!=NULL)
		tailp=tailp->nextp;
	tailp->nextp=p2->nextp; /* linked */
}

/* link p2 to p1's tail */
void merge(Point2D *p1,Point2D *p2)
{
	Point2D *tailp=p1;

	while(tailp->nextp!=NULL)
		tailp=tailp->nextp;
	tailp->nextp=p2; /* linked */
}


void export_eps(Figure * fig, char * file_name)
{
	Point2D *tmp; /* temporary point */
	FILE *fp;

	fp=fopen(file_name,"wt"); /* both read and write */

	fprintf(fp,"%%!PS-Adobe-3.0 EPSF-3.0");
	fprintf(fp,"\n%%%%BoundingBox: %d %d %d %d",(int)fig->llx,(int)fig->lly,(int)fig->urx,(int)fig->ury);
	fprintf(fp,"\n%d setlinewidth",(int)fig->thickness);
	fprintf(fp,"\n%.1f %.1f %.1f setrgbcolor",fig->color.red,fig->color.green,fig->color.blue);

	
	if(fig->text!=NULL)
	{
		fprintf(fp,"\n/Times-Roman findfont\n5 scalefont\nsetfont"); /* configurations */
		while(fig->text!=NULL)
		{
			fprintf(fp,"\n%d %d moveto",(int)fig->text->x,(int)fig->text->y);
			fprintf(fp,"\n(%d) show",fig->text->number);
			fig->text=fig->text->nextt;
		}
	}

	fprintf(fp,"\n%.1f %.1f moveto",fig->point->x,fig->point->y);

	for(tmp=fig->point->nextp;
					tmp!=NULL;
				    	      tmp=tmp->nextp)
	{
		
		fprintf(fp,"\n%.1f %.1f ",tmp->x,tmp->y);
		if(tmp->jump)	fprintf(fp,"moveto");
		else            fprintf(fp,"lineto");
		

	}
	
	fprintf(fp,"\nstroke\n");
	fclose(fp);

}

void draw_polyline(Figure *fig,Point2D * poly_line, int n)
{
	Point2D *point=(Point2D*)malloc(sizeof(Point2D)); /* head */

	/* first element */
	point->x=poly_line[0].x;
	point->y=poly_line[0].y;
	point->jump=FALSE;

	for(int i=1;i<n;i++)
		merge(point,&poly_line[i]); /* pointing that items to list */

	fig->point=point;

}

void draw_circle(Figure *fig,Point2D *center,double r)
{
	double start_x,end_x;

	Figure *fig2=(Figure*)malloc(sizeof(Figure)); 
	Point2D *cursor; /* shifting circle */  

	double pos_circle_func(double x){return   sqrt((r*r)-(x*x));} /* positive circle function */
	double neg_circle_func(double x){return  -sqrt((r*r)-(x*x));} /* negative circle function */
	
	/* start and end points of half circles*/
	start_x=-r; end_x=r;

	duplicate_figure(fig2,fig);

	/* y=sqrt(r^2-x^2) */
	draw_fx(fig,pos_circle_func,start_x,end_x);

	/* y=-sqrt(r^2-x^2) */
	draw_fx(fig2,neg_circle_func,start_x,end_x); 

	append_figures(fig,fig2);

	/* shift the points due to center */
	if(center->x!=0 || center->y!=0)
	{
		cursor=fig->point;
		while(cursor!=NULL)
		{
			cursor->x+=center->x;
			cursor->y+=center->y;
			cursor=cursor->nextp;
		}
	}
	
}

void draw_ellipse(Figure *fig,Point2D *center,double a,double b)
{
	double start_x,end_x;

	Figure *fig2=(Figure*)malloc(sizeof(Figure));
	Point2D *cursor;  

	double pos_ellipse_func(double x){return   sqrt((1-((x*x)/(a*a)))*(b*b));} 
	double neg_ellipse_func(double x){return  -sqrt((1-((x*x)/(a*a)))*(b*b));}

	start_x=-a; end_x=a;

	duplicate_figure(fig2,fig); /* copied fig's component(res,color so on.) to fig2*/

	draw_fx(fig,pos_ellipse_func,start_x,end_x);
	draw_fx(fig2,neg_ellipse_func,start_x,end_x);
	append_figures(fig,fig2); 

	/* shift the points due to center */
	if(center->x!=0 || center->y!=0)
	{
		cursor=fig->point;
		while(cursor!=NULL)
		{
			cursor->x+=center->x;
			cursor->y+=center->y;
			cursor=cursor->nextp;
		}
	}

}

void draw_binary_tree(Figure *fig,Tree *root,double center_x,double center_y)
{	
	double	radius=10.0; /* general size of representation */
	int node_number;

	fig->text=(Text*)malloc(sizeof(Text));
	fig->text->number=root->number;
	fig->text->x=center_x-radius/3.0;
	fig->text->y=center_y;

	Point2D *point = (Point2D*)malloc(sizeof(Point2D));
	point->x=center_x;
	point->y=center_y;

	node_number=find_node_number(root)/2+1;

	fig->point=draw_node(fig,root,point,center_x,center_y,radius,1.0,node_number);

}

Point2D* draw_node(Figure *fig,Tree *branch,Point2D *point,double center_x,double center_y,
	int radius,double x,int node_number)
{

	Figure *fig2 = (Figure*)malloc(sizeof(Figure)); /* temp figure for to use of draw_circle */


	if(branch==NULL)
		return point;

	/* head point */
	point->x=center_x;
	point->y=center_y;


	/* main figure of node */
	duplicate_figure(fig2,fig);
	draw_circle(fig2,point,radius);
	jump_and_merge(point,fig2->point); 

	insert_text(fig->text,branch->number,center_x-radius/3.0,center_y);

	/* left arm of node */
	if(branch->branch1!=NULL)
	{
		insert_point(point,center_x,center_y-radius,TRUE);
		insert_point(point,center_x-(node_number)*x*radius,center_y-(node_number-1)*radius,FALSE);
	}
	
	/* right arm of node */
	if(branch->branch2!=NULL)
	{
		insert_point(point,center_x,center_y-radius,TRUE);
		insert_point(point,center_x+(node_number)*x*radius,center_y-(node_number-1)*radius,FALSE);
	}
		

	/* two branches of each branch */
	draw_node(fig,branch->branch1,point,center_x-radius*x*(node_number),center_y-radius*(node_number),radius,x/2.0,node_number);
	draw_node(fig,branch->branch2,point,center_x+radius*x*(node_number),center_y-radius*(node_number),radius,x/2.0,node_number);

	return point;

}

int find_node_number(Tree *branch)
{
	int number=0;

	if(branch==NULL)
		return number;

	number++;
	number+=find_node_number(branch->branch1);
	number+=find_node_number(branch->branch2);
	return number; 
}

void insert_text(Text *text,int number,double center_x, double center_y)
{
	Text *t=(Text*)malloc(sizeof(Text)); /* new text */

	t->number=number;
	t->x=center_x;
	t->y=center_y;
	t->nextt=NULL;

	while(text->nextt!=NULL)
		text=text->nextt;
	text->nextt=t; /* insert to text list */
	

}

void scale_figure(Figure *fig,double scale_x, double scale_y)
{
	Point2D *cursor=fig->point;

	double max_x=cursor->x,max_y=cursor->y,min_x=cursor->x,min_y=cursor->y, /* extremum points*/
		   center_x,center_y, /* center of the figure */
		   dist_x,dist_y; /* distances to origin */
	
	while(cursor!=NULL)
	{
		if(cursor->x > max_x)	max_x=cursor->x;
		if(cursor->y > max_y)	max_y=cursor->y;
		if(cursor->x < min_x)	min_x=cursor->x;
		if(cursor->y < min_y)	min_y=cursor->y;
		cursor=cursor->nextp;
	}

	center_x=(max_x+min_x)/2.0;
	center_y=(max_y+min_y)/2.0;


	cursor=fig->point;
	while(cursor!=NULL)
	{
		cursor->x=((cursor->x-center_x)*scale_x)+center_x;
		cursor->y=((cursor->y-center_y)*scale_y)+center_y;
		cursor=cursor->nextp;
	}
}

void resize_figure(Figure *fig,Point2D start_roi, Point2D end_roi)
{
	fig->llx=start_roi.x;
	fig->lly=start_roi.y;

	fig->urx=end_roi.x;
	fig->ury=end_roi.y;
}

void append_figures(Figure *fig1, Figure *fig2)
{
	/* since there are figures, therefore two point list, there should not be any lines	   *
	 * between the head of this points, so there will be jump there.                       */

	jump_and_merge(fig1->point,fig2->point);
}

void duplicate_figure(Figure *fig1,Figure *fig2)
{
	fig1->llx=fig2->llx;
	fig1->lly=fig2->lly;
	fig1->urx=fig2->urx;
	fig1->ury=fig2->ury;
	fig1->resolution=fig2->resolution;
	fig1->thickness=fig2->thickness;
	fig1->color.red=fig2->color.red;
	fig1->color.green=fig2->color.green;
	fig1->color.blue=fig2->color.blue;
}

void draw_koch_snowflake(Figure *fig,Point2D *center,double size,int num_iterations)
{
	double h; /* height of the triangle */ 

	/* three corners or head points for three edges */ 
	Point2D *p1,*p2,*p3,*p4; /* p4 for avoid infinite pointing */
	p1=(Point2D*)malloc(sizeof(Point2D));
	p2=(Point2D*)malloc(sizeof(Point2D));
	p3=(Point2D*)malloc(sizeof(Point2D));
	p4=(Point2D*)malloc(sizeof(Point2D));


	h=(size/2.0)*sqrt(3.0); 

	/* p1, left corner*/
	p1->x=center->x-(size/2.0);
	p1->y=center->y-(h/3.0);
	p1->jump=FALSE;

	/* p2, above corner */
	p2->x=center->x;
	p2->y=center->y+(2.0*h/3.0);
	p2->jump=FALSE;

	/* p3, right corner */
	p3->x=center->x+(size/2.0);
	p3->y=center->y-(h/3.0);
	p3->jump=FALSE;

	/* p4 is the same point with p1, except stops pointing */ 
	p4->x=p1->x;
	p4->y=p1->y;
	p4->jump=FALSE;

	koch_curve(p1,p2,size,num_iterations);
	koch_curve(p2,p3,size,num_iterations);
	koch_curve(p3,p4,size,num_iterations);



	fig->point=p1;	

}

void koch_curve(Point2D *p1,Point2D *p2,double size,int num_iterations)
{
	Point2D *newp1,*newp2,*newp3; /* probable three points */

	if(num_iterations==0) /* only link the two points */
	{
		p2->nextp=NULL;
		p1->nextp=p2;

	}
	else
	{
		/* allocation */
		newp1=(Point2D*)malloc(sizeof(Point2D));
		newp2=(Point2D*)malloc(sizeof(Point2D));
		newp3=(Point2D*)malloc(sizeof(Point2D));

		/* finding corners */

		newp1->x=(p1->x*2.0+p2->x)/3.0;
		newp1->y=(p1->y*2.0+p2->y)/3.0;

		newp3->x=(p1->x+(p2->x)*2.0)/3.0;
		newp3->y=(p1->y+(p2->y)*2.0)/3.0;

		newp2=find_third_vertex(p1,p2);

		/* connect all cornes in order */

		p1->nextp=newp1;

		koch_curve(newp1,newp2,size/3.0,num_iterations-1);
		merge(newp1,newp2);

		koch_curve(newp2,newp3,size/3.0,num_iterations-1);
		merge(newp2,newp3);
	
		newp3->nextp=p2;

	}

}

Point2D* find_third_vertex(Point2D *p1,Point2D *p2)
{
	
	Point2D *u,*v, /* two orthogonal vectors of same length */
			*p; /* third vertex */

	u=(Point2D*)malloc(sizeof(Point2D));
	v=(Point2D*)malloc(sizeof(Point2D));
	p=(Point2D*)malloc(sizeof(Point2D));

	u->x=p2->x-p1->x;	u->y=p2->y-p1->y;
	v->x=p1->y-p2->y;	v->y=p2->x-p1->x;

	p->x=p1->x+(0.5*u->x)+(sqrt(3.0)/6.0)*(v->x);
	p->y=p1->y+(0.5*u->y)+(sqrt(3.0)/6.0)*(v->y);

	return p;

}

void draw_fractal_tree(Figure *fig, double center_x,double center_y,double size,int num_iterations)
{
	double angle=60.0;
	Point2D *branch1,*branch2, /*  two big branches of tree */
			*root,*body;     /* starting coordinates */


	/* ALLOCATIONS */
	branch1=(Point2D*)malloc(sizeof(Point2D));
	branch2=(Point2D*)malloc(sizeof(Point2D));
	root=(Point2D*)malloc(sizeof(Point2D));
	body=(Point2D*)malloc(sizeof(Point2D));

	/* root point of the tree */
	root->x=center_x;		root->y=center_y;

	/* main brunch of the tree */
	body->x=root->x;		body->y=root->y+size; 	

	root->nextp=body; /* linked the brunch to root */

	/* points where branching begins */
	branch1->x=body->x; 	branch1->y=body->y;
	branch2->x=body->x; 	branch2->y=body->y;

	frac_branch_points(branch1,*body,angle,size,num_iterations,TRUE);
	frac_branch_points(branch2,*body,angle+60,size,num_iterations,TRUE);

	jump_and_merge(branch1,branch2);

	body->nextp=branch1;

	fig->point=root;
}

void draw_fractal_atree(Figure *fig,double center_x, double center_y, double size, int num_iterations)
{
	double angle=90.0;
	Point2D *branch1, /* only one brunch since it will be asymmetric */
			*root;     /* starting coordinates */

	/* ALLOCATIONS */
	branch1=(Point2D*)malloc(sizeof(Point2D));
	root=(Point2D*)malloc(sizeof(Point2D));

	root->x=center_x;		root->y=center_y; 	

	branch1->x=root->x; 	branch1->y=root->y;
	frac_branch_points(branch1,*root,angle,size,num_iterations,FALSE);

	root->nextp=branch1;

	fig->point=root;

}

void frac_branch_points(Point2D *point,Point2D p,double angle,double size, int num_iterations,int symmetric)
{
	Point2D *p2; /* starting points of two branchs */
	p2=(Point2D*)malloc(sizeof(Point2D));

	double angle1,angle2; /* anglees between brunches */

	if(symmetric)
	{
		angle1=30.0; angle2=30.0;
	}
	else
	{
		angle1=27.0; angle2=57.0;
	}

	/* updating coordinates */
	p2->x=p.x + size*cos(angle*PI/180.0);
	p2->y=p.y + size*sin(angle*PI/180.0);

	insert_point(point,p2->x,p2->y,FALSE); /* insert a new point with that new coordinates */

	if(num_iterations==0)
		return;

	frac_branch_points(point,*p2,angle+angle1,size*0.75,num_iterations-1,symmetric); /* left branch */
	jump_and_merge(point,p2); /* do not draw line between unrelated branches */
	frac_branch_points(point,*p2,angle-angle2,size*0.75,num_iterations-1,symmetric); /* right branch */

}