#include "vectormath.hpp"

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
float orthogonalDistance(Vect2 point, Vect2 lineStart, Vect2 lineEnd)
{
	//Gerade: lineStart+t*(lineEnd-lineStart)
	//Senkrechte darauf durch point: point+q*(A*(lineEnd-lineStart))
			// A: [0 -1]
			//    [1  0]
	//Schnittpunkt der beiden Geraden: Q

	//float t = (point - lineStart)/(lineEnd - lineStart - (lineEnd-lineStart).rotateCCW());
	Vect2 c(lineEnd - lineStart);

	float t = -(c.rotateCCW().x*(lineStart.y - point.y) + point.x*c.rotateCCW().y - lineStart.x*c.rotateCCW().y)/(c.rotateCCW().x*c.y - c.x*c.rotateCCW().y);

	Vect2 Q(lineStart + (lineEnd - lineStart)*t);
	return sqrt((point.x-Q.x)*(point.x-Q.x) + (point.y-Q.y)*(point.y-Q.y));
};
//------------------------------------------------------------------------------------------------
float averageDistance(list<Vect2 > points)
{
	float average = 0;
	int count = (points.size()*(points.size()-1))/2;


	BOOST_FOREACH(Vect2 a, points)
	{
		BOOST_FOREACH(Vect2 b, points)
		{
			if(a != b)
			{
				average += (a-b).length()/count;
			}
		}
	}

	return average;
};
//------------------------------------------------------------------------------------------------
Vect2 averageCenter(list<Vect2 > points)
{
	Vect2 average;
	int count = points.size();

	BOOST_FOREACH(Vect2 a, points)
	{
		average = average + a;
	}

	return average/float(count);
};
//------------------------------------------------------------------------------------------------
list<Vect2 > getNearestPoints(list<Vect2 >::iterator center, list<Vect2 > points, float dist)
{
	list<Vect2 > nearestPoints;
	BOOST_FOREACH(Vect2 p, points)
	{
		if(*center != p)
		{
			if( ((*center) - p).length() <= dist )
			{
				nearestPoints.push_back(p);
			}
		}
	}

	return nearestPoints;
};
//------------------------------------------------------------------------------------------------
#include <iostream>
using namespace std;

