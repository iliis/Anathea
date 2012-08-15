#ifndef GL_TRIANGLE_HPP_INCLUDED
#define GL_TRIANGLE_HPP_INCLUDED

#include "tools/geometry.hpp"
#include "managers/definitions.hpp"

struct Triangle
{
	vector3<FNumber> p1, p2, p3, n;
	GLuint tex;
	Vect uv1, uv2, uv3;
	Color c1, c2, c3;



	void render() const;

	inline void setUniColor(Color c){c1=c; c2=c; c3=c;}
	void calcNormal();
	void flipNormal();
};

#endif // GL_TRIANGLE_HPP_INCLUDED
