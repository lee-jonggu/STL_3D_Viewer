#include "lightwidgetform.h"
#include "ui_lightwidgetform.h"

LightWidgetForm::LightWidgetForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LightWidgetForm)
{
    ui->setupUi(this);

    connect(ui->AmbientSlider, &QSlider::valueChanged, this, [this](int ambient) { emit ambientValue(ambient); });
    connect(ui->DiffuseSlider, &QSlider::valueChanged, this, [this](int diffuse) { emit diffuseValue(diffuse); });
    connect(ui->SpecularSlider, &QSlider::valueChanged, this, [this](int specular) { emit specularValue(specular); });
    connect(ui->SpotColorButton, &QPushButton::clicked, this, [this]() { emit spotValue(); });
    connect(ui->XradioButton, &QRadioButton::clicked, this, [this]() { emit lightX(); });
    connect(ui->YradioButton, &QRadioButton::clicked, this, [this]() { emit lightY(); });
    connect(ui->ZradioButton, &QRadioButton::clicked, this, [this]() { emit lightZ(); });
    connect(ui->IntensityLightSlider, &QSlider::valueChanged, this, [this](int intensity) { emit Intensity(intensity); });
    connect(ui->AmbientColorButton, &QPushButton::clicked, this, [this]() { emit AmbientlightcolorChange(); });
    connect(ui->DiffuseColorButton, &QPushButton::clicked, this, [this]() { emit DiffuselightcolorChange(); });
    connect(ui->SpecularColorButton, &QPushButton::clicked, this, [this]() { emit SpecularlightcolorChange(); });
    connect(ui->OnPushButton, &QPushButton::clicked, this, [this]() {emit SpotOn(); });
    connect(ui->OffPushButton, &QPushButton::clicked, this, [this]() {emit SpotOff(); });
}

LightWidgetForm::~LightWidgetForm()
{
    delete ui;
}
