#pragma once
#ifndef REBOOTCONFIG_H
#define REBOOTCONFIG_H

#pragma once
#include <map>
#include <sstream>

namespace reb {

	//loads a file when the game starts up that holds general information that the whole game can use,
	//most are optional, but at this point the rendering booleans for drawing rigidbodies, particles and sprites
	//are used automatically (this should change in the future obviously since players shouldnt be able to do this)
	class Config {
	public:
		//constructor
		Config() {
			loadConfig();
		}
		//enumeration for config values
		enum configKey { 
			FPS, 
			UPS, 
			WINDOW_WIDTH, 
			WINDOW_HEIGHT, 
			FULLSCREEN, 
			WINDOW_TITLE, 
			CAMERA_WIDTH, 
			CAMERA_HEIGHT, 
			DRAWRIGIDBODIES, 
			DRAWSPRITES, 
			DRAWPARTICLES,
			GUIWIDTH,
			GUIHEIGHT
		};
	private:
		//keeps the directory of the config file as a constant
		const std::string CONFIGDIR = "Content/Configs/config.xml";
		//map holding config values as strings
		std::map<configKey, std::string> m_config;
		//map hoding config key enums and their names within the actual config file
		std::map<std::string, configKey> m_configNames{
			{"fps", configKey::FPS},
			{"ups", configKey::UPS},
			{"winW", configKey::WINDOW_WIDTH},
			{"winH", configKey::WINDOW_HEIGHT},
			{"fullScr", configKey::FULLSCREEN},
			{"winTit", configKey::WINDOW_TITLE},
			{"camW", configKey::CAMERA_WIDTH},
			{"camH", configKey::CAMERA_HEIGHT},
			{"showRigids", configKey::DRAWRIGIDBODIES},
			{"showSpr", configKey::DRAWSPRITES},
			{"showPart", configKey::DRAWPARTICLES},
			{"guiW", configKey::GUIWIDTH},
			{"guiH", configKey::GUIHEIGHT}
		};
		//loads the main game config file
		void loadConfig();
	public:
		//returns the value of the requested config value as the requested data type
		template <typename T>
		T getConfigValue(configKey key)const {
			T temp;
			std::istringstream(m_config.at(key)) >> temp;
			return temp;
		};

		//specific templated function for booleans to specify boolAlpha
		template<>
		bool getConfigValue<bool>(configKey key)const {
			bool temp;
			std::istringstream(m_config.at(key)) >> std::boolalpha >> temp;
			return temp;
		};

		//specific templated function for std::string because that can just return the value in the map
		template<>
		std::string getConfigValue<std::string>(configKey key)const {
			return m_config.at(key);
		};

	};

	
}

#endif