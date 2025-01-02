/*
 * bitshift dynamics - Copyright 2012, All rights reserved
 * Author: Alexander Nassian <nassian@bitshift-dynamics.com>
 */

#include "email.h"

#include <QString>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QByteArray>
#include <QDateTime>
#include <QDesktopServices>
#include <QUrl>
#include <QDebug>

#include "mimetypemanager.h"


/**
  * \todo Documentation
  */
Email::Email(QObject*parent, const QString& mimeTypesPath) :
    QObject(parent),
    mimeTypes(new MimeTypeManager(mimeTypesPath))
{
}

/**
  * \todo Documentation
  */
void Email::openInDefaultProgram()
{
    QString email = QStringLiteral("Content-Type: multipart/alternative; boundary=\"BitshiftDynamicsMailerBoundary\"\r\n");

    // Field used by Microsoft Outlook Express to designate that a message is currently being authored (@link https://stackoverflow.com/questions/64232710/is-the-flag-x-unsent-still-working-to-save-and-open-a-eml-file-as-a-draft)
    email.append(QLatin1String("X-Unsent:1\r\n"));

    // Add sender information

    // Add receiver information
    email.append(QLatin1String("To: "));
    email.append(p.receiverAddress);
    email.append(QLatin1String("\r\n"));

    // Add subject
    email.append(QLatin1String("Subject: "));
    email.append(p.subject);
    email.append(QLatin1String("\r\n"));

    // Add mime version
    email.append(QLatin1String("Mime-Version: 1.0 BitshiftDynamics Mailer\r\n\r\n"));

    // Add body
    email.append(QLatin1String("--BitshiftDynamicsMailerBoundary\r\n"));
    email.append(QLatin1String("Content-Transfer-Encoding: quoted-printable\r\n"));
    email.append(QLatin1String("Content-Type: text/plain;\r\n"));
    email.append(QLatin1String("        charset=iso-8859-1\r\n\r\n"));
    email.append(p.messageText);
    email.append(QLatin1String("\r\n\r\n"));

    // Add attachments
    foreach (QString filePath, p.attachments) {
        QFileInfo fileInfo(filePath);
        QString fileName   = fileInfo.fileName();
        QString fileSuffix = fileInfo.suffix();
        QString mimeType   = mimeTypes->mimeTypeFromExtension(fileSuffix);

        QFile attachmentFile(filePath);
        if (attachmentFile.open(QIODevice::ReadOnly) == false) {
            qCritical() << "Failed loading attachment.";

            emit composerOpened(false);
            return;
        }

        QByteArray fileData = attachmentFile.readAll();
        attachmentFile.close();

        email.append(QLatin1String("\r\n--BitshiftDynamicsMailerBoundary\r\n"));
        email.append(QLatin1String("Content-Type: multipart/mixed;\r\n"));
        email.append(QLatin1String("        boundary=\"BitshiftDynamicsMailerBoundary\"\r\n\r\n"));

        email.append(QLatin1String("--BitshiftDynamicsMailerBoundary\r\n"));
        email.append(QLatin1String("Content-Disposition: inline;\r\n"));
        email.append(QLatin1String("        filename=\"")).append(fileName).append(QLatin1String("\"\r\n"));
        email.append(QLatin1String("Content-Type: ")).append(mimeType).append(QLatin1String(";\r\n"));
        email.append(QLatin1String("        name=\"")).append(fileName).append(QLatin1String("\"\r\n"));
        email.append(QLatin1String("Content-Transfer-Encoding: base64\r\n\r\n"));

        email.append(QString::fromUtf8(fileData.toBase64()));
    }


    // Create temporary file and open it in the user's default email composer
    QString tmpFilePath = QDir::tempPath().append(QStringLiteral("/ComposedEmail-%1.eml").arg(QDateTime::currentDateTime().toSecsSinceEpoch()));
    QFile tmpFile(tmpFilePath);
    if (tmpFile.open(QIODevice::WriteOnly) == false) {
        qCritical() << "Failed opening temp file for email composing:" << tmpFilePath;

        emit composerOpened(false);
        return;
    }

    tmpFile.write(email.toLatin1());
    tmpFile.close();

    emit composerOpened(QDesktopServices::openUrl(QUrl::fromLocalFile(tmpFilePath)));
}
