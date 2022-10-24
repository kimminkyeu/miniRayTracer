/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gl_device.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minkyeki <minkyeki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/03 16:13:40 by kyeu              #+#    #+#             */
/*   Updated: 2022/10/24 14:56:58 by minkyeki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GL_DEVICE_H
# define GL_DEVICE_H

# include <stddef.h>
# include <stdbool.h>

# include "gl_vec2.h"
# include "gl_vec3.h"
# include "vector.h"
# include "thread.h"

// # include "objects.h"
// # include "lights.h"
// # include "camera.h"

/* TODO: 알맞게 상태 바꿀 것. */
typedef enum e_state {
	E_INPUT_NO_STATE, // while keep pressing
	E_INPUT_IS_PRESSED, // while keep pressing
	E_INPUT_UN_PRESSED,
	// ... add later
}	t_state;

typedef struct s_input {
	/* NOTE: add more key-btn state. */
	union {
		struct {
			int KEY_A_state;
			int KEY_S_state;
			int KEY_D_state;
			int KEY_Q_state;
			int KEY_W_state;
			int KEY_E_state;
			int KEY_R_state;
		};
		int key_state[7];
	};
	union {
		struct {
			int MOUSE_LEFT_BTN_state;
			int MOUSE_MIDDLE_BTN_state;
			int MOUSE_RIGHT_BTN_state;
			int MOUSE_SCROLL_UP_state;
			int MOUSE_SCROLL_DOWN_state;
		};
		int	mouse_state[5];
	};
	t_vec2 last_mouse_pos;
}	t_input;

typedef struct s_device t_device;
typedef struct s_image t_image;

typedef struct s_image {
	void	*mlx_ptr; // for image destroy
	void	*img_ptr;
	char	*addr;
	int		bits_per_pixel;
	int		line_length;
	int		endian;

	t_vec2	img_location;
	t_vec2	img_size;

	t_device	*device_ptr; // pointer to device

	// render function for each image.
	int (*img_update_func)(t_device *device, t_image *image);

}	t_image;

typedef struct s_light t_light;
typedef struct s_ambient_light t_ambient_light;
typedef struct s_camera t_camera;

typedef struct s_device {
	void			*mlx;
	void			*win;
	int				win_width;
	int				win_height;
	float			aspect_ratio;

	// for multi-threading
	t_thread_info	thread_info;


	// t_vector		*images; // image 들의 배열.

	int				resolution_ratio; // 1 is higher.

	t_image			*screen_image; // 최종적으로 넘길 이미지.
	t_image			*pixel_image; // 그림만 그릴 이미지.

	// t_vector		*pixels; // 실제로 그릴 이미지.

	t_input			input; // mouse, keyboard input handler


	// t_vec3			eye_pos; // 나중에 camera로 빼기.

	// t_objects			objects;

	t_vector			*objects;

	t_camera			*camera;

	int	is_high_resolution_render_mode;

	t_ambient_light	 	*ambient_light;
	t_vector			*point_lights;
	// t_light				*light;


}	t_device;

# endif /* DEVICE_H */
