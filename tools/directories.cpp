#include "directories.hpp"

//------------------------------------------------------------------------------------------------
string extractFilename(string path)
{
	if(path.find_last_of(dirDelimiter) == path.size()-1)
		path = path.substr(0, path.size()-1);

	int left = path.find_last_of(dirDelimiter);
	return path.substr(left+1, path.find_last_of(".")-left-1);
};
//------------------------------------------------------------------------------------------------
string extractFileExtension(string path)
{
	if(path.find_last_of(dirDelimiter) == path.size()-1)
		path = path.substr(0, path.size()-1);

	int left = path.find_last_of(".");
	return path.substr(left+1);
};
//------------------------------------------------------------------------------------------------
bool existFile(string file)
{
	struct stat buffer;
	if(stat(file.c_str(), &buffer) == 0)
		{return 1;}
	else
		{return 0;}
}
//------------------------------------------------------------------------------------------------
list <string> scanDir(string dir, string extension, bool filterOutPointFiles, bool recursive)
{
	list<string> result;

	if(dir.find_last_of(dirDelimiter) != dir.length()-1){dir += dirDelimiter;} //Pfad sollte mit einem Slash enden
    boost::filesystem::path directory = boost_fs::system_complete(boost_fs::path(dir));
	if(!boost_fs::exists(directory))
		{throw Error("load", "\"" + dir + "\" existiert nicht!");}
	else if(!boost_fs::is_directory(directory))
		{throw Error("load", "\"" + dir + "\" ist kein Ordner!");}
	else
	{
		//cout << "---- scanning \"" << dir << "\" ----" << endl;
		boost_fs::directory_iterator endIter; // default construction yields past-the-end
		for(boost_fs::directory_iterator dirIter(directory); dirIter != endIter; ++dirIter)
		{
#ifdef _WIN32
            string name = dirIter->path().filename();//boost >= 1.36
#else
            string name = dirIter->leaf(); //boost <= 1.35
#endif

            //same goes here:
            //string ext = fs::extension(dirIter->leaf());
#ifdef _WIN32
            string ext = boost_fs::extension(dirIter->path().filename());//extractFileExtension(name);
#else
            string ext = boost_fs::extension(dirIter->leaf());
#endif

            string completePath = dirIter->path().file_string();

            bool isDir = is_directory(dirIter->status());

            if(isDir)
            {
            	if(recursive)
            	{
					if(!filterOutPointFiles || (filterOutPointFiles && (*extractFilename(completePath).begin()) != '.')) //we don't want files which beginn with a point
					{
						//cout << "scanning \"" << completePath << "\" " << endl;
						list<string> tmp = scanDir(completePath, extension, filterOutPointFiles, true);
						result.insert(result.end(), tmp.begin(), tmp.end());
					}
            	}
            }
            else
            {
            	if(filterOutPointFiles && (*extractFilename(completePath).begin()) == '.') //we don't want files which beginn with a point
					continue;
                if(extension != "")
					if(extractFileExtension(completePath) != extension)
						continue;

				result.push_back(completePath);
            }
		};
	}

	return result;
};
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
