/* test.c - test parts of craytrace */
#include <stdio.h>

#include "../vector3.h"

#define ACCEPTED_ERROR 0.001

void print_vec3(vec3 v) {
	printf("vec3(x=%f y=%f z=%f)", v.x, v.y, v.z);
}

// Due to floating point errors I don't think it's wise to just do a.x==b.x
// Because like, if a = {1, 2, 1} and b = {0, 1, 0} + {1, 1, 1}, a == b should be true but b might have been rounded at some point.
int vec3_about_equal(vec3 a, vec3 b) {
	vec3 diff = vec3_abs(vec3_sub(a, b));
	return (diff.x < ACCEPTED_ERROR) && (diff.y < ACCEPTED_ERROR) && (diff.z < ACCEPTED_ERROR);
}


int test_vec3_add() {
	vec3 a =  {1.0, 0.0, 0.0};
	vec3 b = {0.5, 1.0, 0.0};
	vec3 c = {0.0, 0.333, 1.0};
	// Test addition

	vec3 sum = vec3_add(a, b);
	vec3 exp_sum = {1.5, 1.0, 0.0};

	puts("Test vec3 Addition");
	print_vec3(a);
	printf(" + ");
	print_vec3(b);
	printf("\nExpected value:   ");
	print_vec3(exp_sum);
	printf("\nCalculated value: ");
	print_vec3(sum);
	puts("");

	if(!vec3_about_equal(sum, exp_sum)) return -1;

	vec3 sum2 = vec3_add(sum, c);
	vec3 exp_sum2 = {1.5, 1.333, 1.0};

	print_vec3(sum);
	printf(" + ");
	print_vec3(c);
	printf("\nExpected value:   ");
	print_vec3(exp_sum2);
	printf("\nCalculated value: ");
	print_vec3(sum2);
	puts("");

	if(!vec3_about_equal(sum2, exp_sum2)) return -1;

	return 0;
}

int test_vec3_sub() {
	vec3 a =  {1.0, 0.0, 0.0};
	vec3 b = {0.5, 1.0, 0.0};

	vec3 sub = vec3_sub(a, b);
	vec3 exp_sub = {0.5, -1.0};

	puts("Testing vec3 Subtraction");
	print_vec3(a);
	printf(" - ");
	print_vec3(b);
	printf("\nExpected value:   ");
	print_vec3(exp_sub);
	printf("\nCalculated value: ");
	print_vec3(sub);
	puts("");

	if(!vec3_about_equal(sub, exp_sub)) return -1;

	return 0;
}

int test_vec3_dot() {
	vec3 u = {10.0, 2.0, 4.0};
	vec3 v = {2.0, 6.0, 10.0};

	float dot = vec3_dot(u, v);
	float exp_dot = 72.0;

	puts("Testing vec3 Dot Product");
	print_vec3(u);
	printf(" * ");
	print_vec3(v);
	printf("\nExpected value: %f", exp_dot);
	printf("\nCalculated value: %f", dot);
	puts("");
	if(fabsf(dot - exp_dot) > ACCEPTED_ERROR) return -1;

	return 0;
}

int test_vec3_cross() {
	vec3 u = {10.0, 2.0, 4.0};
	vec3 v = {2.0, 6.0, 10.0};
	vec3 cross = vec3_cross(u, v);
	vec3 exp_cross = {-4.0, -92.0, 56.0};

	puts("Testing vec3 Cross Product");
	print_vec3(u);
	printf(" x ");
	print_vec3(v);
	printf("\nExpected value:   ");
	print_vec3(exp_cross);
	printf("\nCalculated value: ");
	print_vec3(cross);
	puts("");

	if(!vec3_about_equal(cross, exp_cross)) return -1;

	return 0;
}

int test_vec3_neg() {
	vec3 a = {1.0, 0.0, -2.00232323};
	vec3 neg_a = vec3_neg(a);
	vec3 exp_neg_a = {-1.0, 0.0, 2.00232323};

	puts("Testing vec3 Negation");
	printf("-1 * ");
	print_vec3(a);
	printf("\nExpected value:   ");
	print_vec3(exp_neg_a);
	printf("\nCalculated value: ");
	print_vec3(neg_a);
	puts("");

	if(!vec3_about_equal(neg_a, exp_neg_a)) return -1;

	return 0;
}

int test_vec3_unit() {
	vec3 a = {3.0, 4.0, 0.0}; 
	vec3 unit = vec3_unit(a);
	vec3 exp_unit = {0.6, 0.8, 0.0};

	puts("Testing vec3 Unit Vector");
	print_vec3(a);
	printf("\nExpected value:   ");
	print_vec3(exp_unit);
	printf("\nCalculated value: ");
	print_vec3(unit);
	puts("");

	if(!vec3_about_equal(unit, exp_unit)) return -1;

	return 0;
}

int test_vec3() {
	int fail = 0;
	if(test_vec3_add()) {
		fail = 1;
		puts("!!! VEC3 ADDITION FAILED.");
	};
	if(test_vec3_sub()) {
		fail = 1;
		puts("!!! VEC3 SUBTRACTION FAILED");
	}
	if(test_vec3_dot()) {
		fail = 1;
		puts("!!! VEC3 DOT PRODUCT FAILED");
	}
	if(test_vec3_cross()) {
		fail = 1;
		puts("!!! VEC3 CROSS PRODUCT FAilED");
	}
	if(test_vec3_neg()) {
		fail = 1;
		puts("!!! VEC3 NEGATION FAILED");
	}
	if(test_vec3_unit()) {
		fail = 1;
		puts("!!! VEC3 UNIT VECTOR FAILED");
	}
	return fail;
}

int main(int argc, char **argv) {
	int vector_test = 0;
	if(test_vec3()) {
		puts("Error in vector test!");
		return -1;
	}
	puts("All tests passed.");
	return 0;
}
