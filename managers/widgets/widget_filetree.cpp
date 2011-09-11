#include "widget_filetree.hpp"

//------------------------------------------------------------------------------
WFileTreeNode::WFileTreeNode(string name, Kernel* k)
 : WContainer(name, k), icon(k->graphicsMgr->getFallbackImage())
{
	label = kernel->guiMgr->createWidget<WText>(name+"_label");
	label->abs_x = this->abs_x.ref();
	label->abs_y = this->abs_y.ref();
	this->width  = label->width.ref();
	this->height = label->height.ref();
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
 : Widget(name, k),
   show_hidden_files(false)
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
	else
		throw Error("notFound", "'"+r+"' isn't a valid directory.");
};
//------------------------------------------------------------------------------
void
WFileTree::update()
{
	rootlist->clear();
	rootlist->abs_x = this->abs_x.ref();
	rootlist->abs_y = this->abs_y.ref();
	this->width     = rootlist->width.ref();
	this->height    = rootlist->height.ref();


	vector<fs::path> content;

	for(fs::directory_iterator it = fs::directory_iterator(root);
	    it != fs::directory_iterator(); ++it)
		content.push_back(it->path());

	sort(content.begin(), content.end());

	list<WidgetPtr> tmp_nodes;

	BOOST_FOREACH(fs::path p, content)
	{
		if(show_hidden_files or p.filename().generic_string()[0] != '.')
		{
			cout << p.filename().generic_string() << endl;

			//shared_ptr<WFileTreeNode> node = kernel->guiMgr->createWidget<WFileTreeNode>(name.get()+"_item");
			//node->setPath(p);

			shared_ptr<WText> node = kernel->guiMgr->createWidget<WText>(name.get()+"_item");

			node->setText(p.filename().generic_string());
			tmp_nodes.push_back(node);
		}
	}

	rootlist->insert(tmp_nodes);
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
