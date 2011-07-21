#ifndef DIRECTORIES_HPP_INCLUDED
#define DIRECTORIES_HPP_INCLUDED

#include <string>
#include <list>
#include <sys/stat.h>
#include <boost/filesystem.hpp>

#include "tools/errorclass.hpp"

namespace boost_fs = boost::filesystem;

using namespace std;

//------------------------------------------------------------------------------------------------
#ifdef _WIN32
	const std::string dirDelimiter = "\\";
#else
	const std::string dirDelimiter = "/";
#endif
//------------------------------------------------------------------------------------------------
string extractFilename(string path);
//------------------------------------------------------------------------------------------------
string extractFileExtension(string path);
//------------------------------------------------------------------------------------------------
bool existFile(string file);
//------------------------------------------------------------------------------------------------
list <string> scanDir(string dir, string extension="", bool filterOutPointFiles=true, bool recursive=true);
//------------------------------------------------------------------------------------------------

#endif //DIRECTORIES_HPP_INCLUDED
