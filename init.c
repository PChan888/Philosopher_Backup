/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kai <kai@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/06 16:59:46 by kai               #+#    #+#             */
/*   Updated: 2026/07/06 16:59:47 by kai              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
** Creates one mutex per fork. On failure, destroys the mutexes that
** were already created so nothing is left half-initialised.
*/
static int	init_forks(t_data *data)
{
	int	i;

	data->forks = malloc(sizeof(pthread_mutex_t) * data->nb_philos);
	if (!data->forks)
		return (0);
	i = 0;
	while (i < data->nb_philos)
	{
		if (pthread_mutex_init(&data->forks[i], NULL) != 0)
		{
			while (--i >= 0)
				pthread_mutex_destroy(&data->forks[i]);
			free(data->forks);
			data->forks = NULL;
			return (0);
		}
		i++;
	}
	return (1);
}

/*
** Gives each philosopher its id, its two forks and a link back to the
** shared data. The last philosopher's right fork wraps around to the
** first fork, closing the circle around the table.
*/
static int	init_philos(t_data *data)
{
	int	i;

	data->philos = malloc(sizeof(t_philo) * data->nb_philos);
	if (!data->philos)
		return (0);
	i = 0;
	while (i < data->nb_philos)
	{
		data->philos[i].id = i + 1;
		data->philos[i].meals_eaten = 0;
		data->philos[i].last_meal = 0;
		data->philos[i].left_fork = &data->forks[i];
		data->philos[i].right_fork = &data->forks[(i + 1) % data->nb_philos];
		data->philos[i].data = data;
		i++;
	}
	return (1);
}

/*
** Parses the program arguments and initialises every mutex and
** philosopher used by the simulation.
*/
int	init_data(t_data *data, int argc, char **argv)
{
	data->nb_philos = (int)ft_atol(argv[1]);
	data->time_to_die = ft_atol(argv[2]);
	data->time_to_eat = ft_atol(argv[3]);
	data->time_to_sleep = ft_atol(argv[4]);
	data->must_eat = -1;
	if (argc == 6)
		data->must_eat = (int)ft_atol(argv[5]);
	data->stopped = 0;
	data->philos = NULL;
	if (!init_forks(data))
		return (0);
	if (pthread_mutex_init(&data->print_lock, NULL) != 0
		|| pthread_mutex_init(&data->meal_lock, NULL) != 0
		|| pthread_mutex_init(&data->stop_lock, NULL) != 0)
		return (0);
	if (!init_philos(data))
	{
		free_data(data);
		return (0);
	}
	return (1);
}

/*
** Destroys every mutex and frees all allocated memory.
*/
void	free_data(t_data *data)
{
	int	i;

	if (data->forks)
	{
		i = 0;
		while (i < data->nb_philos)
		{
			pthread_mutex_destroy(&data->forks[i]);
			i++;
		}
		free(data->forks);
	}
	pthread_mutex_destroy(&data->print_lock);
	pthread_mutex_destroy(&data->meal_lock);
	pthread_mutex_destroy(&data->stop_lock);
	free(data->philos);
}
