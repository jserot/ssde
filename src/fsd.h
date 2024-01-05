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

#ifndef FSD_H
#define FSD_H

#include <QFile>
#include <QTextStream>
#include <QGraphicsScene>

#include "state.h"

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
class QPointF;
class QGraphicsLineItem;
class QFont;
class QColor;
class QGVScene;
QT_END_NAMESPACE

class Fsd : public QGraphicsScene
{
    Q_OBJECT

public:
    enum Mode { InsertState, InsertPseudoState, InsertTransition, InsertLoopTransition, SelectItem, DeleteItem };

    explicit Fsd(QWidget *parent = 0);
    void fromString(QString& json_text);
    QString toString();

    void exportDot(QString fname);
    void renderDot(QGVScene *scene);

    State* initState();
    QList<State*> states();
    QList<Transition*> transitions();

    State* getState(QString id);
    bool hasPseudoState();

public slots:
    void setMode(Mode mode);

signals:
    /* void stateInserted(State *item); */
    /* void transitionInserted(Transition *item); */
    void fsdModified();
    void stateSelected(State *item);
    void transitionSelected(Transition *item);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) override;

    static int stateCounter;

private:
    bool isItemChange(int type);
    State* addState(QPointF pos, QString id);
    State* addPseudoState(QPointF pos);
    Transition* addTransition(State* srcState, State* dstState, QString label, State::Location location);

    Mode mode;
    QGraphicsLineItem *line;  // Line being drawn
    State *startState;

    QWidget *mainWindow;

    static QColor lineColor;
    static QColor boxColor;
};

#endif // FSD_H
