#include "objects.h"

// This determines which IntersectFBMSphere function to use.
// the "Raymarch" one is the new experimental one that:
// 1. uses the sphere's normal to a given point as input the fbm_with_derivative. This ensures the offset looks the same from all p.o.vs
// 2. attemps to implement a ray marching algorithm because you can't do a simple intersection equation
// 3. It's really slow and has a lot of holes. It looks really neat though.
#define USE_RAYMARCH_FBM 1

// Ensure the normal is pointing in the opposite direction of the incident ray
// Return 0 if you had to negate the normal vector - that means the intersection was 'inside' the object
// Return 1 otherwise
// NOTE : this function serves two purposes and that is not very clear, but it kills two birds with one stone
int outward_normal(vec3 incident, vec3 *normal) {
	if(vec3_dot(incident, *normal) > 0.0f) {
		*normal = vec3_neg(*normal);
		return 0;
	}
	return 1;
}

sphere sphere_new(vec3 center, float radius) {
	sphere s = {};
	s.center = center;
	s.radius = radius;
	return s;
}

triangle triangle_new(point3 x, point3 y, point3 z, int double_sided) {
	triangle t = {};
	t.a = x;
	t.b = y;
	t.c = z;
	t.double_sided = double_sided;
	return t;
}

// really hastily made just to get it out there. this will not stay long
int about_equal(vec3 a, vec3 b) {
	const float eps = 0.000001f;
	return !(vec3_len(vec3_sub(a, b)) > eps);
}

// NOTE : this is full of bugs, it's ugly, and poorly written
// But I plan to implement meshes which would make this pretty much redundant, I think.
quad quad_new(memory_region *region, point3 a, point3 b, point3 c, point3 d, int double_sided, texture *text, material *mat) {
	quad q = {};
	vec3 a_b = vec3_sub(b, a);
	vec3 a_c = vec3_sub(c, a);
	vec3 a_d = vec3_sub(d, a);

	vec3 cross_abc = vec3_unit(vec3_cross(a_b, a_c));
	vec3 cross_abd = vec3_unit(vec3_cross(a_b, a_d));

	// Check if all points lie in the same plane
	if(!about_equal((cross_abc), (cross_abd))) {
		if (!about_equal(cross_abc, vec3_abs(cross_abd))) {
					q.tri_a = NULL;
					q.tri_b = NULL;
					return q;
		}
	}

	point3 *tri_b_v1 = NULL;
	point3 *tri_b_v2 = NULL;
	point3 *tri_b_v3 = NULL;

	float ang_ad_ab = vec3_dot(cross_abc, vec3_cross(a_d, a_b)) / (vec3_len(a_b) * vec3_len(a_d));
	float ang_ad_ac = vec3_dot(cross_abc, vec3_cross(a_d, a_c)) / (vec3_len(a_c) * vec3_len(a_d));;
	if (signbit(ang_ad_ab) != signbit(ang_ad_ac)) {
		tri_b_v1 = &c;
		tri_b_v2 = &b;
		tri_b_v3 = &d;
	} else {
		if (fabs(ang_ad_ab) > fabs(ang_ad_ac)) {
			tri_b_v1 = &d;
			tri_b_v2 = &a;
			tri_b_v3 = &c;
		} else {
			tri_b_v1 = &d;
			tri_b_v2 = &a;
			tri_b_v3 = &b;
		}
	}
	object *tri_a = add_triangle(region, a, b, c, text, mat);
	object *tri_b = add_triangle(region, *tri_b_v1, *tri_b_v2, *tri_b_v3, text, mat);

	vec3 norm_b = TriangleNormal(tri_a, vec3_new(0.0, 0.0, 0.0));
	if (!about_equal(cross_abc, norm_b)) {
		vec3 temp = tri_b->shape.triangle.a;
		tri_b->shape.triangle.a = tri_b->shape.triangle.b;
		tri_b->shape.triangle.b = temp;
	}

	tri_a->shape.triangle.double_sided = double_sided;
	tri_b->shape.triangle.double_sided = double_sided;

	q.tri_a = tri_a;
	q.tri_b = tri_b;
	return q;
}

