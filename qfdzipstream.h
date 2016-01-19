#ifndef QFDZIPSTREAM_H
#define QFDZIPSTREAM_H

#include <QIODevice>
#include <QDateTime>

class QTextCodec;

struct QFDZipStreamPrivate;

class QFDZipStream : public QIODevice
{
    Q_OBJECT
public:
    struct EntryCompressionResult
    {
        qint32 CRC32;
        qint64 CompressedSize;
        qint64 UncompressedSize;
    };

private:
    QIODevice* output;
    QFDZipStreamPrivate* state;
    QTextCodec* zipcodec;
public:
    QFDZipStream(QIODevice* out);
    virtual ~QFDZipStream();

    bool beginEntry(const QString& name, bool compress = true, const QDateTime& date = QDateTime());
    bool endEntry(EntryCompressionResult* result = NULL);

    QTextCodec* codec() const { return zipcodec; }
    void setCodec(QTextCodec* codec) { zipcodec = codec; }

    // QIODevice interface
public:
    virtual bool isSequential() const;
    virtual bool open(OpenMode mode = QIODevice::WriteOnly);
    virtual void close();

protected:
    virtual qint64 readData(char *data, qint64 maxlen);
    virtual qint64 writeData(const char *data, qint64 len);
};

#endif // QFDZIPSTREAM_H
