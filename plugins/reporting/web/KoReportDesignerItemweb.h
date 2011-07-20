/*
 * Kexi Report Plugin
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef REPORTENTITYWEB_H
#define REPORTENTITYWEB_H

#include <QObject>
#include "KoReportItemweb.h"
#include <KoReportDesignerItemRectBase.h>

/**
 @author Adam Pigg <adam@piggz.co.uk>
*/
class KoReportDesignerItemweb : public KoReportItemweb, public KoReportDesignerItemRectBase
{
    Q_OBJECT
public:
    KoReportDesignerItemweb(KoReportDesigner *, QGraphicsScene* scene, const QPointF &pos);
    KoReportDesignerItemweb(QDomNode & element, KoReportDesigner *, QGraphicsScene* scene);
//KoReportDesignerItemweb::KoReportDesignerItemweb(){}
    virtual ~KoReportDesignerItemweb();
    virtual void buildXML(QDomDocument & doc, QDomElement & parent);
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
    virtual KoReportDesignerItemweb* clone();

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);

private:
    void init(QGraphicsScene*);

private slots:
    void slotPropertyChanged(KoProperty::Set &, KoProperty::Property &);
   // void slotReportDataChanged();

};

#endif
