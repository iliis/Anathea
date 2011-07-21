#ifndef SLOTS_HPP_INCLUDED
#define SLOTS_HPP_INCLUDED

#include <boost/signals.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#include <map>
#include <strings.h>

#include <iostream>

#include "tools/errorclass.hpp"

using namespace std;

//------------------------------------------------------------------------------
/** Für alle Klassen, die irgendwelche Informationen
  * verteilen sollen. Bei jeder Änderung an einer solchen Klasse kann
  * per slots.call(name) jeder "Abonennt" eines Slots über die Änderung informiert
  * werden.
  *
  * Eine solche Klasse könnte z.B. so aussehen:
  * class Foo
  * {
  * 	int x;
  * 	Slots slots;
  * public:
  * 	void           setX(int X){this->x = X; slots.call("x");
  * 	SignalPtr      getSlot(string name){return slots[name];}
  * };
  *
  *
  * Verwendung allgemein:
  *
  * 	Slot erzeugen:
  * 		slots.add("foo");
  *
  * 	Slot aufrufen:
  * 		slots.call("foo");
  *
  * 	Slot mit Aktion verknüpfen:
  * 		slots.get("foo").connect(some_func);
  * 		slots.get("foo").connect(boost::bind(&WText::setText, a_label, "Hello World"));
  *
  *
  */
class Slots
{
public:
	typedef boost::signal<void (void)> SlotT;
	typedef boost::shared_ptr< SlotT > SignalPtr;
	typedef boost::signals::connection Connection;
private:

	typedef map<string, SignalPtr> SigMap;
	bool deactivated;

	/// Speichert alle verschiedenen "Mailinglisten" (Slots)
	SigMap slots;
	SigMap slots_one_time;

public:
	Slots() : deactivated(false) {};

	/// Fügt einen neuen Slot hinzu (wird am besten im Konstruktor der abgeleiteten Klasse benutzt)
	inline
	void add(string name)
		{this->add(name, SignalPtr(new SlotT));}
	void add(string name, SignalPtr s)
		{
			if(!this->hasSlot(name))
			{
				this->slots[name] = s;
				this->slots_one_time[name] = SignalPtr(new SlotT);
			}
		}

	/// Informiert alle Abonennten eines Slots über eine Änderung
	void call(string name)
	{
		if(!this->deactivated)
		{
			SigMap::iterator it  = this->slots.find(name);
			SigMap::iterator it2 = this->slots_one_time.find(name);
			if(it != this->slots.end() and it2 != this->slots_one_time.end())
			{
				(*(it ->second))();
				(*(it2->second))(); it2->second->disconnect_all_slots();
			}
			else
				cerr << "WARNING: There is no slot named '" << name << "' to call. Omitting." << endl;
				//throw Error("notFound", "There is no slot named '"+name+"' to call.");
		}
	};

	/// Gibt einen Pointer auf einen Slot zurück, so dass man sich dafür an- oder abmelden kann.
	SignalPtr get(const string& name) const
	{
		SigMap::const_iterator it = this->slots.find(name);
		if(it != this->slots.end())
			return it->second;
		else
			throw Error("notFound", "There is no slot named '"+name+"'.");
	}

	inline
	Connection connect(string name, SlotT::slot_function_type func)
	{
		return this->get(name)->connect(func);
	}

	Connection connect_run_once(string name, SlotT::slot_function_type func)
	{
		SigMap::const_iterator it = this->slots_one_time.find(name);
		if(it != this->slots_one_time.end())
		{
			return it->second->connect(func);
		}
		else
			throw Error("notFound", "There is no slot named '"+name+"'.");
	}

	SignalPtr operator[](const string& name) const
		{return this->get(name);}

	/// Testet, ob Klasse überhaupt einen bestimmten Slot hat.
	bool		hasSlot(string name) const {return this->slots.find(name) != this->slots.end();}

	void clear(){this->slots.clear();}
	void deactivate(){this->deactivated = true;}
	void   activate(){this->deactivated = false;}

	virtual	~Slots(){};
};
//------------------------------------------------------------------------------

#endif // SLOTS_HPP_INCLUDED
