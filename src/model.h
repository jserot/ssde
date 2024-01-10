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

#ifndef MODEL_H
#define MODEL_H

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
QT_END_NAMESPACE

class Model : public QGraphicsScene
{
    Q_OBJECT

public:
    enum Mode { InsertState, InsertPseudoState, InsertTransition, InsertLoopTransition, SelectItem, DeleteItem };

    explicit Model(QWidget *parent = 0);
    void fromString(QString& json_text);
    QString toString();

    void exportDot(QString fname);
    void renderDot(QGraphicsView *view, int width, int height);

    State* initState();
    QList<State*> states();
    QList<Transition*> transitions();

    State* getState(QString id);
    bool hasPseudoState();

public slots:
    void setMode(Mode mode);
    Mode getMode(void);

signals:
    // void stateInserted(State *item);
    // void transitionInserted(Transition *item);
    void modelModified();
    void stateSelected(State *item);
    void transitionSelected(Transition *item);
    void mouseEnter(void);
    void mouseLeave(void);

protected:
    bool event(QEvent *event) override;
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

    QGraphicsScene *scene;
};

#endif // MODEL_H
