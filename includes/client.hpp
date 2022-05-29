/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/26 22:24:45 by mrosario          #+#    #+#             */
/*   Updated: 2022/05/29 14:20:37 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_H
# define CLIENT_H

class Client
{
	public:
		typedef std::map<std::string, IRC_Server::t_Channel_Map::iterator, case_insensitive_less>	t_ChanMap;
	private:
		enum State
		{
			UNREGISTERED,
			REGISTERED,
			DISCONNECTED
		}			_state;
		enum Buffer_State
		{
			UNREADY,
			READY
		}					_in_buf_state;
		enum Buffer_State	_out_buf_state;
		std::string			_serveraddr;	//Server to which the Client is connected; can be used in replies requiring <servername>
		int					_sockfd;		//Client's sockfd
		int					_pass_attempts;	//Number of PASS commands sent by client on registration; limiting this to 10
		bool				_pass_validated;
		std::string			_nick;			//Client's nick
		std::string 		_clientaddr;	//Client's IP or canonical hostname, done by getaddrinfo() lookup
		std::string 		_hostname;		//Client's self-reported hostname?
		std::string 		_username;		//Client's username
		std::string 		_realname;		//Client's "real" name (sure it is, Gandalf Baggins)
		std::string			_msg_buf;
		std::string			_out_buf;
		std::string			_message;
		std::string 		_modes;
		t_ChanMap			_channels;		//map of channame-chaniterators to channels to which a client is member; removing client or destroying channel should change this.
		t_ChanMap			_invitelist;	//map of channame-chaniterators to channels to which a client is member; removing client or destroying channel should change this.
		IRC_Server *		_parent_server;

		/* PRIVATE UTILS */
		size_t		get_param_count(void) const;
		std::string	get_cmd(void) const;
		bool		msg_buf_is_crlf_terminated(void) const;
		void		leave_channel(t_ChanMap::iterator const & channel_it);
	public:
		Client(void);
		virtual ~Client(void);
		Client &	operator=(Client const & src);

		size_t const	pos; //Client's position in server's _clients array and _pfds array. Invariant.

		/* MOVE */
		void		move(Client & src);

		/* UTILS */
		static bool	is_endline(char const c);
		bool		is_disconnected();
		bool		msg_is_ready(void) const;
		bool		output_buf_has_unsent_data(void) const;
		bool		is_registered(void) const;
		bool		reg_pass_attempt(void);
		void		send_msg(std::string const & msg);
		void		send_output_buf(void);
		bool		leave_channel(std::string const & channel_name);
		void		leave_all_channels(void);

		/* SETTERS */
		void	flush_msg_buf(size_t stop);
		bool	append_to_msg_buf(char const (& server_msgbuf)[MSG_BUF_SIZE], int nbytes);
		void	set_sockfd(int sockfd);
		void	set_nick(std::string const & nick);
		void	set_username(std::string const & username);
		void	set_realname(std::string const & realname);
		bool	set_clientaddr(char const * remoteIP);
		bool	set_modes(std::string const & modes, std::string & applied_changes);
		bool	set_operator_mode(void);
		void	set_pass_validated(bool state);
		void	set_state_registered(void);
		void	set_state_disconnected(void);
		bool	set_channel_membership(IRC_Server::t_Channel_Map::iterator const & channel_iterator);
		bool	set_channel_invitation(IRC_Server::t_Channel_Map::iterator const & channel_iterator);
		void	set_parent_server(IRC_Server * parent_server);
		bool	set_out_buf(std::string const & msg);
		void	remove_channel_membership(IRC_Server::t_Channel_Map::iterator const & channel_iterator);
		void	remove_channel_membership(t_ChanMap::iterator const & channel_iterator);
		void	remove_channel_invitation(IRC_Server::t_Channel_Map::iterator const & channel_iterator);
		void	remove_channel_invitation(t_ChanMap::iterator const & channel_iterator);

		void	clear(void);

		/* GETTERS */			
		std::vector<std::string>				get_message(void);
		std::string								get_source(void) const;
		std::string const &						get_serveraddr(void) const;
		std::string const &						get_nick(void) const;
		std::string const &						get_username(void) const;
		std::string const &						get_realname(void) const;
		std::string const &						get_hostname(void) const;
		std::string const &						get_clientaddr(void) const;
		std::string const &						get_modes(void) const;
		int										get_sockfd(void) const;
		int										get_pass_attempts(void) const;
		size_t									get_pos(void) const;
		bool									get_pass_validated(void) const;
		std::pair<t_ChanMap::iterator, bool>	get_joined_channel(std::string const & channel_name);
		std::pair<t_ChanMap::iterator, bool>	get_invited_channel(std::string const & channel_name);
		std::string const						get_invites(void);
		t_ChanMap &								get_chanlist(void);
		std::string const &						see_next_message(void) const;
		std::string const &						see_msg_buf(void) const;
};

#endif
