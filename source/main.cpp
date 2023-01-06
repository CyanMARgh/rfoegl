#include "demo_0.h" // glowing borders
#include "demo_1.h" // stripes
#include "demo_2.h" // two-border flat particles
#include "demo_3.h" // default shading
#include "demo_4.h" // atomic counter example 
#include "demo_5.h" // compute shader eample
#include "demo_6.h" // pendulum generator
#include "demo_7.h" // autostereogram
#include "demo_8.h" // WIP

#include <cstdio>
//fixes:
//TODO(!!!!!) clear static storages on window restart

//shader fix
	//TODO bool flip_uv somewhere (or little programm to flip textures)

//TODO framebuffer fix maybe (set_render_target) + imagebuffer class
	//TODO G-framebuffer -> generic framebuffer

//TODO (!!!) fix default lighting

//TODO global/default texture manager + local texture manager objects for models

//mesh fix
	//TODO remove attr_linker field
	//TODO decltype(GL_STATIC_DRAW) -> u32?

//TODO regroup shaders


//utils:
//TODO default textures for normals, diff/spec map
//TODO default 3d models (cube, rect) and maybe generator for sphere & thorus, raw_mesh class = {vector<T>, vector<u32>, compile function}


//features:
//TODO filters with compute shaders
//TODO palette generator
//TODO complex number plotter


int main(int argc, char * argv []) {
	// demo_0();
	// demo_0();
	// demo_1();
	// demo_1();
	// demo_2();
	// demo_2();
	// demo_3();
	// demo_3();
	// demo_4();
	// demo_4();
	// demo_5();
	// demo_5();
	// demo_6();
	// demo_6();
	// demo_7();
	// demo_7();
	demo_8();
    return 0;
}




