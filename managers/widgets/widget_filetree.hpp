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

class WFileTreeNode : public WContainer
{
	fs::path obj;
	shared_ptr<WText> label;
	Image icon;
public:
	WFileTreeNode(string name, Kernel* k);

	void setPath(fs::path p){obj = p; label->setText(p.filename().generic_string());}
	void setIcon(Image i){icon = i;}

	void _draw();
};

class WFileTree : public Widget
{
	fs::path root;
	shared_ptr<WList> rootlist;
public:
	WFileTree(string name, Kernel* k);

	// GET
	//-------------------------------------------------------------------------

	// SET
	//-------------------------------------------------------------------------
	void setRoot(string r);


	// FUNCTIONALITY
	//-------------------------------------------------------------------------
	void update();

	void _draw();
};

#endif // WIDGET_FILETREE_HPP_INCLUDED
