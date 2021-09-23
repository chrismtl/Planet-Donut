//~ Jean-Claude Rihani 325233
//~ Christian Matala-Tala 327317

#ifndef FIELD_H
#define FIELD_H

#include <vector>
#include "geomod.h"

class Field
{
public:
	Field(Point pos = {}, double radius = 0, double capacity = 0);
	Field(Point pos, double radius, double capacity, bool& can_read);

	bool inter(const Field& field) const;
	bool inter(const Circle& circle) const;

	void draw_field() const;
	void write(std::ofstream& out) const;

	std::vector<double> get_field_data() const;
	Circle getCircle() const;
	double getCapacity() const;
	bool is_empty() const;
	double getScore() const;

	bool drill(double amount);
	bool on(Point pos);
	void setScore(double new_score);

private:
	Circle circle;
	double init_capacity;
	double capacity;
	double score;
};

void check_fields(Circle circle, bool& can_read);

void draw_fields();
void write_fields(std::ofstream& out);

void add_field_on(Point pos, std::vector<Field>& base_fields);
bool check_for_field(Point pos, Field& found_field);
bool check_for_field(Point pos, bool& found);
bool check_and_drill(Point pos, double resource);

void clear_fields();

#endif

