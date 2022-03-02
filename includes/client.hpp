/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/26 22:24:45 by mrosario          #+#    #+#             */
/*   Updated: 2022/03/02 16:03:17 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_H
# define CLIENT_H

class Client
{
	//typedef std::set<std::string, IRC_Server::t_Channel_Map::iterator, case_insensitive_less>	t_ChanMap;
	private:
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
		//t_ChanMap	_channels;	//map of channame-chaniterators to channels to which a client is member; removing client or destroying channel should change this.

		/* PRIVATE UTILS */
		size_t		get_param_count(void) const;
		std::string	get_cmd(void) const;
		bool		msg_buf_is_crlf_terminated(void) const;
	public:
		Client(void);
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
		//bool		check_channel_membership(std::) const;

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
		//void	set_channel_membership(IRC_Server::t_Channel_Map::iterator const & channel_iterator);
		//void	remove_channel_membership(IRC_Server::t_Channel_Map::iterator const & channel_iterator);

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

};

#endif
