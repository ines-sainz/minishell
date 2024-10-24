/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danjimen & isainz-r <danjimen & isainz-    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/12 14:15:16 by danjimen          #+#    #+#             */
/*   Updated: 2024/10/24 13:56:01 by danjimen &       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

int	ft_tokenize(t_args *args, t_mini *mini)
{
	args->arg = malloc(ft_strlen(args->input) + 1);
	args->arg_ptr = args->arg;
	args->argc = 0;
	args->in_single_quote = t_false;
	args->in_double_quote = t_false;
	if (add_to_args(args, &args->argc, mini) == ERR)
	{
		free (args->arg);
		args->arg = NULL;
		return (ERR);
	}
	free(args->arg);
	args->arg = NULL;
	return (OK);
}

int	check_delimiter(t_args *args)
{
	t_params	*current;
	t_params	*next;

	current = args->params;
	if (args->params != NULL)
	{
		while (current != NULL)
		{
			next = current->next;
			if (current->type == DELIMITER && current->quotes == t_false
				&& (current->content[0] == '<' || current->content[0] == '>'
					|| current->content[0] == ';' || current->content[0] == '&'
					|| current->content[0] == '|'))
			{
				ft_dprintf(2, "minishell: syntax error near unexpected"
					" token `%c'\n", current->content[0]);
				ft_export_env("?=2", args->mini);
				return (ERR);
			}
			current = next;
		}
	}
	return (OK);
}

static void	loop_args_to_create_list(t_args *args)
{
	int			i;
	t_bool		heredoc_found;

	i = 0;
	heredoc_found = t_false;
	while (i < args->argc)
	{
		if (args->args[i])
		{
			add_argument_to_list(args, &i, &heredoc_found);
			free(args->args[i]);
			args->args[i] = NULL;
			args->quotes[i] = t_false;
		}
		i++;
	}
}

int	parse(t_args *args, t_mini *mini)
{
	int	args_len;

	if (ft_tokenize(args, mini) == ERR)
		return (ERR);
	loop_args_to_create_list(args);
	args_len = ft_retokenize(args->params);
	if ((args_len == 0 && args->params == NULL) || args_len == -1)
	{
		ft_export_env("?=0", mini);
		if (args_len == -1)
			ft_export_env("?=2", mini);
		return (del_params(args), ERR);
	}
	if (check_delimiter(args) == ERR)
		return (del_params(args), ERR);
	update_last_command_env_var(args);
	new_red_exe(args, mini);
	close_inf_outf(mini);
	exe_struct_free(mini);
	mini->n_here_docs = 0;
	mini->n_commands = 0;
	free_last_env(mini);
	return (OK);
}
