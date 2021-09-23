//~ Jean-Claude Rihani 325233
//~ Christian Matala-Tala 327317

#include <iostream>
#include "robot.h"
#include "geomod.h"
#include "message.h"
#include "field.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////ROBOT////////////////////////////////////////////////
Robot::Robot(int uid, size_t color, double dp, Point pos, 
			 Point target, Point base_center, bool reached)
	:uid(uid), color(color), dp(dp), 
	 pos(pos), target(target), my_base(base_center), 
	 reached(reached), neighbors({}), control_mode(REMOTE)
{}

Robot::Robot(const Robot* autre)
	:uid(autre->uid), color(autre->color), dp(autre->dp), pos(autre->pos), 
	 target(autre->target), my_base(autre->my_base), reached(autre->reached), 
	 control_mode(autre->control_mode)
{
	for(auto robot : autre->neighbors)
	{
		neighbors.push_back(robot->copy());
	}
}

Point Robot::getPos() const
{
	return pos;
}

unsigned int Robot::getUid() const
{
	return uid;
}

double Robot::getDp() const
{
	return dp;
}

void Robot::resetDp()
{
	dp = 0;
}

size_t Robot::getColor() const
{
	return color;
}

bool Robot::getReached() const
{
	return reached;
}

ControlMode Robot::getControlMode()
{
	return control_mode;
}

void Robot::setControlMode(ControlMode control_)
{
	control_mode = control_;
}

void Robot::add_neighbors(Robot* neighbor_robot)
{
	neighbors.push_back(neighbor_robot);
}

void Robot::clear_neighbors()
{
	neighbors.clear();
}

void Robot::draw_link()
{
	for(auto& neighbor : neighbors)
	{
		draw_connection(pos, vect_norm(pos, neighbor->getPos()));
	}
}

void Robot::draw_range()
{
	draw_dotted_circle(GRAY, pos.getX(), pos.getY());
}

void Robot::setTarget(Point new_target)
{
	target = new_target;
	reached = false;
}

Point Robot::getTarget() const
{
	return target;
}

void Robot::connexion(vector<Robot*>& network)
{
	network.push_back(this);
	for(auto& neighbor : neighbors)
	{
		bool connect(true);
		for(auto& robot : network)
		{
			if((neighbor->uid == robot->uid) and (neighbor->color == robot->color)) 
				connect = false;
		}
		if(connect)
			neighbor->connexion(network);
	}
}

void Robot::go_to(Point tar)
{
	Vect path(vect_norm(pos, tar));
	
	if(abs(path.getX()) > deltaD)
		pos.setX(pos.getX() + (deltaD/path.getNorm())*path.getX());
	else
		pos.setX(tar.getX());
	
	if(abs(path.getY()) > deltaD)
		pos.setY(pos.getY() + (deltaD/path.getNorm())*path.getY());
	else
		pos.setY(tar.getY());
	
	if(equal(pos, tar)) reached = true;
}

Robot::~Robot()
{}

///////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////PROSPECT//////////////////////////////////////////
Prospect::Prospect(int uid, size_t color, double dp,
				   Point pos, Point target, Point base,
				   bool reached, bool back_home, bool found)
	:Robot (uid, color, dp, pos, target, base, reached),
			back_home(back_home), found(found)
{
	if(dp > (maxD_prosp - dim_max)) low_dp = true;
	else low_dp = false;
}

Prospect::Prospect(int uid, size_t color, double dp,
				   Point pos, Point target, Point base,
				   bool reached, bool back_home, bool found, Field found_field)
	:Robot (uid, color, dp, pos, target, base, reached), back_home(back_home),
	 found(found), found_field(found_field)
{
	if(dp > (maxD_prosp - dim_max)) low_dp = true;
	else low_dp = false;
}

Prospect::Prospect(const Prospect* autre)
	:Robot (autre), back_home(autre->back_home), found(autre->found), 
	 found_field(autre->found_field)
{
	if(dp > (maxD_prosp - dim_max)) low_dp = true;
	else low_dp = false;
}

Prospect* Prospect::copy()
{
	return this;
}

size_t Prospect::getType() const
{
	return 0;
}

void Prospect::draw_robot() const
{
	draw_full_circle_robot(color, pos.getX(), pos.getY());
	if(found)
	{
		draw_valid(color, pos.getX(), pos.getY() + 50*display_rayon_robot * 2);
	}
	else if(!found)
	{
		draw_cross(color, pos.getX(), pos.getY() + 50*display_rayon_robot * 2);
	}
}

void Prospect::write(ofstream& out)
{
	out << "\t" << uid << " " << dp << " " << pos.getX() << " " << pos.getY();
	out << " " << target.getX() << " " << target.getY() << " ";
	
	if(reached) out << "true" << " ";
	else		out << "false" << " ";
	
	if(back_home) out << "true" << " ";
	else		  out << "false" << " ";
	
	if(found) out << "true" << " ";
	else	  out << "false" << " ";
	
	if(found) found_field.write(out);
}

