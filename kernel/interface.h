#ifndef _INTERFACE_H
#define _INTERFACE_H

#define CALL_FUNCTION _IOWR(0xEE, 0, char *)

#define SET_MOCK _IOW(0xEE, 1, char *)

#endif // _INTERFACE_H