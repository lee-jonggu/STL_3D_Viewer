#ifndef SHADINGFORM_H
#define SHADINGFORM_H

#include <QWidget>

namespace Ui {
class ShadingForm;
}

class ShadingForm : public QWidget
{
    Q_OBJECT

public:
    explicit ShadingForm(QWidget *parent = nullptr);
    ~ShadingForm();

private:
    Ui::ShadingForm *ui;

signals:
    void FlatButton();
    void GouraudButton();
    void PhongButton();
    void TextureButton();
};

#endif // SHADINGFORM_H
