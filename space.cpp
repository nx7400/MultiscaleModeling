#include <QMessageBox>
#include <QPainter>
#include <QMouseEvent>
#include <QRectF>
#include <QTimer>
#include <qmath.h>
#include <QtTest/QTest>
#include <windows.h>
#include <math.h>
#include <iostream>
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

    for(int seedId = 0; seedId < maxSeedId; ++seedId) {
        seedIdToColorMap[seedId] = QColor((seedId*20+45)%255, (seedId*12+26)%255, (seedId*112+11)%255);
        colorToSeedIdMap[QColor((seedId*20+45)%255, (seedId*12+26)%255, (seedId*112+11)%255)] = seedId;
    }

    seedIdToColorMap[1000] = QColor(Qt::black);
    colorToSeedIdMap[QColor(Qt::black)] = 1000;
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

void Space::setSeedRadius(int n)
{
    seedRadius = n;
}

int Space::getSeedRadius()
{
    return seedRadius;
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

    for(int i = 0; i < tabWidth; i++){
        for(int j = 0; j< tabHeight; j++){
            previousTab[i][j].seedId = -1;
            currentTab[i][j].seedId = -1;

            previousTab[i][j].state = 0;
            currentTab[i][j].state = 0;

            previousTab[i][j].checked = false;
            currentTab[i][j].checked = false;

            previousTab[i][j].energy = 0;
            currentTab[i][j].energy = 0;

        }
    }

    int counter = 0;
    int tempI, tempJ;
    qsrand(qrand());

    while(counter <= numberOfSeed){

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

    for(int i = 0; i < tabWidth; i++){
        for(int j = 0; j< tabHeight; j++){
            previousTab[i][j].seedId = -1;
            currentTab[i][j].seedId = -1;

            previousTab[i][j].state = 0;
            currentTab[i][j].state = 0;

            previousTab[i][j].checked = false;
            currentTab[i][j].checked = false;

            previousTab[i][j].energy = 0;
            currentTab[i][j].energy = 0;
        }
    }

    int counter = 1;

    int temp = static_cast<int>(sqrt(numberOfSeed));

    int t1 = tabWidth/temp;
    int t2 = tabHeight/temp;

        for(int i = t1/2; i < tabWidth; i = i + t1){
            for(int j = t2/2; j < tabHeight; j = j + t2){

                previousTab[i][j].state = 1;
                previousTab[i][j].seedId = counter;
                counter++;
            }
        }

    update();
}

void Space::gradientSeed()
{

    for(int i = 0; i < tabWidth; i++){
        for(int j = 0; j< tabHeight; j++){
            previousTab[i][j].seedId = -1;
            currentTab[i][j].seedId = -1;

            previousTab[i][j].state = 0;
            currentTab[i][j].state = 0;

            previousTab[i][j].checked = false;
            currentTab[i][j].checked = false;

            previousTab[i][j].energy = 0;
            currentTab[i][j].energy = 0;
        }
    }

    int counter = 0;
    int tempI, tempJ;
    qsrand(qrand());

    int tempHeight = tabHeight/3;

    int numberSeed1 = static_cast<int>(numberOfSeed*0.5);
    int numberSeed2 = static_cast<int>(numberOfSeed*0.3);
    int numberSeed3 = static_cast<int>(numberOfSeed*0.2);

    while(counter <= numberSeed1){

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

    while(counter <= numberSeed2){

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

    while(counter <= numberSeed3){

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

void Space::randomWithRadiusSeed()
{
    bool seedOk = true;
    int counter = 1;
    int tempI, tempJ;
    qsrand(qrand());

    while(counter <= numberOfSeed){

        seedOk = true;

        tempI = qrand()%tabWidth;
        tempJ = qrand()%tabHeight;

        if(previousTab[tempI][tempJ].state == 0)
        {
            for(int i = 0; i < tabWidth; i++){
                for(int j = 0; j < tabHeight; j++){
                    if(static_cast<int>(sqrt(pow((tempI - i),2) + pow((tempJ - j),2))) <= seedRadius )
                    {
                        if(previousTab[i][j].state == 0)
                        {

                        }
                        else
                        {
                            seedOk = false;
                        }

                     }

            }
          }

          if(seedOk)
          {
            for(int i = 0; i < tabWidth; i++){
                for(int j = 0; j < tabHeight; j++){
                    if((static_cast<int>(sqrt(pow((tempI - i),2) + pow((tempJ - j),2))) <= seedRadius) && (previousTab[i][j].state == 0) )
                    {
                        previousTab[tempI][tempJ].state = 1;
                        previousTab[tempI][tempJ].seedId = counter;
                        previousTab[i][j].state = 1;
                        previousTab[i][j].seedId = counter;
                     }
                }
            }
          }
            counter++;
        }
    }
    update();
}

void Space::holesOnSeedBorder()
{
    bool** borderTab;

    borderTab = new bool*[tabWidth];

    for(int i = 0; i < tabWidth; i++)
    {
        borderTab[i] = new bool[tabHeight];
    }

    for(int i = 0; i < tabWidth; i++){
        for(int j = 0; j< tabHeight; j++){
          borderTab[i][j] = false;
        }
    }

    for(int i = 1; i < tabWidth-1; i++){
        for(int j = 1; j< tabHeight-1; j++){

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

    while(counter <= numberOfHoles){

        tempI = qrand()%tabWidth;
        tempJ = qrand()%tabHeight;

        if(borderTab[tempI][tempJ] == true)
        {
            for(int i = 0; i < tabWidth; i++){
                for(int j = 0; j < tabHeight; j++){
                    if((static_cast<int>(sqrt(pow((tempI - i),2) + pow((tempJ - j),2))) <= seedRadius))
                    {
                        previousTab[tempI][tempJ].state = 1;
                        previousTab[tempI][tempJ].seedId = 1000;
                        previousTab[i][j].state = 1;
                        previousTab[i][j].seedId = 1000;
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

    while(counter <= numberOfHoles) {

        tempI = qrand()%tabWidth;
        tempJ = qrand()%tabHeight;

        for(int i = 0; i < tabWidth; i++) {
            for(int j = 0; j < tabHeight; j++) {
                if((static_cast<int>(sqrt(pow((tempI - i),2) + pow((tempJ - j),2))) <= seedRadius))
                {
                    previousTab[tempI][tempJ].state = 1;
                    previousTab[tempI][tempJ].seedId = 1000;
                    previousTab[i][j].state = 1;
                    previousTab[i][j].seedId = 1000;
                 }

            }
        }

        counter++;
    }

    update();
}

void Space::monteCarlo()
{
    int i, j;
    int counter = 0;
    int counter2 = 0;

    //std::cout<<"przed while"<<std::endl;

    while(counter2 <= 2000)
    {
        counter = 0;
        update();
        //(()-4*tabWidth)

        while(counter <= (tabHeight*tabWidth) )
        {
            //update();
            qsrand(qrand());

            i = qrand()%((tabWidth)-2) + 1;
            j = qrand()%((tabHeight)-2) + 1;

            //std::cout<<"w while"<<std::endl;
            //std::cout<<i<<" "<<j<<std::endl;

            if(previousTab[i][j].checked == false)
            {

                if(previousTab[i+1][j].seedId != previousTab[i][j].seedId)
                {
                    previousTab[i][j].energy++;
                }

                if(previousTab[i-1][j].seedId != previousTab[i][j].seedId)
                {
                    previousTab[i][j].energy++;
                }

                if(previousTab[i][j+1].seedId != previousTab[i][j].seedId)
                {
                    previousTab[i][j].energy++;
                }

                if(previousTab[i][j-1].seedId != previousTab[i][j].seedId)
                {
                    previousTab[i][j].energy++;
                }

                if(previousTab[i+1][j+1].seedId != previousTab[i][j].seedId)
                {
                    previousTab[i][j].energy++;
                }

                if(previousTab[i-1][j-1].seedId != previousTab[i][j].seedId)
                {
                    previousTab[i][j].energy++;
                }

                if(previousTab[i+1][j-1].seedId != previousTab[i][j].seedId)
                {
                    previousTab[i][j].energy++;
                }

                if(previousTab[i-1][j+1].seedId != previousTab[i][j].seedId)
                {
                    previousTab[i][j].energy++;
                }

                int oldId = previousTab[i][j].seedId;
                int oldEnergy = previousTab[i][j].energy;
                previousTab[i][j].energy = 0;

                qsrand(qrand());

                int tempRand = qrand()%3;

                if(tempRand == 0)
                    previousTab[i][j].seedId = previousTab[i+1][j].seedId;
                if(tempRand == 1)
                    previousTab[i][j].seedId = previousTab[i][j+1].seedId;
                if(tempRand == 2)
                    previousTab[i][j].seedId = previousTab[i][j-1].seedId;
                if(tempRand == 3)
                    previousTab[i][j].seedId = previousTab[i-1][j].seedId;

                //int newId = qrand()%(numberOfSeed);

                // previousTab[i][j].seedId = newId;

                // std::cout<<"nowe id"<<newId<<std::endl;
                //std::cout<<"stare id"<<oldId<<std::endl;


                if(previousTab[i+1][j].seedId != previousTab[i][j].seedId)
                {
                    previousTab[i][j].energy++;
                }

                if(previousTab[i-1][j].seedId != previousTab[i][j].seedId)
                {
                    previousTab[i][j].energy++;
                }

                if(previousTab[i][j+1].seedId != previousTab[i][j].seedId)
                {
                    previousTab[i][j].energy++;
                }

                if(previousTab[i][j-1].seedId != previousTab[i][j].seedId)
                {
                    previousTab[i][j].energy++;
                }

                if(previousTab[i+1][j+1].seedId != previousTab[i][j].seedId)
                {
                    previousTab[i][j].energy++;
                }

                if(previousTab[i-1][j-1].seedId != previousTab[i][j].seedId)
                {
                    previousTab[i][j].energy++;
                }

                if(previousTab[i+1][j-1].seedId != previousTab[i][j].seedId)
                {
                    previousTab[i][j].energy++;
                }

                if(previousTab[i-1][j+1].seedId != previousTab[i][j].seedId)
                {
                    previousTab[i][j].energy++;
                }

                std::cout<<"stara enegia"<<oldEnergy<<std::endl;
                std::cout<<"nowa enegia"<<previousTab[i][j].energy<<std::endl;

                if( oldEnergy < previousTab[i][j].energy)
                {
                    previousTab[i][j].seedId = oldId;
                    previousTab[i][j].energy = oldEnergy;
                    previousTab[i][j].checked = true;

                }
                else
                {
                    previousTab[i][j].checked = true;
                }

                counter++;
                //std::cout<<"licznik "<<counter<<std::endl;

                //update();

             }

            //update();
         }

        counter2++;
        std::cout<<"licznik2:"<<counter2<<std::endl;
    }
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

    for(int i = 0; i < tabWidth; i++){
     for(int j = 0; j< tabHeight; j++){
        previousTab[i][j].seedId = -1;
        currentTab[i][j].seedId = -1;

        previousTab[i][j].state = 0;
        currentTab[i][j].state = 0;

        previousTab[i][j].checked = false;
        currentTab[i][j].checked = false;

        previousTab[i][j].energy = 0;
        currentTab[i][j].energy = 0;
     }
    }

    update();
}

///////////////////////////////////////////////////////////////////Periodyczne

int Space::itSeedMoore(int i, int j)
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

    return newId;

}

int Space::itSeedVonNeuman(int i, int j)
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
            newId = i;
        }
    }

    return newId;
}

int Space::itSeedHexLeft(int i, int j)
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

    return newId;

}

int Space::itSeedHexRight(int i, int j)
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

    return newId;

}

//////////////////////////////////////////////////////////////////Pochłaniające

int Space::itSeedMoore2(int i, int j)
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
            newId = i;
        }
    }

    return newId;

}

int Space::itSeedVonNeuman2(int i, int j)
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
            newId = i;
        }
    }

    return newId;

}

