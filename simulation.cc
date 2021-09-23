//~ Jean-Claude Rihani 325233
//~ Christian Matala-Tala 327317

#include <cstdlib>
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <array>
#include <memory>

#include "simulation.h"
#include "base.h"
#include "field.h"
#include "geomod.h"
#include "constantes.h"
#include "message.h"

using namespace std;

namespace
{
	std::array<int, 4> nbR; //nbR[0] = nbP , nbR[1] = nbF , nbR[2] = nbT , nbR[3] = nbC
	
	static int state(NBG); // état initial
	static int i(0), total(0);
}

void Simulation::start()
{
	for(auto& base : bases)
	{
		base.clear_neighbors();
		
		for(auto& base_2 : bases)
		{
			base.update_neighbors(base_2);
		}
		
		base.clear_network();
		
		if(base.getResource() < finR)
		{
			base.connexion();
			base.receive_resource();
			base.maintenance();
			
			base.get_field_info();
			base.give_field_score();
			
			base.creation();
			
			base.update_remote();
			base.update_autonomous();
		}
		
		base.clear_info();
	}
	
	for(size_t i(0) ; i < bases.size() ; i++)
	{
		bases[i].check_destruction(i, active_mode);
	}
}

void Simulation::reset_data()
{
	state = NBG;
	i = 0;
	total = 0;
	nbR = {0, 0, 0, 0};
	
	bases.clear();
	clear_fields();
}

void Simulation::draw()
{
	draw_fields();
	
	for(size_t i(0); i < bases.size(); i++)
	{
		if(active_mode[i]) bases[i].draw_base();
	}
}

void Simulation::draw_link()
{
	for(auto& base : bases)
	{
		base.draw_link();
	}
}

void Simulation::draw_range()
{
	for(auto& base : bases)
	{
		base.draw_range();
	}
}

bool Simulation::read(string file_name)
{
	set_max(dim_max);
	string line;
	ifstream file;
	file.open(file_name);
	if(!file.fail())
	{
		can_read = true;
		reset_data();
		while(getline(file >> ws, line) && can_read)
		{
			if((line.empty()) or (line[0] == '#')
							  or (line[0] == '\n')
							  or (line[0] == '\r'))
				continue;
			if(line.find("#") != string::npos)
				line = line.substr(0, line.find("#"));
			
			if(line.size() < max_line) decode_line(line);
		}
		
		if(can_read)
		{
			for(auto& base : bases)
			{
				base.clear_neighbors();
				for(auto& base_2 : bases) base.update_neighbors(base_2);
			}
			cout << message::success() << endl;
		}
		else reset_data();
	}
	else
	{
		can_read = false;
		cout << "Could not open file" << endl;
	}
	return can_read;
}

void Simulation::export_current_map(std::ofstream& out)
{
	out << fixed << setprecision(6);
	
	write_fields(out);
	
	int nbB(0);
	for(size_t i(0); i < active_mode.size(); i++)
	{
		if(active_mode[i]) nbB++;
	}
	
	out << endl << "# nb Base" << endl;
	out << nbB << endl;
	out << "# Bases" << endl;
	
	vector<SimData> bases_data;
	fill_bases_data(bases_data);
	
	for(size_t i(0); i < bases_data.size(); i++)
	{
		if(!active_mode[i]) continue;
		bases[i].write_pos(out);
		out << " " << bases_data[i].ressource << " ";
		out << bases_data[i].nbP << " " << bases_data[i].nbF << " ";
		out << bases_data[i].nbT << " " << bases_data[i].nbC << endl;
		
		bases[i].write_robots(out);
		out << endl;
	}
}

void Simulation::fill_bases_data(std::vector<SimData>& report)
{
	report.clear();
	for(auto& base : bases)
	{
		SimData base_data;
		
		vector<unsigned int> nb_data(5);
		vector<double> res_data(2);
		
		base.fill_nb_data(nb_data);
		base.fill_res_data(res_data);
		
		base_data.Uid = nb_data[0];
		base_data.nbP = nb_data[1];
		base_data.nbF = nb_data[2];
		base_data.nbT = nb_data[3];
		base_data.nbC = nb_data[4];
		base_data.ressource = res_data[0];
		base_data.ressource_p = res_data[1];
		
		report.push_back(base_data);
	}
}

void Simulation::decode_line(string line)
{
	istringstream data(line);
	switch(state)
	{
		case NBG: 
			case_nbf(data); break;
		
		case FIELD:
			++i;
			case_field(data);
			break;

		case NBB:
			case_nbb(data); break;

		case BASE:
			case_base(data); break;
			
		case PROSPECT:
			i++;
			case_prospect(data);
			break;
		  
		case DRILL:
			i++;
			case_drill(data);
			break;
		
		case TRANS:
			i++;
			case_trans(data);
			break;
		
		case COMM:
			i++;
			case_comm(data);
			break;
	}
}

void Simulation::case_nbf(istringstream& data)
{
	data >> total;
	state = FIELD;
	
	if(!total)
	state = NBB;
}

