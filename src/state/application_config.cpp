#include "Engine/state/application_config.h"

ApplicationConfig::ApplicationConfig(
	string application_name,
	string application_description,
	vector<string> application_authors,
	int application_version_major,
	int application_version_minor,
	int application_version_patch,
	string application_version_identifier
)
: application_name(application_name),
  application_description(application_description),
  application_authors(application_authors),
  application_version_major(application_version_major),
  application_version_minor(application_version_minor),
  application_version_patch(application_version_patch),
  application_version_identifier(application_version_identifier)
{

}