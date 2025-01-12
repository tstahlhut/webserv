/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerBlock.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tstahlhu <tstahlhu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/11 14:14:09 by tstahlhu          #+#    #+#             */
/*   Updated: 2025/01/04 17:46:08 by tstahlhu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ServerBlock.hpp"

ServerBlock::ServerBlock( void ) :  _root("./www"), _index("index.html"), _serverName("_"), _host(0), _client_max_body_size(8388608), _autoindex(false), _cgiRoot("./www"), _reDir(""){
	
	_AllowedMethods.push_back("GET");
	_AllowedMethods.push_back("POST");
	_AllowedMethods.push_back("DELETE");
	
}

ServerBlock::~ServerBlock( void ) {}

ServerBlock::ServerBlock( const ServerBlock& a ) {
	
	*this = a;
}

ServerBlock	&ServerBlock::operator=( const ServerBlock &a ) {
	
	_root =						a._root;
	_serverName = 				a._serverName;
	_index = 					a._index;
	_host = 					a._host;	
	_client_max_body_size = 	a._client_max_body_size;
	_errorPages = 				a._errorPages;
	_LocVec = 					a._LocVec;
	_port = 					a._port;
	_AllowedMethods = 			a._AllowedMethods;
	_whatScript = 				a._whatScript;
	_autoindex =				a._autoindex;
	_cgiRoot = 					a._cgiRoot;
	_reDir = 					a._reDir;
	return(*this);
	
}

ServerBlock::ErrorSB::ErrorSB( std::string const &msg ) : _msg(msg){}


ServerBlock::ErrorSB::~ErrorSB()throw(){}


const char *ServerBlock::ErrorSB::what()const throw(){
	
	return(_msg.c_str());
}


bool		ServerBlock::isLocationBlock( const std::string& line ) {
	
	// Check if the line starts with "location", followed by optional whitespace and "{"
	size_t i = 0;

	// Skip leading whitespace
	while (i < line.size() && std::isspace(line[i])) {
		i++;
	}

	// Check if the next part is "location"
	std::string serverStr = "location";
	for (size_t j = 0; j < serverStr.size(); ++j) {
		if (i >= line.size() || line[i] != serverStr[j]) {
			return(false);
		}
		i++;
	}

	// Skip whitespace between "server" and "{"
	while (i < line.size() && std::isspace(line[i])) {
		i++;
	}

	if (i < line.size() && line[i] == '/')
		while (line[i] && !std::isspace(line[i]) && line[i] != '{')
			i++;
	else{
		throw ServerBlock::ErrorSB("Not a valid URI in Location block\n");
		return(false);
	}

	// Skip whitespace between "server" and "{"
	while (i < line.size() && std::isspace(line[i])) {
		i++;
	}

	// Check if the next character is "{"
	return (i < line.size() && line[i] == '{');
}

void	ServerBlock::initVarServerBlock( std::string line ){
	
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
	checkVar(tmp, sLine);
}


void			ServerBlock::checkVar( std::string &tmp, std::stringstream &sLine ) {
	
	if (tmp == "root"){
		std::string fullPath = "";
		if (std::getline(sLine, tmp, ';')){
			 if (sLine.eof()) 
				throw ServerBlock::ErrorSB("Error in initVarConfig: missing semicolon " + tmp);
			// is root valid
			fullPath = removeLeadingDot(trim(tmp));
			if (!fullPath.empty()){
				char *cdirname = get_current_dir_name();
				if (cdirname == NULL)
					throw ServerBlock::ErrorSB("Error in initVarConfig: get_current_dir_name failed ");
				fullPath = cdirname + fullPath + "/";
				free(cdirname);
			}
			DIR *dir = opendir((fullPath).c_str());
			if (dir == NULL){
				closedir(dir);
				throw ServerBlock::ErrorSB("Error in initVarConfig: not a valid root dir " + fullPath);
			}
			closedir(dir);
			_root = trim(tmp);
		}
	}
	else if (tmp == "server_name"){
		if (std::getline(sLine, tmp, ';')){
			if (sLine.eof()) 
				throw ServerBlock::ErrorSB("Error in initVarConfig: missing semicolon " + tmp);
			_serverName = trim(tmp);
		}
	}
	else if (tmp == "listen"){
		std::getline(sLine, tmp, ';');
		if (sLine.eof()) 
				throw ServerBlock::ErrorSB("Error in initVarConfig: missing semicolon " + tmp);
		const char *listen = tmp.c_str();
		while (*listen){
			if (std::isdigit(listen[0]) != 0){
				_port.push_back(std::atoi(listen));
				break;
			}
			listen++;
		}
	}
	else if (tmp == "host"){
		if (std::getline(sLine, tmp, ';')){
			if (sLine.eof()) 
				throw ServerBlock::ErrorSB("Error in initVarConfig: missing semicolon " + tmp);
			_host = ipConvert(trim(tmp));
		}
	}
	else if (tmp == "index"){
		if (std::getline(sLine, tmp, ';')){
			if (sLine.eof()) 
				throw ServerBlock::ErrorSB("Error in initVarConfig: missing semicolon " + tmp);
			_index = trim(tmp);
		}
	}
	else if (tmp == "client_max_body_size"){
		if (std::getline(sLine, tmp, ';')){
			if (sLine.eof()) 
				throw ServerBlock::ErrorSB("Error in initVarConfig: missing semicolon " + tmp);
			int checkInt = std::atoi(tmp.c_str());
			_client_max_body_size = checkInt;
		}
	}
	else if (tmp == "error_page"){
		if (std::getline(sLine, tmp, ';')){

			int errorCode = std::atoi(tmp.c_str());
			std::string errDir = "";
			for (std::string::size_type i = 0; i < tmp.size(); ++i) {
				char ch = tmp[i];
				if (ch == '\t' || ch == ' ')
					continue;
				else {
					if (std::isdigit(ch) != 0)
						continue;
					for (size_t j = i; j < tmp.size(); j++)
					{
						ch = tmp[j];
						if (ch == '\t' || ch == ' ')
							continue;
						errDir += ch;
					}
					errDir += '\0';
					break;
				}
			}
			_errorPages[errorCode] = errDir;
	}
	}
	else if (tmp == "allow_methods"){
		if (std::getline(sLine, tmp, ';')){
			if (sLine.eof()) 
				throw ServerBlock::ErrorSB("Error in initVarConfig: missing semicolon " + tmp);
			// get methods
			std::istringstream mLine(tmp);
			std::string Method;
			std::vector<std::string>		MethodsList;
			while (mLine >> Method){
				MethodsList.push_back(Method);
			}
			_AllowedMethods = MethodsList;
		}
	}
	else if (tmp == "autoindex"){
		if (std::getline(sLine, tmp, ';')){
			if (sLine.eof()) 
				throw ServerBlock::ErrorSB("Error in initVarConfig: missing semicolon " + tmp);
			if (tmp == "on")
				_autoindex = true;
			if (tmp == "off")
				_autoindex = false;
		}
	}
	else if (tmp == "return"){
		if (std::getline(sLine, tmp, ';')){
			if (sLine.eof()) 
       			throw ServerBlock::ErrorSB("Error in initVarServerBlock: missing semicolon " + tmp);
			_reDir = trim(tmp);
		}
	}
}

unsigned int ServerBlock::ipConvert(const std::string host)
{
	std::vector<unsigned int>   parts;
	std::istringstream          ipStream(host);
	std::string                 part;
	unsigned int                ip;
	int                         count = 0;
	while (std::getline(ipStream, part, '.'))
	{
		count++;
		unsigned int num = static_cast<unsigned int>(atoi(part.c_str()));
		parts.push_back(num);
	}
	if (count != 4)
		throw(std::runtime_error("ServerConfig: Error, IP address has invalid format"));
	ip = (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8) | parts[3];
	return (ip);
}

/* ************************************************************************** */
/*                                                                            */
/*						Getters												  */
/*  																		  */
/* ************************************************************************** */


