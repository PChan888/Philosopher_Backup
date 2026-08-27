# Philosophers — 42 School Project

A classic concurrency exercise: philosophers sit around a table, alternating
between eating, sleeping and thinking. There are as many forks as
philosophers, and each philosopher needs **two** forks to eat. The program
must simulate the dinner with one thread per philosopher, avoid deadlocks
and data races, and detect the exact moment a philosopher starves.

## Build

```bash
make
```

Produces the `philo` executable (compiled with `-Wall -Wextra -Werror -pthread`).

## Usage

```bash
./philo nb_philos time_to_die time_to_eat time_to_sleep [nb_meals]
```

| Argument | Meaning |
|---|---|
| `nb_philos` | Number of philosophers (and forks), 1–200 |
| `time_to_die` | Milliseconds a philosopher can go without eating before dying |
| `time_to_eat` | Milliseconds it takes to eat (holding both forks) |
| `time_to_sleep` | Milliseconds spent sleeping |
| `nb_meals` *(optional)* | Simulation stops once everyone has eaten this many times |

### Examples

```bash
./philo 5 800 200 200      # runs forever, nobody should die
./philo 1 800 200 200      # one philosopher, one fork -> dies at ~800ms
./philo 4 410 200 200      # tight timing, nobody should die
./philo 5 800 200 200 3    # stops after everyone has eaten 3 times
```

## Project structure

| File | Role |
|---|---|
| `philo.h` | Structs (`t_data`, `t_philo`) and prototypes |
| `main.c` | Argument validation, thread creation/joining |
| `init.c` | Data, mutex and philosopher initialisation + cleanup |
| `routine.c` | The philosopher life cycle (forks, eat, sleep, think) |
| `monitor.c` | Death/meal-count watcher running in the main thread |
| `utils.c` | Time helpers, precise sleep, thread-safe printing |

## How it works

- **One thread per philosopher**, plus the main thread acting as a monitor.
- **One mutex per fork.** Even-numbered philosophers pick up their right
  fork first while odd ones pick up their left fork first, so two
  neighbours can never hold one fork each and wait forever (deadlock).
- **Shared state is protected by three mutexes:** `print_lock` (clean
  output), `meal_lock` (last-meal timestamps and meal counts), and
  `stop_lock` (the simulation stop flag).
- **The monitor** polls every philosopher's last-meal time; if one exceeds
  `time_to_die`, it sets the stop flag and prints the death message within
  the required 10ms window.
- **`ft_usleep`** sleeps in 200µs slices and re-checks the stop flag, so
  threads exit promptly when the dinner ends.

## Norm compliance

Written for `norminette` v3: max 25 lines and 5 variables per function,
max 5 functions per file, no forbidden keywords, comments only outside
function bodies.

## AI Assistance

<!-- Fill this section out yourself -->

- **Tools used:**
- **What AI was used for:**
- **What I did myself:**
- **What I understood / learned from it:**
