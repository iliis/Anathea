#ifndef WIDGET_FILETREE_HPP_INCLUDED
#define WIDGET_FILETREE_HPP_INCLUDED

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

#include "managers/widget.hpp"
#include "managers/image_mgr.hpp"
#include "managers/kernel.hpp"

#include "managers/widgets/widget_text.hpp"
#include "managers/widgets/widget_button.hpp"
#include "managers/widgets/widget_list.hpp"
#include "managers/widgets/widget_image.hpp"

class WFileTree;

class WFileTreeNode : public WContainer
{
	fs::path obj;
	shared_ptr<WButton> label;
	shared_ptr<WFileTree> subtree;
	Image icon;

	bool collapsed;
public:
	WFileTreeNode(string n, Kernel* k);
	virtual void init();

	void setPath(fs::path p);
	void setIcon(Image i){icon = i;}

	virtual const char* getType()        const {return "FileTreeNode";}

	void collapse();
	void expand();
	void toggle(){if(collapsed) this->expand(); else this->collapse();}

	// PUBLIC PROPERTIES
	//-------------------------------------------------------------------------
	Expression<FNumber> indentation;
};

class WFileTree : public Widget
{
	fs::path root;
	shared_ptr<WList> rootlist;
	bool show_hidden_files;
public:
	WFileTree(string n, Kernel* k);
	virtual void init();

	// GET
	//-------------------------------------------------------------------------
	inline  bool        show_hidden()    const {return this->show_hidden_files;}
	virtual const char* getType()        const {return "FileTree";}

	// SET
	//-------------------------------------------------------------------------
	void setRoot(fs::path r);


	// FUNCTIONALITY
	//-------------------------------------------------------------------------
	void update();

	// PUBLIC PROPERTIES
	//-------------------------------------------------------------------------
	Expression<FNumber> indentation;
};

#endif // WIDGET_FILETREE_HPP_INCLUDED
