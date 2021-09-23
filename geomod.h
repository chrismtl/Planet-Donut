//~ Jean-Claude Rihani 325233
//~ Christian Matala-Tala 327317

#ifndef GEOMOD_H
#define GEOMOD_H

#include "graphic.h"
#include "constantes.h"

class Point
{
public:
	Point (double x = 0, double y = 0, bool norm = true);
	
	double getX() const;
	double getY() const;
	
	void set(double new_x, double new_y);
	void set(Point new_point);
	void setX(double x);
	void setY(double y);

private:
  	double x;
	double y;
};

Point operator+(Point p1, Point p2);

class Vect
{
public:
	Vect(const Point& p1 = Point (0, 0, 0), const Point& p2 = Point (0, 0, 0));
	
	double getX() const;
	double getY() const;
	double getNorm() const;
	
private:
	double x;
  	double y;
	double norm;
};

class Circle
{
public:
	Circle(const Point& center = Point (0, 0, 0), double radius = 0);
	
	Point  getCenter() const;
	double getRadius() const;
	
private:
	Point  center;
	double radius;
};

void set_max(double init_max);

bool equal_zero(double val);

double get_max();

double get_epsilon_zero();

double norm(double v);

void norm(Point& point);

Vect vect_norm(const Point& start, const Point& end);

bool equal(const Point& p1, const Point& p2);

bool in_circle(const Point& point, const Circle& circle);

bool intersection(const Circle& c1, const Circle& c2);

void draw_full_arc(Color color, Point center, double radius, double angle);

void draw_circle(size_t color, double x, double y, double radius = 50*rayon_base);

void draw_full_circle(Color color, double x, double y, double radius,
					  double angle = 2*M_PI);
void draw_full_circle(size_t color, double x, double y, double radius);
void draw_full_circle_robot(size_t color, double x, double y,
							double radius = 50*display_rayon_robot);

void draw_dotted_circle(Color color, double x, double y, double radius = rayon_comm);
void draw_dotted_circle(size_t color, double x, double y, double radius = rayon_comm);

void draw_cross(size_t color, double x, double y);

void draw_star(size_t color, double x, double y);

void draw_plus(size_t color, double x, double y);

void draw_wave(size_t color, double x, double y);

void draw_valid(size_t color, double x, double y);

void draw_square(size_t color, double x, double y_);

void draw_full_square(size_t color, double x, double y);

void draw_connection(Point start, Vect vect);

Color set_color(size_t color_);

#endif
