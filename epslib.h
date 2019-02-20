/*
** epslib.h:
**
** The header file declaring eps format library functions.
**
** Author: Ahmed Semih Özmekik
**
** 
*/

/******************************** WARNING ***********************************
** some functions defined inside functions in this module,      			*
** since this is not allowed in C99 standarts, make sure you don't use that.* 
*****************************************************************************/

#ifndef EPSLIB_H
#define EPSLIB_H

typedef struct Point2D{
	double x,y;
	struct Point2D *nextp; /* next point */
	int jump; /* flag for to jump that point without drawing any line */ 
}Point2D;

typedef struct{
	double red,green,blue; /* RGB */ 
}Color;

typedef struct Text{
	double x,y, /* starting points of txt */
		   size;
	int number; /* 0-999 */
	struct Text *nextt; /* next text */

}Text;

typedef struct{
	double urx, ury, llx, lly, /* upper right(x,y) and lower_left(x,y) */
		   resolution,  
		   thickness; 
	Color color;
	Point2D *point; 
	Text *text; 
}Figure;

typedef struct Tree{
	int number;
	struct Tree *branch1,*branch2;
}Tree;


/* initialization the canvas of the figure with given dimensions, width and height */
Figure * start_figure(double width, double height);

/* some setter functions of figure  */
void set_thickness_resolution(Figure *fig, double resolution, double thickness);
void set_color(Figure * fig, Color c);

/* store in the fig the drawing of the given function in the domain of [start_x,end_x] */
void draw_fx(Figure * fig, double f(double x), double start_x, double end_x);

/* drawing of the polyline which is a set of connected lines in the array poly_line */
void draw_polyline(Figure *fig,Point2D * poly_line, int n);

/* drawing of circle and ellipse, check ellipse's general equation for a and b */
void draw_circle(Figure *fig,Point2D *center,double r);
void draw_ellipse(Figure *fig,Point2D *center,double a,double b);

/* representation of Tree structure with circles and lines */
void draw_binary_tree(Figure *fig,Tree *root,double center_x,double center_y);

/* scale the figure in given dimensions, if smaller figure is desired,		*
 * then scale_x or scale_y must be smaller than 1.0 						*/
void scale_figure(Figure *fig,double scale_x, double scale_y);

/* crop the figure, in other words change the urx,ury,llx,lly */
void resize_figure(Figure *fig,Point2D start_roi, Point2D end_roi);

/* appending fig2 to fig1, fig1's components rule */
void append_figures(Figure *fig1,Figure *fig2);

/* file_name must end with ".eps", for instance "my_file.eps" */ 
void export_eps(Figure * fig, char * file_name);

/* size,  for the size of each edge of equilateral triangle 									 *
 * num_iterations, must be greater or equal to zero, if zero than it means it is just a triangle */
void draw_koch_snowflake(Figure *fig,Point2D *center,double size,int
num_iterations);

/* symmteric and asymmetric drawings of fractal tree,     *
   size, first line size of tree  						  *
   num_iterations, branching number 					  */
void draw_fractal_tree(Figure *fig, double center_x,double center_y,double size,int num_iterations);
void draw_fractal_atree(Figure *fig, double center_x, double center_y,double size, int num_iterations);

#endif 