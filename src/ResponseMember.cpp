/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseMember.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpodack <rpodack@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 18:16:38 by tstahlhu          #+#    #+#             */
/*   Updated: 2025/01/11 17:26:00 by rpodack          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Response.hpp"

void	Response::setStatusLine( Request & ReqObj ) {

	switch (ReqObj.getErrorCode()) {
		case 200:
			_statusLine = "HTTP/1.1 200 OK\r\n";
			return;
		case 302:
			_statusLine = "HTTP/1.1 302 Found\r\n";
			_headerMap["Location: "] = ReqObj.getSB().getReDir();
			return;
		case 405:
			_statusLine = "HTTP/1.1 405 Method Not Allowed\r\n";
			ReqObj.setErrPagePath(405);
			return;
		case 403:
			_statusLine = "HTTP/1.1 403 Forbidden\r\n";
			ReqObj.setErrPagePath(403);
			return;
		case 404:
			_statusLine = "HTTP/1.1 404 Page Not Found\r\n";
			ReqObj.setErrPagePath(404);
			return;
		case 413:
			_statusLine = "HTTP/1.1 413 Payload Too Large\r\n";
			ReqObj.setErrPagePath(413);
			return;
		case 501:
			_statusLine = "HTTP/1.1 501 Not Implemented\r\n"; // see RFC9110: https://www.rfc-editor.org/rfc/rfc9110.html#name-501-not-implemented
			ReqObj.setErrPagePath(501);
			return;
		case 505:
			_statusLine = "HTTP/1.1 505 Version Not Supported\r\n";
			ReqObj.setErrPagePath(505);
			return;
		default:
			_statusLine = "HTTP/1.1 500 Internal Server Error\r\n";
			ReqObj.setErrPagePath(500);
	}
}

void	Response::DELETE( Request & ReqObj ) {

	std::cout << "DELETE: " << ReqObj.getFilename().c_str() << std::endl;
	std::cout << ReqObj.getSB().getRoot() << std::endl;
	
	std::string tmp = ReqObj.getSB().getRoot() + "/cgi-bin/uploaded_files/";

	if (ReqObj.isDir())
		throw 403;
	else if (ReqObj.getFilename().find(tmp) != std::string::npos && (tmp.find_last_of("/") != ReqObj.getFilename().find_last_of("/")))
		throw 403;
	else if (std::remove(ReqObj.getFilename().c_str()) == 0)
		throw 200;
	else
		throw 404;
}

void	Response::setContentType( Request &ReqObj ) {

	//prototype header
	if (ReqObj.getRequestURI() == "/" || ReqObj.getRequestURI().empty())
		_headerMap["Content-Type: "] = "text/html\r\n";
	else if (ReqObj.getRequestURI().find(".html") != std::string::npos)
		_headerMap["Content-Type: "] = "text/html\r\n";
	else if (ReqObj.getRequestURI().find(".php") != std::string::npos) {// same for other kind of script for bonus
		_headerMap["Content-Type: "] = "text/html\r\n";
	}
	else if (ReqObj.getRequestURI().find(".css") != std::string::npos)
		_headerMap["Content-Type: "] = "text/css\r\n";
	else if (ReqObj.getRequestURI().find(".svg") != std::string::npos)
		_headerMap["Content-Type: "] = "image/svg+xml\r\n";
	else if (ReqObj.getRequestURI().find(".jpeg") != std::string::npos)
		_headerMap["Content-Type: "] = "image/jpeg\r\n";
	else if (ReqObj.getRequestURI().find(".jpg") != std::string::npos)
		_headerMap["Content-Type: "] = "image/jpeg\r\n";
	else if (ReqObj.getRequestURI().find(".gif") != std::string::npos)
		_headerMap["Content-Type: "] = "image/gif\r\n";
	else if (ReqObj.getRequestURI().find(".ico") != std::string::npos)
		_headerMap["Content-Type: "] = "image/x-icon\r\n";
	// _headerMap["Content-Length: "] =  longToString(_fileSize) + "\r\n";// if we remove this !!!!!!!!!!!!? can we move the functions around better
}

std::string		Response::parseForSpe( std::string value ) {
	
	std::string result = "";

	for (std::size_t i = 0; i < value.size(); ++i) {
    	char ch = value[i];
    	if (static_cast<unsigned char>(ch) >= 127) { // \377 und \177 indicate EOF
            std::cout << "\\377 found in string!\n";
        } 
		else{
			result += ch;
		}
	}
	return(result);
}

std::string Response::buildhttpResponse( void ) {
	
	std::string httpResponse = "";

	httpResponse += _statusLine;
	for (std::map<std::string, std::string>::iterator it = _headerMap.begin(); it != _headerMap.end(); it++)
	{
		httpResponse += it->first;
		httpResponse += it->second;
	}
	httpResponse += "\r\n";
	std::cout << std::endl << httpResponse << std::endl;
	httpResponse += _body;
	return httpResponse;
}