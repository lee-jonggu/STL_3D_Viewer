#ifndef LIGHTWIDGETFORM_H
#define LIGHTWIDGETFORM_H

#include <QWidget>

namespace Ui {
class LightWidgetForm;
}

class LightWidgetForm : public QWidget
{
    Q_OBJECT

public:
    explicit LightWidgetForm(QWidget *parent = nullptr);
    ~LightWidgetForm();

private:
    Ui::LightWidgetForm *ui;

signals:
    void ambientValue(int);
};



#endif // LIGHTWIDGETFORM_H
