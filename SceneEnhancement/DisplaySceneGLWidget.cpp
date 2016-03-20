#include "DisplaySceneGLWidget.h"
#include <QOpenGLTexture>
#include "Vertex.h"
#include <QKeyEvent>

#include <QDebug>
#include <iostream>
#include "Global.h"


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

static const QVector3D pointLightPositions[] = {
	QVector3D(1.5f, 1.5f, 1.5f),
	//QVector3D(0.0f, 0.0f, 0.0f),
	QVector3D(1.5f, 15.0f, 1.5f),
	QVector3D(-4.0f, 2.0f, -12.0f),
	QVector3D(0.0f, 0.0f, -3.0f)
};

DisplaySceneGLWidget::DisplaySceneGLWidget(QWidget* parent)
	:QGLWidget(parent)
	, m_vbo(QOpenGLBuffer::VertexBuffer)
	, m_ebo(QOpenGLBuffer::IndexBuffer)
{
	parameter = Parameter::GetParameterInstance();

	// for render
	modelMatrix.setToIdentity();
	viewMatrix.translate(0.0, 0.0, -5.0);
	projection.perspective(45, parameter->ScreenWidth / parameter->ScreenHeight, 0.1f, 100.0f);
	setFormat(QGLFormat(QGL::SampleBuffers));

	camera = new Camera(QVector3D(0.5, 0.5, 3.0));
	
	m_assets = Assets::GetAssetsInstance();
	Lights = Utility::ParseLights();
	
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
		camera->ProcessKeyboard(BACKWARD);
		break;
	case Qt::Key_A:
		camera->ProcessKeyboard(LEFT);
		break;
	case Qt::Key_D:
		camera->ProcessKeyboard(RIGHT);
		break;
	case Qt::Key_R:
		camera->Reset();
		break;
	default:
		break;
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
	QVector<FurnitureModel*> furniture_models = m_assets->GetFurnitureModels();
	for (size_t i = 0; i < furniture_models.size(); i++)
	{
		models.push_back(furniture_models[i]);
	}
	

	//// bed
	//Model* bed = new Model("../dataset/model/bed/57764cebd92102552ea98d69e91ba870/model.obj");
	//bed->SetScale(2.1f);
	//bed->SetTranslation(QVector3D(0.9, 0.25, 1.3));
	//models.push_back(bed);

	//// window
	//Model *window = new Model("../dataset/model/window/b780bf561a813e17be1805cdb5abc91b/model.obj");
	//window->SetScale(0.9f);
	//window->SetTranslation(QVector3D(1.5f, 1.2f, 0.0f));
	//window->SetRotation(QVector3D(0, 270, 0));
	//models.push_back(window);	

	//// desk
	//Model* desk = new Model("../dataset/model/desk/81e991df9ff8b970a2ab2154e681ce15/model.obj"); 
	//desk->SetScale(1.1f);
	//desk->SetTranslation(QVector3D(0.2, 0.3, 2.5));
	//desk->SetRotation(QVector3D(0, 0, 0));
	//models.push_back(desk);

	//// chair
	//Model *chair = new Model("../dataset/model/chair/6cf7fc7979e949c72dc9485cd94746f7/model.obj");
	//chair->SetScale(0.5f);
	//chair->SetTranslation(QVector3D(0.35, 0.2, 2.5));
	//chair->SetRotation(QVector3D(0, 180, 0));
	//models.push_back(chair);

	//// cabinet
	//Model *cabinet = new Model("../dataset/model/cabinet/a46373d86967b3fce9aee4515d4383aa/model.obj");
	//cabinet->SetScale(1.5f);
	//cabinet->SetTranslation(QVector3D(2.8, 0.4, 1.0));
	//cabinet->SetRotation(QVector3D(0, 180, 0));
	//models.push_back(cabinet);

	//// carpet
	//Model *carpet = new Model("../dataset/model/carpet/01/model.obj");
	//carpet->SetScale(0.06f);
	//carpet->SetTranslation(QVector3D(0.5, 0.01, 2.6));
	//models.push_back(carpet);
	//
	//// curtain
	//Model *curtain = new Model("../dataset/model/curtain/Window_Curtains2/model.obj");
	//curtain->SetScale(0.03f);
	//curtain->SetTranslation(QVector3D(0.08f, 2.0f, 0.2f));
	//curtain->SetRotation(QVector3D(90, 0, 0));
	//models.push_back(curtain);

	//model = 
	bb = new BoundingBox(QVector3D(0.0f, 0.0f, 0.0f), QVector3D(3.0f, 2.2f, 3.0f));

}

