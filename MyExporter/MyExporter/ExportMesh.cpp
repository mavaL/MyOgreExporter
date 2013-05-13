#include "stdafx.h"
#include "ExportMesh.h"


ExpoMesh::ExpoMesh( IGameNode* node )
:ExpoObject(node)
,m_mesh(nullptr)
{

}

bool ExpoMesh::Export()
{
	assert(m_pNode);

	IGameObject* pObject = m_pNode->GetIGameObject();  
	assert (pObject->GetIGameType() == IGameObject::IGAME_MESH);
	m_mesh = (IGameMesh*) pObject;
	
	return _WriteMesh();
}

bool ExpoMesh::_WriteMesh()
{
	if (!m_mesh->IsEntitySupported())  
	{
		//TODO: use log file
		MessageBox (NULL, "Entity not supported", "Ogre exporter", MB_OK);
		return false;
	}

	//取用数据前必须先InitializeData
	if (!m_mesh->InitializeData())  
	{
		MessageBox (NULL, "Mesh InitializeData() failed!", "Ogre exporter", MB_OK);
		return false;
	}

	_CollectInfo();

	TiXmlDocument *pXmlDoc = new TiXmlDocument();  
	TiXmlDeclaration* declarationElem = new TiXmlDeclaration(_T("1.0"), _T(""), _T(""));  
	pXmlDoc->LinkEndChild(declarationElem);  

	TiXmlElement* meshElem = new TiXmlElement("mesh");  
	pXmlDoc->LinkEndChild(meshElem);  

	for	(size_t i=0; i<m_subMeshes.size(); ++i)
		_WriteSubmesh(m_subMeshes[i], meshElem);

	pXmlDoc->SaveFile("E:\\3ds Max 2010\\plugins\\object.xml");  
	delete pXmlDoc;

	return true;
}

bool ExpoMesh::_WriteSubmesh( const SSubMesh& subMesh, TiXmlElement* xmlParent )
{
	TiXmlElement* meshElem = xmlParent;

	// <submeshes> </submeshes>  
	TiXmlElement* submeshes = new TiXmlElement("submeshes");  
	meshElem->LinkEndChild(submeshes);

	// <submeshNode> </submeshNode>  
	TiXmlElement* submeshNode = new TiXmlElement("submeshNode");  
	submeshes->LinkEndChild(submeshNode);
 
	submeshNode->SetAttribute("material", subMesh.matName.c_str());  
	submeshNode->SetAttribute("usesharedvertices", "false");  
	submeshNode->SetAttribute("use32bitindexes", subMesh.bUse32bitindex ? "true" : "false");
	submeshNode->SetAttribute("operationtype", "triangle_list");

	// <faces> </faces>  
	TiXmlElement* faces = new TiXmlElement("faces");  
	submeshNode->LinkEndChild(faces);  

	int nFace = (int)subMesh.faces.size();
	faces->SetAttribute("count", nFace);  

	char stub[128];
	for (int iFace=0; iFace<nFace; iFace++)  
	{  
		TiXmlElement* face = new TiXmlElement("face");  
		faces->LinkEndChild(face);  

		for (int i = 0; i < 3; i++)
		{  
			memset(stub, 0, 128);  
			sprintf (stub, "v%d", i+1);  
			face->SetAttribute(stub, iFace * 3 + i);  
		}  
	}

	TiXmlElement* geometry = new TiXmlElement("geometry");  
	submeshNode->LinkEndChild(geometry);  

	int nVert = nFace * 3;
	geometry->SetAttribute("vertexcount", nVert);  

	TiXmlElement* vertexbuffer = new TiXmlElement("vertexbuffer");  
	geometry->LinkEndChild(vertexbuffer);  

	vertexbuffer->SetAttribute("positions", "true");  
	vertexbuffer->SetAttribute("normals", "true");  
	vertexbuffer->SetAttribute("colours_diffuse", subMesh.bHasDiffuse ? "true" : "false");  
	vertexbuffer->SetAttribute("texture_coords", 0);

// 	for (index = 0; index < texMap.Count(); index++)  
// 	{  
// 		memset(stub, 0, 128);  
// 		sprintf(stub, "texture_coords_dimension_%d", index);  
// 		vertexbuffer->SetAttribute(stub, "2");  
// 	}  
 
	for (int iFace=0; iFace<nFace; iFace++)
	{  
		for (int i = 0; i < 3; i++)
		{
			const SVertex& vertex = subMesh.faces[iFace].vertexs[i];

			TiXmlElement* vertNode = new TiXmlElement("vertex");  
			vertexbuffer->LinkEndChild(vertNode);  

			//position  
			TiXmlElement* position = new TiXmlElement("position");  
			vertNode->LinkEndChild(position);  

			position->SetDoubleAttribute("x", vertex.position.x);  
			position->SetDoubleAttribute("y", vertex.position.y);  
			position->SetDoubleAttribute("z", vertex.position.z);  

			//diffuse  
			TiXmlElement* colour_diffuse = new TiXmlElement("colour_diffuse");  
			vertNode->LinkEndChild(colour_diffuse);  

			memset(stub, 0, 128);  
			sprintf(stub, "    %f    %f    %f", vertex.diffuse.x, vertex.diffuse.y, vertex.diffuse.z);  
			colour_diffuse->SetAttribute("value", stub);  

			//normal
			TiXmlElement* normal = new TiXmlElement("normal");  
			vertNode->LinkEndChild(normal);  

			normal->SetDoubleAttribute("x", vertex.normal.x);
			normal->SetDoubleAttribute("y", vertex.normal.y);
			normal->SetDoubleAttribute("z", vertex.normal.z);
		}
	}

	return true;
}

