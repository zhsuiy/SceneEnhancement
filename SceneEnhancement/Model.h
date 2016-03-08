#ifndef MODEL_H
#define MODEL_H
#include <QtCore/qstring.h>
#include <QtGui/QOpenGLShader>
#include "Mesh.h"

#include <assimp/scene.h>


class Model
{
public:
	Model(QString path);
	void Draw(QOpenGLShaderProgram *program);
private:
	QVector<Mesh*> meshes;
	QString directory;

	void loadModel(QString path);
	void processNode(aiNode* node, const aiScene *scene);
	Mesh* processMesh(aiMesh* mesh, const aiScene* scene);
	QVector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, QString typeName);
	QOpenGLTexture* TextureFromFile(QString path, QString directory);
	
};


#endif
