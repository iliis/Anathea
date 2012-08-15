#include "gl_triangle.hpp"

//------------------------------------------------------------------------------
void
Triangle::render() const
{
	glBindTexture(GL_TEXTURE_2D, this->tex);

	glBegin(GL_TRIANGLES);
		glNormal3f(n.x, n.y, n.z);

		glTexCoord2f(uv1.x, uv1.y); glColor4f(c1.fr(), c1.fg(), c1.fb(), c1.fa()); glVertex3f(p1.x, p1.y, p1.z);
		glTexCoord2f(uv2.x, uv2.y); glColor4f(c2.fr(), c2.fg(), c2.fb(), c2.fa()); glVertex3f(p2.x, p2.y, p2.z);
		glTexCoord2f(uv3.x, uv3.y); glColor4f(c3.fr(), c3.fg(), c3.fb(), c3.fa()); glVertex3f(p3.x, p3.y, p3.z);
	glEnd();
}
//------------------------------------------------------------------------------
void
Triangle::calcNormal()
{
	this->n =  (p2-p1).crossProduct(p3-p1);
	this->n.normalize();
}
//------------------------------------------------------------------------------
void
Triangle::flipNormal()
{
	this->n = this->n * (-1);
};
//------------------------------------------------------------------------------
