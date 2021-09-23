//~ Jean-Claude Rihani 325233
//~ Christian Matala-Tala 327317

#include <iostream>
#include <cmath>
#include "geomod.h"
#include "graphic.h"
#include "constantes.h"

using namespace std;

namespace
{
	double max_(dim_max);
	double epsilon_zero(max_ * pow(10, -10));
}

////////////////////////// Point

Point::Point (double x, double y, bool to_norm)
	: x(x), y(y)
{
	if(to_norm)
	{
		norm(*this);
	}
}

double Point::getX() const
{	
	return x;
}

double Point::getY() const
{
	return y;
}

void Point::set(double new_x, double new_y)
{
	x = new_x;
	y = new_y;
}

void Point::set(Point new_point)
{
	x = new_point.x;
	y = new_point.y;
}

void Point::setX(double x)
{
	this->x = x;
}

void Point::setY(double y)
{
	this->y = y;
}

Point operator+(Point p1, Point p2)
{
	return Point (p1.getX() + p2.getX(), p1.getY() + p2.getY());
}

////////////////////////// Vector

Vect::Vect(const Point& start, const Point& end)
{
	x = end.getX() - start.getX();
	y = end.getY() - start.getY();
	norm = sqrt((pow(x, 2) + pow(y, 2)));
}

double Vect::getX() const
{
	return x;
}

double Vect::getY() const
{
	return y;
}

double Vect::getNorm() const
{
	return norm;
}

////////////////////////// Circle

Circle::Circle(const Point& center, double radius)
	: center(center)
{	
	if(radius > 0)
		this->radius = radius;
}

Point Circle::getCenter() const
{
	return center;
}

double Circle::getRadius() const
{
	return radius;
}

////////////////////////// Usuel functions

void set_max(double init_max)
{
	if(init_max > 0)
		max_ = init_max;
	
	epsilon_zero = max_ * pow(10, -10);
}

bool equal_zero(double val)
{
	return(abs(val) < epsilon_zero);
}

double get_max()
{
	return dim_max;
}

double get_epsilon_zero()
{
	return epsilon_zero;
}

double norm(double v)
{
	while(abs(v) > dim_max)
	{
		if(v < 0)
			v += 2 * dim_max;
		else
			v -= 2 * dim_max;
	}
	return v;
}

void norm(Point& point)
{
	point.setX(norm(point.getX()));
	point.setY(norm(point.getY()));
}

////////////////////////// Minimal norm vector

Vect vect_norm(const Point& start, const Point& end)
{
	Point end_(end.getX() - 2 * dim_max, end.getY() -2 * dim_max, false);
	Vect vect_min(start, end_);
	
	for(int i(-1) ; i<=1 ; i++)
	{
		for(int j(-1) ; j<=1 ; j++)
		{
			end_.setX(end.getX() + 2 * i * dim_max);
			end_.setY(end.getY() + 2 * j * dim_max);
			
			Vect vect_tmp(start, end_);
			
			if(vect_tmp.getNorm() < vect_min.getNorm())
				vect_min = vect_tmp;
		}
	}
	return vect_min;
}

////////////////////////// Equality tests

bool equal(const Point& p1, const Point& p2)
{
	return(vect_norm(p1, p2).getNorm() < epsilon_zero);
}

bool in_circle(const Point& point, const Circle& circle)
{
	return(vect_norm(point, circle.getCenter()).getNorm() <
		(circle.getRadius() - epsilon_zero));
}

bool intersection(const Circle& c1, const Circle& c2)
{	
	return(vect_norm(c1.getCenter(), c2.getCenter()).getNorm() < 
		(c1.getRadius() + c2.getRadius() - epsilon_zero));
}

////////////////////////// Drawing functions

void draw_full_arc(Color color, Point center, double radius, double angle)
{
	for(int i(-1) ; i<=1 ; i++)
	{
		for(int j(-1) ; j<=1 ; j++)
		{
			graphic_draw_full_arc(color, center.getX(), center.getY(), radius, angle);
		}
	}
}

void draw_circle(size_t color_, double x, double y, double radius)
{
	Color color (set_color(color_));
	graphic_draw_circle(color, x, y, radius);
	
	if(x + radius > dim_max)
		graphic_draw_circle(color, x - 2 * dim_max, y, radius);
		
	if(x - radius < dim_max)
		graphic_draw_circle(color, x + 2 * dim_max, y, radius);
		
	if(y + radius > dim_max)
		graphic_draw_circle(color, x, y - 2 * dim_max, radius);

	if(y - radius < dim_max)
		graphic_draw_circle(color, x, y + 2 * dim_max, radius);
}

void draw_full_circle(Color color, double x, double y, double radius, double angle)
{
	graphic_draw_full_circle(color, x, y, radius, angle);
	
	if(x + radius > dim_max)
		graphic_draw_full_circle(color, x - 2 * dim_max, y, radius, angle);
		
	if(x - radius < dim_max)
		graphic_draw_full_circle(color, x + 2 * dim_max, y, radius, angle);
		
	if(y + radius > dim_max)
		graphic_draw_full_circle(color, x, y - 2 * dim_max, radius, angle);
			
	if(y - radius < dim_max)
		graphic_draw_full_circle(color, x, y + 2 * dim_max, radius, angle);
}

