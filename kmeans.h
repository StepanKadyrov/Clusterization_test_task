#ifndef KMEANS_H
#define KMEANS_H

#include <QVector>
#include <QRandomGenerator>

// Структура для хранения точек данных
struct DataClust{
    QVector<double> data;
    int cluster = -1; // Номер кластера
};

class kmeans
{
public:
    kmeans(int k, int maxIterations);
    void fit(QVector<QVector<double>> data);
    QVector<DataClust> getCentroids() const;
    QVector <int> getLabels();
private:
    QVector<DataClust> dataClustVector;
    int k;  // Количество кластеров
    int maxIterations; // Максимальное число итераций
    QVector<DataClust> centroids; // Центроиды кластеров

    void initializeCentroids();
    int findClosestCluster(const DataClust& point) const;
    void updateCentroids();
};

#endif // KMEANS_H
