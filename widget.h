#ifndef WIDGET_H
#define WIDGET_H

#include <qtablewidget.h>
#include <QWidget>
#include <QString>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include "myglwidget.h"
#include "triad.h"


namespace Ui {
class Widget;
}

class MyGLWidget;

class TRIAD;

class Widget : public QWidget
{
    Q_OBJECT

public:
    QTableWidgetItem *itemss;

    explicit Widget(QWidget *parent = 0);
    ~Widget();
    void drawRedGraph(double value, int graph);
    void drawBlueGraph(double value, int graph);
    void drawGreenGraph(double value, int graph);
    void drawClearGraph(double value, int graph);
    double analyzeString(QString receivedData);
    void analyzeStringRowColumn(QString receivedData, int32_t rowCol[]);
    void fillTable();

signals:
    void ackReceived();
    void getAnglesRM();
    void bdotAckReveived();

private slots:
    void updateReceivedData();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_horizontalSlider_2_valueChanged(int value);
    void on_pushButton_6_clicked();
    void tabChanged();
    void on_pushButton_7_clicked();
    void on_horizontalSlider_valueChanged(int value);
    void on_horizontalSlider_3_valueChanged(int value);
    void on_spinBox_4_valueChanged(int arg1);
    void on_spinBox_3_valueChanged(int arg1);
    void on_spinBox_2_valueChanged(int arg1);
    void on_pushButton_4_clicked();
    void on_horizontalSlider_6_valueChanged(int value);
    void on_horizontalSlider_5_valueChanged(int value);
    void on_horizontalSlider_4_valueChanged(int value);
    void on_spinBox_5_valueChanged(int arg1);
    void on_spinBox_6_valueChanged(int arg1);
    void on_spinBox_7_valueChanged(int arg1);
    void on_pushButton_8_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_9_clicked();

private:
    Ui::Widget *ui;
    QSerialPort *serialPort;
    QIODevice *IoDevice;
    MyGLWidget *mijnGlptr;
    TRIAD* triadPtr;

    int ackPackReceived;
    int bdotAck;
    int previousTab;

};

#endif // WIDGET_H
