/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kai <kai@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/06 16:59:46 by kai               #+#    #+#             */
/*   Updated: 2026/07/06 16:59:47 by kai              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
** Returns 1 if the string is a non-empty sequence of digits only.
*/
static int	is_number(char *str)
{
	int	i;

	i = 0;
	if (!str[0])
		return (0);
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

/*
** Validates argument count, format and value ranges before starting.
*/
static int	check_args(int argc, char **argv)
{
	int	i;

	if (argc != 5 && argc != 6)
	{
		printf("Usage: ./philo nb_philos t_die t_eat t_sleep [nb_meals]\n");
		return (0);
	}
	i = 1;
	while (i < argc)
	{
		if (!is_number(argv[i]) || ft_atol(argv[i]) > 2147483647)
		{
			printf("Error: invalid argument '%s'\n", argv[i]);
			return (0);
		}
		i++;
	}
	if (ft_atol(argv[1]) < 1 || ft_atol(argv[1]) > 200)
	{
		printf("Error: philosophers must be between 1 and 200\n");
		return (0);
	}
	return (1);
}

/*
** Waits for the first `count` philosopher threads to finish.
*/
static void	join_philos(t_data *data, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		pthread_join(data->philos[i].thread, NULL);
		i++;
	}
}

/*
** Launches one thread per philosopher, runs the monitor loop in the
** main thread, then waits for every philosopher to finish.
*/
static int	start_simulation(t_data *data)
{
	int	i;

	data->start_time = get_time_ms();
	i = 0;
	while (i < data->nb_philos)
	{
		data->philos[i].last_meal = data->start_time;
		if (pthread_create(&data->philos[i].thread, NULL,
				&routine, &data->philos[i]) != 0)
		{
			set_stop(data);
			join_philos(data, i);
			return (0);
		}
		i++;
	}
	monitor(data);
	join_philos(data, data->nb_philos);
	return (1);
}

/*
** Entry point: validates input, sets everything up, runs the dinner
** and cleans up all allocated memory and mutexes.
*/
int	main(int argc, char **argv)
{
	t_data	data;

	if (!check_args(argc, argv))
		return (1);
	if (!init_data(&data, argc, argv))
		return (1);
	if (!start_simulation(&data))
	{
		free_data(&data);
		return (1);
	}
	free_data(&data);
	return (0);
}
