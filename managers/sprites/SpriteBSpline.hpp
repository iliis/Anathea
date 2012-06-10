#ifndef SPRITEBSPLINE_HPP_INCLUDED
#define SPRITEBSPLINE_HPP_INCLUDED

//------------------------------------------------------------------------------
class SpriteBSpline : public Sprite
{
//---------- ATTRIBUTES --------------------------------------------------------
	list<Vect> points;
	Color col;
	bool AA;
public:
//---------- CONSTRUCTOR -------------------------------------------------------
	SpriteBSpline(boost::shared_ptr<GraphicsManager> gmgr)
	 : Sprite(gmgr), col(WHITE), AA(true)
		{};

//---------- GET ---------------------------------------------------------------
	inline Color getColor()	{return col;}
	inline bool hasAA()		{return AA;}
			float getLength()	{return 0;}

	Vect getMinSize(){return Vect(0,0);};
	Vect getMaxSize(return Vect(0,0););

	string getType(){return "B-Spline";}

//---------- SET ---------------------------------------------------------------
	inline void setEndPos(Vect p){this->to = p;}
	inline void setColor(Color c){this->col = c;}
	inline void setAA(bool aa){this->AA = aa;}

	void set(ptree params);

//---------- DRAW --------------------------------------------------------------
	void draw(Vect pos=Vect(0,0));
};
//------------------------------------------------------------------------------

#endif // SPRITEBSPLINE_HPP_INCLUDED
