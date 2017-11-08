#include "mainwindowframe.h"
#include <QApplication>

int main(int argc, char *argv[])
{
 /*   QApplication a(argc, argv);
    MainWindow w;
    w.show();
	return a.exec();
*/
    Q_INIT_RESOURCE(tools);
    QApplication app(argc, argv);
    app.setApplicationName("SDI Example");
    app.setOrganizationName("QtProject");
    MainWindowFrame *mainWin = new MainWindowFrame;
    mainWin->show();
    return app.exec(); 
}
