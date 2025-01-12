/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpodack <rpodack@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 18:16:16 by tstahlhu          #+#    #+#             */
/*   Updated: 2025/01/11 18:19:27 by rpodack          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Location.hpp"

Location::Location() : _LocationIndex(""), _LocationRoot(""), _LocationURI(""), _LocationAutoindex(false), _LocationReturn(""){}
Location::~Location(){}
Location::Location(const Location& a){
	*this = a;
}

Location	&Location::operator=(const Location &a){
	_LocationRoot = 		a._LocationRoot;
	_LocationIndex = 		a._LocationIndex;
	_LocationAllowMethod = 	a._LocationAllowMethod;
	_LocationURI = 			a._LocationURI;
	_LocationAutoindex = 	a._LocationAutoindex;
	_LocationReturn = 		a._LocationReturn;
	return(*this);
}

Location::ErrorLB::ErrorLB(std::string const &msg) : _msg(msg){}
Location::ErrorLB::~ErrorLB()throw(){}
const char *Location::ErrorLB::what()const throw(){
	return(_msg.c_str());
}

void		Location::initVarLocation(std::string line){
	std::string tmp = "";
	std::string restLine = "";

	size_t i = 0;
	for (; i < line.size(); i++)
	{
		if (line[i] == ' ' || line[i] == '\t')
			break;
		tmp += line[i];
	}
	for (; line[i] == ' ' || line[i] == '\t'; i++)
		;
	
	for(; i < line.size(); i++)
		restLine += line[i];
	
	std::stringstream sLine(restLine);
	checkVarLoc(tmp, sLine);
}

void 	Location::checkVarLoc(std::string &tmp, std::stringstream &sLine){

	if (tmp == "root"){
		if (std::getline(sLine, tmp, ';')){
			std::string fullPath = "";
			if (sLine.eof()) 
       			throw Location::ErrorLB("Error in initVarLocation: missing semicolon " + tmp);
			// is root valid
			fullPath = removeLeadingDot(trim(tmp));
			if (!fullPath.empty()){
				char *cdirname = get_current_dir_name();
				if (cdirname == NULL)
					throw ServerBlock::ErrorSB("Error in initVarLocaton: get_current_dir_name failed ");
				fullPath = cdirname + fullPath + "/";
				free(cdirname);
			}
			DIR *dir = opendir((fullPath).c_str());
			if (dir == NULL){
				closedir(dir);
				throw ServerBlock::ErrorSB("Error in initVarConfig: not a valid root dir " + fullPath);
			}
			closedir(dir);
			_LocationRoot = trim(tmp);
		}
	}
	else if (tmp == "index"){
		if (std::getline(sLine, tmp, ';')){
			if (sLine.eof()) 
       			throw Location::ErrorLB("Error in initVarLocation: missing semicolon " + tmp);
			_LocationIndex = trim(tmp);
		}
	}
	else if (tmp == "allow_methods"){
		if (std::getline(sLine, tmp, ';')){
			if (sLine.eof()) 
       			throw Location::ErrorLB("Error in initVarLocation: missing semicolon " + tmp);
			std::istringstream mLine(tmp);
			std::string Method;
			while (mLine >> Method){
				_LocationAllowMethod.push_back(Method);
			}
		}
	}
	else if (tmp == "autoindex"){
		if (std::getline(sLine, tmp, ';')){
			if (sLine.eof()) 
       			throw Location::ErrorLB("Error in initVarLocation: missing semicolon " + tmp);
			if (tmp == "on")
				_LocationAutoindex = true;
			if (tmp == "off")
				_LocationAutoindex = false;
		}
	}
	else if (tmp == "return"){
		if (std::getline(sLine, tmp, ';')){
			// std::string reDirectory = "";
			if (sLine.eof()) 
       			throw Location::ErrorLB("Error in initVarLocation: missing semicolon " + tmp);
			// // is root valid
			// reDirectory = removeLeadingDot(trim(tmp));
			// if (!reDirectory.empty())
			// 	reDirectory = get_current_dir_name() + reDirectory + "/";
			// else if (opendir((reDirectory).c_str()) == NULL)
			// 	throw ServerBlock::ErrorSB("Error in initVarConfig: not a valid root dir " + reDirectory);
			_LocationReturn = trim(tmp);
		}
	}
	else if (tmp == "}"){
		;
	}
	else {
		throw Location::ErrorLB("Error in initVarLocation: unknown variable in Location");
	}
}

std::string		Location::trim(const std::string& str) {
	size_t first = str.find_first_not_of(" \t");
	if (first == std::string::npos) return ""; // All spaces or empty string

	size_t last = str.find_last_not_of(" \t");
	return str.substr(first, (last - first + 1));
}

void		Location::parseLocURI(std::string line){
	std::string loc = "location";
	unsigned int i = 0;
	while (line[i] == loc[i] && i < line.size())
	{

		i++;
	}
	while ((line[i] == ' ' || line[i] == '\t') && i < line.size())
	{
		i++;
	}
	
	while (line[i] != ' ' && line[i] != '\t' && i < line.size() && line[i] != '{')
	{
		_LocationURI += line[i];
		i++;
	}
}

std::string Location::removeLeadingDot(const std::string& input) {
    if (!input.empty() && input[0] == '.') {
        return input.substr(1); // Return substring starting from the second character
    }
    return ""; // Return empty string
}

std::string Location::getCurrentWorkingDirectory() {
    char buffer[1024];  // Buffer for the path
    ssize_t len;

    // Open the symbolic link for the current working directory
    int fd = open("/proc/self/cwd", O_RDONLY);
    if (fd == -1) {
        std::cerr << "Error: Unable to open /proc/self/cwd " << std::endl;
        return "";
    }

    // Read the symbolic link to determine the path
    len = read(fd, buffer, sizeof(buffer) - 1);  // Leave space for null terminator
    if (len == -1) {
        std::cerr << "Error: Unable to read /proc/self/cwd " << std::endl;
        close(fd);
        return "";
    }

    close(fd);

    // Null-terminate and convert to std::string
    buffer[len] = '\0';
    return std::string(buffer);
}

std::string	Location::getLocationRoot(){
	return(_LocationRoot);
}

std::string		Location::getLocationIndex(){
	return(_LocationIndex);
}

std::string		Location::getLocationURI(){
	return(_LocationURI);
}

std::vector<std::string>	Location::getLocationAllowMethod(){
	return(_LocationAllowMethod);
}

bool 						Location::getLocationAutoindex(){
	return(_LocationAutoindex);
}

std::string					Location::getReturn(){
	return(_LocationReturn);
}