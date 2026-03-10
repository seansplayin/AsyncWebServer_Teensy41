#ifndef cbuf_h
#define cbuf_h

#include <cstddef>
#include <cstring>

class cbuf {
public:
  cbuf(size_t size) : _size(size), _buf(new char[size]), _bufend(_buf + size), _begin(_buf), _end(_begin) {
  }
  ~cbuf() { delete[] _buf; }
  size_t getSize() const { return _size; }
  size_t room() const {
    if (_end >= _begin) {
      return _bufend - _end + _begin - _buf - 1;
    }
    return _begin - _end - 1;
  }
  size_t size() const {
    if (_end >= _begin) {
      return _end - _begin;
    }
    return _bufend - _begin + _end - _buf;
  }
  bool empty() const { return _begin == _end; }
  bool full() const { return room() == 0; }
  int peek() {
    if (empty()) return -1;
    return static_cast<int>(*_begin);
  }
  size_t read(char* dst, size_t size) {
    size_t bytes_available = this->size();
    size_t size_to_read = (size < bytes_available) ? size : bytes_available;
    size_t size_read = size_to_read;
    if (_end > _begin && size_to_read > (size_t)(_bufend - _begin)) {
      size_t top_size = _bufend - _begin;
      memcpy(dst, _begin, top_size);
      _begin = _buf;
      dst += top_size;
      size_to_read -= top_size;
    }
    memcpy(dst, _begin, size_to_read);
    _begin += size_to_read;
    if (_begin >= _bufend) _begin -= (_bufend - _buf);
    return size_read;
  }
  int read() {
    if (empty()) return -1;
    char c = *_begin;
    _begin++;
    if (_begin >= _bufend) _begin = _buf;
    return static_cast<int>(c);
  }
  size_t write(const char* src, size_t size) {
    size_t bytes_available = room();
    size_t size_to_write = (size < bytes_available) ? size : bytes_available;
    size_t size_written = size_to_write;
    if (_begin > _end && size_to_write > (size_t)(_bufend - _end)) {
      size_t top_size = _bufend - _end;
      memcpy(_end, src, top_size);
      _end = _buf;
      src += top_size;
      size_to_write -= top_size;
    }
    memcpy(_end, src, size_to_write);
    _end += size_to_write;
    if (_end >= _bufend) _end -= (_bufend - _buf);
    return size_written;
  }
  size_t write(char ch) {
    if (full()) return 0;
    *_end = ch;
    _end++;
    if (_end >= _bufend) _end = _buf;
    return 1;
  }
  void flush() {
    _begin = _end = _buf; // Reset pointers to start
  }
  void resizeAdd(size_t addSize) {
    size_t newlen = _size + addSize;
    char *newbuf = new char[newlen];
    size_t bytes_available = size();
    read(newbuf, bytes_available);
    delete[] _buf;
    _size = newlen;
    _buf = newbuf;
    _bufend = _buf + _size;
    _begin = _buf;
    _end = _begin + bytes_available;
  }
 private:
  size_t _size;
  char* _buf;
  const char* _bufend;
  char* _begin;
  char* _end;
};

#endif