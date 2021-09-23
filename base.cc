//~ Jean-Claude Rihani 325233
//~ Christian Matala-Tala 327317

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>
#include <cmath>

#include "base.h"
#include "robot.h"
#include "field.h"
#include "geomod.h"
#include "message.h"
#include "constantes.h"

using namespace std;

double debit(double distance);

///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////MAGIC NUMBERS////////////////////////////////////////
//~ to choose freely for experimenting 

//~ Spiral constants
constexpr double DELTA_TETA(M_PI/8);
constexpr double DELTA_R(10);

//~ Prospects constants
constexpr size_t MAX_PROSP(5);
constexpr double MAX_RADIUS(pow(2,1/2) * dim_max);
constexpr size_t DIST_MIN(51 * rayon_base);

//~ Comms constants
constexpr double RES_COMM_MIN(100);
constexpr int DELTA_X_GRID(rayon_comm - 1);
constexpr int MAX_RB_COMM(55); //default: 49 but 55 in case comms are given in the file

///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////CONSTRUCTORS/////////////////////////////////////////

Base::Base(Point pos, double resource, vector<Base>& bases, bool& can_read)
	:circle(pos, rayon_base), base_com(false), resource(resource), color(bases.size()),
	 my_uid(0)
{
	for(size_t i(0); i < bases.size(); ++i)
	{
		can_read = inter(bases[i]);
	}
	
	check_fields(circle, can_read);
}

Base::Base(const Base& autre)
	:circle(autre.circle), base_com(autre.base_com), resource(autre.resource),
	 color(autre.color), my_uid(autre.my_uid)
{
	for(auto robot : autre.all_robots)
	{
		all_robots.push_back(robot->copy());
	}
	
	for(auto robot : autre.robots_prospect)
	{
		robots_prospect.push_back(robot->copy());
	}
	
	for(auto robot : autre.robots_drill)
	{
		robots_drill.push_back(robot->copy());
	}
	
	for(auto robot : autre.robots_transport)
	{
		robots_transport.push_back(robot->copy());
	}
	
	for(auto robot : autre.robots_comm)
	{
		robots_comm.push_back(robot->copy());
	}
	
	for(auto robot : autre.network)
	{
		network.push_back(robot->copy());
	}
}

Base::~Base()
{
	all_robots.clear();
	robots_prospect.clear();
	robots_drill.clear();
	robots_transport.clear();
	robots_comm.clear();
}
///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////ROBOT/CREATION/////////////////////////////////////////

void Base::newProspect(unsigned int uid, double dp, Point pos, Point target, 
					   bool reached, bool back_home, bool found, bool& can_read)
{
	can_read = uids_validity(uid);
	
	robots_prospect.push_back(new Prospect (uid, color, dp, pos, target,
											circle.getCenter(),
											reached, back_home, found));
	
	all_robots.push_back(robots_prospect.back()->copy());
}

void Base::newProspect(unsigned int uid, double dp, Point pos, Point target,
					   bool reached, bool back_home, bool found,
					   Field found_field, bool& can_read)
{
	can_read = uids_validity(uid);
	robots_prospect.push_back(new Prospect (uid, color, dp, pos, target,
											circle.getCenter(), 
											reached, back_home, found, found_field));
	
	all_robots.push_back(robots_prospect.back()->copy());
}

void Base::createProspect()
{
	Point base(circle.getCenter());
	
	size_t nbP = robots_prospect.size();
	double angle((2*M_PI * nbP) / MAX_PROSP);
	
	Point target(base + Point (DIST_MIN*cos(angle), DIST_MIN*sin(angle)));
	
	robots_prospect.push_back(new Prospect (next_uid(), color, 0, circle.getCenter(),
											target, circle.getCenter(),
											false, false, false));
	
	all_robots.push_back(robots_prospect.back()->copy());
}

void Base::newDrill(unsigned int uid, double dp, Point pos, Point target, 
					bool reached, bool& can_read)
{
	can_read = uids_validity(uid);
	robots_drill.push_back(new Drill (uid, color, dp, pos, target,
									 circle.getCenter(), reached));
	
	all_robots.push_back(robots_drill.back()->copy());
}

void Base::createDrill(unsigned int uid, size_t color, double dp,
					   Point pos, Point target)
{
	robots_drill.push_back(new Drill (uid, color, dp, pos, target, 
							circle.getCenter(), false));
	
	all_robots.push_back(robots_drill.back()->copy());
}

