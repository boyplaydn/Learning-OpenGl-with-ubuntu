#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <iostream>
#include "lodepng.hpp"


GLuint loadBMP_custom(const char * imagepath){

	printf("Reading image %s\n", imagepath);

	// Data read from the header of the BMP file
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	unsigned int width, height;
	// Actual RGB data
	unsigned char * data;

	// Open the file
	FILE * file = fopen(imagepath,"rb");
	if (!file){
		printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath);
		getchar();
		return 0;
	}

	// Read the header, i.e. the 54 first bytes

	// If less than 54 bytes are read, problem
	if ( fread(header, 1, 54, file)!=54 ){ 
		printf("Not a correct BMP file\n");
		fclose(file);
		return 0;
	}
	// A BMP files always begins with "BM"
	if ( header[0]!='B' || header[1]!='M' ){
		printf("Not a correct BMP file\n");
		fclose(file);
		return 0;
	}
	// Make sure this is a 24bpp file
	if ( *(int*)&(header[0x1E])!=0  )         {printf("Not a correct BMP file\n");    fclose(file); return 0;}
	if ( *(int*)&(header[0x1C])!=24 )         {printf("Not a correct BMP file\n");    fclose(file); return 0;}

	// Read the information about the image
	dataPos    = *(int*)&(header[0x0A]);
	imageSize  = *(int*)&(header[0x22]);
	width      = *(int*)&(header[0x12]);
	height     = *(int*)&(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if (imageSize==0)    imageSize=width*height*3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos==0)      dataPos=54; // The BMP header is done that way

	// Create a buffer
	data = new unsigned char [imageSize];

	// Read the actual data from the file into the buffer
	fread(data,1,imageSize,file);

	// Everything is in memory now, the file can be closed.
	fclose (file);

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);
	
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

	// OpenGL has now copied the data. Free our own version
	delete [] data;

	// Poor filtering, or ...
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 

	// ... nice trilinear filtering ...
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	// ... which requires mipmaps. Generate them automatically.
	glGenerateMipmap(GL_TEXTURE_2D);

	// Return the ID of the texture we just created
	return textureID;
}

// Since GLFW 3, glfwLoadTexture2D() has been removed. You have to use another texture loading library, 
// or do it yourself (just like loadBMP_custom and loadDDS)
//GLuint loadTGA_glfw(const char * imagepath){
//
//	// create one opengl texture
//	gluint textureid;
//	glgentextures(1, &textureid);
//
//	// "bind" the newly created texture : all future texture functions will modify this texture
//	glbindtexture(gl_texture_2d, textureid);
//
//	// read the file, call glteximage2d with the right parameters
//	glfwloadtexture2d(imagepath, 0);
//
//	// nice trilinear filtering.
//	gltexparameteri(gl_texture_2d, gl_texture_wrap_s, gl_repeat);
//	gltexparameteri(gl_texture_2d, gl_texture_wrap_t, gl_repeat);
//	gltexparameteri(gl_texture_2d, gl_texture_mag_filter, gl_linear);
//	gltexparameteri(gl_texture_2d, gl_texture_min_filter, gl_linear_mipmap_linear); 
//	glgeneratemipmap(gl_texture_2d);
// Return the ID of the texture we just created
	//return textureID;
//}



#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

GLuint loadDDS(const char * imagepath){

	unsigned char header[124];

	FILE *fp; 
 
	/* try to open the file */ 
	fp = fopen(imagepath, "rb"); 
	if (fp == NULL){
		printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath); getchar(); 
		return 0;
	}
   
	/* verify the type of file */ 
	char filecode[4]; 
	fread(filecode, 1, 4, fp); 
	if (strncmp(filecode, "DDS ", 4) != 0) { 
		fclose(fp); 
		return 0; 
	}
	
	/* get the surface desc */ 
	fread(&header, 124, 1, fp); 

	unsigned int height      = *(unsigned int*)&(header[8 ]);
	unsigned int width	     = *(unsigned int*)&(header[12]);
	unsigned int linearSize	 = *(unsigned int*)&(header[16]);
	unsigned int mipMapCount = *(unsigned int*)&(header[24]);
	unsigned int fourCC      = *(unsigned int*)&(header[80]);

 
	unsigned char * buffer;
	unsigned int bufsize;
	/* how big is it going to be including all mipmaps? */ 
	bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize; 
	buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char)); 
	fread(buffer, 1, bufsize, fp); 
	/* close the file pointer */ 
	fclose(fp);

	unsigned int components  = (fourCC == FOURCC_DXT1) ? 3 : 4; 
	unsigned int format;
	switch(fourCC) 
	{ 
	case FOURCC_DXT1: 
		format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT; 
		break; 
	case FOURCC_DXT3: 
		format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT; 
		break; 
	case FOURCC_DXT5: 
		format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT; 
		break; 
	default: 
		free(buffer); 
		return 0; 
	}

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);	
	
	unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16; 
	unsigned int offset = 0;

	/* load the mipmaps */ 
	for (unsigned int level = 0; level < mipMapCount && (width || height); ++level) 
	{ 
		unsigned int size = ((width+3)/4)*((height+3)/4)*blockSize; 
		glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,  
			0, size, buffer + offset); 
	 
		offset += size; 
		width  /= 2; 
		height /= 2; 

		// Deal with Non-Power-Of-Two textures. This code is not included in the webpage to reduce clutter.
		if(width < 1) width = 1;
		if(height < 1) height = 1;

	} 

	free(buffer); 

	return textureID;


}




