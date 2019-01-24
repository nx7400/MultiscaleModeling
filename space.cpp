#include <QMessageBox>
#include <QPainter>
#include <QMouseEvent>
#include <QRectF>
#include <QTimer>
#include <QDebug>
#include <qmath.h>
#include <windows.h>
#include <math.h>
#include <iostream>
#include <functional>

#include "space.h"

Space::Space(QWidget *parent) :
    QWidget(parent),
    tabHeight(300),
    tabWidth(300),
    numberOfGenerations(500),
    counterOfGenerations(0),
    tempCounter(0),
    neighborhoodType(0),
    maxSeedId(100),
    fileManager(new FileManager())
{
    timer = new QTimer(this);
    timer->setInterval(100);
    connect(timer, SIGNAL(timeout()), this, SLOT(nextGeneration()));

    previousTab = new Seed*[tabWidth];
    currentTab = new Seed*[tabWidth];

    for(int i = 0; i < tabWidth; i++)
    {
        previousTab[i] = new Seed[tabHeight];
        currentTab[i]= new Seed[tabHeight];
    }

    this->image = QImage(300, 300, QImage::Format_RGB32);

    colorToSeedIdMap = std::map<QColor, int, std::function<bool(const QColor&, const QColor&)>>([](const QColor& c1, const QColor& c2)
                                                                                                { return c1.green() < c2.green(); });

    for(int seedId = 0; seedId < maxSeedId; ++seedId)
    {
        seedIdToColorMap[seedId] = QColor((seedId*20+45)%255, (seedId*12+26)%255, (seedId*112+11)%255);
        colorToSeedIdMap[QColor((seedId*20+45)%255, (seedId*12+26)%255, (seedId*112+11)%255)] = seedId;
    }

    seedIdToColorMap[1000] = QColor(Qt::black);
    colorToSeedIdMap[QColor(Qt::black)] = 1000;

    seedIdToColorMap[-1] = QColor(Qt::white);
    colorToSeedIdMap[QColor(Qt::white)] = -1;
}


void Space::setTabWidth(int w)
{
    tabWidth = w;
    this->image.setDotsPerMeterX(w);
}

void Space::setTabHeight(int h)
{
    tabHeight = h;
    this->image.setDotsPerMeterY(h);
}

int Space::getTabWidth()
{
    return tabWidth;
}

int Space::getTabHeight()
{
    return tabHeight;
}

void Space::setNumberOfGenerations(int n)
{
    numberOfGenerations = n;
}

void Space::setNumberOfSeed(int n)
{
    numberOfSeed = n;
}

int Space::getNubmerOfGenerations()
{
    return numberOfGenerations;
}

int Space::getNumberOfSeed()
{
    return numberOfSeed;
}

void Space::setNeighborhoodType(int n)
{
    neighborhoodType = n;
}

int Space::getNeighborhoodType()
{
    return neighborhoodType;
}

void Space::setBCType(int n)
{
    BCType = n;
}

int Space::getBCType(){

    return BCType;
}

void Space::setInclusionsSize(int n)
{
    inclusionsSize = n;
}

int Space::getInclusionsSize()
{
    return inclusionsSize;
}
void Space::setNumberOfHoles(int n)
{
    numberOfHoles = n;
}

int Space::getNumberOfHoles()
{
    return numberOfHoles;
}

FileManager *Space::getFileManager()
{
    return this->fileManager;
}

int Space::getProbabilityThreshold() const
{
    return probabilityThreshold;
}

void Space::setProbabilityThreshold(int value)
{
    probabilityThreshold = value;
}

void Space::setInclusionsType(int n)
{
    inclusionsType = n;
}

void Space::start()
{
    if(!timer->isActive())
    {
        this->image = QImage(this->tabWidth, this->tabHeight, QImage::Format_RGB32);
        timer->start();
    }
}

void Space::stop()
{
    if(timer->isActive())
        timer->stop();
}

void Space::clear()
{
    if(timer->isActive())
        timer->stop();

    for(int i = 0; i < tabWidth; i++)
    {
        for(int j = 0; j< tabHeight; j++)
        {
            previousTab[i][j].seedId = -1;
            currentTab[i][j].seedId = -1;

            previousTab[i][j].state = 0;
            currentTab[i][j].state = 0;

            previousTab[i][j].checked = false;
            currentTab[i][j].checked = false;

        }
    }

    update();
}

void Space::saveToFile()
{
    if(this->fileManager->getFileType() == FileType::TXT)
        this->fileManager->saveToTxtFile(this->previousTab, this->getTabWidth(), this->getTabHeight());
    if(this->fileManager->getFileType() == FileType::BITMAPP)
        this->fileManager->saveToBitMap(this->image);
}

void Space::setFileType(int n)
{
    this->fileManager->setFileType(static_cast<FileType>(n));
}

void Space::loadFromFile()
{
    if(this->fileManager->getFileType() == FileType::TXT)
        this->fileManager->loadFromTxtFile(this->previousTab,
                                           [this](int tabWidth) { this->setTabWidth(tabWidth); },
                                           [this](int tabHeight) { this->setTabHeight(tabHeight); });

    if(this->fileManager->getFileType() == FileType::BITMAPP)
        this->fileManager->loadFromBitMap(this->previousTab,
                                          [this](int tabWidth) { this->setTabWidth(tabWidth); },
                                          [this](int tabHeight) { this->setTabHeight(tabHeight); },
                                           this->colorToSeedIdMap);

    update();
}

