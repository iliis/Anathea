#include "managers/kernel.hpp"


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Kernel::Kernel(int argc, char *argv[])
 :	running(false), init_complete(false)
{
	fs::path working_dir  = fs::current_path();
	fs::path settings_xml = working_dir/"xml"/"config.xml";

	if(argc > 1)
	{
		for(int i=1;i<argc;++i)
		{
			if(strcmp(argv[i], "--working-dir") and i+1<argc)
				working_dir = argv[i+1];

			if(strcmp(argv[i], "--config") and i+1<argc)
				settings_xml = argv[i+1];
		}
	}

	/// hardcodete Default-Werte
	this->settings.appname = "ANATHEA 1.7";
	this->settings.screen.w = 1024;
	this->settings.screen.h =  768;
	this->settings.screen.bpp =  0; ///< benutze momentanen Wert
	this->settings.screen.fullscreen	= false;
	this->settings.screen.doublebuf	= true;
	this->settings.screen.resizable	= false;
	this->settings.screen.background	= BLACK; // /// ACHTUNG: Wird so oder so überschrieben (siehe ein paar Zeilen weiter unten (50))
	this->settings.screen.maxFPS		= 0; ///< deaktiviert

	//this->settings.styles.widgets = working_dir/"xml"/"stylesheets"/"widgets.xml";
	//this->settings.styles.sprites = working_dir/"xml"/"stylesheets"/"sprites.xml";

	/// überschreibe Werte mit denen der config-Datei
	this->settings.file = settings_xml;
	this->settings.pt   = readXML(settings_xml);
	ptree s = this->settings.pt;

	if(s.get_child_optional("misc"))
	{
		this->settings.appname = s.get("misc.appname", this->settings.appname);
	}

	if(s.get_child_optional("screen"))
	{
		this->settings.screen.w   = s.get("screen.w",   this->settings.screen.w);
		this->settings.screen.h   = s.get("screen.h",   this->settings.screen.h);
		this->settings.screen.bpp = s.get("screen.bpp", this->settings.screen.bpp);

		setBoolFromPT(s.get_child("screen"), "fullscreen", this->settings.screen.fullscreen);
		setBoolFromPT(s.get_child("screen"), "doublebuf",  this->settings.screen.doublebuf);
		setBoolFromPT(s.get_child("screen"), "resizeable", this->settings.screen.resizable);

		this->settings.screen.background = Color(s.get("screen.background", this->settings.screen.background.getString()));

		this->settings.screen.maxFPS = s.get("screen.maxFPS", this->settings.screen.maxFPS);
	} else
		cout << "WARNING: no screen-settings found in '" << settings_xml << "'!" << endl;

	/*if(s.get_child_optional("styles"))
	{
		this->settings.styles.widgets = s.get("styles.widgets", this->settings.styles.widgets);
		this->settings.styles.sprites = s.get("styles.sprites", this->settings.styles.sprites);
	} else
		cout << "WARNING: no style-settings found in '" << settings_xml << "'!" << endl;*/
};
//------------------------------------------------------------------------------
Uint32
Kernel::getFlags()
{
	Uint32 flags = SDL_OPENGL;
	if(settings.screen.fullscreen)	flags = flags | SDL_FULLSCREEN;
	if(settings.screen.doublebuf)	flags = flags | SDL_DOUBLEBUF;
	if(settings.screen.resizable)	flags = flags | SDL_RESIZABLE;
	return flags;
}
//------------------------------------------------------------------------------
void
Kernel::init()
{
	/// create managers
	this->inputMgr		= boost::shared_ptr<InputManager>(new InputManager);
	this->graphicsMgr	= boost::shared_ptr<GraphicsManager>(new GraphicsManager);
	this->timeMgr		= boost::shared_ptr<TimeManager>(new TimeManager);
	this->animMgr		= boost::shared_ptr<AnimationManager>(new AnimationManager(timeMgr));

	this->graphicsMgr->init(vector2<int>(settings.screen.w, settings.screen.h), settings.screen.bpp, this->getFlags());
	this->graphicsMgr->setBackgroundColor(settings.screen.background);
	this->graphicsMgr->cls();

	this->inputMgr->addPointKey("left_mouse");
	this->inputMgr->addPointKey("right_mouse");
	this->inputMgr->addPointKey("middle_mouse");
	this->inputMgr->addPointKey("wheelup_mouse");
	this->inputMgr->addPointKey("wheeldown_mouse");

	this->guiMgr = boost::shared_ptr<GuiManager>(new GuiManager(this->graphicsMgr, this));

	SDL_WM_SetCaption(this->settings.appname.c_str(), "");

	/// bindings
	this->inputMgr->addKeyListener     (boost::bind(&GuiManager::keyListener,      this->guiMgr, _1, _2));
	this->inputMgr->addPointMovListener(boost::bind(&GuiManager::pointMovListener, this->guiMgr, _1, _2));
	this->inputMgr->addPointHitListener(boost::bind(&GuiManager::pointHitListener, this->guiMgr, _1));

	this->guiMgr->addEvent("quit");
	this->guiMgr->getEvent("quit")->connect(boost::bind(&Kernel::stop, this));

	this->init_complete = true;
};
//------------------------------------------------------------------------------
void
Kernel::run()
{
	if(!this->init_complete)
		throw ERROR("illegalOperation", "Can't start mainloop. Kernel was not properly initialized.");

	this->running = true;
	Uint32 starttime=0;
	Uint32 msPerFrame = 0;
	if(this->settings.screen.maxFPS > 0)
		msPerFrame = 1000/this->settings.screen.maxFPS;

	this->animMgr->resetInitTime();

	/// Mainloop
	while(this->running)
	{
		/// Fehler die jetzt auftreten können wahrscheinlich
		/// während der Laufzeit "behoben" werden.
		try
		{
			if(msPerFrame > 0)
				starttime = SDL_GetTicks();

			this->processInput();

			this->handleToDos();

			TimeVal delta = this->animMgr->letTimeElapse();

			this->calcFrame(delta);
			if(this->calc_frame_func)
				this->calc_frame_func(delta);

			this->drawFrame();

			/// FPS-Grenze einhalten
			if(msPerFrame > 0)
			{
				Uint32 diff = SDL_GetTicks() - starttime;
				if(diff < msPerFrame)
					SDL_Delay(msPerFrame-diff);
			}
		}
		/// Kritischer Fehler -> Programm kann doch nicht weiterlaufen
		catch(CriticalError& e)
		{
			throw e;
		}
		/// alles andere schon...
		catch(std::exception& e)
		{
			cerr << "EXCEPTION: " << e.what() << endl;
			//this->guiMgr->createDialog("ERROR",e.what(),"window", "button_box",true,"OK");
			this->guiMgr->createPopupOK(string("ERROR:\n")+e.what());
		}
		catch(...)
		{
			cerr << "UNKNOWN RUNTIME-EXCEPTION!" << endl; exit(EXIT_FAILURE);
		}
	}
};
//------------------------------------------------------------------------------
void
Kernel::drawFrame()
{
	this->graphicsMgr->cls();

	this->guiMgr->drawEverything();

	this->graphicsMgr->flip();
};
//------------------------------------------------------------------------------
void
Kernel::processInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		/// check for messages
		switch (event.type)
		{
			/// exit if the window is closed
			case SDL_QUIT:
				this->stop();
				break;
			/// Tastendrücke?
			case SDL_KEYDOWN:
				this->inputMgr->inputKeyDown((KEY)event.key.keysym.sym);
				break;
			case SDL_KEYUP:
				this->inputMgr->inputKeyUp((KEY)event.key.keysym.sym);
				break;
			case SDL_MOUSEBUTTONDOWN:
				this->inputMgr->inputPointDown(getMouseName((KEY)event.button.button));
				break;
			case SDL_MOUSEBUTTONUP:
				this->inputMgr->inputPointUp(getMouseName((KEY)event.button.button));
				break;
			/// Mausbewegungen?
			case SDL_MOUSEMOTION:
			{
				this->inputMgr->inputPoint2("mouse",  event.motion.x, event.motion.y);
				this->inputMgr->inputPoint2("mouse1", event.motion.x, event.motion.y);
				break;
			}
			/// Grafikänderungen?
			case SDL_VIDEORESIZE:
				this->settings.screen.w = event.resize.w;
				this->settings.screen.h = event.resize.h;
				this->graphicsMgr->setVideoMode(vector2<int>(settings.screen.w, settings.screen.h), settings.screen.bpp, this->getFlags());
				break;
		} // end switch
	} // end of message processing
};
//------------------------------------------------------------------------------
void
Kernel::handleToDos()
{
	TimeVal now = this->timeMgr->now();

	BOOST_FOREACH(ToDo& td, this->todos)
		if(td.when <= now)
			td.func();

	//this->todos.clear();
	this->todos.remove_if(boost::bind(&ToDo::expired, _1, now));
};
//------------------------------------------------------------------------------
