#include "widget_filetree.hpp"

//------------------------------------------------------------------------------
WFileTreeNode::WFileTreeNode(string n, Kernel* k)
 : WContainer(n, k), icon(k->graphicsMgr->getFallbackImage()), collapsed(true)
{
	label = kernel->guiMgr->createWidget<WButton>(n+"_button");
	label->getSlot("clicked")->connect(boost::bind(&WFileTreeNode::toggle, this));

	/*Image bbg1 = kernel->graphicsMgr->loadImage("images/buttons/default.png");
		Image bbg2 = bbg1, bbg3 = bbg1;
		bbg1.setUV(Box(0,  0,119,58)); bbg1.setNinePatchData(NinePatchData(true, 6,6,6,6));
		bbg2.setUV(Box(0, 58,119,58)); bbg2.setNinePatchData(NinePatchData(true, 6,6,6,6));
		bbg3.setUV(Box(0,116,119,58)); bbg3.setNinePatchData(NinePatchData(true, 6,6,6,6));
		label->setBGnormal(bbg1);
		label->setBGhover(bbg2);
		label->setBGactive(bbg3);


	label->height = 28; // random :P
	label->setAutoWidth(10);*/
	label->setAlign(LEFT);

	label->abs_x = this->abs_x.ref();
	label->abs_y = this->abs_y.ref();
	this->width  = label->width.ref();
	this->height = label->height.ref() + this->vert_padding.ref();
};
//------------------------------------------------------------------------------
void
WFileTreeNode::init()
{
	this->addChild(this->label);
};
//------------------------------------------------------------------------------
void
WFileTreeNode::setPath(fs::path p)
{
	this->obj = p;
	this->label->setText("   + "+p.filename().generic_string());

	if(this->subtree)
		this->subtree->setRoot(p);
};
//------------------------------------------------------------------------------
void
WFileTreeNode::collapse()
{
	this->collapsed = true;
	this->subtree->hide();

	this->label->setText("   + "+this->obj.filename().generic_string());
	this->height = label->height.ref() + this->vert_padding.ref();
};
//------------------------------------------------------------------------------
void
WFileTreeNode::expand()
{
	if(!this->subtree)
	{
		/// create new subtree
		this->subtree = kernel->guiMgr->createWidget<WFileTree>(this->name.get()+"_subtree");
		this->subtree->indentation  = this->indentation.ref();
		this->subtree->vert_padding = this->vert_padding.ref();
		this->subtree->abs_x = this->abs_x.ref() + this->indentation.ref();
		this->subtree->abs_y = this->abs_y.ref() + this->label->height.ref() + this->vert_padding.ref();

		this->subtree->setRoot(this->obj);
		this->addChild(subtree);
	}

	/// show it
	this->subtree->show();
	this->collapsed = false;

	this->label->setText("   - "+this->obj.filename().generic_string());

	/// and adapt the height
	this->height = this->label->height.ref() + this->subtree->height.ref() + this->vert_padding.ref()*2;
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
WFileTree::WFileTree(string n, Kernel* k)
 : Widget(n, k), root("/"),
   show_hidden_files(false), indentation(30), vert_padding(2)
{
	this->rootlist = kernel->guiMgr->createWidget<WList>(n + "_rootlist");
	//this->rootlist->item_spacing = 5; /// just some random spacing
	/// size update is done in this::update()
};
//------------------------------------------------------------------------------
void
WFileTree::init()
{
	this->addChild(this->rootlist);
}
//------------------------------------------------------------------------------
void
WFileTree::setRoot(fs::path r)
{
	if(fs::exists(r) and fs::is_directory(r))
	{
		this->root = r;
		this->update();
	}
	else
		throw ERROR("notFound", "'"+r.filename().generic_string()+"' isn't a valid directory.");
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

	list<WidgetPtr> tmp_nodes_dirs, tmp_nodes_files;

	BOOST_FOREACH(fs::path p, content)
	{
		if(show_hidden_files or p.filename().generic_string()[0] != '.')
		{
			/// does it contain sub-stuff?
			if(fs::is_directory(p))
			{
				shared_ptr<WFileTreeNode> node = kernel->guiMgr->createWidget<WFileTreeNode>(p.filename().generic_string());

				node->setPath(p);
				node->indentation  = this->indentation.ref();
				node->vert_padding = this->vert_padding.ref();

				tmp_nodes_dirs.push_back(node);
			}
			/// or is it a boring blog of data?
			else if(fs::is_regular_file(p))
			{
				shared_ptr<WText> node = kernel->guiMgr->createWidget<WText>(p.filename().generic_string());

				node->setText(p.filename().generic_string());
				tmp_nodes_files.push_back(node);
			}
		}
	}

	/// we want all the directories first
	tmp_nodes_dirs .sort(order_by_name);
	tmp_nodes_files.sort(order_by_name);
	tmp_nodes_dirs.splice(tmp_nodes_dirs.end(), tmp_nodes_files); /// combine the lists
	rootlist->insert(tmp_nodes_dirs);
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
