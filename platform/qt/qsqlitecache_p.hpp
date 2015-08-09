#ifndef QSQLITECACHE
#define QSQLITECACHE

#include <QAbstractNetworkCache>
#include <QBuffer>
#include <QScopedPointer>
#include <QSqlDatabase>

class QBuffer;
class QString;

class QSqliteCachePrivate : public QAbstractNetworkCache
{
    Q_OBJECT

public:
    QSqliteCachePrivate(const QString &path);
    ~QSqliteCachePrivate() override = default;

    bool isValid() const;

    // SqliteCache implementation.
    qint64 cacheSize() const override;
    QIODevice *data (const QUrl &url) override;
    void insert(QIODevice *device) override;
    QNetworkCacheMetaData metaData(const QUrl &url) override;
    QIODevice *prepare( const QNetworkCacheMetaData &metaData) override;
    bool remove(const QUrl &url) override;
    void updateMetaData(const QNetworkCacheMetaData &metaData) override;

public slots:
    // SqliteCache implementation.
    void clear() override;

private:
    QSqlDatabase m_cache;

    QNetworkCacheMetaData m_metaData;
    QScopedPointer<QBuffer> m_buffer;
};

#endif
