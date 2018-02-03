#include "ZoneParamDialog.h"
#include "ui_zone_param.h"

ZoneParamDialog::ZoneParamDialog(QWidget *parent) :
    QDialog(parent),
	ui(new Ui::ZoneParamDialog)
{
	_zone_hight = 0.0;
    ui->setupUi(this);
}

ZoneParamDialog::~ZoneParamDialog()
{
    delete ui;
}

void ZoneParamDialog::on_pushButtonOK_clicked()
{
	_zone_hight = ui->editZoneHeight->text().toDouble();
	this->hide();
}

void ZoneParamDialog::on_pushButtonCancle_clicked()
{
	this->hide();
}

void ZoneParamDialog::setZoneParam(QString zone_name, double height)
{
	ui->editZoneName->setText(zone_name);
	if (height > -9999.0)
	{
		ui->editZoneHeight->setText(QString::number(height, 10, 4));
	}
	else
	{
		ui->editZoneHeight->setText(QString::number(_zone_hight, 10, 4));
	}
	update();
}

double ZoneParamDialog::get_zone_hight()
{
	return _zone_hight;
}
