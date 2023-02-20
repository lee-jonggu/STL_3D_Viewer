#include "shadingform.h"
#include "ui_shadingform.h"

ShadingForm::ShadingForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShadingForm)
{
    ui->setupUi(this);

    connect(ui->FlattoolButton, &QToolButton::clicked, this, [this]() {emit FlatButton(); });
    connect(ui->GouraudtoolButton, &QToolButton::clicked, this, [this]() {emit GouraudButton(); });
    connect(ui->PhongtoolButton, &QToolButton::clicked, this, [this]() {emit PhongButton(); });
    connect(ui->TexturetoolButton, &QToolButton::clicked, this, [this]() {emit TextureButton(); });
}

ShadingForm::~ShadingForm()
{
    delete ui;
}
