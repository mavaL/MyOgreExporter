/********************************************************************
	created:	13:5:2013   10:24
	filename	ExportObject.h
	author:		maval

	purpose:	导出各类型物体基类
*********************************************************************/
#ifndef ExportObject_h__
#define ExportObject_h__

enum eExpoType
{
	eExpoType_Mesh,
	eExpoType_Material,
	eExpoType_Skeleton,
	eExpoType_Clip
};

class ExpoObject
{
public:
	ExpoObject();
	virtual ~ExpoObject() {}

public:
	virtual bool		Export() = 0;
	virtual bool		CollectInfo() = 0;
	virtual eExpoType	GetType() = 0;

	const std::string&	GetName() const { return m_name; }
	void				SetName(const std::string& name) { m_name = name; } 

protected:
	eExpoType			m_type;
	std::string			m_name;
};

#endif // ExportObject_h__