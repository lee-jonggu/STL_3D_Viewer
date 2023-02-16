#include "shadingform.h"
#include "ui_shadingform.h"

ShadingForm::ShadingForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShadingForm)
{
    ui->setupUi(this);
}

ShadingForm::~ShadingForm()
{
    delete ui;
}
