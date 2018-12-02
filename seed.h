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
    int energy;
    QColor seedColor;
};

#endif // SEED_H
