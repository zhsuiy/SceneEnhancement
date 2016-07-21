#include "DisplaySceneGLWidget.h"
#include <QOpenGLTexture>
#include "Vertex.h"
#include <QKeyEvent>

#include <QDebug>
#include <iostream>
#include "Global.h"
#include "DecorationModel.h"
#include "ProbLearning.h"
#include <QFileDialog>
#include <QImage>

using namespace std;

static const GLfloat vertices[] =
{
	// Positions           // Normals           // Texture Coords
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
	0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
	0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

	0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
	0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
	0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
};

DisplaySceneGLWidget::DisplaySceneGLWidget(ProbLearning *learner, QWidget* parent)
	:QGLWidget(parent)
	, m_vbo(QOpenGLBuffer::VertexBuffer)
	, m_ebo(QOpenGLBuffer::IndexBuffer)
{	
	parameter = Parameter::GetParameterInstance();
	m_assets = Assets::GetAssetsInstance();
	m_learner = learner;
	camera = new Camera(QVector3D(1.29,1.41,4.6),QVector3D(0,1,0),-88,-13.25);
	Lights = Utility::ParseLights();

	// for render
	modelMatrix.setToIdentity();
	viewMatrix.translate(0.0, 0.0, -5.0);
	projection.perspective(45, parameter->ScreenWidth / parameter->ScreenHeight, 0.1f, 100.0f);
	setFormat(QGLFormat(QGL::SampleBuffers));		
	
	is_display_decoration = true;
	
}

DisplaySceneGLWidget::~DisplaySceneGLWidget()
{
	makeCurrent();
	teardownGL();
}

void DisplaySceneGLWidget::teardownGL() const
{

	delete camera;
	delete m_program;
}

void DisplaySceneGLWidget::keyPressEvent(QKeyEvent* event)
{
	switch (event->key())
	{
	case Qt::Key_W:
		camera->ProcessKeyboard(FORWARD);
		break;
	case Qt::Key_S:
		if (event->modifiers() == Qt::ControlModifier)
			SaveImage();
		camera->ProcessKeyboard(BACKWARD);
		break;
	case Qt::Key_A:
		camera->ProcessKeyboard(LEFT);
		break;
	case Qt::Key_D:
		if (event->modifiers() == Qt::ControlModifier)
			UpdateDecorations();
		else
			camera->ProcessKeyboard(RIGHT);
		break;
	case Qt::Key_R:
		camera->Reset();
		break;
	case Qt::Key_U:
		if (event->modifiers() == Qt::ControlModifier)
			UpdateConfig();
		break;
	case Qt::Key_M:
		if (event->modifiers() == Qt::ControlModifier)
			UpdateMaterials();
		break;
	case Qt::Key_Right:
		UpdateMaterialsByLearner();
		break;
	case Qt::Key_Up:
		UpdateDecorationsByLearner();
		break;
	case Qt::Key_T:		
			ToggleTexture();	
		break;

	default:
		break;
	}
	update();
}

void DisplaySceneGLWidget::UpdateDecorations()
{	
	models.clear();
	// remove decoration models from furniture models
	for (size_t i = 0; i <  furniture_models.size(); i++)
	{
		furniture_models[i]->ClearDecorationLayout();
	}
	// update decoration from file ... need improve here
	decoration_models = m_assets->GetUpdatedDecorationModels();
	// add furniture and decoration models to models
	for (size_t i = 0; i < furniture_models.size(); i++)
	{		
		models.push_back(furniture_models[i]);
	}
	for (size_t i = 0; i < decoration_models.size(); i++)
	{
		models.push_back(decoration_models[i]);
	}
	// layout decoration models
	for (size_t i = 0; i < furniture_models.size(); i++)
	{
		//furniture_models[i]->UpdateDecorationLayoutWithConstraints();
		furniture_models[i]->UpdateDecorationLayout();
	}
	update();
}

void DisplaySceneGLWidget::UpdateMaterials()
{
	m_assets->UpdateMaterialMap();
	for (size_t i = 0; i < furniture_models.size(); i++)
	{
		furniture_models[i]->UpdateMeshMaterials();
	}
	//wall_floor_model->UpdateMaterials();
	update();
}

