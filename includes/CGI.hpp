/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mwurch <mwurch@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 18:15:32 by tstahlhu          #+#    #+#             */
/*   Updated: 2024/11/22 19:53:23 by mwurch           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cstdlib>
#include <sys/stat.h>
#include <ctime>  // For time()
#include "Request.hpp"
#include "Response.hpp"

#define TEMP_FOLDER "tmp_php/"

class Request;
class Response;

class CGI
{
    private:
      char **								_env;
		  std::string						_cgi_path;
      int                   _pipe_out[2];
      int                   _PID;
      std::time_t           _Time;

    protected:
      std::map< std::string, std::string>	_env_map;
        
    
    public:
        CGI( void );
        CGI( Request & Req );
        ~CGI( void );
        CGI &operator=( const CGI &a );
        
		    void		initEnv( Request & Req );
        void    createEnvChar( void );
        void    runCGI( Request & ReqObj);
        void	  createTmpFolder( void );
        void    checkContentLength(  Request& Req);
        // static void    handle_alarm( int sig);

        int             getPipe_OutFD()const;
        std::time_t     getTime()const;
        int             getPID()const;

};