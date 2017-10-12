#include <QtWidgets>

#include "mainwindowframe.h"
#include <QPainter>
#include "geomertyconvertor.h"

MainWindowFrame::MainWindowFrame()
{
    init();
	setWindowTitle("AirPhotoPlanner");
	penRegion = QPen(Qt::blue, 3);
	penRoute = QPen(Qt::red, 2);
	min_pix_scale = 1;
	_map_tool_status = MAP_ACTION::MA_NULL;
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

	std::vector<Point2DArray>::iterator iter = ptRegionArrayVec.begin();
	for (; iter != ptRegionArrayVec.end(); iter++)
	{
		Point2DArray::iterator subit = (*iter).begin();

		int start_x = subit->X;
		int start_y = subit->Y;
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

	lon_min = minx - (maxx - minx) / 8.0;
	lon_max = maxx + (maxx - minx) / 8.0;
	lat_min = miny - (maxy - miny) / 8.0;
	lat_max = maxy + (maxy - miny) / 8.0;
	
	QDesktopWidget* desktopWidget = QApplication::desktop();
	clentRect = desktopWidget->screenGeometry();

	std::vector<Point2DArray>::iterator iter_pix = ptRegionArrayVec.begin();
	for (; iter_pix != ptRegionArrayVec.end(); iter_pix++)
	{
		Point2DArray pixArray;
		converWgs2Pix(clentRect, *iter_pix, pixArray);
		pixRegionVec.push_back(pixArray);
	}
}

void MainWindowFrame::converWgs2Pix(
	const QRect &clientRect,
	const Point2DArray &srcArray,
	Point2DArray &desArray)
{
	double pix_x_scale = (double)(clientRect.right() - clientRect.left()) / (double)(lon_max - lon_min);
	double pix_y_scale = (double)(clientRect.bottom() - clientRect.top()) / (double)(lat_max - lat_min);
	min_pix_scale = (pix_x_scale > pix_y_scale) ? pix_y_scale : pix_x_scale;

	Point2DArray::const_iterator iter = srcArray.begin();
	for (; iter != srcArray.end(); iter++)
	{
		Point2D pixpt;
		double tmp_pix = (double)(iter->X - lon_min) * min_pix_scale + clientRect.left();
		pixpt.X = (int)tmp_pix;

		tmp_pix = (double)(lat_max - iter->Y) * min_pix_scale + clientRect.top();
		pixpt.Y = (int)tmp_pix;
		desArray.push_back(pixpt);
	}
}


void MainWindowFrame::about()
{
   QMessageBox::about(this, tr("关于我们"),
            tr("AirPhotoPlanner"
               "New"));
}

void MainWindowFrame::documentWasModified()
{
    setWindowModified(true);
}

void MainWindowFrame::init()
{
    resize(QApplication::desktop()->availableGeometry().size());
//    showFullScreen();
//    setWindowState(Qt::WindowFrameSection);
    isUntitled = true;

 //   textEdit = new QTextEdit;
 //   setCentralWidget(textEdit);
    createActions();
    createMenus();
    createToolBars();
    createStatusBar();

    Qt::WindowFlags flags = Qt::Window;
    flags |=Qt::WindowMinimizeButtonHint;
    flags |=Qt::WindowMaximizeButtonHint;
    flags |=Qt::WindowCloseButtonHint;
    setWindowFlags(flags);
//    setWindowFlags(Qt::WindowMaximizeButtonHint& ~Qt::WindowMinimizeButtonHint);

 //   readSettings();

 //   connect(textEdit->document(), SIGNAL(contentsChanged()),
 //           this, SLOT(documentWasModified()));

 //   setUnifiedTitleAndToolBarOnMac(true);
}

void MainWindowFrame::createActions()
{
    openKML = new QAction(QIcon(":/images/open.png"), tr("&打开摄区KML文件"), this);
//  openKML->setShortcuts(QKeySequence::Open);
    openKML->setStatusTip(tr("Open an KML file"));
    connect(openKML, SIGNAL(triggered()), this, SLOT(open()));

    setParameter = new QAction(QIcon(":/images/setting.png"), tr("&规划航线"), this);
    setParameter->setStatusTip(tr("Start Plan"));
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

    aboutAct = new QAction(tr("&关于"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
//    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

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
    fileMenu = menuBar()->addMenu(tr("&文件"));
//! [implicit tr context]
    fileMenu->addAction(openKML);
    fileMenu->addAction(setParameter);
    fileMenu->addAction(exitAct);

    mapMenu = menuBar()->addMenu(tr("&地图操作"));
    mapMenu->addAction(zoomin);
    mapMenu->addAction(zoomout);
    mapMenu->addAction(pan);
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
}

void MainWindowFrame::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
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
 //   ptrChild_TV = new child_tv(this);
//    param_window.move(this->x() + this->frameSize().width(), this->y());
//    param_window.setKMLFileList(listInputKmlFile);
//    param_window.show();
}

void MainWindowFrame::paintEvent(QPaintEvent *)
{
   QPainter painter(this);
   painter.setPen(penRegion);

   std::vector<Point2DArray>::iterator iter = pixRegionVec.begin();
   for(; iter != pixRegionVec.end(); iter++)
   {
	   Point2DArray::iterator subit = (*iter).begin();

	   int start_x = subit->X;
	   int start_y = subit->Y;
	   subit++;

	   for (; subit != iter->end(); subit++)
	   {
		   painter.drawLine(QPointF(start_x, start_y),
			                QPointF(subit->X, subit->Y));

		   start_x = subit->X;
		   start_y = subit->Y;
	   }
   }

   painter.setPen(penRoute);
   std::vector<Point2DArray>  ptArray = param_window.getRouteDesignPoint();
   std::vector<Point2DArray>::iterator iter_pix = ptArray.begin();
   pixRouteVec.clear();
   for (; iter_pix != ptArray.end(); iter_pix++)
   {
	   Point2DArray pixArray;
	   converWgs2Pix(clentRect, *iter_pix, pixArray);
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

   std::vector<Point2DArray> pixRouteVec;
}

void MainWindowFrame::zoominMap()
{
	_map_tool_status = MAP_ACTION::MA_ZOOMIN;
}

void MainWindowFrame::zoomoutMap()
{
	_map_tool_status = MAP_ACTION::MA_ZOOMOUT;
}

void MainWindowFrame::moveMap()
{
	_map_tool_status = MAP_ACTION::MA_MOVE;
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
			int x_move = event->x() - _left_button_down_pt.x();
			int y_move = event->y() - _left_button_down_pt.y();

			lon_min -= double(x_move) / min_pix_scale;
			lon_max -= double(x_move) / min_pix_scale;

			lat_min += double(y_move) / min_pix_scale;
			lat_max += double(y_move) / min_pix_scale;
		}
		else if (_map_tool_status == MAP_ACTION::MA_ZOOMIN || _map_tool_status == MAP_ACTION::MA_ZOOMOUT)
		{
			set_global_variable(QPoint(event->x(), event->y()), _map_tool_status);
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
				converWgs2Pix(clentRect, *iter_pix, pixArray);
				pixRegionVec.push_back(pixArray);
			}

			update();
		}
	}
}

