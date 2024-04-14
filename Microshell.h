/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Microshell.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thole <thole@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/14 16:43:24 by thole             #+#    #+#             */
/*   Updated: 2024/04/14 16:43:25 by thole            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MICROSHELL_H
# define MICROSHELL_H

#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

typedef struct s_shell{
    int total_pipes;
    int nb_pipe;
    int last_fd;
    pid_t last_pid;
    char **env;
    int status;
    int exit_code;
    char *cmd_tab[1024];
    int pipe[2];
    int is_first;
} t_shell;

#endif