void Space::randomSeed()
{

//    for(int i = 0; i < tabWidth; i++){
//        for(int j = 0; j< tabHeight; j++){
//            previousTab[i][j].seedId = -1;
//            currentTab[i][j].seedId = -1;

//            previousTab[i][j].state = 0;
//            currentTab[i][j].state = 0;

//            previousTab[i][j].checked = false;
//            currentTab[i][j].checked = false;

//            previousTab[i][j].energy = 0;
//            currentTab[i][j].energy = 0;

//        }
//    }

    int counter = 0;
    int tempI, tempJ;
    qsrand(qrand());

    while(counter <= numberOfSeed)
    {
        tempI = qrand()%tabWidth;
        tempJ = qrand()%tabHeight;

        if(previousTab[tempI][tempJ].state == 0)
        {
            previousTab[tempI][tempJ].state = 1;
            previousTab[tempI][tempJ].seedId = counter;
            counter++;
        }
    }

    update();
}

void Space::regularSeed()
{
    for(int i = 0; i < tabWidth; i++)
    {
        for(int j = 0; j< tabHeight; j++)
        {
            previousTab[i][j].seedId = -1;
            currentTab[i][j].seedId = -1;

            previousTab[i][j].state = 0;
            currentTab[i][j].state = 0;

            previousTab[i][j].checked = false;
            currentTab[i][j].checked = false;

        }
    }

    int counter = 1;
    int temp = static_cast<int>(sqrt(numberOfSeed));

    int t1 = tabWidth/temp;
    int t2 = tabHeight/temp;

    for(int i = t1/2; i < tabWidth; i = i + t1)
    {
        for(int j = t2/2; j < tabHeight; j = j + t2)
        {
            previousTab[i][j].state = 1;
            previousTab[i][j].seedId = counter;
            counter++;
        }
    }

    update();
}

void Space::gradientSeed()
{
    for(int i = 0; i < tabWidth; i++)
    {
        for(int j = 0; j< tabHeight; j++)
        {
            previousTab[i][j].seedId = -1;
            currentTab[i][j].seedId = -1;

            previousTab[i][j].state = 0;
            currentTab[i][j].state = 0;

            previousTab[i][j].checked = false;
            currentTab[i][j].checked = false;

        }
    }

    int counter = 0;
    int tempI, tempJ;
    qsrand(qrand());

    int tempHeight = tabHeight/3;

    int numberSeed1 = static_cast<int>(numberOfSeed*0.5);
    int numberSeed2 = static_cast<int>(numberOfSeed*0.3);
    int numberSeed3 = static_cast<int>(numberOfSeed*0.2);

    while(counter <= numberSeed1)
    {
        tempI = qrand()%tabWidth;
        tempJ = qrand()%tempHeight;

        if(previousTab[tempI][tempJ].state == 0)
        {
            if(tempJ < tempHeight)
            {
                previousTab[tempI][tempJ].state = 1;
                previousTab[tempI][tempJ].seedId = counter;
                counter++;
            }
        }
    }

    counter = 0;

    while(counter <= numberSeed2)
    {
        tempI = qrand()%tabWidth;
        tempJ = qrand()%((tempHeight*2-tempHeight) + tempHeight);

        if(previousTab[tempI][tempJ].state == 0)
        {
            if( tempHeight < tempJ < (tempHeight*2))
            {
                previousTab[tempI][tempJ].state = 1;
                previousTab[tempI][tempJ].seedId = counter;
                counter++;
            }
        }
    }

    counter = 0;

    while(counter <= numberSeed3)
    {
        tempI = qrand()%tabWidth;
        tempJ = qrand()%((tabHeight-tempHeight*2) + tempHeight*2);

        if(previousTab[tempI][tempJ].state == 0)
        {
            if( tempHeight*2 < tempJ < tabHeight)
            {
                previousTab[tempI][tempJ].state = 1;
                previousTab[tempI][tempJ].seedId = counter;
                counter++;
            }
        }
    }

    update();
}

void Space::holesOnSeedBorder()
{
    bool** borderTab;
    borderTab = new bool*[tabWidth];

    for(int i = 0; i < tabWidth; i++)
        borderTab[i] = new bool[tabHeight];

    for(int i = 0; i < tabWidth; i++)
        for(int j = 0; j< tabHeight; j++)
            borderTab[i][j] = false;

    for(int i = 1; i < tabWidth-1; i++)
    {
        for(int j = 1; j< tabHeight-1; j++)
        {
            if(previousTab[i+1][j].seedId != previousTab[i][j].seedId)
            {
                borderTab[i][j] = true;
                continue;
            }

            if(previousTab[i-1][j].seedId != previousTab[i][j].seedId)
            {
                borderTab[i][j] = true;
                continue;
            }

            if(previousTab[i][j+1].seedId!= previousTab[i][j].seedId)
            {
                borderTab[i][j] = true;
                continue;
            }

            if(previousTab[i][j-1].seedId != previousTab[i][j].seedId)
            {
                borderTab[i][j] = true;
                continue;
            }

            if(previousTab[i+1][j+1].seedId != previousTab[i][j].seedId)
            {
                borderTab[i][j] = true;
                continue;
            }

            if(previousTab[i-1][j-1].seedId != previousTab[i][j].seedId)
            {
                borderTab[i][j] = true;
                continue;
            }

            if(previousTab[i+1][j-1].seedId != previousTab[i][j].seedId)
            {
                borderTab[i][j] = true;
                continue;
            }

            if(previousTab[i-1][j+1].seedId != previousTab[i][j].seedId)
            {
                borderTab[i][j] = true;
                continue;
            }
        }
    }

    int counter = 0;
    int tempI, tempJ;

    while(counter <= numberOfHoles)
    {
        tempI = qrand()%tabWidth;
        tempJ = qrand()%tabHeight;

        if(borderTab[tempI][tempJ] == true)
        {
            for(int i = 0; i < tabWidth; i++)
            {
                for(int j = 0; j < tabHeight; j++)
                {
                    if(inclusionsType == 0)
                    {
                        if((static_cast<int>(sqrt(pow((tempI - i),2) + pow((tempJ - j),2))) <= inclusionsSize))
                        {
                            previousTab[tempI][tempJ].state = 1;
                            previousTab[tempI][tempJ].seedId = 1000;
                            previousTab[i][j].state = 1;
                            previousTab[i][j].seedId = 1000;
                        }
                    }
                    else if(inclusionsType == 1)
                    {
                        if((i >= tempI) && (i <= (tempI + inclusionsSize)) && (j >= tempJ) && (j <= (tempJ + inclusionsSize)))
                        {
                            previousTab[tempI][tempJ].state = 1;
                            previousTab[tempI][tempJ].seedId = 1000;
                            previousTab[i][j].state = 1;
                            previousTab[i][j].seedId = 1000;
                        }
                    }

                }

            }
            counter++;
        }
    }

    update();
}