void draw_full_circle(size_t color_, double x, double y, double radius)
{
	Color color (set_color(color_));
	graphic_draw_full_circle(color, x, y, radius);
	
	if(x + radius > dim_max)
		graphic_draw_full_circle(color, x - 2 * dim_max, y, radius);
		
	if(x - radius < dim_max)
		graphic_draw_full_circle(color, x + 2 * dim_max, y, radius);
		
	if(y + radius > dim_max)
		graphic_draw_full_circle(color, x, y - 2 * dim_max, radius);

	if(y - radius < dim_max)
		graphic_draw_full_circle(color, x, y + 2 * dim_max, radius);
}

void draw_full_circle_robot(size_t color, double x, double y, double radius)
{
	for(int i(-1) ; i<=1 ; i++)
	{
		for(int j(-1) ; j<=1 ; j++)
		{
			graphic_draw_full_circle(set_color(color), x + 2 * i * dim_max, 
									 y + 2 * j * dim_max, radius);
		}
	}
}

void draw_dotted_circle(Color color, double x, double y, double radius)
{
	graphic_draw_dotted_circle(color, x, y, radius);
	
	if(x + radius > dim_max)
		graphic_draw_dotted_circle(color, x - 2 * dim_max, y, radius);
		
	if(x - radius < dim_max)
		graphic_draw_dotted_circle(color, x + 2 * dim_max, y, radius);
		
	if(y + radius > dim_max)
		graphic_draw_dotted_circle(color, x, y - 2 * dim_max, radius);
			
	if(y - radius < dim_max)
		graphic_draw_dotted_circle(color, x, y + 2 * dim_max, radius);
}

void draw_dotted_circle(size_t color_, double x, double y, double radius)
{
	Color color (set_color(color_));
	graphic_draw_dotted_circle(color, x, y, radius);
	
	if(x + radius > dim_max)
		graphic_draw_dotted_circle(color, x - 2 * dim_max, y, radius);
		
	if(x - radius < dim_max)
		graphic_draw_dotted_circle(color, x + 2 * dim_max, y, radius);
		
	if(y + radius > dim_max)
		graphic_draw_dotted_circle(color, x, y - 2 * dim_max, radius);
			
	if(y - radius < dim_max)
		graphic_draw_dotted_circle(color, x, y + 2 * dim_max, radius);
}

void draw_cross(size_t color, double x, double y)
{
	for(int i(-1) ; i<=1 ; i++)
	{
		for(int j(-1) ; j<=1 ; j++)
		{
			graphic_draw_cross(set_color(color), x + 2 * i * dim_max,
							   y + 2 * j * dim_max);
		}
	}
}

void draw_star(size_t color, double x, double y)
{
	for(int i(-1) ; i<=1 ; i++)
	{
		for(int j(-1) ; j<=1 ; j++)
		{
			graphic_draw_star(set_color(color), x + 2 * i * dim_max,
							  y + 2 * j * dim_max);
		}
	}
}

void draw_plus(size_t color, double x, double y)
{
	for(int i(-1) ; i<=1 ; i++)
	{
		for(int j(-1) ; j<=1 ; j++)
		{
			graphic_draw_plus(set_color(color), x + 2 * i * dim_max,
							  y + 2 * j * dim_max);
		}
	}
}

void draw_wave(size_t color, double x, double y)
{
	for(int i(-1) ; i<=1 ; i++)
	{
		for(int j(-1) ; j<=1 ; j++)
		{
			graphic_draw_wave(set_color(color), x + 2 * i * dim_max, 
							  y + 2 * j * dim_max);
		}
	}
}

void draw_valid(size_t color, double x, double y)
{
	for(int i(-1) ; i<=1 ; i++)
	{
		for(int j(-1) ; j<=1 ; j++)
		{
			graphic_draw_valid(set_color(color), x + 2 * i * dim_max, 
							   y + 2 * j * dim_max);
		}
	}
}

void draw_square(size_t color, double x, double y)
{
	for(int i(-1) ; i<=1 ; i++)
	{
		for(int j(-1) ; j<=1 ; j++)
		{
			graphic_draw_square(set_color(color), x + 2 * i * dim_max, 
								y + 2 * j * dim_max);
		}
	}
}

void draw_full_square(size_t color, double x, double y)
{
	for(int i(-1) ; i<=1 ; i++)
	{
		for(int j(-1) ; j<=1 ; j++)
		{
			graphic_draw_full_square(set_color(color), x + 2 * i * dim_max,
									 y + 2 * j * dim_max);
		}
	}
}

void draw_connection(Point start, Vect vect)
{
	for(int i(-1) ; i<=1 ; i++)
	{
		for(int j(-1) ; j<=1 ; j++)
		{
			graphic_draw_connection(start.getX() + 2 * i * dim_max, 
									start.getY() + 2 * j * dim_max, 
									vect.getX(), vect.getY());
		}
	}
}

Color set_color(size_t color_)
{
	switch(color_%6)
	{
		case 0:
			return RED;
						
		case 1:
			return GREEN;
			
		case 2:
			return BLUE;
						
		case 3:
			return YELLOW;
			
		case 4:
			return MAGENTA;
			
		case 5:
			return CYAN;
	}
	return BLACK;
}
