/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   here_doc.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danjimen <danjimen@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/05 12:25:19 by isainz-r          #+#    #+#             */
/*   Updated: 2024/10/23 21:32:38 by danjimen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

// char	*ft_get_eof(char *eof)
// {
// 	char	*eof_fin;

// 	eof_fin = (char *)malloc(ft_strlen(eof) + 2);
// 	ft_strlcpy(eof_fin, eof, ft_strlen(eof) + 1);
// 	eof_fin[ft_strlen(eof)] = '\n';
// 	eof_fin[ft_strlen(eof) + 1] = '\0';
// 	return (eof_fin);
// }

// int	ft_write_temp(int fd, t_params *param, t_mini *mini)
// {
// 	t_args	here_doc;
// 	char	*eof;
// 	char	*eof_fin;

// 	eof = param->content;
// 	ft_bzero(&here_doc, sizeof(t_args));
// 	here_doc.mini = mini;
// 	signal(SIGINT, signal_here_doc);
// 	g_signal_received = 0;
// 	while (1)
// 	{
// 		if (g_signal_received == SIGINT)
// 			break ;
// 		here_doc.arg = readline("> ");
// 		if (!here_doc.arg)
// 		{
// 			ft_dprintf(2, "minishell: warning: here-document delimited by end-of-file (wanted `EOF')\n");
// 			break ;
// 		}
// 		if (g_signal_received == SIGINT)
// 		{
// 			free(here_doc.arg);
// 			break ;
// 		}
// 		if (eof[0] == '$' && ft_strlen(eof) >= 1 && param->quotes == t_true)
// 			eof_fin = ft_substr(eof, 1, ft_strlen(eof));
// 		else
// 			eof_fin = ft_get_eof(eof);
// 		if ((eof[0] == '\0' && here_doc.arg[0] == '\0')
// 			|| (!ft_strncmp(eof_fin, here_doc.arg, ft_strlen(here_doc.arg))
// 			&& ft_strlen(here_doc.arg) > 0) || (eof[0] == '$'
// 			&& ft_strlen(eof) == 1 && param->quotes == t_true
// 			&& here_doc.arg[0] == '\0'))
// 		{
// 			free(here_doc.arg);
// 			free(eof_fin);
// 			break ;
// 		}
// 		if (param->quotes == t_false)
// 		{
// 			expander(&here_doc, mini);
// 			printf("DB: expanded_arg => %s\n", here_doc.result);
// 			write(fd, here_doc.result, ft_strlen(here_doc.result));
// 			write(fd, "\n", 1);
// 			free(here_doc.arg);
// 			free(here_doc.result);
// 			free(eof_fin);
// 		}
// 		else
// 		{
// 			write(fd, here_doc.arg, ft_strlen(here_doc.arg));
// 			write(fd, "\n", 1);
// 			free(here_doc.arg);
// 			free(eof_fin);
// 		}
// 	}
// 	signal(SIGINT, signal_sigint);
// 	if (g_signal_received == SIGINT)
// 	{
// 		g_signal_received = 0;
// 		close(fd);
// 		return (-2);
// 	}
// 	return (OK);
// }

static char	*get_here_doc_file_name(void)
{
	int		i;
	char	*file_num;
	char	*file_name;

	i = 0;
	file_num = ft_itoa(i);
	file_name = ft_strjoin(file_num, ".tmp");
	free(file_num);
	while (access(file_name, F_OK) == 0)
	{
		i++;
		free(file_name);
		file_num = ft_itoa(i);
		file_name = ft_strjoin(file_num, ".tmp");
		free(file_num);
	}
	return (file_name);
}

static int	ft_here_doc(t_params *param, t_mini *mini, int fd)
{
	t_list	*temp_here_doc;
	char	*file_name;

	file_name = get_here_doc_file_name();
	fd = open(file_name, O_RDWR | O_CREAT | O_TRUNC, 0777);
	if (fd < 0)
		return (free(file_name), ERR);
	temp_here_doc = ft_lstnew(file_name);
	if (temp_here_doc == NULL)
	{
		unlink(file_name);
		ft_lstclear(&mini->here_doc_files, free);
		close(fd);
		return (ERR);
	}
	ft_lstadd_back(&mini->here_doc_files, temp_here_doc);
	if (here_doc_loop(fd, param, mini) == -2)
		return (-2);
	close (fd);
	fd = open(file_name, O_RDONLY, 0777);
	return (fd);
}

int	*get_here_doc(t_params *iter_params, t_args *args, int i)
{
	int	*here_doc_fds;

	here_doc_fds = NULL;
	while (iter_params != NULL)
	{
		if (iter_params->type == HERE_DOC)
			i++;
		iter_params = iter_params->next;
	}
	if (i > 0)
		here_doc_fds = malloc(i * sizeof(int *));
	i = 0;
	iter_params = args->params;
	while (iter_params != NULL)
	{
		if (iter_params->type == HERE_DOC)
		{
			here_doc_fds[i] = ft_here_doc(iter_params->next, args->mini, 0);
			if (here_doc_fds[i] == -2)
				return (here_doc_fds);
			i++;
		}
		iter_params = iter_params->next;
	}
	return (here_doc_fds);
}