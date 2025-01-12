/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tstahlhu <tstahlhu@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 16:04:05 by tstahlhu          #+#    #+#             */
/*   Updated: 2025/01/06 13:08:02 by tstahlhu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../includes/Request.hpp"

const char *Request::ErrorCode::what()const throw(){
	
	return(_msg.c_str());
}

Request::Request(){
	
};

Request::Request( const Request &a ) {
	
	*this = a;
};

Request::Request( ServerBlock SB) : _ErrorCode(200), _CGI(false), _SB(SB), _whatScript("/." ), _CGIoutput(""), _FD(0), _PIDChild(0), _Childfd(0) {
	
};

Request::Request(int ErrCode, ServerBlock SB) :  _ErrorCode(ErrCode), _CGI(false), _SB(SB), _whatScript("/." ), _CGIoutput(""), _FD(0), _PIDChild(0), _Childfd(0) {};

Request::~Request(){

};

Request &Request::operator=(const Request &a){
	_RequestMethod =	a._RequestMethod;
	_RequestURI = 		a._RequestURI;
	_QueryString =		a._QueryString;
	_HTTPVersion = 		a._HTTPVersion;
	_RequestLength = 	a._RequestLength;
	_RequestMsg = 		a._RequestMsg;
	_ErrorCode =		a._ErrorCode;
	_Headers = 			a._Headers;
	_Body = 			a._Body;
	_file = 			a._file;
	_SB = 				a._SB;
	_CGI = 				a._CGI;
	_cgiPath =			a._cgiPath;
	_errPath =			a._errPath;
	_port = 			a._port;
	_CGIoutput = 		a._CGIoutput;
	_FD = 				a._FD;
	// pid = 				a.pid;
	_PIDChild = 		a._PIDChild;
	_Childfd = 			a._Childfd;
	_Cookie =			a._Cookie;
	_SessionID	=		a._SessionID;
	return (*this);
};

std::ostream &operator<<(std::ostream &o, const Request &a){
	o << a.getName();
	return(o);
}


Request::ErrorCode::ErrorCode(std::string const &msg) : _msg(msg) {

};

Request::ErrorCode::~ErrorCode()throw(){
	
};


/* ************************************************************************** */
/*                                                                            */
/*						Getters												  */
/*  																		  */
/* ************************************************************************** */


std::string Request::getName( void ) const {

	return _RequestMsg;
}

std::string Request::getMethod( void ) const {

	return (_RequestMethod);
}

std::string Request::getRequestURI( void ) const {

	return (this->_RequestURI);
}

std::string		Request::getQueryString( void ) const {

	return (this->_QueryString);
}

std::string Request::getHTTPVersion( void ) const {

	return (this->_HTTPVersion);
}

int			Request::getErrorCode( void ) const {

	return(_ErrorCode);
}

bool		Request::getCGI( void ) const {

	return(_CGI);
}

std::string		Request::getcgiPath( void ) const {
	
	return(_cgiPath);
}

std::string	Request::getBody( void ) const {

	return (_Body);
}

std::string	Request::getFilename( void ) const {

	return(_file);
}


ServerBlock	Request::getSB( void ) const {
	
	return(_SB);
}

int		Request::getPort( void ) const {

	return (_port);
}

std::map<std::string, std::string> Request::getHeaders( void ) const {
	
	return (_Headers);
}

std::string		Request::getCGIoutput()const{
	return(_CGIoutput);
}

int	Request::getFD()const{
	
	return(_FD);
}

int	Request::getPIDChild() const{
	
	return(_PIDChild);
}

int	Request::getChildFD() const{
	
	return(_Childfd);
}


int 			Request::getCookie() const{
	return(_Cookie);
}
std::list<int> Request::getSessionID()const{
	return(_SessionID);
}

/* ************************************************************************** */
/*                                                                            */
/*						Setters												  */
/*  																		  */
/* ************************************************************************** */

void		Request::setRequestURI( std::string URI ) {
	
	_RequestURI = URI;
}

void	Request::setFilename( std::string newName ) {
	
	_file = newName;
}

 void 		Request::setErrPagePath( int errCode ) {
	
	std::map<int, std::string> errPages =  getSB().getErrPagesMap();
	std::map<int, std::string>::iterator it = errPages.find(errCode);
	if (it != errPages.end())
		_errPath = getSB().getRoot() + errPages[errCode];
	else
		_errPath = getSB().getRoot() + "/error_pages/" + intToString(errCode) + ".html";
	_file = _errPath;
	std::cout << "setErrPagePath: " << _file << std::endl;
}

void	Request::setErrorCode( int errCode ) {

	this->_ErrorCode = errCode;
}

void	Request::setBody( std::string body ) {
	
	this->_Body = body;
}

void	Request::setCGIoutput(std::string &output){
	
	_CGIoutput = output;
}

void	Request::setFD(int ReqFD){
	
	_FD = ReqFD;
}

void	Request::setPID(int PID){
	
	_PIDChild = PID;
}

void	Request::setCookie(int a){

	_Cookie = a;
}
void	Request::setSession(std::list<int> a){
	
	_SessionID = a;
}
