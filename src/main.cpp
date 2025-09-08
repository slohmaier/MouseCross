#include <QApplication>
#include <QSharedMemory>
#include <QLocalSocket>
#include <QMessageBox>
#include <QTranslator>
#include <QLocale>
#include "MouseCrossApp.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Prevent application from quitting when last window is closed
    QApplication::setQuitOnLastWindowClosed(false);
    
    QCoreApplication::setOrganizationName("MouseCross");
    QCoreApplication::setApplicationName("MouseCross");
    QCoreApplication::setApplicationVersion("0.1.0");
    
    // Load translations
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "mousecross_" + QLocale(locale).name().section('_', 0, 0);
        if (translator.load(":/translations/" + baseName)) {
            app.installTranslator(&translator);
            break;
        }
    }
    
    // Single instance check using QSharedMemory
    QSharedMemory sharedMemory("MouseCross-SingleInstance-Key");
    if (!sharedMemory.create(1)) {
        // Another instance is already running
        if (sharedMemory.attach()) {
            sharedMemory.detach();
        }
        return 0;
    }
    
    MouseCrossApp mouseCrossApp;
    
    if (!mouseCrossApp.init()) {
        return 1;
    }
    
    return app.exec();
}