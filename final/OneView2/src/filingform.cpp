#include "filingform.h"
#include "ui_filingform.h"

FilingForm::FilingForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FilingForm)
{
    ui->setupUi(this);
}

FilingForm::~FilingForm()
{
    delete ui;
}
