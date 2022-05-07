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

// TODO : I only make s a pointer here because I don't feel like fixing all the ->, fix it later (also in IntersectTriangle)
int IntersectSphere(object *obj, ray *r, hit_record *hitrec) {
	sphere *s = &obj->shape.sphere;

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

int IntersectTriangle(object *obj, ray *r, hit_record *hitrec){
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

fbm_shape fbm_shape_new(memory_region *region, float hurst, int octaves, void *obj) {
	fbm_shape fs;
	fs.perlin = add_perlin(region, 32);
	fs.obj = obj;
	fs.hurst = hurst;
	fs.octaves = octaves;
	return fs;
}

int IntersectFBMShape(object *obj, ray *r, hit_record *hitrec) {
	fbm_shape *fbm_obj = &obj->shape.fbm_shape;
	ray warped_ray = ray_new(r->pt, vec3_addf(r->dir, 0.2 * fbm(fbm_obj->perlin, r->dir, fbm_obj->hurst, fbm_obj->octaves)));
	return Intersect(fbm_obj->obj, &warped_ray, hitrec);
}

object make_sphere(point3 center, float r, texture *text, material *mat) {
	object o;
	o.id = Sphere;
	o.shape.sphere = sphere_new(center, r);
	o.text = text;
	o.mat = mat;
	return o;
}

object make_triangle(point3 a, point3 b, point3 c, int double_sided, texture *text, material *mat) {
	object o;
	o.id = Triangle;
	o.shape.triangle = triangle_new(a, b, c, double_sided);
	o.text = text;
	o.mat = mat;
	return o;
}

object make_fbm_shape(memory_region *region, float hurst, int octaves, object *obj) {
	object o;
	o.id = FBMShape;
	o.mat = obj->mat;
	o.text = obj->text;
	o.shape.fbm_shape = fbm_shape_new(region, hurst, octaves, obj);
	return o;
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

object *add_fbm_shape(memory_region *region, float hurst, int octaves, object *obj) {
	object o = make_fbm_shape(region, hurst, octaves, obj);
	return (object *)memory_region_add(region, &o, sizeof(object));
}

int Intersect(object *obj, ray *r, hit_record *hitrec) {
	switch (obj->id) {
		case Sphere:
			return IntersectSphere(obj, r, hitrec);
		case Triangle:
			return IntersectTriangle(obj, r, hitrec);
		case FBMShape:
			return IntersectFBMShape(obj, r, hitrec);
		default:
			return 0;
	}
}
