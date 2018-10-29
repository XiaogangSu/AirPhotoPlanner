#include <QtWidgets>

#include "mainwindowframe.h"
#include <QPainter>
#include "geomertyconvertor.h"

MainWindowFrame::MainWindowFrame()
{
	//	QDesktopWidget* desktopWidget = QApplication::desktop();
	//	clientRect = desktopWidget->screenGeometry();
	//	clientRect = geometry();
	//设置背景黑色
	QPalette pal(this->palette());
	pal.setColor(QPalette::Background, Qt::black);
	setAutoFillBackground(true);
	setPalette(pal);

	setWindowState(Qt::WindowMaximized);
	isUntitled = true;

	//	clientRect = desktopWidget->screenGeometry();
	//	clientRect = geometry();
	//   textEdit = new QTextEdit;
	//   setCentralWidget(textEdit);
	createActions();
	createMenus();
	createToolBars();
	createStatusBar();

	setWindowTitle("AirPhotoPlanner");

	min_pix_scale = 1;
	_map_tool_status = MAP_ACTION::MA_NULL;

	penRoute = QPen(Qt::red, 2);

	penRegion_Not_Select = QPen(Qt::blue, 3);
	penRegion = QPen(Qt::yellow, 3);

	_pen_eagle_eye = QPen(Qt::blue, 1);
	_brush_eagle_eye = QBrush(Qt::darkGray);

	gradRegion =  QRadialGradient(50, 50, 50, 70, 70);
//	gradRegion.setColorAt(0.0, Qt::black);
	brushRegion_Not_Select = QBrush(QColor(0, 0, 255, 80), Qt::CrossPattern); // 最后一项为透明度
	brushRegion = QBrush(QColor(255, 255, 0, 80), Qt::CrossPattern); // 最后一项为透明度

	pixAirport.load(":/images/airport.png");
	brushAirport = QBrush(QPixmap(":/images/airport.png"));
	statusBarLabel = new QLabel;
	setMouseTracking(true);

	_zoom_out_cursor = new QCursor(QPixmap(":/images/zoom_out.png"), -1, -1);
	_zoom_in_cursor = new QCursor(QPixmap(":/images/zoom_in.png"), -1, -1);
	_pan_cursor = new QCursor(QPixmap(":/images/pan.png"), -1, -1);
	_pen_grad = QPen(Qt::darkBlue, 1, Qt::DashLine);
	_pen_scale = QPen(Qt::black, 2, Qt::SolidLine);

	bStartPlan = false;
	_b_flag_show_grad = true;
	_b_flag_show_scale = true;
	_b_flag_show_eagle = true;
	_b_flag_show_dtm = true;
	_b_flag_show_image = true;

	init();
	InitTreeWidget();

	m_zoneParam = new ZoneParamDialog(this);
	m_zoneParam->hide();
//	createEagleEye();
}

void MainWindowFrame::InitTreeWidget()
{
	_area_list_widget = new QTreeWidget;
	_area_list_widget->setHeaderLabels(QStringList() << " " << " " << " ");
	_area_list_widget->setColumnWidth(0, 130);
	_area_list_widget->setColumnWidth(1, 100);
	_area_list_widget->setWindowTitle(" ");

	_p_basic_tree_item = new QTreeWidgetItem(QStringList() << "基本设置");
//	_p_basic_tree_item->setCheckState(0, Qt::Checked);
	_p_zone_tree_item = new QTreeWidgetItem(QStringList() << "摄区管理");

	_p_level_tree_item = new QTreeWidgetItem(QStringList() << "图层选择");
//	_p_zone_tree_item->setCheckState(0, Qt::Checked);

	_area_list_widget->addTopLevelItem(_p_basic_tree_item);
	_area_list_widget->addTopLevelItem(_p_zone_tree_item);
	_area_list_widget->addTopLevelItem(_p_level_tree_item);

	QTreeWidgetItem *item_show = new QTreeWidgetItem;
	item_show->setText(0, "显示网格");
	item_show->setCheckState(0, Qt::Checked);
	_p_basic_tree_item->addChild(item_show);

	QTreeWidgetItem *child_color = new QTreeWidgetItem;
	child_color->setText(0, "网格颜色");
	_comb_clolor_grad = new QComboBox(this);
	connect(_comb_clolor_grad, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comb_clolor_changed()));
	QStringList colorNameList = QColor::colorNames();
	QString colorName;
	foreach(colorName, colorNameList)
	{
		QPixmap pix_color(70, 20);
		pix_color.fill(QColor(colorName));
		_comb_clolor_grad->addItem(QIcon(pix_color), NULL);
		_comb_clolor_grad->setIconSize(QSize(70, 20));
		_comb_clolor_grad->setSizeAdjustPolicy(QComboBox::AdjustToContents);	//设置下拉列表的尺寸符合内容的大小
	}
	_comb_clolor_grad->setCurrentIndex(4);
	_pen_grad.setColor(QColor(colorNameList[4]));
	_p_basic_tree_item->addChild(child_color);
	_area_list_widget->setItemWidget(child_color, 1, _comb_clolor_grad);

	QTreeWidgetItem *item_eagle = new QTreeWidgetItem;
	item_eagle->setText(0, "显示鹰眼");
	item_eagle->setCheckState(0, Qt::Checked);
	_p_basic_tree_item->addChild(item_eagle);

	QTreeWidgetItem *item_scale = new QTreeWidgetItem;
	item_scale->setText(0, "显示比例");
	item_scale->setCheckState(0, Qt::Checked);
	_p_basic_tree_item->addChild(item_scale);

	QTreeWidgetItem *item_dtm = new QTreeWidgetItem;
	item_dtm->setText(0, "DTM");
	item_dtm->setCheckState(0, Qt::Checked);
	_p_level_tree_item->addChild(item_dtm);

	QTreeWidgetItem *item_image = new QTreeWidgetItem;
	item_image->setText(0, "影像");
	item_image->setCheckState(0, Qt::Checked);
	_p_level_tree_item->addChild(item_image);

	_area_list_widget->show();
	_area_list_widget->setMouseTracking(true);
	_area_list_widget->expandAll();

	_p_tree_menu = new QMenu();//tree的右键菜单  
	_p_tree_menu->setTitle("摄区参数");
	connect(_p_tree_menu, SIGNAL(triggered()), this, SLOT(on_zoom_param_menu()));
