#include <QtWidgets>

#include "mainwindowframe.h"
#include <QPainter>
#include "geomertyconvertor.h"

MainWindowFrame::MainWindowFrame()
{
    init();
	setWindowTitle("AirPhotoPlanner");

	min_pix_scale = 1;
	_map_tool_status = MAP_ACTION::MA_NULL;

	penRegion = QPen(Qt::blue, 3);
	penRoute = QPen(Qt::red, 2);
	_pen_eagle_eye = QPen(Qt::blue, 1);
	_brush_eagle_eye = QBrush(Qt::darkGray);

	gradRegion =  QRadialGradient(50, 50, 50, 70, 70);
//	gradRegion.setColorAt(0.0, Qt::black);
	brushRegion = QBrush(QColor(255, 100, 0, 80), Qt::CrossPattern); // 最后一项为透明度

	pixAirport.load(":/images/airport.png");
	brushAirport = QBrush(QPixmap(":/images/airport.png"));
	statusBarLabel = new QLabel;
	setMouseTracking(true);
	bStartPlan = false;

	_zoom_out_cursor = new QCursor(QPixmap(":/images/zoom_out.png"), -1, -1);
	_zoom_in_cursor = new QCursor(QPixmap(":/images/zoom_in.png"), -1, -1);
	_pan_cursor = new QCursor(QPixmap(":/images/pan.png"), -1, -1);
//	createEagleEye();
}

void MainWindowFrame::init()
{
//	QDesktopWidget* desktopWidget = QApplication::desktop();
//	clientRect = desktopWidget->screenGeometry();
	clientRect = geometry();
	//设置背景黑色
	QPalette pal(this->palette());
	pal.setColor(QPalette::Background, Qt::black);
	setAutoFillBackground(true);
	setPalette(pal);
	setWindowState(Qt::WindowMaximized);
	isUntitled = true;

//	clientRect = desktopWidget->screenGeometry();
	clientRect = geometry();
	//   textEdit = new QTextEdit;
	//   setCentralWidget(textEdit);
	createActions();
	createMenus();
	createToolBars();
	createStatusBar();

	zoomLevel = 0;
	QVector<TileID> tileIdList;
	_imageDTMList =
		GetFileList(
		DTM_PATH,
		zoomLevel,
		tileIdList,
		false);

	if (_imageDTMList.size() > 0)
	{
		_eagle_eye_image.load(_imageDTMList[0][0].imageDir);
		GetTileListBoundingbox(_imageDTMList, zoomLevel, _left_top_x, _left_top_y, _right_bottom_x, _right_bottom_y);

		_eagle_left_top_x = _left_top_x;
		_eagle_left_top_y = _left_top_y;
		_eagle_right_bottom_x = _right_bottom_x;
		_eagle_right_bottom_y = _right_bottom_y;

		_pix_maxx = _imageDTMList.size() * DTM_PNG_PIX;
		_pix_maxy = _imageDTMList[0].size() * DTM_PNG_PIX;
		_lon_max = _right_bottom_x;
		_lat_min = _right_bottom_y;

		double boundingbox[4];
		dtmProcess.TileMetersBound(_imageDTMList[0][0].tx, _imageDTMList[0][0].ty, zoomLevel, boundingbox);
		_lonDel_PerDTM = boundingbox[2] - boundingbox[0];
		_latDel_PerDTM = boundingbox[3] - boundingbox[1];

		_pix_x_scale = (double)DTM_PNG_PIX / (double)(_lonDel_PerDTM);
		_pix_y_scale = (double)DTM_PNG_PIX / (double)(_latDel_PerDTM);

		QVector< QVector<QImageDTM> >::iterator iter;
		QVector< QImageDTM >::iterator im;

		for (iter = _imageDTMList.begin(); iter != _imageDTMList.end(); iter++)
		{
			for (im = iter->begin(); im != iter->end(); im++)
			{
				SetDTMLeftTopPIX(*im);
			}
		}
	}
}

MainWindowFrame::MainWindowFrame(const QString &fileName)
{
    init();
//    loadFile(fileName);
}

void MainWindowFrame::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("SDI"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    textEdit->setPlainText(in.readAll());
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File loaded"), 2000);
}

void MainWindowFrame::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}

