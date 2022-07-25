#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

#include <QDialog>
#include <QDialog>
#include <QTime>
#include <QTimer>
#include <QLCDNumber>
#include <wiringPiI2C.h>
#include <wiringPi.h>
#include <QDebug>

#define MAXTIMINGS	85
#define MAXSPEED  1000

#define DHTPIN		7
#define DIRECTION	2
#define STEP		0
#define REL1		25
#define REL2		24



QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = nullptr);
    ~Dialog();

public slots:
    void ReadTimeSlot();
    void motor();
    void senzor();
    void poredi();
private slots:


    void on_checkBox_clicked(bool checked);

    void on_checkBox_2_clicked(bool checked);

    void on_checkBox_3_clicked(bool checked);

    void on_checkBox_6_clicked(bool checked);

    void on_checkBox_4_clicked(bool checked);

    void on_checkBox_5_clicked(bool checked);

private:
    Ui::Dialog *ui;
};
#endif // DIALOG_H
