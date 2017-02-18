#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

class String
{
public:
    // constants
    static const size_t MAX_SIZE = 4611686018427387897u;
    static const size_t npos     = -1;

    // constructors & destructor
    String();
    String(const String& str);
    String(const String& str, size_t subpos, size_t sublen);
    String(const char* s);
    String(const char* s, size_t n);
    String(size_t n, char c);
    // range constructor
    ~String() { delete[] _data; }
    // c-style String
    const char* c_str();

    // assignment operator
    String& operator=(const String& str);
    String& operator=(const char* s);
    String& operator=(char c);

    // capacity
    size_t size() const { return _sz; }
    size_t length() const { return _sz; }
    // max_size()
    void resize(size_t n, char c);
    size_t capacity() const { return _cap; }
    void reserve(size_t n);
    void clear();
    bool empty() const { return _sz == 0; }
    // element access
    char& operator[](size_t pos) { return _data[pos]; }
    const char& operator[](size_t pos) const { return _data[pos]; }
    char& at(size_t pos);
    const char& at(size_t pos) const;

    // modifiers
    String& operator+=(const String& str);
    String& operator+=(const char* s);
    String& operator+=(char c);
    String& append(const String& str);
    String& append(const String& str, size_t subpos, size_t sublen);
    String& append(const char* s);
    String& append(const char* s, size_t n);
    String& append(size_t n, char c);
    void push_back(char c);
    String& assign(const String& str);
    String& assign(const String& str, size_t subpos, size_t sublen);
    String& assign(const char* s);
    String& assign(const char* s, size_t n);
    String& assign(size_t n, char c);
    String& insert (size_t pos, const String& str);
    String& insert (size_t pos, const String& str, size_t subpos, size_t sublen);
    String& insert (size_t pos, const char* s);
    String& insert (size_t pos, const char* s, size_t n);
    String& insert (size_t pos, size_t n, char c);
    String& erase(size_t pos, size_t len);

    static void test();

private:
    static const size_t ALLOC_MASK = 15;
    size_t _sz;
    size_t _cap;
    size_t _min_cap;
    char* _data;

protected:
    void _shrink()
    {
        if (_cap > _min_cap && _sz >> 1 < _cap) {
            _cap          = (_sz + 1) | ALLOC_MASK;
            char* newData = new char[_cap]();
            memcpy(newData, _data, _sz);
            delete[] _data;
            _data = newData;
        }
    }
    void _reserve(size_t n) // doesn't modify min_cap; relies on _sz, only modify it after calling this.
    {
        if (n >= _cap) {
            _cap          = (n + 1) | ALLOC_MASK;
            char* newData = new char[_cap]();
            memcpy(newData, _data, _sz);
            delete[] _data;
            _data = newData;
        }
    }
    void _alloc(size_t n) // doesn't reserve content in data.
    {
        if (n >= _cap) {
            _cap = (n + 1) | ALLOC_MASK;
            delete[] _data;
            _data = new char[_cap]();
        }
    }
};

String::String()
{
    _min_cap = ALLOC_MASK;
    _cap     = ALLOC_MASK;
    _data    = new char[_cap]();
    _sz      = 0;
}

String::String(const String& str)
{
    _min_cap = ALLOC_MASK;
    _sz      = str._sz;
    _cap     = str._cap;
    _data    = new char[_cap]();
    memcpy(_data, str._data, _sz);
}

String::String(const String& str, size_t pos, size_t len = npos)
{
    if (pos >= str._sz)
        throw std::out_of_range("String::String");
    _min_cap = ALLOC_MASK;
    len      = std::min(str._sz, len);
    _sz      = len;
    _cap     = (_sz + 1) | ALLOC_MASK;
    _data    = new char[_cap]();
    memcpy(_data, str._data + pos, len);
}

String::String(const char* s)
{
    size_t len = strlen(s);
    if (len > MAX_SIZE)
        throw std::length_error("String::String");
    _min_cap = ALLOC_MASK;
    _sz      = len;
    _cap     = (_sz + 1) | ALLOC_MASK;
    _data    = new char[_cap]();
    strcpy(_data, s); // Programmer is to provide a legal c-style String.
}

