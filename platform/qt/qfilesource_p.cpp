#include "qfilesource_p.hpp"

#include "qsqlitecache_p.hpp"

#include <mbgl/util/mapbox.hpp>
#include <mbgl/storage/request.hpp>
#include <mbgl/storage/response.hpp>

#include <QByteArray>
#include <QDir>
#include <QNetworkReply>
#include <QScopedPointer>
#include <QSslConfiguration>

QFileSourcePrivate::QFileSourcePrivate()
{
    connect(this, SIGNAL(urlRequested(mbgl::Request*)), this, SLOT(handleUrlRequest(mbgl::Request*)), Qt::QueuedConnection);
    connect(this, SIGNAL(urlCanceled(mbgl::Request*)), this, SLOT(handleUrlCancel(mbgl::Request*)), Qt::QueuedConnection);

    connect(&m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinish(QNetworkReply*)));
    connect(&m_manager, SIGNAL(sslErrors(QNetworkReply*, const QList<QSslError> &)), this, SLOT(sslErrors(QNetworkReply*, const QList<QSslError>&)));
}

void QFileSourcePrivate::setAccessToken(const QString& token)
{
    m_token = token.toUtf8().constData();
}

void QFileSourcePrivate::setCacheDatabase(const QString& path)
{
    QScopedPointer<QSqliteCachePrivate> cache(new QSqliteCachePrivate(path));

    if (cache->isValid()) {
        m_manager.setCache(cache.take());
    }
}

mbgl::Request *QFileSourcePrivate::request(const mbgl::Resource &resource, uv_loop_t *loop, Callback cb)
{
    // WARNING: Must be thread-safe.

    std::string normalizedUrl;

    switch (resource.kind) {
    case mbgl::Resource::Kind::Style:
        normalizedUrl = mbgl::util::mapbox::normalizeStyleURL(resource.url, m_token);
        break;
    case mbgl::Resource::Kind::Source:
        normalizedUrl = mbgl::util::mapbox::normalizeSourceURL(resource.url, m_token);
        break;
    case mbgl::Resource::Kind::Glyphs:
        normalizedUrl = mbgl::util::mapbox::normalizeGlyphsURL(resource.url, m_token);
        break;
    default:
        normalizedUrl = resource.url;
    }

    mbgl::Request* req = new mbgl::Request({ resource.kind, normalizedUrl }, loop, std::move(cb));
    emit urlRequested(req);

    return req;
}

void QFileSourcePrivate::cancel(mbgl::Request *req)
{
    // WARNING: Must be thread-safe.

    req->cancel();
    emit urlCanceled(req);
}

void QFileSourcePrivate::handleUrlRequest(mbgl::Request *req)
{
    QUrl url = QUrl::fromPercentEncoding(QByteArray(req->resource.url.data(), req->resource.url.size()));

    if (url.scheme() == "asset") {
        url.setUrl("file://" + QDir::currentPath() + "/" + url.host() + url.path());
    }

    QPair<QNetworkReply*, QVector<mbgl::Request*>> &data = m_pending[url];
    QVector<mbgl::Request*> &requests = data.second;
    requests.append(req);

    if (requests.size() > 1) {
        return;
    }

    QNetworkRequest qreq = QNetworkRequest(url);
    qreq.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);

    QSslConfiguration config = qreq.sslConfiguration();
    config.setProtocol(QSsl::TlsV1);
    qreq.setSslConfiguration(config);

    data.first = m_manager.get(qreq);
}

void QFileSourcePrivate::handleUrlCancel(mbgl::Request *req)
{
    QUrl url = QUrl::fromPercentEncoding(QByteArray(req->resource.url.data(), req->resource.url.size()));

    auto it = m_pending.find(url);
    if (it == m_pending.end()) {
        req->destruct();
        return;
    }

    QPair<QNetworkReply*, QVector<mbgl::Request*>> &data = it.value();
    QNetworkReply* reply = data.first;
    QVector<mbgl::Request *> &requests = data.second;

    for (int i = 0; i < requests.size(); ++i) {
        if (req == requests.at(i)) {
            requests.remove(i);
            break;
        }
    }

    req->destruct();

    if (requests.empty()) {
        m_pending.erase(it);
        reply->abort();
    }
}

void QFileSourcePrivate::replyFinish(QNetworkReply *reply)
{
    QUrl url = reply->request().url();

    auto it = m_pending.find(url);
    if (it == m_pending.end()) {
        reply->deleteLater();
        return;
    }

    std::shared_ptr<mbgl::Response> response = std::make_shared<mbgl::Response>();
    if (reply->error()) {
        response->message = reply->errorString().toUtf8().constData();
    } else {
        QByteArray bytes = reply->readAll();
        if (bytes.size()) {
            response->status = mbgl::Response::Status::Successful;
            response->data = std::string(bytes.data(), bytes.size());
        }
    }

    QVector<mbgl::Request *> &requests = it.value().second;
    for (auto req : requests) {
        req->notify(response);
    }

    m_pending.erase(it);
    reply->deleteLater();
}

void QFileSourcePrivate::sslErrors(QNetworkReply *reply, const QList<QSslError> &)
{
    // FIXME: Install Mapbox certificates.
    reply->ignoreSslErrors();
}
