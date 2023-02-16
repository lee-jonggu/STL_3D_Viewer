#ifndef CUTFORM_H
#define CUTFORM_H

#include <QWidget>

namespace Ui {
class CutForm;
}

class CutForm : public QWidget
{
    Q_OBJECT

public:
    explicit CutForm(QWidget *parent = nullptr);
    ~CutForm();

private:
    Ui::CutForm *ui;
};

#endif // CUTFORM_H
