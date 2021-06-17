#ifndef STORAGERESTAPI_H
#define STORAGERESTAPI_H

#include <QObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QXmlStreamReader>
#include <QMessageAuthenticationCode>

class StorageRestApi : public QObject
{
    Q_OBJECT
public:
    enum RequestType {GetList, Upload, Download, Delete, None};

public:
    StorageRestApi(QObject* parent = nullptr);

    void updateCredentials(const QString&accName, const QString& accKey,
                           const QString& container_);

    QNetworkReply* listFiles();

    QNetworkReply* uploadFile(const QString& filePath, const QString& blobName, const QString& blobType = "BlockBlob");
    QNetworkReply* uploadFile(const QByteArray& data, const QString& blobName, const QString& blobType = "BlockBlob");
    QNetworkReply* downloadFile(const QString& blobName);
    QNetworkReply* deleteFile(const QString& blobName);

    QNetworkAccessManager* getNetwork() {return m_manager;}

    RequestType getAzureRequestType();

private:
    QString generateCurrentTimeUTC();
    QString generateHeader(const QString&, const QString&, const QString&, const QString&, const QString&, const QString&, const QString&,
                           const QString&, const QString&, const QString&, const QString&, const QString&, const QString&, const QString&);
    QString generateAutorizationHeader(const QString& httpVerb, const QString& container, const QString& blobName,
                                       const QString& currentDateTime, const long& contentLength,
                                       const QStringList additionnalCanonicalHeaders = QStringList(),
                                       const QStringList additionnalCanonicalRessources = QStringList());
    QString generateUrl(const QString& container, const QString& blobName = QString(), const QString& additionnalParameters = QString());

private:
    QString m_version = "2009-09-19";
    QString accName;
    QString accKey;
    QString container;
    QNetworkAccessManager* m_manager;
    RequestType currentRequestType = None;
};

#endif // STORAGERESTAPI_H
