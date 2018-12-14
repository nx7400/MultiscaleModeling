#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), space(new Space(this))
{
    ui->setupUi(this);

    connect(ui->startButton, SIGNAL(clicked()), space, SLOT(start()));
    connect(ui->stopButton, SIGNAL(clicked()), space, SLOT(stop()));
    connect(ui->clearButton, SIGNAL(clicked()), space, SLOT(clear()));

    ui->spaceLayout->addWidget(space);

    ui->tabWidthSpinBox->setMinimum(10);
    ui->tabWidthSpinBox->setMaximum(300);
    ui->tabHeightSpinBox->setMinimum(10);
    ui->tabHeightSpinBox->setMaximum(300);
    ui->generationSpinBox->setMinimum(1);
    ui->generationSpinBox->setMaximum(1000);

    ui->tabWidthSpinBox->setValue(space->getTabWidth());
    ui->tabHeightSpinBox->setValue(space->getTabHeight());
    ui->generationSpinBox->setValue(space->getNubmerOfGenerations());

    ui->comboBox->addItem("Moore");
    ui->comboBox->addItem("von Neuman");
    ui->comboBox->addItem("Heksagonalne prawe");
    ui->comboBox->addItem("Heksagonalne lewe");
    ui->comboBox->addItem("Heksagonalne losowe");
    ui->comboBox->addItem("Extension Of Moore");

    ui->comboBox_2->addItem("Pochłaniające");
    ui->comboBox_2->addItem("Periodyczne");

    ui->fileTypeComboBox->addItem("Tekstowy");
    ui->fileTypeComboBox->addItem("Bitmapa");


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_tabWidthSpinBox_editingFinished()
{
    space->setTabWidth(ui->tabWidthSpinBox->value());
    update();
}

void MainWindow::on_tabHeightSpinBox_editingFinished()
{
    space->setTabHeight(ui->tabHeightSpinBox->value());
    update();
}

void MainWindow::on_generationSpinBox_editingFinished()
{
    space->setNumberOfGenerations(ui->generationSpinBox->value());
    update();
}

void MainWindow::on_numberOfSeedSpinBox_editingFinished()
{
    space->setNumberOfSeed(ui->numberOfSeedSpinBox->value());
    update();
}


void MainWindow::on_randSeedButton_clicked()
{
    space->randomSeed();
    update();
}

void MainWindow::on_comboBox_activated(int index)
{
    space->setNeighborhoodType(index);
}


void MainWindow::on_comboBox_2_activated(int index)
{
    space->setBCType(index);
}

void MainWindow::on_regularSeedButton_clicked()
{
    space->regularSeed();
    update();
}

void MainWindow::on_pushButton_clicked()
{
    space->gradientSeed();
    update();
}

void MainWindow::on_pushButton_2_clicked()
{
    space->randomWithRadiusSeed();
}

void MainWindow::on_seedRadiusSpinBox_editingFinished()
{
    space->setSeedRadius(ui->seedRadiusSpinBox->value());
    update();
}

void MainWindow::on_pushButton_3_clicked()
{
    space->holesOnSeedBorder();
}

void MainWindow::on_numberOfHolesSpinBox_editingFinished()
{
    space->setNumberOfHoles(ui->numberOfHolesSpinBox->value());
    update();
}

void MainWindow::on_saveFileButton_clicked()
{
    space->saveToFile();
}

void MainWindow::on_fileTypeComboBox_activated(int index)
{
    space->setFileType(index);
}

void MainWindow::on_fileName_textEdited(const QString &arg1)
{
    space->getFileManager()->setFileName(arg1);
}

void MainWindow::on_fileName_editingFinished()
{

}

void MainWindow::on_loadFileButton_clicked()
{
    space->loadFromFile();
}

void MainWindow::on_randomHolesButton_clicked()
{
    space->randomHoles();
    update();
}