fbm_shape fbm_shape_new(memory_region *region, noise *noise, float scale, float offset_scale, float hurst, int octaves, void *obj) {
	fbm_shape fs;
	// NOTE : no longer accept uninitialized noise objects
	//fs.noise = (noise == NULL) ? add_perlin_noise(region, 32) : noise;
	fs.noise = noise;
	fs.obj = obj;
	fs.offset_scale = offset_scale;
	fs.scale = scale;
	fs.hurst = hurst;
	fs.octaves = octaves;
	return fs;
}


vec3 SphereNormal(void *self, vec3 relative) {
	return vec3_unit(vec3_sub(relative, ((object *)self)->shape.sphere.center));
}

// TODO : I only make s a pointer here because I don't feel like fixing all the ->, fix it later (also in IntersectTriangle)
int IntersectSphere(void *self, ray *r, hit_record *hitrec, thread_context *thread) {
	object *obj = self;
	sphere *s = &((object *)self)->shape.sphere;

	vec3 T = vec3_sub(r->pt, s->center);
	float a = vec3_lensq(r->dir);
	float half_b = vec3_dot(T, r->dir);
	float c = vec3_lensq(T) - s->radius*s->radius; 

	float discriminant = half_b*half_b - a*c;

	if (discriminant < 0.0f) {
		return 0;
	}

	float root = (-half_b - sqrt(discriminant)) / a;
	if (root < hitrec->t_min || root > hitrec->t) {
		root = (- half_b + sqrt(discriminant)) / a;
		if (root < hitrec->t_min || root > hitrec->t) return 0;
	}

	hitrec->t = root;
	hitrec->pt = pt_on_ray(r, root);
	vec3 n = vec3_div(vec3_sub(hitrec->pt, s->center), s->radius); // dividing by radius is a fast way to get unit vector
	hitrec->hit_front = outward_normal(r->dir, &n);
	hitrec->n = n;
	hitrec->mat = obj->mat;
	hitrec->text = obj->text;
	float phi = acosf(-hitrec->n.z);
	float theta = atan2(-hitrec->n.x, hitrec->n.y) + pi;
	hitrec->u = theta / (2.0f * pi);
	hitrec->v = phi / pi;

	return 1;
}

vec3 TriangleNormal(void *self, vec3 relative) {
	triangle *tri = &((object *)self)->shape.triangle;

	vec3 BA = vec3_sub(tri->b, tri->a);
	vec3 CA = vec3_sub(tri->c, tri->a);
	return vec3_unit(vec3_cross(BA, CA));
}

int IntersectTriangle(void *self, ray *r, hit_record *hitrec, thread_context *thread){
	object *obj = self;
	triangle *tri = &obj->shape.triangle;
	vec3 BA = vec3_sub(tri->b, tri->a);
	vec3 CA = vec3_sub(tri->c, tri->a);

	vec3 dir_cross_CA = vec3_cross(r->dir, CA);
	float determinant = vec3_dot(BA, dir_cross_CA);

	// TODO : change this small value to a variable
	if (fabs(determinant) < 0.0001f) return 0;

	float inv_determinant = 1.0f / determinant;

	vec3 raypointA = vec3_sub(r->pt, tri->a);

	float u = vec3_dot(raypointA, dir_cross_CA) * inv_determinant;

	if (u < 0.0f || u > 1.0f) return 0;

	vec3 raypointA_cross_BA = vec3_cross(raypointA, BA);
	float v = vec3_dot(r->dir, raypointA_cross_BA) * inv_determinant;

	if (v < 0.0f || u + v > 1.0f) return 0;

	float t = vec3_dot(CA, raypointA_cross_BA) * inv_determinant;

	if (t < hitrec->t_min || t > hitrec->t) return 0;

	hitrec->t = t;
	hitrec->pt = pt_on_ray(r, t);
	vec3 n = vec3_cross(BA, CA);
	// NOTE : when the triangle is double sided I want hit front to always be true
	// However I still need to call outward_normal because it sets the normal vector to the appropriate side
	hitrec->hit_front = outward_normal(r->dir, &n) || tri->double_sided;
	hitrec->n = vec3_unit(n);
	hitrec->mat = obj->mat;
	hitrec->text = obj->text;

	// Sphere uv coords
	hitrec->u = u;
	hitrec->v = v;

	return 1;
	// NOTE : u and v are the uv coordinates. I don't need them yet but that'll be useful.
}

