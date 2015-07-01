#ifndef TYPES_H
#define TYPES_H

/* Avoid errors due to types we define below */
#include <stdlib.h>

#ifdef __cplusplus

#include <string>
#include <vector>

class AnsiString
{
private:
  std::string str;
  
public:
  inline AnsiString() {}
  inline AnsiString( const char* cstr ) { str = cstr; }
  inline AnsiString( const std::string& other ) { str = other; }
  inline int Length() const { return (int)str.length(); }
  inline char& operator[] ( size_t pos ) { return str[ pos - 1 ]; }
  inline const char& operator[] ( size_t pos ) const { return str[ pos - 1 ]; }
  inline AnsiString& operator+= ( char c ) { str += c; return *this; }
  inline AnsiString& operator+= ( const AnsiString& other ) { str += other.str; return *this; }
  inline AnsiString SubString( int index, int count ) const { return str.substr( index - 1, count ); }
  inline bool operator== ( const char* other ) { return str.compare( other ) == 0; }
  inline bool operator!= ( const char* other ) { return str.compare( other ) != 0; }
  inline char* c_str() const { return const_cast< char* >( str.c_str() ); }
  
  AnsiString UpperCase() const;
  int AnsiPos( const AnsiString& other ) const;
};

AnsiString operator+ ( const char* lhs, const AnsiString& rhs );

class TStrings
{
private:
  std::vector< AnsiString > list;
  
public:
  int Add( const AnsiString str );
};

#endif

#include <stdint.h>

#undef SBYTE
#undef BYTE
#undef byte
#undef BOOLEAN
#undef SHORT
#undef WORD
#undef LONG
#undef DWORD
#undef ULONG
#undef LONGLONG
#undef __int64
#undef HANDLE
#undef BOOL

typedef int8_t   SBYTE;
typedef uint8_t  BYTE;
typedef uint8_t  byte;
typedef uint8_t  BOOLEAN;

typedef int16_t  SHORT;
typedef uint16_t WORD;
typedef uint16_t Word;

typedef int32_t  LONG;
typedef uint32_t DWORD;
typedef uint32_t ULONG;

typedef int64_t  LONGLONG;
typedef int64_t  __int64;

typedef void*    HANDLE;
typedef int      BOOL;

enum
{
  FALSE = 0,
  TRUE  = 1
};

typedef enum
{
  FixedMedia
}
MEDIA_TYPE;

typedef union
{
  struct
  {
    DWORD LowPart;
    LONG  HighPart;
  };
  struct
  {
    DWORD LowPart;
    LONG  HighPart;
  } u;
  
  LONGLONG QuadPart;
}
LARGE_INTEGER;

typedef struct
{
  LARGE_INTEGER Cylinders;
  MEDIA_TYPE    MediaType;
  DWORD         TracksPerCylinder;
  DWORD         SectorsPerTrack;
  DWORD         BytesPerSector;
}
DISK_GEOMETRY;

typedef struct
{
  LARGE_INTEGER StartingOffset;
  LARGE_INTEGER PartitionLength;
  DWORD         HiddenSectors;
  DWORD         PartitionNumber;
  BYTE          PartitionType;
  BOOLEAN       BootIndicator;
  BOOLEAN       RecognizedPartition;
  BOOLEAN       RewritePartition;
}
PARTITION_INFORMATION;

#endif /* TYPES_H */
