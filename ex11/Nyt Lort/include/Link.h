
#ifndef __LINKLAGET_H__
#define __LINKLAGET_H__

#include <ezV24/ezV24.h>

#define DELIMITER 'A'

namespace Link {

class Link
{
public:
    Link(int bufsize );
    ~Link();
  void send(const char [], short size);
    short receive(char buf[], short size);
private:
    Link( const Link &r){};
    Link& operator=(const Link &) {};
    char *buffer;
    v24_port_t *serialPort;
    short encodeMessage(char* encodedBuffer, const char buf[], short size);
    short decodeMessage(char* buf, const char encodedBuffer[], short size);
};


} /* namespace Linklaget */
#endif /* LINKLAGET_H_ */
