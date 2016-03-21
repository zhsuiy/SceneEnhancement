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
	QVector<Vertex> Vertices;	// 存放顶点信息
	QVector<GLuint> Indices;	// 存放顶点的index
	//QVector<Texture*> Textures;	// 存放所有的texture
	Material *MeshMaterial;
	void Draw(QOpenGLShaderProgram *program);	// 把mesh画出来
	void setupRender();	// 初始化各种buffer,数据从Assimp来
protected:
	/* Render data */
	QOpenGLBuffer VBO;	// vertex buffer object, 顶点实际的数据
	QOpenGLBuffer EBO;	// element buffer object, 顶点index
	QOpenGLVertexArrayObject VAO;	// vertex attribute object, 定义的顶点属性	
	
	
	void updateNormals(); // 更新顶点法向量

};


#endif