void Space::randomHoles()
{
    int counter = 0;
    int tempI, tempJ;

    while(counter <= numberOfHoles)
    {
        tempI = qrand()%tabWidth;
        tempJ = qrand()%tabHeight;

        for(int i = 0; i < tabWidth; i++)
        {
            for(int j = 0; j < tabHeight; j++)
            {
                if(inclusionsType == 0)
                {
                    if((static_cast<int>(sqrt(pow((tempI - i),2) + pow((tempJ - j),2))) <= inclusionsSize))
                    {
                        previousTab[tempI][tempJ].state = 1;
                        previousTab[tempI][tempJ].seedId = 1000;
                        previousTab[i][j].state = 1;
                        previousTab[i][j].seedId = 1000;
                    }
                }
                else if(inclusionsType == 1)
                {
                    if((i >= tempI) && (i <= (tempI + inclusionsSize)) && (j >= tempJ) && (j <= (tempJ + inclusionsSize)))
                    {
                        previousTab[tempI][tempJ].state = 1;
                        previousTab[tempI][tempJ].seedId = 1000;
                        previousTab[i][j].state = 1;
                        previousTab[i][j].seedId = 1000;
                    }
                }
            }
        }

        counter++;
    }

    update();
}

void Space::colorBoundaries()
{
    bool** borderTab;
    borderTab = new bool*[tabWidth];

    for(int i = 0; i < tabWidth; i++)
        borderTab[i] = new bool[tabHeight];

    for(int i = 0; i < tabWidth; i++)
        for(int j = 0; j< tabHeight; j++)
            borderTab[i][j] = false;

    for(int i = 1; i < tabWidth-1; i++)
    {
        for(int j = 1; j< tabHeight-1; j++)
        {
            if(previousTab[i+1][j].seedId != previousTab[i][j].seedId)
            {
                borderTab[i][j] = true;
                continue;
            }

            if(previousTab[i-1][j].seedId != previousTab[i][j].seedId)
            {
                borderTab[i][j] = true;
                continue;
            }

            if(previousTab[i][j+1].seedId!= previousTab[i][j].seedId)
            {
                borderTab[i][j] = true;
                continue;
            }

            if(previousTab[i][j-1].seedId != previousTab[i][j].seedId)
            {
                borderTab[i][j] = true;
                continue;
            }

            if(previousTab[i+1][j+1].seedId != previousTab[i][j].seedId)
            {
                borderTab[i][j] = true;
                continue;
            }

            if(previousTab[i-1][j-1].seedId != previousTab[i][j].seedId)
            {
                borderTab[i][j] = true;
                continue;
            }

            if(previousTab[i+1][j-1].seedId != previousTab[i][j].seedId)
            {
                borderTab[i][j] = true;
                continue;
            }

            if(previousTab[i-1][j+1].seedId != previousTab[i][j].seedId)
            {
                borderTab[i][j] = true;
                continue;
            }
        }
    }


    for(int i = 0; i < tabWidth; i++)
    {
        for(int j = 0; j < tabHeight; j++)
        {
            if(borderTab[i][j] == true)
            {
                previousTab[i][j].state = 1;
                previousTab[i][j].seedId = 1000;

                currentTab[i][j].state = 1;
                currentTab[i][j].seedId = 1000;
             }
        }
    }

    update();

}

void Space::clearSpaceBetweenBoundaries()
{
    for(int i = 0; i < tabWidth; i++)
    {
        for(int j = 0; j < tabHeight; j++)
        {
            if(previousTab[i][j].seedId != 1000)
            {
                previousTab[i][j].seedId = -1;
                currentTab[i][j].seedId = -1;

                previousTab[i][j].state = 1;
                currentTab[i][j].state = 1;
             }
        }
    }

    update();
}

///////////////////////////////////////////////////////////////////Periodic

