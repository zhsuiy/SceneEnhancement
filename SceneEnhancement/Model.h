#ifndef MODEL_H
#define MODEL_H
#include <QtCore/qstring.h>
#include <QtGui/QOpenGLShader>
#include "Mesh.h"

#include <assimp/scene.h>
#include "Material.h"
#include "BoundingBox.h"

class Model
{
public:
	/* constructors and destructors*/
	Model();
	Model(QString path);
	Model(QString path, QVector3D translate, QVector3D rotate, float scale);
	virtual ~Model();

	/* Render */
	virtual void Draw(QOpenGLShaderProgram *program);
	
	/* Attributes */
	void SetTranslation(QVector3D translate);
	void SetScale(float scale);
	void SetRotation(QVector3D rotate);
	QVector3D& GetTranslate() { return m_translate; };
	QVector3D& GetRotate() { return m_rotate; };
	float GetScale() const	{ return m_scale; };

protected:
	void init();

	QVector3D m_translate;
	QVector3D m_rotate;
	float m_scale;

	BoundingBox *boundingBox;
	void updateBoundingBox();
	void GetMinMaxCoordinates(QVector3D &min, QVector3D &max);

	QVector<Mesh*> meshes;
	QVector<Texture*> textures_loaded;
	QMap<QString, Material*> material_assets;	
	QString directory;

	/* Load model using assimp */
	void loadModel(QString path);
	void processNode(aiNode* node, const aiScene *scene);
	Mesh* processMesh(aiMesh* mesh, const aiScene* scene);
	QVector<Texture*> loadMaterialTextures(aiMaterial* mat, aiTextureType type, TextureType typeName);
	QOpenGLTexture* TextureFromFile(QString path, QString directory);

	void updateMeshNormals();
	
};


#endif