void MainWindowFrame::set_global_variable(const QPointF& point, const MAP_ACTION map_action)
{
	if (min_pix_scale == 0)
	{
		return;
	}

	QDesktopWidget* desktopWidget = QApplication::desktop();
	clentRect = desktopWidget->screenGeometry();

	double lon_del = 0.0;
	double lat_del = 0.0;
	if (map_action == MA_ZOOMIN)
	{
		lon_del = (lon_max - lon_min) / 2.0;
		lat_del = (lat_max - lat_min) / 2.0;
	}
	else if (map_action == MA_ZOOMOUT)
	{
		lon_del = (lon_max - lon_min) * 2;
		lat_del = (lat_max - lat_min) * 2;
	}
	if (lon_del == 0 || lat_del == 0)
	{
		return;
	}

	double pix_x_scale = (double)(clentRect.right() - clentRect.left()) / (double)(lon_del);
	double pix_y_scale = (double)(clentRect.bottom() - clentRect.top()) / (double)(lat_del);
	min_pix_scale = (pix_x_scale > pix_y_scale) ? pix_y_scale : pix_x_scale;

	if (min_pix_scale == 0)
	{
		return;
	}

	WGS84Coord wgspt = point2WGS84Coord(point);

	lon_min = wgspt.x - ((double)point.x() / min_pix_scale);
	lon_max = lon_min + lon_del;

	lat_max = wgspt.y + ((double)point.y() / min_pix_scale);
	lat_min = lat_max - lat_del;
}

WGS84Coord MainWindowFrame::point2WGS84Coord(const QPointF& point)
{
	WGS84Coord wgspt;
	if (min_pix_scale != 0)
	{
		wgspt.x = lon_min + (double)(point.x() - clentRect.left()) / min_pix_scale;
		wgspt.y = lat_max - (double)(point.y() - clentRect.top()) / min_pix_scale;
	}

	return wgspt;
}