int Space::itSeedMoorePeriodic(int i, int j, int threshold)
{
    int *tempTab = new int[numberOfSeed];
    for (int i = 0; i < numberOfSeed; i++)
        tempTab[i]=0;

    if(previousTab[(tabWidth+i+1)%tabWidth][j].state == 1 && previousTab[(tabWidth+i+1)%tabWidth][j].seedId != -1)
        tempTab[previousTab[(tabWidth+i+1)%tabWidth][j].seedId]++;

    if(previousTab[(tabWidth+i-1)%tabWidth][j].state == 1 && previousTab[(tabWidth+i-1)%tabWidth][j].seedId != -1)
        tempTab[previousTab[(tabWidth+i-1)%tabWidth][j].seedId]++;

    if(previousTab[i][(tabHeight+j+1)%tabHeight].state == 1 && previousTab[i][(tabHeight+j+1)%tabHeight].seedId != -1)
        tempTab[previousTab[i][(tabHeight+j+1)%tabHeight].seedId]++;

    if(previousTab[i][(tabHeight+j-1)%tabHeight].state == 1 && previousTab[i][(tabHeight+j-1)%tabHeight].seedId != -1)
        tempTab[previousTab[i][(tabHeight+j-1)%tabHeight].seedId]++;

    if(previousTab[(tabWidth+i+1)%tabWidth][(tabHeight+j+1)%tabHeight].state == 1 && previousTab[(tabWidth+i+1)%tabWidth][(tabHeight+j+1)%tabHeight].seedId != -1)
        tempTab[previousTab[(tabWidth+i+1)%tabWidth][(tabHeight+j+1)%tabHeight].seedId]++;

    if(previousTab[(tabWidth+i-1)%tabWidth][(tabHeight+j-1)%tabHeight].state == 1 && previousTab[(tabWidth+i-1)%tabWidth][(tabHeight+j-1)%tabHeight].seedId != -1)
        tempTab[previousTab[(tabWidth+i-1)%tabWidth][(tabHeight+j-1)%tabHeight].seedId]++;

    if(previousTab[(tabWidth+i+1)%tabWidth][(tabHeight+j-1)%tabHeight].state == 1 && previousTab[(tabWidth+i+1)%tabWidth][(tabHeight+j-1)%tabHeight].seedId != -1)
        tempTab[previousTab[(tabWidth+i+1)%tabWidth][(tabHeight+j-1)%tabHeight].seedId]++;

    if(previousTab[(tabWidth+i-1)%tabWidth][(tabHeight+j+1)%tabHeight].state == 1 && previousTab[(tabWidth+i-1)%tabWidth][(tabHeight+j+1)%tabHeight].seedId != -1)
        tempTab[previousTab[(tabWidth+i-1)%tabWidth][(tabHeight+j+1)%tabHeight].seedId]++;

    int max = tempTab[0];
    int newId = -1;

    for(int i = 1; i < numberOfSeed; i++)
    {
        if(tempTab[i] > max)
        {
            max=tempTab[i];
            if(max >= threshold)
                newId = i;
        }
    }

    delete [] tempTab;
    return newId;

}

int Space::itSeedFurtherMoorePeriodic(int i, int j, int threshold)
{
    int *tempTab = new int[numberOfSeed];
    for (int i = 0; i < numberOfSeed; i++){
        tempTab[i]=0;
    }

    if(previousTab[(tabWidth+i+1)%tabWidth][(tabHeight+j+1)%tabHeight].state == 1 && previousTab[(tabWidth+i+1)%tabWidth][(tabHeight+j+1)%tabHeight].seedId != -1)
        tempTab[previousTab[(tabWidth+i+1)%tabWidth][(tabHeight+j+1)%tabHeight].seedId]++;

    if(previousTab[(tabWidth+i-1)%tabWidth][(tabHeight+j-1)%tabHeight].state == 1 && previousTab[(tabWidth+i-1)%tabWidth][(tabHeight+j-1)%tabHeight].seedId != -1)
        tempTab[previousTab[(tabWidth+i-1)%tabWidth][(tabHeight+j-1)%tabHeight].seedId]++;

    if(previousTab[(tabWidth+i+1)%tabWidth][(tabHeight+j-1)%tabHeight].state == 1 && previousTab[(tabWidth+i+1)%tabWidth][(tabHeight+j-1)%tabHeight].seedId != -1)
        tempTab[previousTab[(tabWidth+i+1)%tabWidth][(tabHeight+j-1)%tabHeight].seedId]++;

    if(previousTab[(tabWidth+i-1)%tabWidth][(tabHeight+j+1)%tabHeight].state == 1 && previousTab[(tabWidth+i-1)%tabWidth][(tabHeight+j+1)%tabHeight].seedId != -1)
        tempTab[previousTab[(tabWidth+i-1)%tabWidth][(tabHeight+j+1)%tabHeight].seedId]++;

    int max = tempTab[0];
    int newId = -1;

    for(int i = 1; i < numberOfSeed; i++){
        if(tempTab[i] > max){
            max=tempTab[i];
            if(max >= threshold)
                newId = i;
        }
    }

    delete [] tempTab;
    return newId;

}

int Space::itSeedVonNeumanPeriodic(int i, int j, int threshold)
{
    int *tempTab = new int[numberOfSeed];
    for (int i = 0; i < numberOfSeed; i++){
        tempTab[i]=0;
    }

    if(previousTab[(tabWidth+i+1)%tabWidth][j].state == 1 && previousTab[(tabWidth+i+1)%tabWidth][j].seedId != -1)
        tempTab[previousTab[(tabWidth+i+1)%tabWidth][j].seedId]++;

    if(previousTab[(tabWidth+i-1)%tabWidth][j].state == 1 && previousTab[(tabWidth+i-1)%tabWidth][j].seedId != -1)
        tempTab[previousTab[(tabWidth+i-1)%tabWidth][j].seedId]++;

    if(previousTab[i][(tabHeight+j+1)%tabHeight].state == 1 && previousTab[i][(tabHeight+j+1)%tabHeight].seedId != -1)
        tempTab[previousTab[i][(tabHeight+j+1)%tabHeight].seedId]++;

    if(previousTab[i][(tabHeight+j-1)%tabHeight].state == 1 && previousTab[i][(tabHeight+j-1)%tabHeight].seedId != -1)
        tempTab[previousTab[i][(tabHeight+j-1)%tabHeight].seedId]++;

    int max = tempTab[0];
    int newId = -1;

    for(int i = 1; i < numberOfSeed; i++){
        if(tempTab[i] > max){
            max=tempTab[i];
            if(max >= threshold)
                newId = i;
        }
    }

    delete [] tempTab;
    return newId;
}