void MainWindowFrame::open()
{
    if(0 == listInputKmlFile.size())
    {
        QFileDialog dlg(this,
                        "Choose Flight Region Defination File(s)...",
                        ".",
                        "KML (*.kml *.KML)");
        dlg.setFileMode(QFileDialog::ExistingFiles);
        if(dlg.exec() == QFileDialog::Accepted)
        {
            QStringList slist = dlg.selectedFiles();
            listInputKmlFile.clear();
			for (int i = 0; i < slist.size(); ++i)
			{
				listInputKmlFile.push_back(slist.at(i));
				std::auto_ptr<OGRGeometry> regionORG = COGRGeometryFileReader::GetFirstOGRGeometryFromFile(slist.at(i).toStdString());

				Point2DArray ptArray;
				GeomertyConvertor::OGRGeomery2Point2DArray(regionORG.get(), ptArray);
				ptRegionArrayVec.push_back(ptArray);
			}
        }
    }

    createRegionPixPoint();

/*
    // show the management dialog
    if(NULL != ptrChild_TV)
    {
        delete ptrChild_TV;
    }

    ptrChild_TV = new child_tv(this);

    ptrChild_TV->move(this->x() + this->frameSize().width(), this->y());
    ptrChild_TV->show();
 */
    /*
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty()) {
        MainWindowFrame *existing = findMainWindow(fileName);
        if (existing) {
            existing->show();
            existing->raise();
            existing->activateWindow();
            return;
        }

        if (isUntitled && textEdit->document()->isEmpty()
                && !isWindowModified()) {
            loadFile(fileName);
        } else {
            MainWindowFrame *other = new MainWindowFrame(fileName);
            if (other->isUntitled) {
                delete other;
                return;
            }
            other->move(x() + 40, y() + 40);
            other->show();
        }
    }*/
}

void MainWindowFrame::createRegionPixPoint()
{
	double minx = 999999.9;
	double miny = 999999.9;
	double maxx = 0.0;
	double maxy = 0.0;
	/*
	std::vector<Point2DArray>::iterator iter = ptRegionArrayVec.begin();
	for (; iter != ptRegionArrayVec.end(); iter++)
	{
		Point2DArray::iterator subit = (*iter).begin();

		for (; subit != iter->end(); subit++)
		{
			if (subit->X < minx)
			{
				minx = subit->X;
			}
			if (subit->X > maxx)
			{
				maxx = subit->X;
			}
			if (subit->Y < miny)
			{
				miny = subit->Y;
			}
			if (subit->Y > maxy)
			{
				maxy = subit->Y;
			}
		}
	}

	_lon_min = minx - (maxx - minx) / 8.0;
	_lon_max = maxx + (maxx - minx) / 8.0;
	_lat_min = miny - (maxy - miny) / 8.0;
	_lat_max = maxy + (maxy - miny) / 8.0;
	*/
//	QDesktopWidget* desktopWidget = QApplication::desktop();
//	clientRect = desktopWidget->screenGeometry();
	clientRect = geometry();
	std::vector<Point2DArray>::iterator iter_pix = ptRegionArrayVec.begin();
	for (; iter_pix != ptRegionArrayVec.end(); iter_pix++)
	{
		Point2DArray pixArray;
		converWgs2Pix(clientRect, *iter_pix, pixArray);
		pixRegionVec.push_back(pixArray);
	}
}

void MainWindowFrame::converWgs2Pix(
	const QRect &clientRect,
	const Point2DArray &srcArray,
	Point2DArray &desArray)
{
	Point2DArray::const_iterator iter = srcArray.begin();
	for (; iter != srcArray.end(); iter++)
	{
		double mx = 0.0;
		double my = 0.0;
		dtmProcess.LonLatToMeters(iter->X, iter->Y,	mx, my);
		Point2D pixpt;
		double tmp_pix = (double)(mx - _left_top_x) * _pix_x_scale + clientRect.left();
		pixpt.X = (int)tmp_pix;

		tmp_pix = (double)(_left_top_y - my) * _pix_y_scale + clientRect.top();
		pixpt.Y = (int)tmp_pix;
		desArray.push_back(pixpt);
	}
}

QPoint MainWindowFrame::converWgs2Pix(WGS84Coord wgs)
{
	double mx = 0.0;
	double my = 0.0;
	dtmProcess.LonLatToMeters(wgs.x, wgs.y, mx, my);

	double tmp_pixx = (double)(mx - _left_top_x) * _pix_x_scale + clientRect.left();
	double tmp_pixy = (double)(_left_top_y - my) * _pix_y_scale + clientRect.top();
	QPoint qpt = QPoint((int)tmp_pixx, (int)tmp_pixy);

	return qpt;
}

void MainWindowFrame::about()
{
	m_pCopyRightDialog = new CopyRightDialog(this);
	m_pCopyRightDialog->show();
}

void MainWindowFrame::documentWasModified()
{
    setWindowModified(true);
}

