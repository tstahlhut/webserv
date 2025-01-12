/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestMember.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpodack <rpodack@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 18:16:32 by tstahlhu          #+#    #+#             */
/*   Updated: 2025/01/11 17:25:07 by rpodack          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Request.hpp"

void	Request::extractBody( void ) {

	bool	isChunked = false;

	if (_RequestMsg.find("Transfer-Encoding: chunked") != std::string::npos)
		isChunked = true;
	size_t	bodystart = _RequestMsg.find("\r\n\r\n");
	if (bodystart != std::string::npos && !isChunked) {
		_Body = _RequestMsg.substr(bodystart + 4);
	}
	else if (isChunked) {
		_Body = unchunkBody(_RequestMsg);
	}
	if (_Body.length() > static_cast<unsigned int>(_SB.getMaxBody()))
	 	throw 413;
}

std::string Request::unchunkBody(std::string reqMsg) {

	std::string body;
	size_t bodyStart = reqMsg.find("\r\n\r\n");
	if (bodyStart == std::string::npos) {
		throw std::runtime_error("Error");
	}

	size_t currPos = bodyStart + 4;

	while (currPos < reqMsg.size()) {

		size_t chunkEnd = reqMsg.find("\r\n", currPos);
		if (chunkEnd == std::string::npos) {
			//error
			throw std::runtime_error("Error");
		}

		std::string chunkSizeStr = reqMsg.substr(currPos, chunkEnd - currPos);
		size_t chunkSize = 0;
		std::istringstream(chunkSizeStr) >> std::hex >> chunkSize;

		if (chunkSize == 0) {
			break;
		}

		currPos = chunkEnd + 2;
		if (currPos + chunkSize > reqMsg.size()) {
			throw std::runtime_error("Error");
		}

		body.append(reqMsg.substr(currPos, chunkSize));
		currPos += chunkSize + 2;
	}
	return body;
}

void Request::extractHeaders( std::stringstream & rmsg ) {
	
	std::string line;
	while (std::getline(rmsg, line, '\n')){
		std::string HeaderKey;
		std::string HeaderValue;
		std::stringstream HeaderStream(line);
		std::getline(HeaderStream, HeaderKey, ':');
		std::getline(HeaderStream, HeaderValue, ' ');
		std::getline(HeaderStream, HeaderValue, '\r');
		if (HeaderValue.empty()) {
            break; 
        }
		_Headers[HeaderKey] = HeaderValue;
	}
	std::map<std::string, std::string>::iterator it = _Headers.find("Cookie");
	if (it != _Headers.end()){
		std::string cookieID;
		std::stringstream sid(it->second);
		std::getline(sid, cookieID, '=');
		std::getline(sid, cookieID);
		_Cookie = std::atoi(cookieID.c_str());
	}
	else{
		_Cookie = -1;
	}
}

void	Request::checkHost() {

	std::ostringstream oss;
	std::vector<int>	ports = getSB()._port;

	std::cout << std::endl << "HOST: " << _Headers["Host"] << std::endl;
	
	for (size_t i = 0; i < ports.size(); ++i) {
		oss.str("");
		oss.clear();
		oss << ports[i];
		std::string	portNumber = oss.str();
		std::cout << portNumber << std::endl;
	
		if (_Headers["Host"] == getSB().getServerName() + ":" + portNumber || _Headers["Host"] == "localhost:" + portNumber)
			return;
	}
	throw 404;
}

void	Request::ParseRequest( std::string buffer, int valread ){

	_RequestMsg = buffer;
	_RequestLength = valread;
	
	std::cout << "--- Parse Request --- " << std::endl;
	try {
		std::stringstream rmsg(_RequestMsg);
		std::getline(rmsg, _RequestMethod, ' ');
		std::getline(rmsg, _RequestURI, ' ');
		saveQueryString();
		std::getline(rmsg, _HTTPVersion, '\n');

		if ( this->isLocation()){
		
			if (_SB.getReDir().size() > 0 ) {
				_ErrorCode = 302;
				}
		}
		else if (_SB.getReDir().size() > 0 ) {
			if (_SB.getReDir() != getRequestURI())
				_ErrorCode = 302;
		}
			//	_RequestURI = _SB.getReDir();
		
		if (isCGI()){
			_SB.setWhatScript(_whatScript);
			_SB.useCGILocation();
		}
		
		RequestType();
		checkHTTPVersion();
		isFile();
		extractHeaders(rmsg);
		checkHost();
		extractBody();
	}
	catch (int error_code) {
		_ErrorCode = error_code;
	}
}

void	Request::RequestType( void ) {

	std::vector<std::string> Methods = _SB.getMethodVec();

	for (std::vector<std::string>::iterator it = Methods.begin(); it != Methods.end(); it++){
		if (*it == this->getMethod())
			return ;
	}

	std::vector<std::string> Meth ;
	Meth.push_back("GET");
	Meth.push_back("POST");
	Meth.push_back("DELETE");
	for (std::vector<std::string>::iterator it = Meth.begin(); it != Meth.end(); it++){
		if (*it == this->getMethod()){
			throw 405;
		}
	}
	for (std::vector<std::string>::iterator it = Meth.begin(); it != Meth.end(); it++)
		if (*it == this->getMethod())
			return;
	throw 501;// see RFC9110: https://www.rfc-editor.org/rfc/rfc9110.html#name-501-not-implemented
}

void	Request::saveQueryString( void ) {

	std::string::size_type pos;
	pos = _RequestURI.find("?");
	if (pos != std::string::npos) {
		this->_QueryString = _RequestURI.substr(pos + 1);
		this->_RequestURI = _RequestURI.substr(0, pos);
	}
}

void 		Request::checkHTTPVersion(){
	
	if (_HTTPVersion == "HTTP/1.0\r" || _HTTPVersion == "HTTP/1.1\r"){
		return ;
	}
	_ErrorCode = 505;
	throw ErrorCode("HTTP Version Not Supported: " + _HTTPVersion);
}

bool	Request::isCGI( void ){

	std::string tmp;
	std::stringstream ruri(_RequestURI);
	if (std::getline(ruri, tmp, '.')){
		if (std::getline(ruri, tmp)){
			int len = 2;
			std::string scriptType[2] = {"php", "py"};// extend this list and add a if statement for the path for the bonus
			for (int i = 0; i < len ; i++ )
				if (scriptType[i] == tmp){
					_whatScript = "/." + tmp;
					if (tmp == "php")
						_cgiPath = "/usr/bin/php-cgi";
					if (tmp == "py")
						_cgiPath = "/usr/bin/python3";
					_CGI = true;
					return(true);
				}
		}
	}
	return(false);
}

void		Request::isFile( void ) {
	
	if (_RequestURI == "/") 
		_file = _SB.getRoot() + "/" + _SB.getIndex();
	else if (isCGI()){
		_file =  _SB.getCGIRoot() + _RequestURI;
	}
	else if (isDir()){
		_file = _SB.getRoot() + _RequestURI;
		return;
	}
	else
		_file = _SB.getRoot() + _RequestURI;
	std::ifstream fileBin(_file.c_str(), std::ios::binary | std::ios::ate);
	if (!fileBin.is_open()){
		_ErrorCode = 404;
		fileBin.close();
		std::cerr << "Error: file not found: " <<  _file << std::endl;
		throw 404;
	}
	fileBin.close();
}

bool		Request::isDir( void ){

	if (!_RequestURI.empty() && _RequestURI[_RequestURI.size() - 1] == '/' && _RequestURI.size() != 1) 
		return(true);
	return(false);
}

bool 		Request::isLocation( void ) {

	std::vector<Location> Locs = _SB.getLocVec();
	for (std::vector<Location>::iterator it = Locs.begin(); it != Locs.end(); it++)
	{
			if (it->getLocationURI() == _RequestURI){
				_SB.useLocation(*it);
				return(true);
		}
	}
	return(false);
}

std::string Request::intToString( int num ) {

    std::ostringstream oss;
    oss << num;
    return oss.str();
}