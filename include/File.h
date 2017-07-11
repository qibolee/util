#ifndef _FILE_H_
#define _FILE_H_

#include <string>
#include <stdio.h>

// default file buf size
const unsigned int BUF_SIZE = 8192;

struct ReadBuf {
    char *_buf;
    unsigned int _capacity;
    unsigned int _next;
    unsigned int _size;
};

class File {
    using uint = unsigned int;

public:
    // build file stream, may throw std::runtime_error
    File(const char *path, const char *mode);
    File(const std::string &path, const char *mode);

    File(const File &file) = delete;
    File(File &&file) = delete;
    File &operator=(const File &file) = delete;
    File &operator=(File &&file) = delete;

    // destroy this file stream
    ~File();
    
    const char *get_file_name() const { return _path; }

    const char *get_file_mode() const { return _mode; }

    // read file to buf
    int read(char *buf, uint len);

    // read line to buf
    int read_line(char *buf, uint len);
    
    // write file from buf
    int write(char *buf, uint len);

    // write file from format and argv
    int write_fmt(const char *format, ...);

private:
    FILE *_fp;
    const char *_path;
    const char *_mode;

    int _pos;
    ReadBuf _read_buf;

    // read data to _read_buf
    int do_read();

    // clear read buf
    void clear_read_buf();
};


#endif
