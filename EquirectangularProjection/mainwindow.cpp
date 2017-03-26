#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ep = new EquiretangularProjection;
    ui->scrollArea->setWidget(ep);
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