String::String(const char* s, size_t n)
{
    if (n > MAX_SIZE)
        throw std::length_error("String::String");
    _min_cap = ALLOC_MASK;
    _cap     = (n + 1) | ALLOC_MASK;
    _data    = new char[_cap]();
    _sz      = n;
    memcpy(_data, s, _sz);
}

String::String(size_t n, char c)
{
    if (n > MAX_SIZE)
        throw std::length_error("String::String");
    _min_cap = ALLOC_MASK;
    _cap     = (n + 1) | ALLOC_MASK;
    _data    = new char[_cap]();
    _sz      = n;
    memset(_data, c, _sz);
}

const char* String::c_str() { return _data; }

String& String::operator=(const String& str)
{
    _min_cap = ALLOC_MASK;
    _alloc(str._sz);
    memcpy(_data, str._data, str._sz);
    _sz = str._sz;
    _shrink();
    return *this;
}

String& String::operator=(const char* s)
{
    size_t len = strlen(s);
    if (len > MAX_SIZE)
        throw std::length_error("String::operator=");
    _min_cap = ALLOC_MASK;
    _alloc(len);
    strcpy(_data, s);
    _sz = len;
    _shrink();
    return *this;
}

String& String::operator=(char c)
{
    _min_cap = ALLOC_MASK;
    _sz      = 1;
    _cap     = ALLOC_MASK;
    _data    = new char[_cap]();
    _data[0] = c;
    return *this;
}

void String::resize(size_t n, char c = '\0')
{
    if (n > MAX_SIZE)
        throw std::length_error("String::resize");
    if (n <= _sz) {
        _data[n] = 0;
        _sz      = n;
        _shrink();
        return;
    }
    _reserve(n);
    memset(_data + _sz, c, n - _sz);
    _sz            = n;
    _data[_sz + 1] = '\0';
}

void String::reserve(size_t n = 0)
{
    if (n > MAX_SIZE)
        throw std::length_error("String::reserve");
    _reserve(n);
    _min_cap = n;
}

void String::clear()
{
    _cap = _min_cap = ALLOC_MASK;
    _sz             = 0;
    _data           = new char[_cap]();
}

char& String::at(size_t pos)
{
    if (pos < _sz)
        return _data[pos];
    else
        throw std::out_of_range("String::at");
}

const char& String::at(size_t pos) const
{
    if (pos < _sz)
        return _data[pos];
    else
        throw std::out_of_range("String::at");
}

String& String::operator+=(const String& str) { return this->append(str); }
String& String::append(const String& str)
{
    _reserve(_sz + str._sz);
    memcpy(_data + _sz, str._data, str._sz);
    _sz += str._sz;
    return *this;
}

String& String::operator+=(const char* s) { return this->append(s); }
String& String::append(const char* s)
{
    size_t len = strlen(s);
    if (_sz + len > MAX_SIZE)
        throw std::length_error("String::append");
    _reserve(_sz + len);
    strcpy(_data + _sz, s);
    _sz += len;
    return *this;
}

String& String::operator+=(const char c)
{
    _reserve(_sz + 1);
    _data[_sz++] = c;
    return *this;
}

String& String::append(const String& str, size_t subpos, size_t sublen)
{
    if (subpos >= str._sz)
        throw std::out_of_range("String::append");
    sublen = std::min(str._sz - subpos, sublen);
    if (_sz + sublen > MAX_SIZE)
        throw std::length_error("String::append");
    _reserve(_sz + sublen);
    memcpy(_data + _sz, str._data + subpos, sublen);
    _sz += sublen;
    return *this;
}

String& String::append(const char* s, size_t n)
{
    if (_sz + n > MAX_SIZE)
        throw std::length_error("String::append");
    _reserve(_sz + n);
    memcpy(_data + _sz, s, n);
    _sz += n;
    return *this;
}