void DisplaySceneGLWidget::UpdateMaterialsByLearner()
{
	if (m_learner->IsLearned())
	{
		current_colors = m_learner->GetFurnitureColorPalette(0);		
	}
	UpdateCurrentMaterials();
}

void DisplaySceneGLWidget::UpdateCurrentMaterials()
{	
	for (size_t i = 0; i < furniture_models.size(); i++)
	{
		if (current_colors.contains(furniture_models[i]->Type))
		{
			furniture_models[i]->UpdateMeshMaterials(current_colors[furniture_models[i]->Type]);
		}
	}	
	update();
}

void DisplaySceneGLWidget::UpdateDecorationsByLearner()
{
	// 此处可以考虑使用max flow min cut进行小物件的分配
	if (m_learner->IsLearned())
	{
		models.clear();
		for (size_t i = 0; i < decoration_models.size(); i++)
		{
			decoration_models[i]->IsAssigned = false;
		}
		decoration_models.clear();
		// remove decoration models from furniture models
		for (size_t i = 0; i < furniture_models.size(); i++)
		{
			furniture_models[i]->ClearDecorationLayout();
		}
		// add furniture and decoration models to models
		for (size_t i = 0; i < furniture_models.size(); i++)
		{
			models.push_back(furniture_models[i]);
		}
		
		auto multidecorations = parameter->DecorationMultiTypes;
		auto decorationList = m_learner->GetDecorationTypes(25);
		QVector<DecorationType> decadded;
		// 采样，每个小物件只添加N次
		for (size_t sn = 0; sn < parameter->MaxSupportNumber; sn++)
		{
			for (size_t i = 0; i < decorationList.size(); i++)
			{
				auto furniturelist = decorationList[i].second;
				double sp = static_cast<double>(rand()) / (RAND_MAX);
				double cp = 0; // 累积概率
				for (size_t j = 0; j < furniturelist.size(); j++)
				{
					cp += furniturelist[j].second;
					FurnitureModel * furnituremodel = m_assets->GetFurnitureModelByType(furniturelist[j].first);
					// 概率落到当前家具，且当前家具里面不包含当前decoration model
					if (furnituremodel && sp < cp && !furnituremodel->IsDecorationAdded(decorationList[i].first)) 
					{
						DecorationModel * decmodel = m_assets->GetDecorationModel(decorationList[i].first);
						
						// 暂不考虑墙和地板
						if (furniturelist[j].first.compare("Wall", Qt::CaseInsensitive) == 0
							/*|| furnitures[j].first.compare("Floor", Qt::CaseInsensitive) == 0*/)
						{
							continue;
						}
						if (furnituremodel != nullptr && decmodel != nullptr)
						{
							// if floor, rotate some angle
							if (furnituremodel->Type == "FloorProxy")
							{
								double r = static_cast<double>(rand()) / (RAND_MAX);
								decmodel->SetRotation(QVector3D(0, -(30.0 + r * 120.0), 0));
							}
							else
							{
								decmodel->SetRotation(furnituremodel->GetRotate());
							}


							// 不允许多次
							if (!multidecorations.contains(decmodel->Type))
							{								
								if (!decadded.contains(decmodel->Type)) // 还没添加过
								{
									furnituremodel->AddDecorationModel(decmodel);
									decoration_models.push_back(decmodel);
									if (!decadded.contains(decmodel->Type))
									{
										decadded.push_back(decmodel->Type);
									}									
								}								
							}
							else // 允许多次
							{								
								furnituremodel->AddDecorationModel(decmodel);
								decoration_models.push_back(decmodel);
								if (!decadded.contains(decmodel->Type))
								{
									decadded.push_back(decmodel->Type);
								}								
							}
							break;							
						}
					}
				}
			}
		}
		

		// 小物件所在的家具上都添加
		//for (size_t i = 0; i < decorationList.size(); i++)
		//{
		//	auto furnitures = decorationList[i].second;
		//	int n = furnitures.size() > 5 ? 5 : furnitures.size();
		//	//int n = furnitures.size();
		//	for (size_t j = 0; j < n; j++)
		//	{
		//		DecorationModel * decmodel = m_assets->GetDecorationModel(decorationList[i].first);									
		//		FurnitureModel * furnituremodel = m_assets->GetFurnitureModelByType(furnitures[j].first);
		//		// 暂不考虑墙和地板
		//		if (furnitures[j].first.compare("Wall",Qt::CaseInsensitive) == 0
		//			/*|| furnitures[j].first.compare("Floor", Qt::CaseInsensitive) == 0*/)
		//		{
		//			continue;
		//		}
		//		if (furnituremodel != nullptr && decmodel!=nullptr)
		//		{
		//			furnituremodel->AddDecorationModel(decmodel);					
		//			decoration_models.push_back(decmodel);					
		//		}			
		//	}
		//}		
	}	
	// layout decoration models
	for (size_t i = 0; i < furniture_models.size(); i++)
	{
		//furniture_models[i]->UpdateDecorationLayoutWithConstraints();
		furniture_models[i]->UpdateDecorationLayout();
	}
	// add decoration models to models
	// 只有当模型真正被摆到furniture上的时候才需要被渲染
	for (size_t i = 0; i < decoration_models.size(); i++)
	{
		if (decoration_models[i]->IsAssigned)
		{
			models.push_back(decoration_models[i]);
		}		
	}
	update();
}