void Simulation::case_field(istringstream& data)
{
	double x(0), y(0), radius(0), capacity(0);
	data >> x >> y >> radius >> capacity;
	
	Field (Point (x, y), radius, capacity, can_read);
	
	if(i == total)
	{
		state = NBB;
		total = 0;
		i = 0;
	}
}

void Simulation::case_nbb(istringstream& data)
{
	data >> total;
	bases.reserve(total);
	state = BASE;
}

void Simulation::case_base(istringstream& data)
{
	double x(0), y(0), resource(0);
	data >> x >> y >> resource >> nbR[0] >> nbR[1] >> nbR[2] >> nbR[3];
	
	bases.push_back(Base (Point (x,y), resource, bases, can_read));
	active_mode.push_back(true);
	
	if(nbR[0] != 0)
	{
		state = PROSPECT;
		total = nbR[0];
	}
	else if(nbR[1] != 0)
	{
		state = DRILL;
		total = nbR[1];
	}
	else if(nbR[2] != 0)
	{
		state = TRANS;
		total = nbR[2];
	}
	else if(nbR[3] != 0)
	{
		state = COMM;
		total = nbR[3];
	}
	else
	{
		can_read = bases.back().validity();
	}
}

void Simulation::case_prospect(istringstream& data)
{
	int uid(0);
	double dp(0), x(0), y(0), xb(0), yb(0), xg(0), yg(0);
	double rayong(0), capaciteg(0);
	bool reached(false), back_home(false), found(false);
	string str_reached, str_back_home, str_found;
	data >> uid >> dp >> x >> y >> xb >> yb >> str_reached >> str_back_home;
	data >> str_found;
	
	if(str_reached == "true") reached = true;
	else if(str_reached == "false") reached = false;
	
	if(str_back_home == "true") back_home = true;
	else if(str_back_home == "false") back_home = false;
	
	if(str_found == "true") found = true;
	else if(str_found == "false") found = false;
	
	if(found)
	{
		data >> xg >> yg >> rayong >> capaciteg;
		bases.back().newProspect(uid, dp, Point (x, y), Point (xb, yb),
								  reached, back_home, found,
								  Field(Point (xg, yg), rayong, capaciteg), can_read);
	}
	else
	{
		bases.back().newProspect(uid, dp, Point (x, y), Point (xb, yb),
										  reached, back_home, found, can_read);
	}
	
	case_prospect_verification();
}

void Simulation::case_prospect_verification()
{
	if(i == total)
	{
		if(nbR[1] != 0)
		{
			state = DRILL;
			total = nbR[1];
		}
		else if(nbR[2] != 0)
		{
			state = TRANS;
			total = nbR[2];
		}
		else if(nbR[3] != 0)
		{
			state = COMM;
			total = nbR[3];
		}
		else
			can_read = bases.back().validity();
		i = 0;
	}
}

void Simulation::case_drill(istringstream& data)
{
	int uid(0);
	double dp(0), x(0), y(0), xb(0), yb(0);
	bool reached(false);
	string str_reached;
	data >> uid >> dp >> x >> y >> xb >> yb >> str_reached;
	
	Point pos(x, y);
	Point target(xb, yb);
	
	if(str_reached == "true") reached = true;
	else if(str_reached == "false") reached = false;
	
	bases.back().newDrill(uid, dp, pos, target, reached, can_read);
	
	if(i == total)
	{
		if(nbR[2] != 0)
		{
			state = TRANS;
			total = nbR[2];
		}
		else if(nbR[3] != 0)
		{
			state = COMM;
			total = nbR[3];
		}
		else
			can_read = bases.back().validity();
		i = 0;
	}
}

void Simulation::case_trans(istringstream& data)
{
	int uid(0);
	double dp(0), x(0), y(0), xb(0), yb(0);
	bool reached(false), back_home(false);
	string str_reached, str_back_home;
	data >> uid >> dp >> x >> y >> xb >> yb >> str_reached >> str_back_home;
	
	if(str_reached == "true") reached = true;
	else if(str_reached == "false") reached = false;
	 
	if(str_back_home == "true") back_home = true;
	else if(str_back_home == "false") back_home = false;
	
	bases.back().newTransport(uid, dp, Point (x, y), 
							  Point (xb, yb), reached, back_home, can_read);
	
	if(i == total)
	{
		if(nbR[3] != 0)
		{
			state = COMM;
			total = nbR[3];
		}
		else
			can_read = bases.back().validity();
		i = 0;
	}
}

void Simulation::case_comm(istringstream& data)
{
	unsigned uid(0);
	double dp(0), x(0), y(0), xb(0), yb(0);
	bool reached(false);
	string str_reached;
	data >> uid >> dp >> x >> y >> xb >> yb >> str_reached;
	
	if(str_reached == "true") reached = true;
	else if(str_reached == "false") reached = false;
	
	bases.back().newComm(uid, dp, Point (x, y), Point (xb, yb), reached, can_read);
	
	if(i == total)
	{
		can_read = bases.back().validity();
		state = BASE;
		i = 0;
	}
}
