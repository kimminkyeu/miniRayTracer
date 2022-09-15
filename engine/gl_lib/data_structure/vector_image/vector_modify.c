/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vector_modify.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minkyeki <minkyeki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/24 16:27:38 by minkyeki          #+#    #+#             */
/*   Updated: 2022/09/12 13:46:25 by minkyeki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "gl_std_vector_image.h"

extern void	ft_bzero(void *s, size_t n);
extern void	*new_image_data_malloc(size_t init_capacity);
extern void	vector_image_set_data(t_vector_image *vec, size_t index, t_image data);

// static void	ft_memcpy(void *dest, const void *src, size_t nbyte)
// {
// 	size_t				i;
// 	unsigned char		*u_dest;
// 	const unsigned char	*u_src;

// 	u_dest = dest;
// 	u_src = src;
// 	i = 0;
// 	while (i < nbyte)
// 	{
// 		u_dest[i] = u_src[i];
// 		++i;
// 	}
// }

void	*vector_image_reserve(t_vector_image *vec, size_t new_capacity)
{
	void	*new_data;

	new_data = NULL;
	if (vec->capacity < new_capacity)
	{
		new_data = new_image_data_malloc(new_capacity);
		if (new_data != NULL)
		{
			ft_memcpy(new_data, vec->data, (vec->size * sizeof(t_image)));
			free(vec->data);
			vec->data = new_data;
			vec->capacity = new_capacity;
		}
	}
	return ((void *)(new_data));
}

void	*vector_image_shrink_to_fit(t_vector_image *vec)
{
	void	*shrinked_data;

	shrinked_data = NULL;
	if (vec->size < vec->capacity)
	{
		shrinked_data = new_image_data_malloc(vec->size);
		if (shrinked_data != NULL)
		{
			ft_memcpy(shrinked_data, vec->data, (vec->size * sizeof(t_image)));
			free(vec->data);
			vec->data = shrinked_data;
			vec->capacity = vec->size;
		}
	}
	return (shrinked_data);
}

void	vector_image_push_back(t_vector_image *vec, t_image data_in)
{
	void	*status;

	if (vec == NULL)
		return ;
	if (vec->size >= vec->capacity)
	{
		status = vector_image_reserve(vec, vec->capacity * 2);
		if (status == NULL)
			return ;
	}
	vector_image_set_data(vec, vec->size, data_in);
	(vec->size)++;
}

void	vector_image_pop_back(t_vector_image *vec)
{
	if (vec->size > 0)
	{
		ft_bzero(vec->get_last(vec), sizeof(t_image));
		(vec->size)--;
	}
}
