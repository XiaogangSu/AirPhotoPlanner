#ifndef MAINWINDOWFRAME_H
#define MAINWINDOWFRAME_H
#include <QtWidgets>
#include <QMainWindow>
#include "mainwindow.h"
#include "cogrgeometryfilereader.h"
#include "GomoGeometry2D.h"
#include <QPen>
#include <qfileinfo.h>
#include "DTM\cateye_mercator.h"
#include "qdockwidget.h"
#include "copyrightdialog.h"
#include "ZoneParamDialog.h"
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

//#define DTM_PATH "D:/My_Work/UAVRouter-master/UAVRouter/webmercator/"
#define DTM_PATH "Map/DTM/"
#define IMAGE_PATH "Map/IMAGE/"
#define DTM_PNG_PIX 256
#define TREE_WIDTH 240 
#define SCALE_START_X 30
#define SCALE_START_Y 50 
#define SCALE_LENGTH 100 

#define MAX_LON_WGS 180.0 
#define MIN_LON_WGS -180.0 
#define MAX_LAT_WGS 89.0 
#define MIN_LAT_WGS -89.0 

#define MAX_LON_MER 20037508.3 
#define MIN_LON_MER -20037508.3 
#define MAX_LAT_MER 30240971.9 
#define MIN_LAT_MER -30240971.9 

#define MIN_ZOOM_SCALE 2 
#define MAX_ZOOM_SCALE 13 
#define INVALID_ZONE_HEIGHT -9999.0 

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

struct RegionInfo
{
	QString regionName;
	bool bSelect;
	WGS84Coord minCoord;
	WGS84Coord maxCoord;
	double base_height;
};

//! [class definition with macro]
class MainWindowFrame : public QMainWindow
{
    Q_OBJECT

public:
    MainWindowFrame();
//! [class definition with macro]
    MainWindowFrame(const QString &fileName);
	void init();
protected:
    void closeEvent(QCloseEvent *event);
    void paintEvent(QPaintEvent *);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void resizeEvent(QResizeEvent* size);
private slots:
    void open();
    void about();
    void documentWasModified();
    void setPlanParameter();
	void zoominMap();
	void zoomoutMap();
	void moveMap();
	void selectMap();
	void slotItemPressed(QTreeWidgetItem *item, int column);
	void treeItemChanged(QTreeWidgetItem *item);
	void slotItemChanged(QTreeWidgetItem *item, int column);
//	void on_comb_comb_show_grad_changed();
	void on_comb_clolor_changed();
	void on_start_plan();
	void on_zone_param();
private:
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
	QToolBar *startToolBar;

    QAction *openKML;
    QAction *setParameter;
    QAction *exitAct;
	QAction *startPlan;

    QAction *zoomout;
    QAction *zoomin;
    QAction *pan;
	QAction *selectObj;
    QAction *aboutAct;
    QAction *aboutQtAct;

	QAction *zoneParamAct;

    RenderArea *renderArea;

	QPen penRegion;
	QBrush brushRegion;

	QPen penRegion_Not_Select;
	QBrush brushRegion_Not_Select;;

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
	ZoneParamDialog* m_zoneParam;
    std::list<QString> listInputKmlFile;
    std::vector<Point2DArray> ptRegionArrayVec;

	Mercator dtmProcess;
	void center_to_region();
	void set_global_variable(const QPointF& point, const MAP_ACTION map_action);
	void createRegionPixPoint();

	void reset_bottom_xy();
private:
   std::vector<Point2DArray> pixRegionVec;
   std::vector<Point2DArray> pixRouteVec;

//   std::list<QString> list_kml_name;
//   WGS84Coord min_region;
//   WGS84Coord max_region;
//   std::vector<bool> region_select;
   std::vector<RegionInfo> _regionInfo;

protected:
	void converWgs2Pix(
		const QRect &clientRect,
		const Point2DArray &srcArray,
		Point2DArray &desArray);

	QPoint converWgs2Pix(const QRect &clientRect, WGS84Coord wgs);
	QPoint converMeters2Pix(double x, double y);

	WGS84Coord point2WGS84Coord(const QPointF& point);

	QVector< QVector<QImageDTM> > GetFileList(
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

	void DrawDTM();
	void GetDTMFile();
	void DrawImage();
	void DrawGrid();
	void DrawScale();
	void DrawEagleEye();
	void create_list();
private:
	//地图窗口矩形
	QRect _map_client_rect;
	//左上角墨卡托经度值
	double _left_top_x;
	//左上角墨卡托纬度值
	double _left_top_y;

	//1墨卡托经度值，相当于屏幕坐标
	double _pix_x_scale;
	//1墨卡托纬度值，相当于屏幕坐标
	double _pix_y_scale;

	//右下角墨卡托经度值
	double _right_bottom_x;
	//又下角墨卡托纬度值
	double _right_bottom_y;

	//最大经度值：墨卡托
	double  _lon_max;
	//最大纬度值：墨卡托
	double  _lat_max;

	//最小经度值：墨卡托
	double  _lon_min;
	//最小纬度值：墨卡托
	double  _lat_min;

	//当前比例尺下，每幅地图跨的纬度
	double  _latDel_PerDTM;
	//当前比例尺下，每幅地图跨的经度
	double  _lonDel_PerDTM;

	double  _pix_maxx;
	double  _pix_maxy;

	EagleEye *_pEagleEye;
	QVector< QVector<QImageDTM> > _map_dtm_list;
	QVector< QVector<QImageDTM> > _map_image_list;
	CopyRightDialog * m_pCopyRightDialog;
	double  min_pix_scale;
	int _zone_param_index;
private:
	double _eagle_left_top_x;
	double _eagle_left_top_y;
	double _eagle_right_bottom_x;
	double _eagle_right_bottom_y;

	QImage _eagle_eye_image;
	QPen  _pen_eagle_eye;
	QBrush  _brush_eagle_eye;
private:
	bool _b_flag_resize;
	QTreeWidget* _area_list_widget;
	QTreeWidgetItem* _p_basic_tree_item;
	QTreeWidgetItem* _p_zone_tree_item;
	QTreeWidgetItem* _p_level_tree_item;
	QComboBox *_comb_show_grad;
	QComboBox *_comb_clolor_grad;
	QComboBox *_comb_show_eagle;
	QComboBox *_comb_show_scale;
	QMenu* _p_tree_menu;

	QPen _pen_grad;
	QPen _pen_scale;
	bool _b_flag_show_grad;
	bool _b_flag_show_scale;
	bool _b_flag_show_eagle;
	bool _b_flag_show_dtm;
	bool _b_flag_show_image;
private:
	void InitTreeWidget();
//    Gomo::FlightRoute::FlightParameter m_flight_param;
};

#endif