// load TGA type

typedef struct
{
	unsigned char  identsize;			// size of ID field that follows 18 byte header (0 usually)
	unsigned char  colourmaptype;		// type of colour map 0=none, 1=has palette
	unsigned char  imagetype;			// type of image 2=rgb uncompressed, 10 - rgb rle compressed

	short colourmapstart;				// first colour map entry in palette
	short colourmaplength;				// number of colours in palette
	//unsigned char  colourmapbits;		// number of bits per palette entry 15,16,24,32

	short xstart;						// image x origin
	short ystart;						// image y origin
	short width;						// image width in pixels
	short height;						// image height in pixels
	unsigned char  bits;				// image bits per pixel 24,32
	unsigned char  descriptor;			// image descriptor bits (vh flip bits)

	// pixel data follows header

} TGA_HEADER;


const int IT_COMPRESSED = 10;
const int IT_UNCOMPRESSED = 2;

void LoadCompressedImage(char* pDest, char * pSrc, TGA_HEADER * pHeader)
{
	int w = pHeader->width;
	int h = pHeader->height;
	int rowSize = w * pHeader->bits / 8;
	bool bInverted = ((pHeader->descriptor & (1 << 5)) != 0);
	char * pDestPtr = bInverted ? pDest + (h + 1) * rowSize : pDest;
	int countPixels = 0;
	int nPixels = w * h;

	while (nPixels > countPixels)
	{
		unsigned char chunk = *pSrc++;
		if (chunk < 128)
		{
			int chunkSize = chunk + 1;
			for (int i = 0; i < chunkSize; i++)
			{
				if (bInverted && (countPixels % w) == 0)
					pDestPtr -= 2 * rowSize;
				*pDestPtr++ = pSrc[2];
				*pDestPtr++ = pSrc[1];
				*pDestPtr++ = pSrc[0];
				pSrc += 3;
				if (pHeader->bits != 24)
					*pDestPtr++ = *pSrc++;
				countPixels++;
			}
		}
		else
		{
			int chunkSize = chunk - 127;
			for (int i = 0; i < chunkSize; i++)
			{
				if (bInverted && (countPixels % w) == 0)
					pDestPtr -= 2 * rowSize;
				*pDestPtr++ = pSrc[2];
				*pDestPtr++ = pSrc[1];
				*pDestPtr++ = pSrc[0];
				if (pHeader->bits != 24)
					*pDestPtr++ = pSrc[3];
				countPixels++;
			}
			pSrc += (pHeader->bits >> 3);
		}
	}
}

void LoadUncompressedImage(char* pDest, char * pSrc, TGA_HEADER * pHeader)
{
	int w = pHeader->width;
	int h = pHeader->height;
	int rowSize = w * pHeader->bits / 8;
	bool bInverted = ((pHeader->descriptor & (1 << 5)) != 0);
	for (int i = 0; i < h; i++)
	{
		char * pSrcRow = pSrc +
			(bInverted ? (h - i - 1) * rowSize : i * rowSize);
		if (pHeader->bits == 24)
		{
			for (int j = 0; j < w; j++)
			{
				*pDest++ = pSrcRow[2];
				*pDest++ = pSrcRow[1];
				*pDest++ = pSrcRow[0];
				pSrcRow += 3;
			}
		}
		else
		{
			for (int j = 0; j < w; j++)
			{
				*pDest++ = pSrcRow[2];
				*pDest++ = pSrcRow[1];
				*pDest++ = pSrcRow[0];
				*pDest++ = pSrcRow[3];
				pSrcRow += 4;
			}
		}
	}
}


