#ifndef BIDB2SHP_SHPWRITER_H
#define BIDB2SHP_SHPWRITER_H
#include <string>
#include "shapefil.h"
#include <vector>

typedef enum FLIGTH_POINT_TYPE
{
	AIRPORT = 0,
	GUIDE   = 10,
	ETRANCE_EXIT = 18,
	EXPOSURE     = 3,
	A_POINT_MASK = 9, // the direction of getting  into the strip
	B_POINT_MASK = 17  // the direction of leaving the strip
}enumPointType;

typedef struct _tPoint
{
	int id;
	enumPointType PointType;
	double X;
	double Y;
	double Z;
	_tPoint()
	{
		X = 0.0;
		Y = 0.0;
		Z = 0.0;
	}
}SHP_Point;

class Shpwriter
{
public:
    Shpwriter();
    ~Shpwriter();
    int init(const std::string& strFileName);
    void close_files();
	int add_record(const std::vector<SHP_Point>& pt_vec);
	std::string get_Name(enumPointType type);
private:
    SHPHandle       _h_shp;
    DBFHandle       _h_dbf;
    int             _shape_type;
    int             _point_type;
};
#endif
