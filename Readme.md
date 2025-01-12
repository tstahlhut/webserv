# Webserv - A 42 School Project

## About Webserv

Webserv is a lightweight C++ web server designed and tested on Ubuntu Linux. It supports PHP CGI scripts for dynamic content generation.

To test the web server, we’ve created a sample website (located in the /www/ folder). This website includes an image upload feature that allows you to upload images, which are then displayed in an art gallery on the website.

#### Key Features:

- PHP CGI support for dynamic content processing.
- Image upload functionality with an integrated gallery to display uploaded images.
- Designed to run on Ubuntu Linux (may not be compatible with other operating systems, especially Windows).


## Usage

*Webserv* is a C++ web server, designed and tested on Ubuntu Linux. It may therefore not function correctly on other operating systems. It uses php for CGI scripts. Below are the steps to compile, configure, and run the server on a compatible Linux environment. 

### Prerequisites

1. Ubuntu Linux: The server is built and tested on Ubuntu 20.04+.

2. Dependencies: Ensure the following dependencies are installed:

	- C++ Compiler: Tested with g++ (GCC version 9.3 or higher).

    - Build Tools: make.

    - PHP CGI: The server relies on php-cgi (version 8.1.2 or higher) for handling file uploads. Ensure that php-cgi is installed at: /usr/bin/php-cgi.

	To install the necessary programs, run the following commands:

		sudo apt update
		sudo apt install g++ make php-cgi

### Compilation and Setup

1. Clone the Repository:

		git clone git@github.com:mwurch/webserv.git webserv
		cd webserv

2. Build the Server:

		make

3. Run the server:

		./webserv

## Resources

Here is a list of resources we used. The list is not extensive but comprises the best resources we used.

#### CGI

- [CGI programs in C++](https://www.tutorialspoint.com/cplusplus/cpp_web_programming.htm)

- [GNU cgicc](https://www.gnu.org/software/cgicc/doc/cgi_overview.html): brief introduction to CGI programming

- [Simple C# Webserver with PHP support using CGI](https://blog.dragonbyte.de/2021/02/03/simple-c-web-server-with-php-support-using-cgi/)