void MainWindowFrame::createActions()
{
    openKML = new QAction(QIcon(":/images/open.png"), tr("&导入航摄区域定义文件"), this);
//  openKML->setShortcuts(QKeySequence::Open);
    openKML->setStatusTip(tr("Open an existing file"));
    connect(openKML, SIGNAL(triggered()), this, SLOT(open()));

    setParameter = new QAction(QIcon(":/images/setting.png"), tr("&规划"), this);
    setParameter->setStatusTip(tr("Save the document to disk"));
    connect(setParameter, SIGNAL(triggered()), this, SLOT(setPlanParameter()));

    exitAct = new QAction(tr("Exit"), this);
//  exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

    zoomin = new QAction(QIcon(":/images/zoom_in.png"), tr("&放大"), this);
    zoomin->setStatusTip(tr("放大地图"));
	connect(zoomin, SIGNAL(triggered()), this, SLOT(zoominMap()));

    zoomout = new QAction(QIcon(":/images/zoom_out.png"), tr("&缩小"), this);
    zoomout->setStatusTip(tr("缩小地图"));
	connect(zoomout, SIGNAL(triggered()), this, SLOT(zoomoutMap()));

    pan = new QAction(QIcon(":/images/pan.png"), tr("&漫游"), this);
    pan->setStatusTip(tr("移动地图"));
	connect(pan, SIGNAL(triggered()), this, SLOT(moveMap()));

	selectObj = new QAction(QIcon(":/images/arrow.png"), tr("&选择"), this);
	selectObj->setStatusTip(tr("选择"));
	connect(selectObj, SIGNAL(triggered()), this, SLOT(selectMap()));

    aboutAct = new QAction(tr("&关于"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

/*
    cutAct->setEnabled(false);
    copyAct->setEnabled(false);
    connect(textEdit, SIGNAL(copyAvailable(bool)),
            cutAct, SLOT(setEnabled(bool)));
    connect(textEdit, SIGNAL(copyAvailable(bool)),
            copyAct, SLOT(setEnabled(bool)));*/
}

//! [implicit tr context]
void MainWindowFrame::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&设计"));
//! [implicit tr context]
    fileMenu->addAction(openKML);
    fileMenu->addAction(setParameter);
    fileMenu->addAction(exitAct);
	
    mapMenu = menuBar()->addMenu(tr("&地图操作"));
    mapMenu->addAction(zoomin);
    mapMenu->addAction(zoomout);
    mapMenu->addAction(pan);
	mapMenu->addAction(selectObj);
    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&帮助"));
    helpMenu->addAction(aboutAct);
}

void MainWindowFrame::createToolBars()
{
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(openKML);
    fileToolBar->addAction(setParameter);

    mapToolBar = addToolBar(tr("Map"));
	mapToolBar->addAction(zoomin);
	mapToolBar->addAction(zoomout);
    mapToolBar->addAction(pan);
	mapToolBar->addAction(selectObj);
}

void MainWindowFrame::createStatusBar()
{
}

void MainWindowFrame::readSettings()
{
    QSettings settings;
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    move(pos);
    resize(size);
}

void MainWindowFrame::writeSettings()
{
    QSettings settings;
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

void MainWindowFrame::setCurrentFile(const QString &fileName)
{
    static int sequenceNumber = 1;

    isUntitled = fileName.isEmpty();
    if (isUntitled) {
        curFile = tr("AirPhotoPlanner").arg(sequenceNumber++);
    } else {
        curFile = QFileInfo(fileName).canonicalFilePath();
    }

    textEdit->document()->setModified(false);
    setWindowModified(false);
    setWindowFilePath(curFile);
}

QString MainWindowFrame::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

MainWindowFrame *MainWindowFrame::findMainWindow(const QString &fileName)
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    foreach (QWidget *widget, qApp->topLevelWidgets()) {
        MainWindowFrame *mainWin = qobject_cast<MainWindowFrame *>(widget);
        if (mainWin && mainWin->curFile == canonicalFilePath)
            return mainWin;
    }
    return 0;
}

void MainWindowFrame::setPlanParameter()
{
	param_window.show();
	param_window.setKMLFileList(listInputKmlFile);
	bStartPlan = true;
 //   ptrChild_TV = new child_tv(this);
//    param_window.move(this->x() + this->frameSize().width(), this->y());
//    param_window.setKMLFileList(listInputKmlFile);
//    param_window.show();
}

void MainWindowFrame::paintEvent(QPaintEvent *)
{
	drawDTM();
	DrawGrid();
	QPainter pRegion(this);
	pRegion.setPen(penRegion);
//	pRegion.setBrush(gradRegion);
	pRegion.setBrush(brushRegion);

   std::vector<Point2DArray>::iterator iter = pixRegionVec.begin();
   for(; iter != pixRegionVec.end(); iter++)
   {
	   QPointF *pRegionPoint = new QPointF[iter->size()];
	   Point2DArray::iterator subit = (*iter).begin();

	   int i = 0;
	   for (; subit != iter->end(); subit++)
	   {
		   pRegionPoint[i] = QPointF(subit->X, subit->Y);
		   i++;
	   }

	   pRegion.drawPolygon(pRegionPoint, iter->size());
	   delete[] pRegionPoint;
   }
   
   if (bStartPlan)
   {
	   QPainter painter(this);
	   painter.setPen(penRoute);
	   std::vector<Point2DArray>  ptArray = param_window.getRouteDesignPoint();
	   std::vector<Point2DArray>::iterator iter_pix = ptArray.begin();
	   pixRouteVec.clear();
	   for (; iter_pix != ptArray.end(); iter_pix++)
	   {
		   Point2DArray pixArray;
		   converWgs2Pix(clientRect, *iter_pix, pixArray);
		   pixRouteVec.push_back(pixArray);
	   }

	   std::vector<Point2DArray>::iterator iterR = pixRouteVec.begin();
	   for (; iterR != pixRouteVec.end(); iterR++)
	   {
		   Point2DArray::iterator sbuiter = (*iterR).begin();

		   int start_x = sbuiter->X;
		   int start_y = sbuiter->Y;
		   sbuiter++;

		   painter.drawPoint(QPointF(start_x, start_y));
		   for (; sbuiter != iterR->end(); sbuiter++)
		   {
			   painter.drawLine(QPointF(start_x, start_y),
				   QPointF(sbuiter->X, sbuiter->Y));

			   start_x = sbuiter->X;
			   start_y = sbuiter->Y;
			   painter.drawPoint(QPointF(start_x, start_y));
		   }
	   }

	   //draw airport
	   double lon = 0.0;
	   double lat = 0.0;
	   param_window.getAitportPositon(lon, lat);
	   QPoint qpt = converWgs2Pix(WGS84Coord(lon, lat, 0));
	   painter.setBrush(brushAirport);
	   painter.drawPixmap(qpt, pixAirport);
   }

   createEagleEye();
}

void MainWindowFrame::zoominMap()
{
	_map_tool_status = MAP_ACTION::MA_ZOOMIN;
	this->setCursor(*_zoom_in_cursor);
}

void MainWindowFrame::zoomoutMap()
{
	_map_tool_status = MAP_ACTION::MA_ZOOMOUT;
	this->setCursor(*_zoom_out_cursor);
}

void MainWindowFrame::moveMap()
{
	_map_tool_status = MAP_ACTION::MA_MOVE;
	this->setCursor(*_pan_cursor);
}

void MainWindowFrame::selectMap()
{
	_map_tool_status = MAP_ACTION::MA_SELECTION;
	this->setCursor(Qt::ArrowCursor);
}

void MainWindowFrame::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton && _map_tool_status == MAP_ACTION::MA_MOVE)
	{
		_left_button_down_pt = QPoint(event->x(), event->y());
	}
}

