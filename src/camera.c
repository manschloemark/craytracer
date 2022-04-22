#include "camera.h"

// TODO : implement lens size stuff for blur and depth of field simulation
camera make_camera(point3 origin, point3 target, vec3 vup, float vfov, float aspect_ratio, float focal_length) {
	camera cam = {};
	cam.origin = origin;

	float theta = degrees_to_radians(vfov);
	float h = tanf(theta / 2.0);
	float vp_height = 2.0 * h;
	float vp_width = aspect_ratio * vp_height;

	cam.w = vec3_unit(vec3_sub(origin, target));
	// NOTE : when vup and (origin - target) are parallel the cross product is the 0 vector. This is not good.
	//        Since vup is hard-coded to be (0, 0, 1) This is only an issue when the camera is looking straight down.
	//        In that case I'll assume (-1, 0, 0) is a suitable vup.
	if (vec3_near_zero(vec3_cross(vup, cam.w))) {
		vup = vec3_new(-1.0, 0.0, 0.0);
	}
	cam.u = vec3_unit(vec3_cross(vup, cam.w));
	cam.v = vec3_cross(cam.w, cam.u);

	cam.horizontal = vec3_mul(cam.u, focal_length * vp_width);
	cam.vertical = vec3_mul(cam.v, focal_length * vp_height);
	// vp_corner = origin - horiztonal / 2 - vertical / 2 - w * focal_length
	cam.vp_corner = vec3_sub(
										vec3_sub(
											vec3_sub(origin, vec3_div(cam.horizontal, 2.0)),
											vec3_div(cam.vertical, 2.0)
											),
										vec3_mul(cam.w, focal_length)
										);

	return cam;
}
