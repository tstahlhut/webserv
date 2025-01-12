/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tstahlhu <tstahlhu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 18:15:55 by tstahlhu          #+#    #+#             */
/*   Updated: 2025/01/04 18:30:32 by tstahlhu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <unistd.h>
#include <iostream>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <map>
#include <string>
#include <signal.h>
#include <netdb.h>
#include <ctime>  // For time()
#include <list>
#include <netdb.h> 
#include "Request.hpp"
#include "Response.hpp"
#include "Config.hpp"


class Server
{
		public:
			Server();
			Server(const Server&);
			~Server();
			Server&	operator=(const Server& a);
			void			run(Config &conf);
			static void		setRun_Server();

		private:
		//	Config	conf;
			std::string							_server_name;
			struct	pollfd 						fds[200]; //ok to hard code or should it be more/ flexible?; test with 500; should not overflow
			static bool							RUN_SERVER; 
			int									_nfds; //number of established connections
			int									_listen_fd; //server fd (listening part)
			int									_maxConnection; // max allowed connections -> aus config file?
			// int									_PORT; // aus config file
			std::map<int, struct sockaddr_in> 	_AddrMap;
			struct sockaddr_in 					addr; //info for socket
			//bool								COMPRESS_FDS; 
			bool								close_conn; //if true, connection will be closed
			bool								compress_array ;//if true, tidy up fds array
			char								buffer[20000]; //request is saved here
			std::map <int, Request> 			_ReqArr; //int = fd; request object; all infos of request; um request fd zuordnen zu k;nnen
			std::map<int, ServerBlock>			_FdSB; // int = fd; SB = virtuell server from config
			std::map<int, Request>				_CGIFDReq; // amps fd of gci _pipe_out with request
			std::map<int, std::time_t>			_Kindergarten; // maps starttime with pid
			std::time_t							_StartTime;
			std::list<int>						_SessionID;

			std::vector<ServerBlock>	initialize_listening_sockets( Config &conf );
			int			create_listen_socket(int port, ServerBlock SB);
			void		handle_new_connections( int i );
			void		receiveRequest( int i);
			void		sendResponse( std::string resp, int i );
			void		checkCookies(int i);
			void		close_connection( int i );
			bool 		isListenFD(int currFD);
			Request 	&getReqObj(int fd);
			bool		getRun_Server()const;
			bool		clearConnections();
			int			getNfds();
			static void handle_sigchld(int sig);
			void		ChildTerminator();
			void		compressArray();
			void		removePipeFD();
		
};

void	ctrlC(int sig);

void	my_perror(const std::string errorMsg);
bool	isRequestComplete(std::string request, long &len, bool &isChunked, bool &headerParsed);