//	connect(_area_list_widget, SIGNAL(itemChanged(QTreeWidgetItem * item, int column)), this,
//		     SLOT(treeItemChanged(QTreeWidgetItem * item, int column)));
//	connect(_area_list_widget, &QStandardItemModel::itemChanged, this, &MainWindowFrame::treeItemChanged);

	connect(_area_list_widget, SIGNAL(itemPressed(QTreeWidgetItem*, int)), this, SLOT(slotItemPressed(QTreeWidgetItem*, int)));

	connect(_area_list_widget, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
		this, SLOT(treeItemChanged(QTreeWidgetItem*)));
	connect(_area_list_widget, SIGNAL(itemChanged(QTreeWidgetItem*, int)),
		this, SLOT(slotItemChanged(QTreeWidgetItem*, int)));
        
}

void MainWindowFrame::init()
{
	zoomLevel = MIN_ZOOM_SCALE;
	QVector<TileID> tileIdList;
	//get image
	_map_image_list =
		GetFileList(
		IMAGE_PATH,
		zoomLevel,
		tileIdList,
		false);

	//get DTM
	GetDTMFile();

	if (_map_image_list.size() > 0)
	{
		GetTileListBoundingbox(_map_image_list, zoomLevel, _left_top_x, _left_top_y, _right_bottom_x, _right_bottom_y);

		//the map move to center
		dtmProcess.LonLatToMeters(-174.0, 75.0, _left_top_x, _left_top_y);
		_pix_maxx = _map_image_list.size() * DTM_PNG_PIX;
		_pix_maxy = _map_image_list[0].size() * DTM_PNG_PIX;
		_lon_max = _right_bottom_x;
		_lat_min = _right_bottom_y;

		double boundingbox[4];
		dtmProcess.TileMetersBound(_map_image_list[0][0].tx, _map_image_list[0][0].ty, zoomLevel, boundingbox);
		_lonDel_PerDTM = boundingbox[2] - boundingbox[0];
		_latDel_PerDTM = boundingbox[3] - boundingbox[1];

		_pix_x_scale = (double)DTM_PNG_PIX / (double)(_lonDel_PerDTM);
		_pix_y_scale = (double)DTM_PNG_PIX / (double)(_latDel_PerDTM);
	}

	QVector< QVector<QImageDTM> > _image_eagle_list;
	tileIdList.clear();
	_image_eagle_list =
		GetFileList(
		IMAGE_PATH,
		0,
		tileIdList,
		false);

	if (_map_image_list.size() > 0)
	{
		_eagle_eye_image.load(_image_eagle_list[0][0].imageDir);
		GetTileListBoundingbox(_map_image_list, zoomLevel, _eagle_left_top_x,
			                   _eagle_left_top_y, _eagle_right_bottom_x, _eagle_right_bottom_y);
	}
}