String& String::append(size_t n, char c)
{
    if (_sz + n > MAX_SIZE)
        throw std::length_error("String::append");
    _reserve(_sz + n);
    memset(_data + _sz, c, n);
    _sz += n;
    return *this;
}

void String::push_back(char c)
{
    if (_sz + 1 > MAX_SIZE)
        throw std::length_error("String::append");
    _reserve(_sz + 1);
    _data[_sz++] = c;
}

String& String::assign(const String& str)
{
    return *this = str;
}

String& String::assign(const String& str, size_t subpos, size_t sublen)
{
    if (subpos >= str._sz)
        throw std::out_of_range("String::assign");
    sublen = std::min(str._sz - subpos, sublen);
    _sz    = sublen;
    _reserve(_sz);
    memcpy(_data, str._data + subpos, _sz);
    return *this;
}

String& String::assign(const char* s)
{
    return *this = s;
}

String& String::assign(const char* s, size_t n)
{
    if (n > MAX_SIZE)
        throw std::length_error("String::assign");
    _reserve(n);
    memcpy(_data, s, n);
    _sz = n;
    return *this;
}

String& String::assign(size_t n, char c)
{
    _reserve(n);
    memset(_data, c, n);
    _sz = n;
    _shrink();
    return *this;
}

String& String::insert(size_t pos, const String& str)
{
    if (pos >= _sz)
        throw std::out_of_range("String::insert");
    if (pos + str._sz > MAX_SIZE)
        throw std::length_error("String::insert");
    _reserve(pos + str._sz);
    memcpy(_data + pos, str._data, str._sz);
    _sz = pos + str._sz;
    return *this;
}

String& String::insert(size_t pos, const String& str, size_t subpos, size_t sublen)
{
    if (pos >= _sz || subpos >= str._sz)
        throw std::out_of_range("String::insert");
    sublen = std::min(sublen, str._sz - subpos);
    if (_sz + sublen > MAX_SIZE)
        throw std::length_error("String::insert");
    _reserve(_sz + sublen);
    memcpy(_data + pos + sublen, _data + pos, _sz - pos);
    memcpy(_data + pos, str._data + subpos, sublen);
    _sz += sublen;
    return *this;
}

String& String::insert(size_t pos, const char* s)
{
    if (pos >= _sz)
        throw std::out_of_range("String::insert");
    size_t len = strlen(s);
    if (_sz + len > MAX_SIZE)
        throw std::length_error("String::insert");
    _reserve(_sz + len);
    memcpy(_data + pos + len, _data + pos, _sz - pos);
    memcpy(_data + pos, s, len);
    _sz += len;
    return *this;
}

String& String::insert(size_t pos, const char* s, size_t n)
{
    if (pos >= _sz)
        throw std::out_of_range("String::insert");
    if (_sz + n > MAX_SIZE)
        throw std::length_error("String::insert");
    _reserve(_sz + n);
    memcpy(_data + pos + n, _data + pos, _sz - pos);
    memcpy(_data + pos, s, n);
    _sz += n;
    return *this;
}

String& String::insert(size_t pos, size_t n, char c)
{
    if (pos >= _sz)
        throw std::out_of_range("String::insert");
    if (_sz + n > MAX_SIZE)
        throw std::length_error("String::insert");
    _reserve(_sz + n);
    memcpy(_data + pos + n, _data + pos, _sz - pos);
    memset(_data + pos, c, n);
    _sz += n;
    return *this;
}

String& String::erase(size_t pos = 0, size_t len = npos)
{
    if (pos >= _sz)
        throw std::out_of_range("String::erase");
    len = std::min(len, _sz - pos);
    for (size_t i = 0; i < _sz - pos - len; i++)    // two sections may overlap, therefore cannot use memcpy.
        _data[pos + i] = _data[pos + len + i];
    memset(_data + _sz - len, 0, len);
    _sz -= len;
    _shrink();
    return *this;
}

void String::test()
{
    String a = "123456";
    a.insert(3, 3, '-');
    a.erase(3, 1);
    printf("%s\n", a.c_str());
}

int main()
{
    String::test();
    return 0;
}
