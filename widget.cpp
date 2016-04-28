/*************************************************************************************************
 * This class handles the all the aspects of the user interface itself(excl. the openGL drawing),
 * the UART communication (incl. string parsing etc)
 *
 *
 ************************************************************************************************/

#include <QFile>
#include <string.h>
#include <inttypes.h>
#include "widget.h"
#include "ui_widget.h"
#include "myglwidget.h"
#include "triad.h"

#define PI 3.14159263839

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);



/*************************************************************************************************/
// Sets up the three graphics red, blue and green for the data received through UART from the first sensor
//

        ui->redPlot->addGraph();
        ui->bluePlot->addGraph();
        ui->greenPlot->addGraph();
        ui->clearPlot->addGraph();

        ui->clearPlot->xAxis->setLabel("time");
        ui->clearPlot->yAxis->setLabel("light intensity");
        ui->clearPlot->yAxis->setRange(0, 6535);
        ui->clearPlot->xAxis->setRange(0, 100);

        ui->redPlot->xAxis->setLabel("time");
        ui->redPlot->yAxis->setLabel("light intensity");
        ui->redPlot->yAxis->setRange(0, 6535);
        ui->redPlot->xAxis->setRange(0, 100);

        ui->bluePlot->xAxis->setLabel("time");
        ui->bluePlot->yAxis->setLabel("light intensity");
        ui->bluePlot->yAxis->setRange(0,6535);
        ui->bluePlot->xAxis->setRange(0,100);

        ui->greenPlot->xAxis->setLabel("time");
        ui->greenPlot->yAxis->setLabel("light intensity");
        ui->greenPlot->yAxis->setRange(0,6535);
        ui->greenPlot->xAxis->setRange(0,100);

        /* sets up graphs for the second sensor*/

        ui->clearPlot_2->addGraph();
        ui->redPlot_2->addGraph();
        ui->bluePlot_2->addGraph();
        ui->greenPlot_2->addGraph();


        ui->clearPlot_2->xAxis->setLabel("time");
        ui->clearPlot_2->yAxis->setLabel("light intensity");
        ui->clearPlot_2->yAxis->setRange(0, 6535);
        ui->clearPlot_2->xAxis->setRange(0, 100);


        ui->redPlot_2->xAxis->setLabel("time");
        ui->redPlot_2->yAxis->setLabel("light intensity");
        ui->redPlot_2->yAxis->setRange(0, 6535);
        ui->redPlot_2->xAxis->setRange(0, 100);

        ui->bluePlot_2->xAxis->setLabel("time");
        ui->bluePlot_2->yAxis->setLabel("light intensity");
        ui->bluePlot_2->yAxis->setRange(0,6535);
        ui->bluePlot_2->xAxis->setRange(0,100);

        ui->greenPlot_2->xAxis->setLabel("time");
        ui->greenPlot_2->yAxis->setLabel("light intensity");
        ui->greenPlot_2->yAxis->setRange(0,6535);
        ui->greenPlot_2->xAxis->setRange(0,100);

/*************************************************************************************************/
//Does the necessary setup for the UART communication with the microcontroller
//

            serialPort = new QSerialPort(this);
            serialPort->setPortName("COM4");
            serialPort->setBaudRate(QSerialPort::Baud115200);

            if (!serialPort->open(QIODevice::ReadWrite))
            {
             qDebug("some error when opening port for UART\n");
            }

            connect(serialPort, SIGNAL(readyRead()), this, SLOT(updateReceivedData()));

            //to avoid unnecassary data transmition, we check which tab is active to only
            //get the data which gets displayed on the current tab
            //currentChanged is a built-in signal
            connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabChanged()));
            previousTab = ui->tabWidget->currentIndex();


