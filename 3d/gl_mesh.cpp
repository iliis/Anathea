#include "gl_mesh.hpp"

//------------------------------------------------------------------------------
void
Mesh::render() const
{
	BOOST_FOREACH(const Triangle& t, this->triangles)
		t.render();
};
//------------------------------------------------------------------------------
using namespace std;

void
Mesh::load_from_STL(string filename)
{
	/// for now, this function assumes the file is in ascii-format
	/// will probably behave unexpectedly (= crash) if it's binary

	ifstream file(filename.c_str());

	if(!file.good())
		throw Error("load", "Cannot load STL from '"+filename+"': Cannot open file.");

	while(!file.eof())
	{
		string line;
		getline(file, line);

		Triangle t; vector3<FNumber> tmp;
		if(parse_facet(line.begin(), line.end(), tmp))
		{
			//cout << "BEGIN facet" << endl;
			//cout << " > normal = " << tmp.print() << endl;
			t.n = tmp;

			do {getline(file, line);} while(!parse_vertex(line.begin(), line.end(), tmp));
			//cout << " > vertex = " << tmp.print() << endl;
			t.p1 = tmp;

			do {getline(file, line);} while(!parse_vertex(line.begin(), line.end(), tmp));
			//cout << " > vertex = " << tmp.print() << endl;
			t.p2 = tmp;

			do {getline(file, line);} while(!parse_vertex(line.begin(), line.end(), tmp));
			//cout << " > vertex = " << tmp.print() << endl;
			t.p3 = tmp;

			this->addTriangle(t);
			//cout << "END" << endl;
		}
	}

	file.close();
};
//------------------------------------------------------------------------------
