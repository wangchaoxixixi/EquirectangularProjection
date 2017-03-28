#include "mainwindow.h"
#include "ui_mainwindow.h"
#define PI 3.1415926
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ep = new EquiretangularProjection;
    ui->scrollArea->setWidget(ep);
    ui->scale_horizontalSlider->setMinimum(1);
    ui->scale_horizontalSlider->setMaximum(300);
    ui->angle_horizontalSlider->setMinimum(0);
    ui->angle_horizontalSlider->setMaximum(360);
    ui->angle_horizontalSlider->setValue(180);
    ui->scale_horizontalSlider->setValue(50);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::on_isSphere_checkBox_clicked()
{
    if(ui->isSphere_checkBox->checkState() == Qt::Checked)
    {
        ep->drawSphere = true;
    }
    else
    {
        ep->drawSphere = false;
    }
    ep->update();
}
void MainWindow::on_scale_horizontalSlider_valueChanged(int value)
{
    ep->scale = value / 50.0;
    ep->update();
}
void MainWindow::on_angle_horizontalSlider_valueChanged(int value)
{
    ep->angle = value;
    ep->update();
}
