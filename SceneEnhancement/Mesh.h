#ifndef MESH_H
#define MESH_H

#include <QtCore/qvector.h>
#include <QtWidgets/QAction>
#include "Vertex.h"
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include "Texture.h"
#include "Material.h"

class Shader;

class Mesh 
{
public:
	/* Functions */
	Mesh();
	//Mesh(QVector<Vertex> vertices, QVector<GLuint> indices, QVector<Texture*> textures);
	Mesh(QVector<Vertex> vertices, QVector<GLuint> indices, Material *material);
	
	/* Mesh Data*/
	QVector<Vertex> Vertices;	// ��Ŷ�����Ϣ
	QVector<GLuint> Indices;	// ��Ŷ����index
	//QVector<Texture*> Textures;	// ������е�texture
	Material *MeshMaterial;
	void Draw(QOpenGLShaderProgram *program);	// ��mesh������
	void setupRender();	// ��ʼ������buffer,���ݴ�Assimp��
protected:
	/* Render data */
	QOpenGLBuffer VBO;	// vertex buffer object, ����ʵ�ʵ�����
	QOpenGLBuffer EBO;	// element buffer object, ����index
	QOpenGLVertexArrayObject VAO;	// vertex attribute object, ����Ķ�������	
	
	
	void updateNormals(); // ���¶��㷨����

};


#endif