void DisplaySceneGLWidget::paintGL()
{
	//makeCurrent();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// 清除屏幕和深度缓存

	m_program->bind();
	{

		viewMatrix = camera->GetViewMatrix();
		//viewMatrix.setToIdentity();
		projection.setToIdentity();
		projection.perspective(camera->Zoom, (float)parameter->ScreenWidth / (float)parameter->ScreenHeight, 0.1f, 100.0f);

		m_program->setUniformValue("viewMatrix", viewMatrix);
		m_program->setUniformValue("projection", projection);
		m_program->setUniformValue("viewPos", camera->Position);

		m_program->setUniformValue("material.shininess", 16.0f);

#pragma region lighting
		float ambient = 0.5f;
		float diffuse = 1.0f;
		float specular = 0.1f;
		float linear = 0.09f; // 0.09f;
		float quadratic = 0.032f; // 0.032
		// directional lighting
		/*m_program->setUniformValue("dirLight.direction", 0.2f, 1.0f, 0.3f);
		m_program->setUniformValue("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		m_program->setUniformValue("dirLight.diffuse", 1.0f, 1.0f, 1.0f);
		m_program->setUniformValue("dirLight.specular", 0.5f, 0.5f, 0.5f);*/
		//// Point light 1
		/*m_program->setUniformValue("pointLights[0].position", pointLightPositions[0]);
		m_program->setUniformValue("pointLights[0].ambient", ambient, ambient, ambient);
		m_program->setUniformValue("pointLights[0].diffuse", diffuse, diffuse, diffuse);
		m_program->setUniformValue("pointLights[0].specular", specular, specular, specular);
		m_program->setUniformValue("pointLights[0].constant", 1.0f);
		m_program->setUniformValue("pointLights[0].linear", linear);
		m_program->setUniformValue("pointLights[0].quadratic", quadratic);*/
		//// Point light 2
		//m_program->setUniformValue("pointLights[1].position", pointLightPositions[1]);
		//m_program->setUniformValue("pointLights[1].ambient", ambient, ambient, ambient);
		//m_program->setUniformValue("pointLights[1].diffuse", diffuse, diffuse, diffuse);
		//m_program->setUniformValue("pointLights[1].specular", specular, specular, specular);
		//m_program->setUniformValue("pointLights[1].constant", 1.0f);
		//m_program->setUniformValue("pointLights[1].linear", linear);
		//m_program->setUniformValue("pointLights[1].quadratic", quadratic);
		////// Point light 3
		//m_program->setUniformValue("pointLights[2].position", pointLightPositions[2]);
		//m_program->setUniformValue("pointLights[2].ambient", ambient, ambient, ambient);
		//m_program->setUniformValue("pointLights[2].diffuse", diffuse, diffuse, diffuse);
		//m_program->setUniformValue("pointLights[2].specular", specular, specular, specular);
		//m_program->setUniformValue("pointLights[2].constant", 1.0f);
		//m_program->setUniformValue("pointLights[2].linear", linear);
		//m_program->setUniformValue("pointLights[2].quadratic", quadratic);
		//// Point light 4
		//m_program->setUniformValue("pointLights[3].position", pointLightPositions[3]);
		//m_program->setUniformValue("pointLights[3].ambient", ambient, ambient, ambient);
		//m_program->setUniformValue("pointLights[3].diffuse", diffuse, diffuse, diffuse);
		//m_program->setUniformValue("pointLights[3].specular", specular, specular, specular);
		//m_program->setUniformValue("pointLights[3].constant", 1.0f);
		//m_program->setUniformValue("pointLights[3].linear", linear);
		//m_program->setUniformValue("pointLights[3].quadratic", quadratic);

#pragma endregion 

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
			/*modelMatrix.setToIdentity();
			modelMatrix.scale(1.5f);
			modelMatrix.translate(0.5, 0.15, 0.5);
			m_program->setUniformValue("modelMatrix", modelMatrix);*/
			models[i]->Draw(m_program);
		}



		modelMatrix.setToIdentity();
		m_program->setUniformValue("modelMatrix", modelMatrix);
		bb->Draw(m_program);

	}
	m_program->release();

	//paintLight();

	/*m_program1->bind();
	{
	m_vao1.bind();
	glDrawArrays(GL_TRIANGLES,0,3);
	m_vao1.release();
	}
	m_program1->release();*/
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


