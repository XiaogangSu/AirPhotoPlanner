#-------------------------------------------------
#
# Project created by QtCreator 2013-06-23T16:49:25
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AirPhotoPlanner
TEMPLATE = app

RC_ICONS = logo.ico

INCLUDEPATH = .

INCLUDEPATH += D:/My_Work/library/gdal/Release/include
INCLUDEPATH += ./niGeom

LIBS += D:/My_Work/library/gdal/Release/lib/gdal_i.lib

SOURCES += main.cpp\
        mainwindow.cpp \
    mainwindowframe.cpp \
    flightroutedesign.cpp \
    polygonareaflightroutedesign.cpp \
    linearflightroutedesign.cpp \
    designtaskfactory.cpp \
    flightparameter.cpp \
    cogrgeometryfilereader.cpp \
    gomologging.cpp \
    uavrouteoutputer.cpp \
    geomertyconvertor.cpp \
    UAVRoute.cpp \
    coordinateoutput.cpp \
    multiregiondesigner.cpp \
    flightparameterinput.cpp \
    child_tv.cpp \
    uicontroller.cpp \
    GomoGemetry2D.cpp \
    ./niGeom/source/niPolygon2d.cpp \
    ./niGeom/source/niPolygon2dFn.cpp \
    ./niGeom/source/niGeomMath2d.cpp \
    copyrightdialog.cpp \
    DTM/cateye_mercator.cpp \
    DTM/cateye_srtm.cpp \
    SHP/shpwriter.cpp \
    SHP/dbfopen.c \
    SHP/safileio.c \
    SHP/shpopen.c \
    SHP/shptree.c

HEADERS  += mainwindow.h \
    mainwindowframe.h \
    flightroutedesign.h \
    polygonareaflightroutedesign.h \
    linearflightroutedesign.h \
    designtaskfactory.h \
    flightparameter.h \
    GomoPhotoBasic.h \
    cogrgeometryfilereader.h \
    gomologging.h \
    GomoGeometry2D.h \
    UAVRoute.h \
    uavrouteoutputer.h \
    geomertyconvertor.h \
    coordinateoutput.h \
    multiregiondesigner.h \
    flightparameterinput.h \
    child_tv.h \
    uicontroller.h \
    copyrightdialog.h \
    DTM/cateye_basic.h \
    DTM/cateye_mercator.h \
    DTM/cateye_srtm.h \
    SHP/shapefil.h \
    SHP/shapefileHelper.h \
    SHP/shpwriter.h

FORMS    += mainwindow.ui \
    child_tv.ui \
    copyrightdialog.ui

RESOURCES     = tools.qrc
