#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <iostream>
#include "Global.h"
#include "FurnitureModel.h"
#include "Parameter.h"
#include<algorithm>

Model::Model():m_scale(1.0f)
{
}

Model::Model(QString path):m_scale(1.0f)
{
	this->loadModel(path);
	//directory = path;
	//this->SetModelMatrix();
	init();
}

Model::Model(QString path, QVector3D translate, QVector3D rotate, float scale = 1.0f)
{
	this->loadModel(path);
	this->SetTranslation(translate);
	this->SetRotation(rotate);
	this->SetScale(scale);
	//directory = path;
	//this->SetModelMatrix();
	init();	
}

void Model::init()
{	
	this->updateVertexPosition();
	this->updateMeshNormals();	
	this->updateBoundingBox();	
}

Model::~Model()
{
}



void Model::Draw(QOpenGLShaderProgram *program)
{	
	program->setUniformValue("modelMatrix", modelMatrix);	

	for (int i = 0; i < meshes.size();i++)
	{		
		meshes[i]->Draw(program);
	}
	if (Parameter::GetParameterInstance()->IsDrawBoundingBox && boundingBox != nullptr)
	{
		boundingBox->Draw(program);
	}
}

void Model::SetTranslation(QVector3D translate)
{
	m_translate = translate;
	
}

void Model::SetScale(float scale = 1.0f)
{
	m_scale = scale;
}

void Model::SetRotation(QVector3D rotate)
{
	m_rotate = rotate;
}

void Model::updateBoundingBox()
{
	QVector3D min, max;
	GetMinMaxCoordinates(min, max);
	boundingBox = new BoundingBox(min, max);	
}

void Model::UpdateBoundingBoxWorldCoordinates()
{
	boundingBox->UpdateWorldCoordinates(modelMatrix);	
}

void Model::GetMinMaxCoordinates(QVector3D& min, QVector3D& max)
{
	if (this->meshes.size() > 0 && this->meshes[0]->Vertices.size() > 0)
	{
		min = this->meshes[0]->Vertices[0].position();
		max = this->meshes[0]->Vertices[0].position();
	}
	
	for (size_t i = 0; i < this->meshes.size(); i++)
	{
		QVector3D tmpmin, tmpmax;
		this->meshes[i]->GetMinMaxCoordinates(tmpmin, tmpmax);
		min.setX(tmpmin.x() < min.x() ? tmpmin.x() : min.x());
		min.setY(tmpmin.y() < min.y() ? tmpmin.y() : min.y());
		min.setZ(tmpmin.z() < min.z() ? tmpmin.z() : min.z());
		max.setX(tmpmax.x() > max.x() ? tmpmax.x() : max.x());
		max.setY(tmpmax.y() > max.y() ? tmpmax.y() : max.y());
		max.setZ(tmpmax.z() > max.z() ? tmpmax.z() : max.z());
	}
}

