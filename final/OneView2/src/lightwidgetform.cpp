#include "lightwidgetform.h"
#include "ui_lightwidgetform.h"

LightWidgetForm::LightWidgetForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LightWidgetForm)
{
    ui->setupUi(this);
}

LightWidgetForm::~LightWidgetForm()
{
    delete ui;
}
