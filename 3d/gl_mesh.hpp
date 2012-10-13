#ifndef GL_MESH_HPP_INCLUDED
#define GL_MESH_HPP_INCLUDED

#include <list>
#include <fstream>
#include <boost/foreach.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

#include "3d/gl_triangle.hpp"

namespace qi = boost::spirit::qi;

/* STL triangles are composed as follows:

facet normal ni nj nk
outer loop
vertex v1x v1y v1z
vertex v2x v2y v2z
vertex v3x v3y v3z
endloop
endfacet

*/

//------------------------------------------------------------------------------
template <typename Iter>
bool parse_facet(Iter begin, Iter end, vector3<FNumber>& n)
{
	bool r = qi::phrase_parse(begin, end,
							(
									"facet normal"
									        >> qi::double_[boost::phoenix::ref(n.x) = qi::_1]
									        >> qi::double_[boost::phoenix::ref(n.y) = qi::_1]
									        >> qi::double_[boost::phoenix::ref(n.z) = qi::_1]
							) ,
							qi::space);

	return (begin == end) && r;
};
//------------------------------------------------------------------------------
template <typename Iter>
bool parse_const_string(Iter begin, Iter end, const string& str)
{
	bool r = qi::phrase_parse(begin, end, str, qi::space);
	return (begin == end) && r;
}
//------------------------------------------------------------------------------
template <typename Iter>
bool parse_vertex(Iter begin, Iter end, vector3<FNumber>& v)
{
	bool r = qi::phrase_parse(begin, end,
							(
									"vertex"
									        >> qi::double_[boost::phoenix::ref(v.x) = qi::_1]
									        >> qi::double_[boost::phoenix::ref(v.y) = qi::_1]
									        >> qi::double_[boost::phoenix::ref(v.z) = qi::_1]
							) ,
							qi::space);


	return (begin == end) && r;
}
//------------------------------------------------------------------------------



class Mesh
{
	std::list<Triangle> triangles; int tmp;

public:
	inline void addTriangle(Triangle t){this->triangles.push_back(t);}

	void render() ;

	void load_from_STL(string filename);
};

#endif // GL_MESH_HPP_INCLUDED
