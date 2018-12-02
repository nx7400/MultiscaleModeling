#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <functional>
#include <map>
#include <QString>
#include <QImage>
#include <QColor>

#include "seed.h"


enum FileType {
    TXT,
    BITMAPP
};

class FileManager
{
public:
    FileManager();
    void saveToTxtFile(Seed **array, int tabWidth, int tabHeight);
    void loadFromTxtFile(Seed **array, std::function<void(int)> setTabWidth, std::function<void(int)> setTabHeight);
    void saveToBitMap(QImage image);
    void loadFromBitMap(Seed **array, std::function<void(int)> setTabWidth, std::function<void(int)> setTabHeight, std::map<QColor, int, std::function<bool(const QColor&, const QColor&)>> &colorToSeedIdMap);

    void setFileType(FileType fileType);
    FileType getFileType();
    void setFileName(QString fileName);
    QString getFileName();
private:
    FileType fileType;
    QString fileName;
};

#endif // FILEMANAGER_H
