#include <QApplication>
#include <QPushButton>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include "mydialog.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MyDialog *dialog = new MyDialog;
    dialog->show();

    return app.exec();
}
