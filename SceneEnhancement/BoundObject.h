#ifndef BOUND_OBJECT_H
#define BOUND_OBJECT_H

#include "Mesh.h"

class BoundObject:public Mesh
{
public:
	BoundObject(QVector<Vertex> vertices, QVector<GLuint> indices, Material *material);

private:

};


#endif

