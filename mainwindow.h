#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QFile>
#include <QTextStream>
#include <QPushButton>
#include <QMetaEnum>

#include <QDebug>
#include "kmeans.h"
#include "qcustomplot.h"

struct DataPoint {
    double longitude;
    double latitude;
    QVector<double> values;
};

struct DataEntry {
    double longitude;
    double latitude;
    QVector<double> values;
    int cluster;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    QCustomPlot *plot;
    QCPColorMap *colorMap;
    QVector<DataPoint> data;
    kmeans *kmeansPtr;
    QListWidget *featureClustList;
    int nx, ny; // размеры карты
    QSpinBox *nxSB;
    QSpinBox *nySB;
    double boundary[4]; // x_min, x_max, y_min, y_max
    QSpinBox *numClustSB;
    QSpinBox *maxIterClustSB;
    void plotClusters();
    void fillComboBox();
    QComboBox *gradientComboBox;
    QComboBox *featureComboBox;
    void setupFeatureClustList();
    QCheckBox *interpolCB;
private slots:
    void downloadData();
    void runClustering();
    void plotData();
    void delClust();
    void saveResult();
    void gradientChanged(int index);
};
#endif // MAINWINDOW_H
