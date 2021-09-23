//~ Jean-Claude Rihani 325233
//~ Christian Matala-Tala 327317

#include <iostream>
#include <chrono>
#include <thread>
#include <string>
#include <fstream>
#include <vector>
#include <cairomm/context.h>
#include <bits/stdc++.h>
#include <gtkmm/button.h>

#include "gui.h"
#include "graphic.h"

#define GTK_COMPATIBILITY_MODE
#ifdef GTK_COMPATIBILITY_MODE

static Simulation simulation;
static std::vector<SimData> report;
static bool show_range (false);
static bool show_link (false);
static bool opening(false);

namespace Gtk
{
	template<class T, class... T_Args>
	auto make_managed(T_Args&&... args) -> T*
	{
		return manage(new T(std::forward<T_Args>(args)...));
	}
}
#endif

constexpr unsigned max_tab(10);

///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////Map Area/////////////////////////////////////////

MapArea::MapArea()
{
	WP prm;

	prm.XMIN = -dim_max;
	prm.XMAX = dim_max;
	prm.YMIN = -dim_max;
	prm.YMAX = dim_max;

	setWParam(prm);
}

MapArea::~MapArea(){}

void MapArea::setWParam(WP x) 
{
	prm_area = x;
}

void MapArea::update_show_link()
{
	show_link = !show_link;
	refresh();
}

void MapArea::update_show_range()
{
	show_range = !show_range;
	refresh();
}

void MapArea::update_opening_file()
{
	opening_file = !opening_file;
}

void MapArea::clear()
{
	can_draw = false;
	refresh();
}

void MapArea::refresh()
{
	auto win = get_window();
	if(win)
	{
		Gdk::Rectangle r(0,0, get_allocation().get_width(),
						 get_allocation().get_height());
		win->invalidate_rect(r,false);
	}
}

void MapArea::set_can_draw(bool could_read)
{
	if(could_read) can_draw = true;
	else
	{
		can_draw = false;
	}
}

bool MapArea::get_can_draw() const
{
	return can_draw;
}
 

bool MapArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
	if(!opening)
	{
		Gtk::Allocation allocation = get_allocation();
		const int width = allocation.get_width();
		const int height = allocation.get_height();
		
		int size(0);
		if(width >= height) size = height;
		else size = width;
		
		
		graphic_set_context(cr);
		
		on_draw_1(cr, width, height, size);
		
		if(can_draw)
		{
			simulation.draw();
			
			if(show_link) 
			{
				simulation.draw_link();
			}
			if(show_range) 
			{
				simulation.draw_range();
			}
		}
		
		on_draw_2(cr, width, height, size);
		
		return true;
	}
	return false;
}

void MapArea::on_draw_1(const Cairo::RefPtr<Cairo::Context>& cr, const int width, 
						const int height, int size)
{
	//~ Draw white background
	cr->save();
	cr->set_source_rgb(1, 1, 1);
	cr->set_line_width(height);
	cr->move_to(0, height/2);
	cr->line_to(width, height/2);
	cr->stroke();
	cr->close_path();
	cr->restore();
	
	//~ Draw purple frame
	graphic_draw_frame(size/2, size/2, size, size);
	
	//~ Scale window to prevent distorsion
	cr->translate(size/2, size/2);
	
	std::cout << "===============================================================" << std::endl;
	std::cout << "size          : " << size          << std::endl;
	std::cout << "prm_area.XMAX : " << prm_area.XMAX << std::endl;
	std::cout << "prm_area.XMIN : " << prm_area.XMIN << std::endl;
	std::cout << "scale1        : " << size/(prm_area.XMAX - prm_area.XMIN) << std::endl;
	std::cout << "scale2        : " << size/(prm_area.YMAX - prm_area.YMIN) << std::endl;
	
	cr->scale(size/(prm_area.XMAX - prm_area.XMIN),
					-size/(prm_area.YMAX - prm_area.YMIN));
}

void MapArea::on_draw_2(const Cairo::RefPtr<Cairo::Context>& cr, const int width, 
						const int height, int size)
{
	//~ Draw two grey screens to keep drawings visible only in m_MapArea
	cr->translate(0, 0);
	cr->scale((prm_area.XMAX - prm_area.XMIN)/size,
			  -(prm_area.YMAX - prm_area.YMIN)/size);
	
	cr->save();
	cr->set_source_rgb(242.0/255, 241.0/255, 240.0/255);
	cr->set_line_width(height+2);
	cr->move_to(size/2, 0);
	cr->line_to(width, 0);
	cr->stroke();
	cr->close_path();
	cr->restore();
	
	cr->save();
	cr->set_source_rgb(242.0/255, 241.0/255, 240.0/255);
	cr->set_line_width(width+2);
	cr->move_to(0, size/2);
	cr->line_to(0, height);
	cr->stroke();
	cr->close_path();
	cr->restore();
}

