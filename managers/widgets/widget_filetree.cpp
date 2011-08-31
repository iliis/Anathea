#include "widget_filetree.hpp"

//------------------------------------------------------------------------------
WFileTreeNode::WFileTreeNode(string name, Kernel* k)
 : WContainer(name, k), icon(k->graphicsMgr->getFallbackImage())
{
	label = kernel->guiMgr->createWidget<WText>(name+"_label");
	label->abs_x = this->abs_x;
	label->abs_y = this->abs_y;
	this->width  = label->width;
	this->height = label->height;
};
//------------------------------------------------------------------------------
void
WFileTreeNode::_draw()
{
	this->label->draw();
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
WFileTree::WFileTree(string name, Kernel* k)
 : Widget(name, k)
{
	this->rootlist = kernel->guiMgr->createWidget<WList>(name + "_rootlist");
};
//------------------------------------------------------------------------------
void
WFileTree::setRoot(string r)
{
	fs::path path(r);
	if(fs::exists(path) and fs::is_directory(path))
	{
		this->root = path;
		this->update();
	}
};
//------------------------------------------------------------------------------
void
WFileTree::update()
{
	rootlist->clear();
	rootlist->abs_x = this->abs_x;
	rootlist->abs_y = this->abs_y;
	this->width     = rootlist->width;
	this->height    = rootlist->height;


	vector<fs::path> content;

	for(fs::directory_iterator it = fs::directory_iterator(root);
	    it != fs::directory_iterator(); ++it)
		content.push_back(it->path());

	sort(content.begin(), content.end());

	BOOST_FOREACH(fs::path p, content)
	{
		//if(p.filename().generic_string()[0] != '.')
		{
			cout << p.filename().generic_string() << endl;

			//shared_ptr<WFileTreeNode> node = kernel->guiMgr->createWidget<WFileTreeNode>(name.get()+"_item");
			//node->setPath(p);

			shared_ptr<WText> node = kernel->guiMgr->createWidget<WText>(name.get()+"_item");

			node->setText(p.filename().generic_string());
			rootlist->insert(node);
		}
	}
};
//------------------------------------------------------------------------------
void
WFileTree::_draw()
{
	this->rootlist->draw();
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
