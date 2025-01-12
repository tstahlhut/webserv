/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpodack <rpodack@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 18:16:43 by tstahlhu          #+#    #+#             */
/*   Updated: 2025/01/11 18:22:24 by rpodack          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include "../includes/Request.hpp"
#include "../includes/Response.hpp"
#include "../includes/Config.hpp"
#include "../includes/ServerBlock.hpp"

bool Server::RUN_SERVER = true;

Server::Server() : _nfds(1), _maxConnection(10), close_conn(false), compress_array(false) {
	
	memset(this->fds, 0, sizeof(fds));
}

Server::Server(const Server &) {}

Server::~Server() {}

Server &Server::operator=(const Server &a)
{
	// fds = a.fds;
	RUN_SERVER = a.RUN_SERVER;
	_nfds = a._nfds;
	_listen_fd = a._listen_fd;
	_maxConnection = a._maxConnection;
	_AddrMap = a._AddrMap;
	addr = a.addr;
	close_conn = a.close_conn;
	compress_array = a.compress_array;
	// buffer = a.buffer;
	_ReqArr = a._ReqArr;
	_FdSB = a._FdSB;
	
	return (*this);
}

// added listen_socket_fail to shorten create_listen_socket
int	listen_socket_fail( std::string errorMsg, int & fd ) {

	if (fd >= 0)
		close(fd);
	std::cerr << errorMsg << std::endl;
	return -1;
}

int Server::create_listen_socket(int SockPort, ServerBlock SB)
{
	int on = 1;

	int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_fd < 0)
		return listen_socket_fail("socket() failed", listen_fd);

	// Allow socket descriptor to be reusable
	if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)) < 0)
		return listen_socket_fail("setsockopt() failed", listen_fd);
		
	// server becomes non-blocking
	if (fcntl(listen_fd, F_SETFL, O_NONBLOCK) < 0)
		return listen_socket_fail("fcntl() failed", listen_fd);

	struct sockaddr_in ad;
	memset(&ad, 0, sizeof(addr));
	ad.sin_family = AF_INET; // ipv4
	ad.sin_addr.s_addr = htonl(SB.getHost());
	ad.sin_port = htons(SockPort);

	_AddrMap[listen_fd] = ad;
	if (bind(listen_fd, (struct sockaddr *)&ad, sizeof(ad)) < 0)
		return listen_socket_fail("bind() failed", listen_fd);
	
	if (listen(listen_fd, _maxConnection) < 0)
		return listen_socket_fail("listen() failed", listen_fd);

	return listen_fd;
}

void		Server::ChildTerminator(){
	
	if (_Kindergarten.empty())
		return ;
	std::map<int, std::time_t>::iterator it;
	for (size_t i = 0; i < _Kindergarten.size(); i++)
	{
		
		it = _Kindergarten.begin();
		if ((std::time(NULL) - it->second) >= 5){
			kill(it->first, SIGKILL);
			// find corresponding request 
			std::map<int, Request>::iterator itReq = _ReqArr.begin();
			for (size_t j = 0; j < _ReqArr.size(); j++)
			{
				if (itReq->second.getPIDChild() == it->first ){
					itReq->second.setErrorCode(500);
					itReq->second.setPID(0);
					int fd = itReq->second.getFD(); 
					int k = 0;
					while (k < _nfds)
					{
						if (fds[k].fd == fd){
							fds[k].events = POLLOUT;
							break;	
						}
						k++;
					}
					// remove the fd of the pipe from fds
					removePipeFD();
				}
				itReq++;
			}
			_Kindergarten.erase(it->first);
			break;
		}
		it++;
	}
	
}

std::vector<ServerBlock>	Server::initialize_listening_sockets( Config &conf ) {

	// create several sockets to listen from
	std::vector<ServerBlock> _servers = conf.getServers();
	int lens = _servers.size();
	int countFD = 0;
	for (int i = 0; i < lens; i++)
	{
		int lenp = _servers[i]._port.size();
		for (int j = 0; j < lenp; j++)
		{
			fds[countFD].fd = create_listen_socket(_servers[i]._port[j], _servers[i]);
			if (fds[countFD].fd == -1)
				return _servers; // what does this return do?!
			// map fd with ServerBlock
			_FdSB[fds[countFD].fd] = _servers[i];
			_nfds++;
			fds[countFD].events = POLLIN; // we want to receive
			countFD++;
		}
	}
	return _servers;
}

void	Server::handle_new_connections( int i ) {
	
	int new_connection = 0;
	while (new_connection != -1)
	{
		socklen_t addr_len = sizeof(_AddrMap[fds[i].fd]);
		new_connection = accept(fds[i].fd, (struct sockaddr *)&_AddrMap[fds[i].fd], &addr_len);
		if (new_connection < 0)
		{
			// check if error occured --> errormsg or no incomming connections --> continue
			if (errno != EWOULDBLOCK) // errno is not allowed to be checked
			{
				my_perror("accept() failed");
				RUN_SERVER = false;
			}
			break;
		}
		std::cout << "New Connection " << new_connection << std::endl;
		fds[_nfds].fd = new_connection;
		fds[_nfds].events = POLLIN;
		_FdSB[fds[_nfds].fd] = _FdSB[fds[i].fd];
		_nfds++;
		if (fcntl(new_connection, F_SETFL, O_NONBLOCK) < 0)
		{
			my_perror("fcntl() failed");
			close(new_connection);
			return; // why is here a return? 
		}
	}
}

void	checkMaxBodySize( Request & ReqObj, std::string request)
{
	size_t bodyStart = request.find("\r\n\r\n") + 4;
	size_t clientBodySize = (request.size() - bodyStart);

	if (clientBodySize > ReqObj.getSB().getMaxBody() ) {
		ReqObj.setErrorCode(413);
		std::cout << "Upload too large" << std::endl;
	}
}
void	Server::receiveRequest( int i) {

	long totalBits = 0;
	std::string tmpStr = "";
	bool headerParsed = false;
	bool isChunked = false;
	long contentLength = 0;
	long rc;

	std::map<int, Request>::iterator it = _CGIFDReq.find(fds[i].fd);
	if (it == _CGIFDReq.end()) // check fd if it is maped with request obj -> read from pipe
	{
		if (_ReqArr.find(fds[i].fd) == _ReqArr.end()) {
			_ReqArr[fds[i].fd] = Request( _FdSB[fds[i].fd]);
			std::map<int, Request>::iterator it = _ReqArr.find(fds[i].fd);
			it->second.setFD(fds[i].fd); // safe fd of the request in the request
		}	
		
		while (!isRequestComplete(tmpStr, contentLength, isChunked, headerParsed)) // Loop for receiving data: sollte gest[ckelt werden k;nnen
		{
			rc = recv(fds[i].fd, buffer, sizeof(buffer), 0);
			if (rc == 0)
			{
				std::cout << "Connection " << fds[i].fd << " closed" << std::endl;
				close_conn = true;
				close(fds[i].fd);
				fds[i].fd = -1;
				break;
			}
			
			if (rc < 0)
			{
				_ReqArr[fds[i].fd] = Request(500, _FdSB[fds[i].fd]);
				fds[i].events = POLLOUT;
				break;
			}

			tmpStr.append(buffer, rc);
			totalBits += rc;
		
			if (isRequestComplete(tmpStr, contentLength, isChunked, headerParsed))
			{				
				checkMaxBodySize( _ReqArr[fds[i].fd], tmpStr); 
				fds[i].events = POLLOUT;
				// parse request
				_ReqArr[fds[i].fd].ParseRequest( tmpStr, totalBits );
				if (_ReqArr[fds[i].fd].getCGI() && _ReqArr[fds[i].fd].getErrorCode() == 200)
				{
					CGI CGIObj(_ReqArr[fds[i].fd]);
					CGIObj.createEnvChar();
					CGIObj.runCGI(_ReqArr[fds[i].fd]);
					_StartTime = CGIObj.getTime();
					// map childprocess of CGI with starttime for timeout
					_Kindergarten[CGIObj.getPID()] = _StartTime;
					// listen to fd with poll
					fds[_nfds].fd = CGIObj.getPipe_OutFD();
					fds[_nfds].events = POLLIN;
					fds[i].events = POLLIN;
					_CGIFDReq[fds[_nfds].fd] = _ReqArr[fds[i].fd]; // map fd with request obj
					_nfds++;
					break;
				}
				
			}
		}
	}
	else
	{
		std::string output = "";
		ssize_t bytesRead;
		while ((bytesRead = read(fds[i].fd, buffer, sizeof(buffer) - 1)) > 0)
		{
			buffer[bytesRead] = '\0';
			output += buffer;
		}
		int Reqfd = _CGIFDReq[fds[i].fd].getFD();
		_ReqArr[Reqfd].setCGIoutput(output); 

		// set request fd to listen for pollout
		for ( int j = 0; j < _nfds; j++)
		{
			if (fds[j].fd == Reqfd)
			{
				fds[j].events = POLLOUT;
				break;
			}
		}

		close(fds[i].fd);
		_CGIFDReq.erase(fds[i].fd);
		fds[i].fd = -1;
		compress_array = true;
		return;
	}
}

void Server::checkCookies(int i) {

	int Cookie = getReqObj(fds[i].fd).getCookie();
	if (Cookie  >= 0){
		for (std::list<int>::iterator it = _SessionID.begin(); it != _SessionID.end(); ++it) {
			if (*it == Cookie){
				break;
			}
		}
	}
	else {
		getReqObj(fds[i].fd).setSession(_SessionID);
		_SessionID.push_back(_SessionID.size() + 1);
	}
}

void	Server::sendResponse( std::string resp, int i ) {
	
	size_t total_sent = 0;
	size_t total_size = resp.size();
	long rc = send(fds[i].fd, resp.c_str() , total_size , 0);
	close_conn = true;
	if (rc == 0) {
		return ;
	}
	if (rc < 0) {
		my_perror("send() failed");
		return ;
	}
	total_sent += rc; // Update the total bytes sent
	return ;
}

void	Server::close_connection( int i ) {

	if (close_conn) {
		std::cout << "closed connection" << std::endl;
		_ReqArr.erase(fds[i].fd);
		_FdSB.erase(fds[i].fd);
		if (fds[i].fd != -1)
			close(fds[i].fd);
		fds[i].fd = -1;
		compress_array = true;
		close_conn = false;
	}
	if (compress_array)	{
		compress_array = false;
		compressArray();
	}
}


void Server::run(Config &conf)
{
	int current_size, i;
	signal(SIGINT, ctrlC);
	signal(SIGCHLD, handle_sigchld);

	// create several sockets to listen from
	std::vector<ServerBlock> _servers = initialize_listening_sockets(conf);
	
	// main server loop
	while (RUN_SERVER || !clearConnections())
	{
		if (poll(fds, _nfds, 50) < 0)
			continue;
			
		ChildTerminator();
		compressArray();

		// iterating through all incoming request (?) -> fds
		current_size = _nfds;
		for (i = 0; i < current_size; i++)
		{
			// case 1: no event happened: skip it (but why continue and not break?!)
			if (fds[i].revents == 0)
				continue;

			// case 2: if nothing is set, reset connection, why if and not else if?
			if ((!(fds[i].revents & POLLIN) && !(fds[i].revents & POLLOUT)) || fds[i].revents & POLLHUP)
			{
				close(fds[i].fd);
				fds[i].fd = -1;
				break; // connection must be closed and array compressed 
				// break so that it does not run into else (POLLIN/ POLLOUT)
			}

			//case 3: it's a listenfd: accept all incomming connections on listening socket
			if (isListenFD(fds[i].fd))
				handle_new_connections(i);
			//case 4: it is POLLIN or POLLOUT
			else
			{
				if (fds[i].revents == POLLIN)
				{
					//resolve_hostname(_FdSB[fds[i].fd].getServerName(), "127.0.0.1");
					receiveRequest(i);
				} // End of loop for receiving data
				else if (fds[i].revents == POLLOUT) {
					checkCookies(i);
					Response RespObj(getReqObj(fds[i].fd));
					sendResponse(RespObj.getResponse(), i);
						//break; // do not close the connection yet but check for POLLIN and POLLOUT again
				}
			}
			//close connection if close_conn is set and compress array if necessary
			close_connection(i);
		} //end of for loop (iteration through all fds)
	} //end of main loop
}

//////////////////////////////////////////

void my_perror(const std::string errorMsg)
{
	std::cerr << errorMsg << std::endl;
}

void ctrlC(int sig)
{
	(void)sig;
	Server::setRun_Server();
}

void Server::setRun_Server()
{
	RUN_SERVER = false;
}

bool Server::isListenFD(int currFD)
{
	for (std::map<int, struct sockaddr_in>::iterator it = _AddrMap.begin(); it != _AddrMap.end(); ++it)
	{
		if (it->first == currFD)
			return (true);
	}
	return (false);
}

bool Server::clearConnections()
{
	if (!RUN_SERVER)
	{
		int nf = getNfds();
		int i = nf;
		while ((*this).fds[i].fd)
		{
			close(fds[i].fd);
			_ReqArr.erase(fds[i].fd);
			fds[i].fd = -1;
			compress_array = true;
			close_conn = false;
			i--;
		}
		return (true);
	}
	return (false);
}

bool Server::getRun_Server() const
{
	return (RUN_SERVER);
}

Request &Server::getReqObj(int fd)
{

	return (_ReqArr[fd]);
}

int Server::getNfds()
{
	return (_nfds);
}

bool isRequestComplete(std::string request, long &len, bool &isChunked, bool &headerParsed)
{
	if (!headerParsed)
	{
		size_t headerEnd = request.find("\r\n\r\n");
		if (headerEnd != std::string::npos)
		{
			headerParsed = true;
			size_t lenPos = request.find("Content-Length:");
			if (lenPos != std::string::npos)
			{
				std::istringstream iss(request);
				iss.seekg(lenPos + 15);
				iss >> len;
			}
			size_t chunkedPos = request.find("Transfer-Encoding: chunked");
			if (chunkedPos != std::string::npos)
			{
				isChunked = true;
			}
		}
	}

	if (headerParsed)
	{
		if (isChunked)
			return request.find("0\r\n\r\n") != std::string::npos;
		else if (len > 0)
		{
			size_t bodyStart = request.find("\r\n\r\n") + 4;
			return (request.size() - bodyStart) >= static_cast<size_t>(len);
		}
		else
		{
			return true;
		}
	}
	return false;
}

void Server::handle_sigchld(int sig) {
    // Reap the child process
	(void) sig;
   // waitpid(-1, NULL, WNOHANG); // No-block cleanup of any terminated child
}

void		Server::compressArray(){
	for (int i = 0; i < _nfds; i++)
	{
		if (fds[i].fd == -1)
		{
			while (fds[i+1].fd == -1)
				i++;
			for (int j = i; j < 200 - 1 ; j++)
			{
				fds[j].fd = fds[j + 1].fd;
				fds[j].events = fds[j + 1].events;
				fds[j].revents = fds[j + 1].revents;
			}
			_nfds--;
			while (fds[i].fd == 0)
				i--;
		}
	}
}


void		Server::removePipeFD(){
	std::map<int, Request>::iterator rmPipeFd = _CGIFDReq.begin();
	for (size_t j = 0; j <_CGIFDReq.size(); j++)
	{
		int PipeFd = rmPipeFd->first;
		if (rmPipeFd->second.getChildFD() == PipeFd){
			for (int f = 0; f < _nfds; f++)
			{
				if (fds[f].fd == PipeFd){
					close(fds[f].fd);
					fds[f].fd = -1;
					compressArray();											
					_CGIFDReq.erase(PipeFd);
					break;
				}
			}
			break;
		}
		rmPipeFd++;
	}
}
