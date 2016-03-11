#include "Mesh.h"
#include <QOpenGLFunctions>

Mesh::Mesh() 
	:VBO(QOpenGLBuffer::VertexBuffer)
	,EBO(QOpenGLBuffer::IndexBuffer)
{
}

Mesh::Mesh(QVector<Vertex> vertices, QVector<GLuint> indices, QVector<Texture*> textures)
	:VBO(QOpenGLBuffer::VertexBuffer)
	,EBO(QOpenGLBuffer::IndexBuffer)
{
	//initializeOpenGLFunctions();
	this->Vertices = vertices;
	this->Indices = indices;
	this->Textures = textures;
	this->setupRender();
}


void Mesh::Draw(QOpenGLShaderProgram *program)
{
	// layout 0,1,2 correspond to position, normal, texture respectively
	

	VAO.bind();
	/*program->enableAttributeArray(0);
	program->enableAttributeArray(1);
	program->enableAttributeArray(2);*/
	
	//EBO.bind();
	/*program->setAttributeBuffer(0, GL_FLOAT, Vertex::positionOffset(), Vertex::PositionTupleSize, Vertex::stride());	
	program->setAttributeBuffer(1, GL_FLOAT, Vertex::normalOffset(), Vertex::NormalTupleSize, Vertex::stride());	
	program->setAttributeBuffer(2, GL_FLOAT, Vertex::textureOffset(), Vertex::TextureTupleSize, Vertex::stride());
*/
	// texture
	for (int i = 0; i < Textures.size(); i++)
	{
		Textures[i]->TextureId->bind(i);
		switch (Textures[i]->type)
		{
		case AmbientTexture: 
			program->setUniformValue("material.ambient", i);
			break;
		case DiffuseTexture:
			program->setUniformValue("material.diffuse", i);
			break;
		case SpecularTexture: 
			program->setUniformValue("material.specular", i);
			break;
		default:
			break;
		}
	}
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glDrawElements(GL_TRIANGLES, this->Indices.size() , GL_UNSIGNED_INT, 0);	
	glDrawElements(GL_TRIANGLES, this->Indices.size(), GL_UNSIGNED_INT, 0);
	
	VAO.release();	
}

void Mesh::setupRender()
{
	//int positionSize = Vertices.size() * sizeof(Q);
	VAO.create();
	VBO.create();	
	EBO.create();
	
	VAO.bind();
		
	VBO.bind();
	VBO.setUsagePattern(QOpenGLBuffer::StaticDraw);	
	VBO.allocate(Vertices.constData(), Vertices.size()*sizeof(Vertex));
		
	EBO.bind();
	EBO.setUsagePattern(QOpenGLBuffer::StaticDraw);
	EBO.allocate(Indices.constData(), Indices.size()*sizeof(GLuint));

	

	QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
	f->glEnableVertexAttribArray(0); // location = 0
	f->glVertexAttribPointer(0, Vertex::PositionTupleSize,GL_FLOAT,GL_FALSE, Vertex::stride(), (GLvoid*)Vertex::positionOffset());
	f->glEnableVertexAttribArray(1); // location = 0
	f->glVertexAttribPointer(1, Vertex::NormalTupleSize, GL_FLOAT, GL_FALSE, Vertex::stride(), (GLvoid*)Vertex::normalOffset());
	f->glEnableVertexAttribArray(2); // location = 0
	f->glVertexAttribPointer(2, Vertex::TextureTupleSize, GL_FLOAT, GL_FALSE, Vertex::stride(), (GLvoid*)Vertex::textureOffset());
		
	VBO.release();
	VAO.release();
}


