#include "BoundObject.h"

//BoundObject::BoundObject(QVector<Vertex> vertices, QVector<GLuint> indices, QVector<Texture*> textures)
//	: Mesh(vertices,indices,textures)
//{
//
//}

BoundObject::BoundObject(QVector<Vertex> vertices, QVector<GLuint> indices, Material *material)
	: Mesh(vertices, indices, material)
{

}