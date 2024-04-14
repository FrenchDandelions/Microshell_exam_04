/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thole <thole@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/14 16:43:18 by thole             #+#    #+#             */
/*   Updated: 2024/04/14 16:43:19 by thole            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Microshell.h"

int	ft_strlen(char *str)
{
	int	i;

	i = 0;
	while (str && str[i])
		i++;
	return (i);
}

void	_err(char *str, int mode, int err)
{
	write(2, str, ft_strlen(str));
	if (mode)
		exit(err);
}

void	count_pipes(t_shell *shell, char **argv, int i)
{
	shell->total_pipes = 0;
	shell->nb_pipe = 0;
	while (argv[i])
	{
		if (argv[i + 1] && strcmp(argv[i + 1], "|") == 0)
			shell->total_pipes++;
		if (argv[i + 1] && strcmp(argv[i + 1], ";") == 0)
			break ;
		i++;
	}
}

void	ft_wait(t_shell *shell)
{
	while (errno != ECHILD)
	{
		if (wait(&shell->status) == shell->last_pid)
		{
			if (WIFEXITED(shell->status))
				shell->exit_code = WEXITSTATUS(shell->status);
			else
				shell->exit_code = 128 + WTERMSIG(shell->status);
		}
	}
}

void	cd(char *path, t_shell *shell)
{
	if (!shell->cmd_tab[1] || shell->cmd_tab[2])
	{
		if (shell->nb_pipe)
			_err("error: cd: bad arguments\n", 1, 2);
		else
		{
			_err("error: cd: bad arguments\n", 0, 2);
			shell->exit_code = 2;
		}
	}
	if (chdir(path))
	{
		if (shell->nb_pipe)
		{
			_err("error: cd: cannot change directory to ", 0, 2);
			_err(path, 0, 2);
			_err("\n", 1, 2);
		}
		else
		{
			_err("error: cd: cannot change directory to ", 0, 2);
			_err(path, 0, 2);
			_err("\n", 0, 2);
			shell->exit_code = 2;
		}
	}
}

void	exec(char **cmd_tab, t_shell *shell)
{
	if (shell->nb_pipe != shell->total_pipes)
		if (pipe(shell->pipe))
			_err("error: fatal\n", 1, 2);
	shell->last_pid = fork();
	if (shell->last_pid == -1)
	{
		if (shell->total_pipes)
		{
			if (shell->nb_pipe != shell->total_pipes)
			{
				close(shell->pipe[0]);
				close(shell->pipe[1]);
			}
			close(shell->last_fd);
		}
		_err("error: fatal\n", 1, 2);
	}
	else if (!shell->last_pid)
	{
		if (!shell->is_first && shell->nb_pipe)
		{
			if (dup2(shell->last_fd, STDIN_FILENO) == -1)
				(close(shell->last_fd), _err("error: fatal\n", 1, 2));
			close(shell->last_fd);
		}
		if (shell->nb_pipe != shell->total_pipes && shell->total_pipes)
		{
			if (dup2(shell->pipe[1], STDOUT_FILENO) == -1)
				(close(shell->pipe[1]), close(shell->pipe[0]),
					_err("error: fatal\n", 1, 2));
			close(shell->pipe[1]);
			close(shell->pipe[0]);
		}
		if (strcmp("cd", shell->cmd_tab[0]) == 0)
			cd(shell->cmd_tab[1], shell);
		else if (execve(cmd_tab[0], cmd_tab, shell->env) == -1)
		{
			_err("error : cannot execute ", 0, 2);
			_err(cmd_tab[0], 0, 2);
			_err("\n", 1, 127);
		}
		exit(0);
	}
	else
	{
		if (!shell->is_first)
			close(shell->last_fd);
		shell->last_fd = shell->pipe[0];
		if (shell->nb_pipe != shell->total_pipes)
			close(shell->pipe[1]);
	}
}

void	load_cmds(t_shell *shell, int j, char *cmd)
{
	shell->cmd_tab[j] = cmd;
	shell->cmd_tab[j + 1] = NULL;
}

int	main(int argc, char **argv, char **env)
{
	t_shell	shell;
	int		i;
	int		j;

	shell.env = env;
	shell.status = 0;
	shell.exit_code = 0;
	i = 1;
	while (argv[i] != NULL)
	{
		count_pipes(&shell, argv, i);
		j = 0;
		shell.cmd_tab[0] = NULL;
		shell.is_first = 1;
		while (argv[i] != NULL && strcmp(argv[i], ";"))
		{
			if (strcmp(argv[i], "|") == 0)
			{
				i++;
				exec(shell.cmd_tab, &shell);
				shell.is_first = 0;
				shell.nb_pipe++;
				for (int i = 0; i <= j; i++)
				{
					shell.cmd_tab[i] = NULL;
				}
				j = 0;
			}
			load_cmds(&shell, j, argv[i]);
			j++;
			i++;
		}
		if (strcmp("cd", shell.cmd_tab[0]) == 0)
			cd(shell.cmd_tab[1], &shell);
		else
			exec(shell.cmd_tab, &shell);
		ft_wait(&shell);
		if (argv[i])
			i++;
	}
	return (shell.exit_code);
}
