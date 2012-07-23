#ifndef KERNEL_HPP_INCLUDED
#define KERNEL_HPP_INCLUDED

#include "managers/definitions.hpp"
#include "managers/gui_mgr.hpp"
#include "managers/input_mgr.hpp"
#include "managers/time_mgr.hpp"
#include "managers/animation_mgr.hpp"
#include "tools/errorclass.hpp"
#include "tools/misc.hpp"


struct ToDo
{
	boost::function<void()> func;
	TimeVal when;

	ToDo(boost::function<void()> f, TimeVal t=0) : func(f), when(t) {};

	bool expired(TimeVal now){return this->when <= now;}
};

/** MetaManager, erzeugt und verwaltet alle benötigten Manager.
  *
  */
class Kernel
{
public:
	boost::shared_ptr<GraphicsManager>	 graphicsMgr;
	boost::shared_ptr<GuiManager>		 guiMgr;
	boost::shared_ptr<InputManager>		 inputMgr;
	boost::shared_ptr<TimeManager>		 timeMgr;
	boost::shared_ptr<AnimationManager>  animMgr;


	struct Settings
	{
		fs::path file;
		string appname;
		ptree pt;

		struct Screen
		{
			unsigned int w, h, bpp;
			bool fullscreen, doublebuf, resizable, noframe;
			Color background;
			double maxFPS;
		} screen;
		/*struct Styles
		{
			string	widgets,
					sprites;
		} styles;*/
	} settings;
	Uint32 getFlags();

private:
	bool running, init_complete;
	list<ToDo> todos;

	boost::function<void(TimeVal)> calc_frame_func; ///< set that for your own stuff

	void drawFrame();
	void processInput();
	void handleToDos();
	void checkGLerrors();

public:
	Kernel( int argc, char *argv[] );

	inline bool isInitComplete(){return this->init_complete;}

	virtual void init();
	virtual void run();
	inline void stop(){this->running = false;}

	inline void addToDo        (boost::function<void()> func, TimeVal when=0){this->todos.push_back(ToDo(func,when));};
	inline void addRelativeToDo(boost::function<void()> func, TimeVal when)  {this->todos.push_back(ToDo(func,when+this->timeMgr->now()));};
	inline void setCalcFrameFunc(boost::function<void(TimeVal)> func){this->calc_frame_func = func;}
};

#endif // KERNEL_HPP_INCLUDED
