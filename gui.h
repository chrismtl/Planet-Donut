//~ Jean-Claude Rihani 325233
//~ Christian Matala-Tala 327317

#ifndef GTKMM_EXAMPLE_GUI_H
#define GTKMM_EXAMPLE_GUI_H

#include <gtkmm.h>
#include <gtkmm/application.h>
#include <gtkmm/window.h>
#include <gtkmm/drawingarea.h>

#include "constantes.h"
#include "simulation.h"
#include "geomod.h"

struct WP // window parameters
{
	double XMIN; 
	double XMAX;
	double YMIN;
	double YMAX;
	int HEIGHT;
	double ASP;
};

class MapArea : public Gtk::DrawingArea
{
public:
	MapArea();
	virtual ~MapArea();
	void setWParam(WP x);
	void update_show_link();
	void update_show_range();
	void update_opening_file();
	void clear();
	void refresh();
	void set_can_draw(bool could_read);
	bool get_can_draw() const;
private:
	WP prm_area_ref;
	WP prm_area;
protected:
	//Override default signal handler:
	bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
	void on_draw_1(const Cairo::RefPtr<Cairo::Context>& cr, const int width, 
				   const int height, int size);
	void on_draw_2(const Cairo::RefPtr<Cairo::Context>& cr, const int width, 
				   const int height, int size);
	bool can_draw = true;
	bool opening_file = false;
};

class Gui : public Gtk::Window
{
public:
	Gui();
	virtual ~Gui(); 
	void read_file(char* filename);
	void open_file(std::string file_name);

protected:
	//~ BOXES=====================
	Gtk::Box main_Box, top_Box, side_Box, map_Box, bottom_Box;
	Gtk::Box general_Box, toggle_Box;
	void tree_view_update();
	void refresh();
	
	//~ FRAMES====================
	Gtk::Frame m_Frame_General, m_Frame_Toggle;
	
	//~ BUTTONS===================
	Gtk::Button m_Button_Exit, m_Button_Open, m_Button_Save;
	Gtk::Button m_Button_Start,	m_Button_Step;
	Gtk::ToggleButton m_Button_Toggle_Link, m_Button_Toggle_Range;
	
	//~ SIGNAL HANDLERS===========
	void on_button_clicked_exit() ;
	void on_button_clicked_open() ;
	void on_button_clicked_save() ;
	void on_button_clicked_start();
	void on_button_clicked_step() ;
	void on_button_clicked_link() ;
	void on_button_clicked_range();
	
	bool on_idle();
	
	bool on_key_press_event(GdkEventKey * key_event);
	
	class Model_columns : public Gtk::TreeModel::ColumnRecord
	{
	public:
		Model_columns()
		{
			add(_col_id);
			add(_col_Uid);
			add(_col_nbP);
			add(_col_nbF);
			add(_col_nbT);
			add(_col_nbC);
			add(_col_resource);
			add(_col_resource_percentage);
		}

		Gtk::TreeModelColumn<int> _col_id;
		Gtk::TreeModelColumn<int> _col_Uid;
		Gtk::TreeModelColumn<int> _col_nbP;
		Gtk::TreeModelColumn<int> _col_nbF;
		Gtk::TreeModelColumn<int> _col_nbT;
		Gtk::TreeModelColumn<int> _col_nbC;
		Gtk::TreeModelColumn<double> _col_resource;
		Gtk::TreeModelColumn<int> _col_resource_percentage;
	};
	
	Model_columns _columns;
	MapArea m_MapArea;
	
	Gtk::ScrolledWindow _scrolled_window;
	Gtk::TreeView _tree_view;

	unsigned count = 0;
	bool start = false;
	bool step = false;
};
#endif
