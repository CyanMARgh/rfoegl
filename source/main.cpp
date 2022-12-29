// #include "demo_0.h" // glowing borders
// #include "demo_1.h" // stripes
// #include "demo_2.h" // two-border flat particles
// #include "demo_3.h" // default shading
// #include "demo_4.h" // atomic counter example 
// #include "demo_5.h" // compute shader eample
// #include "demo_6.h" // pendulum generator
#include "demo_7.h" // autostereogram


//TODO make mesh with 5-rule and remove AC (and DEFER maybe too)
//TODO come up with idea for texture manager
//TODO shader class + compute shader
//TODO framebuffer fix maybe (set_render_target) + imagebuffer class
//TODO filters with compute shaders
//TODO default textures for normals, diff/spec map
//TODO bool flip_uv somewhere (or little programm to flip textures)
//TODO default 3d models (cube, rect) and maybe generator for sphere & thorus, raw_mesh class = {vector<T>, vector<u32>, compile function}
//TODO palette generator
//TODO #define support on shader compilaton
//TODO (!!!) fix default lighting
//TODO complex number plotter


int main(int argc, char * argv []) {
	demo_7();
    return 0;
}




