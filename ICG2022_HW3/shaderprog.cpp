#include "shaderprog.h"

#define MAX_BUFFER_SIZE 1024

ShaderProg::ShaderProg()
{
    // Create OpenGL shader program.
    shaderProgId = glCreateProgram();
    if (shaderProgId == 0) {
        std::cerr << "[ERROR] Failed to create shader program" << std::endl;
        exit(1);
    }
    // locM = locV = locP = -1;
    locMVP = -1;
}

ShaderProg::~ShaderProg()
{
    glDeleteProgram(shaderProgId);
}

bool ShaderProg::LoadFromFiles(const std::string vsFilePath, const std::string fsFilePath)
{
    // Load the vertex shader from a source file and attach it to the shader program.
    std::string vs, fs;
    if (!LoadShaderTextFromFile(vsFilePath, vs)) {
        std::cerr << "[ERROR] Failed to load vertex shader source: " << vsFilePath << std::endl;
        return false;
    }
    GLuint vsId = AddShader(vs, GL_VERTEX_SHADER);

    // Load the fragment shader from a source file and attach it to the shader program.
    if (!LoadShaderTextFromFile(fsFilePath, fs)) {
        std::cerr << "[ERROR] Failed to load vertex shader source: " << fsFilePath << std::endl;
        return false;
    };
    GLuint fsId = AddShader(fs, GL_FRAGMENT_SHADER);

    // Link and compile shader programs.
    GLint success = 0;
    GLchar errorLog[MAX_BUFFER_SIZE] = { 0 };
    glLinkProgram(shaderProgId);
    glGetProgramiv(shaderProgId, GL_LINK_STATUS, &success);
    if (success == 0) {
        glGetProgramInfoLog(shaderProgId, sizeof(errorLog), NULL, errorLog);
        std::cerr << "[ERROR] Failed to link shader program: " <<  errorLog << std::endl;
        return false;
    }

    // Now the program already has all stage information, we can delete the shaders now.
    glDeleteShader(vsId);
    glDeleteShader(fsId);

    // Validate program.
    glValidateProgram(shaderProgId);
    glGetProgramiv(shaderProgId, GL_VALIDATE_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgId, sizeof(errorLog), NULL, errorLog);
        std::cerr << "[ERROR] Invalid shader program: " << errorLog << std::endl;
        return false;
    }

    // Update the location of uniform variables.
    GetUniformVariableLocation();

    return true;
}

void ShaderProg::GetUniformVariableLocation()
{
    locMVP = glGetUniformLocation(shaderProgId, "MVP");
}

GLuint ShaderProg::AddShader(const std::string& sourceText, GLenum shaderType)
{
    GLuint shaderObj = glCreateShader(shaderType);
    if (shaderObj == 0) {
        std::cerr << "[ERROR] Failed to create shader with type " << shaderType << std::endl;
        exit(0);
    }

    const GLchar* p[1];
    p[0] = sourceText.c_str();
    GLint lengths[1];
    lengths[0] = (GLint)(sourceText.length());
    glShaderSource(shaderObj, 1, p, lengths);
    glCompileShader(shaderObj);

    GLint success;
    glGetShaderiv(shaderObj, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[MAX_BUFFER_SIZE];
        glGetShaderInfoLog(shaderObj, MAX_BUFFER_SIZE, NULL, infoLog);
        std::cerr << "[ERROR] Failed to compile shader with type: " << shaderType << ". Info: " << infoLog << std::endl;
        exit(1);
    }

    glAttachShader(shaderProgId, shaderObj);

    return shaderObj;
}

bool ShaderProg::LoadShaderTextFromFile(const std::string filePath, std::string& sourceText)
{
    std::ifstream sourceFile(filePath.c_str());
    if(!sourceFile) {
        std::cerr << "[ERROR] Failed to open shader source file: " << filePath << std::endl;
        return false;
    }
    sourceText.assign((std::istreambuf_iterator< char >(sourceFile)),  std::istreambuf_iterator< char >());
    return true;
}

