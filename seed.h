#ifndef SEED_H
#define SEED_H
#include <QColor>

class Seed
{
public:
    Seed();
    int seedId;
    int state;
    bool checked;
    QColor seedColor;
};

#endif // SEED_H
