#include "storagerestapi.h"
#include <QFile>
#include <QSettings>

StorageRestApi::StorageRestApi(QObject* parent) : QObject(parent)
{
     m_manager = new QNetworkAccessManager(this);

     QSettings options("XD", "StickyNotes");
     options.beginGroup("Settings");

     accName = options.value("AccName").toString();
     accKey = options.value("AccKey").toString();
     container = options.value("Container").toString();

     options.endGroup();
}

void StorageRestApi::updateCredentials(const QString&accountName,
                                       const QString& accountKey, const QString& container_)
{
    accName = accountName;
    accKey = accountKey;
    container = container_;
}

StorageRestApi::RequestType StorageRestApi::getAzureRequestType()
{
    return currentRequestType;
}

QNetworkReply* StorageRestApi::listFiles()
{
    currentRequestType = GetList;
    QString currentDateTime = generateCurrentTimeUTC();

    QString url = generateUrl(container, "", "restype=container&comp=list");

    QStringList additionnalCanonicalRessources;
    additionnalCanonicalRessources.append("comp:list");
    additionnalCanonicalRessources.append("restype:container");

    QString authorization = generateAutorizationHeader("GET", container, "", currentDateTime, 0, QStringList(), additionnalCanonicalRessources);

    QNetworkRequest request;

    request.setUrl(QUrl(url));
    request.setRawHeader(QByteArray("Authorization"), QByteArray(authorization.toStdString().c_str()));
    request.setRawHeader(QByteArray("x-ms-date"), QByteArray(currentDateTime.toStdString().c_str()));
    request.setRawHeader(QByteArray("x-ms-version"), QByteArray(m_version.toStdString().c_str()));
    request.setRawHeader(QByteArray("Content-Length"), QByteArray("0"));

    return m_manager->get(request);
}



QNetworkReply* StorageRestApi::uploadFile(const QString& filePath, const QString& blobName, const QString& blobType)
{
    currentRequestType = Upload;
    QByteArray data;
    QFile file(filePath);

    if (file.open(QIODevice::ReadOnly))
    {
      data = file.readAll();
    }
    else
    {
      return nullptr;
    }

    QString currentDateTime = generateCurrentTimeUTC();
    int contentLength = data.size();

    QStringList additionalCanonicalHeaders;
    additionalCanonicalHeaders.append(QString("x-ms-blob-type:%1").arg(blobType));

    QString url = generateUrl(container, blobName);
    QString authorization = generateAutorizationHeader("PUT", container, blobName, currentDateTime, data.size(), additionalCanonicalHeaders, QStringList());

    QNetworkRequest request;

    request.setUrl(QUrl(url));
    request.setRawHeader(QByteArray("Authorization"),QByteArray(authorization.toStdString().c_str()));
    request.setRawHeader(QByteArray("x-ms-date"),QByteArray(currentDateTime.toStdString().c_str()));
    request.setRawHeader(QByteArray("x-ms-version"),QByteArray(m_version.toStdString().c_str()));
    request.setRawHeader(QByteArray("Content-Length"),QByteArray(QString::number(contentLength).toStdString().c_str()));
    request.setRawHeader(QByteArray("x-ms-blob-type"),QByteArray(blobType.toStdString().c_str()));

    return m_manager->put(request, data);
}

QNetworkReply* StorageRestApi::uploadFile(const QByteArray& data, const QString& blobName, const QString& blobType)
{
    currentRequestType = Upload;

    QString currentDateTime = generateCurrentTimeUTC();
    int contentLength = data.size();

    QStringList additionalCanonicalHeaders;
    additionalCanonicalHeaders.append(QString("x-ms-blob-type:%1").arg(blobType));

    QString url = generateUrl(container, blobName);
    QString authorization = generateAutorizationHeader("PUT", container, blobName, currentDateTime, data.size(), additionalCanonicalHeaders, QStringList());

    QNetworkRequest request;

    request.setUrl(QUrl(url));
    request.setRawHeader(QByteArray("Authorization"),QByteArray(authorization.toStdString().c_str()));
    request.setRawHeader(QByteArray("x-ms-date"),QByteArray(currentDateTime.toStdString().c_str()));
    request.setRawHeader(QByteArray("x-ms-version"),QByteArray(m_version.toStdString().c_str()));
    request.setRawHeader(QByteArray("Content-Length"),QByteArray(QString::number(contentLength).toStdString().c_str()));
    request.setRawHeader(QByteArray("x-ms-blob-type"),QByteArray(blobType.toStdString().c_str()));

    return m_manager->put(request, data);
}