int IntersectQuad(void *self, ray *r, hit_record *hitrec, thread_context *thread) {
	object *q_obj = (object *)self;
	quad q = q_obj->shape.quad;
	//hit_record temp = *hitrec;
	int hit_a = IntersectTriangle(q.tri_a, r, hitrec, thread);
	// Pretty sure hitrec prevents hit on b if a is blocking it
	int hit_b = IntersectTriangle(q.tri_b, r, hitrec, thread);
	if (hit_a == 0 && hit_b == 0) {
		return 0;
	}
	// tri_a.a is the bottom left, uv = 0,0
	// tri_b.c is top right, uv = 1,1
	if(!hit_b) {
		hitrec->v *= 0.5;
	}
	
	return hit_a || hit_b;
}

// NOTE: this should never be called anyway
vec3 FBMNormal(void *self, vec3 relative) {
	return vec3_new(0.0f, 0.0f, 0.0f);
}

/* Add offset to warper_ray.pt */
int IntersectFBMShape(void *self, ray *r, hit_record *hitrec, thread_context *thread) {
	object *obj = self;
	fbm_shape *fbm_obj = &obj->shape.fbm_shape;
	vec3 d = vec3_new(0.0f, 0.0f, 0.0f);
	float fbm_value = fbm_obj->offset_scale * fbm_with_derivative(fbm_obj->noise, vec3_mul(r->dir, fbm_obj->scale), fbm_obj->hurst, fbm_obj->octaves, &d);
	vec3 offset = vec3_mul(((object *)fbm_obj->obj)->Normal(fbm_obj->obj, r->dir), fbm_value);

	ray warped_ray = ray_new(vec3_add(r->pt, offset), r->dir); // Offset ray point
	//ray warped_ray = ray_new(r->pt, vec3_add(r->dir, offset)); // Offset ray direciton

	int hit = ((object *)fbm_obj->obj)->Intersect(fbm_obj->obj, &warped_ray, hitrec, thread);
	if (!hit) return hit;

	//d = vec3_unit(d);
	vec3 pt = pt_on_ray(r, hitrec->t);
	d = vec3_div(d, fbm_value);
	vec3 h = vec3_sub(d, vec3_mul(hitrec->n, vec3_dot(d, hitrec->n)));
	vec3 n = vec3_sub(n, h);
	n = vec3_unit(n);
	//hitrec->n = vec3_unit(vec3_new(d.x * pt.x, d.y * pt.y, d.z * pt.z));
	//outward_normal(pt, &n);
	outward_normal(warped_ray.dir, &n);
	hitrec->pt = pt;
	hitrec->n = n;
	return hit;
}

