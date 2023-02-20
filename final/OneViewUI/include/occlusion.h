#ifndef OCCLUSION_H
#define OCCLUSION_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Occlusion; }
QT_END_NAMESPACE

class Occlusion : public QWidget
{
    Q_OBJECT

public:
    Occlusion(QWidget *parent = nullptr);
    ~Occlusion();

signals:
    void AmbientRadius(int);
    void AmbientBias(int);
    void AmbientLerne(int);
    void AmbientBlurOn();
    void AmbientBlurOff();

private:
    Ui::Occlusion *ui;
};
#endif // OCCLUSION_H
