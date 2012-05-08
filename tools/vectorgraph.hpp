#ifndef VECTORGRAPH_HPP_INCLUDED
#define VECTORGRAPH_HPP_INCLUDED

#include "tools/simplegraph.hpp"
#include "tools/geometry.hpp"

void filledRectangleRGBA(SDL_Surface* dst, int x, int y, int w, int h, Uint32 r, Uint32 g, Uint32 b, Uint32 a)
{
	SDL_Rect rect;
	rect.x = x; rect.y = y;
	rect.w = w, rect.h = h;
	SDL_FillRect(dst,&rect,SDL_MapRGBA(dst->format,r,g,b,a));
};

template<typename T>
class Vector2Graph : public SGraph< vector2<T> >
{
	public:
	typedef Node< vector2<T> > N;

	void draw(SDL_Surface* dst, Matrix<3,3> transform=Matrix<3,3>())
	{
		BOOST_FOREACH(Edge< vector2<T> > e, this->getEdges())
		{
			aalineRGBA(dst, e.getA()->get().x, e.getA()->get().y, e.getB()->get().x, e.getB()->get().y, 255,255,255,255);
		}

		BOOST_FOREACH(N* n, this->nodes)
		{
			filledRectangleRGBA(dst,n->get().x-1,n->get().y-1,3,3,255,100,0,255);
		}
	};

	void addLine(vector2<T> a, vector2<T> b)
	{
		N* node_a = this->find(a);
		N* node_b = this->find(b);

		if(node_a == NULL) node_a = new N(a);
		if(node_b == NULL) node_b = new N(b);

		node_a->addEdge(node_b);
		this->add(node_a);
		this->add(node_b);
	}
};

#endif // VECTORGRAPH_HPP_INCLUDED
