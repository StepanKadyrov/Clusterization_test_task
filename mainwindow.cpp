#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    this->setWindowTitle("Тестовое задание");
    this->setMinimumSize(1300, 650);
    this->setStyleSheet("*{font-family: Сondensed; font-size: 14pt; font-weight: Normal;}");

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
    connect(featureComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(plotData()));

    //Параметры графика
    nx = 100, ny = 100; // по умолчанию параметры задаём.
    QLabel *graphicParamLabel = new QLabel("Параметры графика");
    QLabel *nxLabel = new QLabel("Количество ячеек по x");
    QLabel *nyLabel = new QLabel("Количество ячеек по y");

    nxSB = new QSpinBox();
    nxSB->setMinimum(1);
    nxSB->setMaximum(INT_MAX);
    nxSB->setValue(nx);

    nySB = new QSpinBox();
    nySB->setMinimum(1);
    nySB->setMaximum(INT_MAX);
    nySB->setValue(ny);

    QComboBox *gradientSP = new QComboBox();

    QPushButton *replotButton = new QPushButton("Перестроить график");
    connect(replotButton, SIGNAL(clicked()), this, SLOT(plotData()));

    //Параметры кластеризации
    QLabel *clustLabel = new QLabel("Параметры кластеризации");
    QLabel *numClustLabel =  new QLabel("Количество кластеров");
    QLabel *numIterLabel = new QLabel("Максимальное количество итераций");
//    QLabel *metricLabel = new QLabel("Метрика расстояния");
    numClustSB = new QSpinBox();
    numClustSB->setMinimum(1);

    maxIterClustSB = new QSpinBox();
    maxIterClustSB->setMinimum(1);
    maxIterClustSB->setMaximum(INT_MAX);

    //Признаки кластеризации
    featureClustComboBox = new QComboBox();


    QPushButton *clustButton = new QPushButton("Запустить кластеризацию");
    connect(clustButton, SIGNAL(clicked()), this, SLOT(runClustering()));

    QGridLayout *graphicParamLayout = new QGridLayout();
    graphicParamLayout->addWidget(clustLabel, 0, 2, 1, 2); // Почему не расстягивается
    graphicParamLayout->addWidget(graphicParamLabel, 0, 0, 1, 2);

    graphicParamLayout->addWidget(nxLabel, 1, 0);
    graphicParamLayout->addWidget(nxSB, 1, 1);
    graphicParamLayout->addWidget(numClustLabel, 1, 2);
    graphicParamLayout->addWidget(numClustSB, 1, 3);


    graphicParamLayout->addWidget(nyLabel, 2, 0);
    graphicParamLayout->addWidget(nySB, 2, 1);
    graphicParamLayout->addWidget(numIterLabel, 2, 2);
    graphicParamLayout->addWidget(maxIterClustSB, 2, 3);

    graphicParamLayout->addWidget(replotButton, 4, 0, 1, 2);
    graphicParamLayout->addWidget(clustButton, 4, 2, 1, 2);


    //Создаём график
    plot = new QCustomPlot(this);
    plot->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QFont axisFont = QFont("Сondensed", 12, QFont::Normal);
    plot->xAxis->setLabelFont(axisFont);
    plot->yAxis->setLabelFont(axisFont);
    plot->xAxis->setTickLabelFont(axisFont);
    plot->yAxis->setTickLabelFont(axisFont);

    plot->xAxis->setLabel("Долгота");
    plot->yAxis->setLabel("Широта");


    QVBoxLayout *topLayout = new QVBoxLayout();
    topLayout->setAlignment(Qt::AlignCenter);
    topLayout->addWidget(featureComboBox);
    topLayout->addLayout(graphicParamLayout);
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
    //Инициализация центроидов
    int numClust = numClustSB->value();
    int maxIter = maxIterClustSB->value();;
    if (data.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Отсутствуют данные!");
        return;
    }
    // Подготовка данных для кластеризации
    QVector<QVector<double>> dataClust;
    for(int i = 0; i < data.size(); i++){
        //Добавляем признаки для кластеризации
        QVector<double> point = {data[i].latitude, data[i].longitude};
        point.append(data[i].values);
        dataClust.append(point);

    }
    kmeans kmeans(numClust, maxIter);
    kmeans.fit(dataClust); // Кластеризация
    QVector<int> labels = kmeans.getLabels();
    plotClusters(labels);
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
            //В соответствии с данными меняем параметры
            QVector<double> x, y;
            for (const auto &point : data) {
                x.append(point.longitude);
                y.append(point.latitude);
            }
            boundary[0] = *std::min_element(x.begin(), x.end());
            boundary[1] = *std::max_element(x.begin(), x.end());
            boundary[2] = *std::min_element(y.begin(), y.end());
            boundary[3] = *std::max_element(y.begin(), y.end());

            fillComboBox(); // Обновляем комбо-бокс после загрузки данных
        }
        else
        {
            QMessageBox::critical(this, "Ошибка загрузки", "Не удалось открыть файл");
        }
    }

}