void ExpoMesh::_CollectInfo()
{
	Mesh* mesh = m_mesh->GetMaxMesh();
	mesh->buildNormals();

// 	//遍历所有面确定材质个数
// 	std::set<int> matIds;
// 	int nFaces = m_mesh->GetNumberOfFaces();
// 	for (int iFace=0; iFace<nFaces; ++iFace)
// 	{
// 		FaceEx* pFace = m_mesh->GetFace(iFace);
// 		matIds.insert(pFace->matID);
// 	}

	int iVertexCount	= m_mesh->GetNumberOfVerts();  
	int iFaceCount		= m_mesh->GetNumberOfFaces();
	int iDiffuseVert	= m_mesh->GetNumberOfColorVerts();
	Tab<int> texMap		= m_mesh->GetActiveMapChannelNum();
	IGameMaterial* mtl	= m_pNode->GetNodeMaterial();

	SSubMesh submesh;
	submesh.bUse32bitindex = iFaceCount > 65535;
	submesh.bHasDiffuse = iDiffuseVert > 0;
	if(mtl) 
		submesh.matName = mtl->GetMaterialName();
	submesh.faces.resize(iFaceCount);

	for (int iFace=0; iFace<iFaceCount; ++iFace)
	{
		SFace& face = submesh.faces[iFace];
		FaceEx* pFace = m_mesh->GetFace(iFace);

		for (int i = 0; i < 3; i++)
		{  
			//position  
			face.vertexs[i].position = m_mesh->GetVertex(pFace->vert[i]);  
			//diffuse 
			face.vertexs[i].diffuse = m_mesh->GetColorVertex(pFace->vert[i]);
			//alpha
			face.vertexs[i].alpha = m_mesh->GetAlphaVertex(pFace->vert[i]);  
			//normal
			face.vertexs[i].normal = m_mesh->GetNormal(pFace, i);

// 			DWORD idx[3];  
// 			Point3 tv;  
// 			for (int ch = 0; ch < texMap.Count(); ch++)  
// 			{  
// 
// 				if (m_mesh->GetMapFaceIndex(texMap[ch], index, idx))  
// 					tv = m_mesh->GetMapVertex(texMap[ch], idx[i]);  
// 				else  
// 					tv = m_mesh->GetMapVertex(texMap[ch], pFace->vert[i]);  
// 
// 				m_tvMap.insert(std::make_pair(texMap[ch], tv));  
// 			}   
		}  
	}

	m_subMeshes.push_back(std::move(submesh));
}


