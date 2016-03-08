#include "Mesh.h"

Mesh::Mesh(QVector<Vertex> vertices, QVector<GLuint> indices, QVector<Texture> textures)
	:VBO(QOpenGLBuffer::VertexBuffer)
	,EBO(QOpenGLBuffer::IndexBuffer)
{
	this->Vertices = vertices;
	this->Indices = indices;
	this->Textures = textures;
	this->setupMesh();
}


void Mesh::Draw(QOpenGLShaderProgram *program)
{
	program->bind();
	VAO.bind();
	EBO.bind();

	// layout 0,1,2 correspond to position, normal, texture respectively
	program->enableAttributeArray(0);
	program->setAttributeBuffer(0, GL_FLOAT, Vertex::positionOffset(), Vertex::PositionTupleSize, Vertex::stride());
	program->enableAttributeArray(1);
	program->setAttributeBuffer(1, GL_FLOAT, Vertex::normalOffset(), Vertex::NormalTupleSize, Vertex::stride());
	program->enableAttributeArray(1);
	program->setAttributeBuffer(2, GL_FLOAT, Vertex::textureOffset(), Vertex::TextureTupleSize, Vertex::stride());

	// texture unprocessed...


	glDrawElements(GL_TRIANGLES, this->Indices.size() , GL_UNSIGNED_INT, 0);
	VAO.release();
	program->release();
	
}

void Mesh::setupMesh()
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
	EBO.allocate(Vertices.constData(), Indices.size()*sizeof(GLuint));

	VBO.release();
	VAO.release();

}

void Mesh::setupShaders()
{
	 QString vShaderSrc("#version 330\n"
    "layout(location = 0) in vec3 position;\n"
    "layout(location = 1) in vec3 normal;\n"
		 
    "layout(location = 2) in mat4 matrix;\n"
    "smooth out vec4 col;\n"
    "void main() {\n"
    "   col = colour;\n"
    "   gl_Position = matrix * position;\n"
    "}\n");

  QString fShaderSrc("#version 330\n"
    "smooth in vec4 col;\n"
    "void main() {\n"
    "   gl_FragColor = col;\n"
    "}\n");

}