void MainWindowFrame::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		if (_map_tool_status == MAP_ACTION::MA_MOVE)
		{
//			QDesktopWidget* desktopWidget = QApplication::desktop();
//			clientRect = desktopWidget->screenGeometry();
			clientRect = geometry();
			int x_move = event->x() - _left_button_down_pt.x();
			int y_move = event->y() - _left_button_down_pt.y();

			_left_top_x -= double(x_move) / _pix_x_scale;
			_right_bottom_x -= double(x_move) / _pix_x_scale;
			//if (_left_top_x < 0)
			{
				_right_bottom_x = _left_top_x + double(clientRect.width()) / _pix_x_scale;
			}

			_left_top_y += double(y_move) / _pix_y_scale;
			_right_bottom_y += double(y_move) / _pix_y_scale;

			//if (_left_top_y < 0)
			{
				_right_bottom_y = _left_top_y - double(clientRect.height()) / _pix_y_scale;
			}

			QVector<TileID> tileIdList = getTileList(
				WGS84Coord(_left_top_x, _left_top_y, 0.0),
				WGS84Coord(_right_bottom_x, _right_bottom_y, 0.0),
				zoomLevel);

			_imageDTMList = GetFileList(DTM_PATH,
				zoomLevel,
				tileIdList);

			if (_imageDTMList.size() == 0 || _imageDTMList[0].size() == 0)
			{
				return;
			}

			int pixX = _imageDTMList.size() * DTM_PNG_PIX;
			int pixY = _imageDTMList[0].size() * DTM_PNG_PIX;

			if (pixX > clientRect.width())
			{
				_pix_maxx = clientRect.width();
				_right_bottom_x = _left_top_x + (double)clientRect.width() / _pix_x_scale;
			}
			else
			{
				_pix_maxx = pixX;
				_right_bottom_x = _left_top_x - (double)pixX / _pix_x_scale;
			}

			if (pixY > clientRect.height())
			{
				_pix_maxy = clientRect.height();
				_right_bottom_y = _left_top_y - (double)clientRect.y() / _pix_y_scale;
			}
			else
			{
				_pix_maxy = pixY;
				_right_bottom_y = _left_top_y - (double)pixY / _pix_y_scale;
			}
		}
		else if (_map_tool_status == MAP_ACTION::MA_ZOOMIN || _map_tool_status == MAP_ACTION::MA_ZOOMOUT)
		{
//			if (event->x() <= _pix_maxx && event->y() <= _pix_maxy)
			{
				if (_map_tool_status == MAP_ACTION::MA_ZOOMIN)
				{
					if (zoomLevel < 10)
					{
						zoomLevel++;
						set_global_variable(QPoint(event->x(), event->y()), _map_tool_status);
					}
				}
				else
				{
					if (zoomLevel > 0)
					{
						zoomLevel--;
						set_global_variable(QPoint(event->x(), event->y()), _map_tool_status);
					}
				}
			}
		}

		if (_map_tool_status == MAP_ACTION::MA_MOVE ||
			_map_tool_status == MAP_ACTION::MA_ZOOMIN ||
			_map_tool_status == MAP_ACTION::MA_ZOOMOUT)
		{
			pixRegionVec.clear();
			std::vector<Point2DArray>::iterator iter_pix = ptRegionArrayVec.begin();
			for (; iter_pix != ptRegionArrayVec.end(); iter_pix++)
			{
				Point2DArray pixArray;
				converWgs2Pix(clientRect, *iter_pix, pixArray);
				pixRegionVec.push_back(pixArray);
			}

			update();
		}
	}
}
 
