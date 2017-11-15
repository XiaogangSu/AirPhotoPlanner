#ifndef MAINWINDOWFRAME_H
#define MAINWINDOWFRAME_H

#include <QMainWindow>
#include "mainwindow.h"
#include "cogrgeometryfilereader.h"
#include "GomoGeometry2D.h"
#include <QPen>
#include <qfileinfo.h>
#include "DTM\cateye_mercator.h"
#include "qdockwidget.h"
#include "copyrightdialog.h"
using namespace Gomo::Geometry2D;
using namespace Cateye;
using namespace RasterCompiler;

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QTextEdit;
class QLabel;
class QCursor;
QT_END_NAMESPACE
class RenderArea;

#define DTM_PATH "D:/My_Work/UAVRouter-master/UAVRouter/webmercator/"
#define DTM_PATH "Beijing_Srtm_Mercator_Render/"
#define DTM_PNG_PIX 256

class MyWidget : public QWidget
{
public:
	QSize sizeHint() const
	{
		return QSize(270, 900); /* 在这里定义dock的初始大小 */
	}
};

class EagleEye : public QWidget
{
    public:
		EagleEye(QWidget* parent = 0, Qt::WindowFlags f = 0)
		{
			setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
			QWidget(parent,  f);
		}
};

enum MAP_ACTION
{
	MA_NULL    = 0,
	MA_ZOOMIN  = 1,
	MA_ZOOMOUT = 2,
	MA_MOVE = 3,
	MA_SELECTION = 4
};

struct TileID
{
	int zoom;
	int tx;
	int ty;
};

struct QImageDTM
{
	int zoom;
	int tx;
	int ty;
	QString imageDir;
	QImage qImageDTM;
	double metersX;
	double metersY;
	int pixX;
	int pixY;
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
	void mouseMoveEvent(QMouseEvent *event);
private slots:
    void open();
    void about();
    void documentWasModified();
    void setPlanParameter();
	void zoominMap();
	void zoomoutMap();
	void moveMap();
	void selectMap();
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
	QAction *selectObj;
    QAction *aboutAct;
    QAction *aboutQtAct;

    RenderArea *renderArea;

	QPen penRegion;
	QBrush brushRegion;
	QRadialGradient gradRegion;

	QPen penRoute;

	QBrush brushAirport;
	QPixmap pixAirport;

	MAP_ACTION _map_tool_status;

	QPointF _left_button_down_pt;
	QLabel *statusBarLabel;

	bool bStartPlan;

	double zoomLevel;

	QCursor *_zoom_out_cursor;
	QCursor *_zoom_in_cursor;
	QCursor *_pan_cursor;
private:
    MainWindow param_window;
    child_tv *ptrChild_TV;
    std::list<QString> listInputKmlFile;
    std::vector<Point2DArray> ptRegionArrayVec;
	Mercator dtmProcess;
	void set_global_variable(const QPointF& point, const MAP_ACTION map_action);
	void drawDTM();
private:
   void createRegionPixPoint();

   std::vector<Point2DArray> pixRegionVec;

   std::vector<Point2DArray> pixRouteVec;

protected:
	void converWgs2Pix(
		const QRect &clientRect,
		const Point2DArray &srcArray,
		Point2DArray &desArray);

	QPoint converWgs2Pix(WGS84Coord wgs);
	QPoint converMeters2Pix(double x, double y);

	WGS84Coord point2WGS84Coord(const QPointF& point);

	QVector< QVector<QImageDTM> > MainWindowFrame::GetFileList(
		QString path,
		int dtmZoom,
		const QVector<TileID> &tileIdList,
		bool bIsInit = true);

	void orderImageDTM(QVector<QImageDTM> &qImageDTM);
	void orderImageDTM(QVector< QVector<QImageDTM> > &qImageDTM);
	QVector<TileID> getTileList(WGS84Coord start, WGS84Coord end, int zoom);

	bool JudgeInTileX(int tileX, const QVector<TileID> &tileIdList);

	bool JudgeInTileY(int tileY, const QVector<TileID> &tileIdList);

	bool GetTileListToltalPix(
		const QVector< QVector<QImageDTM> > &qImageDTMList,
		int &pixX,
		int &pixY);

	bool GetTileListBoundingbox(
		const QVector< QVector<QImageDTM> > &qImageDTMList,
		const int zoom,
		double  &left_top_x,
		double  &left_top,
		double  &right_bottom_x,
		double  &right_bottom_y);

	bool GetTileListLeftTop(
		const QVector< QVector<QImageDTM> > &qImageDTMList,
		const int zoom,
		double  &left_top_x,
		double  &left_top_y);
	void SetDTMLeftTopPIX(QImageDTM &qDTM);

	bool SetGlobalParam(
		const QVector< QVector<QImageDTM> > &qImageDTMList);

	void GetMaxTileNumber(QRect cRect, int &txNum, int &tyNum);

	void DrawGrid();

	void createEagleEye();
private:
	double _left_top_x;
	double _left_top_y;
	double _right_bottom_x;
	double _right_bottom_y;
	double  _lon_max;
	double  _lat_min;

	double  _lon_min;
	double  _lat_max;
    double _pix_x_scale;
	double _pix_y_scale;
	double  min_pix_scale;
	double  _latDel_PerDTM;
	double  _lonDel_PerDTM;

	double  _pix_maxx;
	double  _pix_maxy;

	QRect clientRect;

	EagleEye *_pEagleEye;
	QVector< QVector<QImageDTM> > _imageDTMList;
	CopyRightDialog * m_pCopyRightDialog;
private:
	double _eagle_left_top_x;
	double _eagle_left_top_y;
	double _eagle_right_bottom_x;
	double _eagle_right_bottom_y;

	QImage _eagle_eye_image;
	QPen  _pen_eagle_eye;
	QBrush  _brush_eagle_eye;
//    Gomo::FlightRoute::FlightParameter m_flight_param;
};

#endif
