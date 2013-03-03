#include <QApplication>
#include "forms/mainwindow.h"

#include "../../../../src/http.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

	// configure http
	Http::setGlobalUserAgent("PluginTester");

    MainWindow w;
    w.show();
    return a.exec();
}
