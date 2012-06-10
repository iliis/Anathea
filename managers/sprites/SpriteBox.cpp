#include "SpriteBox.hpp"

//------------------------------------------------------------------------------
void
SpriteBox::draw(Vect pos)
{
	if(this->isVisible())
	{
		Box shape(pos, this->getSize());
		glDisable(GL_TEXTURE_2D);

		if(this->isFillEnabled())
		{
			glBegin(GL_QUADS);
				glColor4f(fillCol.fr(),fillCol.fg(),fillCol.fb(),fillCol.fa());
				glVertex2f(shape.pos.x,              shape.pos.y);
				glVertex2f(shape.pos.x+shape.size.x, shape.pos.y);
				glVertex2f(shape.pos.x+shape.size.x, shape.pos.y+shape.size.y);
				glVertex2f(shape.pos.x,              shape.pos.y+shape.size.y);
			glEnd();
		}

		if(this->isBorderEnabled())
		{
			float bs = this->borderSize/2;

			glLineWidth(this->borderSize);
			glBegin(GL_LINES);
				glColor4f(borderCol.fr(),borderCol.fg(),borderCol.fb(),borderCol.fa());
				glVertex2f(shape.pos.x-bs,           shape.pos.y);
				glVertex2f(shape.pos.x+shape.size.x, shape.pos.y);

				glVertex2f(shape.pos.x+shape.size.x, shape.pos.y-bs);
				glVertex2f(shape.pos.x+shape.size.x, shape.pos.y+shape.size.y);

				glVertex2f(shape.pos.x+shape.size.x+bs, shape.pos.y+shape.size.y);
				glVertex2f(shape.pos.x,                 shape.pos.y+shape.size.y);

				glVertex2f(shape.pos.x,           shape.pos.y+shape.size.y+bs);
				glVertex2f(shape.pos.x,           shape.pos.y);
			glEnd();
		}

		glEnable(GL_TEXTURE_2D);
	}
};
//------------------------------------------------------------------------------
#include "managers/kernel.hpp"
void
SpriteBox::set(ptree params)
{
	if(!params.empty())
	{
		//setBoolFromPT(params, "draw_border", this->drawBorder);
		setBoolFromPT(params, "draw_fill",   this->drawFill);

		this->borderSize = params.get("border_size", this->borderSize);
		this->borderCol = params.get("border", this->borderCol.getString());
		this->fillCol   = params.get("fill",   this->borderCol.getString());

		this->setSize(Vect(params, "w", "h", this->getSize()));
	};
};
//------------------------------------------------------------------------------
