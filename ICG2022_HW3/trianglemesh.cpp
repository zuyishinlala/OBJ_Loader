#include "trianglemesh.h"

// Constructor of a triangle mesh.
TriangleMesh::TriangleMesh()
{
	numVertices = 0;
	numTriangles = 0;
	objCenter = glm::vec3(0.0f, 0.0f, 0.0f);
	objExtent = glm::vec3(0.0f, 0.0f, 0.0f);
}

// Destructor of a triangle mesh.
TriangleMesh::~TriangleMesh()
{
	for (int i = 0; i < subMeshes.size(); ++i) {
		if(subMeshes[i].material->GetMapKd() != nullptr) delete subMeshes[i].material->GetMapKd();
		delete subMeshes[i].material;
		subMeshes[i].material = nullptr;
		subMeshes[i].vertexIndices.clear();
		glDeleteBuffers(1, &subMeshes[i].iboId);
	}
	glDeleteBuffers(1, &vboId);
	vertices.clear();
	subMeshes.clear();
	numTriangles = 0;
	numVertices = 0;
}

// used for tuple hashing
struct key_hash : public std::unary_function<Index, std::size_t>
{
	std::size_t operator()(const Index& k) const
	{
		return std::get<0>(k) ^ std::get<1>(k) ^ std::get<2>(k);
	}
};

// used for tuple hashing
struct key_equal : public std::binary_function<Index, Index, bool>
{
	bool operator()(const Index& v0, const Index& v1) const
	{
		return (
			std::get<0>(v0) == std::get<0>(v1) &&
			std::get<1>(v0) == std::get<1>(v1) &&
			std::get<2>(v0) == std::get<2>(v1)
			);
	}
};

struct compare {
	bool operator()(p_fui const& p1, p_fui const& p2) {
		return p1.first < p2.first;
	}
};

// Load the geometry and material data from an OBJ file.
bool TriangleMesh::LoadFromFile(const std::string& filePath, const bool normalized)
{	
	// Parse the OBJ file.
	// ---------------------------------------------------------------------------
    // Add your implementation here (HW1 + read *.MTL).
    // ---------------------------------------------------------------------------
	std::ifstream OBJfile(filePath, std::ios::in);
	glm::vec3 maxvalue(-1.0f, -1.0f, -1.0f);
	glm::vec3 minvalue(1000.0f, 1000.0f, 1000.0f);
	if (OBJfile) {
		std::vector<glm::vec3> allVertex;
		std::vector<glm::vec2> allTexture;
		std::vector<glm::vec3> allNormal;
		std::string s;
		std::vector<PhongMaterial*> Materials;
		std::unordered_map<Index, int, key_hash, key_equal> mapV;
		int MaterialIndex = -1;
		while (std::getline(OBJfile, s)) {
			std::string identity = "";
			std::istringstream iss(s);
			iss >> identity;
			if (identity == "v") {		//vertex 
				float f1, f2, f3;
				iss >> f1 >> f2 >> f3;
				glm::vec3 v(f1, f2, f3);
				findmaxmin(maxvalue, minvalue, v);
				allVertex.push_back(v);
			}
			else if (identity == "vt") { //texture
				float f1, f2;
				iss >> f1 >> f2;
				glm::vec2 v(f1, f2);
				allTexture.push_back(v);
			}
			else if (identity == "vn") { //normal
				float f1, f2, f3;
				iss >> f1 >> f2 >> f3;
				glm::vec3 v(f1, f2, f3);
				allNormal.push_back(v);
			}
			else if (identity == "f") { //triangles
				std::vector<unsigned int> cur;
				std::string subs;
				int VertexCount = 0;
				while (iss >> subs)
				{
					int a, b, c;
					sscanf_s(subs.c_str(), "%d/%d/%d", &a, &b, &c);
					a = abs(a);
					b = abs(b);
					c = abs(c);
					auto id = mapV.find(std::make_tuple(--a, --c, --b));
					if (id != mapV.end()) {
						cur.push_back(mapV[std::make_tuple(a, c, b)]);
					}
					else {
						VertexPTN newV(allVertex[a], allNormal[c], allTexture[b]);
						mapV[std::make_tuple(a, c, b)] = numVertices++;
						vertices.push_back(newV);
						cur.push_back(mapV[std::make_tuple(a, c, b)]);
					}
					++VertexCount;
				}
				sortVertices(cur, float(cur.size()));
				for (int i = 1; i < VertexCount - 1; ++i) {
					subMeshes[MaterialIndex].vertexIndices.push_back(cur[0]);
					subMeshes[MaterialIndex].vertexIndices.push_back(cur[i]);
					subMeshes[MaterialIndex].vertexIndices.push_back(cur[i + 1]);
					++numTriangles;
				}
			}
			else if (identity == "mtllib") {
				std::string materialFile = "";
				iss >> materialFile;
				LoadMaterialFile(materialFile, filePath, Materials);
			}
			else if (identity == "usemtl") {
				std::string MaterialName = "";
				iss >> MaterialName;
				MaterialIndex = FindMaterialIdx(MaterialName, Materials);
				if (MaterialIndex == -1) {
					std::cout << "After .mtl file read, Material Not Found " << std::endl;
					break;
				}
			}
		}
	}
	else { std::cout << "File not opened" << std::endl; }
	// Normalize the geometry data.
	float scal_p = 1;
	glm::vec3 Center = (maxvalue + minvalue) * 0.5f; // calculate !normalized OBJ Center
	if (normalized) {
		float maxExtend = -1.0f;
		glm::vec3 extent = maxvalue - minvalue;
		maxExtend = std::max(extent.x, maxExtend);
		maxExtend = std::max(extent.y, maxExtend);
		maxExtend = std::max(extent.z, maxExtend);
		scal_p = 1 / maxExtend;
		glm::mat4x4 gT = glm::translate(glm::mat4x4(1.0f), -Center);
		glm::mat4x4 gS = glm::scale(glm::mat4x4(1.0f), glm::vec3(scal_p, scal_p, scal_p));
		glm::mat4x4 gF = gS * gT;
		for (auto& e : vertices) {
			glm::vec4 v(e.position, 1);
			v = gF * v;
			e.position = glm::vec3(v);
		}
		Center = glm::vec3(0.0f, 0.0f, 0.0f); // Center becomes (0,0,0) if normalized
	}
	objCenter = Center;
	objExtent = (maxvalue - minvalue) * scal_p;
	OBJfile.close();
	createVertexBuffer();
	createIndexBuffer();
	return true;
}


