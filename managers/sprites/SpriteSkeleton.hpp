#ifndef SPRITESKELETON_HPP_INCLUDED
#define SPRITESKELETON_HPP_INCLUDED

#include <list>
#include <map>
#include <string>
#include <iostream>
#include <fstream>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include "managers/sprite_mgr.hpp"
#include "tools/mathfuncs.hpp"
#include "tools/stringhelpers.hpp"

//------------------------------------------------------------------------------
struct Bone
{
	std::list<Bone> children;
	Bone* parent;

	Bone& addBone(Bone b){this->children.push_back(b); this->children.back().parent = this; return this->children.back();}

	double angle, length;

	Vect getVect();
	Vect getAbsoluteVect();
	double getAngle(); /// is relative to parent!

	Bone() : parent(NULL), angle(0), length(0) {this->set_random_name();};
	Bone(double a, double l, std::string n="");

	bool has_collision(Vect pt, double maxdist);
	inline bool empty(){return this->children.empty();}
	double get_orthogonal_dist(Vect pt);

	std::string name;
	std::string get_cvs();

	void set_random_name();

	void mirror(double a=PI/2, double relative_to=0);

	bool delete_child(Bone* b);

	bool operator==(const Bone& b){return this == &b;}
};
//------------------------------------------------------------------------------
/// for ordering bones according to their distance to a point
struct BoneDistContainer
{
	Bone* bone;
	double dist;

	bool operator<(const BoneDistContainer& rhs)
	{
		return this->dist < rhs.dist;
	}
};
//------------------------------------------------------------------------------
class SpriteSkeleton : public Sprite
{
//---------- ATTRIBUTES --------------------------------------------------------
	Color col;
	double linewidth, scale;
	std::list<Bone> rootBones;
	std::map<std::string, Bone*> allBones;

//------------------------------------------------------------------------------
	void _draw(Bone& root, Vect pos=Vect(0,0));
	Box  getBoundings(Bone& root, Vect disp=Vect(0,0));
//------------------------------------------------------------------------------
	void assemble_BoneDistances(std::list<BoneDistContainer>& the_list, Bone& parent, Vect pt);
public:
//---------- CONSTRUCTOR -------------------------------------------------------
	SpriteSkeleton(boost::shared_ptr<GraphicsManager> gmgr)
	 : Sprite(gmgr), col(WHITE), linewidth(1), scale(1)
		{};

	void loadFromFile(std::string filename);
	void saveToFile(std::string filename);

//---------- GET ---------------------------------------------------------------
	inline Color getColor(){return col;}
	inline double getLineWidth(){return this->linewidth;}
	inline std::list<Bone>& getRootBones(){return this->rootBones;}

	Vect getMinSize();
	Vect getMaxSize();
	Box  getBoundings();

	std::string getType(){return "Skeleton";}

	std::string get_cvs();

	Bone* pick_bone(Vect pt);
	Bone* get_bone(std::string name);

//---------- SET ---------------------------------------------------------------
	inline void setColor(Color c){this->col = c;}
	inline void setLineWidth(double l){this->linewidth = l;}
	void setAlpha(ColVal a) {this->alpha = a; col.setSecondaryAlpha(a);}
	void setScale(double s){this->scale = s;}

	Bone& addBone(Bone b){this->rootBones.push_back(b); return this->rootBones.back();}
	void removeBone(Bone* b);

	void set(ptree params);

	void mirror(double a){BOOST_FOREACH(Bone& b, this->rootBones) b.mirror(a);};

//---------- DRAW --------------------------------------------------------------
	void draw(Vect pos=Vect(0,0));

};
//------------------------------------------------------------------------------
struct SkeletonKeyframe
{
	typedef std::map<Bone*, double> DataMap;
	DataMap data;


	double duration;

	SkeletonKeyframe(Bone& root, double _duration) : duration(_duration) {this->add_data(root); assert(duration>0);};
	SkeletonKeyframe(SpriteSkeleton& sk, double _duration);
	SkeletonKeyframe(double _duration) : duration(_duration) {assert(duration>0);};

	void add_data(Bone& root);
	void copy_data(const SkeletonKeyframe& kf);
	void remove_bone(Bone* b){this->data.erase(b);}
	bool has_bone(Bone* b){return this->data.find(b) != this->data.end();}
	void fetch();
	void apply();
	void apply_interpolated(double percent_next, SkeletonKeyframe& next_kf);
	std::string get_cvs();
};
//------------------------------------------------------------------------------
list<SkeletonKeyframe> load_sk_animation(SpriteSkeleton& skeleton, std::string filename);


#endif // SPRITESKELETON_HPP_INCLUDED