void MainWindowFrame::resizeEvent(QResizeEvent* size)
{
	QRect rt = this->geometry();
	_map_client_rect = QRect(
		QPoint(TREE_WIDTH, rt.y() + fileToolBar->height()),
		rt.bottomRight());

	QVector< QVector<QImageDTM> >::iterator iter;
	QVector< QImageDTM >::iterator im;
	for (iter = _map_image_list.begin(); iter != _map_image_list.end(); iter++)
	{
		for (im = iter->begin(); im != iter->end(); im++)
		{
			SetDTMLeftTopPIX(*im);
		}
	}

	_b_flag_resize = true;
}

void MainWindowFrame::create_list()
{
	if (_b_flag_resize)
	{
		QRect rt = this->geometry();
		rt.setWidth(TREE_WIDTH);
		int hight = fileToolBar->height();
		rt.setTop(rt.y() + hight);
		_area_list_widget->setParent(this);
		_area_list_widget->setGeometry(rt);
		_area_list_widget->show();
	}

	_b_flag_resize = false;
}

/*
void MainWindowFrame::on_comb_comb_show_grad_changed()
{
	if (_comb_show_grad->currentIndex() == 0)
	{
		_b_flag_show_grad = true;
	}
	else
	{
		_b_flag_show_grad = false;
	}
	update();
}
*/
void MainWindowFrame::on_comb_clolor_changed()
{
	int num = _comb_clolor_grad->currentIndex();
	QStringList colorNameList = QColor::colorNames();
	_pen_grad.setColor(QColor(colorNameList[num]));
	update();
}

void MainWindowFrame::treeItemChanged(QTreeWidgetItem * item)
{
	if (item == nullptr)
	{
		return;
	}
}

void MainWindowFrame::on_zone_param()
{
	ZoneParamDialog dlg(this);
	if (_zone_param_index < _regionInfo.size())
	{
		dlg.setZoneParam(
			_regionInfo[_zone_param_index].regionName,
			_regionInfo[_zone_param_index].base_height);
	}
	else
	{
		dlg.setZoneParam(
			"",
			0.0);
	}

	int res = dlg.exec();

	double zone_height = dlg.get_zone_hight();
	if (_zone_param_index < _regionInfo.size())
	{
		_regionInfo[_zone_param_index].base_height = zone_height;
	}
}

void MainWindowFrame::slotItemPressed(QTreeWidgetItem *item, int column)
{
	QTreeWidgetItem * pr = item->parent();
	if (pr == _p_zone_tree_item)
	{
		if (!(QGuiApplication::mouseButtons() & Qt::RightButton))
		{
			return;
		}

		_zone_param_index = pr->indexOfChild(item);
		QMenu menu;
		menu.addAction(zoneParamAct);
		menu.exec(QCursor::pos());
	}
}

void MainWindowFrame::slotItemChanged(QTreeWidgetItem *item, int column)
{
	QTreeWidgetItem * pr = item->parent();
	if (pr == _p_basic_tree_item)
	{
		if (item->text(column) == "显示网格")
		{
			_b_flag_show_grad = item->checkState(column);
		}
		else if (item->text(column) == "显示鹰眼")
		{
			_b_flag_show_eagle = item->checkState(column);
		}
		else if (item->text(column) == "显示比例")
		{
			_b_flag_show_scale = item->checkState(column);
		}
	}
	else if (pr == _p_zone_tree_item)
	{
		int num = pr->indexOfChild(item);

		if (_regionInfo.size() > num)
		{
			_regionInfo[num].bSelect = item->checkState(column);
			if (_regionInfo[num].bSelect)
			{
				center_to_region();
			}
		}
	}
	else if (pr == _p_level_tree_item)
	{
		if (item->text(column) == "DTM")
		{
			_b_flag_show_dtm = item->checkState(column);
		}
		else if (item->text(column) == "影像")
		{
			_b_flag_show_image = item->checkState(column);
		}
	}

	update();
}

