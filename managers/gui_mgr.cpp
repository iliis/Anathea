#include "managers/gui_mgr.hpp"
#include "managers/kernel.hpp"

//------------------------------------------------------------------------------
void
GuiManager::enableBoundingBoxes(bool draw)
{
	this->draw_bounding_boxes = draw;
};
//------------------------------------------------------------------------------
void
GuiManager::drawEverything()
{
	/// Die ersten Objekte sind zuoberst, müssen also zuletzt gezeichnet werden...
	BOOST_REVERSE_FOREACH(WidgetPtr w, this->root_widgets)
		w->draw();
};
//------------------------------------------------------------------------------
bool
GuiManager::hasRootWidget(string name)
{
	BOOST_FOREACH(WidgetPtr w, this->root_widgets)
		if(w->name.get() == name)
			return true;

	return false;
};
//------------------------------------------------------------------------------
void
GuiManager::removeFromList(WidgetPtr widget)
{
	this->root_widgets.remove(widget);
};
//------------------------------------------------------------------------------
#include "managers/widgets/all_widgets.hpp"

WidgetPtr
GuiManager::createWidget(string typ, string name)
{
	/// who needs case-sensitivity anyway?
	boost::trim(typ);
	boost::to_lower(typ);

	      if(typ == "button")    return this->createWidget<WButton>   (name);
	else if(typ == "label"
		  or typ == "text")      return this->createWidget<WText>     (name);
	else if(typ == "image")     return this->createWidget<WImage>    (name);
	else if(typ == "container") return this->createWidget<WContainer>(name);
	else if(typ == "filetree")  return this->createWidget<WFileTree> (name);
	else if(typ == "list")      return this->createWidget<WList>     (name);
	else if(typ == "window")    return this->createWidget<WWindow>   (name);
	else
		throw Error("illegalOperation", "GuiManager can't create Widget '"+typ+"' (name:'"+name+"').");
};
//------------------------------------------------------------------------------
/// identisch zu Widget::getChild()
WidgetPtr
GuiManager::getWidget(string path, bool throwIfNotFound)
{
	size_t delimiter = path.find_first_of('/');
	if(delimiter == string::npos)
	{
	/// Pfad bezeichnet direkt Objekt
		BOOST_FOREACH(WidgetPtr c, this->root_widgets)
			if(c->name.get() == path || c->getType() == path)
				return c;
	}
	else
	{
		string root = path.substr(0, delimiter);
		path        = path.substr(delimiter+1);

		BOOST_FOREACH(WidgetPtr c, this->root_widgets)
		{
			if(c->name.get() == root || c->getType() == root)
			{
				WidgetPtr ptr = c->getChild(path);
					if(ptr != WidgetPtr())
						return ptr;
			}
		}

		path = root+"/"+path; ///< für Fehlermeldung
	}

	if(throwIfNotFound)
		throw Error("notFound", "GuiManager has no child named '"+path+"'.");
	else
		return WidgetPtr();
};
//------------------------------------------------------------------------------
void
GuiManager::addWidget(WidgetPtr widget)
{
	if(!widget->hasParent())
		this->addRootWidget(widget);
	else// if (this->hasWidget(widget->get<string>("name")))
		this->removeFromList(widget);
		//this->root_widgets.erase(widget->get<string>("name"));
};
//------------------------------------------------------------------------------
void
GuiManager::addRootWidget(WidgetPtr widget)
{
	/// falls parent existiert: weg damit
	if(widget->hasParent())
		widget->getParent()->removeChild(widget);

	/// bereits in Liste?
	if(!this->hasRootWidget(widget))
	{
		this->root_widgets.push_back(widget);
		/// sobald Widget ein parent bekommt, solls raus aus Liste
		widget->getSlot("parent")->connect(boost::bind(&GuiManager::updateWidgetParent, this, widget));
	}
};
//------------------------------------------------------------------------------
void
GuiManager::deleteWidget(string name)
{
	WidgetPtr tmp = this->getWidget(name, false);
	if(tmp != WidgetPtr())
	{
		tmp->deleteMe(false);
		if(!tmp->hasParent())
			this->removeFromList(tmp);
		else
			cout << "Warning: widget still in list" << endl;
	}
#ifdef DEBUG
	else
		cout << "Warning: Cannot delete Widget '" << name << "': Is not in my list." << endl;
#endif
};
//------------------------------------------------------------------------------
void
GuiManager::updateWidgetParent(WidgetPtr w)
{
	//WidgetPtr w = boost::dynamic_pointer_cast<Widget>(obj);

	/// falls Widget ein root-Element war (also in der Liste ist),
	/// jetzt jedoch ein parent bekommen hat, solls komplett rausfliegen
	if(w->hasParent() && this->hasWidget(w->name))
	{
		//this->root_widgets.erase(w->get<string>("name"));
		this->removeFromList(w);
		w->getSlot("parent")->disconnect(boost::bind(&GuiManager::updateWidgetParent, this, _1));
	}
};
//------------------------------------------------------------------------------
bool
GuiManager::keyListener(KEY name, bool state)
{
	BOOST_FOREACH(WidgetPtr w, this->root_widgets)
		if(w->keyListener(name, state))
			return true; /// Widget oder eines seiner Kinder hat Input verarbeitet

	return false; /// Input wurde nicht behandelt
};
//------------------------------------------------------------------------------
bool
GuiManager::pointMovListener(PointKey oldPoint, PointKey newPoint)
{
	BOOST_FOREACH(WidgetPtr w, this->root_widgets)
		if(w->pointMovListener(oldPoint, newPoint))
			return true; /// Widget oder eines seiner Kinder hat Input verarbeitet

	return false; /// Input wurde nicht behandelt
};
//------------------------------------------------------------------------------
bool
GuiManager::pointHitListener(PointKey pk)
{
	BOOST_FOREACH(WidgetPtr w, this->root_widgets)
		if(w->pointHitListener(pk))
			return true; /// Widget oder eines seiner Kinder hat Input verarbeitet

	return false; /// Input wurde nicht behandelt
};
//------------------------------------------------------------------------------
void
GuiManager::moveRootWidgetToFront(WidgetPtr w)
{
	this->root_widgets.splice(this->root_widgets.begin(), ///< to the beginning
						 this->root_widgets,
						 find(this->root_widgets.begin(),
							  this->root_widgets.end(),
							  w));
};
//------------------------------------------------------------------------------
void
GuiManager::moveRootWidgetToBack(WidgetPtr w)
{
	this->root_widgets.splice(this->root_widgets.end(), ///< to the end
						 this->root_widgets,
						 find(this->root_widgets.begin(),
							  this->root_widgets.end(),
							  w));
};
//------------------------------------------------------------------------------
void
GuiManager::setWidgetDefaults(WidgetPtr widget)
{
	// TODO: cache this stuff, so that we don't have to read files from disk every time
	// TODO: implement the <include>-Tag (implement merge_ptree or something)

	/// load basic defaults
	ptree base = readXML("xml/stylesheets/default/Widget.xml");
	widget->set(base);

	fs::path widget_stylesheet = string("xml/stylesheets/default/")+widget->getType()+".xml";
	widget->set(readXML(widget_stylesheet));

	/// load defaults from runtime-database
	map<string, ptree>::iterator i = this->default_parameters.find(widget->getType());
	if(i != this->default_parameters.end())
		widget->set(i->second);
};
//------------------------------------------------------------------------------
/*
void
GuiManager::createWidgetsFromXML(string file)
{
	ptree pt = readXML(file);
	this->createMultipleWidgets(pt);
};
//------------------------------------------------------------------------------
WidgetPtr
GuiManager::createMultipleWidgets(ptree node, WidgetPtr parent)
{
	WidgetPtr last;
	BOOST_FOREACH(ptree::value_type& v, node)
	{
		if(v.first != "event")
			last = this->createWidgetFromPT(v.second, parent);
	}
	return last;
};*/
//------------------------------------------------------------------------------
WidgetPtr
GuiManager::createWidgetFromPT(ptree node, WidgetPtr parent)
{
	cout << "creating widget from ptree:" << endl;
	printPTree(node);
	cout << endl << endl;

	/// lese Name und Typ aus
	string type = node.get_value<string>();
	string name = node.get("name", type);

	/// erzeuge Widget
	WidgetPtr newWidget = this->createWidget(type, name);
	if(parent != WidgetPtr())
	{
		parent->addChild(newWidget);

		/// kopiere Attribute der Eltern (is this a good idea? default stylesheets should be enough, shouldn't they?)
		//newWidget->copyAttrs(parent);
	}

	newWidget->set(node);

	/*
	this stuff is now done in Widget::set(ptree)


	/// node definiert einen Event?
	if(node.get_optional("event"))
	{
		//if(!node->ToElement()->HasAttribute("event-slot"))
		//	cerr << "Warning: Widget '" << name << "' has event-attribute but no attribute 'event-slot', using 'clicked' instead." << endl;

		string ev   = node.get<string>("event");
		string evsl = node.get("event-slot", "clicked");

		this->connectEvents(newWidget, ev, evsl);
	}

	/// ------- erzeuge Kinder -------
	if(node.get_child_optional("<children>"))
	{
		//ticpp::Iterator< ticpp::Node > child;
		//for(child = child.begin(node); child != child.end(); ++child)
		BOOST_FOREACH(ptree::value_type& child, node.get_child("<children>"))
		{
			if(child.first == "event")
			{
				string event_name = child.second.get<string>("name");
				string event_slot = child.second.get("slot", "clicked");
				this->connectEvents(newWidget, event_name, event_slot);
			}
			else
				this->createWidgetFromPT(child.second, newWidget);
		}
	}*/

	return newWidget;
};
//------------------------------------------------------------------------------
void
GuiManager::connectEvents(WidgetPtr widget, string event_name, string event_slot)
{
	this->events.add(event_name);
	widget->getSlot(event_slot)->connect(boost::bind(&GuiManager::triggerEvent, this, event_name));
};
//------------------------------------------------------------------------------
#include "widgets/widget_container.hpp"
#include "widgets/widget_text.hpp"
#include "widgets/widget_button.hpp"
#include "widgets/widget_image.hpp"

