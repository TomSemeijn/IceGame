#include "Config.h"
#include <rapidxml/rapidxml_utils.hpp>
#include "Functions.h"

namespace reb {
	//loads the config file
	void Config::loadConfig() {
		//gets the xml file
		rapidxml::file<> file(CONFIGDIR.data());
		//parses the xml file
		rapidxml::xml_document<> doc;
		doc.parse<0>(file.data());
		//goes through the nodes to set the values within the config file
		rapidxml::xml_node<> *node = doc.first_node()->first_node();
		do {
			//verifies the current node is a value node
			if (std::strcmp(node->name(), "val") == 0) {
				//gets the first attribute (should be the name)
				rapidxml::xml_attribute<> attr = *node->first_attribute();
				//tries to put the found config value into the config map
				try {
					m_config[m_configNames.at(std::string(attr.value()))] = std::string(attr.next_attribute()->value());
				}
				//catches the out_of_range exception to display a console message in case the name of
				//the config value as given in the config file wasnt defined in the configNames map
				catch (std::out_of_range) {
					writeLn(
						"The config value by the name of '",
						attr.value(),
						"' wasnt defined in the configNames map and thus wasnt loaded into the config map");
				}
			}
		} //sets the current node to the next sibling and terminates when there are no nodes left
		while (node = node->next_sibling());
	}
}