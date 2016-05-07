#include "FurnitureModel.h"
#include "Parameter.h"

FurnitureModel::FurnitureModel()
{
}

FurnitureModel::FurnitureModel(QString type, QString name, QVector3D translate, QVector3D rotate,
							   QVector<FurnitureLocationType> locationTypes, float scale)
				:Model(Utility::GetFurnitureModelPath(type,name),translate,rotate,scale)
{	
	this->Type = type;
	this->LocationTypes = locationTypes;
	
	updateTranslation();
	updateFrontDirection(rotate);	
	this->OrderMaterialByMeshArea();
	this->UpdateMeshMaterials();
	this->SetModelMatrix();
	
	this->UpdateBoundingBoxWorldCoordinates();
	this->DetectSupportRegions(); 
}

void FurnitureModel::SetModelMatrix()
{
	modelMatrix.setToIdentity();
	modelMatrix.translate(m_translate);
	modelMatrix.scale(m_scale);
	modelMatrix.rotate(m_rotate.x(), 1, 0, 0);
	modelMatrix.rotate(m_rotate.y(), 0, 1, 0);
	modelMatrix.rotate(m_rotate.z(), 0, 0, 1);
}


void FurnitureModel::DetectSupportRegions()
{
	QMap<float, QVector<QVector3D>> all_support_vertices;
	for (size_t i = 0; i < this->meshes.size(); i++)
	{
		Mesh *mesh = this->meshes[i];
		for (size_t j = 0; j < mesh->Vertices.size(); j++)
		{
			//  0. test for up direction
			if (abs(mesh->Vertices[j].normal().x()) == 0 && abs(mesh->Vertices[j].normal().y()) == 1 
				&& abs(mesh->Vertices[j].normal().z()) == 0)
			{
				// 1. group vertices according to vertex y position
				float height = mesh->Vertices[j].position().y();
				if (!all_support_vertices.contains(height))
				{
					QVector<QVector3D> vertices;
					vertices.push_back(mesh->Vertices[j].position());
					all_support_vertices[height] = vertices;
				}
				else
				{
					all_support_vertices[height].push_back(mesh->Vertices[j].position());
				}
			}
		}
	}

	// 2. sort support layer according to vertex height	
	QMapIterator<float, QVector<QVector3D>> it(all_support_vertices);
	QVector<QPair<float, QVector<QVector3D>>> support_layers;			
	while (it.hasNext())
	{
		it.next();
		support_layers.push_front(QPair<float, QVector<QVector3D>>(it.key(), it.value()));					
	}
	
	// 3. filter out layers that are too small or too close to last layer
	float bbwidth = this->boundingBox->Width();
	float bbdepth = this->boundingBox->Depth();
	float bbheight = this->boundingBox->Height();
	int support_num = 1; //其他家具默认只有一个摆放区域
	if (this->Type.compare("Cabinet",Qt::CaseInsensitive) == 0) // 橱柜单独处理,可以有多层
	{
		support_num = 10;
	}
	
	float last_height = this->boundingBox->RightUpFront().y() + bbheight / support_num;

	for (size_t i = 0; i < support_layers.size(); i++)
	{
		float min_x = 0, max_x = 0, min_z = 0, max_z = 0;
		float cur_height = support_layers[i].first;
		for (size_t j = 0; j < support_layers[i].second.size(); j++)
		{
			QVector3D &v = support_layers[i].second[j];
			if (min_x > v.x())			
				min_x = v.x();			
			if (max_x < v.x())
				max_x = v.x();
			if (min_z > v.z())
				min_z = v.z();
			if (max_z < v.z())
				max_z = v.z();			
		}
		if (max_x - min_x >= bbwidth/4 && max_z - min_z >= bbdepth/4	// filter out small layers
			&&  abs(last_height - cur_height) >= (bbheight / (support_num + 1))) // two layers should not too close in height
		{
			this->support_regions.push_back(new SupportRegion(min_x, max_x, min_z, max_z, cur_height, modelMatrix));
			last_height = cur_height;
			if (this->support_regions.size() >= support_num)
			{
				break;
			}
		}
	}
}


QVector3D FurnitureModel::GetRelativePosition(DecorationModel* model)
{
	QVector3D translate;
	
	return translate;
}


void FurnitureModel::updateTranslation()
{
	Assets *assets = Assets::GetAssetsInstance();
	for (size_t i = 0; i < LocationTypes.size(); i++)
	{		
		switch (LocationTypes[i])
		{
		case FTBottom:
			this->m_translate.setY(boundingBox->Height() / 2 * m_scale + 0.05f);		
			break;
		case FTUp:
			this->m_translate.setY(assets->RoomHeight - boundingBox->Height()/2 * m_scale - 0.01f);
			break;
		case FTLeft:
			this->m_translate.setX(boundingBox->Width() / 2 * m_scale + 0.01f);
			break;
		case FTRight:
			this->m_translate.setX(assets->RoomWidth - boundingBox->Width() / 2 * m_scale - 0.01f);
			break;
		case FTFront:
			this->m_translate.setZ(assets->RoomDepth - boundingBox->Width() / 2 * m_scale - 0.01f);
			break;
		case FTBack:
			if (m_rotate.y() / 90 == 1 || m_rotate.y() / 90 == 3) // 旋转了			
				this->m_translate.setZ(boundingBox->Width() / 2.0 * m_scale + 0.01f);
			else // 没有旋转，如墙壁
				this->m_translate.setZ(boundingBox->Depth() / 2.0 * m_scale + 0.01f);			
			break;
		default:
			break;
		}
	}	
}

