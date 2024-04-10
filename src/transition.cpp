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
#include <math.h>
#include <QPen>
#include <QPainter>
#include <QSet>
#include <QDebug>
#include "qt_compat.h"
#include "misc.h"

QColor Transition::selectedColor = Qt::darkCyan;
QColor Transition::unSelectedColor = Qt::black;
double Transition::arrowSize = 20.0;

Transition::Transition(State *srcState, State *dstState, QString label, State::Location location, QGraphicsItem *parent)
    : QGraphicsPolygonItem(parent)
{
    mySrcState = srcState;
    myDstState = dstState;
    myLocation = location;
    myLabel = new QGraphicsSimpleTextItem(label, this);
    myLabel->setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setPen(QPen(unSelectedColor, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
}

void Transition::setLabel(QString label)
{
  myLabel->setText(label);
}

QString Transition::getLabel()
{
  return myLabel->text();
}

bool Transition::isInitial()
{
  return mySrcState ? mySrcState->isPseudo() : false;
}

QRectF Transition::boundingRect() const
{
    qreal extra = (pen().width() + 20) / 2.0;

    if ( mySrcState == myDstState )
      return
        QRectF(polygon().at(0), polygon().at(2))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
    else
      return
        QRectF(polygon().at(0),
               QSizeF(polygon().at(1).x() - polygon().at(0).x(),
                      polygon().at(1).y() - polygon().at(0).y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}

QPainterPath Transition::shape() const
{
    QPainterPath path = QGraphicsPolygonItem::shape();
    path.addPolygon(arrowHead);
    return path;
}

void Transition::updatePosition()
{
  QPolygonF p;
  if ( mySrcState == myDstState ) {
    float w = State::boxSize.width();
    float h = State::boxSize.height();
    p << mapFromItem(mySrcState, 0, -0.25*h)
      << mapFromItem(mySrcState, 0, 0.25*h)
      << mapFromItem(mySrcState, w, 0.25*h)
      << mapFromItem(mySrcState, w, -0.25*h);
    }
  else
    p << mapFromItem(mySrcState, 0, 0)
      << mapFromItem(myDstState, 0, 0);
  setPolygon(p);
}

void Transition::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    // qDebug() << "------------- Transition::paint";
    // qDebug() << "Drawing transition between state " << mySrcState->getId() << " and " << myDstState->getId();

    QPen myPen = pen();
    myPen.setColor(isSelected() ? selectedColor : unSelectedColor);
    painter->setPen(myPen);
    painter->setBrush(isSelected() ? selectedColor : unSelectedColor);

    QPolygonF points; // Drawing points
    double angle; // Of the last segment; for drawing the arrow head
    QPointF endPoint; // For anchoring the arrow head
    QPointF midPoint; // For drawing the caption

    float w = State::boxSize.width();
    float h = State::boxSize.height();

    if ( mySrcState == myDstState ) { // Looping transition

      QPointF c = mySrcState->pos(); // Box center
      switch ( myLocation ) {
      case State::East:
        points << c + QPointF(0, 0.25*h)
               << c + QPointF(w, 0.25*h)
               << c + QPointF(w, -0.25*h)
               << c + QPointF(0.5*w, -0.25*h);
        angle = 0;
        break;
      case State::West:
        points << c + QPointF(0, 0.25*h)
               << c + QPointF(-w, 0.25*h)
               << c + QPointF(-w, -0.25*h)
               << c + QPointF(-0.5*w, -0.25*h);
        angle = M_PI;
        break;
      case State::North:
        points << c + QPointF(0.25*w, 0)
               << c + QPointF(0.25*w, -h)
               << c + QPointF(-0.25*w, -h)
               << c + QPointF(-0.25*w, -0.5*h);
        angle = M_PI/2;
        break;
      case State::South:
      case State::None:
        points << c + QPointF(0.25*w, 0)
               << c + QPointF(0.25*w, h)
               << c + QPointF(-0.25*w, h)
               << c + QPointF(-0.25*w, 0.5*h);
        angle = -M_PI/2;
        break;
      }
      endPoint = points.at(3);
      midPoint = (points.at(1) + points.at(2))/2;
    }

    else { // Normal transition

      if (mySrcState->collidesWithItem(myDstState)) return; // Do not draw if start and end statees collide

      // Find the position where to draw the arrow head
      // This is where the line and the end state intersect

      QLineF centerLine(mySrcState->pos(), myDstState->pos());
      QPolygonF endPolygon = myDstState->polygon();
      QPointF p1 = endPolygon.first() + myDstState->pos();  // Item to scene coordinates
      QPointF p2;
      QPointF intersectPoint;
      QLineF polyLine;
      int side;  // 1: North, 2: East, 3: South, 4: West
      for (side = 1; side < endPolygon.count(); ++side) {
        // Iterating over the end box sides to find the intersection (there must be exactly one)
        p2 = endPolygon.at(side) + myDstState->pos();
        polyLine = QLineF(p1, p2);
        QLineF::IntersectType intersectType = POLYLINE_INTERSECT(centerLine, &intersectPoint);
        if (intersectType == QLineF::BoundedIntersection) break;
        p1 = p2;
      }

      // When there are several transitions between the start and end boxes, we don't one to hide another.
      // To avoid this, we count them and assign a rank to each one, which will be used as an offset
      
      QList<Transition*> transitions = mySrcState->getTransitionsTo(myDstState) + myDstState->getTransitionsTo(mySrcState);
      // for ( auto a: transitions ) 
      //   qDebug() << "  " << a->srcState()->getId() << " -> " << a->dstState()->getId();
      transitions = remove_duplicates(transitions); 
      std::sort(transitions.begin(), transitions.end()); // Sorting ensures that the order does not depend on the start state
      int rank = -1;
      for ( int i=0; i<transitions.length() && rank<0; i++ )
        if ( transitions.at(i) == this ) rank = i;
      if ( rank == -1 ) throw std::invalid_argument(std::string("Transition::paint:compute_rank"));

      int n = transitions.length();
      QPointF offset;
      switch ( side ) {
      case 1: case 3: 
        offset = QPointF((rank+1)*w/(n+1)-w/2, 0); break;
      case 2: case 4: 
        offset = QPointF(0, (rank+1)*w/(n+1)-w/2); break;
      default:
        offset = QPoint(0,0); break; // should not happen 
      }

      QLineF line = QLineF(intersectPoint+offset, mySrcState->pos()+offset);
      points << line.p1() << line.p2();

      angle = ::acos(line.dx() / line.length());
      if (line.dy() >= 0) angle = (M_PI * 2) - angle;
      endPoint = line.p1();
      midPoint = (line.p1() + line.p2())/2;
    }

    setPolygon(points);  // The recorded polygon does not include the arrow head
    
    // Build arrow head  

    QPointF arrowP1 = endPoint + QPointF(sin(angle + M_PI/3) * arrowSize, cos(angle + M_PI/3) * arrowSize);
    QPointF arrowP2 = endPoint + QPointF(sin(angle + M_PI - M_PI/3) * arrowSize, cos(angle + M_PI - M_PI/3) * arrowSize);
    arrowHead.clear();
    arrowHead << endPoint << arrowP1 << arrowP2;

    // Draw all
    
    painter->drawPolyline(points);
    painter->drawPolygon(arrowHead);

    myLabel->setPos(midPoint);
}
