/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpodack <rpodack@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/18 13:47:56 by tstahlhu          #+#    #+#             */
/*   Updated: 2025/01/11 18:07:38 by rpodack          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Config.hpp"
#include "../includes/Location.hpp"

Config::ErrorConfig::ErrorConfig(std::string const &msg) : _msg(msg){}
Config::ErrorConfig::~ErrorConfig()throw(){}

const char *Config::ErrorConfig::what()const throw(){
	return(_msg.c_str());
}

Config::Config() : _countBrace(0) {}
Config::~Config() {
	//  for (ServerBlock* loc : _servers) {
    //     delete loc;
    // }
}
Config::Config(const Config& a) {
	*this = a;
}
Config&	Config::operator=(const Config&a) {
	_countBrace = a._countBrace;
	_servers = a._servers;
	return *this;
}

std::vector<ServerBlock> Config::getServers()const{
	return(_servers);
}

bool 		isServerBlock(const std::string& line) {
    // Check if the line starts with "server", followed by optional whitespace and "{"
    size_t i = 0;
    
    // Skip leading whitespace
    while (i < line.size() && std::isspace(line[i])) {
        i++;
    }

    // Check if the next part is "server"
    std::string serverStr = "server";
    for (size_t j = 0; j < serverStr.size(); ++j) {
        if (i >= line.size() || line[i] != serverStr[j]) {
            return false;
        }
        i++;
    }

    // Skip whitespace between "server" and "{"
    while (i < line.size() && std::isspace(line[i])) {
        i++;
    }

    // Check if the next character is '{'
    return (i < line.size() && line[i] == '{');
}

void 	Config::countBraces(std::string line, bool inLocationBlock){
	for (size_t i = 0; line[i]; i++)
	{
		if (line[i] == '{')
			_countBrace++;
		else if (line[i] == '}')
			_countBrace--;
		if (inLocationBlock){
			if (_countBrace < 1 || _countBrace > 2){
				throw Config::ErrorConfig("Error: Braces mismatch");
			}
			continue;
		}
		if (_countBrace < 0 || _countBrace > 1)
			throw Config::ErrorConfig("Error: Braces mismatch");	}
}

void	Config::parse( const char* configFile ) {

	bool	inServerBlock = false;
	bool 	inLocationBlock = false;

	std::ifstream	file(configFile);
	if (file.is_open()) {
		std::string	line;
		ServerBlock *SB = NULL;
		Location *LB = NULL;
		while (std::getline(file, line)) {
			while (line[0] == ' ' || line[0] == 9) // skip space and tab
				line.erase(line.begin());
			if (line.empty() || line[0] == '#' || line[0] == ' ')
				continue;
			line = removeComment(line);
			if (isServerBlock(line) && !inServerBlock){
				inServerBlock = true;
				countBraces(line, inLocationBlock);
				SB = new ServerBlock();
			}
			else if (inServerBlock){
				// if EOF found without throw -> check at the end if serverblock obj exists
				if (inLocationBlock){
					countBraces(line, inLocationBlock);
					LB->initVarLocation(line);
				}
				if (SB->isLocationBlock(line) && !inLocationBlock){
					inLocationBlock = true;
					countBraces(line, inLocationBlock);
					LB = new Location();
					LB->parseLocURI(line);
				}
				else if (!inLocationBlock){
						countBraces(line, inLocationBlock);
						SB->initVarServerBlock(line);
				}
			}
			// do locationBraces match ?
			if (_countBrace == 1 && inLocationBlock){
				inLocationBlock = false;
				SB->_LocVec.push_back(*LB);
				delete LB;
				LB = NULL;
			}

			// do braces match ?
			if (_countBrace == 0 && inServerBlock){
				inServerBlock = false;
				_servers.push_back(*SB);
				delete SB;
				SB = NULL;
			}
		}
	}
	else {
		throw FileOpenErrorException();
	}
	file.close();
	if (_servers.size() == 0){
		throw Config::ErrorConfig("No ServerBlock in Configfile");
	}
	checkPorts();
	return ;
}

void	Config::checkPorts() {
	
	std::vector<int>	allports;
	for (size_t i = 0; i < _servers.size(); i++) {
		for (size_t j = 0; j < _servers[i]._port.size(); j++) {
			if (std::find(allports.begin(), allports.end(), _servers[i]._port[j]) != allports.end())
				throw Config::ErrorConfig("Config Parsing Error: identical ports detected");
			allports.push_back(_servers[i]._port[j]);
		}
			
	}
}

std::string Config::removeComment(std::string line){
	std::string result = "";
	int i = 0;
	while (line[i])
	{
		if (line[i] == '#')
			break;
		result += line[i];
		i++;
	}
	return(result);
}