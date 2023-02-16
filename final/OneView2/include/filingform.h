#ifndef FILINGFORM_H
#define FILINGFORM_H

#include <QWidget>

namespace Ui {
class FilingForm;
}

class FilingForm : public QWidget
{
    Q_OBJECT

public:
    explicit FilingForm(QWidget *parent = nullptr);
    ~FilingForm();

private:
    Ui::FilingForm *ui;
};

#endif // FILINGFORM_H
