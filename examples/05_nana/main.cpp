#include <nana/gui.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/button.hpp>

#include <windows.h>
#include <commctrl.h>
#include <cassert>

int main()
{
	//Define a form.
	nana::form fm(nana::API::make_center(600, 790), nana::appear::decorate<nana::appear::sizable>());

	//Define a label and display a text.
	nana::label lab{fm, "Hello, Nana C++ Library"};
	lab.format(true);

	//Define a button and answer the click event.
	nana::button btn{fm, "Quit"};
	btn.events().click([&fm]{
		fm.close();
	});

	//Layout management
	fm.div("vert <><<><weight=80% text><>><><weight=24<><button><>><>");
	fm["text"]<<lab;
	fm["button"] << btn;
	fm.collocate();

	//Show the form
	fm.show();

	//Start to event loop process, it blocks until the form is closed.
	nana::exec();
}