/*************************************************************************************************/
// initialize widgets
//

            //set integration time
            ui->comboBox->addItem(" ");
            ui->comboBox->addItem("2,4");
            ui->comboBox->addItem("24");
            ui->comboBox->addItem("50");
            ui->comboBox->addItem("101");
            ui->comboBox->addItem("154");
            ui->comboBox->addItem("700");

            //to read registers
            ui->comboBox_2->addItem(" ");
            ui->comboBox_2->addItem("integration time");
            ui->comboBox_2->addItem("gain");

            //check currentactive tab
            tabChanged();

            ui->tableWidget_2->setColumnCount(3);
            ui->tableWidget_2->setRowCount(3);
            ui->tableWidget_2->setColumnWidth(0,75);
            ui->tableWidget_2->setColumnWidth(1,75);
            ui->tableWidget_2->setColumnWidth(2,75);
            //Si
            ui->tableWidget_3->setColumnCount(1);
            ui->tableWidget_3->setRowCount(3);
            ui->tableWidget_3->setColumnWidth(0,50);
            //So
            ui->tableWidget_4->setColumnCount(1);
            ui->tableWidget_4->setRowCount(3);
            ui->tableWidget_4->setColumnWidth(0,50);
            //Mi
            ui->tableWidget_5->setColumnCount(1);
            ui->tableWidget_5->setRowCount(3);
            ui->tableWidget_5->setColumnWidth(0,50);
            //Mo
            ui->tableWidget_6->setColumnCount(1);
            ui->tableWidget_6->setRowCount(3);
            ui->tableWidget_6->setColumnWidth(0,50);

            ui->tableWidget_8->setColumnCount(3);
            ui->tableWidget_8->setRowCount(3);

            ui->tableWidget_9->setColumnCount(3);
            ui->tableWidget_9->setRowCount(3);

            //sliders angles for testing the algorithm
            ui->horizontalSlider_6->setValue(30); //epsi
            ui->horizontalSlider_5->setValue(20); //phi
            ui->horizontalSlider_4->setValue(40); //theta
            ui->spinBox_5->setValue(30);
            ui->spinBox_6->setValue(20);
            ui->spinBox_7->setValue(40);

            //inout to chose which algorithm on first tab
            ui->comboBox_3->addItem(" ");
            ui->comboBox_3->addItem("TRIAD");
            ui->comboBox_3->addItem("Q-method");
            ui->comboBox_3->addItem("QUEST");

            triadPtr = new TRIAD;

/*************************************************************************************************/
// The Mc sends an ACK for each input data it receives. Based on the sent ack, the Qt app will keep on
// sending input values

            connect(this, SIGNAL(ackReceived()),this,SLOT(on_pushButton_4_clicked()));
            connect(this,SIGNAL(bdotAckReveived()),this,SLOT(on_pushButton_9_clicked()));


/*************************************************************************************************/
// When the grid on the first page is filled
// the signal getANngles is emitted which will calculate all three angles based on the data in this grid
            connect(this,SIGNAL(getAnglesRM()),this,SLOT(on_pushButton_5_clicked()));

            ackPackReceived = 0;
            bdotAck=0;
/*************************************************************************************************/
// default values for the input of the specified algorithm on tab1
//
            float inputVal;
            inputVal = 0.2673;
            ui->tableWidget_3->setItem(0,0,new QTableWidgetItem(QString::number(inputVal)));
            inputVal = 0.5345;
            ui->tableWidget_3->setItem(1,0,new QTableWidgetItem(QString::number(inputVal)));
            inputVal = 0.8018;
            ui->tableWidget_3->setItem(2,0,new QTableWidgetItem(QString::number(inputVal)));

            inputVal = -0.3124;
            ui->tableWidget_4->setItem(0,0,new QTableWidgetItem(QString::number(inputVal)));
            inputVal = 0.9370;
            ui->tableWidget_4->setItem(1,0,new QTableWidgetItem(QString::number(inputVal)));
            inputVal = 0.1562;
            ui->tableWidget_4->setItem(2,0,new QTableWidgetItem(QString::number(inputVal)));

            inputVal = 0.7749;
            ui->tableWidget_5->setItem(0,0,new QTableWidgetItem(QString::number(inputVal)));
            inputVal = 0.3448;
            ui->tableWidget_5->setItem(1,0,new QTableWidgetItem(QString::number(inputVal)));
            inputVal = 0.5297;
            ui->tableWidget_5->setItem(2,0,new QTableWidgetItem(QString::number(inputVal)));

            inputVal = 0.6296;
            ui->tableWidget_6->setItem(0,0,new QTableWidgetItem(QString::number(inputVal)));
            inputVal = 0.6944;
            ui->tableWidget_6->setItem(1,0,new QTableWidgetItem(QString::number(inputVal)));
            inputVal = -0.3486;
            ui->tableWidget_6->setItem(2,0,new QTableWidgetItem(QString::number(inputVal)));

            //default values TLE parameters
            ui->lineEdit->setText("201.94908");
            ui->lineEdit_2->setText("14.70988142103530");
            ui->lineEdit_3->setText("158.2168");
            ui->lineEdit_4->setText("185.6639");
            ui->lineEdit_5->setText("98.0719");


            ui->lineEdit_9->setText("0.707");
            ui->lineEdit_10->setText("93");
            ui->lineEdit_11->setText("0.648648");

            ui->lineEdit_6->setText("36650");
            ui->lineEdit_7->setText("1");
            ui->lineEdit_8->setText("2");


}



/**********************************************************************************************/
// all the received data via UART gets displayed via the debug functionality and gets analyzed
//


void Widget::updateReceivedData()
{
    if(serialPort->canReadLine()) //as long as no "\n" is encountered returns is false
      {
         QByteArray result =  serialPort-> readLine(); //reads entire line
         QString command(result); //to convert byte array to string
         qDebug()<<result;

          analyzeString(command);
       }
}



/**********************************************************************************************/
// Analyzes the received string to know which row or column was sent
//


