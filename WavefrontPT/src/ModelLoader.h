//
//
//namespace wpt {
//	bool loadModel(tinygltf::Model& model, const char* filename) {
//		tinygltf::TinyGLTF loader;
//		std::string err;
//		std::string warn;
//		bool res = loader.LoadASCIIFromFile(&model, &err, &warn, filename);
//		if (!warn.empty()) {
//			std::cout << "WARN: " << warn << std::endl;
//		}
//
//		if (!err.empty()) {
//			std::cout << "ERR: " << err << std::endl;
//		}
//
//		if (!res)
//			std::cout << "Failed to load glTF: " << filename << std::endl;
//		else
//			std::cout << "Loaded glTF: " << filename << std::endl;
//
//		return res;
//	}
//
//	void printVertices(tinygltf::Model& model) {
//		const int positionAccessorIndex = model.meshes[0].primitives[0].attributes["POSITION"];
//		const tinygltf::Accessor& accessor = model.accessors[positionAccessorIndex];
//		const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
//		const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];
//
//		const unsigned char* positions = &buffer.data[bufferView.byteOffset + accessor.byteOffset];
//		int stride = accessor.ByteStride(bufferView);
//		printf("stride: %d\n", stride);
//
//		for (size_t i = 0; i < accessor.count; ++i) {
//			const unsigned char* vertexPtr = positions + (i * stride);
//
//			const float* pos = reinterpret_cast<const float*>(vertexPtr);
//			printf("(%f, %f, %f)\n", pos[0], pos[1], pos[2]);
//		}
//	}
//}