MainWindowFrame::MainWindowFrame(const QString &fileName)
{
 //   init();
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
			//not free the memory
            QStringList slist = dlg.selectedFiles();
            listInputKmlFile.clear();
			_regionInfo.clear();

            for (int i = 0; i < slist.size(); ++i)
			{
				listInputKmlFile.push_back(slist.at(i));
				//std::auto_ptr<OGRGeometry> regionORG = COGRGeometryFileReader::GetFirstOGRGeometryFromFile(slist.at(i).toStdString());
				//Point2DArray ptArray;
				//GeomertyConvertor::OGRGeomery2Point2DArray(regionORG.get(), ptArray);
				
                Point2DArray ptArray;
                COGRGeometryFileReader::GetPointsFromFile(ptArray, slist.at(i).toStdString());
				ptRegionArrayVec.push_back(ptArray);

				WGS84Coord min_coord;
				WGS84Coord max_coord;
				min_coord.x = 99999.0;
				min_coord.y = 99999.0;
				max_coord.x = 0;
				max_coord.y = 0;

				Point2DArray::const_iterator iter = ptArray.begin();
				for (; iter != ptArray.end(); iter++)
				{
					if (min_coord.x > iter->X)
					{
						min_coord.x = iter->X;
					}
					if (min_coord.y > iter->Y)
					{
						min_coord.y = iter->Y;
					}

					if (max_coord.x < iter->X)
					{
						max_coord.x = iter->X;
					}
					if (max_coord.y < iter->Y)
					{
						max_coord.y = iter->Y;
					}
				}

				///get KML NAME
				int len = slist.at(i).length();
				int j = len;
				for (; j >= 0; j--)
				{
					if (slist.at(i).mid(j, 1) == "/")
					{
						break;
					}
				}

				QString str = slist.at(i).mid(j + 1, len - j - 5);
				RegionInfo info;
				info.regionName = str;
				info.bSelect = true;
				info.base_height = INVALID_ZONE_HEIGHT;
				dtmProcess.LonLatToMeters(min_coord.x, min_coord.y, info.minCoord.x, info.minCoord.y);
				dtmProcess.LonLatToMeters(max_coord.x, max_coord.y, info.maxCoord.x, info.maxCoord.y);
				_regionInfo.push_back(info);
			}
        }
    }

	center_to_region();
    createRegionPixPoint();
	
	if (_p_zone_tree_item->childCount() > 0)
	{
		for (int i = 0; i < _p_zone_tree_item->childCount(); i++)
		{
			_p_zone_tree_item->removeChild(_p_zone_tree_item->child(i));
		}
	}
	std::vector<RegionInfo>::const_iterator iter = _regionInfo.begin();
	for (; iter != _regionInfo.end(); iter++)
	{
		QTreeWidgetItem *item_zoom = new QTreeWidgetItem;
		item_zoom->setText(0, iter->regionName);
		item_zoom->setCheckState(0, Qt::Checked);
		_p_zone_tree_item->addChild(item_zoom);
	}

	create_list();

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

void MainWindowFrame::center_to_region()
{
	std::vector<RegionInfo>::const_iterator iter =  _regionInfo.begin();
	double minx = 999999999.9;
	double miny = 999999999.9;
	double maxx = -999999999.9;
	double maxy = -999999999.9;
	bool flag = false;
	for (; iter != _regionInfo.end(); iter++)
	{
		if (!iter->bSelect)
		{
			continue;
		}

		if (minx > iter->minCoord.x)
		{
			minx = iter->minCoord.x;
		}
		if (maxx < iter->maxCoord.x)
		{
			maxx = iter->maxCoord.x;
		}
		if (miny > iter->minCoord.y)
		{
			miny = iter->minCoord.y;
		}
		if (maxy < iter->maxCoord.y)
		{
			maxy = iter->maxCoord.y;
		}

		flag = true;
	}

	if (!flag)
	{
		return;
	}

	double tmp_lon_del = maxx - minx;
	double tmp_lat_del = maxy - miny;

	double tmpx = _right_bottom_x - _left_top_x;
	double tmpy = _left_top_y - _right_bottom_y;

	if (tmpx < tmp_lon_del || tmpy < tmp_lat_del)
	{
		do 
		{
			if (zoomLevel == MIN_ZOOM_SCALE)
			{
				break;
			}
			tmpx = tmpx * 2.0;
			tmpy = tmpy * 2.0;

			_lonDel_PerDTM *= 2.0;
			_latDel_PerDTM *= 2.0;
			zoomLevel--;
		} while (tmpx < tmp_lon_del || tmpy < tmp_lat_del);
	}
	else
	{
		while (tmpx > tmp_lon_del && tmpy > tmp_lat_del)
		{
			if (zoomLevel == MAX_ZOOM_SCALE)
			{
				break;
			}

			_lonDel_PerDTM /= 2.0;
			_latDel_PerDTM /= 2.0;

			tmpx = _lonDel_PerDTM * _map_client_rect.width() / (double)DTM_PNG_PIX;
			tmpy = _latDel_PerDTM *_map_client_rect.height() / (double)DTM_PNG_PIX;

			if (tmpx > tmp_lon_del && tmpy > tmp_lat_del)
			{
				zoomLevel++;
			}
			else
			{
				_lonDel_PerDTM *= 2.0;
				_latDel_PerDTM *= 2.0;

				tmpx *= 2.0;
				tmpy *= 2.0;
				break;
			}
		}
	}

	_pix_x_scale = (double)DTM_PNG_PIX / (double)(_lonDel_PerDTM);
	_pix_y_scale = (double)DTM_PNG_PIX / (double)(_latDel_PerDTM);

	_left_top_x = minx - (tmpx - tmp_lon_del) / 2;
	_left_top_y = maxy + (tmpy - tmp_lat_del) / 2;

//	double lon = min_region.x - (tmpx - tmp_lon_del) / 2;
//	double lat = max_region.y + (tmpy - tmp_lat_del) / 2;
//	dtmProcess.LonLatToMeters(lon, lat, _left_top_x, _left_top_y);

	_right_bottom_x = _left_top_x + (double)(_map_client_rect.width()) / _pix_x_scale;
	_right_bottom_y = _left_top_y - (double)(_map_client_rect.height()) / _pix_y_scale;


	if (_right_bottom_x > _lon_max)
	{
		_right_bottom_x = _lon_max;
	}

	if (_right_bottom_y < _lat_min)
	{
		_right_bottom_y = _lat_min;
	}

	reset_bottom_xy();
	createRegionPixPoint();
	update();
}

