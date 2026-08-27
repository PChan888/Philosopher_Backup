/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kai <kai@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/06 16:59:46 by kai               #+#    #+#             */
/*   Updated: 2026/07/06 16:59:47 by kai              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
** Special case for a single philosopher: only one fork exists, so he
** can never eat and simply waits for the monitor to declare him dead.
*/
static void	*lone_philo(t_philo *philo)
{
	pthread_mutex_lock(philo->left_fork);
	print_status(philo, "has taken a fork");
	ft_usleep(philo->data->time_to_die + 10, philo->data);
	pthread_mutex_unlock(philo->left_fork);
	return (NULL);
}

/*
** Locks both forks. Even philosophers grab the right fork first so two
** neighbours never wait on each other forever (deadlock avoidance).
*/
static void	take_forks(t_philo *philo)
{
	pthread_mutex_t	*first;
	pthread_mutex_t	*second;

	first = philo->left_fork;
	second = philo->right_fork;
	if (philo->id % 2 == 0)
	{
		first = philo->right_fork;
		second = philo->left_fork;
	}
	pthread_mutex_lock(first);
	print_status(philo, "has taken a fork");
	pthread_mutex_lock(second);
	print_status(philo, "has taken a fork");
}

/*
** Records the meal time (so the monitor knows this philosopher is not
** starving), eats for time_to_eat ms, then releases both forks.
*/
static void	eat_meal(t_philo *philo)
{
	pthread_mutex_lock(&philo->data->meal_lock);
	philo->last_meal = get_time_ms();
	philo->meals_eaten++;
	pthread_mutex_unlock(&philo->data->meal_lock);
	print_status(philo, "is eating");
	ft_usleep(philo->data->time_to_eat, philo->data);
	pthread_mutex_unlock(philo->left_fork);
	pthread_mutex_unlock(philo->right_fork);
}

/*
** Thinking pause sized to the philosopher's survival margin: someone
** close to starving grabs forks again immediately, while a well-fed
** philosopher waits a bit. This keeps fork access fair and prevents
** slow starvation on odd philosopher counts.
*/
static void	smart_think(t_philo *philo)
{
	long	margin;

	print_status(philo, "is thinking");
	pthread_mutex_lock(&philo->data->meal_lock);
	margin = philo->data->time_to_die
		- (get_time_ms() - philo->last_meal)
		- philo->data->time_to_eat;
	pthread_mutex_unlock(&philo->data->meal_lock);
	if (margin > 0)
		ft_usleep(margin / 2, philo->data);
}

/*
** Life cycle of one philosopher: grab forks, eat, sleep, think, repeat
** until someone dies or everyone has eaten enough. Even philosophers
** start with a small delay so fork access alternates smoothly.
*/
void	*routine(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	if (philo->data->nb_philos == 1)
		return (lone_philo(philo));
	if (philo->id % 2 == 0)
		ft_usleep(philo->data->time_to_eat / 2, philo->data);
	while (!has_stopped(philo->data))
	{
		take_forks(philo);
		eat_meal(philo);
		if (has_stopped(philo->data))
			break ;
		print_status(philo, "is sleeping");
		ft_usleep(philo->data->time_to_sleep, philo->data);
		smart_think(philo);
	}
	return (NULL);
}
