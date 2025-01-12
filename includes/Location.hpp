/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tstahlhu <tstahlhu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 18:15:41 by tstahlhu          #+#    #+#             */
/*   Updated: 2024/11/16 18:15:42 by tstahlhu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include "ServerBlock.hpp"

class Location
{
	/* adding vars here means adding them void		ServerBlock::useLocationVar()*/
    private:
        std::string     			_LocationIndex;
    	std::string					_LocationRoot;
		std::vector<std::string>	_LocationAllowMethod;// is filled but not used in Sb yet
    	std::string					_LocationURI;
		bool 						_LocationAutoindex; // turn on/off directory listing
		std::string					_LocationReturn;
    
    public:
    	Location();
		Location(const Location&);
    	~Location();
		Location	&operator=(const Location &a);

	class ErrorLB : public std::exception
	{
		private:
		std::string _msg;
		public:
		ErrorLB(std::string const &msg);
		~ErrorLB()throw();
			
		const char *what()const throw();
	};
    
    	void			initVarLocation(std::string line);
		void 			checkVarLoc(std::string &tmp, std::stringstream &sLine);
		std::string		trim(const std::string& str);
		void			parseLocURI(std::string line);
		std::string		removeLeadingDot(const std::string& input);
		std::string		getCurrentWorkingDirectory();
		std::string					getLocationRoot();
		std::string					getLocationIndex();
		std::string					getLocationURI();
		std::vector<std::string>	getLocationAllowMethod();
		bool						getLocationAutoindex();
		std::string					getReturn();

};
