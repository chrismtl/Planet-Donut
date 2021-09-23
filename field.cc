//~ Jean-Claude Rihani 325233
//~ Christian Matala-Tala 327317

#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>

#include "field.h"
#include "geomod.h"
#include "message.h"
#include "constantes.h"

using namespace std;

static vector<Field> fields;


///////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////Constructors of Field/////////////////////////////////////

Field::Field(Point pos, double radius, double capacity)
	:circle(pos, radius), init_capacity(capacity), capacity(capacity), score(0)
{
}

Field::Field(Point pos, double radius, double capacity, bool& can_read)
	:circle(pos, radius), init_capacity(capacity), capacity(capacity), score(0)
{
	
	for(size_t i(0); i<fields.size() and can_read; i++)
	{
		can_read = inter(fields[i]);
	}
	if(can_read) fields.push_back(*this);
}

///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////Verifications////////////////////////////////////////

bool Field::inter(const Field& field) const
{
	if(intersection(this->circle, field.circle))
	{
		cout << message::field_superposition((this->circle).getCenter().getX(), 
											 (this->circle).getCenter().getY(),
											 field.circle.getCenter().getX(),
											 field.circle.getCenter().getY());
		return false;
	}
	return true;
}

bool Field::inter(const Circle& circle) const
{
	if(intersection(this->circle, circle))
	{
		cout << message::base_field_superposition(circle.getCenter().getX(),
												  circle.getCenter().getY(),
												  this->circle.getCenter().getX(), 
												  this->circle.getCenter().getY());
		return false;
	}
	return true;
}

void check_fields(Circle circle, bool& can_read)
{
	for(auto field : fields)
	{
		can_read = field.inter(circle);
	}
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////Draw/Write//////////////////////////////////////////

void Field::draw_field() const
{
	double x(circle.getCenter().getX());
	double y(circle.getCenter().getY());
	double radius(circle.getRadius());
	
	double angle((1-(capacity/init_capacity)) * 2 * M_PI);
	double sup_border(0.8*radius);
	double inf_border(0.2*radius);
	
	draw_full_circle(BLACK, x, y, radius);
	
	if(angle and capacity)
	{
		draw_full_arc(WHITE, circle.getCenter(), sup_border, angle);
	}
	
	if(!capacity) draw_full_circle(WHITE, x, y, sup_border);
	
	draw_full_circle(BLACK, x, y, inf_border);
}

void Field::write(ofstream& out) const
{
	out << circle.getCenter().getX() << " " << circle.getCenter().getY() << " ";
	out << circle.getRadius() << " " << capacity;
}

void draw_fields()
{
	for(auto& field : fields)
	{
		field.draw_field();
	}
}

void write_fields(ofstream& out)
{
	out << "# nb Gisement" << endl;
	out << fields.size() << endl;
	
	out << "# Gisements" << endl;
	
	for(auto& field : fields)
	{
		out << "\t";
		field.write(out);
		out << endl;
	}
}

///////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////gets//////////////////////////////////////////////

vector<double> Field::get_field_data() const
{
	return {circle.getCenter().getX(), circle.getCenter().getY(), circle.getRadius(), 
			capacity};
}

Circle Field::getCircle() const
{
	return circle;
}

double Field::getCapacity() const
{
	return capacity;
}

bool Field::is_empty() const
{
	if(capacity) return true;
	return false;
}

double Field::getScore() const
{
	return score;
}

///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////Manipulations////////////////////////////////////////

bool Field::drill(double amount)
{
	if(capacity >= amount)
	{
		capacity -= amount;
		return true;
	}
	return false;
}

bool Field::on(Point pos)
{
	return in_circle(pos, Circle (circle.getCenter(), circle.getRadius()+1));
}

void Field::setScore(double new_score)
{
	score = new_score;
}

bool fill_uid_on(Point pos, size_t& uid)
{
	for(size_t i(0); i < fields.size(); i++)
	{
		if(fields[i].on(pos))
		{
			uid = i;
			return true;
		}
	}
	return false;
}


void add_field_on(Point pos, std::vector<Field>& base_fields)
{
	for(auto field : fields)
	{
		if(field.on(pos)) base_fields.push_back(field);
	}
}

bool check_for_field(Point pos, Field& found_field)
{
	for(size_t i(0); i < fields.size(); i++)
	{
		if(fields[i].on(pos))
		{
			found_field = fields[i];
			return true;
		}
	}
	return false;
}

bool check_for_field(Point pos, bool& found)
{
	for(size_t i(0); i < fields.size(); i++)
	{
		if(fields[i].on(pos))
		{
			found = true;
			return true;
		}
	}
	return false;
}

bool check_and_drill(Point pos, double resource)
{
	for(size_t i(0); i < fields.size(); i++)
	{
		if(fields[i].on(pos))
		{
			return fields[i].drill(resource);
		}
	}
	return false;
}

void clear_fields()
{
	fields.clear();
}
