#include "occlusion.h"
#include "ui_occlusion.h"

Occlusion::Occlusion(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Occlusion)
{
    ui->setupUi(this);
    connect(ui->AmbientRadiusSlider, &QSlider::valueChanged, this, [this](int radius) { emit AmbientRadius(radius); });
    connect(ui->AmbientBiasSlider, &QSlider::valueChanged, this, [this](int bias) { emit AmbientBias(bias); });
    connect(ui->AmbientLernelSlider, &QSlider::valueChanged, this, [this](int Lerne) { emit AmbientLerne(Lerne); });
   // connect(ui->AmbientBlurSlider, &QSlider::valueChanged, this, [this](int blur) { emit AmbientBlur(blur); });
    connect(ui->OnpushButton, &QPushButton::clicked, this, [this] () {emit AmbientBlurOn(); });
    connect(ui->OffpushButton, &QPushButton::clicked, this, [this]() {emit AmbientBlurOff(); });

}

Occlusion::~Occlusion()
{
    delete ui;
}