int Space::itSeedHexLeftPeriodic(int i, int j)
{
    int *tempTab = new int[numberOfSeed];
    for (int i = 0; i < numberOfSeed; i++){
        tempTab[i]=0;
    }

    if(previousTab[(tabWidth+i+1)%tabWidth][j].state == 1 && previousTab[(tabWidth+i+1)%tabWidth][j].seedId != -1)
        tempTab[previousTab[(tabWidth+i+1)%tabWidth][j].seedId]++;

    if(previousTab[(tabWidth+i-1)%tabWidth][j].state == 1 && previousTab[(tabWidth+i-1)%tabWidth][j].seedId != -1)
        tempTab[previousTab[(tabWidth+i-1)%tabWidth][j].seedId]++;

    if(previousTab[i][(tabHeight+j+1)%tabHeight].state == 1 && previousTab[i][(tabHeight+j+1)%tabHeight].seedId != -1)
        tempTab[previousTab[i][(tabHeight+j+1)%tabHeight].seedId]++;

    if(previousTab[i][(tabHeight+j-1)%tabHeight].state == 1 && previousTab[i][(tabHeight+j-1)%tabHeight].seedId != -1)
        tempTab[previousTab[i][(tabHeight+j-1)%tabHeight].seedId]++;

    if(previousTab[(tabWidth+i+1)%tabWidth][(tabHeight+j-1)%tabHeight].state == 1 && previousTab[(tabWidth+i+1)%tabWidth][(tabHeight+j-1)%tabHeight].seedId != -1)
        tempTab[previousTab[(tabWidth+i+1)%tabWidth][(tabHeight+j-1)%tabHeight].seedId]++;

    if(previousTab[(tabWidth+i-1)%tabWidth][(tabHeight+j+1)%tabHeight].state == 1 && previousTab[(tabWidth+i-1)%tabWidth][(tabHeight+j+1)%tabHeight].seedId != -1)
        tempTab[previousTab[(tabWidth+i-1)%tabWidth][(tabHeight+j+1)%tabHeight].seedId]++;

    int max = tempTab[0];
    int newId = -1;

    for(int i = 1; i < numberOfSeed; i++){
        if(tempTab[i] > max){
            max=tempTab[i];
            newId = i;
        }
    }

    delete [] tempTab;
    return newId;

}

int Space::itSeedHexRightPeriodic(int i, int j)
{
    int *tempTab = new int[numberOfSeed];
    for (int i = 0; i < numberOfSeed; i++){
        tempTab[i]=0;
    }

    if(previousTab[(tabWidth+i+1)%tabWidth][j].state == 1 && previousTab[(tabWidth+i+1)%tabWidth][j].seedId != -1)
        tempTab[previousTab[(tabWidth+i+1)%tabWidth][j].seedId]++;

    if(previousTab[(tabWidth+i-1)%tabWidth][j].state == 1 && previousTab[(tabWidth+i-1)%tabWidth][j].seedId != -1)
        tempTab[previousTab[(tabWidth+i-1)%tabWidth][j].seedId]++;

    if(previousTab[i][(tabHeight+j+1)%tabHeight].state == 1 && previousTab[i][(tabHeight+j+1)%tabHeight].seedId != -1)
        tempTab[previousTab[i][(tabHeight+j+1)%tabHeight].seedId]++;

    if(previousTab[i][(tabHeight+j-1)%tabHeight].state == 1 && previousTab[i][(tabHeight+j-1)%tabHeight].seedId != -1)
        tempTab[previousTab[i][(tabHeight+j-1)%tabHeight].seedId]++;

    if(previousTab[(tabWidth+i+1)%tabWidth][(tabHeight+j+1)%tabHeight].state == 1 && previousTab[(tabWidth+i+1)%tabWidth][(tabHeight+j+1)%tabHeight].seedId != -1)
        tempTab[previousTab[(tabWidth+i+1)%tabWidth][(tabHeight+j+1)%tabHeight].seedId]++;

    if(previousTab[(tabWidth+i-1)%tabWidth][(tabHeight+j-1)%tabHeight].state == 1 && previousTab[(tabWidth+i-1)%tabWidth][(tabHeight+j-1)%tabHeight].seedId != -1)
        tempTab[previousTab[(tabWidth+i-1)%tabWidth][(tabHeight+j-1)%tabHeight].seedId]++;


    int max = tempTab[0];
    int newId = -1;

    for(int i = 1; i < numberOfSeed; i++){
        if(tempTab[i] > max){
            max=tempTab[i];
            newId = i;
        }
    }

    delete [] tempTab;
    return newId;

}

