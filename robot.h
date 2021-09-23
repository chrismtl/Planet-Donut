//~ Jean-Claude Rihani 325233
//~ Christian Matala-Tala 327317

#ifndef ROBOT_H
#define ROBOT_H

#include <vector>
#include <iostream>
#include <ostream>
#include <fstream>
#include "geomod.h"
#include "field.h"

enum ControlMode {REMOTE, AUTONOMOUS};

enum RobotType{RB_PROSPECT, RB_DRILL, RB_TRANSPORT, RB_COMM};

class Robot
{
public:
	Robot(const Robot* autre);
	Point getPos() const;
	unsigned int getUid() const;
	double getDp() const ;
	void resetDp();
	size_t getColor() const;
	bool getReached() const;
	virtual size_t getType() const = 0;
	ControlMode getControlMode();
	void setControlMode(ControlMode control_);
	
	virtual Robot* copy() = 0;
	virtual void draw_robot() const = 0;
	void draw_link();
	void draw_range();
	virtual void write(std::ofstream& out) = 0;
	virtual ~Robot();
	
	void add_neighbors(Robot* neighbor_robot);
	void clear_neighbors();
	
	void connexion(std::vector<Robot*>& network);
	
	virtual void move_autonomous() = 0;
	
	void go_to(Point tar);
	void setTarget(Point new_target);
	Point getTarget() const;
	
protected:
	Robot(int uid = 0, size_t color = 0, double dp = 0, Point pos = {}, 
		  Point target = {}, Point base_center = {}, bool reached = false);
	
	int uid;
	size_t color;
	double dp;
	Point pos;
	Point target;
	Point my_base;
	bool reached;
	std::vector<Robot*> neighbors;
	ControlMode control_mode;
};

class Prospect:public Robot
{
public:
	Prospect(int uid = 0, size_t color = 0, double dp = 0, Point pos = {}, 
			 Point target = {}, Point base = {},
			 bool reached = false, bool back_home = false, bool found = false);
	Prospect(const Prospect* autre); 
	Prospect(int uid, size_t color, double dp, Point pos, 
			 Point target, Point base, 
			 bool reached, bool back_home, bool found, Field found_field);
	
	Prospect* copy() override;
	size_t getType() const override;
	void draw_robot() const override;
	void write(std::ofstream& out) override;
	
	void move_autonomous() override;
	
	bool getBackHome() const ;
	void resetBackHome();
	
	Field get_my_field();
	void add_field(std::vector<Field>& base_fields);
	bool getFound() const;
	bool getLowDp() const;
	~Prospect();
	
private:
	bool back_home;
	bool found;
	Field found_field;
	bool low_dp;
};

class Transport:public Robot
{
public:
	Transport(int uid = 0, size_t color = 0, double dp = 0, Point pos = {}, 
			  Point target = {}, Point base = {},
			  bool reached = false, bool filled = false);
	Transport(const Transport* autre);
	Transport* copy() override;
	size_t getType() const override;
	void draw_robot() const override;
	void write(std::ofstream& out) override;
	
	void move_autonomous() override;
	
	bool is_full() const;
	void fill_resource(double resource);
	double load_resource();
	
	bool getFilled() const;
	
	~Transport();

private:
	bool filled;
	double tank;
};

class Drill:public Robot
{
public:
	Drill(int uid = 0, size_t color = 0, double dp = 0, Point pos = {}, 
		  Point target = {}, Point base = {}, bool reached = false,
		  std::vector<Transport*> transports = {});
	Drill(const Drill* autre);
	Drill* copy() override;
	size_t getType() const override;
	void draw_robot() const override;
	void write(std::ofstream& out) override;
	
	void move_autonomous() override;
	
	void add_transport(Transport* transport);
	void add_field(std::vector<Field>& base_fields);
	
	bool getFound();
	~Drill();
private:
	bool found;
	std::vector<Transport*> transports;
};

class Comm:public Robot
{
public:
	Comm(int uid = 0, size_t color = 0, double dp = 0, Point pos = {}, 
		 Point target = {}, Point base = {}, bool reached = false);
	Comm(const Comm* autre);
	Comm* copy() override;
	size_t getType() const override;
	void draw_robot() const override;
	void write(std::ofstream& out) override;
	
	void move_autonomous() override;
	
	~Comm();
};
#endif
