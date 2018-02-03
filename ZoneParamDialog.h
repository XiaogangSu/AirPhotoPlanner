#ifndef ZONEPARAMDLG_H
#define ZONEPARAMDLG_H

#include <QDialog>

namespace Ui {
	class ZoneParamDialog;
}

class ZoneParamDialog : public QDialog
{
    Q_OBJECT

public:
	explicit ZoneParamDialog(QWidget *parent = 0);
	~ZoneParamDialog();
	void setZoneParam(QString zone_name, double heigh);

	double get_zone_hight();
private slots:
    void on_pushButtonOK_clicked();

    void on_pushButtonCancle_clicked();

private:
	Ui::ZoneParamDialog *ui;

	double _zone_hight;
};

#endif // ZONEPARAMDLG_H
