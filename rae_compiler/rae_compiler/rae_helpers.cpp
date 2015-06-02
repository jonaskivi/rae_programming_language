#include "rae_helpers.hpp"

PathType::e checkPathType(string currentFilenamePath)
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
		}
		else if (s.st_mode & S_IFREG)
		{
			//it's a file
			path_type = PathType::FILE;
		}
		else
		{
			//something else
		}
	}
	else
	{
		//error
	}

	return path_type;
}

int createPathIfNotExist(string set_path)
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

int createPath(std::string s)
{
	//mode_t mode = 0755; // No mode_t in Visual Studio 2013...
	size_t pre = 0, pos;
	std::string dir;
	int mdret;

	if (s[s.size() - 1] != '/'){
		// force trailing / so we can handle everything in loop
		s += '/';
	}

	while ((pos = s.find_first_of('/', pre)) != std::string::npos)
	{
		dir = s.substr(0, pos++);
		pre = pos;
		if (dir.size() == 0) continue; // if leading / first time is 0 length
		if ((mdret = _mkdir(dir.c_str()/*, mode*/)) && errno != EEXIST)
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
string parentPath(string original_path)
{
	std::size_t found = original_path.find_last_of("/\\");

	if (found > original_path.size())
		return ""; // no path separators, so empty string is the parent dir.

	//std::cout << " path: " << str.substr(0,found) << '\n';
	//std::cout << " file: " << str.substr(found+1) << '\n';
	//cout << "parent path is: " << original_path.substr(0, found) << "\n";
	return original_path.substr(0, found);
}

string getFilenameFromPath(string original_path)
{
	std::size_t found = original_path.find_last_of("/\\");
	//std::cout << " path: " << str.substr(0,found) << '\n';
	//std::cout << " file: " << str.substr(found+1) << '\n';
	//cout << "file is: " << original_path.substr(found + 1) << "\n";
	return original_path.substr(found + 1);
}

string replaceExtension(string original_path, string new_extension)
{
	// Check if extension has dot.
	std::size_t found_ext_dot = new_extension.find_last_of(".");
	if (found_ext_dot > new_extension.size()) // no dot
		new_extension = "." + new_extension;

	std::size_t found = original_path.find_last_of(".");
	if (found > original_path.size() || found == 0) // no original extension, or/and starts with a dot.
	{
		//cout << "replaceExtension1: " << original_path << " + " << new_extension << " = " << original_path + new_extension << "\n";
		return original_path + new_extension;
	}

	//cout << "replaceExtension: " << original_path << " -> " << original_path.substr(0, found) << " + " << new_extension << " = " << original_path.substr(0, found) + new_extension << "\n";

	return original_path.substr(0, found) + new_extension;
}

