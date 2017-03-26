#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "equiretangularprojection.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    EquiretangularProjection* ep;
private slots:
    void on_isSphere_checkBox_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
