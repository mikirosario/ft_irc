#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "ircserv.hpp"


		enum State
		{
			CONFIG_FAIL = -1,
			OFFLINE,
			ONLINE,
			RESTART
		}			_state;

		// No tocare mucho esta clase por no saber si solo tendremos la clase client
		
		class User
		{
			public:
				User(){};
				User(std::string other_nickname){nickmame = other_nickname;};
				~User(void){};

				std::string getNick(){return(this->nickmame);}
				
				User &	operator=(User const &other)
				{
					if (*this == other)
						this->nickmame = other.nickmame;
        			return(*this);
				}

				bool operator==(const User &other) const 
				{						
        			return (this->nickmame == other.nickmame);
				}
			private:
				std::string nickmame;
		};

		class Client
		{
			private:
			
				typedef std::map<std::string, User>::iterator t_user_ptr;
				enum State
				{
					UNREGISTERED,
					REGISTERED
				}			_state;
				enum Buffer_State
				{
					UNREADY,
					READY
				}			_buf_state;
				
				std::string	_serveraddr;	//Server to which the Client is connected; can be used in replies requiring <servername>
				int			_sockfd;		//Client's sockfd
				int			_pass_attempts;	//Number of PASS commands sent by client on registration; limiting this to 10
				bool		_pass_validated;
				std::string	_nick;			//Client's nick
				std::string _clientaddr;	//Client's IP or canonical hostname, done by getaddrinfo() lookup
				std::string _hostname;		//Client's self-reported hostname?
				std::string _username;		//Client's username
				std::string _realname;		//Client's "real" name (sure it is, Gandalf Baggins)
				//std::string	_longname;		//Client's name in format nickname!username@hostname. maybe replace build_source with this
				std::string	_msg_buf;
				std::string	_message;

				/* PRIVATE UTILS */
				size_t		get_param_count(void) const;
				std::string	get_cmd(void) const;
				bool		msg_buf_is_crlf_terminated(void) const;

			public:

				Client(void);
				Client(User const &src);
				~Client(void);
				Client &	operator=(Client const & src);

				size_t const	pos; //Client's position in server's _clients array and _pfds array. Invariant.

				/* MOVE */
				void		move(Client & src);

				/* UTILS */
				static bool	is_endline(char const c);
				bool		msg_is_ready(void) const;
				bool		is_registered(void) const;
				bool		reg_pass_attempt(void);
				void		send_msg(std::string const & msg) const;

				/* SETTERS */
				void	flush_msg_buf(size_t stop);
				bool	append_to_msg_buf(char const (& server_msgbuf)[MSG_BUF_SIZE], int nbytes);
				void	set_sockfd(int sockfd);
				void	set_nick(std::string const & nick);
				void	set_username(std::string const & username);
				void	set_realname(std::string const & realname);
				bool	set_clientaddr(char const * remoteIP);
				void	set_hostname(std::string const & hostname);
				void	set_pass_validated(bool state);
				void	set_state_registered(void);

				void	clear(void);

				/* GETTERS */
				std::vector<std::string>	get_message(void);
				std::string					get_source(void) const;
				std::string const &			get_serveraddr(void) const;
				std::string const &			get_nick(void) const;
				std::string const &			get_username(void) const;
				std::string const &			get_realname(void) const;
				std::string const &			get_hostname(void) const;
				std::string const &			get_clientaddr(void) const;
				int							get_sockfd(void) const;
				int							get_pass_attempts(void) const;
				size_t						get_pos(void) const;
				bool						get_pass_validated(void) const;
				std::string const &			see_next_message(void) const;
				std::string const &			see_msg_buf(void) const;


				bool operator<(const Client &other) const 
				{
        			return (this->_nick < other._nick);
				}

				bool operator>(const Client &other) const 
				{
        			return (this->_nick > other._nick);
				}

				bool operator<=(const Client &other) const 
				{
        			return (this->_nick <= other._nick);
				}

				bool operator>=(const Client &other) const 
				{
        			return (this->_nick >= other._nick);
				}

				bool operator==(const Client &other) const 
				{
        			return (this->_nick == other._nick);
				}

				bool operator!=(const Client &other) const 
				{
        			return (this->_nick != other._nick);
				}

		};

#endif