void DisplaySceneGLWidget::ToggleTexture()
{
	for (size_t i = 0; i < furniture_models.size(); i++)
	{
		furniture_models[i]->ToggleTextureOn();
	}
	update();
}

void DisplaySceneGLWidget::ToggleDisplayDecorations()
{
	is_display_decoration = !is_display_decoration;
	update();
}

void DisplaySceneGLWidget::RearrangeDecorations()
{	
	// layout decoration models
	for (size_t i = 0; i < furniture_models.size(); i++)
	{
		//furniture_models[i]->UpdateDecorationLayoutWithConstraints();
		furniture_models[i]->UpdateDecorationLayout();
	}	
	update();
}

void DisplaySceneGLWidget::SaveImage()
{
	QImage img = this->grabFrameBuffer();	
	/*QString fileName = QFileDialog::getSaveFileName(this,
		tr("Open Config"),
		"",
		tr("Config Files (*.txt)"));*/
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save as..."), "",
		tr("PNG (*.png);; BMP (*.bmp);;TIFF (*.tiff *.tif);; JPEG (*.jpg *.jpeg)"));
	img.save(fileName);
}

void DisplaySceneGLWidget::SaveFurnitureColor()
{
	QString fileName = QFileDialog::getSaveFileName(this,
		tr("Open Config"),
		"",
		tr("Config Files (*.txt)"));
	if (!fileName.isNull())
	{
		QFile file(fileName); // if not exist, create
		file.open(QIODevice::WriteOnly);
		file.close();
		file.open(QIODevice::ReadWrite);
		if (file.isOpen())
		{
			QTextStream txtOutput(&file);
			QMapIterator<FurnitureType, ColorPalette*> it(current_colors);
			txtOutput << "Furniture Color\n";
			while (it.hasNext())
			{
				it.next();
				txtOutput << it.key() << " = ";
				auto cp = it.value();
				for (size_t i = 0; i < cp->Colors.size(); i++)
				{
					txtOutput << cp->Colors[i].red() << " " <<
						cp->Colors[i].green() << " " <<
						cp->Colors[i].blue() << " ";
				}
				txtOutput << "\n";
			}
		}		
		file.close();
	}	
}

void DisplaySceneGLWidget::SaveDecorations()
{
	QString fileName = QFileDialog::getSaveFileName(this,
		tr("Open Config"),
		"",
		tr("Config Files (*.txt)"));
	if (!fileName.isNull())
	{
		QFile file(fileName); // if not exist, create
		file.open(QIODevice::WriteOnly);
		file.close();
		file.open(QIODevice::ReadWrite);
		if (file.isOpen())
		{
			QTextStream txtOutput(&file);
			QMapIterator<FurnitureType, ColorPalette*> it(current_colors);			
			for (size_t i = 0; i < decoration_models.size(); i++)
			{
				DecorationModel* dm = decoration_models[i];
				if (dm->IsAssigned) // is rendered
				{
					txtOutput << dm->Type << " "
						<< dm->SupportModelType << " "
						<< dm->GetRelativeTranslate().x() << " "
						<< dm->GetRelativeTranslate().y() << " "
						<< dm->GetRelativeTranslate().z() << " "
						<< dm->GetRotate().x() << " "
						<< dm->GetRotate().y() << " "
						<< dm->GetRotate().z() << "\n";
				}
			}		
		}
		file.close();
	}
}

