#include "Mesh.h"
#include <QOpenGLFunctions>

Mesh::Mesh()
	:VBO(QOpenGLBuffer::VertexBuffer)
	, EBO(QOpenGLBuffer::IndexBuffer)
{
}

Mesh::Mesh(QVector<Vertex> vertices, QVector<GLuint> indices, Material *material/*QVector<Texture*> textures*/)
	: VBO(QOpenGLBuffer::VertexBuffer)
	, EBO(QOpenGLBuffer::IndexBuffer)
{
	//initializeOpenGLFunctions();
	this->Vertices = vertices;
	this->Indices = indices;
	//this->Textures = textures;
	this->MeshMaterial = material;
	this->setupRender();
}


void Mesh::Draw(QOpenGLShaderProgram *program)
{
	// layout 0,1,2 correspond to position, normal, texture respectively
	
	VAO.bind();
	
	program->setUniformValue("material.useAmbientMap", MeshMaterial->Ambient->UseMap);
	program->setUniformValue("material.useDiffuseMap", MeshMaterial->Diffuse->UseMap);
	program->setUniformValue("material.useSpecularMap", MeshMaterial->Specular->UseMap);
	program->setUniformValue("material.ambientColor", MeshMaterial->Ambient->Color);
	program->setUniformValue("material.diffuseColor", MeshMaterial->Diffuse->Color);
	program->setUniformValue("material.specularColor", MeshMaterial->Specular->Color);
	program->setUniformValue("material.shininess", MeshMaterial->Shininess);

	for (size_t i = 0; i < MeshMaterial->Ambient->Textures.size(); i++)
	{
		MeshMaterial->Ambient->Textures[i]->Bind();
		program->setUniformValue("material.ambient", MeshMaterial->Ambient->Textures[i]->id);
	}
	for (size_t i = 0; i < MeshMaterial->Diffuse->Textures.size(); i++)
	{
		MeshMaterial->Diffuse->Textures[i]->Bind();
		program->setUniformValue("material.diffuse", MeshMaterial->Diffuse->Textures[i]->id);
	}
	for (size_t i = 0; i < MeshMaterial->Specular->Textures.size(); i++)
	{
		MeshMaterial->Specular->Textures[i]->Bind();
		program->setUniformValue("material.specular", MeshMaterial->Specular->Textures[i]->id);
	}	

// texture
	/*for (int i = 0; i < Textures.size(); i++)
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
	}*/

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
	f->glVertexAttribPointer(0, Vertex::PositionTupleSize, GL_FLOAT, GL_FALSE, Vertex::stride(), (GLvoid*)Vertex::positionOffset());
	f->glEnableVertexAttribArray(1); // location = 1
	f->glVertexAttribPointer(1, Vertex::NormalTupleSize, GL_FLOAT, GL_FALSE, Vertex::stride(), (GLvoid*)Vertex::normalOffset());
	f->glEnableVertexAttribArray(2); // location = 2
	f->glVertexAttribPointer(2, Vertex::TextureTupleSize, GL_FLOAT, GL_FALSE, Vertex::stride(), (GLvoid*)Vertex::textureOffset());

	VBO.release();
	VAO.release();
}


