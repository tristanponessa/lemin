/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fix_overlap.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yforeau <yforeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/19 07:22:56 by yforeau           #+#    #+#             */
/*   Updated: 2019/12/03 07:35:04 by yforeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bfs.h"
#include "solution.h"
#include "lemin_path.h"
#include "lemin_options.h"

void				del_path(t_leminpath **path)
{
	ft_lstdel(&(*path)->nodes, del_path_node);
	ft_memdel((void **)path);
}

static t_leminpath	*cut_overlap_path(t_leminpath *path,
				t_leminpath *old_path, t_lemindata *lda)
{
	t_leminpath	*overlap_path;
	t_list		*ptr;
	int			u;

	overlap_path = ft_memalloc(sizeof(t_leminpath));
	ptr = path->nodes;
	while (ptr)
	{
		u = *(int *)ptr->content;
		if (lda->v[u]->path == old_path)
		{
			ft_lst_push_back(&overlap_path->nodes, &u, sizeof(int));
			++overlap_path->len;
		}
		ptr = ptr->next;
	}
	set_path(old_path, lda, (int[2]){1, 1}, NULL);
	set_path(overlap_path, lda, (int[2]){0, 0}, NULL);
	return (overlap_path);
}

static void			clean_split(t_leminpath *split[2], t_list **solution,
					t_leminpath *overlap, t_lemindata *lda)
{
	if (split[0] && !split[0]->overlap)
	{
		replace_path(solution, split[0], overlap);
		set_path(split[0], lda, (int[2]){1, 1}, NULL);
	}
	if (split[0])
		del_path(&split[0]);
	if (split[1])
		del_path(&split[1]);
	set_path(overlap, lda, (int[2]){0, 0}, overlap);
}

int					fix_overlap(t_lemindata *lda, t_leminpath **path,
					t_list **solution)
{
	t_leminpath	*overlap_path;
	t_leminpath	*split[2];
	int			new_t;

	overlap_path = cut_overlap_path(*path, (*path)->overlap, lda);
	split[1] = NULL;
	new_t = lda->turns;
	if ((split[0] = bfs(lda)) && !split[0]->overlap)
	{
		set_path(split[0], lda, (int[2]){0, 0}, split[0]);
		replace_path(solution, (*path)->overlap, split[0]);
		if ((split[1] = bfs(lda)) && !split[1]->overlap
			&& (new_t = test_solution(*solution, split[1], lda)) < lda->turns)
		{
			set_path(overlap_path, lda, (int[2]){1, 1}, NULL);
			set_path(split[0], lda, (int[2]){0, -1}, split[0]);
			explain_split(lda, *path, overlap_path, split);
			del_path(path);
			*path = split[1];
		}
	}
	if (new_t >= lda->turns)
		clean_split(split, solution, (*path)->overlap, lda);
	del_path(&overlap_path);
	return (new_t);
}
