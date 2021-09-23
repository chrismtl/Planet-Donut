//~ Jean-Claude Rihani 325233
//~ Christian Matala-Tala 327317

#include <cmath>
#include <iostream>
#include "graphic.h"
#include "constantes.h"
 
static const Cairo::RefPtr<Cairo::Context>* ptcr(nullptr);

void set_color(Color color)
{
	switch(color)
	{
		case BLACK:
			(*ptcr)->set_source_rgb(0, 0, 0);
			break;
			
		case GRAY:
			(*ptcr)->set_source_rgb(0.6, 0.7, 0.7);
			break;
			
		case RED:
			(*ptcr)->set_source_rgb(1, 0, 0);
			break;
			
		case GREEN:
			(*ptcr)->set_source_rgb(0, 1, 0);
			break;
			
		case BLUE:
			(*ptcr)->set_source_rgb(0, 0, 1);
			break;
			
		case YELLOW:
			(*ptcr)->set_source_rgb(0.8, 0.8, 0);
			break;
			
		case MAGENTA:
			(*ptcr)->set_source_rgb(1, 0, 1);
			break;	
			
		case CYAN:
			(*ptcr)->set_source_rgb(0, 1, 1);
			break;
		
		case PURPLE:
			(*ptcr)->set_source_rgb(0.6, 0.2, 0.9);
			break;
		
		case WHITE:
			(*ptcr)->set_source_rgb(1, 1, 1);
			break;
	}
}

void graphic_set_context(const Cairo::RefPtr<Cairo::Context>& cr)
{
	static bool init(false);

	if(!init)
	{
		ptcr = &cr;
		init = true;
	}
}

void graphic_draw_frame(double x, double y, double width, double height)
{
	(*ptcr)->save();
	(*ptcr)->set_line_width(2);
	set_color(GRAY);
	
	//draw first line
	(*ptcr)->move_to(x - width/2 , y + height/2);
	(*ptcr)->line_to(x + width/2 , y + height/2);
	//draw second line
	(*ptcr)->move_to(x - width/2 , y + height/2);
	(*ptcr)->line_to(x - width/2 , y - height/2);
	//draw third line
	(*ptcr)->move_to(x + width/2 , y - height/2);
	(*ptcr)->line_to(x - width/2 , y - height/2);
	//draw fourth line
	(*ptcr)->move_to(x + width/2 , y - height/2);
	(*ptcr)->line_to(x + width/2 , y + height/2);
	
	(*ptcr)->stroke();
	
	(*ptcr)->close_path();
	(*ptcr)->restore();
}

void graphic_draw_full_arc(Color color, double xc, double yc, double radius, 
						   double angle)
{
	(*ptcr)->save();
	(*ptcr)->set_line_width(0.1);
	set_color(color);
	
	(*ptcr)->move_to(xc,yc);
	(*ptcr)->line_to(xc,yc+radius);
	(*ptcr)->arc(xc, yc,radius, 0.5*M_PI, 0.5*M_PI+angle);
	
	(*ptcr)->fill_preserve();
	(*ptcr)->stroke();
	(*ptcr)->close_path(); 
	(*ptcr)->restore();
}

void graphic_draw_circle(Color color, double x, double y, double radius)
{
	(*ptcr)->save();
	(*ptcr)->arc(x, y, radius, 0, 2*M_PI);
	(*ptcr)->close_path();
	set_color(color);
	(*ptcr)->stroke();
	
	(*ptcr)->close_path(); 
	(*ptcr)->restore();
}

void graphic_draw_full_circle(Color color, double x, double y, double radius, 
							  double angle)
{
	(*ptcr)->save();
	(*ptcr)->arc(x, y, radius, 0, angle);
	(*ptcr)->close_path();
	set_color(color);
	(*ptcr)->fill_preserve();
	(*ptcr)->stroke();
	
	(*ptcr)->close_path(); 
	(*ptcr)->restore();
}

void graphic_draw_dotted_circle(Color color, double x, double y, double radius)
{
	static double start(0);
	
	(*ptcr)->save();
	set_color(color);
	while  (start < 2*M_PI)
	{
		(*ptcr)->arc(x, y, radius, start, start + M_PI/15);
		(*ptcr)->stroke();
		start += 2*M_PI/15;
	}
	
	start = 0;
	
	(*ptcr)->close_path();
	(*ptcr)->restore();
}

void graphic_draw_cross(Color color, double x, double y)
{
	(*ptcr)->save();
	set_color(color);
	//draw first line
	(*ptcr)->move_to(x + 50*display_rayon_robot, y - 50*display_rayon_robot);
	(*ptcr)->line_to(x - 50*display_rayon_robot, y + 50*display_rayon_robot);
	//draw second line
	(*ptcr)->move_to(x + 50*display_rayon_robot, y + 50*display_rayon_robot );
	(*ptcr)->line_to(x - 50*display_rayon_robot, y - 50*display_rayon_robot);
	(*ptcr)->stroke();
	
	(*ptcr)->close_path();
	(*ptcr)->restore();
}