///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////GUI//////////////////////////////////////////////

void Gui::open_file(std::string file_name)
{
	//Display file_name in terminal
	size_t slash = file_name.find_last_of("/")+1;
	size_t dot = file_name.find(".");
	size_t len = dot-slash;
	std::cout<<"Opening "<<file_name.substr(slash,len)<<std::endl;
	
	bool could_read(simulation.read(file_name));
	
	m_MapArea.set_can_draw(could_read);
	
	if(could_read) m_Button_Toggle_Link.set_active(true);
	
	tree_view_update();
	m_MapArea.refresh();
}

Gui::Gui() :
	main_Box   (Gtk::ORIENTATION_VERTICAL,   8),
	top_Box	(Gtk::ORIENTATION_HORIZONTAL, 8),
	side_Box   (Gtk::ORIENTATION_VERTICAL,   8),
	map_Box    (Gtk::ORIENTATION_HORIZONTAL, 8),
	bottom_Box (Gtk::ORIENTATION_VERTICAL,   8),
	general_Box(Gtk::ORIENTATION_VERTICAL,   8),
	toggle_Box (Gtk::ORIENTATION_VERTICAL,   8),
	m_Frame_General("General"),
	m_Frame_Toggle ("Toogle display"),
	m_Button_Exit("exit", 10), m_Button_Open("Open", 10), m_Button_Save("save", 10),
	m_Button_Start("start", 10), m_Button_Step("Step", 10),
	m_Button_Toggle_Link("Toggle link", 10), m_Button_Toggle_Range("Toggle range", 100)
{
	set_title("Planet Donut");
	set_border_width(0);
	m_Button_Exit.signal_clicked().connect(sigc::mem_fun(*this,
										   &Gui::on_button_clicked_exit));
	m_Button_Open.signal_clicked().connect(sigc::mem_fun(*this,
										   &Gui::on_button_clicked_open));
	m_Button_Save.signal_clicked().connect(sigc::mem_fun(*this,
										   &Gui::on_button_clicked_save));
	m_Button_Start.signal_clicked().connect(sigc::mem_fun(*this,
										   &Gui::on_button_clicked_start));
	m_Button_Step.signal_clicked().connect(sigc::mem_fun(*this,
										   &Gui::on_button_clicked_step));
	m_Button_Toggle_Link.signal_clicked().connect(sigc::mem_fun(*this,
										   &Gui::on_button_clicked_link));
	m_Button_Toggle_Range.signal_clicked().connect(sigc::mem_fun(*this,
										   &Gui::on_button_clicked_range));
	//~ Add outer box to the window
	add(main_Box);
	//~ Put the inner boxes
	main_Box.pack_end(bottom_Box, false, true);
	main_Box.pack_start(top_Box);
	//~ Fill top box with side Box and map Box
	top_Box.pack_start(side_Box, false, true);
	top_Box.pack_start(map_Box);
	//~ Fill side Box with Frames
	side_Box.pack_start(m_Frame_General, false, false);
	side_Box.pack_start(m_Frame_Toggle, false, false);
	//~ Fill Frames with Boxes
	m_Frame_General.add(general_Box);
	m_Frame_Toggle.add(toggle_Box);
	//~ Fill Boxes with buttons
	general_Box.pack_start(m_Button_Exit, false, false);
	general_Box.pack_start(m_Button_Open, false, false);
	general_Box.pack_start(m_Button_Save, false, false);
	general_Box.pack_start(m_Button_Start,false, false);
	general_Box.pack_start(m_Button_Step, false, false);
	toggle_Box.pack_start(m_Button_Toggle_Link);
	toggle_Box.pack_start(m_Button_Toggle_Range);
	//~ Fill map Box with map
	m_MapArea.set_size_request(500, 500);
	map_Box.pack_start(m_MapArea);
	//~ Fill bottom Box with scrolled Window
	bottom_Box.add(_scrolled_window);
	_scrolled_window.set_size_request(-1, 200);
	_scrolled_window.add(_tree_view);
	_scrolled_window.set_policy(Gtk::PolicyType::POLICY_AUTOMATIC,
								Gtk::PolicyType::POLICY_AUTOMATIC);
	_scrolled_window.set_hexpand();
	_tree_view.append_column("Uid", _columns._col_Uid);
	_tree_view.append_column("nbP", _columns._col_nbP);
	_tree_view.append_column("nbF", _columns._col_nbF);
	_tree_view.append_column("nbT", _columns._col_nbT);
	_tree_view.append_column("nbC", _columns._col_nbC);
	_tree_view.append_column_numeric("Amount resource",
									  _columns._col_resource, "%.2f");
	auto cell = Gtk::make_managed<Gtk::CellRendererProgress>();
	int cols_count = _tree_view.append_column("Mission completeness", *cell);
	auto progress_col = _tree_view.get_column(cols_count - 1);
	if(progress_col)
	progress_col->add_attribute(cell->property_value(),
								_columns._col_resource_percentage);
	// Events from keyboard
	add_events(Gdk::KEY_RELEASE_MASK);
	//~ Show all children
	show_all_children();
}