void Model::loadModel(QString path)
{	
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path.toStdString().c_str(), 
		aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->
		mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
		return;
	}
	this->directory = path.left(path.lastIndexOf('/'));
	
	this->processNode(scene->mRootNode, scene);

	std::cout << path.toStdString() << " is loaded" << std::endl;
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
	//QVector<Texture*> textures;
	Material *curMaterial;
	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		// Process vertex positions, normals and texture coordinates
		vertex.setPosition(QVector3D(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));
		if (mesh->HasNormals())
			vertex.setNormal(QVector3D(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));
		else
			std::cout << mesh->mName.C_Str() << std::endl;

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
		QString materialName;
		aiString material_name;
		aiGetMaterialString(material, AI_MATKEY_NAME, &material_name);
		//if (AI_SUCCESS == aiGetMaterialString(material, AI_MATKEY_NAME, &material_name))
			//std::cout << material_name.C_Str() << std::endl;
		materialName = QString(material_name.C_Str());
		if (material_assets.find(materialName) != material_assets.end()) // already exist
		{
			curMaterial = material_assets[materialName];
		}
		else
		{
			MaterialElement *ambient, *diffuse, *specular;
			float shininess = G_Shininess;		
			QVector3D ambientColor, diffuseColor, specularColor;
			aiColor4D ambient_color, diffuse_color, specular_color;
			float opacity = 1.0f;
			if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &ambient_color))
				ambientColor = QVector3D(ambient_color.r, ambient_color.g, ambient_color.b);
			if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuse_color))
				diffuseColor = QVector3D(diffuse_color.r, diffuse_color.g, diffuse_color.b);
			if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &specular_color))
				specularColor = QVector3D(specular_color.r, specular_color.g, specular_color.b);
			aiGetMaterialFloat(material, AI_MATKEY_SHININESS, &shininess);			
			aiGetMaterialFloat(material, AI_MATKEY_OPACITY, &opacity);
			if (shininess == 0)
			{
				shininess = G_Shininess;
			}
			else
			{
				shininess = 32.0*shininess / 1000.0;
			}
			//shininess = shininess > 0 ? shininess : G_Shininess;			

			QVector<Texture*> ambientMaps = this->loadMaterialTextures(material, aiTextureType_AMBIENT, AmbientTexture);
			QVector<Texture*> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, DiffuseTexture);
			QVector<Texture*> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, SpecularTexture);
			

			/*for (size_t i = 0; i < diffuseMaps.size(); i++)
				textures.push_back(diffuseMaps[i]);
			for (size_t i = 0; i < specularMaps.size(); i++)
				textures.push_back(specularMaps[i]);*/
			
			ambient = new MaterialElement(ambientColor, ambientMaps);
			diffuse = new MaterialElement(diffuseColor, diffuseMaps);
			specular = new MaterialElement(specularColor, specularMaps);
			curMaterial = new Material(materialName, ambient, diffuse, specular, shininess, opacity);
			material_assets[materialName] = curMaterial;
		}		
	}
	
	//return new Mesh(vertices, indices, textures);

	return new Mesh(vertices, indices, curMaterial);
	
}

QVector<Texture*> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, TextureType typeName)
{
	QVector<Texture*> textures;
	for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);		
		bool skip = false;
		for (int j = 0; j < textures_loaded.size(); j++)
		{
			if (textures_loaded[j]->path == str)
			{
				textures.push_back(textures_loaded[i]);
				skip = true;
				break;
			}
		}
		if (!skip)
		{
			Texture *texture = new Texture();
			texture->texture = TextureFromFile(str.C_Str(), this->directory);
			texture->type = typeName;
			texture->path = str;
			texture->fullpath = this->directory + '/' + str.C_Str();
			texture->id = textures_loaded.size();
			textures.push_back(texture);
			textures_loaded.push_back(texture);
		}		
	}
	return textures;
}

QOpenGLTexture* Model::TextureFromFile(QString path, QString directory)
{
	//Generate texture ID and load texture data 
	QString filename = path;
	filename = directory + '/' + filename;	
	
	QOpenGLTexture *texture;
	texture = new QOpenGLTexture(QImage(filename).mirrored());
	//texture->setAutoMipMapGenerationEnabled(true);
	texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
	texture->setMagnificationFilter(QOpenGLTexture::Linear);
	texture->setWrapMode(QOpenGLTexture::Repeat);		
	return texture;
}

void Model::updateMeshNormals()
{
	QMatrix4x4 rotationMatrix;
	rotationMatrix.setToIdentity();
	rotationMatrix.rotate(m_rotate.x(), 1, 0, 0);
	rotationMatrix.rotate(m_rotate.y(), 0, 1, 0);
	rotationMatrix.rotate(m_rotate.z(), 0, 0, 1);

	for (size_t i = 0; i < meshes.size(); i++)
	{		
		for (size_t j = 0; j < meshes[i]->Vertices.size(); j++)
		{
			meshes[i]->Vertices[j].setNormal(rotationMatrix*meshes[i]->Vertices[j].normal());
		}
		meshes[i]->setupRender();
	}
}

void Model::updateVertexPosition()
{
	QVector3D min, max;
	GetMinMaxCoordinates(min, max);
	QVector3D offset = (max + min) / 2;
	for (size_t i = 0; i < this->meshes.size(); i++)
	{
		for (size_t j = 0; j < this->meshes[i]->Vertices.size(); j++)
		{
			this->meshes[i]->Vertices[j].setPosition(this->meshes[i]->Vertices[j].position() - offset);
		}
		meshes[i]->setupRender();
	}	
}

