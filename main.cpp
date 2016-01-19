#include <QCoreApplication>
#include "qfdzipstream.h"
#include <QDir>
#include <QTextStream>
#include <stdio.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTextStream out(stdout);
    QStringList args = QCoreApplication::arguments();

    QFile zip(args.size() > 1 ? args.at(1) : "out.zip");
    if(!zip.open(QFile::WriteOnly))
    {
        out << zip.errorString();
        return 1;
    }
    QFileInfo fiz(zip.fileName());

    QFDZipStream fdz(&zip);
    fdz.open();

    QDir d(".");
    QFile f;

    const qint64 maxsize = 1024*1024;
    char* buf = new char[maxsize];

    QFDZipStream::EntryCompressionResult ecr;

    foreach (const QFileInfo& fi, d.entryInfoList(QDir::NoDotAndDotDot | QDir::Files))
    {
        if(fi.absoluteFilePath() == fiz.absoluteFilePath())
            continue;
        f.setFileName(fi.absoluteFilePath());
        if(!f.open(QFile::ReadOnly))
            continue;
        fdz.beginEntry(fi.fileName(), true, fi.lastModified());

        while(!f.atEnd())
        {
            qint64 nb = f.read(buf, maxsize);
            fdz.write(buf, nb);
        }
        f.close();
        fdz.endEntry(&ecr);
        out << fi.fileName() << ": "
            << ecr.UncompressedSize << " -> "
            << ecr.CompressedSize << " ("
            << QString::number(100.0*ecr.CompressedSize/ecr.UncompressedSize, 'f', 2) << "%) CRC="
            << QStringLiteral("%1").arg(ecr.CRC32&0xFFFFFFFF, 8, 16, QChar('0')) << "\n";
        out.flush();
    }

    return 0;
}
