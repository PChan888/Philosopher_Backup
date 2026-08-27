/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kai <kai@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/06 16:59:46 by kai               #+#    #+#             */
/*   Updated: 2026/07/06 16:59:47 by kai              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
** Minimal atol for digit-only strings (arguments are pre-validated).
** Caps at INT_MAX + 1 so check_args can reject oversized values.
*/
long	ft_atol(const char *str)
{
	long	n;
	int		i;

	n = 0;
	i = 0;
	while (str[i] >= '0' && str[i] <= '9')
	{
		n = n * 10 + (str[i] - '0');
		if (n > 2147483647)
			return (2147483648);
		i++;
	}
	return (n);
}

/*
** Returns the current time in milliseconds.
*/
long	get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

/*
** Sleeps for `ms` milliseconds in small slices so the philosopher can
** react quickly when the simulation stops (plain usleep would keep a
** dead table waiting).
*/
void	ft_usleep(long ms, t_data *data)
{
	long	end;

	end = get_time_ms() + ms;
	while (get_time_ms() < end)
	{
		if (has_stopped(data))
			return ;
		usleep(200);
	}
}

/*
** Prints a timestamped status message, unless the simulation is over.
** The print mutex keeps messages from different threads from mixing.
*/
void	print_status(t_philo *philo, char *msg)
{
	pthread_mutex_lock(&philo->data->print_lock);
	if (!has_stopped(philo->data))
		printf("%ld %d %s\n", get_time_ms() - philo->data->start_time,
			philo->id, msg);
	pthread_mutex_unlock(&philo->data->print_lock);
}
