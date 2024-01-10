/***********************************************************************/
/*                                                                     */
/* This file is part of the SSDE (Simple State Diagram Editor) package */
/*                                                                     */
/*  Copyright (c) 2019-present, Jocelyn SEROT (jocelyn.serot@uca.fr)   */
/*                       All rights reserved.                          */
/*                                                                     */
/*    This source code is licensed under the license found in the      */
/*      LICENSE file in the root directory of this source tree.        */
/*                                                                     */
/***********************************************************************/

#include "transition.h"
#include "state.h"
#include "model.h"
#include "mainwindow.h"

#include <QtWidgets>
#include <QFile>
#include <QTextStream>

QString MainWindow::title = "FSD Editor";

int MainWindow::scene_width = 400;
int MainWindow::scene_height = 1000;
double MainWindow::zoomInFactor = 1.25;
double MainWindow::zoomOutFactor = 0.8;
double MainWindow::minScaleFactor = 0.5;
double MainWindow::maxScaleFactor = 2.0;

MainWindow::MainWindow()
{
    createActions();
    createMenus();

    model = new Model(this);
    model->setSceneRect(QRectF(0, 0, scene_width, scene_height));
    connect(model, SIGNAL(stateInserted(State*)), this, SLOT(stateInserted(State*)));
    connect(model, SIGNAL(transitionInserted(Transition*)), this, SLOT(transitionInserted(Transition*)));
    connect(model, SIGNAL(stateSelected(State*)), this, SLOT(stateSelected(State*)));
    connect(model, SIGNAL(transitionSelected(Transition*)), this, SLOT(transitionSelected(Transition*)));
    connect(model, SIGNAL(modelModified()), this, SLOT(modelModified()));
    connect(model, SIGNAL(mouseEnter()), this, SLOT(updateCursor()));
    connect(model, SIGNAL(mouseLeave()), this, SLOT(resetCursor()));
    createToolbar();

    QHBoxLayout *layout = new QHBoxLayout;

    createPropertiesPanel();
    layout->addWidget(properties_panel);

    editView = new QGraphicsView(model);
    editView->setMinimumWidth(200);
    editView->setMinimumHeight(400);
    layout->addWidget(editView);

    dotView = new QGraphicsView();
    dotView->setSceneRect(QRectF(0, 0, scene_width, scene_height));
    dotView->setMinimumWidth(200);
    dotView->setMinimumHeight(400);
    layout->addWidget(dotView);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    setCentralWidget(widget);
    setWindowTitle(title);
    setUnifiedTitleAndToolBarOnMac(true);

    unsaved_changes = false;
    scaleFactor = 1.0;
}

void MainWindow::toolButtonClicked(int)
{
  Model::Mode mode = Model::Mode(toolSet->checkedId());
  model->setMode(mode);
}

void MainWindow::stateInserted(State *state)
{
  Q_UNUSED(state);
}

void MainWindow::transitionInserted(Transition *transition)
{
  Q_UNUSED(transition);
}

void MainWindow::stateSelected(State *state)
{
  properties_panel->setSelectedItem(state);
}

void MainWindow::transitionSelected(Transition *transition)
{
  properties_panel->setSelectedItem(transition);
}

void MainWindow::modelModified()
{
  setUnsavedChanges(true);
}

QCursor cursorOf(Model::Mode mode)
{
  switch ( mode ) {
  case Model::InsertState: return QCursor(QPixmap(":cursors/state.png"),0,0);
  case Model::InsertPseudoState: return QCursor(QPixmap(":cursors/initstate.png"),0,0);
  case Model::InsertTransition: return QCursor(QPixmap(":cursors/transition.png"),0,0);
  case Model::InsertLoopTransition: return QCursor(QPixmap(":cursors/loop.png"),0,0);
  case Model::DeleteItem: return QCursor(QPixmap(":cursors/delete.png"),0,0);
  default: return Qt::ArrowCursor;
  }
}

void MainWindow::updateCursor()
{
  setCursor(cursorOf(model->getMode()));
}

void MainWindow::resetCursor()
{
  setCursor(Qt::ArrowCursor);
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About SSDE"), tr("Finite State Diagram Editor\n(C) J. Sérot\ngithub.com/jserot/ssde\njocelyn.serot@uca.fr"));
}

