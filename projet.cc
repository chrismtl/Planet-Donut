//~ Jean-Claude Rihani 325233
//~ Christian Matala-Tala 327317

#include <iostream>
#include <vector>
#include <gtkmm.h>

#include "simulation.h"
#include "gui.h"

using namespace std;

int main(int argc, char * argv[])
{
	system("clear");
	auto app = Gtk::Application::create("org.gtkmm.example");
	Gui window;
	
	//~ Open file name if given in terminal
	if(argc - 1)
	{
		string file_name;
		file_name = argv[1];
		window.open_file(file_name);
	}
	
	return app->run(window);
}
