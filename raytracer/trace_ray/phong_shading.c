/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   phong_shading.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minkyeki <minkyeki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/30 22:37:25 by minkyeki          #+#    #+#             */
/*   Updated: 2022/10/30 22:38:24 by minkyeki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "trace_ray.h"

/** (1) Calcaultate Ambient texture
 *		: used bilinear interpolation for texture sampling. */
t_vec3	calcuate_ambient(t_device *device, t_hit hit)
{
	t_vec3	ambient_color;
	t_vec3	sample_ambient;

	ambient_color = gl_vec3_multiply_scalar(device->ambient_light->color, device->ambient_light->brightness_ratio);
	if (hit.obj->diffuse_texture != NULL)
	{
		if (hit.obj->diffuse_texture->type == TEXTURE_CHECKER)
			sample_ambient = sample_point(hit.obj->diffuse_texture, hit.uv, false);
		else
			sample_ambient = sample_linear(hit.obj->diffuse_texture, hit.uv, false);
		return (gl_vec3_multiply_vector(ambient_color, sample_ambient));
	}
	else
	{
		ambient_color.r *= (hit.obj->material.diffuse.r / 255.0f);
		ambient_color.g *= (hit.obj->material.diffuse.g / 255.0f);
		ambient_color.b *= (hit.obj->material.diffuse.b / 255.0f);
	}
	return (ambient_color);
}

/** check if hit_point is inside the shadow area. */
bool is_in_shadow(t_device *device, t_hit hit, t_light *light, const t_vec3 hit_point_to_light)
{
	const t_ray		shadow_ray = create_ray(gl_vec3_add_vector(hit.point, gl_vec3_multiply_scalar(hit_point_to_light, 1e-4f)), hit_point_to_light);
	const t_hit		shadow_ray_hit = find_closet_collision(device, &shadow_ray);

	if (shadow_ray_hit.distance < 0.0f \
			|| shadow_ray_hit.obj->material.transparency > 0.0f \
			|| shadow_ray_hit.distance > gl_vec3_get_magnitude(gl_vec3_subtract_vector(light->pos, hit.point)))
	{
		return (false);
	}
	else
		return (true);
}

/** Calculate Diffuse color */
t_vec3	calculate_diffuse(t_hit hit, t_light *light, t_vec3 hit_point_to_light)
{
	const float	_diff = max_float(gl_vec3_dot(hit.normal, hit_point_to_light), 0.0f) * light->brightness_ratio;
	t_vec3		diffuse_final;
	t_vec3		sample_diffuse;
	t_vec3		diffuse;

	diffuse_final = gl_vec3_multiply_scalar(light->color, _diff);
	if (hit.obj->diffuse_texture != NULL)
	{
		if (hit.obj->diffuse_texture->type == TEXTURE_CHECKER)
			sample_diffuse = sample_point(hit.obj->diffuse_texture, hit.uv, false);
		else
			sample_diffuse = sample_linear(hit.obj->diffuse_texture, hit.uv, false);
		return (gl_vec3_multiply_vector(diffuse_final, sample_diffuse));
	}
	else
	{
		diffuse = gl_vec3_multiply_scalar(hit.obj->material.diffuse, _diff);
		diffuse_final.r *= diffuse.r / 255.0f;
		diffuse_final.g *= diffuse.g / 255.0f;
		diffuse_final.b *= diffuse.b / 255.0f;

	}
	return (diffuse_final);
}

/** Calculate Specular [ 2 * (N . L)N - L ] */
t_vec3	calcalate_specular(const t_ray *ray, t_hit hit, t_light *light, t_vec3 hit_point_to_light)
{
	const t_vec3	spec_reflection_dir = gl_vec3_subtract_vector(gl_vec3_multiply_scalar(gl_vec3_multiply_scalar(hit.normal, gl_vec3_dot(hit_point_to_light, hit.normal)), 2.0f), hit_point_to_light);
	const float		_spec = powf(max_float(gl_vec3_dot(gl_vec3_reverse(ray->direction), spec_reflection_dir), 0.0f), hit.obj->material.alpha * light->brightness_ratio);
	const t_vec3	specular_final = gl_vec3_multiply_scalar(gl_vec3_multiply_scalar(light->color, _spec), hit.obj->material.ks);

	return (gl_vec3_multiply_scalar(specular_final, light->brightness_ratio));
}

/** To avoid hit at ray's starting point, move shadow_ray toward forward direction.
 * ----------------------------------------------------------------------------------
 *	(1) [soft-shadow] https://blog.imaginationtech.com/implementing-fast-ray-traced-soft-shadows-in-a-game-engine/
 * */
t_vec3 calculate_phong(t_device *device, const t_ray *ray, t_hit hit, t_light *light)
{
	const t_vec3	hit_point_to_light = gl_vec3_normalize(gl_vec3_subtract_vector(light->pos, hit.point));
	t_vec3			diffuse_color;
	t_vec3			specular_color;
	t_vec3			phong_color;

	phong_color = calcuate_ambient(device, hit);
	if (!is_in_shadow(device, hit, light, hit_point_to_light))
	{
		diffuse_color = calculate_diffuse(hit, light, hit_point_to_light);
		phong_color = gl_vec3_add_vector(diffuse_color, phong_color);
		specular_color = calcalate_specular(ray, hit, light, hit_point_to_light);
		phong_color = gl_vec3_multiply_scalar(phong_color, 1.0f - hit.obj->material.reflection - hit.obj->material.transparency);
		phong_color = gl_vec3_add_vector(phong_color, specular_color);
	}
	return (phong_color);
}