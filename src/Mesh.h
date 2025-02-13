///
///  Mesh.h
///  Basic_Graphics
///
///  Created by Bret Jackson on 1/29/17.
///
///  \brief Allocates and stores a mesh in a VAO on the gpu.
///

#ifndef Mesh_hpp
#define Mesh_hpp

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
//#include <glad/glad.h>

#ifdef _WIN32
#include "GL/glew.h"
#include "GL/wglew.h"
#elif (!defined(__APPLE__))
#include "GL/glxew.h"
#endif

// OpenGL Headers
#if defined(WIN32)
//#define NOMINMAX
#include <windows.h>
#include <GL/gl.h>
#elif defined(__APPLE__)
#define GL_GLEXT_PROTOTYPES
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#else
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif

#include "Texture.h"
#include "GLSLProgram.h"
#include <vector>
#include <memory>

namespace basicgraphics {

	typedef std::shared_ptr<class Mesh> MeshRef;

	class Mesh : public std::enable_shared_from_this<Mesh>
	{
	public:
		struct Vertex {
			glm::vec3 position;
			glm::vec3 normal;
			glm::vec2 texCoord0;
		};


		// Creates a vao and vbo. Usage should be GL_STATIC_DRAW, GL_DYNAMIC_DRAW, etc. Leave data empty to just allocate but not upload.
		Mesh(std::vector<std::shared_ptr<Texture> > textures, GLenum primitiveType, GLenum usage, int allocateVertexByteSize, int allocateIndexByteSize, int vertexOffset, const std::vector<Vertex> &data, int numIndices = 0, int indexByteSize = 0, int* index = nullptr);
		virtual ~Mesh();

		virtual void draw(GLSLProgram &shader);

		void setMaterialColor(const glm::vec4 &color);

		// Returns the number of bytes allocated in the vertexVBO
		int getAllocatedVertexByteSize() const;
		int getAllocatedIndexByteSize() const;
		// Returns the number of bytes actually filled with data in the vertexVBO
		int getFilledVertexByteSize() const;
		int getFilledIndexByteSize() const;
		int getNumIndices() const;

		GLuint getVAOID() const;

		// Update the vbos. startByteOffset+dataByteSize must be <= allocatedByteSize
		void updateVertexData(int startByteOffset, int vertexOffset, const std::vector<Vertex> &data);
		void updateIndexData(int totalNumIndices, int startByteOffset, int indexByteSize, int* index);

	private:
		GLuint _vaoID;
		GLuint _vertexVBO;
		GLuint _indexVBO;
		GLenum _primitiveType;

		int _allocatedVertexByteSize;
		int _allocatedIndexByteSize;
		int _filledVertexByteSize;
		int _filledIndexByteSize;
		int _numIndices;

		glm::vec4 _materialColor;

		std::vector<std::shared_ptr<Texture> > _textures;
	};

}

#endif /* Mesh_hpp */
