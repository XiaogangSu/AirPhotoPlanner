#ifndef UAVROUTEOUTPUTER_H
#define UAVROUTEOUTPUTER_H

#include "UAVRoute.h"
using namespace Gomo::FlightRoute;


class UAVRouteOutputer
{
public:
    UAVRouteOutputer();

    static void OutputRouteDesignFileAsText(const UAVRouteDesign & route_design, const std::string & output_file);

    static void OutputRouteDesignFileAsBinary(const UAVRouteDesign & route_design, const std::string & output_file);
    
    static void OutputRouteDesignFileAsKML(const UAVRouteDesign & route_design, const std::string & output_file);

    static void OutputRouteDesignFileAsKMLLine(const UAVRouteDesign & route_design, const std::string & output_file);

    static void OutputRouteDesignFileAsTextEncrypted(const UAVRouteDesign & route_design, const std::string & output_file);

	static void OutputRouteDesignFileAsSHP(const UAVRouteDesign & route_design, const std::string & output_file);

    // skwPath: skw file
    // txtPath: txt file
    static void OutputRouteDesignFileAsSkw(const UAVRouteDesign &route_design, 
                                           const std::string &skwPath)
};

#endif // UAVROUTEOUTPUTER_H
