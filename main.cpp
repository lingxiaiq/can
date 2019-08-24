#include <QtGui/QApplication>
#include "mainwindow.h"
#include "myinputpanelcontext.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MyInputPanelContext *ic = new MyInputPanelContext;
    a.setInputContext(ic);//将输入上下文与应用程序关联

    MainWindow w;
    w.setWindowFlags(w.windowFlags()& ~Qt::WindowMaximizeButtonHint& ~Qt::WindowMinimizeButtonHint);
    w.showMaximized();
    w.show();

   return a.exec();
}
