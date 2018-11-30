#ifndef __FILE_CLIENT_H__
#define __FILE_CLIENT_H__

#include <iostream>

class file_client
{
public:
    file_client(void);

private:
	void receiveFile (char* fileName, Transport::Transport *transport);
};

#endif /* __FILE_CLIENT_H__ */
