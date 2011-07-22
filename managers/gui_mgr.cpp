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
WidgetPtr
GuiManager::createWidget(string typ, string name)
{
	/*	 if(typ == "button")return this->createWidget<WButton>(name);
	else if(typ == "label") return this->createWidget<WLabel>(name);
	//else if(typ == "image") return this->createWidget<WImage>(name);
	else*/
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
};
//------------------------------------------------------------------------------
WidgetPtr
GuiManager::createWidgetFromPT(ptree node, WidgetPtr parent)
{
	/// lese Name und Typ aus
	string type = node.get_value<string>();
	string name = node.get("name", type);

	/// erzeuge Widget
	WidgetPtr newWidget = this->createWidget(type, name);
	if(parent != WidgetPtr())
	{
		parent->addChild(newWidget);
		/// kopiere Attribute der Eltern
		//newWidget->copyAttrs(parent);
	}

	/// node definiert einen Event?
	if(node.get_optional<string>("event"))
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
	}

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
