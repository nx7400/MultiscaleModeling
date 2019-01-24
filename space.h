#ifndef SPACE_H
#define SPACE_H

#include <QWidget>
#include <QColor>
#include <QImage>
#include <map>
#include <functional>

#include "seed.h"
#include "filemanager.h"


class Space : public QWidget
{
    Q_OBJECT
public:
    explicit Space(QWidget *parent = nullptr);

    QImage image;

    void setTabWidth(int w);
    void setTabHeight(int h);

    void setNumberOfGenerations(int n);
    void setNumberOfSeed(int n);
    void setNeighborhoodType(int n);
    void setBCType(int n);
    void setSeedRadius(int n);
    void setNumberOfHoles(int n);

    int getTabWidth();
    int getTabHeight();
    int getNubmerOfGenerations();
    int getNumberOfSeed();
    int getNeighborhoodType();
    int getBCType();
    int getSeedRadius();
    int getNumberOfHoles();
    FileManager *getFileManager();

    void randomSeed();
    void regularSeed();
    void gradientSeed();
    void randomWithRadiusSeed();
    void holesOnSeedBorder();
    void randomHoles();
    void colorBoundaries();
    void clearSpaceBetweenBoundaries();

    void saveToFile();
    void setFileType(int n);
    void loadFromFile();

    int getProbabilityThreshold() const;
    void setProbabilityThreshold(int value);

protected:
    void paintEvent(QPaintEvent *);
    //void mousePressEvent(QMouseEvent *e);
    //void mouseMoveEvent(QMouseEvent *e);

private:
    int tabHeight;
    int tabWidth;

    int numberOfGenerations;
    int counterOfGenerations; // use this counter to detect end of simulation

    int numberOfSeed; // TODO create vector when this value is set and use this vector to replace tmpTab in many function
    int counterForColor;
    int tempCounter;
    int neighborhoodType;
    int tempRand;
    int BCType;
    int seedRadius;
    int numberOfHoles;
    int maxSeedId;
    int probabilityThreshold = 10;

    Seed** previousTab;
    Seed** currentTab;

    int itSeedMoore(int i,int j, int threshold = 0);
    int itSeedFurtherMoore(int i, int j, int threshold = 0);
    int itSeedVonNeuman(int i, int j, int threshold = 0);
    int itSeedHexLeft(int i, int j);
    int itSeedHexRight(int i, int j);
    int extensionOfMoore(int i, int j);

    int itSeedMoore2(int i,int j, int threshold = 0);
    int itSeedFurtherMoore2(int i, int j, int threshold = 0);
    int itSeedVonNeuman2(int i, int j, int threshold = 0);
    int itSeedHexLeft2(int i, int j);
    int itSeedHexRight2(int i, int j);
    int extensionOfMoore2(int i, int j);

    QTimer* timer;

    FileManager* fileManager;
    std::map<int, QColor> seedIdToColorMap;
    std::map<QColor, int, std::function<bool(const QColor&, const QColor&)>> colorToSeedIdMap;

    std::function<int(int, int)> neighborhoodTypeCallback;

signals:

public slots:
    void start();
    void stop();
    void clear();

private slots:
    void paintTab(QPainter &p);
    void paintGrid(QPainter &p);
    void nextGeneration();
};

#endif // SPACE_H