void MainWindow::createActions()
{
    newDiagramAction = new QAction(tr("N&ew"), this);
    newDiagramAction->setShortcuts(QKeySequence::New);
    connect(newDiagramAction, SIGNAL(triggered()), this, SLOT(newDiagram()));

    openFileAction = new QAction( "&Open", this);
    openFileAction->setShortcut(QKeySequence::Open);
    connect(openFileAction, SIGNAL(triggered()), this, SLOT(openFile()));
 
    saveFileAction = new QAction( "&Save", this);
    saveFileAction->setShortcut(QKeySequence::Save);
    connect(saveFileAction, SIGNAL(triggered()), this, SLOT(save()));
 
    saveFileAsAction = new QAction( "&Save As", this);
    saveFileAsAction->setShortcut(QKeySequence::SaveAs);
    connect(saveFileAsAction, SIGNAL(triggered()), this, SLOT(saveAs()));
 
    aboutAction = new QAction(tr("A&bout"), this);
    aboutAction->setShortcut(tr("F1"));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    exitAction = new QAction(tr("Q&uit"), this);
    exitAction->setShortcuts(QKeySequence::Quit);
    connect(exitAction, SIGNAL(triggered()), this, SLOT(quit()));

    renderDotAction = new QAction(tr("Render DOT"), this);
    renderDotAction->setShortcut(tr("Ctrl+R"));
    connect(renderDotAction, SIGNAL(triggered()), this, SLOT(renderDot()));

    zoomInAction = new QAction(tr("Zoom In"), this);
    zoomInAction->setShortcut(tr("Ctrl++"));
    connect(zoomInAction, SIGNAL(triggered()), this, SLOT(zoomIn()));

    zoomOutAction = new QAction(tr("Zoom Out"), this);
    zoomOutAction->setShortcut(tr("Ctrl+-"));
    connect(zoomOutAction, SIGNAL(triggered()), this, SLOT(zoomOut()));

    exportDotAction = new QAction(tr("E&xport to DOT"), this);
    exportDotAction->setShortcut(tr("Ctrl+E"));
    connect(exportDotAction, SIGNAL(triggered()), this, SLOT(exportDot()));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newDiagramAction);
    fileMenu->addAction(openFileAction);
    fileMenu->addAction(saveFileAction);
    fileMenu->addAction(saveFileAsAction);
    fileMenu->addAction(aboutAction);
    fileMenu->addAction(exitAction);

    dotMenu = menuBar()->addMenu(tr("&Dot"));
    dotMenu->addAction(renderDotAction);
    dotMenu->addAction(zoomInAction);
    dotMenu->addAction(zoomOutAction);
    dotMenu->addAction(exportDotAction);
}

void MainWindow::createToolbar()
{
    QToolButton *selectButton = new QToolButton;
    selectButton->setCheckable(true);
    selectButton->setChecked(true);
    selectButton->setIcon(QIcon(":/icons/select.png"));
    selectButton->setToolTip("Select or move items");
    QToolButton *addStateButton = new QToolButton;
    addStateButton->setCheckable(true);
    addStateButton->setIcon(QIcon(":/icons/state.png"));
    addStateButton->setToolTip("Add state");
    QToolButton *addPseudoStateButton = new QToolButton;
    addPseudoStateButton->setCheckable(true);
    addPseudoStateButton->setIcon(QIcon(":/icons/initstate.png"));
    addPseudoStateButton->setToolTip("Add initial transition");
    QToolButton *addTransitionButton = new QToolButton;
    addTransitionButton->setCheckable(true);
    addTransitionButton->setIcon(QIcon(":/icons/transition.png"));
    addTransitionButton->setToolTip("Add transition between two states");
    QToolButton *addLoopTransitionButton = new QToolButton;
    addLoopTransitionButton->setCheckable(true);
    addLoopTransitionButton->setIcon(QIcon(":/icons/loop.png"));
    addLoopTransitionButton->setToolTip("Add self transition");
    QToolButton *deleteButton = new QToolButton;
    deleteButton->setCheckable(true);
    deleteButton->setIcon(QIcon(":/icons/delete.png"));
    deleteButton->setToolTip("Delete items");

    toolSet = new QButtonGroup(this);
    toolSet->addButton(selectButton, int(Model::SelectItem));
    toolSet->addButton(addStateButton, int(Model::InsertState));
    toolSet->addButton(addPseudoStateButton, int(Model::InsertPseudoState));
    toolSet->addButton(addTransitionButton, int(Model::InsertTransition));
    toolSet->addButton(addLoopTransitionButton, int(Model::InsertLoopTransition));
    toolSet->addButton(deleteButton, int(Model::DeleteItem));
    connect(toolSet, SIGNAL(idClicked(int)), this, SLOT(toolButtonClicked(int)));

    toolBar = addToolBar(tr("Tools"));
    toolBar->addWidget(selectButton);
    toolBar->addWidget(addStateButton);
    toolBar->addWidget(addPseudoStateButton);
    toolBar->addWidget(addTransitionButton);
    toolBar->addWidget(addLoopTransitionButton);
    toolBar->addWidget(deleteButton);
}

