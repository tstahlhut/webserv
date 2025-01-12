/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpodack <rpodack@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 16:18:49 by rpodack           #+#    #+#             */
/*   Updated: 2025/01/11 17:21:50 by rpodack          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Response.hpp"
#include "../includes/CGI.hpp"


Response::ErrorResponse::ErrorResponse(std::string const &msg) : _msg(msg){}
Response::ErrorResponse::~ErrorResponse()throw(){}			
const char *Response::ErrorResponse::what()const throw(){
	return(_msg.c_str());
}

Response::Response(){}

Response::Response( const Response& ) {}

Response::~Response() {}

Response& Response::operator=( const Response& src )
{
	_httpResponse = src._httpResponse;
	_ResLen = src._ResLen;
	_statusLine = src._statusLine;
	_headerMap = src._headerMap;
	_body = src._body;
	_fileSize = src._fileSize;
	_filename = src._filename;
	
	return *this;
}



Response::Response( Request& ReqObj )
{

	try
	{
		processResponse(ReqObj);
	}
	catch (int error_code)
	{
		_body = _statusLine.substr(9);
		_httpResponse = buildhttpResponse();
	}
	
}

void	Response::handleCookies(Request & ReqObj) {
	
	int Cookie = ReqObj.getCookie();
		if (Cookie  >= 0){
			std::cout << "I've seen you here before\n";
		}
		else{
			int id = ReqObj.getSessionID().size() + 1;
			ReqObj.setCookie(id);
			std::ostringstream oss;
    		oss << id;
			_headerMap["Set-Cookie: "] = "sessionId=" + oss.str() + "Path=/;";
		}
}

void		Response::processResponse(Request &ReqObj){
	
	std::cout << std::endl << "---- IN RESPONSE ---" <<std::endl << std::endl;

	try {
		_filename = ReqObj.getFilename();
		std::cout << "process Response: " << _filename << std::endl;
		std::cout << "Error Code: " << ReqObj.getErrorCode() << std::endl;
		setContentType(ReqObj);

		handleCookies(ReqObj);
		if (ReqObj.getErrorCode() != 200){
			throw ReqObj.getErrorCode();
		}
		
		if (ReqObj.isDir() && ReqObj.getMethod() != "DELETE"){
			if (dirListing(ReqObj))
				return;
		}
		if (ReqObj.getMethod() == "DELETE"){
			DELETE(ReqObj);
		}
		if (ReqObj.getCGI() && ReqObj.getErrorCode() == 200){

			std::cout << "IS CGI" << std::endl;
			setBody(ReqObj.getCGIoutput());
			_fileSize = _body.size();
			_headerMap["Content-Length: "] =  longToString(_fileSize) + "\r\n";
			setStatusLine(ReqObj);
			_httpResponse = buildhttpResponse();
			return;
		}
		if (ReqObj.getMethod() != "DELETE")
			getFileSize();
	}
	catch (int error_code) {
		ReqObj.setErrorCode(error_code);
	}	
	setStatusLine(ReqObj);
	if (!ReqObj.isDir())
		_filename = ReqObj.getFilename();
	_body = getBodyFromFile();
	getFileSize();
	_httpResponse = buildhttpResponse();
}

int	Response::dirListing( Request &ReqObj ) {
	
	std::string filename = _filename + ReqObj.getSB().getIndex();
	std::ifstream fileBin(filename.c_str(), std::ios::binary | std::ios::ate);
	if (!fileBin.is_open()){
		if (ReqObj.getSB().getAutoindex() == true){
			DIR *dir = opendir(_filename.c_str());
			if (dir){
				std::ifstream file;
				file.open("./www/dirListing-one.html");
				if (!file.is_open())
				{
						throw 500;
				}
				std::stringstream buffer;
				buffer << file.rdbuf();
				file.close();
				_body = buffer.str();
				buffer.str("");
				buffer.clear();
				struct dirent *entry;
				while ((entry = readdir(dir)) != NULL)
				{
					// put that listing into a body
					_body += "<li>";
					_body += entry->d_name;
					_body += "</li>\n";
				}
				file.open("./www/dirListing-two.html");
				if (!file.is_open())
				{
						throw 500;
				}
				buffer << file.rdbuf();
				file.close();
				_body += buffer.str();
				closedir(dir);
			}
			_fileSize = _body.size();
			_headerMap["Content-Length: "] = longToString(_fileSize) + "\r\n";
			setStatusLine(ReqObj);
			_httpResponse = buildhttpResponse();
			return 1;
		}
		else{
			_filename = "./www/dir_default.html";
			_body = getBodyFromFile();
			return 0;
		}
	}
	else {
		_filename = _filename + ReqObj.getSB().getIndex();
		fileBin.close();
		if (ReqObj.getSB().getAutoindex() == true){
			_body = getBodyFromFile();
		}
		else
			_filename = "./www/dir_default.html";
	}
	return 0;
}

/* ************************************************************************** */
/*                                                                            */
/*						Getters												  */
/*  																		  */
/* ************************************************************************** */

void		Response::getFileSize( void ) {
	
	std::ifstream file(_filename.c_str(), std::ios::binary | std::ios::ate);
	if (!file.is_open())
	{
			std::cout << _filename << std::endl; 
			throw 404;
	}
	_fileSize = file.tellg();
	file.close();
	_headerMap["Content-Length: "] = longToString(_fileSize) + "\r\n";
}

std::string Response::getBodyFromFile( void ) {
	
	std::string body, tmp;
	std::ifstream file;
	bool isBinary = (_filename.find(".jpg") != std::string::npos ||
     							_filename.find(".jpeg") != std::string::npos ||
      						_filename.find(".ico") != std::string::npos ||
							_filename.find(".png") != std::string::npos );

 	if (isBinary)
		file.open(_filename.c_str(), std::ios::binary);
	else
   		file.open(_filename.c_str());

	if (!file.is_open())
		return("");
	std::stringstream buffer;
	buffer << file.rdbuf();
	file.close();
	tmp = buffer.str();
	if (!isBinary)
	{
		body.reserve(tmp.size());
		for (size_t i = 0; i < tmp.size(); i++)
		{
			body += tmp[i]; 
		}
	}
	else
		body = tmp;
	return body;
}

std::string longToString( long value ) {
	
	std::stringstream ss;
	ss << value;
	return ss.str();
}

std::string Response::getResponse( void ) const {
	
	return(_httpResponse);
}

int Response::getResLen( void ) const {

	return (_ResLen);
}

std::string Response::getBody( void ) const {
	
	return(_body);
}

/* ************************************************************************** */
/*                                                                            */
/*						Setters												  */
/*  																		  */
/* ************************************************************************** */

void        Response::setBody( std::string str ) {
	
	_body = str;
}
