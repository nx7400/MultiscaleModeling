#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <space.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_tabWidthSpinBox_editingFinished();

    void on_generationSpinBox_editingFinished();

    void on_numberOfSeedSpinBox_editingFinished();

    void on_tabHeightSpinBox_editingFinished();

    void on_randSeedButton_clicked();

    void on_comboBox_activated(int index);

    void on_comboBox_2_activated(int index);

    void on_regularSeedButton_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_seedRadiusSpinBox_editingFinished();

    void on_pushButton_3_clicked();

    void on_numberOfHolesSpinBox_editingFinished();

    void on_saveFileButton_clicked();

    void on_fileTypeComboBox_activated(int index);

    void on_fileName_editingFinished();

    void on_fileName_textEdited(const QString &arg1);

    void on_loadFileButton_clicked();

    void on_randomHolesButton_clicked();

    void on_spinBox_editingFinished();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

private:
    Ui::MainWindow *ui;
    Space* space;
};

#endif // MAINWINDOW_H
