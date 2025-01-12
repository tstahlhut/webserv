/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerBlock.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tstahlhu <tstahlhu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 12:44:22 by tstahlhu          #+#    #+#             */
/*   Updated: 2025/01/04 17:47:09 by tstahlhu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cstdlib>  // for atoi
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <dirent.h> // for opendir
#include <fcntl.h>
#include <unistd.h>// open close
#include <string.h> 

#include "Location.hpp"

/*
	what about auto index ?
*/
class Location;


class ServerBlock
{
	private:
		std::string 					_root;			// root directory of the first server block -> only parsed but not used yet
		std::string						_index;			// what filetype it's going to be served ?
		std::string 					_serverName;	// serverName directory of the first server block -> only parsed but not used yet
		unsigned int					_host;			// ip adress
		unsigned int					_client_max_body_size;
		
		std::vector<std::string>		_AllowedMethods;// fill in constructor and if in Location use that
		std::string						_whatScript;	// .php or .py
		bool 							_autoindex; 	// turn on/off directory listing
		std::string						_cgiRoot;
		std::string						_reDir;			// redirects request 
		

	public:
		ServerBlock();
		ServerBlock(const ServerBlock&);
		~ServerBlock();
		ServerBlock		&operator=(const ServerBlock &a);
		std::vector<Location>			_LocVec;		// contains the Location objects that belong to this server
		std::vector<int>				_port;			// ports of different server blocks -> used to to bind several ports to listen to
		std::map<int, std::string>		_errorPages;	// maps errorCode with path to errorpage.html

		class ErrorSB : public std::exception
		{
			private:
				std::string _msg;

			public:
				ErrorSB(std::string const &msg);
				~ErrorSB()throw();				
				const char *what()const throw();
		};

		// Getter Functions
		std::string					getRoot( void );
		std::string					getServerName( void );
		std::string					getIndex( void );
		std::vector<std::string>	getMethodVec( void );
		bool						getAutoindex( void );
		std::vector<Location>		getLocVec( void );
		std::map<int, std::string>	getErrPagesMap( void );
		unsigned int				getMaxBody( void );
		u_int32_t					getHost( void );
		std::string					getCGIRoot( void );
		std::string					getReDir(void);

		// Member Functions
		void						initVarServerBlock( std::string line) ;
		bool 						isLocationBlock( const std::string& line );
		void						checkVar( std::string &tmp, std::stringstream &sLine );
		void						setWhatScript( std::string &whatScript );
		void						useCGILocation( void );
		void						useLocation( Location Loc );
		std::string					trim( const std::string& str );
		unsigned int				ipConvert( const std::string host );
		std::string 				removeLeadingDot(const std::string& input);
		std::string 				getCurrentWorkingDirectory();

};


