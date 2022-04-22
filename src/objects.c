#include "objects.h"

// Ensure the normal is pointing in the opposite direction of the incident ray
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

triangle triangle_new(point3 x, point3 y, point3 z) {
	triangle t = {};
	t.a = x;
	t.b = y;
	t.c = z;
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
	hitrec->hit_front = outward_normal(r->dir, &n);
	hitrec->n = vec3_unit(n);
	hitrec->mat = obj->mat;
	hitrec->text = obj->text;

	// Sphere uv coords
	hitrec->u = u;
	hitrec->v = v;

	return 1;
	// NOTE : u and v are the uv coordinates. I don't need them yet but that'll be useful.
}

object make_sphere(point3 center, float r, texture *text, material *mat) {
	object o;
	o.id = Sphere;
	o.shape.sphere = sphere_new(center, r);
	o.text = text;
	o.mat = mat;
	return o;
}

object make_triangle(point3 a, point3 b, point3 c, texture *text, material *mat) {
	object o;
	o.id = Triangle;
	o.shape.triangle = triangle_new(a, b, c);
	o.text = text;
	o.mat = mat;
	return o;
}

object *add_sphere(memory_region *region, point3 center, float r, texture *text, material *mat) {
	object o = make_sphere(center, r, text, mat);
	return (object *)memory_region_add(region, &o, sizeof(object));
}

object *add_triangle(memory_region *region, point3 a, point3 b, point3 c, texture *text, material *mat) {
	object o = make_triangle(a, b, c, text, mat);
	return (object *)memory_region_add(region, &o, sizeof(object));
}

int Intersect(object *obj, ray *r, hit_record *hitrec) {
	switch (obj->id) {
		case Sphere:
			return IntersectSphere(obj, r, hitrec);
		case Triangle:
			return IntersectTriangle(obj, r, hitrec);
		default:
			return 0;
	}
}