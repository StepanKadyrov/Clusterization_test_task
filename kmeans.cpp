#include "kmeans.h"
#include <QtDebug>

// Конструктор
kmeans::kmeans(int k, int maxIterations)
    : k(k), maxIterations(maxIterations) {}

// Метод кластеризации
void kmeans::fit(QVector<QVector<double>> data)
{
    //Подготовка данных
    dataClustVector.clear();
    for (const auto& row : data) {
        dataClustVector.append({row});
    }
    initializeCentroids();

    QVector<int> labels(data.size(), -1); // Метки кластеров
    bool changed;
    for (int iter = 0; iter < maxIterations; ++iter) {
        changed = false;
        // Назначение точек кластерам
        for (int i = 0; i < data.size(); i++) {
            int newCluster = findClosestCluster(dataClustVector[i]);
            if (labels[i] != newCluster) {
                labels[i] = newCluster;
                dataClustVector[i].cluster = newCluster;
                changed = true;
            }
        }

        if (!changed) break; // Если нет изменений — выход

        updateCentroids();
    }

    // Записываем кластеры в точки данных
    for (int i = 0; i < data.size(); i++) {
        dataClustVector[i].cluster = labels[i];
    }
}

// Инициализация центроидов случайными точками
void kmeans::initializeCentroids()
{
    centroids.clear();
    for (int i = 0; i < k; i++) {
        centroids.append(DataClust{dataClustVector[QRandomGenerator::global()->bounded(dataClustVector.size())].data, i});
    }
}

// Поиск ближайшего кластера
int kmeans::findClosestCluster(const DataClust& point) const
{
    double minDist = std::numeric_limits<double>::max();
    int bestCluster = -1;
    for (int i = 0; i < k; i++) {
        double dist = 0.0;
        for (int f = 0; f < point.data.size(); f++) {
            double diff = point.data[f] - centroids[i].data[f];
            dist += diff * diff;
        }
        if (dist < minDist) {
            minDist = dist;
            bestCluster = i;
        }
    }

    return bestCluster;
}

// Обновление центроидов
void kmeans::updateCentroids()
{
    QVector<DataClust> newCentroids(k, DataClust{QVector(dataClustVector[0].data.size(), 0.0)});
    QVector<int> counts(k, 0);

    for (int i = 0; i < dataClustVector.size(); i++) {
        int cluster = dataClustVector[i].cluster;
        for (int f = 0; f < dataClustVector[i].data.size(); f++) {
            newCentroids[cluster].data[f] += dataClustVector[i].data[f];
        }
        counts[cluster]++;
    }

    // Усреднение координат центроидов
    for (int i = 0; i < k; i++) {
        if (counts[i] > 0) {
            for (int f = 0; f < newCentroids[i].data.size(); f++) {
                newCentroids[i].data[f] /= counts[i];
            }
        }
    }

    centroids = newCentroids;
}

// Получение центроидов
QVector<DataClust> kmeans::getCentroids() const
{
    return centroids;
}

QVector<int> kmeans::getLabels()
{
    QVector<int> labelsVector;
    for(int i = 0; i < dataClustVector.size(); i++){
        labelsVector.append(dataClustVector[i].cluster);
    }
    return labelsVector;
}