QNetworkReply* StorageRestApi::downloadFile(const QString& blobName)
{
    currentRequestType = Download;
    QString currentDateTime = generateCurrentTimeUTC();

    QString url = generateUrl(container, blobName);

    QString authorization = generateAutorizationHeader("GET", container, blobName, currentDateTime, 0);

    QNetworkRequest request;

    request.setUrl(QUrl(url));
    request.setRawHeader(QByteArray("Authorization"), QByteArray(authorization.toStdString().c_str()));
    request.setRawHeader(QByteArray("x-ms-date"), QByteArray(currentDateTime.toStdString().c_str()));
    request.setRawHeader(QByteArray("x-ms-version"), QByteArray(m_version.toStdString().c_str()));
    request.setRawHeader(QByteArray("Content-Length"), QByteArray("0"));

    return m_manager->get(request);
}

QNetworkReply* StorageRestApi::deleteFile(const QString& blobName)
{
    currentRequestType = Delete;
    QString currentDateTime = generateCurrentTimeUTC();

    QString url = generateUrl(container, blobName);

    QString authorization = generateAutorizationHeader("DELETE", container, blobName, currentDateTime, 0);


    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setRawHeader(QByteArray("Authorization"), QByteArray(authorization.toStdString().c_str()));
    request.setRawHeader(QByteArray("x-ms-date"), QByteArray(currentDateTime.toStdString().c_str()));
    request.setRawHeader(QByteArray("x-ms-version"), QByteArray(m_version.toStdString().c_str()));

    return m_manager->get(request);
}

QString StorageRestApi::generateCurrentTimeUTC()
{
    return QLocale(QLocale::English).toString(QDateTime::currentDateTimeUtc(), "ddd, dd MMM yyyy hh:mm:ss").append(" GMT");
}

QString StorageRestApi::generateHeader(
        const QString& httpVerb, const QString& contentEncoding, const QString& contentLanguage, const QString& contentLength,
        const QString& contentMd5, const QString& contentType, const QString& date, const QString& ifModifiedSince,
        const QString& ifMatch, const QString& ifNoneMatch, const QString& ifUnmodifiedSince, const QString& range,
        const QString& canonicalizedHeaders, const QString& canonicalizedResource)
{
    QString result = "";

    result = result + httpVerb + "\n";
    result = result + contentEncoding + "\n";
    result = result + contentLanguage + "\n";
    result = result + contentLength + "\n";
    result = result + contentMd5 + "\n";
    result = result + contentType + "\n";
    result = result + date + "\n";
    result = result + ifModifiedSince + "\n";
    result = result + ifMatch + "\n";
    result = result + ifNoneMatch + "\n";
    result = result + ifUnmodifiedSince + "\n";
    result = result + range + "\n";
    result = result + canonicalizedHeaders + "\n";
    result = result + canonicalizedResource;

    return result;
}

QString StorageRestApi::generateAutorizationHeader(const QString& httpVerb, const QString& container,
                                                    const QString& blobName, const QString& currentDateTime,
                                                    const long& contentLength, const QStringList additionnalCanonicalHeaders,
                                                    const QStringList additionnalCanonicalRessources)
{
    // Create canonicalized header
    QString canonicalizedHeaders;
    for (const QString& additionnalCanonicalHeader : additionnalCanonicalHeaders)
    {
        canonicalizedHeaders.append(additionnalCanonicalHeader+"\n");
    }
    canonicalizedHeaders.append(QString("x-ms-date:%1\nx-ms-version:%2").arg(currentDateTime, m_version));

    // Create canonicalized ressource
    QString canonicalizedResource;
    if (blobName.isEmpty())
    {
        canonicalizedResource = QString("/%1/%2").arg(accName, container);
    }
    else
    {
        canonicalizedResource = QString("/%1/%2/%3").arg(accName, container, blobName);
    }

    for (const QString& additionnalCanonicalRessource : additionnalCanonicalRessources)
    {
        canonicalizedResource.append("\n"+additionnalCanonicalRessource);
    }

    // Create signature
    QString signature = generateHeader(httpVerb, "", "", QString::number(contentLength), "", "", "", "",
                                     "", "", "", "", canonicalizedHeaders, canonicalizedResource);



    // Create authorization header
    QByteArray authorizationHeader = QMessageAuthenticationCode::hash(
                QByteArray(signature.toUtf8()),
                QByteArray(QByteArray::fromBase64(accKey.toStdString().c_str())),
                QCryptographicHash::Sha256);
    authorizationHeader = authorizationHeader.toBase64();

    return QString("SharedKey %1:%2").arg(accName, QString(authorizationHeader));
}

QString StorageRestApi::generateUrl(const QString& container, const QString& blobName, const QString& additionnalParameters)
{
    QString blobEndPoint = QString("https://%1.blob.core.windows.net/").arg(accName);
    QString url = blobEndPoint + container;
    if (!blobName.isEmpty())
    {
        url.append("/" + blobName);
    }

    if (!additionnalParameters.isEmpty())
    {
        url.append("?"+additionnalParameters);
    }

    return url;
}
