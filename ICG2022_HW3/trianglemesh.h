#ifndef TRIANGLEMESH_H
#define TRIANGLEMESH_H

#include "headers.h"
#include "material.h"
#include <tuple> 
#include <unordered_map>
#include <queue>
#include <algorithm> 
typedef std::pair<float, unsigned int> p_fui;
typedef std::tuple<int, int, int> Index;

// VertexPTN Declarations.
struct VertexPTN
{
	VertexPTN() {
		position = glm::vec3(0.0f, 0.0f, 0.0f);
		normal = glm::vec3(0.0f, 1.0f, 0.0f);
		texcoord = glm::vec2(0.0f, 0.0f);
	}
	VertexPTN(glm::vec3 p, glm::vec3 n, glm::vec2 uv) {
		position = p;
		normal = n;
		texcoord = uv;
	}
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texcoord;
};

// SubMesh Declarations.
struct SubMesh
{
	SubMesh() {
		material = nullptr;
		iboId = 0;
	}
	PhongMaterial* material;
	GLuint iboId;
	std::vector<unsigned int> vertexIndices;
};


// TriangleMesh Declarations.
class TriangleMesh
{
public:
	// TriangleMesh Public Methods.
	TriangleMesh();
	~TriangleMesh();
	
	// Load the model from an *.OBJ file.
	bool LoadFromFile(const std::string& filePath, const bool normalized = true);
	
	// Show model information.
	void ShowInfo();

	// -------------------------------------------------------
	// Feel free to add your methods or data here.
	// -------------------------------------------------------

	int GetNumVertices() const { return numVertices; }
	int GetNumTriangles() const { return numTriangles; }
	int GetNumSubMeshes() const { return (int)subMeshes.size(); }

	glm::vec3 GetObjCenter() const { return objCenter; }
	glm::vec3 GetObjExtent() const { return objExtent; }

	void LoadMaterialFile(std::string, std::string, std::vector<PhongMaterial*>&);
	int FindMaterialIdx(std::string, std::vector<PhongMaterial*>&);
	void sortVertices(std::vector<unsigned int>&, float);
	void findmaxmin(glm::vec3&, glm::vec3&, glm::vec3);
	void createVertexBuffer();
	void createIndexBuffer();
	void SetMaterialUniform(PhongShadingDemoShaderProg*&, int);
	void Draw(int);
private:
	// -------------------------------------------------------
	// Feel free to add your methods or data here.
	// -------------------------------------------------------

	// TriangleMesh Private Data.
	GLuint vboId;
	
	std::vector<VertexPTN> vertices;
	// For supporting multiple materials per object, move to SubMesh.
	// GLuint iboId;
	// std::vector<unsigned int> vertexIndices;
	std::vector<SubMesh> subMeshes;

	int numVertices;
	int numTriangles;
	glm::vec3 objCenter;
	glm::vec3 objExtent;
};


#endif
