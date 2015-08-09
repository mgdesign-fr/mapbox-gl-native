#include "qsqlitecache_p.hpp"

#include <QDateTime>
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>

#include <mbgl/storage/sqlite_cache.hpp>
#include <mbgl/util/compression.hpp>


QSqliteCachePrivate::QSqliteCachePrivate(const QString &path)
    : m_cache(QSqlDatabase::addDatabase("QSQLITE"))
{
    m_cache.setDatabaseName(path);

    if (!m_cache.open()) {
        qWarning() << "Error opening " << path << ":" << m_cache.lastError().text();
    }
}

bool QSqliteCachePrivate::isValid() const
{
    return m_cache.isOpen();
}

qint64 QSqliteCachePrivate::cacheSize() const
{
    // Not implemented.

    return 0;
}

QIODevice *QSqliteCachePrivate::data(const QUrl &)
{
    m_buffer->open(QIODevice::ReadOnly);

    return m_buffer.take();
}

void QSqliteCachePrivate::insert(QIODevice *)
{
    // Not implemented.
}

QNetworkCacheMetaData QSqliteCachePrivate::metaData(const QUrl &url)
{
    QString normalized(url.toString());

    // XXX: In the cache, we use % encoding for the URLs, but only
    // for whitespace on the fonts names.
    if (normalized.contains("/font")) {
        normalized = QUrl::toPercentEncoding(normalized, ":/?=");
        normalized = normalized.replace("%2520", "%20");
        normalized = normalized.replace("%2C", "%2c");
    }

    std::string str(mbgl::unifyMapboxURLs(
        std::string(normalized.toUtf8().constData(), normalized.size())));

    // FIXME: Too many data conversions and copies.
    QString unified = QString::fromUtf8(str.data(), str.size());

    QSqlQuery query(m_cache);
    query.prepare("SELECT compressed, data, expires FROM http_cache WHERE url=?");
    query.bindValue(0, unified);
    query.exec();

    if (!query.first()) {
        return QNetworkCacheMetaData();
    }

    m_buffer.reset(new QBuffer);
    QByteArray cacheData(query.value(1).toByteArray());

    if (query.value(0).toBool()) { // Compressed.
        std::string compressed(cacheData.constData(), cacheData.size());
        std::string decompressed = mbgl::util::decompress(compressed);

        m_buffer->setData(QByteArray(decompressed.data(), decompressed.size()));
    } else {
        m_buffer->setData(cacheData);
    }

    m_metaData.setExpirationDate(QDateTime::fromMSecsSinceEpoch(query.value(2).toULongLong()));
    m_metaData.setUrl(url);
    m_metaData.setSaveToDisk(true);

    return m_metaData;
}

QIODevice *QSqliteCachePrivate::prepare(const QNetworkCacheMetaData &)
{
    // Not implemented.

    return nullptr;
}

bool QSqliteCachePrivate::remove(const QUrl &)
{
    // Not implemented.

    return true;
}

void QSqliteCachePrivate::updateMetaData(const QNetworkCacheMetaData &)
{
    // Not implemented.
}

void QSqliteCachePrivate::clear()
{
    // Not implemented.
}
