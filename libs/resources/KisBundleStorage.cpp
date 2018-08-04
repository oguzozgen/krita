/*
 * Copyright (C) 2018 Boudewijn Rempt <boud@valdyas.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KisBundleStorage.h"

#include <QDebug>
#include <QFileInfo>

#include <KisTag.h>
#include "KisResourceStorage.h"
#include "KoResourceBundle.h"
#include "KoResourceBundleManifest.h"

class BundleTagIterator : public KisResourceStorage::TagIterator
{
public:

    BundleTagIterator(KoResourceBundle *bundle, const QString &resourceType)
        : m_bundle(bundle)
        , m_resourceType(resourceType)
    {
        QList<KoResourceBundleManifest::ResourceReference> resources = m_bundle->manifest().files(resourceType);
        Q_FOREACH(KoResourceBundleManifest::ResourceReference resource, resources) {
            Q_FOREACH(const QString &tagname, resource.tagList) {
                if (!m_tags.contains(tagname)){
                    KisTagSP tag = QSharedPointer<KisTag>(new KisTag());
                    tag->setName(tagname);
                    tag->setComment(tagname);
                    tag->setUrl(bundle->filename() + ':' + tagname);
                    m_tags[tagname] = tag;
                }
                m_tags[tagname]->setDefaultResources(m_tags[tagname]->defaultResources() << resource.resourcePath);
            }
        }
        m_tagIterator.reset(new QListIterator<KisTagSP>(m_tags.values()));
    }

    bool hasNext() const override
    {
        return m_tagIterator->hasNext();
    }
    void next() const override
    {
        const_cast<BundleTagIterator*>(this)->m_tag = m_tagIterator->next();
    }

    QString url() const override { return m_tag ? m_tag->url() : QString(); }
    QString name() const override { return m_tag ? m_tag->name() : QString(); }
    QString comment() const override {return m_tag ? m_tag->comment() : QString(); }
    KisTagSP tag() const override { return m_tag; }

private:
    QHash<QString, KisTagSP> m_tags;
    KoResourceBundle *m_bundle {0};
    QString m_resourceType;
    QScopedPointer<QListIterator<KisTagSP> > m_tagIterator;
    KisTagSP m_tag;
};


class BundleIterator : public KisResourceStorage::ResourceIterator
{
public:
    BundleIterator(KoResourceBundle *bundle, const QString &resourceType)
        : m_bundle(bundle)
        , m_resourceType(resourceType)
    {
        m_entriesIterator.reset(new QListIterator<KoResourceBundleManifest::ResourceReference>(m_bundle->manifest().files(resourceType)));
    }

    bool hasNext() const override
    {
        return m_entriesIterator->hasNext();
    }

    void next() const override
    {
        KoResourceBundleManifest::ResourceReference ref = m_entriesIterator->next();
        const_cast<BundleIterator*>(this)->m_resourceReference = ref;
    }

    QString url() const override
    {
        return m_resourceReference.resourcePath;
    }

    QString type() const override
    {
        return m_resourceType;
    }

    QDateTime lastModified() const override
    {
        return QFileInfo(m_bundle->filename()).lastModified();
    }

    /// This only loads the resource when called
    QByteArray md5sum() const override
    {
        return m_resourceReference.md5sum;
    }

    /// This only loads the resource when called
    KoResourceSP resource() const override
    {
        return m_bundle->resource(m_resourceType, m_resourceReference.resourcePath);
    }

private:

    KoResourceBundle *m_bundle {0};
    QString m_resourceType;
    QScopedPointer<QListIterator<KoResourceBundleManifest::ResourceReference> > m_entriesIterator;
    KoResourceBundleManifest::ResourceReference m_resourceReference;

};


class KisBundleStorage::Private {
public:
    QScopedPointer<KoResourceBundle> bundle;
};


KisBundleStorage::KisBundleStorage(const QString &location)
    : KisStoragePlugin(location)
    , d(new Private())
{
    d->bundle.reset(new KoResourceBundle(location));
    if (!d->bundle->load()) {
        qWarning() << "Could not load bundle" << location;
    }
}

KisBundleStorage::~KisBundleStorage()
{
}

KisResourceStorage::ResourceItem KisBundleStorage::resourceItem(const QString &url)
{
    return KisResourceStorage::ResourceItem();
}

KoResourceSP KisBundleStorage::resource(const QString &url)
{
    return 0;
}

QSharedPointer<KisResourceStorage::ResourceIterator> KisBundleStorage::resources(const QString &resourceType)
{
    return QSharedPointer<KisResourceStorage::ResourceIterator>(new BundleIterator(d->bundle.data(), resourceType));
}

QSharedPointer<KisResourceStorage::TagIterator> KisBundleStorage::tags(const QString &resourceType)
{
    return QSharedPointer<KisResourceStorage::TagIterator>(new BundleTagIterator(d->bundle.data(), resourceType));
}

QImage KisBundleStorage::thumbnail() const
{
    return d->bundle->image();
}

QString KisBundleStorage::metaData(const QString &key) const
{
    return d->bundle->metaData(key);
}
