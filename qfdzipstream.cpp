#include "qfdzipstream.h"
#include <QTextCodec>

extern "C" {
#include "fdzipstream/fdzipstream.h"
}
struct QFDZipStreamPrivate
{
    ZIPstream* stream;
    ZIPentry* entry;
    QFDZipStreamPrivate() : stream(NULL), entry(NULL) {}
};

QFDZipStream::QFDZipStream(QIODevice *out) : output(out)
{
    state = new QFDZipStreamPrivate;
    zipcodec = QTextCodec::codecForName("utf-8");
}

QFDZipStream::~QFDZipStream()
{
    close();
    delete state;
}

bool QFDZipStream::beginEntry(const QString &name, bool compress, const QDateTime &date)
{
    if(state->entry)
    {
        setErrorString("Call endEntry() before beginning new entry");
        return true;
    }
    const QDateTime& dt = date.isValid() ? date : QDateTime::currentDateTimeUtc();
    time_t tm = dt.toTime_t();
    if(!(state->entry = zs_entrybegin(state->stream, zipcodec->fromUnicode(name).data(),
                                      tm, compress ? ZS_DEFLATE : ZS_STORE, NULL)))
    {
        setErrorString("Error while writing to QFDZipStream");
        return false;
    }
    return true;
}

bool QFDZipStream::endEntry(EntryCompressionResult *result)
{
    if(!state->entry)
        return true;
    bool res = true;
    if(!zs_entryend(state->stream, state->entry, NULL))
    {
        res = false;
        setErrorString("Error while writing to QFDZipStream");
    }
    if(result)
    {
        result->CompressedSize = state->entry->CompressedSize;
        result->UncompressedSize = state->entry->UncompressedSize;
        result->CRC32 = state->entry->CRC32;
    }
    state->entry = NULL;
    return res;
}

bool QFDZipStream::isSequential() const
{
    return false;
}

extern "C" ssize_t qiodevice_writer(void* userdata, const void *buf, size_t count)
{
    QIODevice* dev = (QIODevice*)userdata;
    return (ssize_t)dev->write((const char*)buf, count);
}

bool QFDZipStream::open(QIODevice::OpenMode mode)
{
    if(mode & QIODevice::ReadOnly)
        return false;

    if(!QIODevice::open(mode))
        return false;

    if(!(state->stream = zs_init_custom_writer(&qiodevice_writer, output, state->stream)))
        return false;

    return true;
}

qint64 QFDZipStream::readData(char *, qint64)
{
    Q_ASSERT(false);
    qFatal("read from QFDZipStream");
}

qint64 QFDZipStream::writeData(const char *data, qint64 len)
{
    if(!state->entry)
    {
        setErrorString("Call beginEntry() before writing");
        return -1;
    }
    if(!zs_entrydata(state->stream, state->entry, (uint8_t*)data, len, NULL))
    {
        setErrorString("Error while writing to QFDZipStream");
        return -1;
    }
    return len;
}

void QFDZipStream::close()
{
    if(state->entry)
        endEntry();
    zs_finish(state->stream, NULL);
    zs_free(state->stream);
    QIODevice::close();
}