void TriangleMesh::LoadMaterialFile(std::string MaterialName, std::string filePath, std::vector<PhongMaterial*>& Materials) {
	std::string MaterialPath = filePath.substr(0, filePath.find_last_of('/')) + "/" + MaterialName;
	std::ifstream fileMat(MaterialPath, std::ios::in);
	PhongMaterial* newMaterial = nullptr;
	if (fileMat) {
		std::string s;
		while (std::getline(fileMat, s)) {
			//std::cout << s << std::endl;
			std::string identity = "";
			std::istringstream iss(s);
			iss >> identity;
			if (identity == "newmtl") {
				if(newMaterial != nullptr) Materials.push_back(newMaterial);
				newMaterial = new PhongMaterial();
				std::string MatName = "";
				iss >> MatName;
				newMaterial->SetName(MatName);
			}
			else if (identity == "Ns") {
				float Ns;
				iss >> Ns;
				newMaterial->SetNs(Ns);
			}
			else if (identity == "Ka") {
				glm::vec3 var(0.0f, 0.0f, 0.0f);
				iss >> var.x >> var.y >> var.z;
				newMaterial->SetKa(var);
			}
			else if (identity == "Kd") {
				glm::vec3 var(0.0f, 0.0f, 0.0f);
				iss >> var.x >> var.y >> var.z;
				newMaterial->SetKd(var);
			}
			else if (identity == "Ks") {
				glm::vec3 var(0.0f, 0.0f, 0.0f);
				iss >> var.x >> var.y >> var.z;
				newMaterial->SetKs(var);
			}
			else if (identity == "map_Kd") {
				std::string TextureName = "";
				iss >> TextureName;
				std::string TextureFilePath = filePath.substr(0, filePath.find_last_of('/')) + "/" + TextureName;
				ImageTexture* texture = new ImageTexture(TextureFilePath);
				//texture->Preview();
				newMaterial->SetMapKd(texture);
			}
		}
	}
	else { std::cout << ".mtl File Not Found :( " << std::endl; }
	if(newMaterial != nullptr) Materials.push_back(newMaterial);
	subMeshes = std::vector<SubMesh>(Materials.size());
	for (int i = 0; i < Materials.size() ; ++i) {
		subMeshes[i].material = Materials[i];
	}
	fileMat.close();
}
int TriangleMesh::FindMaterialIdx(std::string Name, std::vector<PhongMaterial*>& Materials) {
	for (int i = 0; i < Materials.size(); ++i) {
		if (Name == Materials[i]->GetName()) {
			return i;
		}
	}
	return -1;
}

