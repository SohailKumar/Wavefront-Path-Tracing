#include <iostream>
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
// #define TINYGLTF_NO_EXTERNAL_IMAGE // Use this if you don't want STB
#include "tiny_gltf.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "ModelLoader.h"
//#include <Windows.h>	

int ModelLoadMain() {
	std::string filename = "./models/Cube/Cube.gltf";

	tinygltf::Model model;
	if (!wpt::loadModel(model, filename.c_str())) return 1;

	wpt::printVertices(model);

	IMGUI_CHECKVERSION();

	return 0;
}