#ifndef MATRIX_HPP_INCLUDED
#define MATRIX_HPP_INCLUDED

#include <vector>
#include <math.h>

#include <boost/foreach.hpp>
#include <boost/array.hpp>
#include <boost/multi_array.hpp>
#include <boost/lexical_cast.hpp>

#include "tools/geometry.hpp"
#include "tools/errorclass.hpp"

using namespace std;

typedef double Number;
typedef boost::multi_array<Number, 2> Array;
typedef Array::index Index;

///           Zeilen    Spalten
template <Index ROWS, Index COLS>
class Matrix
{
	Array M;

	void throwOutOfBounds(Index r, Index c)
	{
		 throw Error("outOfBounds", string("Index (")+r+" or "+c+") too large for matrix<"+ROWS+","+COLS+">.");
	}

public:
	Matrix()
	 : M(boost::extents[ROWS][COLS])
	 {this->makeIdentity();}

	string toString()
	{
		string str;
		for(Index r = 0; r < ROWS; ++r)
		{
			str.append("[");
			for(Index c = 0; c < COLS; ++c)
			{
				str.append("\t");

				str.append(ToString(M[r][c]));

				if(c < COLS-1) str.append(",");
			}
			str.append(" ]\n");
		}

		return str;
	}

	inline Number&
	operator() (Index r, Index c)
	{
		if(c>=COLS or r >=ROWS) throwOutOfBounds(r,c);
		return M[r][c];
	}

	inline Number
	operator() (Index r, Index c) const
	{
		if(c>=COLS or r >=ROWS) throwOutOfBounds(r,c);
		return M[r][c];
	}

	Matrix& makeIdentity()
	{
		for(Index r=0; r<ROWS; ++r)
		for(Index c=0; c<COLS; ++c)
			M[r][c] = (r==c?1:0);

		return *this;
	}

	Matrix& makeZero()
	{
		for(Index r=0; r<ROWS; ++r)
		for(Index c=0; c<COLS; ++c)
			M[r][c] = 0;

		return *this;
	}

	void swapRows(Index r1, Index r2)
	{
		if(r1>=ROWS or r2>=ROWS) throwOutOfBounds(r1,r2);

		if(r1 != r2)
			for(Index c=0; c<COLS; ++c)
			{
				Number tmp = M[r1][c];
				M[r1][c]   = M[r2][c];
				M[r2][c]   = tmp;
			}
	}

	void swapColumns(Index c1, Index c2)
	{
		if(c1>=COLS or c2>=COLS) throwOutOfBounds(c1,c2);

		if(c1 != c2)
			for(Index r=0; r<ROWS; ++r)
			{
				Number tmp = M[r][c1];
				M[r][c1]   = M[r][c2];
				M[r][c2]   = tmp;
			}
	}

	void multiplyScalar(Number n)
	{
		for(Index r=0; r<ROWS; ++r)
		for(Index c=0; c<COLS; ++c)
			M[r][c] = M[r][c] * n;
	}

	/// calculates this*B
	template<Index COLS_B>
	Matrix<ROWS, COLS_B> multiply(Matrix<COLS, COLS_B>& B)
	{
		Matrix<ROWS, COLS_B> result;

		for(Index r=0; r<ROWS; ++r)
		for(Index c=0; c<COLS_B; ++c)
		{
			Number tmp=0;
			for(Index t=0; t<COLS; ++t)
				tmp += M[r][t] * B(t,c);

			result(r,c) = tmp;
		}

		return result;
	}

	Matrix<ROWS,COLS> add(Matrix<ROWS,COLS> B)
	{
		Matrix<ROWS,COLS> result;

		for(Index r=0; r<ROWS; ++r)
		for(Index c=0; c<COLS; ++c)
			result(r,c) = M[r][c] + B(r,c);

		return result;
	}


	bool gauss()
	{
		cout << "WARNING: using UNTESTED gauss-algorithm!" << endl;

		for(Index c=0; c<COLS; ++c)
		{
			/// find pivot
			Index piv=0; Number max=0;
			for(Index r=c; r<ROWS; ++r)
				if(abs(M[r][c]) > max){max = abs(M[r][c]); piv = r;}

			if(max == 0) return false; /// no [unique] solution

			/// swap found pivot row with current
			this->swapRows(piv, c); ///< can't fail as c<ROWS or else max would have been == 0

			/// normalize pivot row
			Number p = M[c][c];
			M[c][c]  = 1;
			for(Index crest=c+1; crest<COLS; ++crest)
				M[c][crest] = M[c][crest] / p;

			/// Eliminate rest of column
			for(Index subrow=c+1; subrow<ROWS; ++subrow)
			{
				Number first = M[subrow][c];
				for(Index subcol=c;   subcol<COLS; ++subcol)
				{
					M[subrow][subcol] = M[subrow][subcol] - first*M[c][subcol];
				}
			}
		}
		return true;
	};
};


template <Index ROWS_A, Index COLS, Index COLS_B> inline
Matrix<ROWS_A, COLS_B> operator * (Matrix<ROWS_A,COLS> A, Matrix<COLS,COLS_B>& B) {return A.multiply(B);}

vector2<Number> operator * (Matrix<3,3>& A, vector2<Number>& v);
vector2<Number> operator * (Matrix<2,2>& A, vector2<Number>& v);

//------------------------------------------------------------------------------
//---------- HELPER FUNCTIONS --------------------------------------------------
//------------------------------------------------------------------------------
#include "tools/stringhelpers.hpp"
Matrix<3,3> transformationMatrix2H(string code);

//---------- ROTATION ----------------------------------------------------------
// H = Homogeneous
Matrix<3,3> rotationMatrix2H(Number angle);

//---------- TRANSLATION -------------------------------------------------------
Matrix<3,3> translationMatrix2H(vector2<Number> direction);

//---------- SCALING -----------------------------------------------------------
Matrix<3,3> scaleMatrix2H(vector2<Number> factor);

//---------- SKEWING -----------------------------------------------------------
Matrix<3,3> skewXMatrix2H(Number angle);
Matrix<3,3> skewYMatrix2H(Number angle);

//------------------------------------------------------------------------------



#endif // MATRIX_HPP_INCLUDED
