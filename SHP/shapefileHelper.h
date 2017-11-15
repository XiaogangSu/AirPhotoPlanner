#ifndef SHAPEFILE_HELPER_H
#define SHAPEFILE_HELPER_H

#include "shapefil.h"

#define FILE_MODE_READ_WRITE    "rb+"
#define FILE_MODE_READ          "rb"

class ShpHandleCollector
{
public:
	ShpHandleCollector(){ m_shpHandle = NULL; }
	ShpHandleCollector(SHPHandle& pShpHandle)
	{
		m_shpHandle = pShpHandle;
	}
	void SetShpInstance(SHPHandle pShpHandle)
	{
		m_shpHandle = pShpHandle;
	}
	~ShpHandleCollector()
	{
		SHPClose(m_shpHandle);
	}
private:
	SHPHandle m_shpHandle;
};

class DbfHandleCollector
{
public:
	DbfHandleCollector(){m_dbfHandle = NULL;}
	DbfHandleCollector(DBFHandle pDbfHandle)
	{
		m_dbfHandle = pDbfHandle;
	}
	void SetDbfInstance(DBFHandle pDbfHandle)
	{
		m_dbfHandle = pDbfHandle;
	}
	~DbfHandleCollector()
	{
		DBFClose(m_dbfHandle);
	}
private:
	DBFHandle m_dbfHandle;
};
#endif