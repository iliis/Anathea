#include <iostream>
#include "boost/lexical_cast.hpp"

#include "managers/kernel.hpp"
#include "managers/widgets/widget_image.hpp"
#include "managers/widgets/widget_text.hpp"
#include "managers/widgets/widget_button.hpp"
#include "managers/widgets/widget_list.hpp"

using namespace std;


class TestApp : public Kernel
{
public:
	TestApp( int argc, char *argv[] ) : Kernel(argc,argv) {};

	void calcFrame(TimeVal delta){cout << "calculating Frame. Time: " << toSeconds(delta)*1000 << " ms (= " << 1/toSeconds(delta) << " FPS)" << endl;}

	void escapeKeyListener(KEY key, bool state)
	{
		if(key == KEY_ESCAPE and state)
			this->stop();
	}
};

class TestTexture
{
	string name;
public:
	TestTexture(string name) : name(name) {cout << "new texture " << name << " created" << endl;};
	~TestTexture(){cout << "texture " << name << " deleted" << endl;}

	string getName() const {return name;}
	void   setName(string n){this->name = n;}
};


class TestImg
{
	TestTexture* p_texture;
	int* counter;

	void release()
	{
		(*counter) --;
		if(*counter == 0)
		{
			delete counter;
			if(p_texture) delete p_texture;
		}

		counter   = NULL;
		p_texture = NULL;
	}
public:
	TestImg()            : p_texture(NULL), counter(new int(1)) {};
	TestImg(string name) : p_texture(new TestTexture(name)), counter(new int(1)) {};

	TestImg(const TestImg& img) : p_texture(img.p_texture), counter(counter) {++*counter;};

	void operator=(const TestImg& img)
	{
		if(img.p_texture != this->p_texture)
		{
			release();
			this->p_texture = img.p_texture;
			this->counter   = img.counter;
			++*counter;
		}
	}

	~TestImg()
	{
		release();
	}

	void setNull()
	{
		release();
		this->counter = new int(1);
	}

	const TestTexture& get(){assert(p_texture); return *p_texture;}
	void print(){cout << *counter << "  '" << (p_texture?p_texture->getName():" NULL ") << "'" << endl;}

	const int& getCount(){return *counter;}
};


void add_some_text(shared_ptr<WText> w)
{
	w->setText(w->getText()+"\nanother line");
};

#include <limits.h>
using boost::property_tree::ptree;

int
main(int argc, char *argv[])
{
	try
	{
		cout << "Hallo Welt. This is Anathea 1.7." << endl;

		TestApp kernel(argc,argv);
		kernel.init();

		kernel.inputMgr->addKeyListener(boost::bind(&TestApp::escapeKeyListener, &kernel, _1, _2)); ///< sollte eigentlich in TestApp hinein


		shared_ptr<WList> wcontainer = kernel.guiMgr->createWidget<WList>("a container");
		wcontainer->abs_x =  10;
		wcontainer->abs_y =  10;
		wcontainer->width = 900;
		wcontainer->height= 400;
		wcontainer->draw_bounding_box = true;


		shared_ptr<WImage> wi = kernel.guiMgr->createWidget<WImage>("test");
		wi->setImage("images/testimg.png");

		shared_ptr<WImage> wi2 = kernel.guiMgr->createWidget<WImage>("test2");
		wi2->setImage("images/greenedge_shadow.png");
		//wi2->getImage().setUV(Box(10,10,45,45));
		wi2->getImage().setNinePatchData(NinePatchData(true,2,2,33,33));
		wi2->width = 300;
		wi2->height = 100;

		wi->setImage("images/anathea_icon.png");

		shared_ptr<WText> wt = kernel.guiMgr->createWidget<WText>("foobar");

		wt->setText("Hallo Welt asf asdf asdf \na sdfiweiofasjod f\nthis is the third line :)\nsome chars:*ç%&/()=?\nmore rubbish... nor not");
		wt->abs_x = 10;
		wt->abs_y = wi->height.ref()+10;

		wi2->abs_x = 20;
		wi2->abs_y = wi->height.ref()+wt->height.ref()+20;

		shared_ptr<WButton> wb = kernel.guiMgr->createWidget<WButton>("just a button");
		wb->abs_x=wi->width.ref()+20;
		wb->abs_y=20;
		wb->setText("just a Button :) asfasdfi aslf jasdlfij\nsecond line...");
		wb->setTripleBG("images/buttons/button1.png");
		 //kernel.graphicsMgr->loadImage("images/green.png"));
		wb->getSlot("clicked")->connect(boost::bind(add_some_text, wt));


		shared_ptr<WButton> button_exit = kernel.guiMgr->createWidget<WButton>("quit");
		button_exit->setTripleBG("images/buttons/close.png");
		button_exit->abs_x = kernel.graphicsMgr->screen_width .ref() - button_exit->width .ref() - 20;
		button_exit->abs_y = 20;
		button_exit->getSlot("clicked")->connect(boost::bind(&Kernel::stop, &kernel));

		wcontainer->insert(wi);
		wcontainer->insert(wi2);
		wcontainer->insert(wt);
		wcontainer->insert(wb);
		wcontainer->hideOverflow(true);
		wcontainer->removeChild(wi2);

		//kernel.guiMgr->createWidgetsFromXML("xml/layout1.xml");
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
