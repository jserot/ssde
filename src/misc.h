#ifndef MISC_H
#define MISC_H

#include <QList>
#include <QSet>

template <typename T>
QSet<T> QListToQSet(const QList<T>& qlist)
{
#if QT_VERSION >= 0x060000
  return QSet<T> (qlist.constBegin(), qlist.constEnd());
#else
  return QSet<T>::fromList(qlist);
#endif
}

template <typename T>
QList<T> remove_duplicates(const QList<T>& l)
{
  return QListToQSet(l).values();
}

#endif