// NOTE : I just realized why this shape has bugs in it's reflections
// NOTE : when a ray starts inside the 'max possible sphere' it doesn't realize it!!!
int IntersectFBMSphere(void *self, ray *r, hit_record *hitrec, thread_context *thread) {
#if USE_RAYMARCH_FBM
	object *obj = self;
	fbm_shape *fbm_obj = &obj->shape.fbm_shape;
	object *sph = ((object *)fbm_obj->obj);

	hit_record temp_hitrec = *hitrec;
	object temp_sphere = *sph;

	float max_radius = sph->shape.sphere.radius + fbm_obj->offset_scale;
	temp_sphere.shape.sphere.radius = max_radius;

	// Check if starting point of ray is inside of the 'max possible sphere'
	// if it is then you should go straight to the raymarch step
	float dist_to_sphere = vec3_len(vec3_sub(r->pt, temp_sphere.shape.sphere.center));
	if (dist_to_sphere < max_radius) {
		temp_hitrec.t = 0.0f;
	}
	// An appropriate bounding sphere is the child sphere it's radius extended by fbm_obj->offset_scale
	// if the ray doesn't intersect with this sphere you can just exit
	else if(!sph->Intersect(&temp_sphere, r, &temp_hitrec, thread)) {
		return 0;
	}

	// If you make it here you are going to have to check against the actual fbm offset values
	int hit = 0;
	vec3 d = vec3_new(0.0f, 0.0f, 0.0f);
	vec3 normal;
	vec3 noise_input = vec3_new(0.0f, 0.0f, 0.0f);
	float fbm_value, offset_radius;
	float noise_scale = fbm_obj->scale * sph->shape.sphere.radius;

	// First check if the initial intersection was a glance - it only intersected the sphere at one point.
	// If that's the case then the only way you need to draw the sphere is if the offset at that point is the max
	float min_t = temp_hitrec.t;
	hit_record temp_hitrec2 = *hitrec;
	// Make a new ray that is slight forward along the ray to avoid hitting the same spot twice
	ray ray_from_hit = ray_new(pt_on_ray(r, min_t + 0.01f), r->dir);

	if(!sph->Intersect(&temp_sphere, &ray_from_hit, &temp_hitrec2, thread)) { // No intersection
		noise_input = vec3_mul(temp_hitrec.n, noise_scale);
		fbm_value = fbm_obj->offset_scale * fbm_with_derivative(fbm_obj->noise, noise_input, fbm_obj->hurst, fbm_obj->octaves, &d);
		offset_radius = sph->shape.sphere.radius + fbm_value;
		normal = temp_hitrec.n;

		if (offset_radius >= max_radius){
			hit = 1;
		}
	} else {
		// In this case the incident ray cuts through the "largest possible sphere"
		// Need to check if the offset_radii intersect the ray at any point between the two intersections 
		float max_t = temp_hitrec2.t + min_t;
		float t_inc = (max_t - min_t) * 0.001f / noise_scale; // divide by noise scale because large noise scale has faster changes
		float current_t = min_t; // NOTE: this t_increment might be too largs. It's super possible I'll just miss fbm values
		// NOTE : is there a faster way to get the normals for different t values?
		//        could I use the dot product or projections somehow?
		point3 raypt, raypt_to_center;
		float dist_to_center = 0.0f;
		float prev_delta = 0.1f;
		float marker = 0.0f;
		float EPSILON = 0.0002f;
		float t_inc_save = t_inc;
		// Backtracking is a flag that tells whether or not you have crossed a potential intersection
		// backtracking is kind of like a binary search - you keep halfing the increment and checking which direction you should go next
		// spheres with large radii will have larger t_increments which means it's less likely any given t value is an exact intersection
		// so add more backtracking steps based on the size of the sphere.
		int backtracking = 0;
		int BACKTRACKING_STEPS = 10 + (int)(max_radius * 0.1f);
		hit_record loop_hitrec = *hitrec;
		while ((current_t < max_t)) {
			loop_hitrec.t_min = current_t;
			loop_hitrec.t_max = current_t + t_inc;
			raypt = pt_on_ray(r, current_t);
			raypt_to_center = vec3_sub(raypt, sph->shape.sphere.center);
			dist_to_center = vec3_len(raypt_to_center);
			normal = vec3_unit(raypt_to_center);
			fbm_value = fbm_obj->offset_scale * fbm_with_derivative(fbm_obj->noise, vec3_mul(normal, noise_scale), fbm_obj->hurst, fbm_obj->octaves, &d);
			offset_radius = sph->shape.sphere.radius + fbm_value;
			float delta = (dist_to_center - offset_radius);

			if(fabs(delta) < EPSILON) {
				temp_sphere.shape.sphere.radius = offset_radius;
				hit = sph->Intersect(&temp_sphere, r, &loop_hitrec, thread);
				EPSILON *= 0.5f;
			}
			if (hit) break;
			if (signbit(prev_delta) == signbit(delta)) {
				if (backtracking) t_inc *= 0.5f;
				else t_inc *= 1.1f; // NOTE : this factor of 1.2f is kind of just a guess at a number that speeds up the render without skipping over the edges
			} else {
				// Sign has changed since last check.
				// If you aren't already backtracking, start backtracking and set backtracking to max number of backtracking steps
				// Also save place you start backtracking from
				if(!backtracking) {
					marker = current_t;
					backtracking = BACKTRACKING_STEPS;
				}
				// Flip increment sign so you are moving towards the intercept
				t_inc *= -0.5f;
			}
			if (backtracking) {
				--backtracking;
				if(!backtracking) {
					current_t = marker;
					t_inc = t_inc_save;
				}
			}
			prev_delta = delta;
			current_t += t_inc;
		}

		if(!hit) {
			noise_input = vec3_mul(temp_hitrec2.n, noise_scale);
			fbm_value = fbm_obj->offset_scale * fbm_with_derivative(fbm_obj->noise, noise_input, fbm_obj->hurst, fbm_obj->octaves, &d);
			offset_radius = sph->shape.sphere.radius + fbm_value;
			if (offset_radius >= temp_sphere.shape.sphere.radius) hit = 1;
		}
	}

	if(!hit) return 0;

	temp_sphere.shape.sphere.radius = offset_radius;
	hit = sph->Intersect(&temp_sphere, r, hitrec, thread);
	if(!hit) return 0;

	//d = vec3_unit(d);
	//d = vec3_div(d, offset_radius);
	d = vec3_mul(d, fbm_obj->scale);
	// p is the point on the sphere where offset = 0.
	vec3 p = vec3_mul(hitrec->n, sph->shape.sphere.radius);
	vec3 h = vec3_sub(vec3_mul(d, fbm_obj->offset_scale), vec3_mul(hitrec->n, vec3_dot(d, hitrec->n)));
	vec3 n = vec3_sub(p, vec3_mul(h, offset_radius));

	n = vec3_unit(n);
	//hitrec->hit_front = outward_normal(r->dir, &n);
	hitrec->n = n;
	return hit;
}
#else
	object *obj = self;
	fbm_shape *fbm_obj = &obj->shape.fbm_shape;
	object *sph = ((object *)fbm_obj->obj);
	hit_record temp_hitrec = *hitrec;
	object temp_sphere = *sph;
	temp_sphere.shape.sphere.radius += fbm_obj->offset_scale;

	// First check if the ray would hit the sphere in the 'worst' case (largest possible radius)
	// this saves SO MUCH TIME just because of how expensive fbm is.
	if(!sph->Intersect(&temp_sphere, r, &temp_hitrec)) {
		return 0;
	}

	vec3 d = vec3_new(0.0f, 0.0f, 0.0f);
	vec3 spt = vec3_mul(temp_hitrec.pt, fbm_obj->scale / fbm_obj->offset_scale);
	float fbm_value = fbm_obj->offset_scale * fbm_with_derivative(fbm_obj->noise, spt, fbm_obj->hurst, fbm_obj->octaves, &d);
	float offset = sph->shape.sphere.radius + fbm_value;
	temp_sphere.shape.sphere.radius = offset;
	int hit = sph->Intersect(&temp_sphere, r, hitrec, thread);
	if (!hit) return hit;

	//d = vec3_unit(d);
	d = vec3_div(d, fbm_value);
	// p is the point on the sphere where offset = 0.
	vec3 p = vec3_sub(vec3_sub(hitrec->pt, vec3_mul(hitrec->n, fbm_value)), sph->shape.sphere.center);
	vec3 h = vec3_sub(d, vec3_mul(p, vec3_dot(d, p)));
	vec3 n = vec3_sub(p, vec3_mul(h, fbm_obj->offset_scale));

	n = vec3_unit(n);
	outward_normal(r->dir, &n);
	hitrec->n = n;
	return hit;
}
#endif