void
GuiManager::createPopupOK(string text)
{
	shared_ptr<WContainer> popup = this->createWidget<WContainer>("popup");
	this->addRootWidget(popup);

	popup->abs_x = this->graphicsMgr->screen_width .ref()/2 - popup->width .ref()/2;
	popup->abs_y = this->graphicsMgr->screen_height.ref()/2 - popup->height.ref()/2;
	//popup->draw_bounding_box = true;

	Image bg = kernel->graphicsMgr->loadImage("images/blue_edge.png");
	bg.setNinePatchData(NinePatchData(true,2,2,10,10));

	shared_ptr<WImage> container_bg = this->createWidget<WImage>("popup_image");
	container_bg->setImage(bg);
	popup->addChild(container_bg);
	container_bg->fitToParent();



	shared_ptr<WText> label = this->createWidget<WText>("popup_text");
	label->setText(text);
	popup->addChild(label);
	label->setRelativeToParent(CENTER, true, MIDDLE, true);
	label->abs_y = popup->abs_y.ref() + 15;

	shared_ptr<WButton> button = this->createWidget<WButton>("popup_button");
	button->setText("OK");
	Image bbg1 = kernel->graphicsMgr->loadImage("images/buttons/default.png");
		Image bbg2 = bbg1, bbg3 = bbg1;
		bbg1.setUV(Box(0,  0,119,58)); bbg1.setNinePatchData(NinePatchData(true, 6,6,6,6));
		bbg2.setUV(Box(0, 58,119,58)); bbg2.setNinePatchData(NinePatchData(true, 6,6,6,6));
		bbg3.setUV(Box(0,116,119,58)); bbg3.setNinePatchData(NinePatchData(true, 6,6,6,6));
		button->setBGnormal(bbg1);
		button->setBGhover(bbg2);
		button->setBGactive(bbg3);

	popup->addChild(button);
	button->getSlot("clicked")->connect(boost::bind(&Widget::deleteMe,popup,true));

	popup->width  = MAX<FNumber>(label->width.ref(),button->width.ref()) + 30;
	popup->height = label->height.ref() + button->height.ref() + 45;

	button->setRelativeToParent(CENTER, true, MIDDLE, true);
	button->abs_y = label->abs_y.ref() + label->height.ref() + 15;

	container_bg->moveToBG();
	popup->moveToFront();
};
//------------------------------------------------------------------------------
