#include "StdCharInStream.h"
#include "CharInStreaming.h"

namespace Core::Processing
{

#if defined(__MINGW32__) || defined(__MINGW64__)
StdCharInStream::StdCharInStream(FILE* fd) : fd(fd)
{
  VALIDATE_NOT_NULL(fd);
#else
StdCharInStream::StdCharInStream(InStream *s) : stream(s)
{
  VALIDATE_NOT_NULL(s);
#endif
}

StdCharInStream::~StdCharInStream() {}

#if defined(__MINGW32__) || defined(__MINGW64__)
WChar StdCharInStream::get()
{
  WChar c;
  fread(&c, sizeof(c), 1, this->fd);
  return c;
#else
Char StdCharInStream::get()
{
  Char c;
  this->stream->get(c);
  return c;
#endif
}

Bool StdCharInStream::isEof()
{
#if defined(__MINGW32__) || defined(__MINGW64__)
  return feof(this->fd);
#else
  return this->stream->eof();
#endif
}

CharInStreaming::CharInStreamingType StdCharInStream::getType()
{
  return CharInStreaming::CharInStreamingType::STD_CHAR_IN_STREAM;
}

} // namespace