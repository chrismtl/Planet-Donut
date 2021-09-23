//~ Jean-Claude Rihani 325233
//~ Christian Matala-Tala 327317

#ifndef SIMULATION_H
#define SIMULATION_H

#include <fstream>
#include <string>
#include <vector>
#include <array>

#include "base.h"
#include "field.h"

enum Etat_lecture {NBG, FIELD, NBB, BASE, PROSPECT, DRILL, TRANS, COMM};

struct SimData
{
	unsigned int Uid;
	unsigned int nbP;
	unsigned int nbF;
	unsigned int nbT;
	unsigned int nbC;
	double ressource;
	double ressource_p;
};

class Simulation
{
public:
	void start();
	void reset_data();
	void draw();
	void draw_link();
	void draw_range();
	bool read(std::string file_name);
	void export_current_map(std::ofstream& out);
	void fill_bases_data(std::vector<SimData>& report);
	
private:
	void decode_line(std::string line);
	bool can_read;

	void case_nbf(std::istringstream& data);
	void case_field(std::istringstream& data);
	void case_nbb(std::istringstream& data);
	void case_base(std::istringstream& data);
	
	void case_prospect(std::istringstream& data);
	void case_prospect_verification();
	void case_drill(std::istringstream& data);
	void case_trans(std::istringstream& data);
	void case_comm(std::istringstream& data);
	
	std::vector<Base> bases;
	std::vector<bool> active_mode;
};
#endif
