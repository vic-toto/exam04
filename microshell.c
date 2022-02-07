#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h> 

typedef struct s_node
{
	char **args;
	int		pipe;
	int		fd[2];
	struct s_node *next;
}	t_node;

size_t	ft_strlen(char *str)
{
	size_t i = 0;
	while (str[i])
		i++;
	return (i);
}

void	print(char *str)
{
	write(1, str, ft_strlen(str));
}

void	ft_quit()
{
	print("error: fatal\n");
	exit (0);
}

//char	**ft_split(char *line, char c)
//{
//	int	i = 0;
//	char	**ret;
//	int		j;
//	int		k;
//
//	if (!(ret = malloc(sizeof(char *) * 100)))
//		ft_quit();
//	j = 0;
//	while (line[i])
//	{
//		k = 0;
//		while (line[i] != c || line[i] != 0)
//			ret[j][k++] = line[i++];
//		while (line[i] == c && line[i] != 0)
//			i++;
//		j++;
//	}
//	if (ret[j - 1][0] == '|' || ret[j - 1][0] == ';')
//		ret[j - 1] = 0;
//	else
//		ret[j] = 0;
//	return (ret);
//}

void	lstadd_back(t_node **lst, t_node *new)
{
	t_node *tmp = *lst;
	if (!(tmp))
		*lst = new;
	else
	{
		while (tmp)
			tmp = tmp->next;
		tmp->next = new;
	}
}

void	new_node(char **cmd, int *i, t_node **lst)
{
	int j = 0;
	t_node	*new = malloc(sizeof(t_node));
	if (!(new))
		ft_quit();
	if (!(new->args = malloc(sizeof(char *) * 100)))
		ft_quit();
	new->fd[0] = 0;
	new->fd[1] = 0;
	new->pipe = 0;
	new->next = 0;
	while (cmd[*i] && strcmp(cmd[*i], ";") && (strcmp(cmd[*i], "|")))
	{
		//new->args[j] = malloc(sizeof(cmd[*i]));
		new->args[j++] = cmd[(*i)++];
		//j++;
		//(*i)++;
	}
	new->args[j] = 0;
	if (cmd[*i] && (!(strcmp(cmd[*i], "|"))))
		new->pipe = 1;
	//lstadd_back(lst, new);
	t_node *tmp = *lst;
	if (!(tmp))
		*lst = new;
	else
	{
		while (tmp)
			tmp = tmp->next;
		tmp->next = new;
	}
}

int		cd(char **cmd)
{
	if (cmd[1] == 0 || cmd[2] != 0)
	{
		print("error: cd: bad arguments\n");
		return (1);
	}
	if (chdir(cmd[1]))
	{
		print("error: cd: cannot change directory to ");
		print(cmd[1]);
		print("\n");
		return (1);
	}
	return (0);
}


int	exec(t_node *p, char **env)
{
	pid_t pid;

	if (!(p) || !(p->args[0]))
		return (1);
	if (!(strncmp(p->args[0], "cd", 2)))
		return (cd(p->args));
	if ((pid = fork()) == -1)
		ft_quit();
	if (pid == 0)
	{
		if (p->fd[1])
			dup2(p->fd[1], 1);
		if (p->fd[0])
			dup2(p->fd[0], 0);
		execve(p->args[0], p->args, env);
		print("error: cannot execute ");
		print(p->args[0]);
		print("\n");
		exit (0);
	}
	waitpid(pid, 0, 0);
	if (p->fd[0])
		close(p->fd[0]);
	if (p->fd[1])
		close(p->fd[1]);
	return (0);
}

void	do_shit(t_node *lst, char **env)
{
	t_node	*tmp = lst;
	int		tmp_fd[2];

	while (tmp)
	{
		if (tmp == NULL)
			return ;
		if (tmp->pipe != 0)
		{
			if (pipe(tmp_fd))
				ft_quit();
			tmp->fd[1] = tmp_fd[1];
			tmp->next->fd[0] = tmp_fd[0];
		}
		if (exec(tmp, env) != 0)
			return ;
		tmp = tmp->next;
	}
}

void	free_matrix(char **matrix)
{
	int	i = 0;
	if (!(matrix) || !(matrix[i]))
	while (matrix[i] != 0)
		free(matrix[i++]);
	free(matrix);
}

void	free_shit(t_node *p)
{
	t_node *tmp;

	tmp = p;
	while (p)
	{
		tmp = p;
		tmp->next = NULL;
		p = p->next;
		free_matrix(p->args);
		free(tmp);
	}
}

int    main(int argc, char **argv, char **env)
{
	int i = 0;
	t_node *lst = 0;
	while (++i < argc)
	{
		if (!(strcmp(argv[i], ";")))	
			continue;
		new_node(argv, &i, &lst);
	}
	do_shit(lst, env);
	free_shit(lst);
	return (0);
}