void Widget::analyzeStringRowColumn(QString receivedData, int32_t rowCol[2])
{

    QString rij, kol;

    if(receivedData.contains("m:"))
    {
        receivedData.remove("m:");
    }

    rij = receivedData.at(0);
    if(rij == " ")
    {
        rij = receivedData.at(1);
    }
    rowCol[0] = rij.toInt();

    kol = receivedData.at(2);
    if(kol == " ")
    {
        kol = receivedData.at(1);
    }
    rowCol[1] = kol.toInt();
}



/**********************************************************************************************/
// Analyzes the received string to know whether it s sensors data, RM data,etc
//


double Widget::analyzeString(QString receivedData)
{
    int iReceivedData;
    int index = ui->tabWidget->currentIndex();


    if(receivedData.contains("ack"))//acknowledged input data
    {
        ackPackReceived++;
        //we now know the amount of received packages

        if(ackPackReceived<12)
        {emit(ackReceived());} //if ack received from mc => new data can be sent to mc

        if(ackPackReceived>=12)
        {ackPackReceived = 0;}
    }
    if(receivedData.contains("bdotA"))
    {
        bdotAck++;
        emit(bdotAckReveived());
        qDebug("qt received Ack\n");
    }
    if(receivedData.contains("bdot current"))
    {

        //ui->label_35->setText("current calculated");
        receivedData = receivedData.remove("bdot current = ");
        receivedData = receivedData + "mA";
        ui->label_35->setText(receivedData);
        qDebug("final result bdot\n");
        qDebug()<<receivedData;

    }
    if(receivedData.contains("dutycycle:"))
    {
        float value=0;
        receivedData = receivedData.remove("dutycycle:");
        value = receivedData.toFloat();
        qDebug("dutycycle:\n");
        qDebug()<<value<<endl;
        //value = QString::number(receivedData);
        receivedData = QString::number(value/100000);
        receivedData += "%";
        qDebug()<<receivedData<<endl;
        ui->label_37->setText(receivedData);
    }

     if(receivedData.contains("m:")) //rotation matrix
      {
         QString rotationMatrix;
         float receivedRM;
         int32_t rowCol[2]={0};

          receivedData = receivedData.remove("m: ");
          analyzeStringRowColumn(receivedData, rowCol);
          receivedData = receivedData.mid(7);
          receivedRM = receivedData.toInt();
          receivedRM = receivedRM / 1000000000;
          rotationMatrix = QString::number(receivedRM);

            ui->widget->update();
            ui->widget->updateGL();


            ui->tableWidget_2->setItem(rowCol[0],rowCol[1], new QTableWidgetItem(rotationMatrix));
            if(rowCol[0]==2)
            {
                if(rowCol[1]==2)
                {
                    emit getAnglesRM();
                }
            }
      }

     //displays the content of the sensors read register
    if(receivedData.contains("l:"))
    {
       if (index==3){
       receivedData = receivedData.remove("l: ");
       ui->label_16->setText(receivedData);
       }
    }


    if(receivedData.contains("c:")) //clear data sensor
    {

        if(index==1)
        {    //active page = graph page
            receivedData = receivedData.remove(" c: ");
           if (receivedData.contains("s0"))
           {
                receivedData = receivedData.remove("s0");
                iReceivedData = receivedData.toDouble();             
                drawClearGraph(iReceivedData,0);
            }

            else if (receivedData.contains("s1"))
            {

                receivedData = receivedData.remove("s1");
                qDebug("na parsing\n");
                qDebug()<<receivedData;
                iReceivedData = receivedData.toDouble();
                drawClearGraph(iReceivedData,1);
            }
        }

    }


    if(receivedData.contains("r:")) //red data sensor
    {
        if(index==1)
        {    //active page = graph page

            receivedData = receivedData.remove(" r: ");
           if (receivedData.contains("s0"))
           {
                receivedData = receivedData.remove("s0");
                iReceivedData = receivedData.toDouble();
                drawRedGraph(iReceivedData,0);
            }

            else if (receivedData.contains("s1"))
            {
                receivedData = receivedData.remove("s1");
                iReceivedData = receivedData.toDouble();
                drawRedGraph(iReceivedData,1);
            }
        }

        else if (index == 2)//active page numerical reprez
         {
            if (receivedData.contains("s0"))
            {
                receivedData = receivedData.remove("s0");
                ui->listWidget->addItem(receivedData);

                if(ui->listWidget->count() > ui->spinBox->value())
                {
                    ui->listWidget->clear();
                }
            }

            else if (receivedData.contains("s1"))
            {
                receivedData = receivedData.remove("s1");
                ui->listWidget_2->addItem(receivedData);
                if(ui->listWidget_2->count() > ui->spinBox->value())
                {
                    ui->listWidget_2->clear();
                }
            }

            ui->listWidget->scrollToBottom();
            ui->listWidget_2->scrollToBottom();
        }
    }

    else if(receivedData.contains("g:")) //green data sensor
    {
        if(index==1){
        receivedData = receivedData.remove(" g: ");

        if (receivedData.contains("s0"))
        {
            receivedData = receivedData.remove("s0");
            iReceivedData = receivedData.toDouble();
            drawGreenGraph(iReceivedData,0);
        }

        else if (receivedData.contains("s1"))
        {
            receivedData = receivedData.remove("s1");
            iReceivedData = receivedData.toDouble();
            drawGreenGraph(iReceivedData,1);
        }
     }


        /*active page numerical reprez*/
         else if (index == 2)
          {
             if (receivedData.contains("s0"))
             {
                 receivedData = receivedData.remove("s0");
                 ui->listWidget->addItem(receivedData);
             }

             else if (receivedData.contains("s1"))
             {
                 receivedData = receivedData.remove("s1");
                 ui->listWidget_2->addItem(receivedData);

             }
             ui->listWidget->scrollToBottom();
             ui->listWidget_2->scrollToBottom();
         }
    }

    else if(receivedData.contains("b:")) //blue data sensor
    {

        if(index==1){
        receivedData = receivedData.remove( "b: ");

        if (receivedData.contains("s0"))
        {
            receivedData = receivedData.remove("s0");
            iReceivedData = receivedData.toDouble();
            drawBlueGraph(iReceivedData,0);
        }

        else if (receivedData.contains("s1"))
        {
            receivedData = receivedData.remove("s1");
            iReceivedData = receivedData.toDouble();
            drawBlueGraph(iReceivedData,1);
        }


         }

         else if (index == 2)//active page numerical reprez
          {

             if (receivedData.contains("s0"))
             {
                 receivedData = receivedData.remove("s0");
                 ui->listWidget->addItem(receivedData);
             }

             else if (receivedData.contains("s1"))
             {
                 receivedData = receivedData.remove("s1");
                 ui->listWidget_2->addItem(receivedData);
             }

             ui->listWidget->scrollToBottom();
             ui->listWidget_2->scrollToBottom();
         }
    }

    return iReceivedData;

}


