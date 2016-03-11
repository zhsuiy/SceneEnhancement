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
	Mesh();
	Mesh(QVector<Vertex> vertices, QVector<GLuint> indices, QVector<Texture*> textures);
	
	/* Mesh Data*/
	QVector<Vertex> Vertices;	// ��Ŷ�����Ϣ
	QVector<GLuint> Indices;	// ��Ŷ����index
	QVector<Texture*> Textures;	// ������е�texture
	
	void Draw(QOpenGLShaderProgram *program);	// ��mesh������
	
protected:
	/* Render data */
	QOpenGLBuffer VBO;	// vertex buffer object, ����ʵ�ʵ�����
	QOpenGLBuffer EBO;	// element buffer object, ����index
	QOpenGLVertexArrayObject VAO;	// vertex attribute object, ����Ķ�������	
	
	void setupRender();	// ��ʼ������buffer,���ݴ�Assimp��

};


#endif

