#ifndef IMAGE_TEXTURE_H
#define IMAGE_TEXTURE_H

#include "headers.h"

// Texture Declarations.
class ImageTexture
{
public:
	// Texture Public Methods.
	ImageTexture(const std::string filePath);
	~ImageTexture();

	void Bind(GLenum textureUnit);
	void Preview();
	std::string GetPath() const { return texFilePath; }

private:
	// Texture Private Data.
	std::string texFilePath;
	GLuint textureObj;
	int imageWidth;
	int imageHeight;
	int numChannels;
	cv::Mat texImage;
};

#endif