int Space::extensionOfMoorePeriodic(int i, int j)
{
    int mooreThreshold = 8;
    int vonNeumanThreshold = 3;
    int furtherMooreThreshold = 3;

    if(itSeedMoorePeriodic(i, j, mooreThreshold) != -1)
       return itSeedMoorePeriodic(i, j, mooreThreshold);
    else if(itSeedVonNeumanPeriodic(i, j, vonNeumanThreshold) != -1)
        return itSeedVonNeumanPeriodic(i, j, vonNeumanThreshold);
    else if(itSeedFurtherMoorePeriodic(i, j, furtherMooreThreshold) != - 1)
        return itSeedFurtherMoorePeriodic(i, j, furtherMooreThreshold);

    qsrand(qrand());
    if(qrand()%100 < probabilityThreshold)
        return itSeedMoorePeriodic(i, j);
    else
        return -1;
}

//////////////////////////////////////////////////////////////////Absorbing

int Space::itSeedMooreAbsorbing(int i, int j, int threshold)
{
    int *tempTab = new int[numberOfSeed];
    for (int i = 0; i < numberOfSeed; i++){
        tempTab[i]=0;
    }

    if(previousTab[i+1][j].state == 1 && previousTab[i+1][j].seedId != -1)
        tempTab[previousTab[i+1][j].seedId]++;

    if(previousTab[i-1][j].state == 1 && previousTab[i-1][j].seedId != -1)
        tempTab[previousTab[i-1][j].seedId]++;

    if(previousTab[i][j+1].state == 1 && previousTab[i][j+1].seedId != -1)
        tempTab[previousTab[i][j+1].seedId]++;

    if(previousTab[i][j-1].state == 1 && previousTab[i][j-1].seedId != -1)
        tempTab[previousTab[i][j-1].seedId]++;

    if(previousTab[i+1][j+1].state == 1 && previousTab[i+1][j+1].seedId != -1)
        tempTab[previousTab[i+1][j+1].seedId]++;

    if(previousTab[i-1][j-1].state == 1 && previousTab[i-1][j-1].seedId != -1)
        tempTab[previousTab[i-1][j-1].seedId]++;

    if(previousTab[i+1][j-1].state == 1 && previousTab[i+1][j-1].seedId != -1)
        tempTab[previousTab[i+1][j-1].seedId]++;

    if(previousTab[i-1][j+1].state == 1 && previousTab[i-1][j+1].seedId != -1)
        tempTab[previousTab[i-1][j+1].seedId]++;

    int max = tempTab[0];
    int newId = -1;

    for(int i = 1; i < numberOfSeed; i++){
        if(tempTab[i] > max){
            max=tempTab[i];
            if(max >= threshold)
                newId = i;
        }
    }

    delete [] tempTab;
    return newId;

}

int Space::itSeedFurtherMooreAbsorbing(int i, int j, int threshold)
{
    int *tempTab = new int[numberOfSeed];
    for (int i = 0; i < numberOfSeed; i++){
        tempTab[i]=0;
    }

    if(previousTab[i+1][j+1].state == 1 && previousTab[i+1][j+1].seedId != -1)
        tempTab[previousTab[i+1][j+1].seedId]++;

    if(previousTab[i-1][j-1].state == 1 && previousTab[i-1][j-1].seedId != -1)
        tempTab[previousTab[i-1][j-1].seedId]++;

    if(previousTab[i+1][j-1].state == 1 && previousTab[i+1][j-1].seedId != -1)
        tempTab[previousTab[i+1][j-1].seedId]++;

    if(previousTab[i-1][j+1].state == 1 && previousTab[i-1][j+1].seedId != -1)
        tempTab[previousTab[i-1][j+1].seedId]++;

    int max = tempTab[0];
    int newId = -1;

    for(int i = 1; i < numberOfSeed; i++){
        if(tempTab[i] > max){
            max=tempTab[i];
            if(max >= threshold)
                newId = i;
        }
    }

    delete [] tempTab;
    return newId;

}

int Space::itSeedVonNeumanAbsorbing(int i, int j, int threshold)
{
    int *tempTab = new int[numberOfSeed];
    for (int i = 0; i < numberOfSeed; i++){
        tempTab[i]=0;
    }

    if(previousTab[i+1][j].state == 1 && previousTab[i+1][j].seedId != -1)
        tempTab[previousTab[i+1][j].seedId]++;

    if(previousTab[i-1][j].state == 1 && previousTab[i-1][j].seedId != -1)
        tempTab[previousTab[i-1][j].seedId]++;

    if(previousTab[i][j+1].state == 1 && previousTab[i][j+1].seedId != -1)
        tempTab[previousTab[i][j+1].seedId]++;

    if(previousTab[i][j-1].state == 1 && previousTab[i][j-1].seedId != -1)
        tempTab[previousTab[i][j-1].seedId]++;


    int max = tempTab[0];
    int newId = -1;

    for(int i = 1; i < numberOfSeed; i++){
        if(tempTab[i] > max){
            max=tempTab[i];
            if(max >= threshold)
                newId = i;
        }
    }

    delete [] tempTab;
    return newId;

}

int Space::itSeedHexLeftAbsorbing(int i, int j)
{
    int *tempTab = new int[numberOfSeed];
    for (int i = 0; i < numberOfSeed; i++){
        tempTab[i]=0;
    }

    if(previousTab[i+1][j].state == 1 && previousTab[i+1][j].seedId != -1)
        tempTab[previousTab[i+1][j].seedId]++;

    if(previousTab[i-1][j].state == 1 && previousTab[i-1][j].seedId != -1)
        tempTab[previousTab[i-1][j].seedId]++;

    if(previousTab[i][j+1].state == 1 && previousTab[i][j+1].seedId != -1)
        tempTab[previousTab[i][j+1].seedId]++;

    if(previousTab[i][j-1].state == 1 && previousTab[i][j-1].seedId != -1)
        tempTab[previousTab[i][j-1].seedId]++;

    if(previousTab[i+1][j-1].state == 1 && previousTab[i+1][j-1].seedId != -1)
        tempTab[previousTab[i+1][j-1].seedId]++;

    if(previousTab[i-1][j+1].state == 1 && previousTab[i-1][j+1].seedId != -1)
        tempTab[previousTab[i-1][j+1].seedId]++;


    int max = tempTab[0];
    int newId = -1;

    for(int i = 1; i < numberOfSeed; i++){
        if(tempTab[i] > max){
            max=tempTab[i];
            newId = i;
        }
    }

    delete [] tempTab;
    return newId;

}