/**********************************************************************************************************/
// every time sensordata is received a signal-slot mechanism is used, which ends up here.
// The drawXXXGraph will draw the received data


void Widget::drawClearGraph(double value, int graph)
{
    static int i=0;
    static QVector <double> tijd(101);
    static QVector <double> lightIntensivity(101);

    static int i2=0;
    static QVector <double> tijd2(101);
    static QVector <double> lightIntensivity2(101);


    if (graph==0)
    {
        tijd[i]=i;
        lightIntensivity[i] = value;

        i++;
        ui->clearPlot->yAxis->setRange(0, 6535);
        ui->clearPlot->graph(0)->setPen(QPen(Qt::black));
        ui->clearPlot->graph(0)->setData(tijd,lightIntensivity);
        ui->clearPlot->graph(0)->rescaleAxes();
        ui->clearPlot->replot();

        if(i==100)
        {i=0;}
    }

    else if (graph == 1)
    {
        tijd2[i2]=i2;
        lightIntensivity2[i2] = value;

        i2++;

        ui->clearPlot_2->yAxis->setRange(0, 6535);
        ui->clearPlot_2->graph(0)->setPen(QPen(Qt::black));
        ui->clearPlot_2->graph(0)->setData(tijd2,lightIntensivity2);
        ui->clearPlot_2->graph(0)->rescaleAxes();
        ui->clearPlot_2->replot();

        if(i2==100)
        {i2=0;}
    }

}


void Widget::drawRedGraph(double value, int graph)
{
    static int i=0;
    static QVector <double> tijd(101);
    static QVector <double> lightIntensivity(101);

    static int i2=0;
    static QVector <double> tijd2(101);
    static QVector <double> lightIntensivity2(101);

    if (graph==0)
    {

         tijd[i]=i;
        lightIntensivity[i] = value;
        i++;

        ui->redPlot->yAxis->setRange(0, 6535);
        ui->redPlot->graph(0)->setPen(QPen(Qt::red));
        ui->redPlot->graph(0)->setData(tijd,lightIntensivity);
        ui->redPlot->graph(0)->rescaleAxes();
        ui->redPlot->replot();

        if(i==100)
        {i=0;}
    }

    else if (graph == 1)
    {
        tijd2[i2]=i2;
        lightIntensivity2[i2] = value;

        i2++;

        ui->redPlot_2->yAxis->setRange(0, 6535);
        ui->redPlot_2->graph(0)->setPen(QPen(Qt::red));
        ui->redPlot_2->graph(0)->setData(tijd2,lightIntensivity2);
        ui->redPlot_2->graph(0)->rescaleAxes();
        ui->redPlot_2->replot();

        if(i2==100)
        {i2=0;}
    }

}