void graphic_draw_star(Color color, double x, double y)
{
	(*ptcr)->save();
	set_color(color);
	//draw first line
	(*ptcr)->move_to(x + 50*display_rayon_robot*sqrt(2)*3/2,
					 y - 50*display_rayon_robot*sqrt(2)*3/2);
	(*ptcr)->line_to(x - 50*display_rayon_robot*sqrt(2)*3/2,
					 y + 50*display_rayon_robot*sqrt(2)*3/2);
	//draw second line
	(*ptcr)->move_to(x + 50*display_rayon_robot*sqrt(2)*3/2,
					 y + 50*display_rayon_robot*sqrt(2)*3/2);
	(*ptcr)->line_to(x - 50*display_rayon_robot*sqrt(2)*3/2,
					 y - 50*display_rayon_robot*sqrt(2)*3/2);
	//third line
	(*ptcr)->move_to(x, y + 50*display_rayon_robot*3);
	(*ptcr)->line_to(x, y - 50*display_rayon_robot*3);
	//fourth line
	(*ptcr)->move_to(x + 50*display_rayon_robot*3, y);
	(*ptcr)->line_to(x - 50*display_rayon_robot*3, y);
	(*ptcr)->stroke();
	
	(*ptcr)->close_path();
	(*ptcr)->restore();
}

void graphic_draw_plus(Color color, double x, double y)
{
	(*ptcr)->save();
	set_color(color);
	(*ptcr)->move_to(x, y + 50*display_rayon_robot*3);
	(*ptcr)->line_to(x, y - 50*display_rayon_robot*3);

	(*ptcr)->move_to(x + 50*display_rayon_robot*3, y);
	(*ptcr)->line_to(x - 50*display_rayon_robot*3, y);
	(*ptcr)->stroke();
	
	(*ptcr)->close_path();
	(*ptcr)->restore();
}

void graphic_draw_wave(Color color, double x, double y)
{
	(*ptcr)->save();
	set_color(color);
	
	(*ptcr)->arc(x, y, 50*display_rayon_robot - 7, M_PI/6, 3*M_PI/4);
	(*ptcr)->stroke();
	(*ptcr)->arc(x, y, 50*display_rayon_robot, M_PI/4, 3*M_PI/4);
	(*ptcr)->stroke();
	(*ptcr)->arc(x, y, 50*display_rayon_robot + 7, M_PI/4, 3*M_PI/4);
	(*ptcr)->stroke();
	
	(*ptcr)->close_path();
	(*ptcr)->restore();
}

void graphic_draw_valid(Color color, double x, double y)
{
	(*ptcr)->save();
	set_color(color);
	//draw first line
	(*ptcr)->move_to(x - 0.75*50*display_rayon_robot , y + 50*display_rayon_robot);
	(*ptcr)->line_to(x, y);
	//draw second line
	(*ptcr)->move_to(x, y);
	(*ptcr)->line_to(x + 50*display_rayon_robot , y + 2*50*display_rayon_robot);
	(*ptcr)->stroke();
	
	(*ptcr)->close_path();
	(*ptcr)->restore();
}

void graphic_draw_square(Color color, double x, double y)
{
	(*ptcr)->save();
	set_color(color);
	
	//draw first line
	(*ptcr)->move_to(x - 50*display_rayon_robot * 6.0/4,
					 y + 50*display_rayon_robot * 6.0/4);
	(*ptcr)->line_to(x + 50*display_rayon_robot * 6.0/4,
					 y + 50*display_rayon_robot * 6.0/4);
	(*ptcr)->line_to(x + 50*display_rayon_robot * 6.0/4,
					 y - 50*display_rayon_robot * 6.0/4);
	(*ptcr)->line_to(x - 50*display_rayon_robot * 6.0/4,
					 y - 50*display_rayon_robot * 6.0/4);
	(*ptcr)->line_to(x - 50*display_rayon_robot * 6.0/4,
					 y + 50*display_rayon_robot * 6.0/4);
	
	(*ptcr)->stroke();
	
	(*ptcr)->close_path();
	(*ptcr)->restore();
}

void graphic_draw_full_square(Color color, double x, double y)
{
	(*ptcr)->save();
	set_color(BLACK);
	
	//draw first line
	(*ptcr)->move_to(x - 50*display_rayon_robot * 6.0/4,
					 y + 50*display_rayon_robot * 6.0/4);
	(*ptcr)->line_to(x + 50*display_rayon_robot * 6.0/4,
					 y + 50*display_rayon_robot * 6.0/4);
	(*ptcr)->line_to(x + 50*display_rayon_robot * 6.0/4,
					 y - 50*display_rayon_robot * 6.0/4);
	(*ptcr)->line_to(x - 50*display_rayon_robot * 6.0/4,
					 y - 50*display_rayon_robot * 6.0/4);
	(*ptcr)->line_to(x - 50*display_rayon_robot * 6.0/4,
					 y + 50*display_rayon_robot * 6.0/4);
	(*ptcr)->fill_preserve();
	
	(*ptcr)->stroke();
	
	(*ptcr)->close_path();
	(*ptcr)->restore();
	
	graphic_draw_square(color, x, y);
}

void graphic_draw_connection(double x, double y, double x_, double y_)
{
	(*ptcr)->save();
	set_color(PURPLE);
	
	//draw first line
	(*ptcr)->move_to(x, y);
	(*ptcr)->line_to(x + x_, y + y_);
	
	(*ptcr)->stroke();
	
	(*ptcr)->close_path();
	(*ptcr)->restore();
}
