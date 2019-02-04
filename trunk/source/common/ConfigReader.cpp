#include "ConfigReader.h"
#include "util.h"
#include "string.h"
#include "log.h"

#define XML_SAFE_ATTR_VALUE(node, name) (node)->first_attribute(name) == NULL ? NULL : (node)->first_attribute(name)->value()


ConfigReader::ConfigReader(Server* serv, Database* db) {
	assert(serv);
	assert(db);

	server = serv;
	DB = db;
}

ConfigReader::~ConfigReader() {

}

char* ConfigReader::GetFileContent(const char *file) {
	uint32_t count;
	uint32_t size;
	char *buf;
	FILE *f;

	if ((f = fopen(file, "rb")) == NULL) {
		fprintf(stderr, "Could not open '%s': %s\n", file, appStrError().c_str());
		return NULL;
	}

	_fseeki64(f, 0, SEEK_END);
	size = (uint32_t)_ftelli64(f);
	rewind(f);

	if ((buf = (char *)malloc(size + 1)) == NULL) {
		fclose(f);
		fprintf(stderr, "Out of memory trying to allocate %ld bytes for config reader buffer", size + 1);
		return NULL;
	}

	count = (uint32_t)fread(buf, 1, size, f);
	fclose(f);

	if (size != count) {
		free(buf);
		fprintf(stderr, "Tried to read %u bytes from %s but only read %u", size, file, count);
		return NULL;
	}

	buf[count] = '\0';
	return buf;
}

bool ConfigReader::ReadConfig(std::string file) {
	xml_node<> *root, *node;
	xml_document<>* doc = new xml_document<>;
	char *buf;

	if ((buf = GetFileContent(file.c_str())) == NULL)
		return false;

	doc->parse<0>(buf);
	root = doc->first_node("eq2emu");

	if (root != NULL) {
		if ((node = root->first_node("server")) != NULL)
			ReadServerConfig(node);
		if ((node = root->first_node("database")) != NULL)
			ReadDatabaseConfig(node);
		if ((node = root->first_node("logging")) != NULL)
			ReadLogConfig(node);
	}

	free(buf);
	return true;
}

void ConfigReader::ReadServerConfig(xml_node<> *node) {
	const char *val;

	if ((val = XML_SAFE_ATTR_VALUE(node, "host")) != NULL)
		server->SetHost(val);
	if ((val = XML_SAFE_ATTR_VALUE(node, "port")) != NULL && IsUnsignedInt(val))
		server->SetPort(atoul(val));
}

void ConfigReader::ReadDatabaseConfig(xml_node<> *node) {
	const char *val;

	if ((val = XML_SAFE_ATTR_VALUE(node, "host")) != NULL)
		DB->SetHost(val);
	if ((val = XML_SAFE_ATTR_VALUE(node, "port")) != NULL && IsUnsignedInt(val))
		DB->SetPort(atoul(val));
	if ((val = XML_SAFE_ATTR_VALUE(node, "user")) != NULL)
		DB->SetUser(val);
	if ((val = XML_SAFE_ATTR_VALUE(node, "password")) != NULL)
		DB->SetPassword(val);
	if ((val = XML_SAFE_ATTR_VALUE(node, "database")) != NULL)
		DB->SetDatabase(val);
}

void ConfigReader::ReadLogConfig(xml_node<> *node) {
	xml_node<> *node_category, *node_type;
	const char *val1, *val2;
	LogCategory category;
	LogType type;
	int level, output, color;
	bool on;

	if ((val1 = XML_SAFE_ATTR_VALUE(node, "format")) != NULL && strcasecmp(val1, "xml") == 0)
		LogSetFormat(LOG_FORMAT_XML);
	if ((val1 = XML_SAFE_ATTR_VALUE(node, "split")) != NULL && strcasecmp(val1, "true") == 0)
		LogSetSplit(true);
	if ((val1 = XML_SAFE_ATTR_VALUE(node, "pid")) != NULL && strcasecmp(val1, "true") == 0)
		LogSetPID(true);

	for (node_category = node->first_node("category"); node_category != NULL; node_category = node_category->next_sibling()) {
		for (node_type = node_category->first_node("type"); node_type != NULL; node_type = node_type->next_sibling()) {
			val1 = XML_SAFE_ATTR_VALUE(node_category, "name");
			val2 = XML_SAFE_ATTR_VALUE(node_type, "name");

			if (val1 != NULL && val2 != NULL) {
				category = LogCategoryByName(val1);
				type = LogTypeByName(val2);

				if (category != LOG_INVALID && type != LOG_TYPE_INVALID) {
					on = false;
					level = 0;
					output = 0;
					color = LOG_WHITE;

					if ((val1 = XML_SAFE_ATTR_VALUE(node_type, "level")) != NULL)
						level = atoi(val1);
					if ((val1 = XML_SAFE_ATTR_VALUE(node_type, "enabled")) != NULL && strcasecmp(val1, "true") == 0)
						on = true;
					if ((val1 = XML_SAFE_ATTR_VALUE(node_type, "logs")) != NULL)
						output = atoi(val1);

					if ((val1 = XML_SAFE_ATTR_VALUE(node_type, "color")) != NULL) {
						if (strcasecmp(val1, "whitebold") == 0)        color = LOG_WHITE_BOLD;
						else if (strcasecmp(val1, "red") == 0)         color = LOG_RED;
						else if (strcasecmp(val1, "redbold") == 0)     color = LOG_RED_BOLD;
						else if (strcasecmp(val1, "green") == 0)       color = LOG_GREEN;
						else if (strcasecmp(val1, "greenbold") == 0)   color = LOG_GREEN_BOLD;
						else if (strcasecmp(val1, "blue") == 0)        color = LOG_BLUE;
						else if (strcasecmp(val1, "bluebold") == 0)    color = LOG_BLUE_BOLD;
						else if (strcasecmp(val1, "yellow") == 0)      color = LOG_YELLOW;
						else if (strcasecmp(val1, "yellowbold") == 0)  color = LOG_YELLOW_BOLD;
						else if (strcasecmp(val1, "cyan") == 0)        color = LOG_CYAN;
						else if (strcasecmp(val1, "cyanbold") == 0)    color = LOG_CYAN_BOLD;
						else if (strcasecmp(val1, "magenta") == 0)     color = LOG_MAGENTA;
						else if (strcasecmp(val1, "magentabold") == 0) color = LOG_MAGENTA_BOLD;
					}

					LogSet(category, type, on, level, output, color);
				}
			}
		}
	}
}