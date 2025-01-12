/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpodack <rpodack@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/18 13:01:34 by tstahlhu          #+#    #+#             */
/*   Updated: 2025/01/11 18:04:41 by rpodack          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _CONFIG_HPP_
# define _CONFIG_HPP_

# include <iostream>
# include <sstream>
# include <fstream>
# include <vector>
# include <algorithm>
# include "ServerBlock.hpp"

class ServerBlock;

class Config
{
	public:
		Config();
		Config(const Config&);
		~Config();
		Config&	operator=(const Config&);
		std::vector<ServerBlock> getServers()const;

		void		parse( const char* configFile );
		std::string removeComment(std::string line);
		void 		countBraces(std::string line, bool inLocationBlock);

		class FileOpenErrorException: public std::exception {
			public:
				virtual const char* what ( void ) const throw() {
					return "Could not open config file";
				}
		};

	class ErrorConfig : public std::exception
	{
		private:
		std::string _msg;
		public:
		ErrorConfig(std::string const &msg);
		~ErrorConfig()throw();
			
		const char *what()const throw();
	};

	private:
		int 						_countBrace;
		std::vector<ServerBlock>	_servers;
		void						checkPorts();
};


#endif