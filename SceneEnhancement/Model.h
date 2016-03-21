#ifndef MODEL_H
#define MODEL_H
#include <QtCore/qstring.h>
#include <QtGui/QOpenGLShader>
#include "Mesh.h"

#include <assimp/scene.h>
#include "Material.h"


class Model
{
public:
	Model();
	Model(QString path);
	Model(QString path, QVector3D translate, QVector3D rotate, float scale);
	void Draw(QOpenGLShaderProgram *program);
	void SetTranslation(QVector3D translate);
	void SetScale(float scale);
	void SetRotation(QVector3D rotate);
protected:
	QVector3D m_translate;
	QVector3D m_rotate;
	float m_scale;

	QVector<Mesh*> meshes;
	QVector<Texture*> textures_loaded;
	QMap<QString, Material*> material_assets;
	QString directory;

	void loadModel(QString path);
	void processNode(aiNode* node, const aiScene *scene);
	Mesh* processMesh(aiMesh* mesh, const aiScene* scene);
	QVector<Texture*> loadMaterialTextures(aiMaterial* mat, aiTextureType type, TextureType typeName);
	QOpenGLTexture* TextureFromFile(QString path, QString directory);

	void updateMeshNormals();
	
};


#endif
