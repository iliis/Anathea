#include "SpriteSkeleton.hpp"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Bone::Bone(double a, double l, std::string n)
 : parent(NULL), angle(a), length(l), name(n)
{
	if(n.empty())
		this->set_random_name();
};
//------------------------------------------------------------------------------
void
Bone::set_random_name()
{
	this->name = "b_"+ToString(rand());
}
//------------------------------------------------------------------------------

double
Bone::getAngle()
{
	if(this->parent)
		return this->parent->getAngle() + this->angle;
	else
		return this->angle;
}
//------------------------------------------------------------------------------
Vect
Bone::getVect()
{
	double a = this->getAngle();
	return Vect(cos(a)*length,
				sin(a)*length);
};
//------------------------------------------------------------------------------
Vect
Bone::getAbsoluteVect()
{
	if(this->parent)
		return this->parent->getAbsoluteVect() + this->getVect();
	else
		return this->getVect();
};
//------------------------------------------------------------------------------
double
Bone::get_orthogonal_dist(Vect pt)
{
	Vect dir = this->getVect();
	Vect ortho_pt = orthproj_to_line(Vect(0,0), dir, pt);

	Vect::T factor = (dir.x==0?ortho_pt.x/dir.y : ortho_pt.x/dir.x);

	     if(factor < 0)
		return pt.length();
	else if(factor > 1)
		return (pt-dir).length();
	else
		return (pt-ortho_pt).length();
};
//------------------------------------------------------------------------------
bool
Bone::has_collision(Vect pt, double maxdist)
{
	return this->get_orthogonal_dist(pt) <= maxdist;
}
//------------------------------------------------------------------------------
std::string
Bone::get_cvs()
{
	std::string result;

	result.append(this->name);
	result.append(", ");

	if(this->parent)
		result.append(this->parent->name);
	else
		result.append("NULL");

	result.append(", "+ToString(this->length)+", "+ToString(this->angle)+'\n');

	BOOST_FOREACH(Bone& b, this->children)
		result.append(b.get_cvs());

	return result;
};
//------------------------------------------------------------------------------
void
Bone::mirror(double a, double relative_to)
{
	double old_angle = this->getAngle();
	this->angle = mk_angle_in_range((2*a) - (this->angle+relative_to));

	if(this->parent)
		this->angle -= parent->getAngle();

	this->angle = mk_angle_in_range(this->angle);

	BOOST_FOREACH(Bone& b, this->children)
	{
		b.angle -= this->getAngle() - old_angle;
		b.mirror(a, this->angle+relative_to);
	}
};
//------------------------------------------------------------------------------
bool
Bone::delete_child(Bone* b)
{
	if(b)
	{
		size_t s = this->children.size();

		this->children.remove(*b);

		if(s == this->children.size())
		{
			BOOST_FOREACH(Bone& c, this->children)
				if(c.delete_child(b)) return true;
		} else return true;
	}

	return false;
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void
SpriteSkeleton::loadFromFile(std::string filename)
{
	using namespace std;
	using boost::lexical_cast;

	ifstream file(filename.c_str());

	if(file.is_open())
	{
		this->allBones.clear();
		this->rootBones.clear();

		string line; int line_nr=1;
		while(file.good())
		{
			getline(file, line);
			if(line != "")
			{
				list<std::string> vals = splitString(line, ",");

				if(vals.size() != 4)
					throw ERROR("parse", "Can't parse skeleton data from file '"+filename+"' at line "+line_nr+": expected 4 values.");

				string name   = trimString(vals.front()); vals.pop_front();
				string parent = trimString(vals.front()); vals.pop_front();
				double length = lexical_cast<double>(trimString(vals.front())); vals.pop_front();
				double angle  = lexical_cast<double>(trimString(vals.front())); vals.pop_front();

				Bone* b = NULL;
				if(parent == "NULL")
					b = &(this->addBone(Bone(angle,length,name)));
				else
				{
					map<string,Bone*>::iterator it = this->allBones.find(parent);
					if(it != this->allBones.end())
						b = &(it->second->addBone(Bone(angle,length, name)));
					else
						throw ERROR("parse", "Can't parse skeleton data from file '"+filename+"' at line "+line_nr+": Couln't find parent '"+parent+"'.");
				}
				this->allBones[name] = b;

			}
			++line_nr;
		}

		file.close();
	}
	else
		throw ERROR("load", "Can't load skeleton from file '"+filename+"': Can't open that file.");
};
//------------------------------------------------------------------------------
void
SpriteSkeleton::saveToFile(std::string filename)
{
	ofstream file(filename.c_str());
	if(file.is_open())
	{
		file << this->get_cvs();
		file.close();
	}
	else
		throw ERROR("fail", "Can't write skeleton-data to file '"+filename+"'.");
};
//------------------------------------------------------------------------------
Vect
SpriteSkeleton::getMinSize()
{
	return Vect(0,0);
};
//------------------------------------------------------------------------------
Vect
SpriteSkeleton::getMaxSize()
{
	return Vect(0,0);
};
//------------------------------------------------------------------------------
Box
SpriteSkeleton::getBoundings()
{
	Box bb(0,0,0,0);

	BOOST_FOREACH(Bone& b, this->rootBones)
	{
		Box tmp = this->getBoundings(b);

		bb.pos.x  = min(bb.pos.x, tmp.pos.x);
		bb.pos.y  = min(bb.pos.y, tmp.pos.y);
		bb.size.x = max(bb.size.x, tmp.size.x);
		bb.size.y = max(bb.size.y, tmp.size.y);
	}

	return bb;
};
//------------------------------------------------------------------------------
Box
SpriteSkeleton::getBoundings(Bone& root, Vect disp)
{
	Vect rv = root.getVect();
	Box bb(disp,disp+rv);

	if(bb.size.x < 0)
	{
		bb.size.x *= -1;
		bb.pos.x  -= bb.size.x;
	}

	if(bb.size.y < 0)
	{
		bb.size.y *= -1;
		bb.pos.y  -= bb.size.y;
	}

	BOOST_FOREACH(Bone& b, root.children)
	{
		Box tmp = this->getBoundings(b, rv);

		bb.pos.x  = min(bb.pos.x, tmp.pos.x);
		bb.pos.y  = min(bb.pos.y, tmp.pos.y);
		bb.size.x = max(bb.size.x, tmp.size.x);
		bb.size.y = max(bb.size.y, tmp.size.y);
	}

	return bb;
};
//------------------------------------------------------------------------------
std::string
SpriteSkeleton::get_cvs()
{
	std::string result;

	BOOST_FOREACH(Bone& b, this->rootBones)
		result.append(b.get_cvs());

	return result;
};
//------------------------------------------------------------------------------
void
SpriteSkeleton::draw(Vect pos)
{
	glLineWidth(this->linewidth);
	glPointSize(this->linewidth);

	glDisable(GL_TEXTURE_2D);
	glColor4f(this->col.fr(),
			  this->col.fg(),
			  this->col.fb(),
			  this->col.fa());

	BOOST_FOREACH(Bone& b, this->rootBones)
		this->_draw(b, pos);

	glEnable(GL_TEXTURE_2D);
};
//------------------------------------------------------------------------------
void
SpriteSkeleton::_draw(Bone& root, Vect pos)
{
	Vect v = root.getVect()*this->scale;

	glBegin(GL_LINES);
		glVertex2f(pos.x,     pos.y);
		glVertex2f(pos.x+v.x, pos.y+v.y);
	glEnd();

	glBegin(GL_POINTS);
		glVertex2f(pos.x, pos.y);
	glEnd();

	BOOST_FOREACH(Bone& b, root.children)
		this->_draw(b, pos+v);
};
//------------------------------------------------------------------------------
void
SpriteSkeleton::assemble_BoneDistances(std::list<BoneDistContainer>& the_list, Bone& parent, Vect pt)
{
	BoneDistContainer my_container;
	my_container.bone = &parent;
	my_container.dist = parent.get_orthogonal_dist(pt);

	the_list.push_back(my_container);

	Vect v = parent.getVect();
	BOOST_FOREACH(Bone& c, parent.children)
		this->assemble_BoneDistances(the_list, c, pt-v);
};
//------------------------------------------------------------------------------
Bone*
SpriteSkeleton::pick_bone(Vect pt)
{
	if(this->rootBones.empty()) return NULL;

	std::list<BoneDistContainer> values;

	BOOST_FOREACH(Bone& r, this->rootBones)
		this->assemble_BoneDistances(values, r, pt);

	values.sort();
	return values.front().bone;
};
//------------------------------------------------------------------------------
Bone*
SpriteSkeleton::get_bone(std::string name)
{
	map<string,Bone*>::iterator it = this->allBones.find(name);

	if(it != this->allBones.end())
		return &(*it->second);
	else
		return NULL;
}
//------------------------------------------------------------------------------
//#include "managers/kernel.hpp"
void
SpriteSkeleton::set(ptree params)
{
	this->col       = params.get("color", this->col.getString());
	this->linewidth = params.get("linewidth", this->linewidth);
};
//------------------------------------------------------------------------------
void
SpriteSkeleton::removeBone(Bone* b)
{
	if(b)
	{
		size_t s = this->rootBones.size();

		this->rootBones.remove(*b);

		if(s == this->rootBones.size())
		{
			BOOST_FOREACH(Bone& c, this->rootBones)
				if(c.delete_child(b)) return;
		};
	}
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
SkeletonKeyframe::SkeletonKeyframe(SpriteSkeleton& sk, double _duration)
 : duration(_duration)
{
	assert(duration>0);

	BOOST_FOREACH(Bone& root, sk.getRootBones())
		this->add_data(root);
};
//------------------------------------------------------------------------------
void
SkeletonKeyframe::add_data(Bone& root)
{
	this->data[&root] = root.angle;

	BOOST_FOREACH(Bone& c, root.children)
		this->add_data(c);
};
//------------------------------------------------------------------------------
void
SkeletonKeyframe::fetch()
{
	BOOST_FOREACH(DataMap::value_type& d, this->data)
		d.second = d.first->angle;
};
//------------------------------------------------------------------------------
void
SkeletonKeyframe::apply()
{
	BOOST_FOREACH(const DataMap::value_type& d, this->data)
		d.first->angle = d.second;
};
//------------------------------------------------------------------------------
void
SkeletonKeyframe::apply_interpolated(double percent_next, SkeletonKeyframe& next_kf)
{
	assert(percent_next >= 0);
	assert(percent_next <= 1);

	BOOST_FOREACH(const DataMap::value_type& d, this->data)
	{
		if(next_kf.has_bone(d.first))
			d.first->angle = interpolate_angle(d.second, next_kf.data[d.first], percent_next);
	}
};
//------------------------------------------------------------------------------
std::string
SkeletonKeyframe::get_cvs()
{
	std::string result("");
	BOOST_FOREACH(const DataMap::value_type& d, this->data)
		result.append(d.first->name+", "+(d.second)+'\n');

	return result;
};
//------------------------------------------------------------------------------
void
SkeletonKeyframe::copy_data(const SkeletonKeyframe& kf)
{
	this->data = kf.data;
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
list<SkeletonKeyframe> load_sk_animation(SpriteSkeleton& skeleton, std::string filename)
{
	list<SkeletonKeyframe> anim;

	using namespace std;
	using boost::lexical_cast;

	ifstream file(filename.c_str());

	if(file.is_open())
	{
		string line; int line_nr=1;
		SkeletonKeyframe* kf = NULL;
		while(file.good())
		{
			getline(file, line);
			if(line.substr(0, 8) == "KEYFRAME" or kf == NULL)
			{
				anim.push_back(SkeletonKeyframe(lexical_cast<double>(line.substr(9))));
				kf = &anim.back();
			}
			else if(line != "")
			{
				list<std::string> vals = splitString(line, ",");

				if(vals.size() != 2)
					throw ERROR("parse", "Can't parse skeleton-animation data from file '"+filename+"' at line "+line_nr+": expected 2 values.");

				string name   = trimString(vals.front()); vals.pop_front();
				double angle  = lexical_cast<double>(trimString(vals.front())); vals.pop_front();

				Bone* b = skeleton.get_bone(name);

				if(b != NULL)
					kf->data[b] = angle;
				else
					throw ERROR("parse", "Can't parse skeleton-animation data from file '"+filename+"' at line "+line_nr+": Couln't find bone '"+name+"'.");

			}
			++line_nr;
		}

		if(anim.empty())
			throw ERROR("parse", "There is not skeleton-animation data in file '"+filename+"'.");

		file.close();

		//cout << this->skeleton.get_cvs() << endl << endl;
		//cout << this->get_animation_cvs() << endl;
		return anim;
	}
	else
		throw ERROR("load", "Can't load skeleton-animation from file '"+filename+"': Can't open that file.");
};
