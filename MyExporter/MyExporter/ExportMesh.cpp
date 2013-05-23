#include "stdafx.h"
#include "ExportMesh.h"
#include "ExportMaterial.h"
#include "MyExporter.h"
#include "ExportConfig.h"


ExpoMesh::ExpoMesh( IGameNode* node )
:ExpoObject()
,m_pNode(node)
,m_mesh(nullptr)
{
	m_name = "E:\\3ds Max 2010\\plugins\\";
	m_name += m_pNode->GetName();
	m_name += ".mesh.xml";
}

bool ExpoMesh::Export()
{
	bool bSucceed = _WriteMesh();

	//finished
	m_pNode->ReleaseIGameObject();
	m_mesh = nullptr;

	return bSucceed;
}

bool ExpoMesh::_WriteMesh()
{
	TiXmlDocument *pXmlDoc = new TiXmlDocument();  
	TiXmlDeclaration* declarationElem = new TiXmlDeclaration(_T("1.0"), _T(""), _T(""));  
	pXmlDoc->LinkEndChild(declarationElem);  

	TiXmlElement* meshElem = new TiXmlElement("mesh");  
	pXmlDoc->LinkEndChild(meshElem);  

	_WriteSubmesh(m_subMesh, meshElem);

	pXmlDoc->SaveFile(m_name.c_str());  
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
	TiXmlElement* submeshNode = new TiXmlElement("submesh");
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

		const unsigned int* indexes = subMesh.faces[iFace].vertexs;
		for (int i = 0; i < 3; i++)
		{  
			memset(stub, 0, 128);  
			sprintf (stub, "v%d", i + 1);  
			face->SetAttribute(stub, indexes[i]);  
		}  
	}

	TiXmlElement* geometry = new TiXmlElement("geometry");  
	submeshNode->LinkEndChild(geometry);
	geometry->SetAttribute("vertexcount", subMesh.vertexList.size());  

	TiXmlElement* vertexbuffer = new TiXmlElement("vertexbuffer");  
	geometry->LinkEndChild(vertexbuffer);
	vertexbuffer->SetAttribute("positions", "true");  
	vertexbuffer->SetAttribute("normals", "true");  
	vertexbuffer->SetAttribute("colours_diffuse", subMesh.bHasDiffuse ? "true" : "false");  
	vertexbuffer->SetAttribute("texture_coords", subMesh.uvCount);

	for (int iUv=0; iUv<subMesh.uvCount; ++iUv)  
	{  
		memset(stub, 0, 128);  
		sprintf(stub, "texture_coords_dimension_%d", iUv);  
		vertexbuffer->SetAttribute(stub, "float2");  
	}  
 
	for (size_t iVert=0; iVert<subMesh.vertexList.size(); ++iVert)
	{  
		const SVertex& vertex = subMesh.vertexList[iVert];

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

		//uv
		for (int ti=0; ti<subMesh.uvCount; ti++) 
		{
			const Point3& uvw = vertex.uv[ti];

			TiXmlElement* uvNode = new TiXmlElement("texcoord");  
			vertNode->LinkEndChild(uvNode);  
			uvNode->SetDoubleAttribute("u", uvw.x);
			uvNode->SetDoubleAttribute("v", 1.0 - uvw.y);
		}
	}

	if (m_subMesh.bSkined)
	{
		//bone assignments
		TiXmlElement* boneassignments = new TiXmlElement("boneassignments");  
		submeshNode->LinkEndChild(boneassignments);

		for (size_t iVert=0; iVert<subMesh.vertexList.size(); ++iVert)
		{
			const auto& weightMap = subMesh.vertexList[iVert].weights;
			for (auto itWt=weightMap.begin(); itWt!=weightMap.end(); ++itWt)
			{
				TiXmlElement* assignNode = new TiXmlElement("vertexboneassignment");  
				boneassignments->LinkEndChild(assignNode);
				assignNode->SetAttribute("vertexindex", iVert);
				assignNode->SetAttribute("boneindex", itWt->first);
				assignNode->SetDoubleAttribute("weight", itWt->second);
			}
		}

		//skeleton link
		TiXmlElement* skelNode = new TiXmlElement("skeletonlink");  
		meshElem->LinkEndChild(skelNode);
		skelNode->SetAttribute("name", m_subMesh.skeletonName.c_str());
	}

	return true;
}

