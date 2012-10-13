#include <iostream>
#include "boost/lexical_cast.hpp"

#include "managers/kernel.hpp"
#include "managers/widgets/all_widgets.hpp"

#include "3d/gl_mesh.hpp"

using namespace std;




class TestApp : public Kernel
{
public:
	TestApp( int argc, char *argv[] ) : Kernel(argc,argv) {};

	void calcFrame(TimeVal delta)//{cout << "calculating Frame. Time: " << toSeconds(delta)*1000 << " ms (= " << 1/toSeconds(delta) << " FPS)" << endl;}
	{

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


void setclock(TimeVal delta, shared_ptr<WText> wclock, Kernel& k)//{cout << "calculating Frame. Time: " << toSeconds(delta)*1000 << " ms (= " << 1/toSeconds(delta) << " FPS)" << endl;}
{
	int m = k.timeMgr->getCurMinutes();

	string min = ToString(m);
	if(m < 10) min = "0"+min;

	wclock->setText(ToString(k.timeMgr->getCurHours())+":"+min);
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

void update_screenshot(Kernel& k, WidgetPtr srcwidget, shared_ptr<WImage> wdest, TimeVal)
{
	Image dest = wdest->getImage();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	glViewport(0,0,200,200);
	glLoadIdentity();
	glOrtho(400.0f, 600, 600, 400.0f, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);

	glPushAttrib( GL_SCISSOR_BIT );

	/// todo: add glOrtho call to properly project our stuff. handle scissors correclty.

	/*

	glBindTexture(GL_TEXTURE_2D, dest.getTexture());
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, 200, 200, 0);*/
CHECK_GL_ERROR();


	GLuint fb = 0;
	glGenFramebuffers(1, &fb);
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	CHECK_GL_ERROR();

	glBindTexture(GL_TEXTURE_2D, dest.getTexture());
	CHECK_GL_ERROR();


	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 200, 200, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

CHECK_GL_ERROR();
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	CHECK_GL_ERROR();

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dest.getTexture(), 0);

	GLenum draw_buffers[2] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, draw_buffers);
	CHECK_GL_ERROR();

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw ERROR("fail", "frambuffer-thing didn't work");


	k.graphicsMgr->cls();

	//srcwidget->draw();
	k.guiMgr->drawEverything();

	CHECK_GL_ERROR();

	glBindFramebuffer(GL_FRAMEBUFFER, 0); // reset (eg. use normal screen again)

	glDeleteFramebuffers(1, &fb);

CHECK_GL_ERROR();




	glViewport(0,0,k.graphicsMgr->screen_width, k.graphicsMgr->screen_height);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	glPopAttrib();

	dest.setUV(Box(0,1,1,-1), true);

	wdest->setImage(dest);

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

		//kernel.inputMgr->addKeyListener(boost::bind(&TestApp::escapeKeyListener, &kernel, _1, _2)); ///< sollte eigentlich in TestApp hinein

		shared_ptr<WText> wclock = kernel.guiMgr->createWidget<WText>("clock");
		wclock->setText("Hallo Welt");
		wclock->setFont(kernel.graphicsMgr->loadFont("fonts/courier.ttf", 100));
		wclock->rel_x = kernel.graphicsMgr->screen_width.ref()/2  - wclock->width.ref()/2;
		wclock->rel_y = kernel.graphicsMgr->screen_height.ref()/2 - wclock->height.ref()/2;

		kernel.guiMgr->addWidget(wclock);

		kernel.setCalcFrameFunc(boost::bind(&setclock, _1, wclock, kernel));

		/*shared_ptr<WList> wcontainer = kernel.guiMgr->createWidget<WList>("a container");
		wcontainer->abs_x =  10;
		wcontainer->abs_y =  10;
		wcontainer->width = 900;
		wcontainer->height= 400;
		wcontainer->draw_bounding_box = true;


		shared_ptr<WButton> testbutton = kernel.guiMgr->createWidget<WButton>("qwer");
		testbutton->setText("#############################\nasdfsaf sdf\nsdfsafsd\nasdfsfdsf\nasdfsadfsfd\nasdf\nqwefsfe\n##############");
		//testbutton->getSlot("clicked")->connect(boost::bind(&Widget::fadeOutAndDelete, testbutton, 2, true));
		testbutton->abs_x = 300;

		Image screenshot = kernel.graphicsMgr->createNewImage(VectInt(200,200));



		shared_ptr<WImage> screenshot_widget = kernel.guiMgr->createWidget<WImage>("screenshot");
		screenshot_widget->setImage(screenshot);
		screenshot_widget->abs_x = kernel.graphicsMgr->screen_width.ref() - screenshot_widget->width.ref() - 10;
		screenshot_widget->abs_y = 100;
		screenshot_widget->draw_bounding_box = true;





		/ *testbutton->set(readXML("xml/stylesheets/button_orange.xml"));
		testbutton->setAutoHeight();
		testbutton->setAutoWidth();*/


		/*testbutton->abs_x = 300;
		testbutton->abs_y = 10;
		testbutton->width = 400;
		testbutton->height = 100;
		testbutton->draw_bounding_box = true;* /
		kernel.guiMgr->addWidget(testbutton);


		shared_ptr<WImage> wi = kernel.guiMgr->createWidget<WImage>("test");
		wi->setImage("images/testimg.png");

		shared_ptr<WImage> wi2 = kernel.guiMgr->createWidget<WImage>("test2");
		wi2->setImage("images/greenedge_shadow.png");
		//wi2->getImage().setUV(Box(10,10,45,45));
		wi2->getImage().setNinePatchData(NinePatchData(true,2,2,33,33));
		wi2->width  = 300;
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
		//wb->abs_y=wb->;
		//wb->setTripleBG("images/buttons/button1.png");

		Image bbg1 = kernel.graphicsMgr->loadImage("images/buttons/default.png");
		Image bbg2 = bbg1, bbg3 = bbg1;
		bbg1.setUV(Box(0,  0,119,58)); bbg1.setNinePatchData(NinePatchData(true, 6,6,6,6));
		bbg2.setUV(Box(0, 58,119,58)); bbg2.setNinePatchData(NinePatchData(true, 6,6,6,6));
		bbg3.setUV(Box(0,116,119,58)); bbg3.setNinePatchData(NinePatchData(true, 6,6,6,6));
		wb->setBGnormal(bbg1);
		wb->setBGhover(bbg2);
		wb->setBGactive(bbg3);

		wb->setText("just a Button :) asfasdfi aslf jasdlfij\nsecond line...");
		wb->width = 200;
		//wb->setAutoHeight(6);




		 //kernel.graphicsMgr->loadImage("images/green.png"));
		wb->getSlot("clicked")->connect(boost::bind(add_some_text, wt));


		shared_ptr<WButton> button_exit = kernel.guiMgr->createWidget<WButton>("quit");
		button_exit->setTripleBG("images/buttons/close.png");
		button_exit->setSizeToBG();
		button_exit->abs_x = kernel.graphicsMgr->screen_width .ref() - button_exit->width .ref() - 20;
		button_exit->abs_y = 20;
		button_exit->getSlot("clicked")->connect(boost::bind(&Kernel::stop, &kernel));
		button_exit->setText("quit");
		button_exit->getLabel()->cast<WText>()->setFontSize(20);
		button_exit->getLabel()->cast<WText>()->setColor(WHITE);


		wcontainer->insert(wi);
		wcontainer->insert(wi2);
		wcontainer->insert(wt);
		wcontainer->insert(wb);
		//wcontainer->hideOverflow(true);
		wcontainer->removeChild(wi2);

		shared_ptr<WImage> cont_border = kernel.guiMgr->createWidget<WImage>("container border");
		cont_border->setImage("images/edge.png");
		cont_border->getImage().setNinePatchData(NinePatchData(true,7,7,7,7));
		Image edge = cont_border->getImage();
		cont_border->abs_x  = wcontainer->abs_x.ref() - 3;
		cont_border->abs_y  = wcontainer->abs_y.ref() - 3;
		cont_border->width  = wcontainer->width.ref() + 6;
		cont_border->height = wcontainer->height.ref()+ 6;

		shared_ptr<WButton> wclose = kernel.guiMgr->createWidget<WButton>("window close");
		wclose->setTripleBG("images/cross.png",HORIZONTAL);
		wclose->width = wclose->height.ref();
		wclose->getLabel()->visible = false;
		shared_ptr<WWindow> awindow = kernel.guiMgr->createWidget<WWindow>("a window for testing");
		awindow->abs_x = 300;
		awindow->abs_y = 100;
		awindow->width = 500;
		awindow->height = 800;
		awindow->setTitleBar("images/yellow.png"); awindow->getTitleBar().setNinePatchData(NinePatchData(true,7,7,7,7));
		awindow->setBG("images/orange.png");       awindow->getBG      ().setNinePatchData(NinePatchData(true,7,7,7,7));
		awindow->setCloseButton(wclose);
		awindow->border = 5;
		awindow->setTitle("hallo welt: this is a Window!");
		awindow->getTitleWidget()->cast<WText>()->setColor(Color("black"));
		awindow->getTitleWidget()->cast<WText>()->setFontSize(20);

		//awindow->getContainer()->hideOverflow(false);
		wi2->abs_x = awindow->abs_x.ref() - 100;

		shared_ptr<WFileTree> wfiletree = kernel.guiMgr->createWidget<WFileTree>("asdf");
		wfiletree->setRoot("/home/samuel/");
		awindow->getContainer()->insert(wfiletree);
		//awindow->getContainer()->insert(wi2);


		shared_ptr<WImage> testimg2 = kernel.guiMgr->createWidget<WImage>("asdfycxvwev");
		testimg2->setImage("images/anathea_icon.png");
		awindow->getContainer()->addChild(testimg2);
		testimg2->rel_x = 200;





		Mesh m;
		m.load_from_STL("replicator_grill.stl");

		shared_ptr<WGLViewport> testviewport = kernel.guiMgr->createWidget<WGLViewport>("an opengl viewport");
		testviewport->addMesh(m);
		testviewport->width  = 300;
		testviewport->height = 300;
		testviewport->rel_x = 100;
		testviewport->rel_y = kernel.graphicsMgr->screen_height.ref() - testviewport->height.ref() - 100;
		testviewport->draw_bounding_box = true;
		testviewport->setBackground(Color(A_OPAQUE, A_TRANSPARENT, A_TRANSPARENT, A_HALF_TRANSPARENT));
		testviewport->render();





		kernel.guiMgr->addWidget(testviewport);
		kernel.guiMgr->addWidget(awindow);
		kernel.guiMgr->addWidget(wcontainer);
		kernel.guiMgr->addWidget(button_exit);
		kernel.guiMgr->addWidget(screenshot_widget);


		kernel.guiMgr->createPopupOK("popup test\nlet0s see if this still works...");



		testbutton->getSlot("clicked")->connect(boost::bind(&Widget::moveAnim, awindow, Vect(500,300), 2));


		kernel.setCalcFrameFunc(boost::bind(&update_screenshot, kernel, awindow, screenshot_widget, _1));


		//kernel.guiMgr->createWidgetsFromXML("xml/layout1.xml");*/
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