void MainWindowFrame::mouseMoveEvent(QMouseEvent *e)
{
	WGS84Coord wgs = point2WGS84Coord(QPointF(e->x(), e->y()));
	double lon = 0.0;
	double lat = 0.0;
	dtmProcess.MetersToLonLat(wgs.x, wgs.y, lon, lat);
	if (lon >360)
	{
		lon -= 360;
	}

	QPalette pe;
	pe.setColor(QPalette::WindowText, Qt::blue);

	statusBarLabel->setPalette(pe);
	statusBarLabel->setText("                     WGS84: lon " + QString::number(lon) + ", lat " + QString::number(lat));
	statusBar()->addWidget(statusBarLabel);
}

void MainWindowFrame::set_global_variable(const QPointF& point, const MAP_ACTION map_action)
{
	if (min_pix_scale == 0)
	{
		return;
	}

//	QDesktopWidget* desktopWidget = QApplication::desktop();
//	clientRect = desktopWidget->screenGeometry();
	clientRect = geometry();
	double lon_del = 0.0;
	double lat_del = 0.0;
	WGS84Coord wgsptBefore = point2WGS84Coord(point);

	if (map_action == MA_ZOOMIN)
	{
		_lonDel_PerDTM = _lonDel_PerDTM / 2.0;
		_latDel_PerDTM = _latDel_PerDTM / 2.0;
	}
	else if (map_action == MA_ZOOMOUT)
	{
		_lonDel_PerDTM = _lonDel_PerDTM * 2.0;
		_latDel_PerDTM = _latDel_PerDTM * 2.0;
	}

	_pix_x_scale = (double)DTM_PNG_PIX / (double)(_lonDel_PerDTM);
	_pix_y_scale = (double)DTM_PNG_PIX / (double)(_latDel_PerDTM);

	_left_top_x = wgsptBefore.x - ((double)point.x() / _pix_x_scale);;
	_left_top_y = wgsptBefore.y + ((double)point.y() / _pix_y_scale);;
	
	double lon = 0.0;
	double lat = 0.0;
	dtmProcess.MetersToLonLat(_left_top_x, _left_top_y, lon, lat);

	_right_bottom_x = _left_top_x + (double)(clientRect.width()) / _pix_x_scale;
	_right_bottom_y = _left_top_y - (double)(clientRect.height()) / _pix_y_scale;
	if (_right_bottom_x > _lon_max)
	{
		_right_bottom_x = _lon_max;
	}

	if (_right_bottom_y < _lat_min)
	{
		_right_bottom_y = _lat_min;
	}

	QVector<TileID> tileIdList = getTileList(
		        WGS84Coord(_left_top_x, _left_top_y, 0.0),
		        WGS84Coord(_right_bottom_x, _right_bottom_y, 0.0),
				zoomLevel);

	_imageDTMList = GetFileList(DTM_PATH,
		                        zoomLevel,
		                        tileIdList);

	int pixX = _imageDTMList.size() * DTM_PNG_PIX;
	int pixY = _imageDTMList[0].size() * DTM_PNG_PIX;

	if (pixX > clientRect.width())
	{
		_pix_maxx = clientRect.width();
		_right_bottom_x = _left_top_x + (double)clientRect.width() / _pix_x_scale;
	}
	else
	{
		_pix_maxx = pixX;
		_right_bottom_x = _left_top_x + (double)pixX / _pix_x_scale;
	}

	if (pixY > clientRect.height())
	{
		_pix_maxy = clientRect.height();
		_right_bottom_y = _left_top_y - (double)clientRect.height() / _pix_y_scale;
	}
	else
	{
		_pix_maxy = pixY;
		_right_bottom_y = _left_top_y - (double)pixY / _pix_y_scale;
	}
}

WGS84Coord MainWindowFrame::point2WGS84Coord(const QPointF& point)
{
	WGS84Coord wgspt;
	if (min_pix_scale != 0)
	{
		wgspt.x = _left_top_x + (double)point.x() / _pix_x_scale;
		wgspt.y = _left_top_y - (double)point.y() / _pix_y_scale;
	}

	return wgspt;
}