void FurnitureModel::updateFrontDirection(QVector3D& rotate)
{
	float x = rotate.x();
	float y = rotate.y();
	float z = rotate.z();

	if (x==0 && y==0 && z==0)	
		furniture_front_direction = XPos;	
	else if(x == 0 && y == 90 && z == 0 )
		furniture_front_direction = ZPos;
	else if (x == 0 && y == 180 && z==0)
		furniture_front_direction = XNeg;
	else if (x == 0 && y==270 && z==0)
		furniture_front_direction = ZNeg;
	else
		furniture_front_direction = Invalid;
}

int compare(const QPair<QString, float> &a1, const QPair<QString, float> &a2)
{
	return a1.second > a2.second;
}

void FurnitureModel::OrderMaterialByMeshArea()
{
	QMap<QString, float> tmp;
	QVector<QPair<QString, float>> materials;

	for (size_t i = 0; i < this->meshes.size(); i++)
	{
		QString materialName = this->meshes[i]->MeshMaterial->Name;
		tmp[materialName] += this->meshes[i]->GetArea();
	}

	QMapIterator<QString, float> it(tmp);
	while (it.hasNext())
	{
		it.next();
		materials.push_back(QPair<QString, float>(it.key(), it.value()));
	}

	std::sort(materials.begin(), materials.end(), compare);
	for (size_t i = 0; i < materials.size(); i++)
	{
		ordered_materials.push_back(material_assets[materials[i].first]);
	}
}

void FurnitureModel::UpdateMeshMaterials()
{
	Assets *assets = Assets::GetAssetsInstance();
	if (assets->MaterialMap.contains(Type))
	{
		QVector<QString> colorNames = assets->MaterialMap[Type];
		int n = colorNames.size();
		for (size_t i = 0; i < this->ordered_materials.size(); i++)
		{
			QString color = colorNames[i%n];
			if (Utility::QStrIsImagePath(color))
			{
				ordered_materials[i]->Diffuse->UseMap = true;
				QOpenGLTexture *gl_texture;
				gl_texture = new QOpenGLTexture(QImage(color).mirrored());
				//texture->setAutoMipMapGenerationEnabled(true);
				gl_texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
				gl_texture->setMagnificationFilter(QOpenGLTexture::Linear);
				gl_texture->setWrapMode(QOpenGLTexture::MirroredRepeat);

				Texture *texture = new Texture();
				texture->texture = gl_texture;
				texture->type = DiffuseTexture;

				QVector<Texture*> tmptextures;
				tmptextures.push_back(texture);
				ordered_materials[i]->Diffuse->Textures = tmptextures;
			}
			else
			{
				ordered_materials[i]->Diffuse->UseMap = false;
				ordered_materials[i]->Diffuse->Color = assets->GetColorByName(colorNames[i%n]);
			}
		}
	}
}

void FurnitureModel::UpdateMeshMaterials(ColorPalette* color_palette)
{
	Parameter *para = Parameter::GetParameterInstance();
	QVector<QColor> colors = color_palette->Colors;
	for (size_t i = 0; i < this->ordered_materials.size(); i++)
	{
		if (para->FurnitureTypesUseTextures.contains(this->Type)) // use texture
		{
			ordered_materials[i]->Diffuse->UseMap = true;			
			QVector<Texture*> tmptextures;
			Texture *t = Utility::GetNearestColorTexture(this->Type, color_palette);
			assert(t != nullptr);
			tmptextures.push_back(t);
			ordered_materials[i]->Diffuse->Textures = tmptextures;
		}
		else
		{
			ordered_materials[i]->Diffuse->UseMap = false;
			QColor color = colors[i%colors.size()];
			ordered_materials[i]->Diffuse->Color = QVector3D(color.red() / 255.0, color.green() / 255.0, color.blue() / 255.0);
		}
	}
}

void FurnitureModel::AddDecorationModel(DecorationModel* model)
{
	decoration_models.push_back(model);
}