void MainWindowFrame::createRegionPixPoint()
{
	pixRegionVec.clear();
	std::vector<Point2DArray>::iterator iter_pix = ptRegionArrayVec.begin();
	for (; iter_pix != ptRegionArrayVec.end(); iter_pix++)
	{
		Point2DArray pixArray;
		converWgs2Pix(_map_client_rect, *iter_pix, pixArray);
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

QPoint MainWindowFrame::converWgs2Pix(const QRect &clientRect, WGS84Coord wgs)
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

    setParameter = new QAction(QIcon(":/images/setting.png"), tr("&规划参数"), this);
    setParameter->setStatusTip(tr("Save the document to disk"));
    connect(setParameter, SIGNAL(triggered()), this, SLOT(setPlanParameter()));

	startPlan = new QAction(QIcon(":/images/start.png"), tr("&规划"), this);
	startPlan->setStatusTip(tr("Start Plan"));
	connect(startPlan, SIGNAL(triggered()), this, SLOT(on_start_plan()));

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

	zoneParamAct = new QAction(tr("&设置参数"), this);
	connect(zoneParamAct, SIGNAL(triggered()), this, SLOT(on_zone_param()));

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
	fileMenu->addAction(startPlan);
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
	fileToolBar->setAutoFillBackground(true);
	fileToolBar->setMouseTracking(true);
    mapToolBar = addToolBar(tr("Map"));
	mapToolBar->addAction(zoomin);
	mapToolBar->addAction(zoomout);
    mapToolBar->addAction(pan);
	mapToolBar->addAction(selectObj);
	mapToolBar->setAutoFillBackground(true);
	mapToolBar->setMouseTracking(true);
	startToolBar = addToolBar(tr("Run"));
	startToolBar->addAction(startPlan);
	startToolBar->setAutoFillBackground(true);
	startToolBar->setMouseTracking(true);
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
}

void MainWindowFrame::on_start_plan()
{
	double base_height = param_window.getBaseHeight();
	double flightHeigt = param_window.getFlightHeigh(base_height);

	std::vector<double> zoneflighHeight_vec;
	std::vector<RegionInfo>::const_iterator iter = _regionInfo.begin();
	for (; iter != _regionInfo.end(); iter++)
	{
		if (iter->base_height > INVALID_ZONE_HEIGHT)
		{
			double tmp = param_window.getFlightHeigh(iter->base_height);
			zoneflighHeight_vec.push_back(tmp);
		}
		else
		{
			zoneflighHeight_vec.push_back(flightHeigt);
		}
	}

	param_window.setZoneHeight(zoneflighHeight_vec);
	param_window.start_Design();
	QMessageBox::information(this,
		tr("Success"),
		tr("Air line planning completion!"));
}
void MainWindowFrame::paintEvent(QPaintEvent *)
{
	DrawImage();
	DrawDTM();
	DrawGrid();
	DrawScale();
	QPainter pRegion(this);
//	pRegion.setBrush(gradRegion);

   std::vector<Point2DArray>::iterator iter = pixRegionVec.begin();
   int index = 0;
   for(; iter != pixRegionVec.end(); iter++)
   {
	   pRegion.setBrush(brushRegion_Not_Select);
	   pRegion.setPen(penRegion_Not_Select);
	   if (_regionInfo.size() > index)
	   {
		   if (_regionInfo[index].bSelect)
		   {
			   pRegion.setBrush(brushRegion);
			   pRegion.setPen(penRegion);
		   }
	   }

	   index++;
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
		   converWgs2Pix(_map_client_rect, *iter_pix, pixArray);
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
	   QPoint qpt = converWgs2Pix(_map_client_rect, WGS84Coord(lon, lat, 0));
	   painter.setBrush(brushAirport);
	   painter.drawPixmap(qpt, pixAirport);
   }

   DrawEagleEye();
   create_list();
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
	this->setCursor(Qt::ClosedHandCursor);
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

			int x_move = event->x() - _left_button_down_pt.x();
			int y_move = event->y() - _left_button_down_pt.y();

			_left_top_x -= double(x_move) / _pix_x_scale;
			_right_bottom_x -= double(x_move) / _pix_x_scale;
			//if (_left_top_x < 0)
			{
				_right_bottom_x = _left_top_x + double(_map_client_rect.width()) / _pix_x_scale;
			}

			_left_top_y += double(y_move) / _pix_y_scale;
			_right_bottom_y += double(y_move) / _pix_y_scale;

			//if (_left_top_y < 0)
			{
				_right_bottom_y = _left_top_y - double(_map_client_rect.height()) / _pix_y_scale;
			}

			QVector<TileID> tileIdList = getTileList(
				WGS84Coord(_left_top_x, _left_top_y, 0.0),
				WGS84Coord(_right_bottom_x, _right_bottom_y, 0.0),
				zoomLevel);

			//get image
			_map_image_list = GetFileList(IMAGE_PATH,
				zoomLevel,
				tileIdList);

			//get DTM
			GetDTMFile();

			if (_map_image_list.size() == 0 || _map_image_list[0].size() == 0)
			{
				return;
			}

			int pixX = _map_image_list.size() * DTM_PNG_PIX;
			int pixY = _map_image_list[0].size() * DTM_PNG_PIX;

			if (pixX > _map_client_rect.width())
			{
				_pix_maxx = _map_client_rect.width();
				_right_bottom_x = _left_top_x + (double)_map_client_rect.width() / _pix_x_scale;
			}
			else
			{
				_pix_maxx = pixX;
				_right_bottom_x = _left_top_x - (double)pixX / _pix_x_scale;
			}

			if (pixY > _map_client_rect.height())
			{
				_pix_maxy = _map_client_rect.height();
				_right_bottom_y = _left_top_y - (double)_map_client_rect.y() / _pix_y_scale;
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
					if (zoomLevel < MAX_ZOOM_SCALE)
					{
						zoomLevel++;
						set_global_variable(QPoint(event->x(), event->y()), _map_tool_status);
					}
				}
				else
				{
					if (zoomLevel > MIN_ZOOM_SCALE)
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
				converWgs2Pix(_map_client_rect, *iter_pix, pixArray);
				pixRegionVec.push_back(pixArray);
			}

			update();
		}
	}
}
 
