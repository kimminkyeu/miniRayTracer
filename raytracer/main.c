/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minkyeki <minkyeki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/25 13:54:43 by minkyeki          #+#    #+#             */
/*   Updated: 2022/10/24 09:05:59 by minkyeki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <math.h>
#include "camera.h"
#include "include/main.h"
#include "gl_vec3.h"
#include "helper.h"
#include "libft.h"
#include "thread.h"
#include "parser.h"


// *  NOTE:  https://www.youtube.com/watch?v=lXlXqUEEJ94
// * Adding an Interactive 3D Camera System // Ray Tracing series
int input_handler(t_device *device)
{
	t_camera *camera = device->camera;

	t_vec2 mouse_pos = input_get_mouse_pos(device);
	t_vec2 delta = gl_vec2_multiply_scalar((gl_vec2_subtract_vector(mouse_pos, device->input.last_mouse_pos)), 0.1f);
	device->input.last_mouse_pos = mouse_pos;


	if (device->is_high_resolution_render_mode == false && input_is_key_down(device, KEY_R))
	{
		printf("[change to High Resolution mode]\n");
		device->is_high_resolution_render_mode = true;
		update(device);
		return (0);
	}

	if (!(input_is_mouse_down(device, MOUSE_RIGHT_CLICK)))
	{
		return (0);
	}
	// below code will be executed only if mouse is pressed
	bool moved = false;
	float speed = 0.2f;

	// Movement
	if (input_is_key_down(device, KEY_W))
	{
		camera->pos = gl_vec3_add_vector(camera->pos, gl_vec3_multiply_scalar(camera->look_at, speed));
		moved = true;
	}
	else if (input_is_key_down(device, KEY_S))
	{
		camera->pos = gl_vec3_subtract_vector(camera->pos, gl_vec3_multiply_scalar(camera->look_at, speed));
		moved = true;
	}
	if (input_is_key_down(device, KEY_A))
	{
		camera->pos = gl_vec3_subtract_vector(camera->pos, gl_vec3_multiply_scalar(camera->right_direction, speed));
		moved = true;
	}
	else if (input_is_key_down(device, KEY_D))
	{
		camera->pos = gl_vec3_add_vector(camera->pos, gl_vec3_multiply_scalar(camera->right_direction, speed));
		moved = true;
	}
	if (input_is_key_down(device, KEY_Q))
	{
		camera->pos = gl_vec3_subtract_vector(camera->pos, gl_vec3_multiply_scalar(camera->up_direction, speed));
		moved = true;
	}
	else if (input_is_key_down(device, KEY_E))
	{
		camera->pos = gl_vec3_add_vector(camera->pos, gl_vec3_multiply_scalar(camera->up_direction, speed));
		moved = true;
	}

	if (delta.x != 0.0f || delta.y != 0.0f)
	{
		t_vec4 cam_orientation = gl_vec4_4f(camera->look_at.x, camera->look_at.y, camera->look_at.z, 0.0f);
		t_vec4 cam_up = gl_vec4_4f(camera->up_direction.x, camera->up_direction.y, camera->up_direction.z, 0.0f);
		cam_orientation = gl_vec4_multiply_matrix(gl_mat4x4_rotate_x_axis(-delta.y),cam_orientation);
		cam_orientation = gl_vec4_multiply_matrix(gl_mat4x4_rotate_y_axis(delta.x),cam_orientation);
		cam_up = gl_vec4_multiply_matrix(gl_mat4x4_rotate_x_axis(-delta.y),cam_up);
		cam_up = gl_vec4_multiply_matrix(gl_mat4x4_rotate_y_axis(delta.x),cam_up);
		camera->look_at.x = cam_orientation.x;
		camera->look_at.y = cam_orientation.y;
		camera->look_at.z = cam_orientation.z;
		camera->up_direction.x = cam_up.x;
		camera->up_direction.y = cam_up.y;
		camera->up_direction.z = cam_up.z;
		// printf("camera Look-At  (%f | %f | %f)\t", camera->look_at.x, camera->look_at.y, camera->look_at.z);
		// printf("Up (%f | %f | %f)\n", camera->up_direction.x, camera->up_direction.y, camera->up_direction.z);
		moved = true;
	}


	if(moved)
	{
		device->is_high_resolution_render_mode = false;

		// if camera has been moved, then recalculate camera datas
		// TODO:  아래 공식을 parse_camera에서 한번 해줘야 함. (recalculate_camera() 함수로 빼기)
		update_camera_geometry(device);
		// modify the U and V vectors to match the size and aspect ratio.
		update(device);
	}
	return (0);
}


int	main(int ac, char **av)
{
	t_device	*device;

	if (ac != 2)
	{
		// TODO:  ... err message
		return (EXIT_FAILURE);
	}
	const int WIDTH = 1600;
	const int HEIGHT = 800;
	const int LOW_RESOLUTION_RATIO = 20; // max is 1
	const int THREAD_NUM = 24;

	/** (1) Init engine && create image */
	device = engine_init(WIDTH, HEIGHT, "42 Mini-RayTracing", LOW_RESOLUTION_RATIO);

	/** (2) Load files. (Map data etc...) then store data to [t_device] structure */
	// parse_rt_file_to_device(device, av[1]);
	parse_rt_file_to_device(device, av[1]);

	/** (3) start rendering via threads */
	t_thread_info *info = &device->thread_info;
	info->thread_group = ft_calloc(1, sizeof(t_thread) * THREAD_NUM);
	info->thread_num = THREAD_NUM;
	info->finished_thread_num = 0;
	if (pthread_mutex_init(&(info->finished_num_mutex), NULL) != 0)
		print_error_and_exit(device, "error in pthread_mutex_init()\n");

	/** (4) update image; */
	update(device);

	/** (4) loop mlx */
	mlx_loop_hook(device->mlx, input_handler, device);


	mlx_loop(device->mlx);
	return (0);
}
