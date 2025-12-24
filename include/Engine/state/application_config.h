#pragma once
#include <cstdint>
#include <string>
#include <vector>

using std::vector;
using std::string;

class ApplicationConfig {
public:
	ApplicationConfig(
		string application_name,
		string application_description,
		vector<string> application_authors,
		int application_version_major,
		int application_version_minor,
		int application_version_patch,
		string application_version_identifier
	);
	// TODO: Make a function that loads this in from a "scene config" file

	string application_name;
	string application_description;
	vector<string> application_authors;
	int application_version_major;
	int application_version_minor;
	int application_version_patch;
	string application_version_identifier;
};