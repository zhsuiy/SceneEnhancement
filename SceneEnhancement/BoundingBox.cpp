#include "BoundingBox.h"

BoundingBox::BoundingBox(QVector3D leftBottomBack, QVector3D rightUpFront) :Mesh()
{
	this->m_left_bottom_back = leftBottomBack;
	this->m_right_up_front = rightUpFront;
	setupData();
	setupRender();

}

BoundingBox::~BoundingBox()
{
}

void BoundingBox::Draw(QOpenGLShaderProgram* program)
{
	setupShaderProgram(program);
	VAO.bind();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glDrawElements(GL_TRIANGLES, this->Indices.size() , GL_UNSIGNED_INT, 0);	
	glDrawElements(GL_LINES, this->Indices.size(), GL_UNSIGNED_INT, 0);
	VAO.release();
}

void BoundingBox::setupData()
{
	updateVertices();
	updateTextures();
	// indices will not change
	if (Indices.isEmpty())
	{
		GLuint arr_indices[] = {
			0,1,
			1,2,
			2,3,
			3,0,
			4,5,
			5,6,
			6,7,
			7,4,
			0,4,
			3,7,
			1,5,
			2,6
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

	// x
	Vertices.push_back(Vertex(QVector3D(neg_x, neg_y, pos_z), QVector3D(0, 0, 1))); // front
	Vertices.push_back(Vertex(QVector3D(pos_x, neg_y, pos_z), QVector3D(0, 0, 1)));
	Vertices.push_back(Vertex(QVector3D(pos_x, pos_y, pos_z), QVector3D(0, 0, 1)));
	Vertices.push_back(Vertex(QVector3D(neg_x, pos_y, pos_z), QVector3D(0, 0, 1)));

	Vertices.push_back(Vertex(QVector3D(neg_x, neg_y, neg_z), QVector3D(0, 0, 1))); // back
	Vertices.push_back(Vertex(QVector3D(pos_x, neg_y, neg_z), QVector3D(0, 0, 1)));
	Vertices.push_back(Vertex(QVector3D(pos_x, pos_y, neg_z), QVector3D(0, 0, 1)));
	Vertices.push_back(Vertex(QVector3D(neg_x, pos_y, neg_z), QVector3D(0, 0, 1)));
}

void BoundingBox::updateTextures()
{
	MeshMaterial = new Material();
	MeshMaterial->Diffuse = new MaterialElement(QVector3D(1.0,0.5,1.0));
}
