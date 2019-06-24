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
#include <fstream>

#include <QtWidgets>

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
    QToolButton *addStateButton = new QToolButton;
    addStateButton->setCheckable(true);
    addStateButton->setIcon(QIcon(":/images/state.png"));
    QToolButton *addPseudoStateButton = new QToolButton;
    addPseudoStateButton->setCheckable(true);
    addPseudoStateButton->setIcon(QIcon(":/images/initstate.png"));
    QToolButton *addTransitionButton = new QToolButton;
    addTransitionButton->setCheckable(true);
    addTransitionButton->setIcon(QIcon(":/images/transition.png"));
    QToolButton *addLoopTransitionButton = new QToolButton;
    addLoopTransitionButton->setCheckable(true);
    addLoopTransitionButton->setIcon(QIcon(":/images/loop.png"));
    QToolButton *deleteButton = new QToolButton;
    deleteButton->setCheckable(true);
    deleteButton->setIcon(QIcon(":/images/delete.png"));

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
    std::ifstream file;
    std::string fname;

    checkUnsavedChanges();
    
    do {
        fname = QFileDialog::getOpenFileName(this, "Open file", "", "FSD file (*.fsd)") .toStdString();
        if(fname.empty()) return;
        file.open(fname);
        if(!file) QMessageBox::warning(this, "Error", "Unable to open file");
    } while ( !file );
    try
    {
        std::string json_data;
        // Fills json_data with the content of the whole file
        file.seekg(0, std::ios::end);
        json_data.reserve(file.tellg());
        file.seekg(0, std::ios::beg);
        json_data.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        fsd->fromString(json_data);
    }
    catch(const std::exception& e)
    {
        QMessageBox::warning(this, "Error", "Unable to import : " + QString(e.what()));
        return;
    }
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

void MainWindow::saveToFile(std::string fileName)
{
  std::ofstream file;
  file.open(fileName);
  if( !file ) QMessageBox::warning(this, "Error", "Unable to open file");
  file << fsd->toString();
  setUnsavedChanges(false);
}

void MainWindow::save()
{
    if ( currentFileName.empty() ) saveAs();
    else saveToFile(currentFileName);
}

void MainWindow::saveAs()
{
    std::string fname;
    fname = QFileDialog::getSaveFileName( this, "Save to file", "", "FSD file (*.fsd)") .toStdString();
    if ( fname.empty() ) return;
    saveToFile(fname);
}

void MainWindow::exportDot()
{
    std::string fname;
    std::ofstream file;
    fname = QFileDialog::getSaveFileName( this, "Export to DOT file", "", "DOT file (*.dot)") .toStdString();
    if ( fname.empty() ) return;
    file.open(fname);
    if( !file ) QMessageBox::warning(this, "Error", "Unable to open file");
    fsd->exportDot(file);
}

void MainWindow::quit()
{
    checkUnsavedChanges();
    close();
}
