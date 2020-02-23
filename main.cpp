#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow window;
    window.setWindowTitle("Image Viewer");
    window.show();
    // to avoid stack smashing error on exit
    int retvalue = a.exec();
    return  retvalue;
}
