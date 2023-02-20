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
    void diffuseValue(int);
    void specularValue(int);
    void spotValue();
    void lightX();
    void lightY();
    void lightZ();
    void Intensity(int);
    void AmbientlightcolorChange();
    void DiffuselightcolorChange();
    void SpecularlightcolorChange();
    void SpotOn();
    void SpotOff();
};



#endif // LIGHTWIDGETFORM_H
