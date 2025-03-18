#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    this->setWindowTitle("Тестовое задание");
    this->setMinimumSize(1300, 650);
    this->setStyleSheet("*{font-family: Comic Sans MS; font-size: 14pt; font-weight: Normal;}");

    // Создаём главный виджет и делаем его центральным для окна

    QWidget *mainWid = new QWidget();
    this->setCentralWidget(mainWid);

    // Создаём меню

    QMenu *menu = new QMenu("&Меню", mainWid);
    QMenuBar *menuBar = new QMenuBar(this);
    menuBar->setStyleSheet("background:transparent");
    menu->addAction("&Загрузить файл", this, SLOT(downloadData()));
    menu->addAction("&Сохранить результат", this, SLOT(saveFile()));
    menu->addAction("&Выход", this, SLOT(close()));
    menuBar->addMenu(menu);
    this->setMenuWidget(menuBar);

    //Создание списка графиков для выбора

    featureComboBox = new QComboBox(this);
    connect(featureComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(plotData(int)));

    //Создаём график
    plot = new QCustomPlot(this);
    plot->setMinimumSize(600, 400);

    QVBoxLayout *topLayout = new QVBoxLayout();
    topLayout->setAlignment(Qt::AlignCenter);
    topLayout->addWidget(featureComboBox);
    topLayout->addWidget(plot);
    mainWid->setLayout(topLayout);

//    runClusteringButton = new QPushButton("Запустить кластеризацию", this);
//    plot = new QCustomPlot(this);
//    plot->setMinimumSize(600, 400);

//    layout->addWidget(plot, 1);
//    setCentralWidget(centralWidget);
//    resize(800, 600);

//    connect(runClusteringButton, &QPushButton::clicked, this, &MainWindow::runClustering);
//    loadData("clustering.dat");
//    plotData();
}

void MainWindow::runClustering()
{
    qDebug() << "Кластеризация запущена!";
}


MainWindow::~MainWindow()
{
}


void MainWindow::downloadData()
{
    QString filename = QFileDialog::getOpenFileName(this, "Загрузка данных", "", "*.dat *.txt");
    if(!filename.isEmpty())
    {
        QFile file(filename);
        QTextStream in(&file);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            data.clear();
            while (!in.atEnd()) {
                QString line = in.readLine();
                QStringList line_values = line.split(" ", Qt::SkipEmptyParts);

                DataPoint point;
                point.longitude = line_values[0].toDouble();
                point.latitude = line_values[1].toDouble();
                for (int i = 2; i < line_values.size(); i++) {
                    point.values.append(line_values[i].toDouble());
                }
                data.append(point);
            }

            file.close();

            fillComboBox(); // Обновляем комбо-бокс после загрузки данных
        }
        else
        {
            QMessageBox::critical(this, "Ошибка загрузки", "Не удалось открыть файл");
        }
    }

}


void MainWindow::plotData(int numberGraphic)
{
    plot->clearPlottables();
    QVector<double> x, y, z;
    for (const auto &point : data) {
        x.append(point.latitude);
        y.append(point.longitude);
        z.append(point.values[numberGraphic]);
    }

    // По поводу затирания данных тут подумать. Не надо ли принудительно удалять
    QCPColorMap *colorMap = new QCPColorMap(plot->xAxis, plot->yAxis);
    int nx = 100, ny = 100; // Тут подумать
    colorMap->data()->setSize(nx, ny);

    double minX = *std::min_element(x.begin(), x.end()); //Надо ли это каждый раз считать
    double maxX = *std::max_element(x.begin(), x.end());
    double minY = *std::min_element(y.begin(), y.end());
    double maxY = *std::max_element(y.begin(), y.end());

    colorMap->data()->setRange(QCPRange(minX, maxX),
                               QCPRange(minY, maxY));

    for (int i = 0; i < x.size(); ++i) {
        int ix = (nx - 1) * (x[i] - minX) / (maxX - minX);
        int iy = (ny - 1) * (y[i] - minY) / (maxY - minY);
        colorMap->data()->setCell(ix, iy, z[i]);
    }

    QCPColorScale *colorScale = new QCPColorScale(plot);
    plot->plotLayout()->addElement(0, 1, colorScale);
    colorMap->setColorScale(colorScale);
    colorScale->setType(QCPAxis::atRight);
    colorMap->setGradient(QCPColorGradient::gpJet);
    colorMap->rescaleDataRange();

    plot->rescaleAxes();
    plot->replot();

}


void MainWindow::fillComboBox()
{
    //Обновляем ComboBox, в котором выбираются графики
    int numValues = data[0].values.size();
    featureComboBox->clear();
    for (int i = 0; i < numValues; i++) {
        featureComboBox->addItem(QString("Признак %1").arg(i + 1), i);
    }
}