std::string	ServerBlock::getRoot( void ) {
	
	return(_root);
}

std::string	ServerBlock::getServerName( void ) {

	return(_serverName);
}

std::string		ServerBlock::getIndex( void ) {
	
	return(_index);
}

std::vector<std::string>	ServerBlock::getMethodVec( void ) {
	
	return(_AllowedMethods);
}

bool	ServerBlock::getAutoindex( void ) {
	
	return(_autoindex);
}

std::vector<Location>		ServerBlock::getLocVec( void ) {
	
	return(_LocVec);
}

std::map<int, std::string>	ServerBlock::getErrPagesMap( void ) {
	
	return(_errorPages);
}

unsigned int	ServerBlock::getHost( void ) {
	
	return(_host); 
}


unsigned int		ServerBlock::getMaxBody( void ) {
	
	return(_client_max_body_size);
}

std::string		ServerBlock::getCGIRoot( void ) {
	
	return(_cgiRoot);
}

std::string					ServerBlock::getReDir(void){
	return(_reDir);
}


/* ************************************************************************** */
/*                                                                            */
/*						Member Functions									  */
/*  																		  */
/* ************************************************************************** */


void			ServerBlock::setWhatScript( std::string &whatScript ){
	
	_whatScript = whatScript;
}

void		ServerBlock::useCGILocation( void ) {
	
	std::vector<Location>::iterator it = _LocVec.begin();
	for (; it != _LocVec.end(); it++)
	{
		if ((it)->getLocationURI() == _whatScript){
			if ((it)->getLocationRoot().length() > 0)
				_cgiRoot = (it)->getLocationRoot();
			if ((it)->getLocationIndex().length() > 0)
				_index = (it)->getLocationIndex();
			if (it->getLocationAllowMethod().size() > 0)
				_AllowedMethods = it->getLocationAllowMethod();
			if (it->getLocationAutoindex())
				_autoindex = true;
			// add here
		}
	}
}

void		ServerBlock::useLocation( Location Loc ) {
	
	if (Loc.getLocationRoot().length() > 0)
		_root = Loc.getLocationRoot();
	if (Loc.getLocationIndex().length() > 0)
		_index = Loc.getLocationIndex();
	if (Loc.getLocationAllowMethod().size() > 0)
		_AllowedMethods = Loc.getLocationAllowMethod();
	if (Loc.getLocationAutoindex())
		_autoindex = true;
	else{
		_autoindex = false;
	}
	if (Loc.getReturn().size() > 0)
		_reDir = Loc.getReturn();
	// add here
}

std::string		ServerBlock::trim(const std::string& str) {
	size_t first = str.find_first_not_of(" \t");
	if (first == std::string::npos) return ""; // All spaces or empty string

	size_t last = str.find_last_not_of(" \t");
	return str.substr(first, (last - first + 1));
}

std::string ServerBlock::removeLeadingDot(const std::string& input) {
    if (!input.empty() && input[0] == '.') {
        return input.substr(1); // Return substring starting from the second character
    }
    return ""; // Return empty string
}

std::string ServerBlock::getCurrentWorkingDirectory() {
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