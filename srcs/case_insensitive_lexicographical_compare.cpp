/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   case_insensitive_lexicographical_compare.cp        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/27 19:19:59 by mrosario          #+#    #+#             */
/*   Updated: 2022/02/27 20:14:40 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ircserv.hpp"

/*!
** @brief	Performs a case-insensitive lexicographical compare of
**			str1 with str2 in which ASCII characters 97-122 (a-z)
**			are equal to ASCII characters 65-90 (A-Z), returning
**			true if str1 is less than str2 and otherwise false.
**
** @details	Use in std::locale("C") only and only with strings containing
**			exclusively ASCII characters. This is the default C++ locale, but to
**			be extra sure it is explicitly set at program initiation.
** @param	str1	First string to compare.
** @param	str2	Second string to compare.
** @return			true if str1 is less than str2, otherwise false
*/
bool	IRC_Server::case_insensitive_less::operator() (std::string const & str1, std::string const & str2) const
{
	typedef std::string::const_iterator	str_it;
	str_it	index_str1 = str1.begin(), last_str1 = str1.end(),
			index_str2 = str2.begin(), last_str2 = str2.end();

	while (index_str1 != last_str1)
	{
		if (index_str2 == last_str2 || std::toupper(*index_str2) < std::toupper(*index_str1))
			return (false);
		else if (std::toupper(*index_str1) < std::toupper(*index_str2))
			return (true);
		++index_str1; ++index_str2;
	}
	return (index_str2 != last_str2);
}
