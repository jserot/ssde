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
#include "fsd.h"
#include "mainwindow.h"

#include <QtWidgets>
#include <QFile>
#include <QTextStream>

QString MainWindow::title = "FSD Editor";

MainWindow::MainWindow()
{
    createActions();
    createMenus();

    fsd = new Fsd(this);
    fsd->setSceneRect(QRectF(0, 0, 400, 600));
    // connect(fsd, SIGNAL(stateInserted(State*)), this, SLOT(stateInserted(State*)));
    // connect(fsd, SIGNAL(transitionInserted(Transition*)), this, SLOT(transitionInserted(Transition*)));
    connect(fsd, SIGNAL(stateSelected(State*)), this, SLOT(stateSelected(State*)));
    connect(fsd, SIGNAL(transitionSelected(Transition*)), this, SLOT(transitionSelected(Transition*)));
    connect(fsd, SIGNAL(fsdModified()), this, SLOT(fsdModified()));
    createToolbar();

    QHBoxLayout *layout = new QHBoxLayout;
    view = new QGraphicsView(fsd);
    view->setMinimumWidth(200);
    view->setMinimumHeight(400);
    createPropertiesPanel();
    layout->addWidget(view);
    layout->addWidget(properties_panel);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    setCentralWidget(widget);
    setWindowTitle(title);
    setUnifiedTitleAndToolBarOnMac(true);

    unsaved_changes = false;
}

void MainWindow::toolButtonClicked(int)
{
    fsd->setMode(Fsd::Mode(toolSet->checkedId()));
}

// void MainWindow::stateInserted(State *state)
// {
// }

// void MainWindow::transitionInserted(Transition *transition)
// {
// }


void MainWindow::stateSelected(State *state)
{
  properties_panel->setSelectedItem(state);
}

void MainWindow::transitionSelected(Transition *transition)
{
  properties_panel->setSelectedItem(transition);
}

void MainWindow::fsdModified()
{
  setUnsavedChanges(true);
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About SSDE"), tr("Finite State Diagram Editor\n(C) J. Sérot, 2019\ngithub.com/jserot/ssde\njocelyn.serot@uca.fr"));
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

    exportDotAction = new QAction(tr("E&xport to DOT"), this);
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

    exportMenu = menuBar()->addMenu(tr("&Export"));
    exportMenu->addAction(exportDotAction);
}

void MainWindow::createToolbar()
{
    QToolButton *selectButton = new QToolButton;
    selectButton->setCheckable(true);
    selectButton->setChecked(true);
    selectButton->setIcon(QIcon(":/images/select.png"));
    selectButton->setToolTip("Select or move items");
    QToolButton *addStateButton = new QToolButton;
    addStateButton->setCheckable(true);
    addStateButton->setIcon(QIcon(":/images/state.png"));
    addStateButton->setToolTip("Add state");
    QToolButton *addPseudoStateButton = new QToolButton;
    addPseudoStateButton->setCheckable(true);
    addPseudoStateButton->setIcon(QIcon(":/images/initstate.png"));
    addPseudoStateButton->setToolTip("Add initial transition");
    QToolButton *addTransitionButton = new QToolButton;
    addTransitionButton->setCheckable(true);
    addTransitionButton->setIcon(QIcon(":/images/transition.png"));
    addTransitionButton->setToolTip("Add transition between two states");
    QToolButton *addLoopTransitionButton = new QToolButton;
    addLoopTransitionButton->setCheckable(true);
    addLoopTransitionButton->setIcon(QIcon(":/images/loop.png"));
    addLoopTransitionButton->setToolTip("Add self transition");
    QToolButton *deleteButton = new QToolButton;
    deleteButton->setCheckable(true);
    deleteButton->setIcon(QIcon(":/images/delete.png"));
    deleteButton->setToolTip("Delete items");

    toolSet = new QButtonGroup(this);
    toolSet->addButton(selectButton, int(Fsd::SelectItem));
    toolSet->addButton(addStateButton, int(Fsd::InsertState));
    toolSet->addButton(addPseudoStateButton, int(Fsd::InsertPseudoState));
    toolSet->addButton(addTransitionButton, int(Fsd::InsertTransition));
    toolSet->addButton(addLoopTransitionButton, int(Fsd::InsertLoopTransition));
    toolSet->addButton(deleteButton, int(Fsd::DeleteItem));
    connect(toolSet, SIGNAL(buttonClicked(int)), this, SLOT(toolButtonClicked(int)));

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
  fsd->fromString(txt);
  properties_panel->clear();
  currentFileName = fname;
  setUnsavedChanges(false);
}

void MainWindow::newDiagram()
{
  checkUnsavedChanges();
  fsd->clear();
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
  os << fsd->toString();
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
  fsd->exportDot(fname);
}

void MainWindow::quit()
{
    checkUnsavedChanges();
    close();
}