int Space::itSeedHexRightAbsorbing(int i, int j)
{
    int *tempTab = new int[numberOfSeed];
    for (int i = 0; i < numberOfSeed; i++){
        tempTab[i]=0;
    }

    if(previousTab[i+1][j].state == 1 && previousTab[i+1][j].seedId != -1)
        tempTab[previousTab[i+1][j].seedId]++;

    if(previousTab[i-1][j].state == 1 && previousTab[i-1][j].seedId != -1)
        tempTab[previousTab[i-1][j].seedId]++;

    if(previousTab[i][j+1].state == 1 && previousTab[i][j+1].seedId != -1)
        tempTab[previousTab[i][j+1].seedId]++;

    if(previousTab[i][j-1].state == 1 && previousTab[i][j-1].seedId != -1)
        tempTab[previousTab[i][j-1].seedId]++;

    if(previousTab[i+1][j+1].state == 1 && previousTab[i+1][j+1].seedId != -1)
        tempTab[previousTab[i+1][j+1].seedId]++;

    if(previousTab[i-1][j-1].state == 1 && previousTab[i-1][j-1].seedId != -1)
        tempTab[previousTab[i-1][j-1].seedId]++;


    int max = tempTab[0];
    int newId = -1;

    for(int i = 1; i < numberOfSeed; i++){
        if(tempTab[i] > max){
            max=tempTab[i];
            newId = i;
        }
    }

    delete [] tempTab;
    return newId;

}

int Space::extensionOfMooreAbsorbing(int i, int j)
{
    int mooreThreshold = 8;
    int vonNeumanThreshold = 3;
    int furtherMooreThreshold = 3;

    if(itSeedMooreAbsorbing(i, j, mooreThreshold) != -1)
       return itSeedMooreAbsorbing(i, j, mooreThreshold);
    else if(itSeedVonNeumanAbsorbing(i, j, vonNeumanThreshold) != -1)
        return itSeedVonNeumanAbsorbing(i, j, vonNeumanThreshold);
    else if(itSeedFurtherMooreAbsorbing(i, j, furtherMooreThreshold) != - 1)
        return itSeedFurtherMooreAbsorbing(i, j, furtherMooreThreshold);

    qsrand(qrand());
    if(qrand()%100 < probabilityThreshold)
        return itSeedMooreAbsorbing(i, j);
    else
        return -1;
}

void Space::nextGeneration()
{
    if(BCType == 0)
    {
        counterOfGenerations++;

        for(int i = 1; i < tabWidth-1; i++)
        {
            for(int j = 1; j < tabHeight-1; j++)
            {
                if(previousTab[i][j].state == 0)
                {
                    if(neighborhoodType == 0)
                    {
                        if(itSeedMooreAbsorbing(i,j) != -1)
                        {
                            currentTab[i][j].seedId = itSeedMooreAbsorbing(i,j);
                            currentTab[i][j].state = 1;
                        }
                    }

                    if(neighborhoodType == 1)
                    {
                        if(itSeedVonNeumanAbsorbing(i,j) != -1)
                        {
                            currentTab[i][j].seedId = itSeedVonNeumanAbsorbing(i,j);
                            currentTab[i][j].state = 1;
                        }
                    }

                    if(neighborhoodType == 2)
                    {
                        if(itSeedHexLeftAbsorbing(i,j) != -1)
                        {
                            currentTab[i][j].seedId = itSeedHexLeftAbsorbing(i,j);
                            currentTab[i][j].state = 1;
                        }
                    }

                    if(neighborhoodType == 3)
                    {
                        if(itSeedHexRightAbsorbing(i,j) != -1)
                        {
                            currentTab[i][j].seedId = itSeedHexRightAbsorbing(i,j);
                            currentTab[i][j].state = 1;
                        }
                    }

                    if(neighborhoodType == 4)
                    {
                        qsrand(qrand());
                        tempRand = qrand()%2;

                        if(tempRand == 0)
                        {
                            if(itSeedHexLeftAbsorbing(i,j) != -1)
                            {
                                currentTab[i][j].seedId = itSeedHexLeftAbsorbing(i,j);
                                currentTab[i][j].state = 1;
                            }
                        }
                        if(tempRand == 1)
                        {
                            if(itSeedHexRightAbsorbing(i,j) != -1)
                            {
                                currentTab[i][j].seedId = itSeedHexRightAbsorbing(i,j);
                                currentTab[i][j].state = 1;
                            }
                        }
                    }

                    if(neighborhoodType == 5)
                    {
                        int tmpSeedId = extensionOfMooreAbsorbing(i,j);
                        if(tmpSeedId != -1)
                        {
                            currentTab[i][j].seedId = tmpSeedId;
                            currentTab[i][j].state = 1;
                        }
                    }
                }
            }
        }

        for(int i = 0; i < tabWidth; i++)
            for(int j = 0; j < tabHeight; j++)
                previousTab[i][j]=currentTab[i][j];

        update();

        if(counterOfGenerations==numberOfGenerations)
            timer->stop();
    }

    if(BCType == 1)
    {
        counterOfGenerations++;

        for(int i = 0; i < tabWidth; i++)
        {
            for(int j = 0; j < tabHeight; j++)
            {
                if(previousTab[i][j].state == 0)
                {
                    if(neighborhoodType == 0)
                    {
                        if(itSeedMoorePeriodic(i,j) != -1)
                        {
                            currentTab[i][j].seedId = itSeedMoorePeriodic(i,j);
                            currentTab[i][j].state = 1;
                        }
                    }

                    if(neighborhoodType == 1)
                    {
                        if(itSeedVonNeumanPeriodic(i,j) != -1)
                        {
                            currentTab[i][j].seedId = itSeedVonNeumanPeriodic(i,j);
                            currentTab[i][j].state = 1;
                        }
                    }

                    if(neighborhoodType == 2)
                    {
                        if(itSeedHexLeftPeriodic(i,j) != -1)
                        {
                            currentTab[i][j].seedId = itSeedHexLeftPeriodic(i,j);
                            currentTab[i][j].state = 1;
                        }
                    }

                    if(neighborhoodType == 3)
                    {
                        if(itSeedHexRightPeriodic(i,j) != -1)
                        {
                            currentTab[i][j].seedId = itSeedHexRightPeriodic(i,j);
                            currentTab[i][j].state = 1;
                        }
                    }

                    if(neighborhoodType == 4)
                    {
                        qsrand(qrand());
                        tempRand = qrand()%2;

                        if(tempRand == 0)
                        {
                            if(itSeedHexLeftPeriodic(i,j) != -1)
                            {
                                currentTab[i][j].seedId = itSeedHexLeftPeriodic(i,j);
                                currentTab[i][j].state = 1;
                            }
                        }
                        if(tempRand == 1)
                        {
                            if(itSeedHexRightPeriodic(i,j) != -1)
                            {
                                currentTab[i][j].seedId = itSeedHexRightPeriodic(i,j);
                                currentTab[i][j].state = 1;
                            }
                        }
                    }

                    if(neighborhoodType == 5)
                    {
                        int tmpSeedId = extensionOfMoorePeriodic(i,j);
                        if(tmpSeedId != -1)
                        {
                            currentTab[i][j].seedId = tmpSeedId;
                            currentTab[i][j].state = 1;
                        }
                    }
                }
            }
        }

        for(int i = 0; i < tabWidth; i++)
        {
            for(int j = 0; j < tabHeight; j++)
            {
                previousTab[i][j]=currentTab[i][j];
                // TODO check sum of state to find out if simualtion should stop
            }
        }

        update();

        if(counterOfGenerations==numberOfGenerations)
            timer->stop();
    }
}