// ------------------------------------------------------------------------------------------------

FillColorShaderProg::FillColorShaderProg()
{
    locFillColor = -1;
}

FillColorShaderProg::~FillColorShaderProg()
{}

void FillColorShaderProg::GetUniformVariableLocation()
{
    ShaderProg::GetUniformVariableLocation();
    locFillColor = glGetUniformLocation(shaderProgId, "fillColor");
}

// ------------------------------------------------------------------------------------------------

PhongShadingDemoShaderProg::PhongShadingDemoShaderProg()
{
    locM = -1;
    locNM = -1;
    locCameraPos = -1;
    locKa = -1;
    locKd = -1;
    locKs = -1;
    locNs = -1;
    locAmbientLight = -1;
    locDirLightDir = -1;
	locDirLightRadiance = -1;
	locPointLightPos = -1;
	locPointLightIntensity = -1;
    // -------------------------------------------------------
	// Add your code for initializing the data of spot light.
	// -------------------------------------------------------
    locSpotLightPos = -1;
    locSpotLightIntensity = -1;
    locSpotLightDir = -1;
    locSpotLightCutOffStart = -1;
    locSpotLightTotalWidth = -1;
    // -------------------------------------------------------
	// Add your code for initializing the data of textures.
	// -------------------------------------------------------
    locMapKd = -1;
    lochaveMapKd = -1;
}

PhongShadingDemoShaderProg::~PhongShadingDemoShaderProg()
{}

void PhongShadingDemoShaderProg::GetUniformVariableLocation()
{
    ShaderProg::GetUniformVariableLocation();
    locM = glGetUniformLocation(shaderProgId, "worldMatrix");
    locNM = glGetUniformLocation(shaderProgId, "normalMatrix");
    locCameraPos = glGetUniformLocation(shaderProgId, "cameraPos");
    locKa = glGetUniformLocation(shaderProgId, "Ka");
    locKd = glGetUniformLocation(shaderProgId, "Kd");
    locKs = glGetUniformLocation(shaderProgId, "Ks");
    locNs = glGetUniformLocation(shaderProgId, "Ns");
    locAmbientLight = glGetUniformLocation(shaderProgId, "ambientLight");
    locDirLightDir = glGetUniformLocation(shaderProgId, "dirLightDir");
	locDirLightRadiance = glGetUniformLocation(shaderProgId, "dirLightRadiance");
	locPointLightPos = glGetUniformLocation(shaderProgId, "pointLightPos");
	locPointLightIntensity = glGetUniformLocation(shaderProgId, "pointLightIntensity");
    // -------------------------------------------------------
	// Add your code for getting the location of data of spot light.
	// -------------------------------------------------------
    locSpotLightPos = glGetUniformLocation(shaderProgId, "spotLightPos");
    locSpotLightIntensity = glGetUniformLocation(shaderProgId, "spotLightIntensity");
    locSpotLightDir = glGetUniformLocation(shaderProgId, "spotLightDir");
    locSpotLightCutOffStart = glGetUniformLocation(shaderProgId, "spotLightCutOffStart");
    locSpotLightTotalWidth = glGetUniformLocation(shaderProgId, "spotLightTotalWidth");
    // -------------------------------------------------------
	// Add your code for getting the location of texture variable.
	// -------------------------------------------------------
    locMapKd = glGetUniformLocation(shaderProgId, "mapKd");
    lochaveMapKd = glGetUniformLocation(shaderProgId, "havemapKd");
}

// ------------------------------------------------------------------------------------------------

SkyboxShaderProg::SkyboxShaderProg()
{
    locMapKd = -1;
}

SkyboxShaderProg::~SkyboxShaderProg()
{}

void SkyboxShaderProg::GetUniformVariableLocation()
{
    ShaderProg::GetUniformVariableLocation();
    locMapKd = glGetUniformLocation(shaderProgId, "mapKd");
}
