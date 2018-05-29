#ifndef MYDIALOG_H
#define MYDIALOG_H

#include <QDialog>
#include <stack>
#include <queue>

class QLabel;
class QPushButton;
class QGridLayout;
class QButtonGroup;

using namespace std;

class MyDialog : public QDialog
{
    Q_OBJECT
public:
    MyDialog(QWidget *parent=0);
    QLabel *label;
    QButtonGroup *buttongroup;
    QPushButton **button;
    QGridLayout *glayout;
    QString s;
    bool isDec;
    string lastPressed;

    QString toHex(QString);
    QString toDec(QString);
    bool isOperator(string);
    bool sameOrGreaterPrecedence(string, string);
    void applyOperator(stack<string> &, stack<long> &);
    void evaluateExpression();

public slots:
    void displayClicked(int id);
};

#endif // MYDIALOG_H