void Widget::drawBlueGraph(double value, int graph)
{
    static int i=0;
    static QVector <double> tijd(101);
    static QVector <double> lightIntensivity(101);

    static int i2=0;
    static QVector <double> tijd2(101);
    static QVector <double> lightIntensivity2(101);


    if (graph==0)
    {
        tijd[i]=i;
        lightIntensivity[i] = value;

        i++;
        ui->bluePlot->yAxis->setRange(0, 6535);
        ui->bluePlot->graph(0)->setPen(QPen(Qt::blue));
        ui->bluePlot->graph(0)->setData(tijd,lightIntensivity);
        ui->bluePlot->graph(0)->rescaleAxes();
        ui->bluePlot->replot();

        if(i==100)
        {i=0;}
    }

    else if (graph == 1)
    {
        tijd2[i2]=i2;
        lightIntensivity2[i2] = value;

        i2++;

        ui->bluePlot_2->yAxis->setRange(0, 6535);
        ui->bluePlot_2->graph(0)->setPen(QPen(Qt::blue));
        ui->bluePlot_2->graph(0)->setData(tijd2,lightIntensivity2);
        ui->bluePlot_2->graph(0)->rescaleAxes();
        ui->bluePlot_2->replot();

        if(i2==100)
        {i2=0;}
    }

}


void Widget::drawGreenGraph(double value, int graph)
{
    static int i=0;
    static QVector <double> tijd(101);
    static QVector <double> lightIntensivity(101);

    static int i2=0;
    static QVector <double> tijd2(101);
    static QVector <double> lightIntensivity2(101);


    if (graph==0)
    {
        tijd[i]=i;
        lightIntensivity[i] = value;

        i++;

        ui->greenPlot->yAxis->setRange(0, 6535);
        ui->greenPlot->graph(0)->setPen(QPen(Qt::green));
        ui->greenPlot->graph(0)->setData(tijd,lightIntensivity);
        ui->greenPlot->graph(0)->rescaleAxes();
        ui->greenPlot->replot();

        if(i==100)
        {i=0;}
     }

    else if (graph == 1)
    {

        tijd2[i2]=i2;
        lightIntensivity2[i2] = value;

        i2++;

        ui->greenPlot_2->yAxis->setRange(0, 6535);
        ui->greenPlot_2->graph(0)->setPen(QPen(Qt::green));
        ui->greenPlot_2->graph(0)->setData(tijd2,lightIntensivity2);
        ui->greenPlot_2->graph(0)->rescaleAxes();
        ui->greenPlot_2->replot();

        if(i2==100)
        {i2=0;}

    }
}


Widget::~Widget()
{
    delete ui;
}


/**********************************************************************************************************/
//to avoid unnecassary datatransmition, we check which tab is active to only get the data which gets displayed on the current tab
//we send to the mc which tab is active so it knows which data it needs to transmit


void Widget::tabChanged()
{

    int index = ui->tabWidget->currentIndex();
    int i,n=0;

    switch(index)
    {
        case(0):
            if(previousTab==1) //send twice actual tab due to "high" data transmtion otherwise packets get lost
            {
                serialPort->write("z0\n");
                for(i=0;i<1000;i++)
                {n++;}
            }
            if(previousTab==2) //send twice actual tab due to "high" data transmtion otherwise packets get lost
            {
                serialPort->write("z0\n");
                for(i=0;i<1000;i++)
                {n++;}
            }
            qDebug("0 is open - 3D");
            serialPort->write("z0\n");
            break;

        case(1):
            if(previousTab==1)
            {
                serialPort->write("z1\n");
                for(i=0;i<1000;i++)
                {n++;}
            }
            if(previousTab==2) //send twice actual tab due to "high" data transmtion otherwise packets get lost
            {
                serialPort->write("z1\n");
                for(i=0;i<1000;i++)
                {n++;}
            }
             qDebug("only raw data graphs\n");
             serialPort->write("z1\n");
             break;

        case(2):
            if(previousTab==1)
            {
                serialPort->write("z2\n");
                for(i=0;i<1000;i++)
                {n++;}
            }
            if(previousTab==2) //send twice actual tab due to "high" data transmtion otherwise packets get lost
            {
                serialPort->write("z2\n");
                for(i=0;i<1000;i++)
                {n++;}
            }
            qDebug("vectors and raw data");
            serialPort->write("z2\n");
            break;

        case(3):
           if(previousTab==1)
           {
                serialPort->write("zx\n"); //only used to read and setup registers
                for(i=0;i<1000;i++)
                {n++;}
            }
        case(4):
            if(previousTab==1)
            {
                serialPort->write("zx\n"); //only used to read and setup registers
                for(i=0;i<1000;i++)
                {n++;}
            }
             qDebug("sensor setup");
             serialPort->write("z4\n"); //only used to read and setup registers
        break;

        case(5):
        if(previousTab==1)
        {
            serialPort->write("z5\n"); //only used to read and setup registers
            for(i=0;i<1000;i++)
            {n++;}
        }
        if(previousTab==2) //send twice actual tab due to "high" data transmtion otherwise packets get lost
        {
            serialPort->write("z5\n");
            for(i=0;i<1000;i++)
            {n++;}
        }
         qDebug("bdot");
         serialPort->write("z5\n"); //only used to read and setup registers
    break;
    }

    previousTab = index;
}