list<group> groupPoints(list<group>::iterator start, list<group> points, float dist)
{
	for(list<group>::iterator g = points.begin(); g != points.end(); ++g)
	{
		if(start != g)
		{
			if( ((*g).p - (*start).p).length() <= dist)
			{
				if((*g).nr != (*start).nr)
				{
					if((*g).nr != 0){cout << "WARNING: groupPoints(): point has already a group." << endl;}

					(*g).nr = (*start).nr;
					points = groupPoints(g, points, dist);
				}
			}
		}
	}

	return points;
};
//------------------------------------------------------------------------------------------------
list<list<Vect2 > > seperateByDistance(list<Vect2 > points, float dist)
{
	if(points.empty()){throw Error("illegalOperation", "seperateByDistance() needs at least two points!");}
	if(points.size() == 1)
	{
		cout << "WARNING: seperateByDistance(): Can't seperate a single Point..." << endl;
		list<list<Vect2 > > final;
		final.push_back(points);
		return final;
	}

	list<group> grouped_points;
	BOOST_FOREACH(Vect2 P, points)
	{
		group g;
		g.p = P;
		g.nr = 0;
		grouped_points.push_back(g);
	};

	int curNr = 1;
	for(list<group>::iterator g = grouped_points.begin(); g != grouped_points.end(); ++g)
	{
		if((*g).nr == 0)
		{
			(*g).nr = curNr; ++curNr;
			grouped_points = groupPoints(g, grouped_points, dist);
		}
	}
	grouped_points.sort();

	list<list<Vect2> > finalGroup;

	int last_nr = 0;
	list<Vect2> tmp;
	BOOST_FOREACH(group g, grouped_points)
	{
		if(last_nr != g.nr)
		{
			if(!tmp.empty())
			{
				finalGroup.push_back(tmp);
				tmp.clear();
			}
			last_nr = g.nr;
		}

		tmp.push_back(g.p);
	}
	finalGroup.push_back(tmp);

	return finalGroup;
};
//------------------------------------------------------------------------------------------------
list<Vect2 > simplifyLine(list<Vect2 > line, float maxDist)
{
	//siehe http://de.wikipedia.org/wiki/Douglas-Peucker-Algorithmus
	list<Vect2 >::iterator maxPoint = line.begin();
	float max = 0;

	if(line.size() > 2)
	{
		//Punkt mit dem grössten Abstand suchen:
		for(list<Vect2 >::iterator it = ++line.begin(); it != --line.end(); ++it)
		{
			float dist = orthogonalDistance((*it), (*line.begin()), (*--line.end()));
			if(dist > max)
			{
				max = dist;
				maxPoint = it;
			}
		}

		if(max >= maxDist)
		{
			list<Vect2 > tmp;

			tmp.insert(tmp.begin(), line.begin(), maxPoint);
			list<Vect2 > line1(simplifyLine(tmp, maxDist));

			tmp.clear();
			tmp.insert(tmp.begin(), maxPoint, line.end());
			list<Vect2 > line2(simplifyLine(tmp, maxDist));

			line1.erase(--line1.end());
			line1.splice(line1.end(), line2);
			line = line1;
		}
		else
		{
			Vect2 A(*line.begin());
			Vect2 B(*--line.end());
			line.clear();
			line.push_back(A);
			line.push_back(B);
		}
	}

	return line;
};
//------------------------------------------------------------------------------------------------
list<UniqueBox>
arrangeInBoxRaster(Vect2 dest, list<UniqueBox> boxes, double border)
{
	unsigned int numberOfBoxes		= boxes.size();

	unsigned int numberOfCollumns	= min((int)ceil(sqrt(numberOfBoxes)), 5);
	unsigned int numberOfRows		= numberOfBoxes / numberOfCollumns;

	Vect2 newSize = Vect2(dest.x/numberOfRows, dest.y/numberOfCollumns);

	list<UniqueBox>::iterator b = boxes.begin();
	for(unsigned int y = 0; y < numberOfCollumns && b != boxes.end(); ++y)
	{
		for(unsigned int x = 0; x < numberOfRows && b != boxes.end(); ++x)
		{
			Vect2 origShape = (*b).shape.size;
			(*b).shape.pos.x = x * newSize.x;
			(*b).shape.pos.y = y * newSize.y;
			(*b).shape.size = fitWhileRetainRatio((*b).shape.size, newSize-Vect2(border, border));
			(*b).shape.center(Box((*b).shape.pos, newSize));
			(*b).scale =  (*b).shape.size.x / origShape.x;

			++b;
		}
	}
	return boxes;
};
//------------------------------------------------------------------------------------------------
/**
 * Passt mehrere Rechtecke so an, dass sie in ein grösseres hineinpassen.
 *
 * Übergabe: Eine Liste von Boxen, ein "Ziel"-Rechteck sowie optional noch einen Rand.
 * Die Funktion arbeitet intern mit zwei "Versionen" von sich selber:
 * wird arrangeInBoxLines ohne den Parameter height (bzw. mit NULL) aufgerufen, so wird eine veränderte
 * Liste mit Rechtecken zurückgegeben.
 * Im anderen Fall werden jedoch die Rechtecke nicht verändert, sondern nur die Gesamthöhe berechtet,
 * die "erreicht" wird, bei einer best. Zeilenhöhe. Diese wird im ersten Fall durch Approximation berechnet.
 *
 * @param dest		Das "Zielrechteck", in welches die Rechtecke hineingepasst werden sollen.
 * @param boxes		Eine Liste von Rechtecken, die so skaliert werden, dass sie gemeinsam Platz in dest haben.
 * @param border	Optional kann einen Abstand bestimmt werden, der zwischen den Rechtecken untereinander und
 *					zum Rand bestehen soll.
 *
 * @param stepsize	Bestimmt Optional in welchen Schritten die optimale Zeilenhöhe angenähert werden soll.
 *					Falls extreme Performance gewünscht wird, kann so die Genauigkeit heruntergestellt werden.
 *					Allerdings wäre es in diesem Fall sinnvoller die Funktion zu optimieren ;)
 * @param height	Wird nur intern verwendet.
					In height wird die Gesamthöhe aller "geordneter" Rechtecke  bei einer best. Zeilenhöhe gespeichert.
 * @param rowheight	Wird nur intern verwendet. Wenn nicht angegeben, so wird die Zeilenhöhe angenähert.

 * @return			Eine Liste von Rechtecken mit skalierten Grössen sowie angepassten Koordinaten, so dass
					alle in das Rechteck dest hineinpassen ohne sich oder dest zu überlappen.

 **/
list<UniqueBox>
arrangeInBoxLines (Vect2 dest, list<UniqueBox> boxes, double border, double stepsize, double* height, double rowheight)
{
	if(height == NULL && rowheight == 0)
	{
		/// finde die optimale Zeilenhöhe durch Annäherung:
		rowheight=dest.y; double h = 0;
		do
		{
			arrangeInBoxLines(dest, boxes, border, 0, &h, rowheight);

			rowheight -= stepsize;
		}while(h > dest.y && rowheight > 0);

		rowheight += stepsize;
	}

	double curw = border;
	double curh = border;

	BOOST_FOREACH(UniqueBox b, boxes)
	{
		Vect2 curSize = b.shape.size.scaleYto(rowheight);

		curw += curSize.x+border;
		if(curw > dest.x && curw > curSize.x+border)
		{
			curw  = curSize.x+border*2;
			curh += rowheight+border;
		}

		if(height == NULL)
		{
			b.scale = rowheight / b.shape.size.y;
			b.shape.pos = Vect2(curw - curSize.x - border, curh);
		}
	}
	if(height != NULL)
		*height = curh + rowheight + border;

	return boxes;
};
//------------------------------------------------------------------------------------------------
