#ifndef WIDGET_GLVIEWPORT_HPP_INCLUDED
#define WIDGET_GLVIEWPORT_HPP_INCLUDED

#include "managers/widget.hpp"
#include "managers/image_mgr.hpp"
#include "managers/kernel.hpp"

#include "3d/gl_mesh.hpp"

class WGLViewport : public Widget
{
	GLuint framebuffer;
	VectInt tex_size; ///< the actual size of the texture and framebuffer
	GLuint texture;

	void switchToFB();
	void switchToScreen();

	void resizeTexture(VectInt newsize);
	void createTexture();

	void render();

	list<Mesh> meshes;

public:
	WGLViewport(string Name, Kernel* k);
	~WGLViewport();

	virtual void   _set(ptree n){};

	inline void resizeViewport(){this->resizeTexture(VectInt(this->width, this->height));}; ///< resize buffers to match widgetsize

	virtual const char* getType()        const {return "GLViewport";}


	inline void addMesh(const Mesh& m){this->meshes.push_back(m);}

protected:
	virtual void _draw();
};

#endif // WIDGET_GLVIEWPORT_HPP_INCLUDED
