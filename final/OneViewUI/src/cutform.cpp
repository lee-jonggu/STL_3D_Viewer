#include "cutform.h"
#include "ui_cutform.h"

CutForm::CutForm(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::CutForm)
{
    ui->setupUi(this);

}

CutForm::~CutForm()
{
    delete ui;
}

void CutForm::on_pathToolButton_clicked()
{
    emit clickPathButton();
}

void CutForm::on_connectToolButton_clicked()
{
    emit clickConnectButton();
}

void CutForm::on_hideToolButton_clicked()
{
    emit clickHideButton(); 
}

void CutForm::on_colorToolButton_clicked()
{
    emit clickColorButton();
}