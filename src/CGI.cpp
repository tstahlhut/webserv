/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpodack <rpodack@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 12:10:45 by tstahlhu          #+#    #+#             */
/*   Updated: 2025/01/11 17:24:21 by rpodack          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/CGI.hpp"
#include "../includes/Request.hpp"
#include "../includes/Response.hpp"


CGI::CGI( void ){}

CGI::~CGI( void ){
	   if (!_env) return; // Guard against nullptr

    // Determine the size of the _envay (assuming nullptr-termination convention)
    size_t size = 0;
    while (_env[size] != NULL) {
        delete[] _env[size]; // Free each string
        ++size;
    }

    // Delete the _envay of pointers
    delete[] _env;
}

CGI &CGI::operator=( const CGI &a ){

	_env = a._env;
	_env_map = a._env_map;

	return(*this);
}

CGI::CGI( Request & Req ) {
	
	initEnv( Req );
	createTmpFolder(); // php needs a temporary folder to be able to process uploaded files
}

int             CGI::getPipe_OutFD()const{
	return(_pipe_out[0]);
}

void	CGI::createTmpFolder( void ) {
	
	struct stat	tmp;

	//check if tmp folder exists and is directory
	if (stat(TEMP_FOLDER, &tmp) != 0 || !(tmp.st_mode & S_IFDIR)) {
		if (mkdir(TEMP_FOLDER, 0744) != 0) {
			std::cerr << "CGI: Failed to create temporary directory for PHP CGI: " << strerror(errno) <<  " : " << TEMP_FOLDER << std::endl;
			return ;
		}
	}
	// check file type and permissions
	if (stat(TEMP_FOLDER, &tmp) == 0 && tmp.st_mode != 040744) {
		chmod(TEMP_FOLDER, 0744);
	}
}

void CGI::checkContentLength( Request& Req) {

	if (this->_env_map["CONTENT_LENGTH"] == "") {
		
		std::string body = Req.getBody();
		long size = body.size();
		std::ostringstream oss;
		oss << size;
		this->_env_map["CONTENT_LENGTH"] = oss.str();
	}
}

void	CGI::initEnv( Request& Req ) {

	std::map<std::string , std::string>	http_header = Req.getHeaders();

	// server & config
	this->_env_map["GATEWAY_INTERFACE"] = "CGI/1.1"; // version of CGI specification
	this->_env_map["SERVER_NAME"] = Req.getSB().getServerName(); // "webserver" _server_name from serverBlock or Server or use IP address (host in serverBlock)
	this->_env_map["SERVER_SOFTWARE"] = "WEBSERV";
	this->_env_map["SERVER_PROTOCOL"] = "HTTP/1.1"; // version of the protocol the client is using
	this->_env_map["REDIRECT_STATUS"] = "200";
	size_t pos = http_header["Host"].find(":");
	if (pos != std::string::npos) {
		this->_env_map["SERVER_PORT"] = http_header["Host"].substr(pos + 1); //port on which the server receives the request
	}
	this->_env_map["DOCUMENT_ROOT"] = Req.getSB().getCGIRoot(); //"./www/cgi-bin"; -> aus ServerBlock
	this->_env_map["ORIGINAL_ROOT"] = Req.getSB().getRoot();
	this->_env_map["TMPDIR"] = "tmp_php/";
	this->_env_map["TEMP"] = "tmp_php/";

	// from client
	this->_env_map["REQUEST_URI"] = Req.getRequestURI(); // URI used to access the script, including the path and query string
	this->_env_map["SCRIPT_NAME"] = Req.getRequestURI(); // the virtual path to the script, e.g. /cgi-bin/form.php
	this->_env_map["SCRIPT_FILENAME"] = this->_env_map["DOCUMENT_ROOT"] + Req.getRequestURI(); // the absolute path to the script, e.g. /www/cgi-bin/form.php
	this->_env_map["QUERY_STRING"] = Req.getQueryString(); // part of URL after "?", used for GET
	this->_env_map["USER_AGENT"] = http_header["User-Agent"];
	this->_env_map["HTTP_ACCEPT"] = http_header["Accept"];
	this->_env_map["HTTP_ACCEPT_CHARSET"] = http_header["Accept-Charset"];
	this->_env_map["HTTP_ACCEPT_ENCODING"] = http_header["Accept-Encoding"];
	this->_env_map["HTTP_ACCEPT_LANGUAGE"] = http_header["Accept-Language"];
	this->_env_map["CONTENT_LENGTH"] = http_header["Content-Length"];
	checkContentLength(Req);
	this->_env_map["CONTENT_TYPE"] = http_header["Content-Type"];
	this->_env_map["REQUEST_METHOD"] = Req.getMethod();
	this->_env_map["REFERER"] = http_header["Referer"];

	this->_env_map["HTTP_COOKIE"] = ""; // for bonus part: cookies

}

void		CGI::createEnvChar( void ) {
	
	size_t	mapLen = _env_map.size();
	_env = new char*[mapLen + 1];
	int i = 0;
	for (std::map<std::string, std::string>::iterator it = _env_map.begin(); it != _env_map.end(); it++)
	{
		
		std::string tmp = it->first + "=" + it->second;
		_env[i] = new char[tmp.size() + 1];
		std::copy(tmp.begin(), tmp.end(), _env[i]);
		_env[i][tmp.size()] = '\0';
		i++;
		tmp = "";
	}
	_env[i] = NULL;
	return;
}


void        CGI::runCGI(Request &ReqObj) {
	
	int pipe_in[2];
	pipe(_pipe_out);
	pipe(pipe_in);
	int pid = fork();
	_PID = pid;
	ReqObj.setPID(pid);
	_Time = std::time(NULL);
	if (pid == 0){
		close(_pipe_out[0]); // Close read end of pipe_out
		close(pipe_in[1]); // Close write end of pipe_in
		if (dup2(_pipe_out[1], STDOUT_FILENO) == -1 ||
			dup2(pipe_in[0], STDIN_FILENO) == -1) {
				exit(1);
		}
		close(_pipe_out[1]);
		close(pipe_in[0]);

		std::string path = ReqObj.getcgiPath();
		std::string name = ReqObj.getFilename();
		const char *str[] = {path.c_str(), name.c_str(), NULL};
		execve(str[0], const_cast<char* const*>(str), this->_env);
		exit(1);
	}
	else{
		close(_pipe_out[1]);// Close write end of pipe_out
		close(pipe_in[0]); // Close read end of pipe_in
		write(pipe_in[1], ReqObj.getBody().c_str(), ReqObj.getBody().length());
		close(pipe_in[1]);
	}
}

std::time_t     CGI::getTime()const{
	return(_Time);
}

int             CGI::getPID()const{
	return(_PID);
}