char * loadTGA(const char * szFileName, int * width, int * height, int * bpp)
{

	FILE * f;

	if (fopen(szFileName, "rb") != 0)
		return NULL;

	TGA_HEADER header;
	fread(&header, sizeof(header), 1, f);

	fseek(f, 0, SEEK_END);
	int fileLen = ftell(f);
	fseek(f, sizeof(header) + header.identsize, SEEK_SET);

	if (header.imagetype != IT_COMPRESSED && header.imagetype != IT_UNCOMPRESSED)
	{
		fclose(f);
		return NULL;
	}

	if (header.bits != 24 && header.bits != 32)
	{
		fclose(f);
		return NULL;
	}

	int bufferSize = fileLen - sizeof(header) - header.identsize;
	char * pBuffer = new char[bufferSize];
	fread(pBuffer, 1, bufferSize, f);
	fclose(f);

	*width = header.width;
	*height = header.height;
	*bpp = header.bits;
	char * pOutBuffer = new char[header.width * header.height * header.bits / 8];

	switch (header.imagetype)
	{
	case IT_UNCOMPRESSED:
		LoadUncompressedImage(pOutBuffer, pBuffer, &header);
		break;
	case IT_COMPRESSED:
		LoadCompressedImage(pOutBuffer, pBuffer, &header);
		break;
	}

	delete[] pBuffer;

	return pOutBuffer;
}
GLuint loadPNG(const char * szFileNam, int wrapping_mode)
{
	// Load file and decode image.
	unsigned width, height;
	std::vector<unsigned char> image;
	unsigned error = lodepng::decode(image, width, height, szFileNam);

	// If there's an error, display it.
	if (error != 0) {
		std::cout << "error " << error << ": " << lodepng_error_text(error) << std::endl;
		return 0;
	}
	
	// Texture size must be power of two for the primitive OpenGL version this is written for. Find next power of two.
	size_t u2 = 1; while (u2 < width) u2 *= 2;
	size_t v2 = 1; while (v2 < height) v2 *= 2;
	// Ratio for power of two version compared to actual version, to render the non power of two image with proper size.
	double u3 = (double)width / u2;
	double v3 = (double)height / v2;

	// Make power of two version of the image.
	std::vector<unsigned char> image2(u2 * v2 * 4);
	for (size_t y = 0; y < height; y++)
		for (size_t x = 0; x < width; x++)
			for (size_t c = 0; c < 4; c++) {
				image2[4 * u2 * y + 4 * x + c] = image[4 * width * y + 4 * x + c];
			}
	//std::reverse(image.begin(), image.end());
	//std::vector<unsigned char> image3(width*height);

	//std::reverse(image2.begin(), image2.end());
	// ... nice trilinear filtering ...
	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);
	if(wrapping_mode == 1) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	if(wrapping_mode == 2) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	// ... which requires mipmaps. Generate them automatically.
	glGenerateMipmap(GL_TEXTURE_2D);

	// Return the ID of the texture we just created
	return textureID;
}
GLuint createTeture2D(const char * imagepath)
{
	// read image file
	//unsigned width, height;
	int * width = new int();
	int *height = new int();
	int *bpp = new int();
	//std::vector<unsigned char> data = loadPNG(imagepath, width, height);
	char * data = loadTGA(imagepath, width, height, bpp);
	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Give the image to OpenGL
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, *width, *height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	// OpenGL has now copied the data. Free our own version
	//delete[] data;

	// Poor filtering, or ...
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 

	// ... nice trilinear filtering ...
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	// ... which requires mipmaps. Generate them automatically.
	glGenerateMipmap(GL_TEXTURE_2D);

	// Return the ID of the texture we just created
	return textureID;
}

GLuint loadPNG_CubeMap(char * faces [6])
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	// Load file and decode image.
	unsigned width, height;
	for (unsigned int i = 0; i < 6; i++) {
		std::vector<unsigned char> image;
		unsigned error = lodepng::decode(image, width, height, faces[i]);

		// If there's an error, display it.
		if (error != 0) {
			std::cout << "error " << error << ": " << lodepng_error_text(error) << std::endl;
			return 0;
		}
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);
	
	
		// Texture size must be power of two for the primitive OpenGL version this is written for. Find next power of two.
		size_t u2 = 1; while (u2 < width) u2 *= 2;
		size_t v2 = 1; while (v2 < height) v2 *= 2;
		// Ratio for power of two version compared to actual version, to render the non power of two image with proper size.
		double u3 = (double)width / u2;
		double v3 = (double)height / v2;

		// Make power of two version of the image.
		std::vector<unsigned char> image2(u2 * v2 * 4);
		for (size_t y = 0; y < height; y++)
			for (size_t x = 0; x < width; x++)
				for (size_t c = 0; c < 4; c++) {
					image2[4 * u2 * y + 4 * x + c] = image[4 * width * y + 4 * x + c];
				}
	}
	//std::reverse(image.begin(), image.end());
	//std::vector<unsigned char> image3(width*height);

	//std::reverse(image2.begin(), image2.end());
	// ... nice trilinear filtering ...
	// Create one OpenGL texture
	// GLuint textureID;
	// glGenTextures(1, &textureID);

	// // "Bind" the newly created texture : all future texture functions will modify this texture
	// glBindTexture(GL_TEXTURE_2D, textureID);
	
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	// ... which requires mipmaps. Generate them automatically.

	// Return the ID of the texture we just created
	return textureID;
}