QVector< QVector<QImageDTM> > MainWindowFrame::GetFileList(
	QString path,
	int dtmZoom,
	const QVector<TileID> &tileIdList,
	bool bIsInit)
{
	QVector< QVector<QImageDTM> > toalImage;
	QString pathname = path + QString::number(dtmZoom);
	QDir dir(path + QString::number(dtmZoom));
	QFileInfoList file_list = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
	QFileInfoList folder_list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

	for (int i = 0; i != folder_list.size(); i++)
	{
		QString strTx = folder_list.at(i).baseName();

		if (tileIdList.size() > 0)
		{
			if (!JudgeInTileX(strTx.toInt(), tileIdList))
			{
				continue;
			}
		}

		QString name = folder_list.at(i).absoluteFilePath();

		QDir subdir(name);
		QFileInfoList subfile_list = subdir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);

		QVector<QImageDTM> subImage;
		for (int j = 0; j != subfile_list.size(); j++)
		{
			QString strTy = subfile_list.at(j).baseName();

			if (tileIdList.size() > 0)
			{
				if (!JudgeInTileY(strTy.toInt(), tileIdList))
				{
					continue;
				}
			}

			QString subName = subfile_list.at(j).absoluteFilePath();
			QImageDTM qImageDtm;
			qImageDtm.tx = strTx.toInt();
			qImageDtm.ty = strTy.toInt();
			qImageDtm.zoom = dtmZoom;
			qImageDtm.imageDir = subName;
			if (bIsInit)
			{
				SetDTMLeftTopPIX(qImageDtm);
			}
			subImage.push_back(qImageDtm);
		}

		orderImageDTM(subImage);
		toalImage.push_back(subImage);
	}
	orderImageDTM(toalImage);

	QVector< QVector<QImageDTM> >::iterator iter;
	QVector< QImageDTM >::iterator im;

	/*
	for (iter = toalImage.begin(); iter != toalImage.end(); iter++)
	{
		for (im = iter->begin(); im != iter->end(); im++)
		{
			im->qImageDTM.load(im->imageDir);
		}
	}
	*/
	return toalImage;
}

void MainWindowFrame::SetDTMLeftTopPIX(QImageDTM &qDTM)
{
	double bound[4];
	dtmProcess.TileMetersBound(qDTM.tx, qDTM.ty, zoomLevel, bound);

	qDTM.metersX = bound[0];
	qDTM.metersY = bound[3];

	qDTM.pixX = (int)((qDTM.metersX - _left_top_x) * _pix_x_scale + 0.5);
	qDTM.pixY = (int)((_left_top_y - qDTM.metersY) * _pix_y_scale + 0.5);
}

bool MainWindowFrame::JudgeInTileX(const int tileX, const QVector<TileID> &tileIdList)
{
	if (tileIdList.size() <= 0)
	{
		return false;
	}
	
	QVector<TileID>::const_iterator iter = tileIdList.begin();
	for (; iter != tileIdList.end(); iter++)
	{
		if (iter->tx == tileX)
		{
			return true;
		}
	}

	return false;
}

bool MainWindowFrame::JudgeInTileY(const int tileY, const QVector<TileID> &tileIdList)
{
	if (tileIdList.size() <= 0)
	{
		return false;
	}

	QVector<TileID>::const_iterator iter = tileIdList.begin();
	for (; iter != tileIdList.end(); iter++)
	{
		if (iter->ty == tileY)
		{
			return true;
		}
	}

	return false;
}

void MainWindowFrame::orderImageDTM(QVector<QImageDTM> &qImageDTM)
{
	for (int i = 0; i != qImageDTM.size(); i++)
	{
		for (int j = i + 1; j < qImageDTM.size(); j++)
		{
			if (qImageDTM[i].ty < qImageDTM[j].ty)
			{
				QImageDTM tmp = qImageDTM[i];
				qImageDTM[i] = qImageDTM[j];
				qImageDTM[j] = tmp;
			}
		}
	}
}

void MainWindowFrame::orderImageDTM(QVector< QVector<QImageDTM> > &qImageDTM)
{
	for (int i = 0; i != qImageDTM.size(); i++)
	{
		for (int j = i + 1; j < qImageDTM.size(); j++)
		{
			if (qImageDTM[i].size() > 0 && qImageDTM[j].size() > 0)
			{
				if (qImageDTM[i][0].tx > (qImageDTM[j][0].tx))
				{
					QVector<QImageDTM> tmp = qImageDTM[i];
					qImageDTM[i] = qImageDTM[j];
					qImageDTM[j] = tmp;
				}
			}
		}
	}
}

