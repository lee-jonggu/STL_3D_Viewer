#include "cutform.h"
#include "ui_cutform.h"

CutForm::CutForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CutForm)
{
    ui->setupUi(this);
}

CutForm::~CutForm()
{
    delete ui;
}