void Widget::on_pushButton_clicked()
{

    if(ui->radioButton->isChecked()==true)
    {
       serialPort->write("gain: x1\n");
    }

    else if(ui->radioButton_2->isChecked()==true)
    {
        serialPort->write("gain: x4\n");
    }

    else if(ui->radioButton_3->isChecked()==true)
    {
        serialPort->write("gain: x16\n");
    }

    else if(ui->radioButton_7->isChecked()==true)
    {
        serialPort->write("gain: x60\n");
    }



    /*float angle = (ui->widget->getCubeAngle(1));
    angle += 15;
    if (angle>=360)
    {angle=0;}

    ui->widget->setCubeAngle(angle,1);
    qDebug("hoek: ");
    qDebug()<<angle<<endl;
*/
}


/********************************************************************************************/
// Write desired measurements from the sensors to file
//

void Widget::on_pushButton_2_clicked()
{
    QString itemValue;

    if(ui->radioButton_4->isChecked()==true){
    QString filename="C:/Users/yalishanda dao/Desktop/sensor1.txt";

    QFile file( filename );
    if ( file.open(QIODevice::ReadWrite) )
    {
        QTextStream stream( &file );

        for(int i = 0; i < ui->listWidget->count(); ++i)
        {
            itemValue= ui->listWidget->item(i)->text();
             stream << itemValue << endl;
        }
    }
   }

    else if(ui->radioButton_5->isChecked()==true){

        QString filename="C:/Users/yalishanda dao/Desktop/sensor2.txt";

        QFile file( filename );
        if ( file.open(QIODevice::ReadWrite) )
        {
            QTextStream stream( &file );

            for(int i = 0; i < ui->listWidget_2->count(); ++i)
            {
                itemValue= ui->listWidget_2->item(i)->text();
                 stream << itemValue << endl;
            }
        }

    }

    else if(ui->radioButton_6->isChecked()==true){
    QString filename="C:/Users/yalishanda dao/Desktop/sensor3.txt";

    QFile file( filename );
    if ( file.open(QIODevice::ReadWrite) )
    {
        QTextStream stream( &file );

        for(int i = 0; i < ui->listWidget_3->count(); ++i)
        {
            itemValue= ui->listWidget_3->item(i)->text();
             stream << itemValue << endl;
        }
    }

    }

    else
    {return;}
}


/**********************************************************************************************************/
// gets the values from sliders on the first tab, creates corresponding RM and updates the CubeSat drawing
//

void Widget::on_pushButton_3_clicked()
{
    QString interValue;
    int value;
    QString strValue;
    float theta, epsilon, phi;

    float firstRotMatrix[3][3]={0};


    value = ui->horizontalSlider->value();
    epsilon = value;
    ui->widget->setGreekAngles(1,value);


    value = ui->horizontalSlider_2->value();
    phi = value;
    ui->widget->setGreekAngles(2,value);

    value = ui->horizontalSlider_3->value();
    theta = value;
    ui->widget->setGreekAngles(3,value);

   /* theta = ui->widget->getGreekAngles(1);
    epsilon = ui->widget->getGreekAngles(2);
    phi = ui->widget->getGreekAngles(3);
*/

    phi=(phi*PI)/180;
    epsilon=(epsilon*PI)/180;
    theta=(theta*PI)/180;

    firstRotMatrix[0][0] = cos(phi) * cos(epsilon)  - cos(theta) * sin(phi) * sin(epsilon);
    firstRotMatrix[0][1] = cos(theta) * cos(epsilon) * sin(phi) + cos(phi) * sin(epsilon);
    firstRotMatrix[0][2] = sin(theta) * sin(phi);
    firstRotMatrix[1][0] = (-cos(epsilon)) * sin(phi) - cos(theta) * cos(phi) * sin(epsilon);
    firstRotMatrix[1][1] = cos(theta) * cos(phi) * cos(epsilon) - sin(phi) * sin(epsilon);
    firstRotMatrix[1][2] = cos(phi) * sin(theta);
    firstRotMatrix[2][0] = sin(theta) * sin(epsilon);
    firstRotMatrix[2][1] = (-cos(epsilon)) * sin(theta);
    firstRotMatrix[2][2] = cos(theta);

    for(int i=0;i<3;i++) //QString::number( firstRotMatrix[i][j])
   {
        for(int j=0;j<3;j++)
        {
            ui->tableWidget_2->setItem(i,j, new QTableWidgetItem(QString::number( firstRotMatrix[i][j])));
        }
   }


    ui->widget->update();
}

void Widget::on_horizontalSlider_valueChanged(int value)
{
    ui->spinBox_2->setValue(value);
}