void MainWindow::createPropertiesPanel()
{
    properties_panel = new PropertiesPanel(this);
    properties_panel->setMinimumWidth(180);
    properties_panel->setMaximumWidth(360);
}

void MainWindow::setUnsavedChanges(bool unsaved_changes)
{
    this->unsaved_changes = unsaved_changes;
    setWindowTitle(unsaved_changes ? title + " (Unsaved changes)" : title);
}

void MainWindow::checkUnsavedChanges()
{
    if ( unsaved_changes ) {
        QMessageBox save_message;
        save_message.setText("Do you want to save your changes?");
        save_message.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        save_message.setDefaultButton(QMessageBox::Save);
        switch( save_message.exec() ) {
            case QMessageBox::Cancel: return;
            case QMessageBox::Save: save();
            }
        }
}

void MainWindow::openFile()
{
  checkUnsavedChanges();
    
  QString fname = QFileDialog::getOpenFileName(this, "Open file", "", "FSD file (*.fsd)");
  if ( fname.isEmpty() ) return;
  QFile file(fname);
  qDebug() << "Opening file " << file.fileName();
  file.open(QIODevice::ReadOnly);
  if ( file.error() != QFile::NoError ) {
    QMessageBox::warning(this, "","Cannot open file " + file.fileName());
    return;
  }
  QTextStream is(&file);
  QString txt = is.readAll();
  model->fromString(txt);
  editView->ensureVisible(model->itemsBoundingRect());
  properties_panel->clear();
  currentFileName = fname;
  setUnsavedChanges(false);
}

void MainWindow::newDiagram()
{
  checkUnsavedChanges();
  model->clear();
  properties_panel->clear();
  currentFileName.clear();
  setUnsavedChanges(false);
}

void MainWindow::saveToFile(QString fileName)
{
  QFile file(fileName); 
  file.open(QIODevice::WriteOnly | QIODevice::Text);
  if ( file.error() != QFile::NoError ) {
    QMessageBox::warning(this, "","Cannot open file " + file.fileName());
    return;
  }
  QTextStream os(&file);
  os << model->toString();
  setUnsavedChanges(false);
}

void MainWindow::save()
{
    if ( currentFileName.isEmpty() ) saveAs();
    else saveToFile(currentFileName);
}

void MainWindow::saveAs()
{
  QString fname = QFileDialog::getSaveFileName( this, "Save to file", "", "FSD file (*.fsd)");
  if ( fname.isEmpty() ) return;
  saveToFile(fname);
}

void MainWindow::exportDot()
{
  QString fname = QFileDialog::getSaveFileName( this, "Export to DOT file", "", "DOT file (*.dot)");
  if ( fname.isEmpty() ) return;
  model->exportDot(fname);
}

void MainWindow::renderDot()
{
  model->renderDot(dotView, scene_width, scene_height);
}

void MainWindow::zoomIn()
{
  zoom(zoomInFactor);
}

void MainWindow::zoomOut()
{
  zoom(zoomOutFactor);
}

void MainWindow::zoom(double factor)
{
  dotView->scale(factor, factor);
  //dotView->ensureVisible(model->itemsBoundingRect());
  scaleFactor *= factor;
  zoomInAction->setEnabled(scaleFactor <= maxScaleFactor);
  zoomOutAction->setEnabled(scaleFactor >= minScaleFactor);
}

void MainWindow::quit()
{
    checkUnsavedChanges();
    close();
}