int Space::itSeedHexLeft2(int i, int j)
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

    return newId;

}

int Space::itSeedHexRight2(int i, int j)
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

    return newId;

}

void Space::nextGeneration()
{
    if(BCType == 0){

        counterOfGenerations++;

            for(int i = 1; i < tabWidth-1; i++){
                for(int j = 1; j < tabHeight-1; j++){
                    if(previousTab[i][j].state == 0){
                        if(neighborhoodType == 0){

                            if(itSeedMoore2(i,j) != -1){

                                currentTab[i][j].seedId = itSeedMoore2(i,j);
                                currentTab[i][j].state = 1;

                            }
                        }

                        if(neighborhoodType == 1){
                            if(itSeedVonNeuman2(i,j) != -1){

                                currentTab[i][j].seedId = itSeedVonNeuman2(i,j);
                                currentTab[i][j].state = 1;

                            }
                        }

                        if(neighborhoodType == 2){
                            if(itSeedHexLeft2(i,j) != -1){

                                currentTab[i][j].seedId = itSeedHexLeft2(i,j);
                                currentTab[i][j].state = 1;

                            }
                        }

                        if(neighborhoodType == 3){
                            if(itSeedHexRight2(i,j) != -1){

                                currentTab[i][j].seedId = itSeedHexRight2(i,j);
                                currentTab[i][j].state = 1;

                            }
                        }

                        if(neighborhoodType == 4){

                            qsrand(qrand());
                            tempRand = qrand()%2;

                            if(tempRand == 0){

                                if(itSeedHexLeft2(i,j) != -1){

                                    currentTab[i][j].seedId = itSeedHexLeft2(i,j);
                                    currentTab[i][j].state = 1;

                                }
                            }
                            if(tempRand == 1){

                                if(itSeedHexRight2(i,j) != -1){

                                    currentTab[i][j].seedId = itSeedHexRight2(i,j);
                                    currentTab[i][j].state = 1;

                                }
                            }

                        }

                    }
                }
            }

            for(int i = 0; i < tabWidth; i++){
                for(int j = 0; j < tabHeight; j++){
                    previousTab[i][j]=currentTab[i][j];
                }
           }

        update();

        if(counterOfGenerations==numberOfGenerations)
            timer->stop();
    }

    if(BCType == 1){

        counterOfGenerations++;


            for(int i = 0; i < tabWidth; i++){
                for(int j = 0; j < tabHeight; j++){
                    if(previousTab[i][j].state == 0){
                        if(neighborhoodType == 0){

                            if(itSeedMoore(i,j) != -1){

                                currentTab[i][j].seedId = itSeedMoore(i,j);
                                currentTab[i][j].state = 1;

                            }
                        }

                        if(neighborhoodType == 1){
                            if(itSeedVonNeuman(i,j) != -1){

                                currentTab[i][j].seedId = itSeedVonNeuman(i,j);
                                currentTab[i][j].state = 1;

                            }
                        }

                        if(neighborhoodType == 2){
                            if(itSeedHexLeft(i,j) != -1){

                                currentTab[i][j].seedId = itSeedHexLeft(i,j);
                                currentTab[i][j].state = 1;

                            }
                        }

                        if(neighborhoodType == 3){
                            if(itSeedHexRight(i,j) != -1){

                                currentTab[i][j].seedId = itSeedHexRight(i,j);
                                currentTab[i][j].state = 1;

                            }
                        }

                        if(neighborhoodType == 4){

                            qsrand(qrand());
                            tempRand = qrand()%2;

                            if(tempRand == 0){

                                if(itSeedHexLeft(i,j) != -1){

                                    currentTab[i][j].seedId = itSeedHexLeft(i,j);
                                    currentTab[i][j].state = 1;

                                }
                            }
                            if(tempRand == 1){

                                if(itSeedHexRight(i,j) != -1){

                                    currentTab[i][j].seedId = itSeedHexRight(i,j);
                                    currentTab[i][j].state = 1;

                                }
                            }


                        }

                    }
                }
            }

            for(int i = 0; i < tabWidth; i++){
                for(int j = 0; j < tabHeight; j++){
                    previousTab[i][j]=currentTab[i][j];
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
    for(double i = cellWidth; i <= width(); i += cellWidth){
        p.drawLine(i, 0, i, height());
    }

    double cellHeight = static_cast<double>(height())/tabHeight;
    for(double i = cellHeight; i<= height(); i+= cellHeight){
        p.drawLine(0, i, width(), i);
    }

    p.drawRect(borders);

}

void Space::paintTab(QPainter &p)
{

    double cellWidth = static_cast<double>(width())/tabWidth;
    double cellHeight = static_cast<double>(height())/tabHeight;

    for(int i = 0; i < tabWidth; i++){
        for(int j = 0; j < tabHeight; j++){
            if(previousTab[i][j].state == 1){
                qreal left = (qreal)(cellWidth*i - cellWidth);
                qreal top = (qreal)(cellHeight*j - cellHeight);
                QRectF r(left, top, (qreal)cellWidth, (qreal)cellHeight);

                if(previousTab[i][j].seedId == 1000) //TODO - refactor
                {
                    p.fillRect(r,QBrush(QColor(0,0,0 )));
                    this->image.setPixelColor(i, j, this->seedIdToColorMap[previousTab[i][j].seedId]);
                } else{
                    p.fillRect(r,QBrush(this->seedIdToColorMap[previousTab[i][j].seedId]));
                    this->image.setPixelColor(i, j, this->seedIdToColorMap[previousTab[i][j].seedId]);
                }

            }
        }
    }

}

void Space::mouseMoveEvent(QMouseEvent *e)
{
    double cellWidth = static_cast<double>(width())/tabWidth;
    double cellHeight = static_cast<double>(height())/tabHeight;

    int i = floor(e->y()/cellHeight)+1;
    int j = floor(e->x()/cellWidth)+1;
    if(previousTab[i][j].state == 0)
    {
        previousTab[i][j].state = 1;
        previousTab[i][j].seedId = 0;
        update();
    }
}

void Space::mousePressEvent(QMouseEvent *e)
{
    double cellWidth = static_cast<double>(width())/tabWidth;
    double cellHeight = static_cast<double>(height())/tabHeight;

    int i = floor(e->y()/cellHeight)+1;
    int j = floor(e->x()/cellWidth)+1;
    if(previousTab[i][j].state == 0)
    {
        previousTab[i][j].state = 1;
        previousTab[i][j].seedId = 0;
        update();
    }
    else
    {
        previousTab[i][j].state = 0;
        previousTab[i][j].seedId = -1;
        update();
    }

}
