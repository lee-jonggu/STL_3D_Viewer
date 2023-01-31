#ifndef STLVIEWER_H
#define STLVIEWER_H

#include <QMainWindow>
#include <vtkSTLReader.h>
#include <vtkMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <QColorDialog.h>
#include "CustomVTKWidget.h"
#include <vtkPolyData.h>
#include <vtkAxesActor.h> 
#include <vtkTransform.h> 
#include <QSlider>
#include <QResizeEvent>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class STLViewer; }
QT_END_NAMESPACE

class STLViewer : public QMainWindow
{
    Q_OBJECT

public:
    STLViewer(QWidget *parent = nullptr);
    ~STLViewer();

protected:
    virtual void resizeEvent(QResizeEvent* event) override;

private:
    Ui::STLViewer *ui;

    CustomVTKWidget* customVTKWidget;

    vtkSmartPointer<vtkActor> mActor; 
    
    QColorDialog* mColorDialog;   
    QSlider* mSlider;  

signals:

private slots:
    void ClickedOpen(bool);                         // Menu -> Open 
    void SetColor(QColor);                          // Actor Color Change
    void SetOpacity(int);                           // Acotr Opacity Change

};
#endif // STLVIEWER_H