void FurnitureModel::UpdateDecorationLayout()
{
	int layer = this->support_regions.size() - 1;
	SupportRegion *support_region;
	for (size_t i = 0; i < decoration_models.size(); i++)
	{
		support_region = this->support_regions[layer];
		DecorationModel *model = decoration_models[i];

		AdaptDecorationLocationType(model);
		float tx = 0, tz = 0;
		for (size_t j = 0; j < model->LocationTypes.size(); j++)
		{
			switch (model->LocationTypes[j])
			{
			case NotSet:
				tx = model->GetRelativeTranslate().x();
				tz = model->GetRelativeTranslate().z();
				break;
			/*case Center:
				tx = 0;
				tz = 0;*/
			case Left:
				tx = -(this->boundingBox->Width() / 2 * m_scale
					- (abs(this->boundingBox->WorldLeftBottomBack().x() - support_region->MinX))
					- model->boundingBox->Width() / 2 * model->GetScale());				
				break;
			case Right:
				tx = this->boundingBox->Width() / 2 * m_scale
					- abs(this->boundingBox->WorldRightUpFront().x() - support_region->MaxX)
					- model->boundingBox->Width() / 2 * model->GetScale();				
				break;
			case Back:
				tz = -(this->boundingBox->Depth() / 2 * m_scale
					- abs(this->boundingBox->WorldLeftBottomBack().z() - support_region->MinZ)
					- model->boundingBox->Depth() / 2 * model->GetScale());				
				break;
			case Front:
				tz = this->boundingBox->Depth() / 2 * m_scale
					- abs(this->boundingBox->WorldRightUpFront().z() - support_region->MaxZ)
					- model->boundingBox->Depth() / 2 * model->GetScale();				
				break;
			default:
				break;
			}
		}		
		AdaptTranslateAccord2FrontDirection(tx,tz); // 根据家具的朝向调整translate
		float ty = support_region->Height - this->m_translate.y()  + abs(model->boundingBox->LeftBottomBack().y())*model->GetScale();
		model->SetRelativeTranslate(tx, ty, tz);
		
		if (!support_region->TryPutDecorationModel(model)) // 如果一直放不上去，会卡在这里
		{
			if (layer == 0)
			{
				break;
			}
			i--; // replace;
			if (layer > 0 && support_regions.size() > 1)
				layer--;

		}
		else // place successfully
			// if (not highest layer && multilayer && (modelnum >= 2 || take up too much area))
		{	if (layer > 0 && support_regions.size() > 1
				&& (support_region->ModelNum >= 2 || !support_region->IsSpaceEnough()))
			{
				layer--;
			}
		}		
	}	
}

void FurnitureModel::ClearDecorationLayout()
{
	decoration_models.clear();
	for (size_t i = 0; i < this->support_regions.size(); i++)
	{
		support_regions[i]->Clear();
	}
}

void FurnitureModel::AdaptDecorationLocationType(DecorationModel *model) const
{
	for (size_t i = 0; i < model->LocationTypes.size(); i++)
	{
		switch (furniture_front_direction)
		{
		case Invalid:
			break;
		case XPos:
			if (model->LocationTypes[i] == Left) model->LocationTypes[i] = Front;
			else if (model->LocationTypes[i] == Right) model->LocationTypes[i] = Back;
			else if (model->LocationTypes[i] == Front) model->LocationTypes[i] = Right;
			else if (model->LocationTypes[i] == Back) model->LocationTypes[i] = Left;
			else model->LocationTypes[i] = model->LocationTypes[i]; // keep unchanged
			break;
		case XNeg:
			if (model->LocationTypes[i] == Left) model->LocationTypes[i] = Back;
			else if (model->LocationTypes[i] == Right) model->LocationTypes[i] = Front;
			else if (model->LocationTypes[i] == Front) model->LocationTypes[i] = Left;
			else if (model->LocationTypes[i] == Back) model->LocationTypes[i] = Right;
			else model->LocationTypes[i] = model->LocationTypes[i]; // keep unchanged
			break;
		case ZPos:
			break;
		case ZNeg:
			if (model->LocationTypes[i] == Left) model->LocationTypes[i] = Right;
			else if (model->LocationTypes[i] == Right) model->LocationTypes[i] = Left;
			else if (model->LocationTypes[i] == Front) model->LocationTypes[i] = Back;
			else if (model->LocationTypes[i] == Back) model->LocationTypes[i] = Front;
			else model->LocationTypes[i] = model->LocationTypes[i]; // keep unchanged
			break;
		default:
			break;
		}
	}
}

void FurnitureModel::AdaptTranslateAccord2FrontDirection(float& tx, float& tz)
{
	float tmpx = tx;
	float tmpz = tz;
	switch (furniture_front_direction)
	{
	case Invalid:
		break;
	case XPos:		
		break;
	case XNeg:
		tx = -tmpx;
		tz = -tmpz;
		break;
	case ZPos:
		tx = tmpz;
		tz = -tmpx;
		break;
	case ZNeg:
		tx = tmpz;
		tz = tmpx;
		break;
	default:
		break;
	}
}

QVector3D& FurnitureModel::getTranslate(float x, float y, float z)
{
	float tx = 0, ty = 0, tz = 0;
	switch (furniture_front_direction)
	{
	case Invalid:
		break;
	case XPos:
		
		break;
	case XNeg:
		break;
	case ZPos:
		break;
	case ZNeg:
		break;
	default:
		break;
	}
	return QVector3D(tx, ty, tz);
}