void MainWindowFrame::drawDTM()
{
	QPainter painter(this);

//	QVector<TileID> tileIdList = getTileList(WGS84Coord(lon_min, lat_min, 0.0), 
//		                                     WGS84Coord(lon_max, lat_max, 0.0),
//											 zoomLevel);

//	QVector< QVector<QImageDTM> > qImageDTMList = GetFileList(DTM_PATH, zoomLevel, tileIdList);
    
//	if (!SetGlobalParam(qImageDTMList))
//	{
//		return;
//	}

	int px = 0; 
	int py = 0;
	int tmph = 0;

	QVector< QVector<QImageDTM> >::iterator iter;
	QVector< QImageDTM >::iterator im;

	for (iter = _imageDTMList.begin(); iter != _imageDTMList.end(); iter++)
	{
		tmph = py = 0;
		for (im = iter->begin(); im != iter->end(); im++)
		{
			//平滑
			QImage qtm;
			qtm.load(im->imageDir);

			qtm = qtm.scaled(qtm.width(), qtm.height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
			painter.drawPixmap(im->pixX, im->pixY, qtm.width(), qtm.height(), QPixmap::fromImage(qtm));
			py += qtm.width();
			tmph = qtm.height();
		}

		px += tmph;
	}
}

QVector<TileID> MainWindowFrame::getTileList(WGS84Coord start, WGS84Coord end, int zoom)
{
	QVector<TileID> tileIdList;

	int start_tx = 0;
	int start_ty = 0;
	///latitude Ascending from bottom to top
	dtmProcess.MetersToTile(start.x, start.y, zoom, start_tx, start_ty);

	int end_tx = 0;
	int end_ty = 0;
	dtmProcess.MetersToTile(end.x, end.y, zoom, end_tx, end_ty);

	for (int i = start_tx; i <= end_tx; i++)
	{
		for (int j = end_ty; j <= start_ty; j++)
		{
			TileID tileId;
			tileId.tx = i;
			tileId.ty = j;
			tileId.zoom = zoom;
			tileIdList.push_back(tileId);
		}
	}

	return tileIdList;
}

bool MainWindowFrame::GetTileListBoundingbox(
	const QVector< QVector<QImageDTM> > &qImageDTMList,
	const int zoom,
	double  &left_top_x,
	double  &left_top_y,
	double  &right_bottom_x,
    double  &right_bottom_y)
{
	if (qImageDTMList.size() <= 0 || qImageDTMList[0].size() <= 0)
	{
		return false;
	}

	double startbd[4];
	dtmProcess.TileMetersBound(qImageDTMList[0][0].tx, qImageDTMList[0][0].ty, zoom, startbd);

	left_top_x = startbd[0];
	left_top_y = startbd[3];

	int size = qImageDTMList.size();
	int subSize = qImageDTMList[size - 1].size();

	double endbd[4];
	dtmProcess.TileMetersBound(qImageDTMList[size - 1][subSize - 1].tx,
		                       qImageDTMList[size - 1][subSize - 1].ty,
							   zoom,
							   endbd);

	right_bottom_x = startbd[2];
	right_bottom_y = startbd[1];

	return true;
}

bool MainWindowFrame::GetTileListLeftTop(
	const QVector< QVector<QImageDTM> > &qImageDTMList,
	const int zoom,
	double  &left_top_x,
	double  &left_top_y)
{
	if (qImageDTMList.size() <= 0 || qImageDTMList[0].size() <= 0)
	{
		return false;
	}

	double startbd[4];
	dtmProcess.TileMetersBound(qImageDTMList[0][0].tx, qImageDTMList[0][0].ty, zoom, startbd);

	left_top_x = startbd[0];
	left_top_y = startbd[3];

	return true;
}

bool MainWindowFrame::SetGlobalParam(
	const QVector< QVector<QImageDTM> > &qImageDTMList)
{
	if (qImageDTMList.size() <= 0 || qImageDTMList[0].size() <= 0)
	{
		return false;
	}

	double  minx = 0.0;
	double  maxx = 0.0;
	double  miny = 0.0;
	double  maxy = 0.0;
	GetTileListBoundingbox(qImageDTMList, zoomLevel, minx, miny, maxx, maxy);

	int pixX = qImageDTMList.size() * DTM_PNG_PIX;
	int pixY = qImageDTMList[0].size() * DTM_PNG_PIX;

//	QDesktopWidget* desktopWidget = QApplication::desktop();
//	QRect cRect = desktopWidget->availableGeometry();
	QRect cRect = geometry();
	if (pixX < cRect.width() && pixY <= cRect.height())
	{

	}
	else
	{

//		int txNum, int tyNum;
//		GetMaxTileNumber(cRect, txNum, tyNum);
	}

	return true;
}

bool MainWindowFrame::GetTileListToltalPix(
	const QVector< QVector<QImageDTM> > &qImageDTMList,
	int &pixX,
	int &pixY)
{
	if (qImageDTMList.size() <= 0 || qImageDTMList[0].size() <= 0)
	{
		return false;
	}
	pixX = qImageDTMList.size() * DTM_PNG_PIX;
	pixY = qImageDTMList[0].size() * DTM_PNG_PIX;
	return true;
}

void MainWindowFrame::GetMaxTileNumber(QRect cRect, int &txNum, int &tyNum)
{
	txNum = cRect.width() / DTM_PNG_PIX + 2;
	tyNum = cRect.height() / DTM_PNG_PIX + 2;
}

QPoint MainWindowFrame::converMeters2Pix(double x, double y)
{
	double tmp_pixx = (double)(x - _left_top_x) * _pix_x_scale + clientRect.left();
	double tmp_pixy = (double)(_left_top_y - y) * _pix_y_scale + clientRect.top();
	return QPoint((int)tmp_pixx, (int)tmp_pixy);
}

void MainWindowFrame::DrawGrid()
{
	QRect rt = geometry();
	if (zoomLevel < 2)
	{
		return;
	}

	int scale = 10;
	if (zoomLevel < 5)
	{
		scale = 50;
	}
	else if (zoomLevel < 7)
	{
		scale = 10;
	}
	else if (zoomLevel < 11)
	{
		scale = 1;
	}
	QPainter painter(this);
	QPen penG = QPen(Qt::darkBlue, 1, Qt::DashLine);
	painter.setPen(penG);
	double lon_left_top = 0.0;
	double lat_left_top = 0.0;
	dtmProcess.MetersToLonLat(_left_top_x, _left_top_y, lon_left_top, lat_left_top);

	int leftx = (lon_left_top / scale + 1) * scale;
	int lefty = (lat_left_top / scale + 1) * scale;

	double max_x = _left_top_x + clientRect.width() / _pix_x_scale;
	double min_y = _left_top_y - clientRect.height() / _pix_x_scale;

	if (lefty < -89)
	{
		lefty = -89;
	}

	if (lefty > 89)
	{
		lefty = 89;
	}

	double mx = 0.0;
	double my = 0.0;
	dtmProcess.LonLatToMeters(leftx, lefty, mx, my);

	while (mx <= max_x)
	{
		QPoint fPoint = converMeters2Pix(mx, _left_top_y);
		fPoint = QPoint(fPoint.x(), rt.y());
		QPoint ePoint = QPoint(fPoint.x(), rt.height());
		painter.drawLine(fPoint, ePoint);

		painter.drawText(QPoint(fPoint.x(), rt.y() + 20), QString::number(leftx));
		painter.drawText(ePoint, QString::number(leftx));

		leftx += scale;
		double ty = 0.0;
		dtmProcess.LonLatToMeters(leftx, _left_top_y, mx, ty);
	}
	
	while (my >= min_y)
	{
		QPoint fPoint = converMeters2Pix(_left_top_x, my);
		fPoint = QPoint(0, fPoint.y());
		QPoint ePoint = QPoint(clientRect.width(), fPoint.y());
		painter.drawLine(fPoint, ePoint);

		painter.drawText(fPoint, QString::number(lefty));
		painter.drawText(ePoint, QString::number(lefty));

		lefty -= scale;
		dtmProcess.LonLatToMeters(_left_top_x, lefty, mx, my);


	}
}

void MainWindowFrame::createEagleEye()
{
	QRect rt = geometry();
	QPainter painter(this);
//	painter.setPen(penRegion);

	painter.setBrush(_brush_eagle_eye);
	double minx = rt.width() - rt.height() / 4;
	double miny = rt.height() * 4 / 5;
	double length = rt.height() / 5;

	QRect rtEye = QRect(
		minx,
		miny,
		length,
		length);
	painter.drawRect(rtEye);

	_eagle_eye_image.scaled(DTM_PNG_PIX, DTM_PNG_PIX, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	painter.drawPixmap(minx, miny, length, length, QPixmap::fromImage(_eagle_eye_image));

	if (zoomLevel != 1)
	{
		double sub_w = length *
			           (_right_bottom_x - _left_top_x) /
					   (_eagle_right_bottom_x - _eagle_left_top_x);

		double sub_h = length *
			           (_left_top_y - _right_bottom_y) /
					   (_eagle_left_top_y - _eagle_right_bottom_y);

		if (sub_w < 10 || sub_h < 10)
		{
			sub_w = 10;
			sub_h = 10;
		}

		double top_x = fabs(length * (_left_top_x - _eagle_left_top_x) / (_eagle_right_bottom_x - _eagle_left_top_x)) + minx;
		double top_y = fabs(length * (_eagle_left_top_y - _left_top_y) / (_eagle_left_top_y - _eagle_right_bottom_y)) + miny;

		QRect sub_rt = QRect(
			top_x,
			top_y,
			sub_w,
			sub_h);

		QPainter subpainter(this);
		subpainter.setPen(_pen_eagle_eye);
		subpainter.drawRect(sub_rt);
	}
}