void MainWindowFrame::mouseMoveEvent(QMouseEvent *e)
{
	if (_map_tool_status != MAP_ACTION::MA_SELECTION && _map_tool_status != MA_NULL)
	{
		if (e->x() < _map_client_rect.left() ||
			e->x() > _map_client_rect.right() ||
			e->y() < _map_client_rect.top() ||
			e->y() > _map_client_rect.bottom())
		{
			this->setCursor(Qt::ArrowCursor);
		}
		else
		{
			if (this->cursor().shape() == Qt::ArrowCursor)
			{
				if (_map_tool_status == MAP_ACTION::MA_ZOOMIN)
				{
					this->setCursor(*_zoom_in_cursor);
				}
				else if (_map_tool_status == MAP_ACTION::MA_ZOOMOUT)
				{
					this->setCursor(*_zoom_out_cursor);
				}
				else
				{
					this->setCursor(Qt::ClosedHandCursor);
				}
			}
		}
	}

	WGS84Coord wgs = point2WGS84Coord(QPointF(e->x(), e->y()));
	double lon = 0.0;
	double lat = 0.0;
	dtmProcess.MetersToLonLat(wgs.x, wgs.y, lon, lat);
	if (lon > 180)
	{
		lon = 180;
	}
	else if (lon < -180)
	{
		lon = -180;
	}
	if (lat > 90)
	{
		lat = 90;
	}
	else if (lat < -90)
	{
		lat = -90;
	}
	QPalette pe;
	pe.setColor(QPalette::WindowText, Qt::blue);

	statusBarLabel->setPalette(pe);
	statusBarLabel->setText("                                                                    WGS84: lon " +
		            QString::number(lon) + ", lat " + QString::number(lat));
	statusBar()->addWidget(statusBarLabel);
}

