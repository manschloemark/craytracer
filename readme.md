C Ray Tracer
--

Simple project I am working on to implement what I have learned so far.

This project will help me practice C, architecture, graphics programming principles, and math.

This is largely based off of Peter Shirley's Ray Tracing in One Weekend except in C rather than C++.
I am using the book as a reference rather than plainly copying it.

![A demo scene](./examples/ex.jpg)

Usage
---
Currently must be ran on a Linux machine.  
To compile run `source build.sh` in ./src/

| Command Line Argument | Description | Default |
--- | --- | ---
| -o, --output=FILE          | Path where render is stored. Must be .ppm file | N/A |
| -w, --width=WIDTH          | Width of output image in pixels. | 720 |
| -h, --height=HEIGHT        | Height of output image in pixels. | width * 9/16  |
| -n, --num-samples=N_SAMPLES  | Use (integer) N_SAMPLES for each pixel | 10 |
| -s, --seed=SEED              | Seed to pass to stdlib.h srand(). | Uses current time as seed|
| -?, --help                   | Give this help list |
| --usage                  | Give a short usage message |