void Base::newTransport(unsigned int uid, double dp, Point pos, Point target, 
						bool reached, bool back_home, bool& can_read)
{
	can_read = uids_validity(uid);
	robots_transport.push_back(new Transport (uid, color, dp, pos, target,
											  circle.getCenter(), reached, back_home));
	
	all_robots.push_back(robots_transport.back()->copy());
}

void Base::createTransport(unsigned int uid, size_t color, double dp,
						   Point pos, Point target)
{
	robots_transport.push_back(new Transport (uid, color, dp, pos, target, 
							   circle.getCenter(), false, false));
		
	all_robots.push_back(robots_transport.back()->copy());
}

void Base::newComm(unsigned int uid, double dp, Point pos, Point target, 
				   bool reached, bool& can_read)
{
	can_read = uids_validity(uid);
	
	if(equal(circle.getCenter(), pos) and !base_com)
	{
		base_com = true;
		my_uid = uid;
		
		if(robots_comm.empty()) robots_comm.push_back(new Comm (uid, color, dp,
																pos, target,
																circle.getCenter(),
																reached));
		else
		{
			robots_comm.push_back(robots_comm.front());
			robots_comm[0] = new Comm (uid, color, dp, pos, target, circle.getCenter(),
									   reached);
		}
	}
	else
	{
		robots_comm.push_back(new Comm (uid, color, dp, pos, target,
										circle.getCenter(), reached));
	}
	all_robots.push_back(robots_comm.back()->copy());
}

void Base::createComm(Point target)
{
	robots_comm.push_back(new Comm (next_uid(), color, 0, circle.getCenter(), 
									target, circle.getCenter(), false));
	
	all_robots.push_back(robots_comm.back()->copy());
}
///////////////////////////////////////////////////////////////////////////////////////