// This is an error but it looks kind of neat
int IntersectConfettiSphere(void *self, ray *r, hit_record *hitrec, thread_context *thread) {
	object *obj = self;
	fbm_shape *fbm_obj = &obj->shape.fbm_shape;
	object *sph = ((object *)fbm_obj->obj);
	hit_record temp_hitrec = *hitrec;
	object temp_sphere = *sph;
	temp_sphere.shape.sphere.radius += (fbm_obj->offset_scale * 1.1f); // NOTE: my noise functions seem to return values slightly greater than 1

	// First check if the ray would hit the sphere in the 'worst' case (largest possible radius)
	// this saves SO MUCH TIME just because of how expensive fbm is.
	if(!sph->Intersect(&temp_sphere, r, &temp_hitrec, thread)) {
		return 0;
	}

	// If the ray intersects the sphere with the largest possible radius you need to check if it intersects with the real fbm offsets
	// To generate offsets on the sphere I will use the vectors normal to the sphere at the points
	// This way the sphere looks the same from all points of view.
	//
	// But this is going to be pretty expensive.
	//

	int hit = 0;
	vec3 d = vec3_new(0.0f, 0.0f, 0.0f);
	vec3 noise_input = vec3_new(0.0f, 0.0f, 0.0f);
	float fbm_value, offset_radius;

	// First check if the initial intersection was a glance - it only intersected the sphere at one point.
	// If that's the case then the only way you need to draw the sphere is if the offset at that point is the max
	float min_t = temp_hitrec.t;
	hit_record temp_hitrec2 = *hitrec;
	ray ray_from_hit = ray_new(pt_on_ray(r, min_t + 0.001f), r->dir);

	if(!sph->Intersect(&temp_sphere, &ray_from_hit, &temp_hitrec2, thread)) {
		noise_input = vec3_mul(temp_hitrec.n, fbm_obj->scale);
		fbm_value = fbm_obj->offset_scale * fbm_with_derivative(fbm_obj->noise, noise_input, fbm_obj->hurst, fbm_obj->octaves, &d);
		offset_radius = sph->shape.sphere.radius + fbm_value;

		if (offset_radius >= temp_sphere.shape.sphere.radius) hit = 1;
	} else {
		// In this case the incident ray cuts through the "largest possible sphere"
		// Need to check if the offset_radii intersect the ray at any point between the two intersections 
		float max_t = temp_hitrec2.t + min_t;
		float t_inc = (max_t - min_t) / fbm_obj->scale * 0.001f; // Aim to check 100 points along the ray scaled to the inverse of the fbm object
		float current_t = min_t; // NOTE: this t_increment might be too largs. It's super possible I'll just miss fbm values
		float marker = NAN; // This is used to 
		// NOTE : is there a faster way to get the normals for different t values?
		//        could I use the dot product or projections somehow?
		point3 ray_point, normal;
		float dist_to_center;
		while (!hit && (current_t < max_t)) {
			ray_point = pt_on_ray(r, current_t);
			normal = vec3_sub(ray_point, sph->shape.sphere.center);
			dist_to_center = vec3_len(normal);
			noise_input = vec3_unit(normal);
			fbm_value = fbm_obj->offset_scale * fbm_with_derivative(fbm_obj->noise, vec3_mul(noise_input, fbm_obj->scale), fbm_obj->hurst, fbm_obj->octaves, &d);
			offset_radius = sph->shape.sphere.radius + fbm_value;

			// NOTE : this is very poorly done. Clean it up because you end up backtracking multiple times
 			// if the offset radius is greater than the current distance to center then there is an intersection behind you OR there is a "floating island" type thing
			/*
			if (offset_radius > dist_to_center) {
				marker = current_t;
				current_t -= (t_inc * 0.4f);
			} else if (offset_radius < dist_to_center) {
				if (marker == marker) {
					current_t = marker; marker = NAN;
				}
				current_t += t_inc;
			} else {
				hit = 1;
			}
			*/
			if(fabs(offset_radius-dist_to_center) < 0.04f) hit = 1;
			else current_t += t_inc;
		}

		if(!hit) {
			noise_input = vec3_mul(temp_hitrec2.n, fbm_obj->scale);
			fbm_value = fbm_obj->offset_scale * fbm_with_derivative(fbm_obj->noise, noise_input, fbm_obj->hurst, fbm_obj->octaves, &d);
			offset_radius = sph->shape.sphere.radius + fbm_value;
			if (offset_radius >= temp_sphere.shape.sphere.radius) hit = 1;
		}
	}

	if(!hit) return 0;

	temp_sphere.shape.sphere.radius = offset_radius;

	hit = sph->Intersect(&temp_sphere, r, hitrec, thread);

	//d = vec3_unit(d);
	d = vec3_div(d, fbm_value);
	// p is the point on the sphere where offset = 0.
	vec3 p = vec3_sub(vec3_sub(hitrec->pt, vec3_mul(hitrec->n, fbm_value)), sph->shape.sphere.center);
	vec3 h = vec3_sub(d, vec3_mul(p, vec3_dot(d, p)));
	vec3 n = vec3_sub(p, vec3_mul(h, fbm_obj->offset_scale));

	n = vec3_unit(n);
	outward_normal(r->dir, &n);
	hitrec->n = n;
	return hit;
}

