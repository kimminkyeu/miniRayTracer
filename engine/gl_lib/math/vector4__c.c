/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vector4__c.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minkyeki <minkyeki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/26 12:15:56 by minkyeki          #+#    #+#             */
/*   Updated: 2022/10/26 12:17:30 by minkyeki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gl_vec4.h"

t_vec4	normal4(t_vec4 v)
{
	float	len;

	len = len4(v);
	return (vec4_4f(v.x / len, v.y / len, v.z / len, v.w));
}

float	dot4(t_vec4 v1, t_vec4 v2)
{
	return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w);
}

float	len4(t_vec4 v)
{
	return (sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z) + (v.w * v.w)));
}

/** FIX: check if cross-product code is valid!  */
t_vec4	cross4(t_vec4 v1, t_vec4 v2)
{
	float	x;
	float	y;
	float	z;

	x = v1.y * v2.z - v2.y * v1.z;
	y = v1.z * v2.x - v2.z * v1.x;
	z = v1.x * v2.y - v2.x * v1.y;
	return (vec4_4f(x, y, z, 1.0f));
}

/** NOTE : because float 0.0f is not exactly 0, use epsilon. */
t_vec4	gl_vec4_multiply_matrix(t_mat4x4 m, t_vec4 v)
{
	t_vec4	o;
	float	epsilon;

	epsilon = 0.00001f;
	o.x = v.x * m.m[0][0] + v.y * m.m[0][1] + v.z * \
		m.m[0][2] + v.w * m.m[0][3];
	o.y = v.x * m.m[1][0] + v.y * m.m[1][1] + v.z * \
		m.m[1][2] + v.w * m.m[1][3];
	o.z = v.x * m.m[2][0] + v.y * m.m[2][1] + v.z * \
		m.m[2][2] + v.w * m.m[2][3];
	o.w = v.x * m.m[3][0] + v.y * m.m[3][1] + v.z * \
		m.m[3][2] + v.w * m.m[3][3];
	if (!(o.w < epsilon && o.w > -epsilon) \
			&& !(o.w < 1.0f + epsilon && o.w > 1.0f - epsilon))
	{
		o.x /= o.w;
		o.y /= o.w;
		o.z /= o.w;
	}
	return (vec4_4f(o.x, o.y, o.z, 1.0f));
}
