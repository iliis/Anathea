#ifndef SKELETON_EDITOR_HPP_INCLUDED
#define SKELETON_EDITOR_HPP_INCLUDED

#include "managers/widget.hpp"
#include "managers/sprites/SpriteSkeleton.hpp"

/**
 *  LM    select bone
 *  CTRL-LM		set center
 *  SHIFT-LM     add bone
 *  DEL   delete selected bone
 *
 *  F1    load
 *  F2    save
 *  NUM+/k  frame++
 *  NUM-/j  frame--
 *  i     insert frame
 *  r     remove frame
 *  s     save current state to current frame
 *  q     remove current bone from all frames (=> will not be animated)
 *  LEFT/RIGHT
 *        modify angle of selected bone
 *  UP/DONW
 *        modify length of selected bone
 *  p     start/stop animation
 *  c     copy frame
 *  v     paste frame (overwrite current)
 *
 */

class SkeletonEditorWidget : public Widget
{
	SpriteSkeleton skeleton;
	Bone *nearest_bone, *active_bone;
	Vect  center;

	Image bg_img;

	list<SkeletonKeyframe> keyframes;
	list<SkeletonKeyframe>::iterator active_kf, copy_kf;

	void add_bone_to_keyframes(Bone& b);
	void insert_keyframe();
	void remove_keyframe();
	TimeVal time_delta; bool animate;
protected:
	virtual	void _set(ptree n){};
	virtual	void _draw();
	virtual bool handleKeyEvent(KEY key, bool state);
	virtual bool handlePointMovEvent(PointKey oldPoint, PointKey newPoint);
	virtual bool handlePointHitEvent(PointKey k);
	void animate_skeleton(TimeVal now);
public:
	SkeletonEditorWidget(string name, Kernel* k);

	void setSkeleton(SpriteSkeleton sk){this->skeleton = sk;}
	SpriteSkeleton&  getSkeleton(){return this->skeleton;}
	std::string get_animation_cvs();

	void save_animation(std::string filename);
	void load_animation(std::string filename);

	virtual const char* getType() const {return "SkeletonEditor";}
};


#endif // SKELETON_EDITOR_HPP_INCLUDED
