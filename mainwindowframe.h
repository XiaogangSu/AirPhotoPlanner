#ifndef MAINWINDOWFRAME_H
#define MAINWINDOWFRAME_H

#include <QMainWindow>
#include "mainwindow.h"
#include "cogrgeometryfilereader.h"
#include "GomoGeometry2D.h"
#include <QPen>

using namespace Gomo::Geometry2D;

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QTextEdit;
QT_END_NAMESPACE
class RenderArea;

enum MAP_ACTION
{
	MA_NULL    = 0,
	MA_ZOOMIN  = 1,
	MA_ZOOMOUT = 2,
	MA_MOVE = 3,
	MA_SELECTION = 4
};

struct WGS84Coord
{
	WGS84Coord() : x(), y(), z() {}
	WGS84Coord(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}
	WGS84Coord(const WGS84Coord& copy) : x(copy.x), y(copy.y), z(copy.z) {}
	WGS84Coord& operator=(const WGS84Coord& copy)
	{
		x = copy.x;
		y = copy.y;
		z = copy.z;

		return *this;
	}
	double x;
	double y;
	double z;
};

//! [class definition with macro]
class MainWindowFrame : public QMainWindow
{
    Q_OBJECT

public:
    MainWindowFrame();
//! [class definition with macro]
    MainWindowFrame(const QString &fileName);

protected:
    void closeEvent(QCloseEvent *event);
    void paintEvent(QPaintEvent *);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
private slots:
    void open();
    void about();
    void documentWasModified();
    void setPlanParameter();
	void zoominMap();
	void zoomoutMap();
	void moveMap();
private:
    void init();
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    void loadFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);
    MainWindowFrame *findMainWindow(const QString &fileName);

    QTextEdit *textEdit;
    QString curFile;
    bool isUntitled;

    QMenu *fileMenu;
    QMenu *mapMenu;
    QMenu *helpMenu;

    QToolBar *fileToolBar;
    QToolBar *mapToolBar;

    QAction *openKML;
    QAction *setParameter;
    QAction *exitAct;

    QAction *zoomout;
    QAction *zoomin;
    QAction *pan;

    QAction *aboutAct;
    QAction *aboutQtAct;

    RenderArea *renderArea;

	QPen penRegion;
	QPen penRoute;

	MAP_ACTION _map_tool_status;

	QPointF _left_button_down_pt;
private:
    MainWindow param_window;
    child_tv *ptrChild_TV;
    std::list<QString> listInputKmlFile;
    std::vector<Point2DArray> ptRegionArrayVec;
	void set_global_variable(const QPointF& point, const MAP_ACTION map_action);
private:
   void createRegionPixPoint();

   std::vector<Point2DArray> pixRegionVec;

   std::vector<Point2DArray> pixRouteVec;

protected:
	void converWgs2Pix(
		const QRect &clientRect,
		const Point2DArray &srcArray,
		Point2DArray &desArray);

	WGS84Coord point2WGS84Coord(const QPointF& point);
private:
	double  lon_min;
	double  lon_max;
	double  lat_min;
	double  lat_max;
	double min_pix_scale;
	QRect clentRect;
//    Gomo::FlightRoute::FlightParameter m_flight_param;
};

#endif
