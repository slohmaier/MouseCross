#include <QApplication>
#include "MouseCrossApp.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    QCoreApplication::setOrganizationName("MouseCross");
    QCoreApplication::setApplicationName("MouseCross");
    QCoreApplication::setApplicationVersion("1.0.0");
    
    MouseCrossApp mouseCrossApp;
    
    if (!mouseCrossApp.init()) {
        return 1;
    }
    
    return app.exec();
}