object make_sphere(point3 center, float r, texture *text, material *mat) {
	object o;
	o.Intersect = (*IntersectSphere);
	o.Normal = (*SphereNormal);
	o.id = Sphere;
	o.shape.sphere = sphere_new(center, r);
	o.text = text;
	o.mat = mat;
	return o;
}

object make_triangle(point3 a, point3 b, point3 c, int double_sided, texture *text, material *mat) {
	object o;
	o.Intersect = (*IntersectTriangle);
	o.Normal = (*TriangleNormal);
	o.id = Triangle;
	o.shape.triangle = triangle_new(a, b, c, double_sided);
	o.text = text;
	o.mat = mat;
	return o;
}

// If a quads given points are not in the same plane it cannot be created.
// So to be able to tell when that happens without changing a bunch of stuff
// I'm being lazy and adding a SHAPE_ERROR enum to ShapeID.
// This way my program won't crash
object make_quad(memory_region *region, vec3 a, vec3 b, vec3 c, vec3 d, int double_sided, texture *text, material *mat) {
	object o;
	o.shape.quad = quad_new(region, a, b, c, d, double_sided, text, mat);
	o.id = Quad;
	if (o.shape.quad.tri_a == NULL || o.shape.quad.tri_b == NULL) {
		o.id = SHAPE_ERROR;
	}
	o.Intersect = (*IntersectQuad);
	o.text = text;
	o.mat = mat;

