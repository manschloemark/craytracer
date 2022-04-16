C Ray Tracer
--
A minimal raytracer written in C.  
This is largely inspired by Peter Shirley's Ray Tracing in One Weekend, but it uses C rather than C++.  

This project will help me practice C, architecture, graphics programming principles, and math.  

![A demo scene](./examples/ex.jpg)

Usage
---
Written on Manjaro Linux and I have not tested other distros.  
To compile:  
`cd src/`  
`source build.sh`  
build.sh compiles the program using gcc.  
the executable, crt.exe will be in craytracer/build/  

| Command Line Argument | Description | Default |
--- | --- | ---
| -o, --output=FILE          | Path where render is stored. Must be .ppm file | N/A |
| -w, --width=WIDTH          | Width of output image in pixels. | 720 |
| -h, --height=HEIGHT        | Height of output image in pixels. | width * 9/16  |
| -n, --num-samples=N_SAMPLES  | Use (integer) N_SAMPLES for each pixel | 10 |
| -s, --seed=SEED              | Seed to pass to stdlib.h srand(). | Uses current time as seed|
| -?, --help                   | Give this help list |
| --usage                  | Give a short usage message |


