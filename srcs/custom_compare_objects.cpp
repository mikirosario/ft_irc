/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   custom_compare_objects.cpp                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/27 19:19:59 by mrosario          #+#    #+#             */
/*   Updated: 2022/06/09 17:39:22 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ircserv.hpp"

/*!
** @brief	Performs a case-insensitive comparison of str1 with str2 in which
**			ASCII characters 97-122 (a-z) are equal to ASCII characters 65-90
**			(A-Z). This is meant ONLY for username, servername and channelname
**			comparisons!
**
** @details	Use in std::locale("C") only and only with strings containing
**			exclusively ASCII characters. This is the default C++ locale, but to
**			be extra sure it is explicitly set at program initiation. For
**			casemapping=ascii mode.
** @param	str1	First string to compare.
** @param	str2	Second string to compare.
** @return			true if both strings compare equal, otherwise false
*/
bool		case_insensitive_ascii_compare(std::string const & str1, std::string const & str2)
{
	if (str1.size() == str2.size())
	{
		for (std::string::size_type i = 0, strsize = str1.size(); i < strsize; ++i)
			if (std::toupper(str1[i]) != std::toupper(str2[i])) //locale is "C"; names are limited to ASCII alphanumeric
				return (false);
		return (true);
	}
	return (false);
}

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
bool	case_insensitive_less::operator() (std::string const & str1, std::string const & str2) const
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

/*!
** @brief	Performs a case-insensitive wildcard-sensitive equality match, where
**			'?' stands for any single character and '*' stands for any series of
**			characters until the next character.
**
**			Thanks geeksforgeeks.com. ;)
** @param	normal_str			The normal string to compare against.
** @param	wildcard_pattern	string with the wildcard pattern.
** @return	true if @a normal_str matches any string in the wildcard pattern,
**			otherwise false
*/
bool	wildcard_matching_equality(std::string const & normal_str, std::string const & wildcard_pattern)
{
	std::string	str = normal_str;
	std::string pattern = wildcard_pattern;
	size_t		str_size = str.size();
	size_t		pattern_size = pattern.size();
	bool		lookup[str_size + 1][pattern_size + 1];										//bool matrix; x normal_str, y wildcard_str

	if (pattern_size == 0)																	//empty pattern can only match with empty string
		return (str_size == 0);
	std::memset(lookup, false, sizeof(lookup));												//initialize bool matrix to false
	for (std::string::iterator it = str.begin(), end = str.end(); it != end; ++it)			//make normal_str case-insensitive
		*it = std::toupper(*it);
	for (std::string::iterator it = pattern.begin(), end = pattern.end(); it != end; ++it)	//make wildcard_str case-insensitive
		*it = std::toupper(*it);
	lookup[0][0] = true;																	//empty pattern matches empty string
	for (size_t j = 1; j <= pattern_size; ++j)												//Only lone '*' can match with empty string
		if (pattern[j - 1] == '*')
			lookup[0][j] = lookup[0][j - 1];
	for (size_t i = 1; i <= str_size; ++i)													//fill in table from bottom up
	{
		for (size_t j = 1; j <= pattern_size; ++j)
		{
			// Two cases if we see a '*'
            // a) We ignore ‘*’ character and move
            //    to next  character in the pattern,
            //     i.e., ‘*’ indicates an empty sequence.
            // b) '*' character matches with ith
            //     character in input
			if (pattern[j - 1] == '*')
				lookup[i][j] = lookup[i][j - 1] || lookup[i - 1][j];
			
			// Current characters are considered as
            // matching in two cases
            // (a) current character of pattern is '?'
            // (b) characters actually match
			else if (pattern[j - 1] == '?' || str[i - 1] == pattern[j - 1])
				lookup[i][j] = lookup[i - 1][j - 1];
			else
				lookup[i][j] = false;
		}
	}
	return lookup[str_size][pattern_size];
}

/*!
** @brief	Performs a case-insensitive wildcard-sensitive equality match of a
**			normal string against a wildcard pattern, a wildcard pattern against
**			a normal string or a wildcard pattern against a wildcard pattern.
**
** @details	This can work as a double-equality match (wildcard_pattern on either
**			side or both sides) by inverting the parameter order and or-ing both
**			results.
** @param	str1	The first string to compare.
** @param	str2	The second string to compare.
** @return	true if the strings compare as equal, otherwise false
*/
bool	dual_wildcard_matching_equality(std::string const & str1, std::string const & str2)
{
	return wildcard_matching_equality(str1, str2) || wildcard_matching_equality(str2, str1);
}
