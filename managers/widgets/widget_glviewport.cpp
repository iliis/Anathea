#include "widget_glviewport.hpp"

//------------------------------------------------------------------------------
WGLViewport::WGLViewport(string Name, Kernel* k)
 : Widget(Name,k), framebuffer(0),
   tex_size(300,300), texture(0), depth_buffer(0), rotation(0)
{
	glGenFramebuffers(1, &this->framebuffer);
	CHECK_GL_ERROR();

	this->createTexture();
};
//------------------------------------------------------------------------------
WGLViewport::~WGLViewport()
{
	glDeleteFramebuffers(1, &this->framebuffer);
	glDeleteRenderbuffers(1, &this->depth_buffer);
};
//------------------------------------------------------------------------------
void
WGLViewport::switchToFB()
{
	glBindFramebuffer(GL_FRAMEBUFFER, this->framebuffer);

	/// save currently used attributes (eg. viewport)
	glPushAttrib( GL_VIEWPORT_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT | GL_SCISSOR_BIT | GL_TRANSFORM_BIT );

	//GLenum draw_buffers[2] = {GL_COLOR_ATTACHMENT0};
	//glDrawBuffers(1, draw_buffers);

	glShadeModel(GL_SMOOTH);

    glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);

	/// set up perspective
	glViewport(0, 0, tex_size.x, tex_size.y);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	gluPerspective(45.0f, (GLfloat) tex_size.x / (GLfloat) tex_size.y, 0.1f, 500.0f);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	gluLookAt(100,0,100, 0,0,0, 0,1,0);
	glRotatef(this->rotation, 0, 1, 0);
	rotation += 0.1;
	if(rotation > 360) rotation -= 360;

	/// use some simple lightning
	GLfloat LightAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat LightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat LightPos[]     = { 20, 100, 100 };

	glLightfv(GL_LIGHT1, GL_AMBIENT,  LightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE,  LightDiffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, LightPos);
	glEnable (GL_LIGHT1);

	glEnable(GL_LIGHTING);

	/// enable simple coloring materials (enabling lighting changes the way colors work)
	glEnable(GL_COLOR_MATERIAL);
	//glColorMaterial() // is set by default correclty

	/// enable depth
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);



	glDisable(GL_SCISSOR_TEST);

	CHECK_GL_ERROR();

};
//------------------------------------------------------------------------------
void
WGLViewport::render()
{
	switchToFB();

	glClearColor(background.fr(), background.fg(), background.fb(), background.fa());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	BOOST_FOREACH(Mesh& m, this->meshes)
		m.render();

	/*glBindTexture(GL_TEXTURE_2D, 0);
	glBegin(GL_TRIANGLES);
		glColor3f(0,1,0);
		glVertex3f(0,0,1);
		glVertex3f(1,0,0);
		glVertex3f(0,1,0);
	glEnd();*/

	CHECK_GL_ERROR();

	switchToScreen();
};
//------------------------------------------------------------------------------
void
WGLViewport::switchToScreen()
{
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	CHECK_GL_ERROR();

	glPopAttrib();  CHECK_GL_ERROR();/// restore previously saved attributes
	glBindFramebuffer(GL_FRAMEBUFFER, 0); /// reset (use normal screen again)

	glDrawBuffer(GL_BACK);

	CHECK_GL_ERROR();
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

	CHECK_GL_ERROR();

	/// important, otherwise we get garbage only
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	/// disables texture-repetations so that there are no border-artefacts
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	CHECK_GL_ERROR();

	// attach texture to framebuffer color buffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->texture, 0);

	CHECK_GL_ERROR();


	glGenRenderbuffersEXT(1, &this->depth_buffer); // render buffer
	// initialize depth renderbuffer
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, this->depth_buffer);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, tex_size.x, tex_size.y);

	// attach renderbuffer to framebufferdepth buffer
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, this->depth_buffer);


	CHECK_GL_ERROR();


	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw ERROR("init", "Something went wrong initializing a framebuffer object.");

	glBindFramebuffer(GL_FRAMEBUFFER, 0); /// reset (use normal screen again)

	CHECK_GL_ERROR();
};
//------------------------------------------------------------------------------
void
WGLViewport::_draw()
{
	this->render();

	Box pos = this->getBB();

	glColor4f(1.0f,1.0f,1.0f,this->alpha.get());
	glBindTexture(GL_TEXTURE_2D, this->texture);


	glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex2f(pos.pos.x, 				pos.pos.y);
		glTexCoord2f(1, 0); glVertex2f(pos.pos.x + pos.size.x, 	pos.pos.y);
		glTexCoord2f(1, 1); glVertex2f(pos.pos.x + pos.size.x, 	pos.pos.y + pos.size.y);
		glTexCoord2f(0, 1); glVertex2f(pos.pos.x,				pos.pos.y + pos.size.y);
	glEnd();

	CHECK_GL_ERROR();
};
//------------------------------------------------------------------------------
