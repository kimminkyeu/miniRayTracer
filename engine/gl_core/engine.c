/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   engine.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minkyeki <minkyeki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/26 18:16:30 by minkyeki          #+#    #+#             */
/*   Updated: 2022/09/13 17:45:24 by minkyeki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gl_engine.h"

extern void		engine_set_key_event(t_device *device, int (*f_key_press)(), int (*f_key_release)());
extern void		engine_set_mouse_event(t_device *device, int (*f_mouse_press)(), int (*f_mouse_release)());
extern int		handle_key_press(int key_code, void *param);
extern int		handle_key_release(int key_code, void *param);
extern int		handle_mouse_press(int key_code, int x, int y, void *param);
extern int		handle_mouse_release(int key_code, int x, int y, void *param);

void	engine_push_image(t_device *device, t_image *image, int x, int y)
{
	if (x <= device->win_width && x >= 0 && y <= device->win_height && y >= 0)
		if (device != NULL && image != NULL && image->img_ptr != NULL)
			mlx_put_image_to_window(device->mlx, device->win, image->img_ptr, x, y);
}

void	engine_exit(t_device *device, bool is_error)
{

	if (device != NULL && device->images != NULL)
	{
		size_t i = 0;
		while (i < device->images->size)
		{
			printf("destroying image %zd\n", i);
			mlx_destroy_image(device->mlx, ((t_image *)device->images->data[i])->img_ptr);
			i++;
		}
		delete_vector(&device->images);
	}
	if (device->win != NULL)
		mlx_destroy_window(device->mlx, device->win);

	if (device != NULL && device->mlx != NULL)
		free(device->mlx);

	if (device != NULL)
		free(device);

	if (is_error == ERROR)
		exit(EXIT_FAILURE);
	else
		exit(EXIT_SUCCESS);
}



void	engine_new_image(t_device *device, t_vec2 img_size, t_vec2 img_location, int (*f_update_func)())
{
	t_vector *images = device->images;
	t_image	*new_image;

	new_image = ft_calloc(1, sizeof(*new_image));

	if (img_size.width <= 0 || img_size.height <= 0)
		return ;
	if (device->mlx != NULL)
	{
		new_image->img_ptr = mlx_new_image(device->mlx, img_size.width, img_size.height);
		new_image->addr = mlx_get_data_addr(new_image->img_ptr, \
				&(new_image->bits_per_pixel), \
				&(new_image->line_length), &(new_image->endian));
		new_image->img_size = img_size;
		new_image->img_location = img_location;
		new_image->img_update_func = f_update_func;
	}
	images->push_back(images, new_image);
	printf("New image created\n");
}

int		handle_exit(t_device *device)
{
	ft_putstr_fd("Closing Program....\n", STDOUT);
	engine_exit(device, SUCCESS);
	return (0);
}
t_device	*engine_init(int _win_width, int _win_height, char *title)
{
	t_device	*device;

	device = ft_calloc(1, sizeof(*device));
	if (device == NULL)
		engine_exit(device, ERROR);
	device->mlx = mlx_init();
	if (device->mlx == NULL)
		engine_exit(device, ERROR);

	device->win_width = _win_width;
	device->win_height = _win_height;

	device->win = mlx_new_window(device->mlx, device->win_width, device->win_height, title);
	if (device->win == NULL)
		engine_exit(device, ERROR);

	/** NOTE: New image vector (Array of images) */
	device->images = new_vector(5);

	/** initialize device input data */
	mlx_hook(device->win, ON_DESTROY, 0, handle_exit, device);
	engine_set_key_event(device, handle_key_press, handle_key_release);
	engine_set_mouse_event(device, handle_mouse_press, handle_mouse_release);
	printf("Event handler set done\n");

	return (device);
}

// void	engine_register_render_func(t_device *device, int (*render_func)())
// {
	// device->engine_render_func = render_func;
// }
static void draw_render_time(t_device *device, long long time, t_vec2 location, int argb)
{
	char	*str;

	mlx_string_put(device->mlx, device->win, location.x, location.y, argb, "Last render(ms)");
	mlx_string_put(device->mlx, device->win, location.x, location.y + 20, argb, ":");
	str = ft_itoa(time);
	mlx_string_put(device->mlx, device->win, location.x + 12, location.y + 20, argb, str);
	free(str);
}

/** FIXME: Need function testing! (한번만 렌더하는 함수) */
int	engine_update_images(t_device *device)
{
	t_image	*img_ptr;
	size_t i;
	long long render_start_time;
	long long render_end_time;

	i = 0;
	render_start_time = get_time_ms();
	while (i < device->images->size)
	{
		img_ptr = device->images->data[i];
		img_ptr->img_update_func(device, img_ptr);
		engine_push_image(device, img_ptr, img_ptr->img_location.x, img_ptr->img_location.y);
		i++;
	}
	render_end_time = get_time_ms();
	draw_render_time(device, render_end_time - render_start_time, gl_vec2(30, 30), WHITE);
	return (0);
}

/* TODO: change it's name to engine_render_loop() */
void	engine_render(t_device *device)
{
	// engine_update_images(device);
	mlx_loop_hook(device->mlx, engine_update_images, device);
	mlx_loop(device->mlx);
}

/** NOTE: if handler changes, reset engine */
void		engine_set_key_event(t_device *device, int (*f_key_press)(), int (*f_key_release)())
{
	if (device == NULL)
		return ;
	if (f_key_press != NULL)
		mlx_hook(device->win, ON_KEY_PRESS, KeyPressMask, f_key_press, device);
	if (f_key_release != NULL)
		mlx_hook(device->win, ON_KEY_RELEASE, KeyReleaseMask, f_key_release, device);
}

void		engine_set_mouse_event(t_device *device, int (*f_mouse_press)(), int (*f_mouse_release)())
{
	if (device == NULL)
		return ;
	if (f_mouse_press != NULL)
		mlx_hook(device->win, ON_MOUSE_PRESS, ButtonPressMask, f_mouse_press, device);
	if (f_mouse_release != NULL)
		mlx_hook(device->win, ON_MOUSE_RELEASE, ButtonReleaseMask, f_mouse_release, device);
}