#include <stdio.h>
#include "../perlin_noise.h"

// NOTE : this is not a test like a unit test it's just a program to get some information about what values fbm produces

int main(int argc, char **argv) {
	int num_samples = 10000;
	if (argc > 1) num_samples = atoi(argv[1]);

	FILE *outfile = stdout;
	if (argc > 2) {
		outfile = fopen(argv[2], "w+");
	}
	if (!outfile) return -1;

	perlin p = perlin_new(256);  

	for (int i = 0; i < num_samples; ++i) {
		vec3 random = vec3_random();
		float fbm_v = fbm(&p, random, 1.0, 24);
		fprintf(outfile, "%f ", fbm_v);
	}
	fclose(outfile);
	return 0;
}
