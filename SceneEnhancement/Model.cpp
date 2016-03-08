#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <iostream>

Model::Model(QString path)
{
	
}

void Model::Draw(QOpenGLShaderProgram *program)
{
	for (int i = 0; i < meshes.size();i++)
	{
		meshes[i]->Draw(program);
	}
}

void Model::loadModel(QString path)
{	
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path.toStdString().c_str(), 
		aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->
		mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
		return;
	}
	this->directory = path.left(path.lastIndexOf('/'));
	
	this->processNode(scene->mRootNode, scene);


}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	// Process all the node¡¯s meshes (if any)	
	for (GLuint i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		this->meshes.push_back(this->processMesh(mesh, scene));
	}
	// Then do the same for each of its children
	for (GLuint i = 0; i < node->mNumChildren; i++)
	{
		this->processNode(node->mChildren[i], scene);
	}
}

Mesh* Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	QVector<Vertex> vertices;
	QVector<GLuint> indices;
	QVector<Texture> textures;
	
	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		// Process vertex positions, normals and texture coordinates
		vertex.setPosition(QVector3D(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));
		vertex.setNormal(QVector3D(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));
		if (mesh->mTextureCoords[0]) // Does the mesh contain texture coordinates?		
			vertex.setTexture(QVector2D(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y));
		else
			vertex.setTexture(QVector2D(0.0f, 0.0f));		
		vertices.push_back(vertex);
	}
	
	// Process indices
	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	// Process material
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		QVector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		QVector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		for (size_t i = 0; i < diffuseMaps.size(); i++)
			textures.push_back(diffuseMaps[i]);		
		for (size_t i = 0; i < specularMaps.size(); i++)
			textures.push_back(specularMaps[i]);		
	}
	
	return new Mesh(vertices, indices, textures);
}

QVector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, QString typeName)
{
	QVector<Texture> textures;
	for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		Texture texture;
		texture.TextureId = TextureFromFile(str.C_Str(), this->directory);
		//texture.type = typeName;
		//texture.path = str;
		textures.push_back(texture);
	}
	return textures;
}

QOpenGLTexture* Model::TextureFromFile(QString path, QString directory)
{
	//Generate texture ID and load texture data 
	QString filename = path;
	filename = directory + '/' + filename;
	
	//glGenTextures(1, &textureID);
	//int width, height;


	QOpenGLTexture *texture = new QOpenGLTexture(QImage(filename).mirrored());
	texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
	texture->setMagnificationFilter(QOpenGLTexture::Linear);
	texture->setWrapMode(QOpenGLTexture::Repeat);	
	texture->setAutoMipMapGenerationEnabled(true);
	


	//unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
	// Assign texture to ID
	//glBindTexture(GL_TEXTURE_2D, textureID);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	//glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);*/
	//glBindTexture(GL_TEXTURE_2D, 0);
	//SOIL_free_image_data(image);
	return texture;
}