	return o;
}

object make_fbm_shape(memory_region *region, noise *noise, float scale, float offset_scale, float hurst, int octaves, object *obj) {
	object o;
	o.Intersect = (*IntersectFBMShape);
	o.id = FBMShape;
	o.mat = obj->mat;
	o.text = obj->text;
	o.shape.fbm_shape = fbm_shape_new(region, noise, scale, offset_scale, hurst, octaves, obj);
	return o;
}

object make_fbm_sphere(memory_region *region, noise *noise, float scale, float offset_scale, float hurst, int octaves, object *obj) {
	object fbm_obj = make_fbm_shape(region, noise, scale, offset_scale, hurst, octaves, obj);
	fbm_obj.Intersect = (*IntersectFBMSphere);
	return fbm_obj;
}

object *add_sphere(memory_region *region, point3 center, float r, texture *text, material *mat) {
	object o = make_sphere(center, r, text, mat);
	return (object *)memory_region_add(region, &o, sizeof(object));
}

// Default - a double sided triangle
object *add_triangle(memory_region *region, point3 a, point3 b, point3 c, texture *text, material *mat) {
	object o = make_triangle(a, b, c, 1, text, mat);
	return (object *)memory_region_add(region, &o, sizeof(object));
}

object *add_single_sided_triangle(memory_region *region, point3 a, point3 b, point3 c, texture *text, material *mat) {
	object o = make_triangle(a, b, c, 0, text, mat);
	return (object *)memory_region_add(region, &o, sizeof(object));
}

object *add_quad(memory_region *region, vec3 a, vec3 b, vec3 c, vec3 d, texture *text, material *mat) {
	object q = make_quad(region, a, b, c, d, 1, text, mat);
	if (q.id == SHAPE_ERROR) {
		return NULL;
	}
	return (object *)memory_region_add(region, &q, sizeof(object));

}

object *add_ss_quad(memory_region *region, vec3 a, vec3 b, vec3 c, vec3 d, texture *text, material *mat) {
	object q = make_quad(region, a, b, c, d, 0, text, mat);
	if (q.id == SHAPE_ERROR) {
		return NULL;
	}
	return (object *)memory_region_add(region, &q, sizeof(object));
}

object *add_fbm_shape(memory_region *region, noise *noise, float scale, float offset_scale, float hurst, int octaves, object *obj) {
	object o = make_fbm_shape(region, noise, scale, offset_scale, hurst, octaves, obj);
	return (object *)memory_region_add(region, &o, sizeof(object));
}

object *add_fbm_sphere(memory_region *region, noise *noise, float scale, float offset_scale, float hurst, int octaves, object *obj) {
	object o = make_fbm_sphere(region, noise, scale, offset_scale, hurst, octaves, obj);
	return (object *)memory_region_add(region, &o, sizeof(object));
}