void Prospect::move_autonomous()
{
	if(back_home) go_to(my_base);
	else if(!reached and (dp < (maxD_prosp - pow(dim_max, 1/2)))) go_to(target);
	else if(!reached)
	{
		back_home = true;
		low_dp = true;
	}
	else
	{
		found = check_for_field(pos, found_field);
		
		back_home = true;
	}
}

bool Prospect::getBackHome() const
{
	return back_home;
}

void Prospect::resetBackHome()
{
	back_home = false;
}

Field Prospect::get_my_field()
{
	return found_field;
}

void Prospect::add_field(std::vector<Field>& base_fields)
{
	base_fields.push_back(found_field);
}

bool Prospect::getFound() const
{
	return found;
}

bool Prospect::getLowDp() const
{
	return low_dp;
}

Prospect::~Prospect() {}

///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////DRILL////////////////////////////////////////////////
Drill::Drill(int uid, size_t color, double dp, Point pos, Point target, Point base,
			 bool reached, vector<Transport*> transports)
	:Robot (uid, color, dp, pos, target, base, reached),
		   found(false), transports(transports)
{}

Drill::Drill(const Drill* autre)
	:Robot (autre)
{	
	for(auto robot : autre->transports)
	{
		transports.push_back(robot->copy());
	}
}

Drill* Drill::copy()
{
	return this;
}

void Drill::draw_robot() const
{
	draw_full_circle_robot(color, pos.getX(), pos.getY());
	
	if(reached)
		draw_star(color, pos.getX(), pos.getY());
	else draw_plus(color, pos.getX(), pos.getY());
}

size_t Drill::getType() const
{
	return 1;
}

void Drill::write(ofstream& out)
{
	out << "\t" << uid << " " << dp << " ";
	out << pos.getX() << " " << pos.getY();
	out << " " << target.getX() << " " << target.getY() << " ";
	
	if(reached) out << "true" << " ";
	else 		out << "false" << " ";
}

void Drill::move_autonomous()
{
	if(!reached) go_to(target);
	else
	{
		if(check_for_field(pos, found))
		{
			if(transports.size() and check_and_drill(pos, deltaR))
			{
				transports[0]->fill_resource(deltaR);
			}
		}
	}
	transports.clear();
}

void Drill::add_transport(Transport* transport)
{
	transports.push_back(transport);
}

void Drill::add_field(std::vector<Field>& base_fields)
{
	//~ Asks field module to add the field the drill is on to base_fields
	add_field_on(pos, base_fields);
}

bool Drill::getFound()
{
	return found;
}

Drill::~Drill() {}

///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////TRANSPORT////////////////////////////////////////////
Transport::Transport(int uid, size_t color, double dp, Point pos, 
					 Point target, Point base, bool reached, bool filled)
	:Robot (uid, color, dp, pos, target, base, reached), filled(filled)
{
	if(filled) tank = deltaR;
	else tank = 0;
}

Transport::Transport(const Transport* autre)
	:Robot (autre), filled(autre->filled), tank(autre->tank)
{}

size_t Transport::getType() const
{
	return 2;
}

void Transport::draw_robot() const
{
	if(filled)
	{
		draw_full_square(color, pos.getX(), pos.getY());
	}
	else
	{
		draw_square(color, pos.getX(), pos.getY());
	}
	
	draw_full_circle_robot(color, pos.getX(), pos.getY());
}

Transport* Transport::copy()
{
	return this;
}

void Transport::write(ofstream& out)
{
	out << "\t" << uid << " " << dp << " ";
	out << pos.getX() << " " << pos.getY();
	out << " " << target.getX() << " " << target.getY() << " ";
	
	if(reached) out << "true" << " ";
	else 		out << "false" << " ";
}

void Transport::move_autonomous()
{
	if(reached and filled) setTarget(my_base);
	if(!reached) go_to(target);
}

bool Transport::is_full() const
{
	return (bool)tank;
}

void Transport::fill_resource(double resource)
{
	tank = resource;
	filled = true;
}

double Transport::load_resource()
{
	double tmp_tank = tank;
	tank = 0;
	filled = false;
	return tmp_tank;
}

bool Transport::getFilled() const
{
	return filled;
}

Transport::~Transport() {}


///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////COMM/////////////////////////////////////////////////
Comm::Comm(int uid, size_t color, double dp, Point pos, Point target, Point base,
		   bool reached)
	:Robot (uid, color, dp, pos, target, base, reached)
{}

Comm::Comm(const Comm* autre)
	:Robot (autre)
{}

size_t Comm::getType() const
{
	return 3;
}

void Comm::draw_robot() const
{
	draw_full_circle_robot(color, pos.getX(), pos.getY());
	draw_wave(color, pos.getX(), pos.getY() + 50*display_rayon_robot * 5.0/4);
}

void Comm::write(ofstream& out)
{
	out << "\t" << uid << " " << dp << " ";
	out << pos.getX() << " " << pos.getY();
	out << " " << target.getX() << " " << target.getY() << " ";
	
	if(reached) out << "true" << " ";
	else 		out << "false" << " ";
}

Comm* Comm::copy()
{
	return this;
}

void Comm::move_autonomous()
{
	if(!reached) go_to(target);
}

Comm::~Comm() {}
