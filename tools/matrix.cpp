#include "matrix.hpp"


vector2<Number> operator * (Matrix<3,3>& A, vector2<Number>& v)
{
	Matrix<3,1> X; X.makeIdentity();
	X(0,0) = v.x;
	X(1,0) = v.y;
	X(2,0) = 1;
	X = A*X;
	return vector2<Number>(X(0,0),X(1,0));
};

vector2<Number> operator * (Matrix<2,2>& A, vector2<Number>& v)
{
	Matrix<2,1> X;
	X(0,0) = v.x;
	X(1,0) = v.y;
	X = A*X;
	return vector2<Number>(X(0,0),X(1,0));
};

//---------- FROM STRING -------------------------------------------------------
Matrix<3,3>
transformationMatrix2H(string code)
{
	//cout << "transfomation matrix of '" << code << "':" << endl;

	Matrix<3,3> TM; TM.makeIdentity();

	if(code.empty()) return TM;

	size_t p2 = 0, p = code.find("(");
	if(p == string::npos) throw Error("parse", "non-empty string without command :/");

	while(p != string::npos)
	{
		Matrix<3,3> M; M.makeIdentity();

		string command = trimString(code.substr(p2, p-p2), " ,\r\t\n()");

		p2 = code.find(")", p);
		list<string> params = splitStringWithChars(code.substr(p+1, p2-p-1), ", ", true);

		p = code.find("(",p2);

		if(!params.empty())
		{
			size_t argc = params.size();
			vector<Number> argv(argc);

			size_t i=0;
			BOOST_FOREACH(string& p, params)
			{
				try{
					argv[i] =  boost::lexical_cast<Number>(p);}
				catch (...){
					throw Error("parse", "Cannot convert string '"+p+"' to number.");}

				++i;
			}

			if(command == "matrix")
			{
				if(argc != 6) throw Error("parse", "Found a matrix-transformation-command that has not 6 parameters.");

				M(0,0) = argv[0]; M(0,1) = argv[2]; M(0,2) = argv[4];
				M(1,0) = argv[1]; M(1,1) = argv[3]; M(1,2) = argv[5];
				cout << "transformationmatrix: " << endl << M.toString() << endl;
			}
			else if(command == "translate")
			{
				if(argc > 2) throw Error("parse", "Found a translation-command with more than two parameters.");

				if(argc == 1)
					M = translationMatrix2H(vector2<Number>(argv[0],0));
				else
					M = translationMatrix2H(vector2<Number>(argv[0],argv[1]));
			}
			else if(command == "scale")
			{
				if(argc > 2) throw Error("parse", "Found a scale-command with more than two parameters.");

				if(argc == 1)
					M = scaleMatrix2H(vector2<Number>(argv[0],argv[0]));
				else
					M = scaleMatrix2H(vector2<Number>(argv[0],argv[1]));
			}
			else if(command == "skewX")
			{
				if(argc > 1) throw Error("parse", "Found a skewX-command with more than one parameter.");
				M = skewXMatrix2H(argv[0]);
			}
			else if(command == "skewY")
			{
				if(argc > 1) throw Error("parse", "Found a skewY-command with more than one parameter.");
				M = skewYMatrix2H(argv[0]);
			}
			else if(command == "rotate")
			{
				M = rotationMatrix2H(argv[0]);

				if(argc == 3)
				{
					Matrix<3,3> T1 = translationMatrix2H(vector2<Number>(argv[1],argv[2]));
					Matrix<3,3> T2 = translationMatrix2H(vector2<Number>(-argv[1],-argv[2]));

					M = (T1 * M) * T2;
				}
				else if(argc != 1)
					throw Error("parse", "Found a rotate-command with "+ToString(argc)+" parameters.");
			}
			else
				throw Error("parse", "Found an unknown transformation-command: '"+command+"'.");

			TM = TM * M;
		}
		else
			throw Error("parse", "Command '"+command+"' has no parameters! Das gaht gar nid.");
	}

	//cout << TM.toString() << endl;

	return TM;
};


//---------- ROTATION ----------------------------------------------------------
// H = Homogeneous
Matrix<3,3> rotationMatrix2H(Number angle)
{
	Matrix<3,3> M; M.makeIdentity();

	M(0,0) = cos(angle); M(0,1) = -sin(angle);
	M(1,0) = sin(angle); M(1,1) = cos(angle);

	return M;
};

//---------- TRANSLATION -------------------------------------------------------
Matrix<3,3> translationMatrix2H(vector2<Number> direction)
{
	Matrix<3,3> M; M.makeIdentity();

	M(0,2) = direction.x;
	M(1,2) = direction.y;

	return M;
};

//---------- SCALING -----------------------------------------------------------
Matrix<3,3> scaleMatrix2H(vector2<Number> factor)
{
	Matrix<3,3> M; M.makeIdentity();

	M(0,0) = factor.x;
	M(1,1) = factor.y;

	return M;
};

//---------- SKEWING -----------------------------------------------------------
Matrix<3,3> skewXMatrix2H(Number angle)
{
	Matrix<3,3> M; M.makeIdentity();

	M(0,1) = tan(angle);

	return M;
};

Matrix<3,3> skewYMatrix2H(Number angle)
{
	Matrix<3,3> M; M.makeIdentity();

	M(1,0) = tan(angle);

	return M;
};

//------------------------------------------------------------------------------
