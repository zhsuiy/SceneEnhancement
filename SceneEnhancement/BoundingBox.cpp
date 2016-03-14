#include "BoundingBox.h"

BoundingBox::BoundingBox( QVector3D leftBottomBack, QVector3D rightUpFront):Mesh()
{	
	this->m_left_bottom_back = leftBottomBack;
	this->m_right_up_front = rightUpFront;
	setupData();
	setupRender();
	
}

BoundingBox::~BoundingBox()
{
}

void BoundingBox::setupData()
{
	updateVertices();
	updateTextures();
	// indices will not change
	if (Indices.isEmpty())
	{
		GLuint arr_indices[] = {
			0,1,2, // First Triangle
			0,2,3,
			4,5,6,
			4,6,7,
			8,9,10,
			8,10,11,
			12,13,14,
			12,14,15
		};
		for (size_t i = 0; i < 24; i++)
		{
			Indices.push_back(arr_indices[i]);
		}
	}	
}

void BoundingBox::updateVertices()
{
	float neg_x = m_left_bottom_back.x();
	float neg_y = m_left_bottom_back.y();
	float neg_z = m_left_bottom_back.z();
	float pos_x = m_right_up_front.x();
	float pos_y = m_right_up_front.y();
	float pos_z = m_right_up_front.z();
	Vertices.clear();
	Vertices.push_back(Vertex(QVector3D(neg_x, neg_y, neg_z), QVector3D(0, 0, 1), QVector2D(0, 0))); // back
	Vertices.push_back(Vertex(QVector3D(pos_x, neg_y, neg_z), QVector3D(0, 0, 1), QVector2D(1, 0)));
	Vertices.push_back(Vertex(QVector3D(pos_x, pos_y, neg_z), QVector3D(0, 0, 1), QVector2D(1, 1)));
	Vertices.push_back(Vertex(QVector3D(neg_x, pos_y, neg_z), QVector3D(0, 0, 1), QVector2D(0, 1)));

	Vertices.push_back(Vertex(QVector3D(neg_x, neg_y, pos_z), QVector3D(1, 0, 0), QVector2D(0, 0))); // left
	Vertices.push_back(Vertex(QVector3D(neg_x, neg_y, neg_z), QVector3D(1, 0, 0), QVector2D(1, 0)));
	Vertices.push_back(Vertex(QVector3D(neg_x, pos_y, neg_z), QVector3D(1, 0, 0), QVector2D(1, 1)));
	Vertices.push_back(Vertex(QVector3D(neg_x, pos_y, pos_z), QVector3D(1, 0, 0), QVector2D(0, 1)));	

	Vertices.push_back(Vertex(QVector3D(neg_x, neg_y, pos_z), QVector3D(0, 1, 0), QVector2D(0, 0))); // bottom
	Vertices.push_back(Vertex(QVector3D(pos_x, neg_y, pos_z), QVector3D(0, 1, 0), QVector2D(1, 0)));
	Vertices.push_back(Vertex(QVector3D(pos_x, neg_y, neg_z), QVector3D(0, 1, 0), QVector2D(1, 1)));
	Vertices.push_back(Vertex(QVector3D(neg_x, neg_y, neg_z), QVector3D(0, 1, 0), QVector2D(0, 1)));

	Vertices.push_back(Vertex(QVector3D(neg_x, pos_y, pos_z), QVector3D(0, 1, 0), QVector2D(0, 0))); // top
	Vertices.push_back(Vertex(QVector3D(pos_x, pos_y, pos_z), QVector3D(0, 1, 0), QVector2D(1, 0)));
	Vertices.push_back(Vertex(QVector3D(pos_x, pos_y, neg_z), QVector3D(0, 1, 0), QVector2D(1, 1)));
	Vertices.push_back(Vertex(QVector3D(neg_x, pos_y, neg_z), QVector3D(0, 1, 0), QVector2D(0, 1)));
}

void BoundingBox::updateTextures()
{
	QOpenGLTexture *gl_texture;
	QString filename = "./Resources/texture/floor.jpg";
	gl_texture = new QOpenGLTexture(QImage(filename).mirrored());
	//texture->setAutoMipMapGenerationEnabled(true);
	gl_texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
	gl_texture->setMagnificationFilter(QOpenGLTexture::Linear);
	gl_texture->setWrapMode(QOpenGLTexture::MirroredRepeat);

	Texture *texture = new Texture();
	texture->texture = gl_texture;
	texture->type = DiffuseTexture;
	
	QVector<Texture*> tmptextures;
	tmptextures.push_back(texture);

	MeshMaterial = new Material();
	MeshMaterial->Diffuse = new MaterialElement(tmptextures);

	
	
	//this->Textures.push_back(texture);
	
}
