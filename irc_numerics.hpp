/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc_numerics.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/12 12:54:08 by miki              #+#    #+#             */
/*   Updated: 2022/02/21 15:59:30 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRC_NUMERICS_H
# define IRC_NUMERICS_H

/* -- RPL NUMERICS -- */
#define RPL_WELCOME "001"

/* -- ERR NUMERICS -- */
#define ERR_UNKNOWNERROR "400"
#define ERR_UNKNOWNCOMMAND "421"
#define ERR_NONICKNAMEGIVEN "431"
#define ERR_ERRONEOUSNICKNAME "432"
#define ERR_NICKNAMEINUSE "433"
#define ERR_NEEDMOREPARAMS "461"
#define ERR_ALREADYREGISTERED "462"
#define ERR_PASSWDMISMATCH "464"

#endif