void TriangleMesh::sortVertices(std::vector<unsigned int>& cur, float size) {
	if (size == 3) return;
	std::priority_queue<p_fui, std::vector<p_fui>, compare> pq;
	glm::vec3 center(0.0f, 0.0f, 0.0f);
	for (auto e : cur) {
		center.x += vertices[e].position.x;
		center.y += vertices[e].position.y;
		center.z += vertices[e].position.z;
	}
	center /= size;
	glm::vec3 x_axis = vertices[cur[0]].position - center;
	glm::vec3 y_axis = vertices[cur[1]].position - center; // temporary, not real y_axis
	glm::vec3 cur_normal = glm::cross(x_axis, y_axis); // polygon normal 
	y_axis = glm::cross(cur_normal, x_axis); // real y axis at current polygon
	x_axis = glm::normalize(x_axis);
	y_axis = glm::normalize(y_axis);
	for (int i = 0; i < int(size); ++i) {
		glm::vec3 dist = vertices[cur[i]].position - center;
		dist = glm::normalize(dist);
		float x_dot = glm::dot(dist, x_axis);
		float y_dot = glm::dot(dist, y_axis);
		y_dot < 0 ? x_dot = -x_dot : x_dot += 10;
		pq.push(std::make_pair(x_dot, cur[i]));
	}
	int index = 0;
	while (!pq.empty()) {
		p_fui top = pq.top();
		cur[index++] = top.second;
		pq.pop();
	}
}

void TriangleMesh::findmaxmin(glm::vec3& maxval, glm::vec3& minval, glm::vec3 cur) {
	maxval.x = std::max(maxval.x, cur.x);
	maxval.y = std::max(maxval.y, cur.y);
	maxval.z = std::max(maxval.z, cur.z);
	minval.x = std::min(minval.x, cur.x);
	minval.y = std::min(minval.y, cur.y);
	minval.z = std::min(minval.z, cur.z);
}

void TriangleMesh::createVertexBuffer() {
	glGenBuffers(1, &vboId);
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexPTN), &vertices.front(), GL_STATIC_DRAW);
}
void TriangleMesh::createIndexBuffer() {
	for (int i = 0; i < subMeshes.size(); ++i) {
		glGenBuffers(1, &subMeshes[i].iboId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, subMeshes[i].iboId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, subMeshes[i].vertexIndices.size() * sizeof(unsigned int), &subMeshes[i].vertexIndices.front(), GL_STATIC_DRAW);
	}
}
void TriangleMesh::Draw(int index) {
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPTN), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPTN), (const GLvoid*)12);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPTN), (const GLvoid*)24);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, subMeshes[index].iboId);
	glDrawElements(GL_TRIANGLES, GLsizei(subMeshes[index].vertexIndices.size()), GL_UNSIGNED_INT, 0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

void TriangleMesh::SetMaterialUniform(PhongShadingDemoShaderProg*& phongShadingShader, int index) {
	SubMesh& g = subMeshes[index];
	glUniform3fv(phongShadingShader->GetLocKa(), 1, glm::value_ptr(g.material->GetKa()));
	glUniform3fv(phongShadingShader->GetLocKd(), 1, glm::value_ptr(g.material->GetKd()));
	glUniform3fv(phongShadingShader->GetLocKs(), 1, glm::value_ptr(g.material->GetKs()));
	glUniform1f(phongShadingShader->GetLocNs(), g.material->GetNs());
	if (g.material->GetMapKd() != nullptr) {
		// bind texture
		g.material->GetMapKd()->Bind(GL_TEXTURE0);
		glUniform1i(phongShadingShader->GetLocMapKd(), 0);
		//boolean uniform , if have texture than set flag true
		glUniform1i(phongShadingShader->GethaveLocMapKd(), true);
	}
	else {
		//boolean uniform, if !have texture than set flag false
		glUniform1i(phongShadingShader->GethaveLocMapKd(), false); 
	}
}
// Show model information.
void TriangleMesh::ShowInfo()
{
	std::cout << "# Vertices: " << numVertices << std::endl;
	std::cout << "# Triangles: " << numTriangles << std::endl;
	std::cout << "Total " << subMeshes.size() << " subMeshes loaded" << std::endl;
	for (unsigned int i = 0; i < subMeshes.size(); ++i) {
		const SubMesh& g = subMeshes[i];
		std::cout << "SubMesh " << i << " with material: " << g.material->GetName() << std::endl;
		std::cout << "Num. triangles in the subMesh: " << g.vertexIndices.size() / 3 << std::endl;
	}
	std::cout << "Model Center: " << objCenter.x << ", " << objCenter.y << ", " << objCenter.z << std::endl;
	std::cout << "Model Extent: " << objExtent.x << " x " << objExtent.y << " x " << objExtent.z << std::endl;
}

