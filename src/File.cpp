#include <stdexcept>
#include <algorithm>
#include <string>
#include <stdarg.h>
#include <string.h>

#include "File.h"

File::File(const char *path, const char *mode): _fp{fopen(path, mode)}, _path{path}, _mode{mode}, 
    _pos{0}, _read_buf{new char[BUF_SIZE], BUF_SIZE, 0, 0} {

    // open file failed, throw runtime_error    
    if (_fp == nullptr) {
        throw std::runtime_error{"File: cannot open file"};
    }

    // this FILE stream not use buf
    setbuf(_fp, nullptr);
}

File::File(const std::string &path, const char *mode): File{path.c_str(), mode} {

}

File::~File() {

    if (_fp != nullptr) {
        fclose(_fp);
        _fp = nullptr;
    }
    if (_read_buf._buf != nullptr) {
        delete [] _read_buf._buf;
        _read_buf._buf = nullptr;
    }
}

int File::read(char *buf, uint len) {

    int tot_read = 0;
    if (buf == nullptr) {
        throw std::invalid_argument("File::read(): invalid argument");
    }
    while (tot_read < len) {
        if (_read_buf._next == _read_buf._size) {
            if (this->do_read() == 0) {
                break;
            }
        }
        int copy_size = std::min(len - tot_read, _read_buf._size - _read_buf._next);
        strncpy(buf + tot_read, _read_buf._buf + _read_buf._next, copy_size);
        tot_read += copy_size;
        _read_buf._next += copy_size;
        _pos += copy_size;
    }
    return tot_read;
}

int File::read_line(char *buf, uint len) {
    
    char ch;
    int tot_read = 0;
    if (buf == nullptr) {
        throw std::invalid_argument("File::read_line(): invalid argument");
    }
    if (len == 0) {
        return 0;
    }
    while (true) {
        if (_read_buf._next == _read_buf._size) {
            if (this->do_read() == 0) {
                break;
            }
        }
        ch = _read_buf._buf[_read_buf._next++];
        ++_pos;
        if (tot_read < len - 1) {
            buf[tot_read++] = ch;
        }
        if (ch == '\n') {
            break;
        }
    }
    buf[len - 1] = '\0';
    return tot_read;
}

int File::write(const char *buf, uint len) {

    if (buf == nullptr) {
        throw std::invalid_argument("File::write(): invalid argument");
    }
    fseek(_fp, _pos, SEEK_SET);
    uint res = fwrite(buf, sizeof(char), len, _fp);
    _pos += res;
    return res;
}

int File::write_fmt(const char *fmt, ...) {

    if (fmt == nullptr) {
        throw std::invalid_argument("File::write_fmt(): invalid argument");
    }
    va_list ap;
    va_start(ap, fmt);
    fseek(_fp, _pos, SEEK_SET);
    int res = vfprintf(_fp, fmt, ap);
    va_end(ap);
    _pos += res;;
    clear_read_buf();
    return res;
}

int File::do_read() {

    fseek(_fp, _pos, SEEK_SET);
    if (feof(_fp)) {
        _read_buf._size = 0;
        _read_buf._next = 0;
        return 0;
    }
    int s = fread(_read_buf._buf, sizeof(char), _read_buf._capacity, _fp);
    _read_buf._size = s;
    _read_buf._next = 0;
    return s;
}

void File::clear_read_buf() {

    _read_buf._next = 0;
    _read_buf._size = 0;
}

