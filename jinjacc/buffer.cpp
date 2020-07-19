#include "buffer.h"
#include <fstream>
#include <cstring>

#define     INITIAL_CAPACITY    (10)

Buffer::Buffer()
{
  m_current = 0;
  m_capacity = INITIAL_CAPACITY;
  m_data = new uint8_t[INITIAL_CAPACITY];
}

Buffer::~Buffer()
{
  delete[] m_data;
}

void Buffer::reallocate(uint32_t minToAdd)
{
  uint32_t sizeToInsert;
  uint8_t* data;
  sizeToInsert = std::max(m_capacity + minToAdd, m_capacity*2);
  
  data = new uint8_t[sizeToInsert];
  std::copy(m_data, m_data+m_current, data);
  
  delete[] m_data;
  m_data = data;
  m_capacity = sizeToInsert;
}

void Buffer::checkAndReallocateSize(uint32_t size)
{
  if ((m_current + size) > m_capacity)
  {
    reallocate(size);
  }
}


int32_t Buffer::insertString(const std::string& s)
{  
  int32_t offset  = m_current;
  uint32_t size = s.size() + 1;
  
  checkAndReallocateSize(size);
  
  const char* p = s.c_str();  
  std::copy(p, p+size, m_data+ m_current);

  m_current += size;
  return offset;
}

int32_t Buffer::insert(const int8_t i)
{
  checkAndReallocateSize(sizeof(int8_t));
  int32_t offset = m_current;
  m_data[m_current] = i;
  m_current++;
  return offset;
}

int32_t Buffer::insert(const int32_t i)
{
  checkAndReallocateSize(sizeof(int32_t));
  int32_t offset = m_current;
  int32_t d = htobe32(i);
  ::memcpy(&m_data[m_current], &d, sizeof(int32_t));
  m_current+= sizeof(int32_t);
  return offset;
}

void Buffer::write(std::ofstream* outFile)
{
  outFile->write((const char*) m_data, m_current);
}

int32_t Buffer::insert(const double d)
{
  checkAndReallocateSize(sizeof(double));
  int32_t offset = m_current;
  int64_t t;
  t = *((int64_t*) &d);
  t = htobe64(t);
  
  ::memcpy(&m_data[m_current], &t, sizeof(double));
  m_current+= sizeof(double);
  return offset;
}
