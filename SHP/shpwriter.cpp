#include "shpwriter.h"

Shpwriter::Shpwriter()
{
}

Shpwriter::~Shpwriter()
{

}

int Shpwriter::init(const std::string& strFileName)
{
     _shape_type = SHPT_POINTZ;
    _h_shp = SHPCreate(strFileName.c_str(), _shape_type);
    if (_h_shp == NULL)
    {
        printf("Create shape file failed:%s\n", strFileName.c_str());
        return -1;
    }
    _h_dbf = DBFCreate(strFileName.c_str());
    if (_h_dbf == NULL)
    {
        printf("Create dbf file failed:%s\n", strFileName.c_str());
        return -1;
    }
    DBFAddField(_h_dbf, "Id", FTInteger, 128, 0);
	DBFAddField(_h_dbf, "Type", FTInteger, 128, 0);
	DBFAddField(_h_dbf, "TypeName", FTString, 128, 0);
    return 0;
}

void Shpwriter::close_files()
{
    SHPClose(_h_shp);
    DBFClose(_h_dbf);
}

int Shpwriter::add_record(const std::vector<SHP_Point>& pt_vec)
{
	std::vector<SHP_Point>::const_iterator iter = pt_vec.begin();
	for (; iter != pt_vec.end(); iter++)
	{
		SHPObject *p_sobject = SHPCreateSimpleObject(_shape_type, 1, &iter->X, &iter->Y, &iter->Z);
		if (p_sobject != NULL)
		{
			int n = SHPWriteObject(_h_shp, -1, p_sobject);
			SHPDestroyObject(p_sobject);

			int i_record = DBFGetRecordCount(_h_dbf);
			DBFWriteIntegerAttribute(_h_dbf, i_record, 0, iter->id);
			DBFWriteIntegerAttribute(_h_dbf, i_record, 1, iter->PointType);
			std::string name = get_Name(iter->PointType);
			DBFWriteStringAttribute(_h_dbf, i_record, 2, name.c_str());
		}
	}

    return 0;
}

std::string Shpwriter::get_Name(enumPointType type)
{
	switch (type)
	{
	case enumPointType::AIRPORT:
		return "机场";
	case enumPointType::A_POINT_MASK:
		return "准备进入航线点";
	case enumPointType::GUIDE:
		return "引导点";
	case enumPointType::EXPOSURE:
		return "曝光点";
	case enumPointType::ETRANCE_EXIT:
		return "准备退出点";
	case enumPointType::B_POINT_MASK:
		return "退出点";
	default:
		break;
	}
}