#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class Doctor;
class STLViewer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionDoctor_triggered();
    void on_actionStlViewer_triggered();

private:
    Ui::MainWindow *ui;
    Doctor *doctor;
    STLViewer *stlviewer;
};

#endif // MAINWINDOW_H
