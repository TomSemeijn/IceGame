#ifndef REBOOTLIGHTTYPEINFO_H
#define REBOOTLIGHTTYPEINFO_H

#pragma once
#include <string>

namespace reb {

	//the info necessary to register a lightsource type
	struct LightTypeInfo {
		std::string name;
		int maxInstances;
		std::string lightFunc;
		std::string extraUniforms;
	};

}

#endif