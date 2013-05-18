#include "stdafx.h"
#include "ExportMaterial.h"
#include "MyExporter.h"
#include "ExportConfig.h"

ExpoMaterial::ExpoMaterial( IGameNode* node )
:m_stream("")
,m_node(node)
{
	m_material = m_node->GetNodeMaterial();
	assert(m_node->GetIGameObject()->GetIGameType() == IGameObject::IGAME_MESH && "Construct ExpoMaterial with IGameMesh!");

	m_name = "E:\\3ds Max 2010\\plugins\\";
	m_name += m_node->GetName();
	m_name += ".material";
}

bool ExpoMaterial::Export()
{
	_CollectInfo();

	std::ofstream materialFile;
	materialFile.open(m_name.c_str(), std::ios::out);

	if (!materialFile.is_open())
		return false;

	materialFile << m_stream;

	materialFile.close();

	return true;
}

void ExpoMaterial::_CollectInfo()
{
	std::stringstream of;
	std::string matName = m_material ? m_material->GetMaterialName() : CONFIG.m_defaultMaterialName;
	
	of << "material " << matName << std::endl;
	of << std::showpoint;
	of << "{" << std::endl;

	of << "\ttechnique" << std::endl;
	of << "\t{" << std::endl;

	int numSubMtl = 0;

	if (m_material != NULL) 
	{
		numSubMtl = m_material->GetSubMaterialCount();

		if (numSubMtl > 0) 
		{
			int i;
			for (i=0; i<numSubMtl; i++) 
			{
				_StreamPass(of, m_material->GetSubMaterial(i));
			}
		}
		else
			_StreamPass(of, m_material);
	}
	else 
	{
		_StreamPass(of, m_material);
	}

	of << "\t}" << std::endl;
	of << "}" << std::endl;

	m_stream = of.str();
}

bool ExpoMaterial::_StreamPass( std::ostream &of, IGameMaterial *mtl )
{
	of << "\t\tpass" << std::endl;
	of << "\t\t{" << std::endl;

	if(!mtl || !mtl->IsEntitySupported())
	{
		of << "\t\t}" << std::endl;
		return true;
	}

	Point4 val4;
	Point3 val3;
	PropType pt;
	IGameProperty* p = mtl->GetAmbientData();

	if (p) 
	{
		pt = p->GetType();

		if (pt == IGAME_POINT3_PROP) 
		{
			p->GetPropertyValue(val3);
			of << "\t\t\tambient " << val3.x << " " << val3.y << " " << val3.z << " " << std::endl;
		}

		if (pt == IGAME_POINT4_PROP) 
		{
			p->GetPropertyValue(val4);
			of << "\t\t\tambient " << val4.x << " " << val4.y << " " << val4.z << " " << val4.w << " " << std::endl;
		}
	}

	p = mtl->GetDiffuseData();
	if (p) 
	{
		pt = p->GetType();

		if (pt == IGAME_POINT3_PROP) 
		{
			p->GetPropertyValue(val3);
			of << "\t\t\tdiffuse " << val3.x << " " << val3.y << " " << val3.z << " " << std::endl;
		}

		if (pt == IGAME_POINT4_PROP) 
		{
			p->GetPropertyValue(val4);
			of << "\t\t\tdiffuse " << val4.x << " " << val4.y << " " << val4.z << " " << val4.w << " " << std::endl;
		}
	}

	p = mtl->GetSpecularData();
	if (p) 
	{
		pt = p->GetType();

		if (pt == IGAME_POINT3_PROP) 
		{
			p->GetPropertyValue(val3);
			of << "\t\t\tspecular " << val3.x << " " << val3.y << " " << val3.z << " " << std::endl;
		}

		if (pt == IGAME_POINT4_PROP) 
		{
			p->GetPropertyValue(val4);
			of << "\t\t\tspecular " << val4.x << " " << val4.y << " " << val4.z << " " << val4.w << " " << std::endl;
		}
	}

	p = mtl->GetEmissiveData();
	if (p) 
	{
		pt = p->GetType();

		if (pt == IGAME_POINT3_PROP) 
		{
			p->GetPropertyValue(val3);
			of << "\t\t\temissive " << val3.x << " " << val3.y << " " << val3.z << " " << std::endl;
		}

		if (pt == IGAME_POINT4_PROP) 
		{
			p->GetPropertyValue(val4);
			of << "\t\t\temissive " << val4.x << " " << val4.y << " " << val4.z << " " << val4.w << " " << std::endl;
		}
	}

	int numTexMaps = mtl->GetNumberOfTextureMaps();
	for (int texMapIdx = 0; texMapIdx < numTexMaps; texMapIdx++) 
	{
		IGameTextureMap* tmap = mtl->GetIGameTextureMap(texMapIdx);
		if (tmap) 
		{
			of << "\n\t\t\ttexture_unit " << std::endl;
			of << "\t\t\t{" << std::endl;

			std::string bmap(tmap->GetBitmapFileName());
			bmap = bmap.substr(bmap.find_last_of('\\') + 1);
			of << "\t\t\t\ttexture " << bmap << std::endl;
			of << "\t\t\t}" << std::endl;
		}
	}

	of << "\t\t}" << std::endl;

	return true;
}
