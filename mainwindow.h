#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "flightparameter.h"
#include "child_tv.h"
#include <set>
#include "UAVRoute.h"
#include "copyrightdialog.h"
#include "GomoGeometry2D.h"
using namespace Gomo::Geometry2D;
using namespace Gomo::FlightRoute;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:

    void on_toolButtonAirport_clicked();

    void on_toolButton_Region_clicked();

    void on_cmdDesignStart_clicked();

    void on_toolButtonOutputSelect_clicked();

    void on_btn_unittest_clicked();

    void on_MainWindow_iconSizeChanged(const QSize &iconSize);

    void on_radioSinglePolygon_clicked();

    void on_radioMultiPolygon_clicked();

    void on_radioMultiPolygon_toggled(bool checked);

    void on_radioSinglePolygon_toggled(bool checked);

    void on_menuAboutUs_triggered();

protected:
    Ui::MainWindow *ui;
    child_tv *ptrChild_TV;


    CopyRightDialog * m_pCopyRightDialog;

    bool fillInFlightParamRegionFiles();
public:
    //std::set<QString> setInputKmlFile;
    std::list<QString> listInputKmlFile;

	void setKMLFileList(std::list<QString> list);

	std::vector<Point2DArray> getRouteDesignPoint();

	void getAitportPositon(double &lon, double &lat);
protected:
    Gomo::FlightRoute::FlightParameter m_flight_param;

	std::vector<Point2DArray> ptRouteArrayVec;
private:
	void createRouteDesignPoint(const UAVRouteDesign routeDesignWGS84);
};

#endif // MAINWINDOW_H
