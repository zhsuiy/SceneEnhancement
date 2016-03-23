#include "DecorationModel.h"
#include "Utility.h"
#include "Assets.h"

DecorationModel::DecorationModel(FurnitureType furnitureType, DecorationType decType, 
								DecorationLocType locType, float scale)
	:Model()
{
	support_model_type = furnitureType;
	m_support_model = Assets::GetAssetsInstance()->GetFurnitureModelByType(support_model_type);
	
	m_translate = m_support_model->GetTranslate();
	m_rotate = m_support_model->GetRotate();
	m_scale = scale;
	
	// set locationtype
	LocationType = Utility::GetLocationTypeFromString(locType);
	QString modelPath = Utility::GetDecorationModelPath(decType);
	this->loadModel(modelPath);
	this->updateMeshNormals();


}

void DecorationModel::Draw(QOpenGLShaderProgram* program)
{	
	QMatrix4x4 modelMatrix;
	modelMatrix.setToIdentity();

	modelMatrix.translate(m_translate);

	modelMatrix.rotate(m_rotate.x(), 1, 0, 0);
	modelMatrix.rotate(m_rotate.y(), 0, 1, 0);
	modelMatrix.rotate(m_rotate.z(), 0, 0, 1);
	
	modelMatrix.translate(0, -0.1, 0.1);

	modelMatrix.scale(m_scale);

	program->setUniformValue("modelMatrix", modelMatrix);

	for (int i = 0; i < meshes.size(); i++)
	{
		meshes[i]->Draw(program);
	}
	

}
