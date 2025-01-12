/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tstahlhu <tstahlhu@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 12:05:55 by tstahlhu          #+#    #+#             */
/*   Updated: 2025/01/06 13:24:07 by tstahlhu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#pragma once

#include <iostream>
#include <sstream>
#include <map>
#include <list>
#include "Response.hpp"
#include "ServerBlock.hpp"

class Response;

/*
	_ErrorCode is default 200
*/

class Request {
	
	private:
		std::string							_RequestMsg;
		int									_RequestLength;
	
	
	protected:
		std::string 						_RequestMethod;
		std::string 						_RequestURI;
		std::string							_QueryString;
		std::string 						_HTTPVersion;
		std::map<std::string , std::string> _Headers;
		std::string 						_Body;
		int									_ErrorCode;
		bool								_CGI;
		std::string							_file;
		ServerBlock							_SB;
		std::string							_whatScript;
		std::string							_cgiPath;
		int									_port;
		std::string 						_errPath;
		std::string							_CGIoutput;
		int									_FD;
		int									_PIDChild;
		int									_Childfd;
		int									_Cookie;
		std::list<int>						_SessionID;// from server
	
	public:
		Request();
		Request(const Request &a);
		Request( ServerBlock SB );
		Request(int ErrCode, ServerBlock SB);
		~Request();

		Request &operator=(const Request &a);

		class ErrorCode : public std::exception
		{
			private:
			std::string _msg;

			public:
			ErrorCode(std::string const &msg);
			~ErrorCode() throw();
			
			const char *what()const throw();
		};
		// int pid; // thats the pid to wait for before building the response
		// Getters
		std::string 	getRequestURI( void ) const;
		std::string		getQueryString( void ) const;
		std::string 	getHTTPVersion( void ) const;
		std::string 	getName( void ) const;
		std::string 	getMethod( void ) const;
		int				getErrorCode( void )const;
		bool			getCGI( void )const;
		std::string		getBody( void ) const;
		std::string 	getFilename( void )const;
		ServerBlock		getSB( void )const;
		std::string		getcgiPath( void ) const;
		int				getPort( void ) const;
		std::map<std::string , std::string>	getHeaders( void ) const;
		std::string		getCGIoutput( void ) const;
		int				getFD( void ) const;
		int				getPIDChild( void ) const;
		int				getChildFD( void ) const;
		int 			getCookie( void ) const;
		std::list<int> getSessionID( void ) const;

		// Setters
		void			setFilename( std::string newName );
		void			setRequestURI(std::string URI);
		// void			setPort( int port );
		void 			setErrPagePath(int errCode);
		void			setErrorCode( int errCode);
		void			setBody( std::string body);
		void			setCGIoutput(std::string &output);
		void			setFD(int ReqFD);
		void			setPID(int PID);
		void 			setCookie(int);
		void			setSession(std::list<int>);

		// Member Functions
		void 		ParseRequest( std::string buffer, int valread );
		void		extractBody( void );
		void 		extractHeaders( std::stringstream & rmsg );
		void 		RequestType( void );
		void		saveQueryString( void );
		void 		checkHTTPVersion( void );
		void		checkHost( void );
		bool		isCGI( void );
		void		isFile( void );
		bool		isDir( void );
		bool		isLocation( void );
		// void		useLocation(Location Loc);
		std::string intToString(int num);

		static std::string unchunkBody(std::string reqMsg);
};

	std::ostream &operator<<(std::ostream &o, const Request &a);

