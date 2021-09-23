//~ Jean-Claude Rihani 325233
//~ Christian Matala-Tala 327317

#ifndef BASE_H
#define BASE_H

#include <vector>
#include <memory>
#include "field.h"
#include "geomod.h"
#include "robot.h"

class Base
{
public:
	Base(Point pos, double resource, std::vector<Base>& bases, bool& can_read);
	Base (const Base& autre);
	
	~Base();
	
	void newProspect	(unsigned int uid, double dp, Point pos, Point target, 
						 bool reached, bool back_home, bool found, bool& can_read);
						
	void newProspect	(unsigned int uid, double dp, Point pos, Point target,
						 bool reached, bool back_home, bool found, Field found_field,
						 bool& can_read);
						
	void newDrill		(unsigned int uid, double dp, Point pos, Point target,
						 bool reached, bool& can_read);
						 
	void newTransport	(unsigned int uid, double dp, Point pos, Point target,
						 bool reached, bool back_home, bool& can_read);
						
	void newComm		(unsigned int uid, double dp, Point pos, Point target,
						 bool reached, bool& can_read);
	
	bool validity();
	
	void draw_base();
	void draw_link();
	void draw_range();
	
	double getResource() const;
	
	void fill_nb_data(std::vector<unsigned int>& nb_data);
	void fill_res_data(std::vector<double>& res_data);
	
	void write_pos(std::ofstream& out);
	void write_robots(std::ofstream& out);
	
	//~ Algorithm methods
	void update_neighbors(const Base& base_2);
	void clear_neighbors();
	
	void connexion();
	void clear_network();
	
	void receive_resource();
	void maintenance();
	
	
	void get_field_info();
	void give_field_score();
	std::vector<int> transports_on(Circle circle);
	
	void creation();
	
	void update_remote();
	void update_autonomous();
	
	void clear_info();
	
	void check_destruction(size_t i, std::vector<bool>& active_mode);

private:
	void add_transport_drill(unsigned int uid_drill, unsigned int uid_transport);
	bool inter(const Base& base) const;
	bool uids_validity(unsigned int uid);
	unsigned int next_uid();
	
	bool is_new(Field new_field);
	bool on_spot(Point target);
	
	void createProspect();
	void createDrill(unsigned int uid, size_t color, double dp,
					 Point pos, Point target);
	void createTransport(unsigned int uid, size_t color, double dp,
						 Point pos, Point target);
	void createComm(Point target);
	
	void creation_prosp(size_t& count);
	
	void creation_drill(size_t& count, std::vector<bool>& nbD_my_fields);
	
	void creation_trans(size_t& count, std::vector<int>& nbT_my_fields);
	void check_create_trans(size_t& i, size_t& count,
							std::vector<int>& nbT_my_fields, const Point& target);
	
	void creation_com(size_t& count);
	void upgrade_grid(size_t& count,
					  std::vector<size_t>& not_placed, std::vector<size_t>& placed);
	bool spot_taken(Point& new_spot, std::vector<size_t>& placed);
	void create_new_spot(int& i, int& j, Point& base, Point& new_spot,
						 size_t& count,
						 std::vector<size_t>& placed, std::vector<size_t>& not_placed);
	
	Circle circle;
	bool base_com;
	double resource;
	size_t color;
	unsigned int my_uid;
	std::vector<unsigned int> uids;
	
	std::vector<Robot*> all_robots;
	
	std::vector<Prospect*>	robots_prospect;
	std::vector<Drill*>		robots_drill;
	std::vector<Transport*> robots_transport;
	std::vector<Comm*>		robots_comm;
	
	std::vector<Robot*>		network;
	
	std::vector<Field> my_fields;
};

#endif