bool Base::validity()
{
	if(!base_com)
	{
		cout << message::missing_robot_communication(this->circle.getCenter().getX(), 
													 this->circle.getCenter().getY());
		return false;
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////DRAWING/////////////////////////////////////////
void Base::draw_base()
{
	draw_circle(color, circle.getCenter().getX(), circle.getCenter().getY());
	
	for(auto& robot : all_robots)
	{
		robot->draw_robot();
	}
}

void Base::draw_link()
{
	for(auto& robot : all_robots)
	{
		robot->draw_link();
	}
}

void Base::draw_range()
{
	for(auto& robot : all_robots)
	{
		robot->draw_range();
	}
}
///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////EXPORT/////////////////////////////////////////////
double Base::getResource() const
{
	return resource;
}

void Base::fill_nb_data(vector<unsigned int>& nb_data)
{
	nb_data[0] = my_uid;
	nb_data[1] = robots_prospect.size();
	nb_data[2] = robots_drill.size();
	nb_data[3] = robots_transport.size();
	nb_data[4] = robots_comm.size();
}

void Base::fill_res_data(vector<double>& res_data)
{
	if(resource < finR)
	{
		res_data[0] = resource;
		res_data[1] = (100*resource) / finR;
	}
	else
	{
		res_data[0] = finR;
		res_data[1] = 100;
	}
}

void Base::write_pos(ofstream& out)
{
	out << circle.getCenter().getX() << " " << circle.getCenter().getY() << " ";
}

void Base::write_robots(ofstream& out)
{
	static vector<string> type_names({"Prospect", "Drill",
									  "Transport", "Communication"});
	
	out << "\t#Prospect" << endl;
	for(auto robot : robots_prospect)
	{
		out << "\t";
		robot->write(out);
		out << endl;
	}
	
	out << "\t#Drill" << endl;
	for(auto robot : robots_drill)
	{
		out << "\t";
		robot->write(out);
		out << endl;
	}
	
	out << "\t#Transport" << endl;
	for(auto robot : robots_transport)
	{
		out << "\t";
		robot->write(out);
		out << endl;
	}
	
	out << "\t#Comm" << endl;
	for(auto robot : robots_comm)
	{
		out << "\t";
		robot->write(out);
		out << endl;
	}
}
///////////////////////////////////////////////////////////////////////////////////////

void Base::add_transport_drill(unsigned int drill_uid, unsigned int transport_uid)
{
	size_t i_drill, j_trans;
	
	for(size_t i(0); i < robots_drill.size(); i++) 
	{
		if(robots_drill[i]->getUid() == drill_uid) i_drill = i;
	}
	
	for(size_t j(0); j < robots_transport.size(); j++)
	{
		if(robots_transport[j]->getUid() == transport_uid) j_trans = j;
	}
	
	if(!robots_transport[j_trans]->is_full())
	{
		robots_drill[i_drill]->add_transport(robots_transport[j_trans]->copy());
	}
}

///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////VERIFICATIONS////////////////////////////////////////
bool Base::inter(const Base& base) const
{
	if(intersection(this->circle, base.circle))
	{
		cout << message::base_superposition((this->circle).getCenter().getX(), 
											  (this->circle).getCenter().getY(), 
											  base.circle.getCenter().getX(), 
											  base.circle.getCenter().getY());
		return false;
	}
	return true;
}

bool Base::uids_validity(unsigned int uid)
{
	for(auto robot : all_robots)
	{
		if(uid == robot->getUid())
		{
			cout << message::identical_robot_uid(uid);
			return false;
		}
	}
	
	uids.push_back(uid);
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////

void Base::update_neighbors(const Base& base_2)
{
	for(auto& robot_1 : all_robots)
	{
		for(auto& robot_2 : base_2.all_robots)
		{
			if(equal(circle.getCenter(), base_2.circle.getCenter()))
			{
				if(robot_1->getUid() == robot_2->getUid()) continue;
				
				//~ Search for a transport on a drill that is ready to get filled
				if((robot_1->getType() == RB_DRILL)
				   and (robot_2->getType() == RB_TRANSPORT)
				   and (equal(robot_1->getPos(), robot_2->getPos())))
				{
					if(!((Transport*)(robot_2))->getFilled())
					{
						add_transport_drill(robot_1->getUid(), robot_2->getUid());
					}
				}
			}
			
			if(in_circle(robot_2->getPos(), Circle (robot_1->getPos(), rayon_comm)))
				robot_1->add_neighbors(robot_2);
		}
	}
}

void Base::clear_neighbors()
{
	for(auto& robot : all_robots)
	{
		robot->clear_neighbors();
	}
}

void Base::connexion()
{
	robots_comm.front()->connexion(network);
	for(auto& mine : all_robots)
	{
		bool control_(false);
		for(auto& robot : network)
		{
			if((robot->getUid() == mine->getUid()) and (robot->getColor() == color))
				control_ = true;
		}
		
		if(control_) mine->setControlMode(REMOTE);
		else mine->setControlMode(AUTONOMOUS);
	}
}

void Base::receive_resource()
{
	//~ Receive resources
	double resource_aquired;
	for(auto& robot : robots_transport)
	{
		if(equal(robot->getPos(), circle.getCenter()) and robot->is_full())
		{
			resource_aquired += robot->load_resource();
		}
	}
	resource += resource_aquired;
}

bool Base::is_new(Field new_field)
{
	Point center(0,0);
	Point new_center(new_field.getCircle().getCenter());
	
	for(auto field : my_fields)
	{
		center = field.getCircle().getCenter();
		if(equal(center, new_center)) return false;
	}
	return true;
}

void Base::get_field_info()
{
	my_fields.clear();
	//~ Search for field info in its network
	for(auto robot : network)
	{
		//~ Search for field from prospect
		if(robot->getType() == RB_PROSPECT and ((Prospect*)(robot))->getFound())
		{
			//~ Verifies if its a new field
			if(is_new(((Prospect*)(robot))->get_my_field()))
			{
				((Prospect*)(robot))->add_field(my_fields);
			}
		}
		
		//~ Search for fields from drill
		if(robot->getType() == RB_DRILL and ((Drill*)(robot))->getFound())
		{
			//~ Verifies if its a new field
			Field new_field;
			
			if(check_for_field(robot->getPos(), new_field))
			{
				if(is_new(new_field))
				{
					my_fields.push_back(new_field);
				}
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////FIELD/SCORE////////////////////////////////////////
void Base::give_field_score()
{
	double sum_score(0);
	
	for(auto& field : my_fields)
	{
		vector<int> nb_transports(transports_on(field.getCircle()));
		int my_trans = nb_transports[0];
		int other_trans = nb_transports[1];
		
		//~ Calculates distance from base center to field center
		double dist_center = vect_norm(circle.getCenter(),
									   field.getCircle().getCenter()).getNorm();
		
		//~ We know the drill on field is at the shortest distance from base
		double distance = (dist_center - field.getCircle().getRadius());
		
		//~ Calculates resource debit (per second) of the field
		//~ (just for one trans if my_trans = 0)
		int nb_trans(1);
		if(my_trans) nb_trans = my_trans;
		
		double field_debit(nb_trans * debit(distance));
		
		double score = field.getCapacity();
		score *= (1 + other_trans) * field_debit;
		score /= (1 + my_trans);
		
		field.setScore(score);
		sum_score += score;
	}
	
	if(sum_score)
	{
		for(auto& field : my_fields)
		{
			field.setScore( (field.getScore())/sum_score );
		}
	}
}

double debit(double distance)
{
	double transport_speed(deltaD/0.025);
	double path_time(2 * (distance/transport_speed));
	
	return (deltaR / path_time);
}

vector<int> Base::transports_on(Circle field_circle)
{
	Point field_center(field_circle.getCenter());
	double field_radius(field_circle.getRadius());
	Point target(0,0);
	
	//~ Calculates how many ransports from my base ([0])
	//~ and from other bases ([1]) are on this field
	int my_transports(0), other_transports(0);
	for(auto robot : network)
	{
		target = robot->getTarget();
		
		if(robot->getType() == RB_TRANSPORT
		   and vect_norm(target, field_center).getNorm() <= (field_radius + 1))
		{
			if(robot->getColor() == color) my_transports++;
			else other_transports++;
		}
	}
	
	return {my_transports, other_transports};
}
///////////////////////////////////////////////////////////////////////////////////////

void Base::clear_network()
{
	network.clear();
}

void Base::maintenance()
{
	for(auto& robot : all_robots)
	{
		if(equal(robot->getPos(), circle.getCenter()))
		{
			resource -= robot->getDp()*cost_repair;
			robot->resetDp();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////CREATION/////////////////////////////////////////////
void Base::creation()
{
	//~ Calculates if we want to send a drill,
	//~ and how much transport with it, to a field
	vector<bool> nbD_my_fields;
	vector<int> nbT_my_fields;
	
	for(auto& field : my_fields)
	{
		bool yes_drill(true);
		for(auto& drill : robots_drill)
		{
			if((field.getScore() < 0.15) or in_circle(drill->getTarget(), 
			   Circle(field.getCircle().getCenter(), 
			   field.getCircle().getRadius() + 1)))
			{
				yes_drill = false;
			}
		}
		nbD_my_fields.push_back(yes_drill);
		nbT_my_fields.push_back(field.getScore()*robots_transport.size());
	}
	
	//~ Counter to verify we create 3 robots per simulation
	size_t count(0);
	
	//~ Manage creation of every types of robots
	creation_drill(count, nbD_my_fields);
	creation_trans(count, nbT_my_fields);
	creation_com  (count);
	creation_prosp(count);
}

//////////////////////////////////CREATION/DRILL///////////////////////////////////////
void Base::creation_drill(size_t& count, vector<bool>& nbD_my_fields)
{
	bool drill_placed(true);
	for(auto& robot : robots_drill)
		if(!robot->getReached()) drill_placed = false;
	
	if(drill_placed)
	{
		for(size_t i(0); i < nbD_my_fields.size() ; i++)
		{
			if(nbD_my_fields[i])
			{
				Point field_pos(my_fields[i].getCircle().getCenter());
				double field_radius(my_fields[i].getCircle().getRadius());
				Vect best_vect(vect_norm(circle.getCenter(), field_pos));
				Point target(field_pos.getX() - 
							 best_vect.getX()*field_radius/best_vect.getNorm(), 
							 field_pos.getY() - 
							 best_vect.getY()*field_radius/best_vect.getNorm());
				for(auto& robot : robots_drill)
				{
					if(equal(robot->getPos(), circle.getCenter()))
					{
						robot->setTarget(target);
						nbD_my_fields[i] = false;
						break;
					}
				}
				if((nbD_my_fields[i])
				   and (count < max_robots)
				   and (resource > cost_forage))
				{
					resource -= cost_forage;
					createDrill(next_uid(), color, 0, circle.getCenter(), target);
					count++;
				}
			}
		}
	}
}

//////////////////////////////////CREATION/TRANSPORT///////////////////////////////////
void Base::creation_trans(size_t& count, vector<int>& nbT_my_fields)
{
	for(size_t i(0); i <  nbT_my_fields.size() ; i++)
	{
		Point target(circle.getCenter());
		for(auto& drill : robots_drill)
		{
			if(drill->getControlMode() == REMOTE
			   and in_circle(drill->getPos(),
							 Circle (my_fields[i].getCircle().getCenter(),
							 my_fields[i].getCircle().getRadius() + 1)))
			{
				target = drill->getPos();
			}
		}
		
		check_create_trans(i, count, nbT_my_fields, target);
		
		for(auto& robot : robots_transport)
		{
			if(equal(robot->getTarget(), target))
				nbT_my_fields[i]--;
			
			if(equal(robot->getPos(), target)
			   and (nbT_my_fields[i] <= 0)
			   and (my_fields[i].getCapacity() == 0)
			   and (!robot->getFilled()))
				robot->setTarget(circle.getCenter());
				
			if((equal(robot->getTarget(), circle.getCenter())
			   and (nbT_my_fields[i] > 0))
			   and equal(robot->getPos(), circle.getCenter()))
			{
				robot->setTarget(target);
				nbT_my_fields[i]--;
			}
		}
	}
}

void Base::check_create_trans(size_t& i, size_t& count,
							  vector<int>& nbT_my_fields, const Point& target)
{
	if((((my_fields[i].getScore() >= 0.15) and (nbT_my_fields[i] == 0)) and 
		 (resource > + cost_forage)) and (count < max_robots))
	{
		resource -= cost_transp;
		createTransport(next_uid(), color, 0, circle.getCenter(), target);
		count++;
	}
	
	if((((my_fields[i].getScore() >= 0.30) and (nbT_my_fields[i] <= 1)) and 
		 (resource > + cost_forage)) and (count < max_robots))
	{
		resource -= cost_transp;
		createTransport(next_uid(), color, 0, circle.getCenter(), target);
		count++;
	}
	if((((my_fields[i].getScore() >= 0.50) and (nbT_my_fields[i] <= 2)) and 
		 (resource > + cost_forage)) and (count < max_robots))
	{
		resource -= cost_transp;
		createTransport(next_uid(), color, 0, circle.getCenter(), target);
		count++;
	}
}

///////////////////////////////////CREATION/COMM///////////////////////////////////////
void Base::creation_com(size_t& count)
{
	Point base(circle.getCenter());
	vector<size_t> not_placed;
	vector<size_t> placed;
	
	//~ Checks if all my comms are at their target
	bool comm_placed(true);
	for(size_t n(0); n < robots_comm.size(); n++)
	{
		if(robots_comm[n]->getControlMode() == REMOTE)
		{
			if(!robots_comm[n]->getReached())
			{
				//~ Checks if its target is on spot of the grid
				if(on_spot(robots_comm[n]->getTarget()))
					comm_placed = false;
				
				else not_placed.push_back(n);
			}
			else if(on_spot(robots_comm[n]->getPos())
					and robots_comm[n]->getUid() != my_uid)
			{
				placed.push_back(n);
			}
		}
	}
	//~ Checks if one of its robot is outside the network (is AUTONOMOUS)
	bool autonomous(false);
	for(auto& robot : all_robots)
	{
		if(robot->getControlMode() == AUTONOMOUS
		   and (robot->getType() != RB_COMM)) autonomous = true;
	}

	if(comm_placed
	   and autonomous
	   and resource > RES_COMM_MIN) upgrade_grid(count, not_placed, placed);
}

bool Base::on_spot(Point target)
{
	Point base(circle.getCenter());
	
	return (((((int)(vect_norm(base, target).getX()) % DELTA_X_GRID ) == 0 ) and 
			(((int)(vect_norm(base, target).getY()))) % DELTA_X_GRID) == 0);
}

void Base::upgrade_grid(size_t& count,
						vector<size_t>& not_placed, vector<size_t>& placed)
{
	Point base(circle.getCenter());
	size_t nb_placed(placed.size());
	int grid_size(0);
	
	if	   (nb_placed < 8 ) grid_size = 1;
	else if(nb_placed < 24) grid_size = 2;
	else if(nb_placed < 49) grid_size = 3;
	
	for(int i(-grid_size); i <= grid_size and count < max_robots; i++)
	{
		if(abs(i) == grid_size)
		{
			for(int j(-grid_size); j <= grid_size and count < max_robots; j++)
			{
				Point new_spot(base + Point (i * DELTA_X_GRID, j * DELTA_X_GRID));
				if(spot_taken(new_spot, placed)) continue;
				create_new_spot(i,j, base, new_spot, count, placed, not_placed);
			}
		}
		else
		{
			for(int j : {-grid_size, grid_size})
			{
				Point new_spot(base + Point (i * DELTA_X_GRID, j * DELTA_X_GRID));
				if(count == max_robots) break;
				if(spot_taken(new_spot, placed)) continue;
				create_new_spot(i,j, base, new_spot, count, placed, not_placed);
			}
		}
	}
}

bool Base::spot_taken(Point& new_spot, vector<size_t>& placed)
{
	for(size_t k(0); k < placed.size(); k++)
		if(equal(robots_comm[placed[k]]->getTarget(), new_spot)) return true;
	
	return false;
}

void Base::create_new_spot(int& i, int& j, Point& base, Point& new_spot,
						   size_t& count,
						   vector<size_t>& placed, vector<size_t>& not_placed)
{
	if(not_placed.size())
	{
		robots_comm[not_placed.back()]->setTarget(new_spot);
		not_placed.pop_back();
	}
	else
	{
		if(robots_comm.size() < MAX_RB_COMM and (resource > cost_com))
		{
			resource -= cost_com;
			createComm(new_spot);
			count++;
		}
	}
}
/////////////////////////////////CREATION/PROSPECT/////////////////////////////////////
void Base::creation_prosp(size_t& count)
{
	if(((robots_prospect.size() < robots_drill.size())
	   or (robots_prospect.size() < MAX_PROSP))
	   and (resource > 201 + cost_prosp) and (count < max_robots))
	{
		resource -= cost_prosp;
		createProspect();
		count++;
	}
}

unsigned int Base::next_uid()
{
	static bool i(true);
	if(i and uids.size())
		sort(uids.begin(), uids.end());
	
	i= false;
	
	if(uids.size() != 0)
	{
		unsigned int uid(uids.back() + 1);
		uids.push_back(uid);
		return(uid);
	}
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////

void Base::update_remote()
{
	Point base(circle.getCenter());
	for(auto& robot : all_robots)
	{
		if(robot->getControlMode() == REMOTE)
		{
			Point pos(robot->getPos());
			
			if(robot->getType() == RB_PROSPECT
			   and ( ((Prospect*)(robot))->getBackHome())
			   and (!((Prospect*)(robot))->getLowDp())
			   and vect_norm(pos, base).getNorm() > 50
			   and vect_norm(pos, base).getNorm() < MAX_RADIUS)
			{
				Vect dist(vect_norm(circle.getCenter(), robot->getTarget()));
				double angle(atan(dist.getY() / dist.getX()));
				
				if((dist.getX() < 0 and dist.getY() > 0)
				   or (angle < M_PI/2 and dist.getX() < 0)) angle += M_PI;
				else if(dist.getX() < 0 and dist.getY() < 0) angle += M_PI;
				
				angle += (DELTA_TETA);
				
				double r(dist.getNorm());
				if(r < dim_max) r += DELTA_R;
				
				robot->setTarget(Point (base.getX() + (r*cos(angle)),
										base.getY() + (r*sin(angle))));
				
				((Prospect*)(robot))->resetBackHome();
			}
			robot->move_autonomous(); ///changement
		}
	}
}

void Base::update_autonomous()
{
	for(auto robot : all_robots)
	{
		if(robot->getUid() == my_uid) continue;
		
		if(robot->getControlMode() == AUTONOMOUS) robot->move_autonomous();
	}
}

void Base::clear_info()
{
	my_fields.clear();
}

void Base::check_destruction(size_t i, vector<bool>& active_mode)
{
	if(resource <= get_epsilon_zero())
	{
		all_robots.clear();
		robots_prospect.clear();
		robots_drill.clear();
		robots_transport.clear();
		robots_comm.clear();
		
		active_mode[i] = false;
	}
}
