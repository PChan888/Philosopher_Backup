/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kai <kai@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/06 16:59:46 by kai               #+#    #+#             */
/*   Updated: 2026/07/06 16:59:47 by kai              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
** Thread-safe read of the simulation stop flag.
*/
int	has_stopped(t_data *data)
{
	int	stopped;

	pthread_mutex_lock(&data->stop_lock);
	stopped = data->stopped;
	pthread_mutex_unlock(&data->stop_lock);
	return (stopped);
}

/*
** Thread-safe write of the simulation stop flag.
*/
void	set_stop(t_data *data)
{
	pthread_mutex_lock(&data->stop_lock);
	data->stopped = 1;
	pthread_mutex_unlock(&data->stop_lock);
}

/*
** Returns 1 and announces the death if this philosopher has gone
** longer than time_to_die without starting a meal. The death message
** bypasses print_status because it must print even after the stop.
*/
static int	check_death(t_philo *philo)
{
	long	since_meal;

	pthread_mutex_lock(&philo->data->meal_lock);
	since_meal = get_time_ms() - philo->last_meal;
	pthread_mutex_unlock(&philo->data->meal_lock);
	if (since_meal > philo->data->time_to_die)
	{
		set_stop(philo->data);
		pthread_mutex_lock(&philo->data->print_lock);
		printf("%ld %d died\n", get_time_ms() - philo->data->start_time,
			philo->id);
		pthread_mutex_unlock(&philo->data->print_lock);
		return (1);
	}
	return (0);
}

/*
** Returns 1 when every philosopher has eaten the required number of
** meals (only used when the optional 5th argument is given).
*/
static int	all_ate(t_data *data)
{
	int	i;
	int	done;

	if (data->must_eat < 0)
		return (0);
	done = 1;
	pthread_mutex_lock(&data->meal_lock);
	i = 0;
	while (i < data->nb_philos)
	{
		if (data->philos[i].meals_eaten < data->must_eat)
			done = 0;
		i++;
	}
	pthread_mutex_unlock(&data->meal_lock);
	return (done);
}

/*
** Main-thread loop: watches every philosopher and stops the dinner on
** the first death or once everyone has eaten enough.
*/
void	monitor(t_data *data)
{
	int	i;

	while (1)
	{
		i = 0;
		while (i < data->nb_philos)
		{
			if (check_death(&data->philos[i]))
				return ;
			i++;
		}
		if (all_ate(data))
		{
			set_stop(data);
			return ;
		}
		usleep(500);
	}
}
