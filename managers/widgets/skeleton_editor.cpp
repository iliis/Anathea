#include "skeleton_editor.hpp"
#include "managers/kernel.hpp"

//------------------------------------------------------------------------------
SkeletonEditorWidget::SkeletonEditorWidget(string name, Kernel* k)
 : Widget(name, k), skeleton(k->graphicsMgr), nearest_bone(NULL), active_bone(NULL),
   bg_img(k->graphicsMgr->loadImage("gfx/creatures/stickman/running_right.png")), time_delta(0), animate(false)
{
	this->bg_img.alpha = A_HALF_TRANSPARENT;

	skeleton.setLineWidth(2);

	this->keyframes.push_back(SkeletonKeyframe(this->skeleton, 0.2));
	this->active_kf = this->keyframes.begin();
	this->copy_kf   = this->keyframes.end();

	k->animMgr->addCustomAnim(boost::bind(&SkeletonEditorWidget::animate_skeleton, this, _1));
};
//------------------------------------------------------------------------------
void
SkeletonEditorWidget::_draw()
{
	Vect p = this->center;

	if(!this->animate and false)
	{
		int total_nr = 7 /*this->keyframes.size()*/, frame = 0;
		for(list<SkeletonKeyframe>::iterator i = this->keyframes.begin(); i != this->active_kf; ++i) ++frame;
		frame = frame % total_nr;
		Box uv(frame/double(total_nr), 0, 1/double(total_nr), 1);
		Vect imgsize = bg_img.getSize().cast<Vect::T>(); imgsize.x = imgsize.x/total_nr;
		this->bg_img.draw(Box( this->getSize().x/2 - (imgsize.x*this->getSize().y/imgsize.y)/2,0,imgsize.x*this->getSize().y/imgsize.y, this->getSize().y), uv);
	}

	this->skeleton.draw(p);

	if(nearest_bone)
	{
		Vect v1 = (nearest_bone->parent?nearest_bone->parent->getAbsoluteVect():Vect(0,0));
		Vect v2 =  nearest_bone->getVect();

		glDisable(GL_TEXTURE_2D);
		glLineWidth(10);
		glBegin(GL_LINES);
			glVertex2f(p.x+v1.x,      p.y+v1.y);
			glVertex2f(p.x+v1.x+v2.x, p.y+v1.y+v2.y);
		glEnd();
		glLineWidth(this->skeleton.getLineWidth());

		glEnable(GL_TEXTURE_2D);
	}

	if(active_bone)
	{
		Vect v1 = (active_bone->parent?active_bone->parent->getAbsoluteVect():Vect(0,0));
		Vect v2 =  active_bone->getVect();

		glDisable(GL_TEXTURE_2D);
		glLineWidth(5);
		glColor3f(1.0f,0.0f,0.0f);
		glBegin(GL_LINES);
			glVertex2f(p.x+v1.x,      p.y+v1.y);
			glVertex2f(p.x+v1.x+v2.x, p.y+v1.y+v2.y);
		glEnd();
		glColor3f(1.0f,1.0f,1.0f);
		glLineWidth(this->skeleton.getLineWidth());

		glEnable(GL_TEXTURE_2D);
	}

	SkeletonKeyframe tmp(this->skeleton,0.1);
	Box b(5,this->getHeight()-45,40,40);
	this->skeleton.setScale(0.05);
	BOOST_FOREACH(SkeletonKeyframe& kf, this->keyframes)
	{
		drawRectangle(b, (&kf == &(*this->copy_kf)?ORANGE:WHITE), 1, &kf == &(*this->active_kf));

		if(&kf == &(*this->active_kf)) this->skeleton.setColor(BLACK); else this->skeleton.setColor(WHITE);
		kf.apply();

		this->skeleton.draw(b.pos+Vect(20,20));

		b.pos.x += 45;
	}
	this->skeleton.setColor(WHITE);
	this->skeleton.setScale(1);
	tmp.apply();

	if(this->active_bone)
	{
		drawPoint(this->getAbsPos()+this->center, 5, WHITE);
		drawPoint(this->getAbsPos()+this->center+this->active_bone->getAbsoluteVect(), 10, RED);
	}
	else
		drawPoint(this->getAbsPos()+this->center, 10, RED);
};
//------------------------------------------------------------------------------
void
SkeletonEditorWidget::animate_skeleton(TimeVal now)
{
	if(this->animate)
	{
		while(now-this->time_delta >= fromSeconds(this->active_kf->duration))
		{
			this->time_delta += fromSeconds(this->active_kf->duration);
			++this->active_kf;
			if(this->active_kf == this->keyframes.end()) this->active_kf = this->keyframes.begin();
		}

		list<SkeletonKeyframe>::iterator next = this->active_kf; ++next;
		if(next == this->keyframes.end()) next = this->keyframes.begin();

		this->active_kf->apply_interpolated(toSeconds(now-time_delta) / this->active_kf->duration, *next);
	} else
		this->time_delta = now;
};
//------------------------------------------------------------------------------
void
SkeletonEditorWidget::add_bone_to_keyframes(Bone& b)
{
	BOOST_FOREACH(SkeletonKeyframe& kf, this->keyframes)
		kf.add_data(b);
};
//------------------------------------------------------------------------------
void SkeletonEditorWidget::insert_keyframe()
{
	this->active_kf = this->keyframes.insert(++this->active_kf, SkeletonKeyframe(this->skeleton, 0.2));
};
//------------------------------------------------------------------------------
void SkeletonEditorWidget::remove_keyframe()
{
	if(this->keyframes.size()>1)
		this->active_kf = this->keyframes.erase(this->active_kf);
}
//------------------------------------------------------------------------------
bool
SkeletonEditorWidget::handleKeyEvent(KEY key, bool state)
{
	if(state and key == KEY_p)
	{
		this->animate = !this->animate;
		if(!this->animate)
			this->active_kf->apply();
		return true;
	}

	if(state and !this->animate)
	{
		if(this->active_bone)
		{
			bool shift = this->kernel->inputMgr->getKeyState(KEY_LSHIFT);

			if(key == KEY_LEFT)
				this->active_bone->angle -= PI/(shift?200:10);
			else if(key == KEY_RIGHT)
				this->active_bone->angle += PI/(shift?200:10);
			else if(key == KEY_UP)
				this->active_bone->length -= (shift?2:20);
			else if(key == KEY_DOWN)
				this->active_bone->length += (shift?2:20);
			else if(key == KEY_DELETE and this->active_bone->empty())
			{
				this->skeleton.removeBone(this->active_bone);
				BOOST_FOREACH(SkeletonKeyframe& kf, this->keyframes)
					kf.remove_bone(this->active_bone);

				this->active_bone  = NULL;
				this->nearest_bone = NULL;
			}
			else if(key == KEY_q)
			{
				BOOST_FOREACH(SkeletonKeyframe& kf, this->keyframes)
					kf.remove_bone(this->active_bone);
			}
		}

		switch(key)
		{
			case KEY_i:
				this->insert_keyframe(); break;
			case KEY_s:
				this->active_kf->fetch(); break;
			case KEY_KP_PLUS:
				++this->active_kf; if(this->active_kf == this->keyframes.end()) this->active_kf = this->keyframes.begin();
				this->active_kf->apply();
				break;
			case KEY_KP_MINUS:
				--this->active_kf; if(this->active_kf == this->keyframes.end()) --this->active_kf;
				this->active_kf->apply();
				break;
			case KEY_r:
				this->remove_keyframe(); break;
			case KEY_m:
				this->skeleton.mirror(PI/2); /// um vertikale achse (90°) spiegeln
				break;
			case KEY_c:
				this->copy_kf = this->active_kf; break;
			case KEY_v:
				if(this->copy_kf != this->active_kf and this->copy_kf != this->keyframes.end())
				{
					this->active_kf->copy_data(*this->copy_kf);
					this->active_kf->apply();
				}break;
			case KEY_F1:
			{
				this->skeleton.loadFromFile("skeletons/player.skt");
				this->skeleton.setLineWidth(2);
				this->load_animation("animations/player.ska");

				cout << "LOADED" << endl; break;
			}
			case KEY_F2:
				this->skeleton.saveToFile("skeletons/player.skt");
				this->save_animation("animations/player.ska");
				cout << "SAVED" << endl; break;
			default:
				return false;
		}

		return true;
	}

	return false;
};
//------------------------------------------------------------------------------
bool
SkeletonEditorWidget::handlePointMovEvent(PointKey oldPoint, PointKey newPoint)
{
	Vect p = newPoint.point.pos - this->center;
	if(p.length() < 20) ///< Klick in die Mitte
		this->nearest_bone = NULL; ///< select "root"
	else
		this->nearest_bone = this->skeleton.pick_bone(p);

	return false;
};
//------------------------------------------------------------------------------
bool
SkeletonEditorWidget::handlePointHitEvent(PointKey k)
{
	if(k.state)
	{
		if(this->kernel->inputMgr->getKeyState(KEY_LCTRL))
			this->center = k.point.pos;

		else if (this->kernel->inputMgr->getKeyState(KEY_LSHIFT))
		{
			Vect pos = k.point.pos - this->center;

			if(this->active_bone)
			{
				pos = pos - this->active_bone->getAbsoluteVect();

				this->add_bone_to_keyframes(
						this->active_bone ->addBone(Bone(atan(pos.y/pos.x)+(pos.x<0?PI:0)-this->active_bone->getAngle(), sqrt(pos.x*pos.x+pos.y*pos.y))));
			}
			else
				this->add_bone_to_keyframes(
						this->skeleton.addBone(Bone(atan(pos.y/pos.x)+(pos.x<0?PI:0), sqrt(pos.x*pos.x+pos.y*pos.y))));
		} else
			this->active_bone = this->nearest_bone;

		return true;
	}

	return false;
};
//------------------------------------------------------------------------------
std::string
SkeletonEditorWidget::get_animation_cvs()
{
	std::string result;

	BOOST_FOREACH(SkeletonKeyframe& k, this->keyframes)
	{
		result.append("KEYFRAME:"+ToString(k.duration)+"\n");
		result.append(k.get_cvs());
	}

	return result;
};
//------------------------------------------------------------------------------
void
SkeletonEditorWidget::save_animation(std::string filename)
{
	ofstream file(filename.c_str());
	if(file.is_open())
	{
		file << this->get_animation_cvs();
		file.close();
	}
	else
		throw Error("fail", "Can't write animation-data to file '"+filename+"'.");
};
//------------------------------------------------------------------------------
void
SkeletonEditorWidget::load_animation(std::string filename)
{
	this->keyframes = load_sk_animation(this->skeleton, filename);
	this->active_kf = this->keyframes.begin();
	this->active_kf->apply();
};
//------------------------------------------------------------------------------
