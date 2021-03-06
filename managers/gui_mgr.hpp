#ifndef GUI_MGR_HPP_INCLUDED
#define GUI_MGR_HPP_INCLUDED

#include "boost/bind.hpp"

#include "managers/widget.hpp"
#include "managers/input_mgr.hpp"
#include "managers/image_mgr.hpp"
#include "tools/ticpp/ticpp.h"

using namespace std;

class Kernel;

//------------------------------------------------------------------------------
/// Verwaltet Widgets, speichert von jedem einen smartpointer, so dass keines gelöscht wird...
class GuiManager/* : public enable_shared_from_this<GuiManager>*/
{
	list<WidgetPtr> root_widgets;

	boost::shared_ptr<GraphicsManager>	 graphicsMgr;
	Kernel*				kernel;

	int unique; ///< Nummer, die bei jedem "Gebrauch" inkrementiert wird

	Slots events; ///< Dinge wie "Dialog_OK" oderso...

	void removeFromList(WidgetPtr widget);

	WidgetPtr focus_widget; ///< Widget das den Fokus hat. Ansich empfaengt jedes Widget Inputs, aber dieses kriegts zuerst.


	map<string, ptree> default_parameters; ///< Standardeinstellungen für die Widgets (eg. Schrift, Hintergrund, ...)

public:
	Expression<bool> draw_bounding_boxes;

	GuiManager( boost::shared_ptr<GraphicsManager> GraphicsMgr,
				Kernel* _kernel)
		: graphicsMgr(GraphicsMgr), kernel(_kernel), unique(0)
		   {assert(_kernel);};

	~GuiManager() {BOOST_FOREACH(WidgetPtr c, this->root_widgets) c->clearSlots();}

	inline	bool		hasWidget(string name){return this->getWidget(name, false) != WidgetPtr();}
			bool		hasRootWidget(string name);
	inline	bool		hasRootWidget(WidgetPtr widget){return find(root_widgets.begin(), root_widgets.end(), widget) != root_widgets.end();};
			WidgetPtr	getWidget(string path, bool throwIfNotFound=true);
			void		moveRootWidgetToFront(WidgetPtr w);
			void		moveRootWidgetToBack(WidgetPtr w);

	template<typename T>
	boost::shared_ptr<T> castWidget(string path, bool throwIfNotFound=true)
	{
		return boost::static_pointer_cast<T,Widget>(this->getWidget(path,throwIfNotFound));
	};

	/// wenn Widget kein parent hat, wird es vom GuiManager gespeichert,
	/// sobals allerdings ein parent kriegt, wirds fallengelassen...
			void		addWidget(WidgetPtr widget);
			void		addRootWidget(WidgetPtr widget);
			void		deleteWidget(string name);
			void		updateWidgetParent(WidgetPtr w);

			void		enableBoundingBoxes(bool draw = true);

			void		drawEverything();

	bool keyListener(KEY name, bool state);
	bool pointMovListener(PointKey oldPoint, PointKey newPoint);
	bool pointHitListener(PointKey pk);

	inline void setFocusToWidget(WidgetPtr w) {
		this->focus_widget = w;
	}

	inline void removeFocus() {
		this->focus_widget.reset();
	}

	inline bool hasFocus(WidgetPtr w) {
			return w == this->focus_widget;
	}


	template <typename T>
	boost::shared_ptr<T> createWidget(string name, ptree properties = ptree())
	{
		boost::shared_ptr<T> newWidget(new T(name, this->kernel));
		newWidget->init();

		//this->addWidget(newWidget);
		newWidget->draw_bounding_box = this->draw_bounding_boxes.ref();

		this->setWidgetDefaults(newWidget);
		newWidget->set(properties);

		return newWidget;
	};

	WidgetPtr createWidget(string typ, string name);

	void setWidgetDefaults(WidgetPtr widget);

	WidgetPtr  createWidgetFromPT   (ptree node, WidgetPtr parent = WidgetPtr());
	//WidgetPtr  createMultipleWidgets(ptree node, WidgetPtr parent = WidgetPtr());

	void connectEvents(WidgetPtr widget, string event_name, string event_slot);


	inline Slots::SignalPtr getEvent(string name){return events[name];}
	inline Slots&           getEvents(){return this->events;}
	inline void            triggerEvent(string name){this->events.call(name);}
	inline void            addEvent(string name){this->events.add(name);}

	int getUniqueNr(){return unique++;}

	void createPopupOK(string text);
};
//------------------------------------------------------------------------------

#endif // GUI_MGR_HPP_INCLUDED