void Widget::on_horizontalSlider_2_valueChanged(int value)
{
    ui->spinBox_3->setValue(value);
}

void Widget::on_horizontalSlider_3_valueChanged(int value)
{
    ui->spinBox_4->setValue(value);
}

/********************************************************************/
// sends the desired values to the mc to parameterize the sensor
//

void Widget::on_pushButton_6_clicked()
{
    QString value = (ui->comboBox->currentText());

    if(value=="2,4")
    {
        serialPort->write("t:2,4\n");
        qDebug("2.4sent");
    }
    else if(value=="24")
    {
        serialPort->write("t:24\n");
        qDebug("24 sent");
    }
    else if(value=="50")
    {
        serialPort->write("t:50\n");
        qDebug("50 sent");
    }

    else if(value=="101")
    {
        serialPort->write("t:101\n");
        qDebug("101 sent");
    }
    else if(value=="154")
    {
        serialPort->write("t:154\n");
        qDebug("154 sent");
    }
    else if(value=="700")
    {
        serialPort->write("t:700\n");
        qDebug("700 sent");
    }
}

/*******************************************************************/
// Checks whether the integration time or the gain have to be parameterized
//

void Widget::on_pushButton_7_clicked()
{
    QString value = ui->comboBox_2->currentText();

    if(value == "integration time")
    {
        qDebug("sent r 0x81");
        serialPort->write("r 0x81\n");
    }
    else if (value == "gain")
    {
        qDebug("sent r 0x8f");
        serialPort->write("r 0x8f\n");
    }

}

void Widget::on_spinBox_4_valueChanged(int arg1)
{
    ui->horizontalSlider_3->setValue(arg1);
}

void Widget::on_spinBox_3_valueChanged(int arg1)
{
    ui->horizontalSlider_2->setValue(arg1);
}

void Widget::on_spinBox_2_valueChanged(int arg1)
{
    ui->horizontalSlider->setValue(arg1);
}

/*******************************************************************/
// send input data for algorithm
//

void Widget::on_pushButton_4_clicked()
{
    int i=0;
    int n=0;
    QString prefix;
    QString value;
    QString strValue;
    QTableWidgetItem *theItem;
    QString algorithm;

    int k=0;

    for(i=0;i<10000;i++) //put a delay before resending otherwise strange stuff happens
    {
        n++;
    }

      if(ackPackReceived == 0 )
      {
        if (ui->comboBox_3->currentText() == "TRIAD")
        {serialPort->write("T\n");
        }
        if (ui->comboBox_3->currentText() == "QUEST")
        {serialPort->write("Q\n");
        }
        theItem = ui->tableWidget_3->item(0,0);
        prefix = "Si1:";
      }
      if(ackPackReceived == 1 )
      {
        theItem = ui->tableWidget_3->item(1,0);
        prefix = "Si2:";
      }
      if(ackPackReceived == 2 )
      {
        theItem = ui->tableWidget_3->item(2,0);
        prefix = "Si3:";
      }

      if(ackPackReceived == 3 )
      {
        theItem = ui->tableWidget_4->item(0,0);
        prefix = "So1:";
      }

      if(ackPackReceived == 4 )
      {
        theItem = ui->tableWidget_4->item(1,0);
        prefix = "So2:";
      }
      if(ackPackReceived == 5 )
      {
        theItem = ui->tableWidget_4->item(2,0);
        prefix = "So3:";
      }

      if(ackPackReceived == 6 )
      {
        theItem = ui->tableWidget_5->item(0,0);
        prefix = "Mi1:";
      }

      if(ackPackReceived == 7 )
      {
        theItem = ui->tableWidget_5->item(1,0);
        prefix = "Mi2:";
      }
      if(ackPackReceived == 8 )
      {
        theItem = ui->tableWidget_5->item(2,0);
        prefix = "Mi3:";
      }


      if(ackPackReceived == 9 )
      {
        theItem = ui->tableWidget_6->item(0,0);
        prefix = "Mo1:";
      }

      if(ackPackReceived == 10 )
      {
        theItem = ui->tableWidget_6->item(1,0);
        prefix = "Mo2:";
      }
      if(ackPackReceived == 11 )
      {
        theItem = ui->tableWidget_6->item(2,0);
        prefix = "Mo3:";
      }

 QString theText = theItem->text();
 strValue = prefix + theText + "\n";
 const char *myconstchar = strValue.toLatin1().data();
 serialPort->write(myconstchar);
 qDebug("sending:");
 qDebug()<<myconstchar;
 for(i=0;i<10000;i++) //put a delay before resending otherwise strange stuff happens
 {
     n++;
 }

}

void Widget::on_horizontalSlider_6_valueChanged(int value)
{
        ui->spinBox_5->setValue(value);
}

void Widget::on_horizontalSlider_5_valueChanged(int value)
{
        ui->spinBox_6->setValue(value);
}

