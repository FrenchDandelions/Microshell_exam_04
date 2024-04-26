/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thole <thole@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 14:54:52 by thole             #+#    #+#             */
/*   Updated: 2024/04/26 14:54:54 by thole            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "microshell.h"

int	ft_strlen(char *s)
{
	int	i;

	i = 0;
	while (s && s[i])
		i++;
	return (i);
}

void	print_err(char *s, char *s2, int mode)
{
	if (s)
		write(2, s, ft_strlen(s));
	if (s2)
		write(2, s2, ft_strlen(s2));
	write(2, "\n", 1);
	if (mode)
		exit(0);
}

void	child(int tmp_fd, char **argv, int i, char **envp)
{
	argv[i] = NULL;
	if (dup2(tmp_fd, STDIN_FILENO) == -1)
		(close(tmp_fd), print_err("error: fatal", NULL, 1));
	close(tmp_fd);
	if (execve(argv[0], argv, envp) == -1)
		print_err("error: cannot execute ", argv[0], 1);
	exit(0);
}

void	cd(char **argv, int i)
{
	argv[i] = NULL;
	if (argv[0] && argv[1] && !argv[2])
	{
		if (chdir(argv[1]))
			print_err("error: cd: cannot change directory to ", argv[1], 0);
	}
	else
		print_err("error: cd: bad arguments", NULL, 0);
}

int	main(int argc, char **argv, char **envp)
{
	int		tmp_fd;
	int		fd[2];
	int		i;
	pid_t	f;

	(void)argc;
	if ((tmp_fd = dup(STDIN_FILENO)) == -1)
		print_err("error: fatal", NULL, 1);
	i = 0;
	f = -1;
	while (argv[i] && argv[i + 1])
	{
		argv = &(argv[i + 1]);
		i = 0;
		while (argv[i] && strcmp(argv[i], "|") && strcmp(argv[i], ";"))
			i++;
		if (strcmp(argv[0], "cd") == 0)
			cd(argv, i);
		else if (!argv[i] || strcmp(argv[i], ";") == 0)
		{
			f = fork();
			if (f == -1)
				(close(tmp_fd), print_err("error: fatal", NULL, 1));
			else if (!f)
				child(tmp_fd, argv, i, envp);
			else
			{
				close(tmp_fd);
				while (waitpid(-1, NULL, WUNTRACED) != -1)
					;
				if ((tmp_fd = dup(STDIN_FILENO)) == -1)
					print_err("error: fatal", NULL, 1);
			}
		}
		else if (argv[i] && strcmp(argv[i], "|") == 0)
		{
			if (pipe(fd) == -1)
				print_err("error: fatal", NULL, 1);
			f = fork();
			if (f == -1)
				print_err("error: fatal", NULL, 1);
			else if (!f)
			{
				if (dup2(fd[1], STDOUT_FILENO) == -1)
					(close(fd[0]), close(fd[1]), print_err("error: fatal", NULL,
							1));
				close(fd[0]);
				close(fd[1]);
				child(tmp_fd, argv, i, envp);
			}
			else
			{
				close(fd[1]);
				close(tmp_fd);
				tmp_fd = fd[0];
			}
		}
	}
	close(tmp_fd);
	return (0);
}
