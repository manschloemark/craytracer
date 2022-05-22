#ifndef CAMERA_H
#define CAMERA_H

#include "common.h"
#include "ray.h"

// TODO : implement lens size stuff for blur and depth of field simulation
typedef struct {
	point3 origin, vp_corner;
	vec3 horizontal, vertical;
	vec3 u, v, w;
} camera;

camera make_camera(point3 origin, point3 target, vec3 vup, float vfov, float aspect_ratio, float focal_length);

static inline ray camera_cast_ray(camera *cam, float u, float v, thread_context *thread) {
	//dir = vp_corner + (horizontal * %across) + (vertical + %across) - origin
	vec3 dir = vec3_sub(vec3_add(vec3_add(cam->vp_corner, vec3_mul(cam->horizontal, u)), vec3_mul(cam->vertical, v)), cam->origin);
	ray r = ray_new(cam->origin, dir);
	return r;
}
#endif