void Widget::on_horizontalSlider_4_valueChanged(int value)
{
        ui->spinBox_7->setValue(value);
}

void Widget::on_spinBox_5_valueChanged(int arg1)
{
    ui->horizontalSlider_6->setValue(arg1);
}

void Widget::on_spinBox_6_valueChanged(int arg1)
{
    ui->horizontalSlider_5->setValue(arg1);
}

void Widget::on_spinBox_7_valueChanged(int arg1)
{
    ui->horizontalSlider_4->setValue(arg1);
}

/*******************************************************************************/
//check algorithm
//

void Widget::on_pushButton_8_clicked()
{
    int i=0;
    int j=0;
    int k=0;
    int l=0;

    float myEpsilon, phi, theta;

  /*  float V1i[3]={0.2673, 0.5345, 0.8018};
    float V2i[3]={-0.3124, 0.9370, 0.1562};
    float V1b[3]={0.7749, 0.3448, 0.5297};
    float V2b[3]={0.6296, 0.6944, -0.3486};
*/

    float V1i[3]={0};
    float V2i[3]={5,9,10};
    float V1b[3]={36650,1,2};
    float V2b[3]={0};

    float RM[9]={0};
    QString text;

    text = (ui->lineEdit_6->text());
    V1b[0] = text.toFloat() ;
    text = (ui->lineEdit_7->text());
    V1b[1] = text.toFloat();
    text = (ui->lineEdit_8->text());
    V1b[2] = text.toFloat();

    myEpsilon = ui->horizontalSlider_6->value();
    phi = ui->horizontalSlider_5->value();
    theta = ui->horizontalSlider_4->value();
    triadPtr->setAngleRM(1, phi);
    triadPtr->setAngleRM(3, myEpsilon);
    triadPtr->setAngleRM(2, theta);

    for(j=0;j<3;j++)
    {
         triadPtr->setupVectors(V1i, V2i, V1b, V2b,RM);
         triadPtr->calcTRIAD(V1i, V1b, V2i, V2b,RM);

        for(k=1;k<=3;k++)
        {
           for(l=1;l<=3;l++)
           {
               if(j==0){
                ui->tableWidget_8->setItem(k-1,l-1,new QTableWidgetItem(QString::number(RM[i])));
                i++;
                }
               if(j==2){
                ui->tableWidget_9->setItem(k-1,l-1,new QTableWidgetItem(QString::number(RM[i])));
                i++;
                }
           }
        }
        i=0;
    }

}


/******************************************************************/
// calculates angles out of the rotation matrix
//

void Widget::on_pushButton_5_clicked()
{
    float testTheta;
    float testEpsi;
    float testPhi;

    float value;
    QString theText;
    QTableWidgetItem *theItem;


    theItem = ui->tableWidget_2->item(2,2);
    theText = theItem->text();
    value = theText.toFloat();
    testTheta = lround((cos(value)*180.0)/3.14);
    ui->widget->setGreekAngles(3,testTheta);
    ui->horizontalSlider_3->setValue(testTheta);

    theItem = ui->tableWidget_2->item(2,1);
    theText = theItem->text();
    value = theText.toFloat();
    testEpsi = ((-(value))/(sin(testTheta)));
    testEpsi = (cos(testEpsi));
    testEpsi = (testEpsi) * (180/3.14);
    testEpsi = lround(testEpsi);
    ui->widget->setGreekAngles(1,testEpsi);
    ui->horizontalSlider->setValue(testEpsi);

    theItem = ui->tableWidget_2->item(1,2);
    theText = theItem->text();
    value = theText.toFloat();
    testPhi = ((value)/(sin(testTheta)));
    testPhi = (cos(testPhi))*180.0/3.14;
    testPhi = lround(testPhi);
    ui->widget->setGreekAngles(2,testPhi);
    ui->horizontalSlider_2->setValue(testPhi);


    ui->widget->update();
}

void Widget::on_pushButton_9_clicked()
{
    int i,n=0;
    int a;

    if(bdotAck ==0)
    {
        QString angle = ui->lineEdit_9->text();

        angle = "a: " +angle + "\n";
        const char *myConstChar =  angle.toLatin1().data();
        serialPort->write(myConstChar);
        qDebug("received ack 0\n");
    }

    if(bdotAck ==1)
    {
        QString windings = ui->lineEdit_10->text();

        windings = "w:" + windings + "\n";
        const char *myConstChar2 = windings.toLatin1().data();
        serialPort->write(myConstChar2);
        qDebug("received ack 1\n");
    }

    if(bdotAck ==2)
    {
        QString area = ui->lineEdit_11->text();

        area = "s:" + area + "\n"; //surface
        const char *myConstChar3 = area.toLatin1().data();
        serialPort->write(myConstChar3);
        qDebug("Qt sent:");
        qDebug()<<myConstChar3;
        qDebug("received ack 3\n");
        bdotAck=0;
     }



}



