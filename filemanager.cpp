#include "filemanager.h"

#include <iostream>
#include <QDir>
#include <QFile>
#include <QDebug>
#include<QMessageBox>

FileManager::FileManager()
{
    this->fileName = "default_name";
}

void FileManager::saveToTxtFile(Seed **array, int tabWidth, int tabHeight)
{
    QMessageBox msgBox;

    QFile file(this->fileName + ".txt");

    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream txtStream(&file);

        txtStream << tabWidth << " " << tabHeight << "\n";

        for(int i = 0; i < tabWidth; ++i)
        {
            for(int j = 0; j < tabHeight; ++j)
            {
                txtStream << i << " " << j << " " << array[i][j].state << " " << array[i][j].seedId << "\n";
            }
        }

        msgBox.setText("Zapisano do pliku tekstowego: " + this->fileName + ".txt");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.exec();

    }
    else
    {
        qDebug() << "Could not open the file";
        msgBox.setText("Nie mozna utworzyc pliku tekstowego: " + this->fileName + ".txt");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
    }

}

void FileManager::loadFromTxtFile(Seed **array, std::function<void(int)> setTabWidth, std::function<void(int)> setTabHeight)
{
    QMessageBox msgBox;

    QFile file(this->fileName + ".txt");

    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream txtStream(&file);

        QString dimensionsTmp = txtStream.readLine();
        QStringList dimensions = dimensionsTmp.split(' ');
        qDebug() << dimensions;


        int tabWidth = dimensions[0].toInt();
        int tabHeight = dimensions[1].toInt();

        setTabWidth(tabWidth);
        setTabHeight(tabHeight);

        while(!txtStream.atEnd())
        {
            QString seedParamsStr = txtStream.readLine();
            QStringList seedParamsList = seedParamsStr.split(' ');

            int x = seedParamsList[0].toInt();
            int y = seedParamsList[1].toInt();
            int state = seedParamsList[2].toInt();
            int seedId = seedParamsList[3].toInt();

            array[x][y].state = state;
            array[x][y].seedId = seedId;
        }


    }
    else
    {
        qDebug() << "Could not open the file";
        msgBox.setText("Nie mozna otworzyc pliku tekstowego: " + this->fileName + ".txt");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
    }
}

void FileManager::saveToBitMap(QImage image)
{
    QMessageBox msgBox;

    QFile file(this->fileName + ".png");

    if(file.open(QIODevice::WriteOnly))
    {
        image.save(&file, "PNG");

        msgBox.setText("Zapisano do bitmapy: " + this->fileName + ".png");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.exec();

    }
    else
    {
        qDebug() << "Could not open the file";
        msgBox.setText("Nie mozna utworzyc bitmapy: " + this->fileName + ".png");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
    }

}

void FileManager::loadFromBitMap(Seed **array, std::function<void(int)> setTabWidth, std::function<void(int)> setTabHeight,
                                 std::map<QColor, int, std::function<bool(const QColor&, const QColor&)>> &colorToSeedIdMap)
{
    QMessageBox msgBox;

    QFile file(this->fileName + ".png");
    QImage tmp_image;

    if(file.open(QIODevice::ReadOnly))
    {
        tmp_image.load(&file, "PNG");

        setTabWidth(tmp_image.width());
        setTabHeight(tmp_image.height());


        for(int i = 0; i < tmp_image.width(); ++i)
        {
            for(int j = 0; j < tmp_image.height(); ++j)
            {
                array[i][j].state = 1;
                array[i][j].seedId = colorToSeedIdMap[tmp_image.pixelColor(i,j)];
            }
        }

    }
    else
    {
        qDebug() << "Could not open the file";
        msgBox.setText("Nie mozna otworzyc bitmapy: " + this->fileName + ".png");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
    }

}

void FileManager::setFileType(FileType fileType)
{
    this->fileType = fileType;
}

FileType FileManager::getFileType()
{
    return this->fileType;
}

void FileManager::setFileName(QString fileName)
{
    this->fileName = fileName;
}

QString FileManager::getFileName()
{
    return this->fileName;
}


