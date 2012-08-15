#include "widget_glviewport.hpp"

//------------------------------------------------------------------------------
WGLViewport::WGLViewport(string Name, Kernel* k)
 : Widget(Name,k), framebuffer(0),
   tex_size(1024,1024), texture(0)
{
	glGenFramebuffers(1, &this->framebuffer);
	CHECK_GL_ERROR();

	this->createTexture();
};
//------------------------------------------------------------------------------
WGLViewport::~WGLViewport()
{
	glDeleteFramebuffers(1, &this->framebuffer);
};
//------------------------------------------------------------------------------
void
WGLViewport::switchToFB()
{
	glBindFramebuffer(GL_FRAMEBUFFER, this->framebuffer);

	/// save currently used attributes (eg. viewport)
	glPushAttrib( GL_VIEWPORT_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, tex_size.x, tex_size.y);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();							// Reset The Projection Matrix

	gluPerspective(45.0f, (GLfloat) tex_size.x / (GLfloat) tex_size.y, 0.1f, 100.0f);
	// Calculate The Aspect Ratio Of The Window

	glMatrixMode(GL_MODELVIEW);

};
//------------------------------------------------------------------------------
void
WGLViewport::switchToScreen()
{
	glPopAttrib(); /// restore previously saved attributes
	glBindFramebuffer(GL_FRAMEBUFFER, 0); /// reset (use normal screen again)

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
};
//------------------------------------------------------------------------------
void
WGLViewport::resizeTexture(VectInt newsize)
{
	assert(newsize.x > 0);
	assert(newsize.y > 0);

	/*if(newsize.x <= this->tex_size.x && newsize.y <= this->tex_size.y)
	{
		/// do nothing, as our buffer is still large enough
		/// only resize the UV coordinates

	}
	else*/
	{
		/// create the texture
		//this->tex_size.x = nextPowerOfTwo(newsize.x);
		//this->tex_size.y = nextPowerOfTwo(newsize.y);
		this->tex_size = newsize;
		this->createTexture();
	}

	//this->texture.setUV(Box(0,0,newsize.x, newsize.y));
}
//------------------------------------------------------------------------------
void
WGLViewport::createTexture()
{
	glBindFramebuffer(GL_FRAMEBUFFER, this->framebuffer);

	glGenTextures(1, &this->texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_size.x, tex_size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	// attach texture to framebuffer color buffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->texture, 0);

	CHECK_GL_ERROR();

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw ERROR("init", "Something went wrong initializing a framebuffer object.");
};
//------------------------------------------------------------------------------
void
WGLViewport::render()
{
	switchToFB();

	BOOST_FOREACH(Mesh& m, this->meshes)
		m.render();

	switchToScreen();
};
//------------------------------------------------------------------------------
void
WGLViewport::_draw()
{
	/*this->texture.color.a = this->alpha;

	this->texture.draw(this->getBB());*/
	Box pos = this->getBB();

	glBindTexture(GL_TEXTURE_2D, this->texture);

	Color c = this->color;

	glBegin(GL_QUADS);
		glColor4f(c.fr(),c.fg(),c.fb(),c.fa());

		glTexCoord2f(0, 0); glVertex2f(pos.pos.x, 				pos.pos.y);
		glTexCoord2f(1, 0); glVertex2f(pos.pos.x + pos.size.x, 	pos.pos.y);
		glTexCoord2f(1, 1); glVertex2f(pos.pos.x + pos.size.x, 	pos.pos.y + pos.size.y);
		glTexCoord2f(0, 1); glVertex2f(pos.pos.x,				pos.pos.y + pos.size.y);
	glEnd();
};
//------------------------------------------------------------------------------