bool ExpoMesh::CollectInfo()
{
	assert(m_pNode);
	IGameObject* pObject = m_pNode->GetIGameObject();  
	assert (pObject->GetIGameType() == IGameObject::IGAME_MESH);
	m_mesh = static_cast<IGameMesh*>(pObject);

	if (!m_mesh->IsEntitySupported())  
	{
		char msg[MAX_PATH];
		sprintf_s(msg, MAX_PATH, "Error: IsEntitySupported() failed!!!	[%s]", m_name.c_str());
		MYEXPORTER.dlgExpo->LogInfo(msg);

		return false;
	}

	//取用数据前必须先InitializeData
	if (!m_mesh->InitializeData())  
	{
		char msg[MAX_PATH];
		sprintf_s(msg, MAX_PATH, "Mesh InitializeData() failed!!!	[%s]", m_name.c_str());
		MYEXPORTER.dlgExpo->LogInfo(msg);

		return false;
	}

	Mesh* mesh = m_mesh->GetMaxMesh();

	if(CONFIG.m_bBuildNormal)
		mesh->buildNormals();

	//收集骨骼信息
	if(m_pNode->GetIGameObject()->IsObjectSkinned() &&
		MYEXPORTER.DoCollectInfo(eExpoType_Skeleton, m_pNode, this))
	{
		m_subMesh.bSkined = true;
		m_subMesh.skeletonName = m_pNode->GetName();
		m_subMesh.skeletonName += ".skeleton";
	}

	int iVertexCount	= m_mesh->GetNumberOfVerts();  
	int iFaceCount		= m_mesh->GetNumberOfFaces();
	int iDiffuseVert	= m_mesh->GetNumberOfColorVerts();
	Tab<int> texMap		= m_mesh->GetActiveMapChannelNum();
	IGameMaterial* mtl	= m_pNode->GetNodeMaterial();

	m_subMesh.bUse32bitindex = iFaceCount > 65535;
	m_subMesh.bHasDiffuse = iDiffuseVert > 0;
	m_subMesh.uvCount = texMap.Count();

	//UV层等于1和大于1要分别处理,这是max的缺陷
	if (texMap.Count() > 1) --m_subMesh.uvCount;

	if(mtl) m_subMesh.matName = mtl->GetMaterialName();
	else	m_subMesh.matName = ExpoConfig::GetSingleton().m_defaultMaterialName;
	
	m_subMesh.faces.resize(iFaceCount);

	//收集顶点信息
	auto& verts = m_subMesh.vertexList;
	for (int iFace=0; iFace<iFaceCount; ++iFace)
	{
		SFace& face = m_subMesh.faces[iFace];
		FaceEx* pFace = m_mesh->GetFace(iFace);

		for (int i = 0; i < 3; i++)
		{
			Point3 pos = m_mesh->GetVertex(pFace->vert[i]) * CONFIG.m_unitScale;
			Point3 normal = m_mesh->GetNormal(pFace, i);

			DWORD index = pFace->vert[i];
			bool bNewVertex = true;
			
			auto iter = m_subMesh.posIdxMap.find(pos);
			if (iter != m_subMesh.posIdxMap.end())
			{
				const auto& idxs = iter->second;
				for (size_t iIdx=0; iIdx<idxs.size(); ++iIdx)
				{
					const SVertex& vert = verts[idxs[iIdx]];
					//顶点坐标和法线都相同,是相同顶点
					if (vert.position == pos && vert.normal == normal)
					{
						index = idxs[iIdx];
						bNewVertex = false;
						break;
					}
				}
			}

			if (bNewVertex)
			{
				SVertex newVertex;
				//position  
				newVertex.position = pos;
				//diffuse 
				newVertex.diffuse = m_mesh->GetColorVertex(index);
				//alpha
				newVertex.alpha = m_mesh->GetAlphaVertex(index);  
				//normal
				newVertex.normal = m_mesh->GetNormal(pFace, i);

				//uv
				int ch = m_subMesh.uvCount > 1 ? 1 : 0;
				for (; ch < texMap.Count(); ch++)
				{
					DWORD idx[3];
					Point3 tv;

					if (m_mesh->GetMapFaceIndex(texMap[ch], iFace, idx))  
						tv = m_mesh->GetMapVertex(texMap[ch], idx[i]);  
					else  
						tv = m_mesh->GetMapVertex(texMap[ch], index);  

					newVertex.uv.push_back(Point3(tv.x, tv.y, tv.z));
				}

				//bone weights
				if(m_subMesh.bSkined)
				{
					const auto& vertAssin = m_subMesh.pSkeleton->GetVertexAssigns();
					newVertex.weights = vertAssin[index].weights;
				}

				verts.push_back(newVertex);
				face.vertexs[i] = verts.size() - 1;
				m_subMesh.posIdxMap[pos].push_back(verts.size() - 1);
			}
			else
			{
				face.vertexs[i] = index;
				m_subMesh.posIdxMap[pos].push_back(index);
			}
		}  
	}

	//收集材质信息
	MYEXPORTER.DoCollectInfo(eExpoType_Material, m_pNode);

	return true;
}


