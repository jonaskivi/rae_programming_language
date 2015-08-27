#include "rae_helpers.hpp"

bool isWhiteSpace(const char& set_char)
{
	if( set_char == ' ' || set_char == '\n' || set_char == '\t' || set_char == '\r')
	return true;
	//else
	return false;
}

bool isWhiteSpace(const string& set)
{
	if(set.size() == 0)
	return false;

	for(uint i = 0; i < set.size(); ++i)
	{
		if( !isWhiteSpace(set[i]) )
		{
			return false;
		}
	}
	return true;
}

// Remove a char from string, return resulting string
string removeFromString(const string& set, const char rem)
{
	//std::string::size_type pos = set.find_first_of(rem);
	//string res = set.r
	string str = set;
	str.erase(std::remove(str.begin(), str.end(), rem), str.end());
	return str;
}

string replaceCharInString(string str, const string& replace, char ch)
{
	// set our locator equal to the first appearance of any character in replace
	size_t found = str.find_first_of(replace);

	while (found != string::npos)
	{ // While our position in the sting is in range.
		str[found] = ch; // Change the character at position.
		found = str.find_first_of(replace, found+1); // Relocate again.
	}

	return str; // return our new string.
}

// Basically adds ./ to begin and / to end of a directory string in some circumstances.
string makeDirPathBetter(const string& set_path)
{
	if (set_path.size() < 1)
		return set_path;
	
	string res = set_path;
	
	if (res.back() != '/' && res.back() != '\\')
		res = res + "/";
	
	if (res.front() == '/' || res.front() == '.')
		return res;
	
#ifdef _WIN32
	// Check for windows drive:
	if (res.size() > 3) // Windows drive letter, a colon, and dir separator
	{
		if ( res[1] == ':' )
			return res;
	}
#endif

	res = "./" + res;
	return res;
}

PathType::e checkPathType(const string& currentFilenamePath)
{
	PathType::e path_type;
	path_type = PathType::DOES_NOT_EXIST; //we're a bit pessimistic here.

	struct stat s;

	if (stat(currentFilenamePath.c_str(), &s) == 0)
	{
		if (s.st_mode & S_IFDIR)
		{
			//it's a directory
			path_type = PathType::DIRECTORY;
			cout << currentFilenamePath << " : Is a directory.\n";
		}
		else if (s.st_mode & S_IFREG)
		{
			//it's a file
			path_type = PathType::FILE;
			cout << currentFilenamePath << " : Is a file.\n";
		}
		else
		{
			//something else
			cout << currentFilenamePath << " : Is something else or does not exist.\n";
		}
	}
	else
	{
		//error
		cout << currentFilenamePath << " : Does not exist.\n";
	}

	return path_type;
}

int createPathIfNotExist(const string& set_path)
{
	if (checkPathType(set_path) != PathType::DOES_NOT_EXIST)
		return 0;

	int ret = createPath(set_path);
	if ( ret == 0)
	{
		cout << "Created folder: " << set_path << "\n";
	}
	else
	{
		cout << "Failed to create folder: " << set_path << "\n";
	}
	return ret;
}

int createPath(const std::string& set)
{
	size_t pre = 0, pos;
	std::string dir;
	int mdret;
	std::string s = set;

	if (s[s.size() - 1] != '/'){
		// force trailing / so we can handle everything in loop
		s += '/';
	}

	while ((pos = s.find_first_of('/', pre)) != std::string::npos)
	{
		dir = s.substr(0, pos++);
		pre = pos;
		if (dir.size() == 0) continue; // if leading / first time is 0 length

#ifdef WIN32
		if ((mdret = _mkdir(dir.c_str()/*, mode*/)) && errno != EEXIST)
 
#else //#if defined(__APPLE_) || defined(LINUX)
        mode_t mode = 0755;
        if ((mdret = mkdir(dir.c_str(), mode)) && errno != EEXIST)
#endif
		{
			return mdret;
		}
	}
	return mdret;
}

/*
void createPath(string set_path)
{
	std::size_t found = set_path.find_last_of("/\\");
	if (found > set_path.size())
		set_path = set_path + "/";
	else if (found != set_path.size() - 1)
	{
		string temp_path = parentPath(set_path);
		if (temp_path != "")
			set_path = temp_path;
	}

	int status = mkdir( set_path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
}
*/
string parentPath(const string& original_path)
{
	std::size_t found = original_path.find_last_of("/\\");

	if (found > original_path.size())
		return ""; // no path separators, so empty string is the parent dir.

	//std::cout << " path: " << str.substr(0,found) << '\n';
	//std::cout << " file: " << str.substr(found+1) << '\n';
	//cout << "parent path is: " << original_path.substr(0, found) << "\n";
	return original_path.substr(0, found);
}

string getFilenameFromPath(const string& original_path)
{
	std::size_t found = original_path.find_last_of("/\\");
	//std::cout << " path: " << str.substr(0,found) << '\n';
	//std::cout << " file: " << str.substr(found+1) << '\n';
	//cout << "file is: " << original_path.substr(found + 1) << "\n";
	return original_path.substr(found + 1);
}

string replaceExtension(const string& original_path, const string& new_extension)
{
	// Check if extension has dot.
	string ext = new_extension;
	std::size_t found_ext_dot = new_extension.find_last_of(".");
	if (found_ext_dot > ext.size()) // no dot
		ext = "." + ext;

	std::size_t found = original_path.find_last_of(".");
	if (found > original_path.size() || found == 0) // no original extension, or/and starts with a dot.
	{
		//cout << "replaceExtension1: " << original_path << " + " << new_extension << " = " << original_path + new_extension << "\n";
		return original_path + ext;
	}

	//cout << "replaceExtension: " << original_path << " -> " << original_path.substr(0, found) << " + " << new_extension << " = " << original_path.substr(0, found) + new_extension << "\n";

	return original_path.substr(0, found) + ext;
}