void DisplaySceneGLWidget::ReadFurnitureColor()
{
	QString fileName = QFileDialog::getOpenFileName(this,
		tr("Open Config"),
		"",
		tr("Config Files (*.txt)"));
	if (!fileName.isNull())
	{
		current_colors = Utility::ReadImageFurnitureInfo(fileName);
	}
	UpdateCurrentMaterials();
}

void DisplaySceneGLWidget::ReadDecorations()
{
	QString fileName = QFileDialog::getOpenFileName(this,
		tr("Open Config"),
		"",
		tr("Config Files (*.txt)"));
	if (!fileName.isNull())
	{
		QFile *file = new QFile(fileName);
		if (!file->open(QIODevice::ReadWrite | QIODevice::Text))
		{
			std::cout << "Can't open file " + fileName.toStdString() << endl;
			return;
		}

		// initialize
		models.clear();
		for (size_t i = 0; i < decoration_models.size(); i++)
		{
			decoration_models[i]->IsAssigned = false;
		}
		decoration_models.clear();
		// remove decoration models from furniture models
		for (size_t i = 0; i < furniture_models.size(); i++)
		{
			furniture_models[i]->ClearDecorationLayout();
		}
		// add furniture and decoration models to models
		for (size_t i = 0; i < furniture_models.size(); i++)
		{
			models.push_back(furniture_models[i]);
		}

		while (!file->atEnd())
		{
			QByteArray line = file->readLine();
			QString str(line);
			QStringList parts = str.split(' ', QString::SkipEmptyParts);

			if (parts.size() == 8)
			{
				DecorationType dt = parts[0];
				FurnitureType ft = parts[1];
				float x = parts[2].toFloat();
				float y = parts[3].toFloat();
				float z = parts[4].toFloat();
				float rx = parts[5].toFloat();
				float ry = parts[6].toFloat();
				float rz = parts[7].toFloat();

				FurnitureModel * furnituremodel = m_assets->GetFurnitureModelByType(ft);
				DecorationModel * decmodel = m_assets->GetDecorationModel(dt);
				if (furnituremodel != nullptr && decmodel !=nullptr)
				{
					decmodel->SetRelativeTranslate(x, y, z);
					decmodel->SetRotation(QVector3D(rx, ry, rz));
					furnituremodel->AddDecorationModel(decmodel);
					decoration_models.push_back(decmodel);					
					models.push_back(decmodel);
				}				
			}		
		}
	}
	update();
}

void DisplaySceneGLWidget::initializeGL()
{
	initializeOpenGLFunctions();
	// Enable depth buffer
	glEnable(GL_DEPTH_TEST);
	// Enable back face culling
	//glEnable(GL_CULL_FACE);

	//glEnable(GL_MULTISAMPLE);
	//glEnable(GL_LINE_SMOOTH);
	//glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glClearColor(0.3, 0.3, 0.3, 0);

	// Application-specific initialization
	{
		// Create Shader (Do not release until VAO is created)
		m_program = new QOpenGLShaderProgram();
		m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, "./shaders/mesh.vert");
		m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, "./shaders/meshMaterial.frag");
		m_program->link();
	}

	initLights();
	furniture_models = m_assets->GetFurnitureModels();	
	decoration_models = m_assets->GetDecorationModels();
	for (size_t i = 0; i < furniture_models.size(); i++)
	{
		models.push_back(furniture_models[i]);
	}
	for (size_t i = 0; i < decoration_models.size(); i++)
	{
		models.push_back(decoration_models[i]);
	}
	for (size_t i = 0; i < furniture_models.size(); i++)
	{
		//furniture_models[i]->UpdateDecorationLayoutWithConstraints();
		furniture_models[i]->UpdateDecorationLayout();
	}	
}

void DisplaySceneGLWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// 清除屏幕和深度缓存

	m_program->bind();
	{
		viewMatrix = camera->GetViewMatrix();	
		projection.setToIdentity();
		projection.perspective(camera->Zoom, (float)parameter->ScreenWidth / (float)parameter->ScreenHeight, 0.1f, 100.0f);
		//projection.perspective(camera->Zoom, 1384.0f/726.0f, 0.1f, 100.0f);

		m_program->setUniformValue("viewMatrix", viewMatrix);
		m_program->setUniformValue("projection", projection);
		m_program->setUniformValue("viewPos", camera->Position);

		m_program->setUniformValue("material.shininess", 2.0f);
		
		/*if (dynamic_cast<PointLight*>(Lights[1]))
		{
			dynamic_cast<PointLight*>(Lights[1])->Position = camera->Position;
		}*/

		for (size_t i = 0; i < Lights.size(); i++)
		{
			Lights[i]->SetShaderProgram(m_program);
		}

		for (size_t i = 0; i < models.size(); i++)
		{
			if(dynamic_cast<DecorationModel*>(models[i]))
			{
				if (is_display_decoration)
				{
					models[i]->Draw(m_program);
				}
			}
			else
			{
				models[i]->Draw(m_program);
			}			
		}

	}
	m_program->release();
	//paintLight();	
}


void DisplaySceneGLWidget::resizeGL(int width, int height)
{


	glViewport(0, 0, (GLint)width, (GLint)height);	// 重置当前的视口
													//glMatrixMode(GL_PROJECTION);				// 选择投影矩阵
													//glLoadIdentity();							// 重置投影矩阵
													////gluPerspective(45.0,(GLfloat)width/(GLfloat)height,0.1,100.0); // 建立透视投影矩阵
													//glMatrixMode(GL_MODELVIEW); // 选择模型观察矩阵
													//glLoadIdentity();			// 重置模型观察矩阵

	projection.setToIdentity();

	// Set perspective projection
	projection.perspective(45, qreal(width) / qreal(height ? height : 1), 0.1f, 100.0f);


}

void DisplaySceneGLWidget::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		mouseButton = Qt::LeftButton;
		mouseCurPos = QVector2D(event->localPos());
		mouseLastPos = QVector2D(event->localPos());
		//std::cout << "local pos" << mouseCurrentPosition.x << "," << mouseCurrentPosition.y() << endl;
	}
	else if (event->button() == Qt::RightButton)
	{
		mouseButton = Qt::RightButton;
		mouseCurPos = QVector2D(event->localPos());
		mouseLastPos = QVector2D(event->localPos());
	}


}
QVector3D DisplaySceneGLWidget::getArcBallVector(int x, int y)
{
	QVector3D P = QVector3D(1.0*x / parameter->ScreenWidth * 2 - 1.0,
		1.0*y / parameter->ScreenHeight * 2 - 1.0,
		0);
	P.setY(-P.y());
	float OP_squared = P.x() * P.x() + P.y() * P.y();
	if (OP_squared <= 1 * 1)
		P.setZ(sqrt(1 * 1 - OP_squared));  // Pythagore
	else
		P.normalize(); // nearest point
	return P;

}


void DisplaySceneGLWidget::mouseMoveEvent(QMouseEvent* event)
{
	if (mouseButton == Qt::LeftButton)
	{
		mouseCurPos.setX(event->localPos().x());
		mouseCurPos.setY(event->localPos().y());

		if (mouseCurPos.x() != mouseLastPos.x() || mouseCurPos.y() != mouseLastPos.y())
		{
			camera->ProcessMouseMovement(mouseCurPos.x() - mouseLastPos.x(), mouseLastPos.y() - mouseCurPos.y());
			mouseLastPos.setX(mouseCurPos.x());
			mouseLastPos.setY(mouseCurPos.y());
			update();
		}
	}

	else if (mouseButton == Qt::RightButton)
	{
		mouseCurPos.setX(event->localPos().x());
		mouseCurPos.setY(event->localPos().y());
		camera->ProcessRightMouseMovement(mouseCurPos.x() - mouseLastPos.x(), mouseLastPos.y() - mouseCurPos.y());
		mouseLastPos.setX(mouseCurPos.x());
		mouseLastPos.setY(mouseCurPos.y());
		update();
	}
}

