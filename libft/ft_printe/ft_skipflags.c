/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_skipflags.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: majosue <majosue@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/29 12:14:33 by majosue           #+#    #+#             */
/*   Updated: 2020/10/24 14:04:34 by majosue          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printe.h"

/*
** Move pointer str to first non flag position
*/

int	ft_skipflags(char **str)
{
	char *flags[5];
	char *str1;

	str1 = *str;
	ft_gettab(&flags, 0);
	ft_chkflags(str, flags);
	if (str1 == *str)
		return (0);
	return (1);
}
