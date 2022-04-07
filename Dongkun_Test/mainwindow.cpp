#include <vtkSmartPointer.h>
#include <vtkCubeSource.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkCamera.h>
#include <vtkPolyData.h>
#include <vtkDataSetMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include "vtkSTLReader.h"
#include <vtkSTLWriter.h>
#include "vtkPolyDataMapper.h"

#include "vtkClipPolyData.h"
#include <vtkLight.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QString>
#include <QColorDialog>
#include <QDebug>
#include <QMessageBox>
#include <QPalette>
#include <typeinfo>
#include <QRadioButton>
#include <QLineEdit>
#include <QLine>
#include <ModelRender.h>
#include <QMessageBox>
#include <QGraphicsBlurEffect>
#include "vtkAutoInit.h"
#include "form.h"

VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{

    setWindowOpacity(1);
    QGraphicsBlurEffect* ef = new QGraphicsBlurEffect;
    ef->setBlurRadius(0);
    ef->setBlurHints(QGraphicsBlurEffect::AnimationHint);
    this->setGraphicsEffect(ef);

    this->UiSetup();
    this->InitOpenGLWindow();
    this->filterFunctionConnect();
    this->BuildPreLoad();
    this->PositionChange();

    //this->setStyleSheet(":/Icons/B.jpg");

    renderer->SetBackground(BR,BG,BB);
    renderWindow->Render();

    connect(ui->actionFileOpen, &QAction::triggered, this, &MainWindow::readSTL);
    connect(ui->CalcA,&QPushButton::clicked,this,&MainWindow::CalcA);
    connect(ui->CalcV,&QPushButton::clicked,this,&MainWindow::CalcV);
    connect(ui->axes,&QPushButton::clicked,this,&MainWindow::axes);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::UiSetup()
{
    ui->setupUi(this);
    this->setWindowTitle("Group 3 D Viewer");
}

void MainWindow::InitOpenGLWindow()
{
    ui->qvtkWidget->SetRenderWindow(renderWindow);
    ui->qvtkWidget->GetRenderWindow()->AddRenderer(renderer);
}

void MainWindow::BuildPreLoad()
{
    connect(ui->ball, &QAction::triggered, this, &MainWindow::BuildSphere);
    connect(ui->arrow, &QAction::triggered, this, &MainWindow::BuildArrow);
    connect(ui->cone, &QAction::triggered, this, &MainWindow::BuildCone);

    connect(ui->AColor, &QAction::triggered, this, &MainWindow::setActorColor);
    connect(ui->BColor, &QAction::triggered, this, &MainWindow::setBackgndColor);
}

void MainWindow::filterFunctionConnect()
{
    connect(ui->smooth,&QPushButton::clicked,this,&MainWindow::LaunchSmoothFilter);
    connect(ui->smoothadd,&QPushButton::clicked,this,&MainWindow::LaunchSmoothFilteradd);
    connect(ui->Cam,&QPushButton::released,this,&::MainWindow::handlCam);
    connect(ui->outline,&QPushButton::clicked,this,&MainWindow::LaunchOutLineFilter);
    connect(ui->clip,&QPushButton::clicked,this,&MainWindow::LaunchClipFilter);
    connect(ui->shrink,&QPushButton::clicked,this,&MainWindow::LaunchShrinkFilter);
    connect(ui->reflection,&QPushButton::clicked,this,&MainWindow::LaunchReflectFilter);
    connect(ui->Remove,&QPushButton::clicked,this,&MainWindow::RemoveFilter);
}

void MainWindow::PositionChange()
{
    //ui->changeX->setTracking(true);
    //ui->changeY->setTracking(true);
    //ui->changeZ->setTracking(true);
    connect(ui->changeX,&QSlider::valueChanged,this,&MainWindow::changeX);
    connect(ui->changeY,&QSlider::valueChanged,this,&MainWindow::changeY);
    connect(ui->changeZ,&QSlider::valueChanged,this,&MainWindow::changeZ);

    connect(ui->RX,&QSlider::valueChanged,this,&MainWindow::changeRX);
    connect(ui->RY,&QSlider::valueChanged,this,&MainWindow::changeRY);
    connect(ui->RZ,&QSlider::valueChanged,this,&MainWindow::changeRZ);
}

void MainWindow::BuildCone(){
    ModelRender* ModelNew = new ModelRender();
    ModelNew->buildCone(renderer);
    renderer->SetBackground(BR,BG,BB);
    renderWindow->Render();
    ModelList->push_back(*ModelNew);
    FileName.push_back("Cone");
    upDateList();
}

void MainWindow::BuildSphere(){
    ModelRender* ModelNew = new ModelRender();
    ModelNew->buildSphere(renderer);
    renderWindow->Render();
    ModelList->push_back(*ModelNew);
    FileName.push_back("Sphere");
    upDateList();
}

void MainWindow::BuildArrow(){
    ModelRender* ModelNew = new ModelRender();
    ModelNew->buildArrow(renderer);
    renderWindow->Render();
    ModelList->push_back(*ModelNew);
    FileName.push_back("Arrow");
    upDateList();
}

void MainWindow::setActorColor(){
    Model->setActorColor();
    renderWindow->Render();
    //Model->RenderingStarts(renderer);
}

void MainWindow::setBackgndColor(){
    QColor ColourDialog = QColorDialog::getColor();
    BR=ColourDialog.redF();
    BG=ColourDialog.greenF();
    BB=ColourDialog.blueF();
    renderer->SetBackground(BR,BG,BB);
    renderWindow->Render();
    //Model->RenderingStarts(renderer);
}

void MainWindow::LaunchReflectFilter()
{
   vtkFilter->reflect(Model,renderer);
   renderWindow->Render();
   ui->statusbar->showMessage(tr("Launch Reflect Filter"),2000);
}

void MainWindow::LaunchOutLineFilter()
{
   vtkFilter->outLine(Model,renderer,x,y,z);
   renderWindow->Render();
   ui->statusbar->showMessage(tr("Launch OutLine Filter"),2000);
}

void MainWindow::LaunchSmoothFilter()
{
    vtkFilter->smooth(Model,renderer);
    renderWindow->Render();
    ui->statusbar->showMessage(tr("Launch Smooth Filter independent"),2000);
}

void MainWindow::LaunchSmoothFilteradd()
{
    vtkFilter->smoothadd(Model,renderer);
    renderWindow->Render();
    ui->statusbar->showMessage(tr("Launch Smooth Filter add on"),2000);
}

void MainWindow::LaunchShrinkFilter()
{
    vtkFilter->shrinkFilter(Model,renderer);
    renderWindow->Render();
    ui->statusbar->showMessage(tr("Launch Shrink Filter"),2000);
}

void MainWindow::LaunchClipFilter()
{
    vtkFilter->clipFilter(Model,renderer);
    renderWindow->Render();
    ui->statusbar->showMessage(tr("Launch Cliip Filter"),2000);
}

void MainWindow::RemoveFilter()
{
    vtkFilter->RemoveFilter(Model,renderer);
    renderWindow->Render();
    ui->statusbar->showMessage(tr("Remove Filter"),2000);
}

void MainWindow::handlCam()
{
    renderer->ResetCamera();
    renderer->GetActiveCamera()->Azimuth(30);
    renderer->GetActiveCamera()->Elevation(30);
    renderer->ResetCameraClippingRange();
    renderWindow->Render();
}

void MainWindow::readSTL()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Open STL File"), "./", tr("STL Files(*.stl)"));
    Model->STLfileReader(file);
    Model->RenderingStarts(renderer);
    ModelList->push_back(*Model);
    FileName.push_back(file);
    upDateList();
}