void Space::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    paintGrid(p);
    paintTab(p);
}

void Space::paintGrid(QPainter &p)
{
    QRect borders(0, 0, width()-1, height()-1);
    QColor gridColor = "#000000";
    gridColor.setAlpha(10);
    p.setPen(gridColor);

    double cellWidth = static_cast<double>(width())/tabWidth;
    for(double i = cellWidth; i <= width(); i += cellWidth)
        p.drawLine(i, 0, i, height());

    double cellHeight = static_cast<double>(height())/tabHeight;
    for(double i = cellHeight; i<= height(); i+= cellHeight)
        p.drawLine(0, i, width(), i);

    p.drawRect(borders);
}

void Space::paintTab(QPainter &p)
{

    double cellWidth = static_cast<double>(width())/tabWidth;
    double cellHeight = static_cast<double>(height())/tabHeight;

    for(int i = 0; i < tabWidth; i++)
    {
        for(int j = 0; j < tabHeight; j++)
        {
            if(previousTab[i][j].state == 1)
            {
                qreal left = (qreal)(cellWidth*i - cellWidth);
                qreal top = (qreal)(cellHeight*j - cellHeight);
                QRectF r(left, top, (qreal)cellWidth, (qreal)cellHeight);

                if(previousTab[i][j].seedId == 1000) //TODO - refactor
                {
                    p.fillRect(r,QBrush(QColor(0,0,0)));
                    this->image.setPixelColor(i, j, this->seedIdToColorMap[previousTab[i][j].seedId]);
                }
                else
                {
                    p.fillRect(r,QBrush(this->seedIdToColorMap[previousTab[i][j].seedId]));
                    this->image.setPixelColor(i, j, this->seedIdToColorMap[previousTab[i][j].seedId]);
                }
            }
        }
    }
}

//void Space::mouseMoveEvent(QMouseEvent *e)
//{
//    double cellWidth = static_cast<double>(width())/tabWidth;
//    double cellHeight = static_cast<double>(height())/tabHeight;

//    int i = floor(e->y()/cellHeight)+1;
//    int j = floor(e->x()/cellWidth)+1;
//    if(previousTab[i][j].state == 0)
//    {
//        previousTab[i][j].state = 1;
//        previousTab[i][j].seedId = 0;
//        update();
//    }
//}

//void Space::mousePressEvent(QMouseEvent *e)
//{
//    double cellWidth = static_cast<double>(width())/tabWidth;
//    double cellHeight = static_cast<double>(height())/tabHeight;

//    int i = floor(e->y()/cellHeight)+1;
//    int j = floor(e->x()/cellWidth)+1;
//    if(previousTab[i][j].state == 0)
//    {
//        previousTab[i][j].state = 1;
//        previousTab[i][j].seedId = 0;
//        update();
//    }
//    else
//    {
//        previousTab[i][j].state = 0;
//        previousTab[i][j].seedId = -1;
//        update();
//    }

//}
