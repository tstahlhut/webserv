/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tstahlhu <tstahlhu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 18:15:50 by tstahlhu          #+#    #+#             */
/*   Updated: 2025/01/04 17:47:18 by tstahlhu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <map>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring> // For std::strerror
#include <cstddef>
#include <unistd.h> // execve
#include <sys/wait.h> // waitpid
#include <dirent.h> // opendir
#include <cstdio>



#include "CGI.hpp"
#include"Request.hpp"
#include "ServerBlock.hpp"

class Request;
class CGI;

class Response
{
		private:
				
			std::string 								_httpResponse;
			int 										_ResLen;
			std::string									_statusLine;
			std::map <std::string, std::string>			_headerMap;
			std::string									_body;
			long                                		_fileSize;
			std::string									_filename;


		public:


			class ErrorResponse : public std::exception
			{
				private:
					std::string _msg;

				public:
					ErrorResponse(std::string const &msg);
					~ErrorResponse()throw();				
					const char *what()const throw();
			};

			Response( void );
			Response( const Response& );
			Response( Request &ReqObj );
			Response& operator=( const Response& src );
			~Response( );
		
			std::string	getResponse( void ) const;
			int			getResLen( void )const;
			std::string	getBody( void )const;
			void		getFileSize( void );
			std::string	getBodyFromFile( void );

			void		setBody( std::string str );
			void		setStatusLine( Request & ReqObj );

			void		processResponse(Request &ReqObj);

			std::string	parseForSpe( std::string value );
			char **		createEnv( void );
			std::string	buildhttpResponse( void );
			void		GET( Request ReqObj );
			void		POST( Request ReqObj );
			void		DELETE( Request & ReqObj );
			void		setContentType( Request &ReqObj );
			int			dirListing( Request &ReqObj );
			void		handleCookies(Request & ReqObj);
};

std::string longToString( long value );