void MainWindowFrame::reset_bottom_xy()
{
	QVector<TileID> tileIdList = getTileList(
		WGS84Coord(_left_top_x, _left_top_y, 0.0),
		WGS84Coord(_right_bottom_x, _right_bottom_y, 0.0),
		zoomLevel);

	//get image
	_map_image_list = GetFileList(IMAGE_PATH,
		zoomLevel,
		tileIdList);

	//get DTM
	GetDTMFile();

	if (_map_image_list.size() > 0)
	{
		int pixX = _map_image_list.size() * DTM_PNG_PIX;
		int pixY = _map_image_list[0].size() * DTM_PNG_PIX;

		if (pixX > _map_client_rect.width())
		{
			_pix_maxx = _map_client_rect.width();
			_right_bottom_x = _left_top_x + (double)_map_client_rect.width() / _pix_x_scale;
		}
		else
		{
			_pix_maxx = pixX;
			_right_bottom_x = _left_top_x + (double)pixX / _pix_x_scale;
		}

		if (pixY > _map_client_rect.height())
		{
			_pix_maxy = _map_client_rect.height();
			_right_bottom_y = _left_top_y - (double)_map_client_rect.height() / _pix_y_scale;
		}
		else
		{
			_pix_maxy = pixY;
			_right_bottom_y = _left_top_y - (double)pixY / _pix_y_scale;
		}
	}
	else
	{
		_pix_maxx = _map_client_rect.width();
		_right_bottom_x = _left_top_x + (double)_map_client_rect.width() / _pix_x_scale;

		_pix_maxy = _map_client_rect.height();
		_right_bottom_y = _left_top_y - (double)_map_client_rect.height() / _pix_y_scale;
	}
}

void MainWindowFrame::set_global_variable(const QPointF& point, const MAP_ACTION map_action)
{
	if (min_pix_scale == 0)
	{
		return;
	}

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

	_left_top_x = wgsptBefore.x - ((double)(point.x() - _map_client_rect.left()) / _pix_x_scale);
	_left_top_y = wgsptBefore.y + ((double)(point.y() - _map_client_rect.top()) / _pix_y_scale);

	double lon = 0.0;
	double lat = 0.0;
	dtmProcess.MetersToLonLat(_left_top_x, _left_top_y, lon, lat);

	_right_bottom_x = _left_top_x + (double)(_map_client_rect.width()) / _pix_x_scale;
	_right_bottom_y = _left_top_y - (double)(_map_client_rect.height()) / _pix_y_scale;

	if (_right_bottom_x > _lon_max)
	{
		_right_bottom_x = _lon_max;
	}

	if (_right_bottom_y < _lat_min)
	{
		_right_bottom_y = _lat_min;
	}

	reset_bottom_xy();
}

