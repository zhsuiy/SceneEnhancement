#ifndef MESH_H
#define MESH_H

#include <QtCore/qvector.h>
#include <QtWidgets/QAction>
#include "Vertex.h"
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include "Texture.h"

class Shader;

class Mesh 
{
public:
	/* Functions */
	Mesh(QVector<Vertex> vertices, QVector<GLuint> indices, QVector<Texture*> textures);

	/* Mesh Data*/
	QVector<Vertex> Vertices;	// 存放顶点信息
	QVector<GLuint> Indices;	// 存放顶点的index
	QVector<Texture*> Textures;	// 存放所有的texture
	
	void Draw(QOpenGLShaderProgram *program);	// 把mesh画出来
	
private:
	/* Render data */
	QOpenGLBuffer VBO;	// vertex buffer object, 顶点实际的数据
	QOpenGLBuffer EBO;	// element buffer object, 顶点index
	QOpenGLVertexArrayObject VAO;	// vertex attribute object, 定义的顶点属性	
	
	void setupMesh();	// 初始化各种buffer,数据从Assimp来
	void setupShaders();
};


#endif

