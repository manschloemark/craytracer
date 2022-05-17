#include "objects.h"

// Ensure the normal is pointing in the opposite direction of the incident ray
// Return 0 if you had to negate the normal vector - that means the intersection was 'inside' the object
// Return 1 otherwise
// NOTE : this function serves two purposes and that is not very clear, but it kills two birds with one stone
int outward_normal(vec3 incident, vec3 *normal) {
	if(vec3_dot(incident, *normal) > 0.0) {
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

fbm_shape fbm_shape_new(memory_region *region, noise *noise, float scale, float offset_scale, float hurst, int octaves, void *obj) {
	fbm_shape fs;
	fs.noise = (noise == NULL) ? add_perlin_noise(region, 32) : noise;
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
int IntersectSphere(void *self, ray *r, hit_record *hitrec) {
	object *obj = self;
	sphere *s = &((object *)self)->shape.sphere;

	vec3 T = vec3_sub(r->pt, s->center);
	float a = vec3_lensq(r->dir);
	float half_b = vec3_dot(T, r->dir);
	float c = vec3_lensq(T) - s->radius*s->radius; 

	float discriminant = half_b*half_b - a*c;

	if (discriminant < 0.0) {
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
	hitrec->u = theta / (2.0 * pi);
	hitrec->v = phi / pi;

	return 1;
}

vec3 TriangleNormal(void *self, vec3 relative) {
	triangle *tri = &((object *)self)->shape.triangle;

	vec3 BA = vec3_sub(tri->b, tri->a);
	vec3 CA = vec3_sub(tri->c, tri->a);
	return vec3_unit(vec3_cross(BA, CA));
}

int IntersectTriangle(void *self, ray *r, hit_record *hitrec){
	object *obj = self;
	triangle *tri = &obj->shape.triangle;
	vec3 BA = vec3_sub(tri->b, tri->a);
	vec3 CA = vec3_sub(tri->c, tri->a);

	vec3 dir_cross_CA = vec3_cross(r->dir, CA);
	float determinant = vec3_dot(BA, dir_cross_CA);

	// TODO : change this small value to a variable
	if (fabs(determinant) < 0.0001) return 0;

	float inv_determinant = 1.0 / determinant;

	vec3 raypointA = vec3_sub(r->pt, tri->a);

	float u = vec3_dot(raypointA, dir_cross_CA) * inv_determinant;

	if (u < 0.0 || u > 1.0) return 0;

	vec3 raypointA_cross_BA = vec3_cross(raypointA, BA);
	float v = vec3_dot(r->dir, raypointA_cross_BA) * inv_determinant;

	if (v < 0.0 || u + v > 1.0) return 0;

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

// NOTE: this should never be called anyway
vec3 FBMNormal(void *self, vec3 relative) {
	return vec3_new(0.0, 0.0, 0.0);
}

/* Add offset to warper_ray.pt */
int IntersectFBMShape(void *self, ray *r, hit_record *hitrec) {
	object *obj = self;
	fbm_shape *fbm_obj = &obj->shape.fbm_shape;
	vec3 d = vec3_new(0.0, 0.0, 0.0);
	float fbm_value = fbm_obj->offset_scale * fbm_with_derivative(fbm_obj->noise, vec3_mul(r->dir, fbm_obj->scale), fbm_obj->hurst, fbm_obj->octaves, &d);
	vec3 offset = vec3_mul(((object *)fbm_obj->obj)->Normal(fbm_obj->obj, r->dir), fbm_value);

	ray warped_ray = ray_new(vec3_add(r->pt, offset), r->dir); // Offset ray point
	//ray warped_ray = ray_new(r->pt, vec3_add(r->dir, offset)); // Offset ray direciton

	int hit = ((object *)fbm_obj->obj)->Intersect(fbm_obj->obj, &warped_ray, hitrec);
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

int IntersectFBMSphere(void *self, ray *r, hit_record *hitrec) {
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

	vec3 d = vec3_new(0.0, 0.0, 0.0);
	vec3 spt = vec3_mul(temp_hitrec.pt, fbm_obj->scale / fbm_obj->offset_scale);
	float fbm_value = fbm_obj->offset_scale * fbm_with_derivative(fbm_obj->noise, spt, fbm_obj->hurst, fbm_obj->octaves, &d);
	float offset = sph->shape.sphere.radius + fbm_value;
	temp_sphere.shape.sphere.radius = offset;
	int hit = sph->Intersect(&temp_sphere, r, hitrec);
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

object *add_fbm_shape(memory_region *region, noise *noise, float scale, float offset_scale, float hurst, int octaves, object *obj) {
	object o = make_fbm_shape(region, noise, scale, offset_scale, hurst, octaves, obj);
	return (object *)memory_region_add(region, &o, sizeof(object));
}

object *add_fbm_sphere(memory_region *region, noise *noise, float scale, float offset_scale, float hurst, int octaves, object *obj) {
	object o = make_fbm_sphere(region, noise, scale, offset_scale, hurst, octaves, obj);
	return (object *)memory_region_add(region, &o, sizeof(object));
}