void DisplaySceneGLWidget::mouseReleaseEvent(QMouseEvent* event)
{
	mouseButton = Qt::NoButton;
}

void DisplaySceneGLWidget::wheelEvent(QWheelEvent* event)
{
	int diff = event->delta() - scrollDelta;

	camera->ProcessMouseScroll(event->delta() / 120);

	scrollDelta = event->delta();

	update();
}

void DisplaySceneGLWidget::printVersionInformation()
{
	QString glType;
	QString glVersion;
	QString glProfile;

	// Get Version Information
	//glType = (context()->isOpenGLES()) ? "OpenGL ES" : "OpenGL";
	glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));

	// Get Profile Information
#define CASE(c) case QSurfaceFormat::c: glProfile = #c; break
	switch (format().profile())
	{
		CASE(NoProfile);
		CASE(CoreProfile);
		CASE(CompatibilityProfile);
	}
#undef CASE

	// qPrintable() will print our QString w/o quotes around it.
	//qDebug() << qPrintable(glType) << qPrintable(glVersion) << "(" << qPrintable(glProfile) << ")";
	qDebug() << qPrintable(glVersion) << "(" << qPrintable(glProfile) << ")";
}

void DisplaySceneGLWidget::initLights()
{

	// Create Shader (Do not release until VAO is created)
	light_program = new QOpenGLShaderProgram();
	light_program->addShaderFromSourceFile(QOpenGLShader::Vertex, "./shaders/light.vert");
	light_program->addShaderFromSourceFile(QOpenGLShader::Fragment, "./shaders/light.frag");
	light_program->link();
	light_program->bind();

	// Create Vertex Array Object
	light_vao.create();
	light_vao.bind();

	// Create Buffer (Do not release until VAO is created)
	m_vbo.create();
	m_vbo.bind();
	m_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	m_vbo.allocate(vertices, sizeof(vertices));

	light_program->enableAttributeArray(0);

	//m_program->enableAttributeArray(2);
	//   m_program->setAttributeBuffer(0, GL_FLOAT, 0, 3 , 8*sizeof(GLfloat));      // 3表示的是这一个属性里面有几个分量
	//m_program->setAttributeBuffer(1, GL_FLOAT, 3*sizeof(GLfloat), 3,  8*sizeof(GLfloat));
	//m_program->setAttributeBuffer(2, GL_FLOAT, 6 * sizeof(GLfloat), 2, 8 * sizeof(GLfloat));
	//

	light_program->setAttributeBuffer(0, GL_FLOAT, 0, 3, 8 * sizeof(GLfloat));      // 3表示的是这一个属性里面有几个分量

																					// Release (unbind) all	
	m_vbo.release();
	light_vao.release();
	//m_ebo.release();
	light_program->release();


}

void DisplaySceneGLWidget::paintLight()
{
	light_program->bind();
	{
		viewMatrix = camera->GetViewMatrix();
		projection.setToIdentity();
		projection.perspective(camera->Zoom, (float)parameter->ScreenWidth / (float)parameter->ScreenHeight, 0.1f, 100.0f);
		light_program->setUniformValue("viewMatrix", viewMatrix);
		light_program->setUniformValue("projection", projection);
		light_vao.bind();
		for (int i = 0; i < Lights.size(); i++)
		{
			if (dynamic_cast<PointLight*>(Lights[i]))
			{
				modelMatrix.setToIdentity();
				modelMatrix.translate(dynamic_cast<PointLight*>(Lights[i])->Position);
				modelMatrix.scale(0.2f);
				light_program->setUniformValue("modelMatrix", modelMatrix);
				light_program->setUniformValue("LightColor", Lights[i]->Diffuse);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				//glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
				//glDrawArrays(GL_TRIANGLES, 0, sizeof(sg_vertexes) / sizeof(sg_vertexes[0]));
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
			
		}
		light_vao.release();
	}
}


void DisplaySceneGLWidget::UpdateConfig()
{
	parameter->Update();
	UpdateMaterials();
	UpdateDecorations();
	update();
}

