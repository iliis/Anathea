#ifndef ORDERED_LIST_HPP_INCLUDED
#define ORDERED_LIST_HPP_INCLUDED

template <typename IndexT, typename ContentT>
class SortedList
{
public:
	struct SortedList_Item
	{
		IndexT   index;
		ContentT content;

		SortedList_Item(IndexT i, ContentT c) : index(i), content(c) {};

		ContentT operator*(){return this->content;}
		ContentT operator->(){return this->content;}

		bool operator==(const SortedList_Item& r)
			{return this->index == r.index && this->content == r.content;}

		bool operator>(const SortedList_Item& r)
			{return this->index > r.index;}

		bool operator<(const SortedList_Item& r)
			{return this->index < r.index;}
	};
private:

	list<SortedList_Item> data; // smallest Index first


public:
	inline size_t size()   {return this->data.size();}
	inline bool   isEmpty(){return this->size() == 0;}

	inline typename list<SortedList_Item>::iterator begin(){return this->data.begin();}
	inline typename list<SortedList_Item>::iterator end()  {return this->data.end();}

	void insert(IndexT where, ContentT what)
	{
		if(this->isEmpty()  or  (--this->data.end())->index < where) // is new element bigger even than last one?
			{this->data.push_back(SortedList_Item(where, what));} // insert at end
		else
		{
			typename list<SortedList_Item>::iterator it = this->data.begin();
			/// do a iterative search to find insert position (interpolation search with lists anyone?)
			for(; it != this->data.end(); ++it)
				if(it->index > where) break;

			this->data.insert(it, SortedList_Item(where, what));
		}
	};

	list<ContentT> getElementsSEq(IndexT index)
	{
		list<ContentT> ret;

		BOOST_FOREACH(SortedList_Item& i, this->data)
		{
			if(i.index > index) return ret;
			else ret.push_back(i);
		}
	}

	list<ContentT> getAndRemoveElementsSEq(IndexT index)
	{
		list<ContentT> ret;
		list<SortedList_Item> new_data;

		BOOST_FOREACH(SortedList_Item& i, this->data)
		{
			if(i.index <= index)
				ret.push_back(i.content);
			else
				new_data.push_back(i);
		}

		this->data = new_data;
		return ret;
	}
};

#endif // ORDERED_LIST_HPP_INCLUDED
