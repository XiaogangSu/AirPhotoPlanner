#include "cogrgeometryfilereader.h"
#include <QDebug>
#include <sstream>
using std::ostringstream;

COGRGeometryFileReader::COGRGeometryFileReader()
{
}

std::auto_ptr<OGRGeometry>
COGRGeometryFileReader::GetFirstOGRPointFromFile(std::string ogrfile)
{
    std::auto_ptr<OGRGeometry>  geom_ptr;

    geom_ptr =COGRGeometryFileReader::GetFirstOGRGeometryFromFile(ogrfile);

    if( geom_ptr.get() != NULL
        && wkbFlatten(geom_ptr->getGeometryType()) == wkbPoint )
    {
        return geom_ptr;
    }
    else
    {        
        return std::auto_ptr<OGRGeometry>(NULL);
    }


}

std::auto_ptr<OGRGeometry>
COGRGeometryFileReader::GetFirstOGRLineStringFromFile(std::string ogrfile)
{

    std::auto_ptr<OGRGeometry>  geom_ptr;

    geom_ptr =COGRGeometryFileReader::GetFirstOGRGeometryFromFile(ogrfile);

    if( geom_ptr.get() != NULL
                && wkbFlatten(geom_ptr->getGeometryType()) == wkbLineString )
    {
        return geom_ptr;
    }
    else
    {
        return std::auto_ptr<OGRGeometry>(NULL);
    }

}

std::auto_ptr<OGRGeometry>
COGRGeometryFileReader::GetFirstOGRPolygonFromFile(std::string ogrfile)
{

    std::auto_ptr<OGRGeometry>  geom_ptr;

    geom_ptr =COGRGeometryFileReader::GetFirstOGRGeometryFromFile(ogrfile);

    if( geom_ptr.get() != NULL
                && wkbFlatten(geom_ptr->getGeometryType()) == wkbPolygon )
    {
        return geom_ptr;
    }
    else
    {
        return std::auto_ptr<OGRGeometry>(NULL);
    }
}

std::auto_ptr<OGRGeometry>
COGRGeometryFileReader::GetFirstOGRGeometryFromFile(std::string ogrfile)
{
    OGRRegisterAll();

    OGRDataSource       *poDS;

    poDS = OGRSFDriverRegistrar::Open( ogrfile.c_str(), FALSE );
    if( poDS == NULL )
    {

        return std::auto_ptr<OGRGeometry>(NULL);
    }

    ostringstream streamdebug;
    streamdebug.str("");
    streamdebug<< "GetFirstOGRGeometryFromFile!"<<std::endl;
    qDebug(streamdebug.str().c_str());

    std::auto_ptr<OGRGeometry> geom_ptr;

    OGRLayer  *poLayer;

    poLayer = poDS->GetLayer(0);

    OGRFeature *poFeature;

    poLayer->ResetReading();
    if ( (poFeature = poLayer->GetNextFeature()) != NULL )
    {
        OGRGeometry *poGeometry;

        poGeometry = poFeature->GetGeometryRef();
        if( poGeometry != NULL)
        {
            // colne the geometry and set to the return variable
            geom_ptr=std::auto_ptr<OGRGeometry>(poGeometry->clone());
        }

        OGRFeature::DestroyFeature( poFeature );
    }

    OGRDataSource::DestroyDataSource( poDS );

    return geom_ptr;
}

void COGRGeometryFileReader::GetPointsFromFile(Point2DArray &pointArray, std::string filePath)
{
    OGRRegisterAll();
    OGRDataSource *pDs = OGRSFDriverRegistrar::Open(filePath.c_str());

    OGRLayer *pLayer = pDs->GetLayer(0);
    pLayer->ResetReading();
    OGRFeature *pFeature = NULL;
    while ((pFeature = pLayer->GetNextFeature()) != NULL)
    {
        OGRGeometry *pGeo = pFeature->GetGeometryRef();
        if (wkbFlatten(pGeo->getGeometryType()) == wkbPolygon)
        {
            OGRPolygon *pPolygon = (OGRPolygon*)pGeo;
            for (int i = 0; i < pPolygon->getExteriorRing()->getNumPoints(); ++i)
            {
                double x = pPolygon->getExteriorRing()->getX(i);
                double y = pPolygon->getExteriorRing()->getY(i);
                Point2D p(x, y);
                pointArray.push_back(p);
            }
        }
        OGRFeature::DestroyFeature(pFeature);
    }

    OGRDataSource::DestroyDataSource(pDs);
}
