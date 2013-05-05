#include <iostream>
#include <stdlib.h>

#include "anathea/managers/kernel.hpp"
#include "anathea/managers/widgets/all_widgets.hpp"


using namespace std;




class TestApp : public Kernel
{
	shared_ptr<WText> wclock;

public:
	TestApp( int argc, char *argv[] ) : Kernel(argc,argv) {};

	void init()
	{
		Kernel::init();
		this->inputMgr->addKeyListener(boost::bind(&TestApp::escapeKeyListener, this, _1, _2));

		wclock = this->guiMgr->createWidget<WText>("clock");
		wclock->setText("Hallo Welt");
		wclock->setFont(this->graphicsMgr->loadFont("fonts/FreeMono.ttf", 100));
		wclock->rel_x = this->graphicsMgr->screen_width.ref()/2  - wclock->width.ref()/2;
		wclock->rel_y = this->graphicsMgr->screen_height.ref()/2 - wclock->height.ref()/2;

		this->guiMgr->addWidget(wclock);
	}

	void calcFrame(TimeVal delta)
	{
		// cout << "calculating Frame. Time: " << toSeconds(delta)*1000 << " ms (= " << 1/toSeconds(delta) << " FPS)" << endl;}
		int m = this->timeMgr->getCurMinutes();

		string minutes = ToString(m);
		if(m < 10) minutes = "0"+minutes;

		wclock->setText(ToString(this->timeMgr->getCurHours())+":"+minutes);
	}

	void escapeKeyListener(KEY key, bool state)
	{
		if(state) /// only react to key-DOWN events
		{
			switch(key)
			{
			case KEY_ESCAPE:
				this->stop();
				break;

			case KEY_b:
				this->guiMgr->draw_bounding_boxes = !this->guiMgr->draw_bounding_boxes;
				break;

			default:
				break;
			}
		}
	}
};



int main(char argc, char *argv[])
{
	try
	{
		TestApp kernel(argc,argv);

		kernel.init();
		kernel.run();

		return EXIT_SUCCESS;
	}
	catch(ticpp::Exception& e)
	{
		cerr << "TinyXML++ caused an EXCEPTION: " << e.what() << endl;
	}
	catch(CriticalError& e)
	{
		cerr << "CRITICAL EXCEPTION: " << e.what() << endl;
	}
	catch(std::exception& e)
	{
		cerr << "EXCEPTION: " << e.what() << endl;
	}
	catch(...)
	{
		cerr << "UNDEFINED ERROR" << endl;
	}

	cout << endl << "this was a critical error..." << endl;
	cout << endl << "Press ENTER to quit" << endl;
	waitForEnter();

	return EXIT_FAILURE;
}
