#ifndef TEXTURE_H
#define TEXTURE_H
#include <QOpenGLTexture>
#include <QVector3D>
#include <3rdparty/include/assimp/types.h>

enum TextureType
{
	AmbientTexture,
	DiffuseTexture,
	SpecularTexture
};
class Texture
{
public:
	Texture();
	QOpenGLTexture *TextureId;
	TextureType type;
	aiString path;
	float Transparency;


};

#endif

