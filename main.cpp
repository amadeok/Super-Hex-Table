#include "mainwindow.h"
#include "table.h"
#include <QApplication>
#include <QSpinBox>
#include <QAction>
#include <QFile>
int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

    MainWindow w;

    // set stylesheet
    if (w.settings_win->enable_dark_theme)
    {
        QFile file(":/dark/stylesheet.qss");
        file.open(QFile::ReadOnly | QFile::Text);
        QTextStream stream(&file);
        a.setStyleSheet(stream.readAll());
    }

    w.show();

    return a.exec();

}
