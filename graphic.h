//~ Jean-Claude Rihani 325233
//~ Christian Matala-Tala 327317

#ifndef GRAPHIC_H
#define GRAPHIC_H

#include <gtkmm/drawingarea.h>
#include <cairomm/context.h>
#include "constantes.h"

enum Color {BLACK, GRAY, RED, GREEN, BLUE, YELLOW, MAGENTA, CYAN, PURPLE, WHITE};

void graphic_set_context(const Cairo::RefPtr<Cairo::Context>& cr);

void graphic_draw_frame(double x, double y, double width, double height);

void graphic_draw_full_arc(Color color, double xc, double yc, double radius, 
						   double angle);

void graphic_draw_circle(Color color, double x, double y, 
						 double radius = 50*rayon_base);

void graphic_draw_full_circle(Color color, double x, double y, 
							  double radius = 50*display_rayon_robot,
							  double angle = 2*M_PI);

void graphic_draw_dotted_circle(Color color, double x, double y, 
								double radius = rayon_comm);

void graphic_draw_cross(Color color, double x, double y);

void graphic_draw_star(Color color, double x, double y);

void graphic_draw_plus(Color color, double x, double y);

void graphic_draw_wave(Color color, double x, double y);

void graphic_draw_valid(Color color, double x, double y);

void graphic_draw_square(Color color, double x, double y_);

void graphic_draw_full_square(Color color, double x, double y_);

void graphic_draw_connection(double x, double y, double x_, double y_);

#endif // GRAPHIC_H

