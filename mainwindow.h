#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QFileDialog>
#include <QVBoxLayout>

#include <QDebug>

// Ещё не использовал
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QFile>
#include <QTextStream>

#include <algorithm>

#include "qcustomplot.h"

struct DataPoint {
    double longitude;
    double latitude;
    QVector<double> values;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    QCustomPlot *plot;
    QVector<DataPoint> data;
    QComboBox *featureComboBox;

    void fillComboBox();

private slots:
    void downloadData();
    void runClustering();
    void plotData(int numberGraphic);

};
#endif // MAINWINDOW_H