void MainWindow::plotData()
{
    int numGraphic = featureComboBox->currentIndex();
    if (numGraphic == -1) return; // Обрабатываем очистку featureComboBox при загрузке новых данных
    plot->clearPlottables();

    //По поводу затирания данных тут подумать. Не надо ли принудительно удалять
//    if(colorMap != nullptr){
//        qDebug() << "плохо";
//        delete colorMap;
//    }

    colorMap = new QCPColorMap(plot->xAxis, plot->yAxis);//Надо ли каждый раз пересоздавать, можно ли просто очистить и на ней же построить

    nx = nxSB->value();
    ny = nySB->value();
    colorMap->data()->setSize(nx, ny);

    colorMap->data()->setRange(QCPRange(boundary[0], boundary[1]),
                               QCPRange(boundary[2], boundary[3]));

    for (int ix = 0; ix < nx; ++ix) {
        for (int iy = 0; iy < ny; ++iy) {
            colorMap->data()->setCell(ix, iy, qQNaN());
        }
    }

    int ix, iy;
    for(const auto &point : data)
    {
        colorMap->data()->coordToCell(point.longitude, point.latitude, &ix, &iy);
        colorMap->data()->setCell(ix, iy, point.values[numGraphic]);
    }

    if (plot->plotLayout()->elementCount() > 1) {
        plot->plotLayout()->removeAt(1); // Удаляем старую цветовую шкалу
    }

    QCPColorScale *colorScale = new QCPColorScale(plot);
    plot->plotLayout()->addElement(0, 1, colorScale);
    colorMap->setColorScale(colorScale);
    colorScale->setType(QCPAxis::atRight);
    colorScale->axis()->setLabel(QString("Признак %1").arg(numGraphic + 1));
    colorMap->rescaleDataRange(true);
    colorMap->setInterpolate(true);
    QCPColorGradient gradient = QCPColorGradient::gpJet; // Базовый градиент
    gradient.setNanHandling(QCPColorGradient::nhTransparent);
    colorMap->setGradient(gradient);
    plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    plot->xAxis->setScaleRatio(plot->yAxis);
    plot->rescaleAxes();
    // Выравниваем масштаб осей. Тут пока непонятно, подумать
    double xRange = boundary[1] - boundary[0];
    double yRange = boundary[3] - boundary[2];
    double maxRange = std::max(xRange, yRange);
    double xMid = (boundary[0] + boundary[1]) / 2.0;
    double yMid = (boundary[2] + boundary[3]) / 2.0;

    plot->xAxis->setRange(xMid - maxRange / 2.0, xMid + maxRange / 2.0);
    plot->yAxis->setRange(yMid - maxRange / 2.0, yMid + maxRange / 2.0);
    plot->replot();

}

//При удалении QComboBox выдаёт сигнал об изменении текущей позиции

void MainWindow::fillComboBox()
{
    //Обновляем ComboBox, в котором выбираются графики
    int numValues = data[0].values.size();
    featureComboBox->clear(); //испускает -1, когда очищается комбо-бокс для новых данных
    for (int i = 0; i < numValues; i++) {
        featureComboBox->addItem(QString("Признак %1").arg(i + 1), i);
    }
}

void MainWindow::plotClusters(QVector<int> labels)
{
    // Цвета для кластеров
    QVector<QColor> clusterColors = {Qt::red, Qt::blue, Qt::green, Qt::magenta, Qt::cyan, Qt::yellow};

    // Очистка предыдущих кластеров
    plot->clearPlottables();

    //Создание графиков для каждого кластера
    QMap<int, QCPGraph*> clusterGraphs;
    for (int i = 0; i< data.size(); i++) {
        int cluster = labels[i];
        if (!clusterGraphs.contains(cluster)) {
            QCPGraph* graph = plot->addGraph();
            graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 6)); // Точки
            graph->setLineStyle(QCPGraph::lsNone); // Без линий
            graph->setPen(QPen(clusterColors[cluster % clusterColors.size()])); // Цвет кластера
            clusterGraphs[cluster] = graph;
        }
        clusterGraphs[cluster]->addData(data[i].longitude, data[i].latitude);
    }

    plot->replot();
}