Gui::~Gui()
{}

void Gui::read_file(char* filename)
{
	simulation.read(filename);
	m_MapArea.refresh();
	tree_view_update();
}

void Gui::refresh()
{
	m_MapArea.refresh();
	simulation.start();
	tree_view_update();
}

bool Gui::on_key_press_event(GdkEventKey * key_event)
{
	if(key_event->type == GDK_KEY_PRESS)
	{
		switch(gdk_keyval_to_unicode(key_event->keyval))
		{
			case '1':
				on_button_clicked_step();
				return true;
			case 's':
				on_button_clicked_start();
				break;
			case 'q':
				on_button_clicked_exit();
				break;
			default:
				break;
		}
	}
	
	return Gtk::Window::on_key_press_event(key_event);
}

void Gui::on_button_clicked_exit()
{
	system("clear");
	exit(0);
}

void Gui::on_button_clicked_open()
{
	opening = true;
	bool was_started = start;
	if(was_started)
		on_button_clicked_start();
	
	Gtk::FileChooserDialog dialog("Please choose a file",
	Gtk::FILE_CHOOSER_ACTION_OPEN);
	dialog.set_transient_for(*this);
	dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
	dialog.add_button("_Open", Gtk::RESPONSE_OK);
	int result = dialog.run();
	switch(result)
	{
		case(Gtk::RESPONSE_OK):
		{
			std::string filename = dialog.get_filename();
			Simulation simulation2;
			simulation = simulation2;
			simulation.reset_data();
			std::vector<char> cstr(filename.c_str(), 
								   filename.c_str() + filename.size() + 1);
			open_file(&filename[0]);
			break;
		}
		case(Gtk::RESPONSE_CANCEL):
		{
			if(was_started)
				on_button_clicked_start();
			break;
		}
		default: break;
	}
	opening = false;
}

void Gui::on_button_clicked_save()
{
	opening = true;
	bool was_started = start;
	if(was_started)
		on_button_clicked_start();
		
	Gtk::FileChooserDialog dialog("Please choose a file",
								  Gtk::FILE_CHOOSER_ACTION_SAVE);
	dialog.set_transient_for(*this);
	dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
	dialog.add_button("_Save", Gtk::RESPONSE_OK);
	
	int result = dialog.run();
	
	switch(result)
	{
		case(Gtk::RESPONSE_OK):
		{
			std::ofstream file(dialog.get_filename());
			
			if(!file.fail()) simulation.export_current_map(file);
			file.close();
			break;
		}
		case(Gtk::RESPONSE_CANCEL):
		{
			if(was_started)
				on_button_clicked_start();
			break;
		}
		default: break;
	}
	opening = false;
}

void Gui::on_button_clicked_start()
{
	start = !start;
	if(start)
	{
		m_Button_Start.set_label("stop");
		Glib::signal_idle().connect( sigc::mem_fun(*this, &Gui::on_idle));
	}
	else
		m_Button_Start.set_label("start");
}

void Gui::on_button_clicked_step()
{
	if(!start) step = true; 
	if(step)
		Glib::signal_idle().connect(sigc::mem_fun(*this, &Gui::on_idle));
}

void Gui::on_button_clicked_link()
{
	m_MapArea.update_show_link();
}

void Gui::on_button_clicked_range()
{
	m_MapArea.update_show_range();
}

bool Gui::on_idle()
{
	if(start)
	{
		refresh();
		std::this_thread::sleep_for(std::chrono::milliseconds(25));
	}
	else if(step)
	{
		step = false;
		refresh();
	}
	return true;  
}

void Gui::tree_view_update()
{
	Glib::RefPtr<Gtk::ListStore> ref_tree_model = Gtk::ListStore::create(_columns);
	_tree_view.set_model(ref_tree_model);
	
	if(true)
	{
		simulation.fill_bases_data(report);
		
		for(size_t i = 0 ; i <report.size() ; ++i)
		{
			auto row = *(ref_tree_model->append());
			row[_columns._col_Uid] = report[i].Uid;
			row[_columns._col_nbP] = report[i].nbP;
			row[_columns._col_nbF] = report[i].nbF;
			row[_columns._col_nbT] = report[i].nbT;
			row[_columns._col_nbC] = report[i].nbC;
			row[_columns._col_resource] = report[i].ressource;
			row[_columns._col_resource_percentage] = report[i].ressource_p;
		}
	}
}