WGS84Coord MainWindowFrame::point2WGS84Coord(const QPointF& point)
{
	WGS84Coord wgspt;
	if (min_pix_scale != 0)
	{
		wgspt.x = _left_top_x + (double)(point.x() - _map_client_rect.x())/ _pix_x_scale;
		wgspt.y = _left_top_y - (double)(point.y() - _map_client_rect.y())/ _pix_y_scale;
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

//	QVector< QVector<QImageDTM> >::iterator iter;
//	QVector< QImageDTM >::iterator im;

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

	qDTM.pixX = (int)((qDTM.metersX - _left_top_x) * _pix_x_scale + 0.5) + _map_client_rect.left();
	qDTM.pixY = (int)((_left_top_y - qDTM.metersY) * _pix_y_scale + 0.5) + _map_client_rect.top();
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
			if (qImageDTM[i].ty > qImageDTM[j].ty)
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

void MainWindowFrame::GetDTMFile()
{
	if (!_b_flag_show_dtm)
	{
		return;
	}

	QVector<TileID> tileIdList = getTileList(
		WGS84Coord(_left_top_x, _left_top_y, 0.0),
		WGS84Coord(_right_bottom_x, _right_bottom_y, 0.0),
		zoomLevel);

	_map_dtm_list = GetFileList(
		                  DTM_PATH,
		                  zoomLevel,
		                  tileIdList);
}

void MainWindowFrame::DrawDTM()
{
	if (!_b_flag_show_dtm)
	{
		return;
	}

	QPainter painter(this);

	QVector< QVector<QImageDTM> >::iterator iter;
	QVector< QImageDTM >::iterator im;

	for (iter = _map_dtm_list.begin(); iter != _map_dtm_list.end(); iter++)
	{
		for (im = iter->begin(); im != iter->end(); im++)
		{
			//平滑
			QImage qtm;
			qtm.load(im->imageDir);

			qtm = qtm.scaled(qtm.width(), qtm.height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
			painter.drawPixmap(im->pixX , im->pixY, qtm.width(), qtm.height(), QPixmap::fromImage(qtm));
		}
	}
}

void MainWindowFrame::DrawImage()
{
	if (!_b_flag_show_image)
	{
		return;
	}

	QPainter painter(this);

	QVector< QVector<QImageDTM> >::iterator iter;
	QVector< QImageDTM >::iterator im;

	for (iter = _map_image_list.begin(); iter != _map_image_list.end(); iter++)
	{
		for (im = iter->begin(); im != iter->end(); im++)
		{
			//平滑
			QImage qtm;
			qtm.load(im->imageDir);

			qtm = qtm.scaled(qtm.width(), qtm.height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
			painter.drawPixmap(im->pixX, im->pixY, qtm.width(), qtm.height(), QPixmap::fromImage(qtm));
		}
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

	int size = qImageDTMList.size();
	int subSize = qImageDTMList[size - 1].size();

	double endbd[4];
	dtmProcess.TileMetersBound(qImageDTMList[size - 1][subSize - 1].tx,
		                       qImageDTMList[size - 1][subSize - 1].ty,
							   zoom,
							   endbd);


	left_top_x = startbd[0];
	left_top_y = endbd[3];
	right_bottom_x = endbd[2];
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
	double tmp_pixx = (double)(x - _left_top_x) * _pix_x_scale + _map_client_rect.left();
	double tmp_pixy = (double)(_left_top_y - y) * _pix_y_scale + _map_client_rect.top();
	return QPoint((int)tmp_pixx, (int)tmp_pixy);
}

void MainWindowFrame::DrawGrid()
{
	if (!_b_flag_show_grad)
	{
		return;
	}
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
	painter.setPen(_pen_grad);
	double lon_left_top = 0.0;
	double lat_left_top = 0.0;
	dtmProcess.MetersToLonLat(_left_top_x, _left_top_y, lon_left_top, lat_left_top);

	int leftx = (lon_left_top / scale + 1) * scale;
	int lefty = (lat_left_top / scale + 1) * scale;

	double max_x = _left_top_x + _map_client_rect.width() / _pix_x_scale;
	double min_y = _left_top_y - _map_client_rect.height() / _pix_x_scale;

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

		painter.drawText(QPoint(fPoint.x(), rt.y() + _map_client_rect.y()), QString::number(leftx));
		painter.drawText(ePoint, QString::number(leftx));

		leftx += scale;
		double ty = 0.0;
		dtmProcess.LonLatToMeters(leftx, _left_top_y, mx, ty);
	}
	
	while (my >= min_y)
	{
		QPoint fPoint = converMeters2Pix(_left_top_x, my);
		fPoint = QPoint(_map_client_rect.left(), fPoint.y());
		QPoint ePoint = QPoint(_map_client_rect.right(), fPoint.y());
		painter.drawLine(fPoint, ePoint);

		painter.drawText(fPoint, QString::number(lefty));
		painter.drawText(QPoint(ePoint.x() - 15, ePoint.y()), QString::number(lefty));

		lefty -= scale;
		dtmProcess.LonLatToMeters(_left_top_x, lefty, mx, my);
	}
}

void MainWindowFrame::DrawScale()
{
	if (!_b_flag_show_scale)
	{
		return;
	}

	QPainter painter(this);
	painter.setPen(_pen_scale);

	QPoint fPoint = QPoint(_map_client_rect.x() + SCALE_START_X, _map_client_rect.bottom() - SCALE_START_Y);
	QPoint ePoint = QPoint(_map_client_rect.x() + SCALE_START_X + SCALE_LENGTH, 
	                    	_map_client_rect.bottom() - SCALE_START_Y);


	painter.drawLine(QPoint(fPoint.x(), fPoint.y() - 10), fPoint);
	painter.drawLine(fPoint, ePoint);
	painter.drawLine(ePoint, QPoint(ePoint.x(), ePoint.y() - 10));
	painter.drawLine(QPoint(fPoint.x() + SCALE_LENGTH / 2, fPoint.y()),
		             QPoint(fPoint.x() + SCALE_LENGTH / 2, fPoint.y() -5));


	WGS84Coord start = point2WGS84Coord(fPoint);
	WGS84Coord end   = point2WGS84Coord(ePoint);
	double length = sqrt((start.x - end.x)*(start.x - end.x) +
		                 (start.x - end.x)*(start.x - end.x));

	QString strText;
	if (length > 1000)
	{
		if (length > 10000)
		{
			strText = QString::number((int)length / 1000);
		}
		else
		{
			int tmp = length / 100.0;
			strText = QString::number((double)tmp / 10.0);
		}

		strText += " km";
	}
	else
	{
		strText = QString::number((int)length);
		strText += " m";
	}
	painter.drawText(QPoint(ePoint.x() + 5, ePoint.y()), strText);

}

void MainWindowFrame::DrawEagleEye()
{
	if (!_b_flag_show_eagle)
	{
		return;
	}
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
