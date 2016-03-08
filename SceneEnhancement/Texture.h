#ifndef TEXTURE_H
#define TEXTURE_H
#include <QOpenGLTexture>
#include <QVector3D>
class Texture
{
public:
	Texture();
	QOpenGLTexture *TextureId;
	QVector3D DiffuseColor;
	float Transparency;


};

#endif