void MainWindow::axes()
{
    Model->showaxes(renderer);
    renderWindow->Render();
    ui->statusbar->showMessage(tr("Show Axes"),2000);
}

void MainWindow::CalcA()
{
    double area = vtkFilter->calcA(Model);
    QMessageBox msgBox;
    msgBox.setText(tr("The area is:%1").arg(area));
    msgBox.exec();
}

void MainWindow::CalcV()
{
    double vol = vtkFilter->calcV(Model);
    QMessageBox msgBox;
    msgBox.setText(tr("The volume is:%1").arg(vol));
    msgBox.exec();
}

void MainWindow::changeX()
{
    x = ui->changeX->value();
    Model->changeX(x,renderer);
    renderWindow->Render();
    ui->statusbar->showMessage(tr("Position Change X"),2000);
}

void MainWindow::changeY()
{
    y = ui->changeY->value();
    Model->changeY(y,renderer);
    renderWindow->Render();
    ui->statusbar->showMessage(tr("Position Change Y"),2000);
}

void MainWindow::changeZ()
{
    z = ui->changeZ->value();
    Model->changeZ(z,renderer);
    renderWindow->Render();
    ui->statusbar->showMessage(tr("Position Change Z"),2000);
}

void MainWindow::changeRX()
{
    float x;
    x = ui->RX->value()/60;
    Model->changeRX(x,renderer);
    renderWindow->Render();
    ui->statusbar->showMessage(tr("Angle Change X"),2000);
}

void MainWindow::changeRY()
{
    float y;
    y = ui->RY->value()/60;
    Model->changeRY(y,renderer);
    renderWindow->Render();
    ui->statusbar->showMessage(tr("Angle Change Y"),2000);
}

void MainWindow::changeRZ()
{
    float z;
    z = ui->RZ->value()/60;
    Model->changeRZ(z,renderer);
    renderWindow->Render();
    ui->statusbar->showMessage(tr("Angle Change Z"),2000);
}

void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    ui->tabWidget->removeTab(index);
}

void MainWindow::on_actionAdd_triggered()
{
    ui->tabWidget->addTab(new Form() , QString("Tab %0").arg(ui->tabWidget->count() + 1));
    ui->tabWidget->setCurrentIndex(ui->tabWidget->count() - 1);
}

void MainWindow::on_clear_clicked()
{
    ui->list->clear();
    ModelList->clear();
}

void MainWindow::upDateList()
{
    ui->list->clear();
    QString name;
    for(int i=0; i<ModelList->size(); i++)
    {
        name = FileName[i];
        ui->list->addItem(name);
    }
}

void MainWindow::on_Test_clicked()
{
    renderWindow->Render();
}

void MainWindow::on_list_itemSelectionChanged()
{
    int row;
    row = ui->list->currentRow();

    ModelList->at(0).setActorColor();

    renderWindow->Render();
    ui->statusbar->showMessage(tr("Selection Changed"),2000);
}















