/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpodack <rpodack@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 18:16:20 by tstahlhu          #+#    #+#             */
/*   Updated: 2025/01/11 17:53:56 by rpodack          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include "../includes/Config.hpp"

#define	DEFAULT_CONFIG_FILE	"conf"

int main(int argc, char const *argv[])
{
    (void)argc;//
    (void)argv;//
    Server serv;

	try {
		Config	conf;

		if (argc == 2)
			conf.parse(argv[1]);
		else
			conf.parse(DEFAULT_CONFIG_FILE);
    	serv.run(conf);
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
    return 0;
}
