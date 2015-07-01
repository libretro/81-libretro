#include <types.h>

AnsiString AnsiString::UpperCase() const
{
  AnsiString res = *this;
  int len = Length();
  
  for ( int i = 0; i < len; i++ )
  {
    res[ i ] = toupper( res[ i ] );
  }
  
  return res;
}

int AnsiString::AnsiPos( const AnsiString& other ) const
{
  size_t pos = str.find( other.c_str() );
  return pos == std::string::npos ? 0 : pos + 1;
}

AnsiString operator+ ( const char* lhs, const AnsiString& rhs )
{
  AnsiString res = lhs;
  res += rhs;
  return res;
}

int TStrings::Add( const AnsiString str )
{
  list.push_back( str );
  return list.size();
}
