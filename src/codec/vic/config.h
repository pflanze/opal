/************ Change log
 *
 * $Log: config.h,v $
 * Revision 1.2002  2003/03/15 23:42:59  robertj
 * Update to OpenH323 v1.11.7
 *
 * Revision 1.11  2002/10/10 05:40:11  robertj
 * VxWorks port, thanks Martijn Roest
 *
 * Revision 1.10  2002/06/13 09:54:43  craigs
 * Removed iostream.h to reduce gcc incompatibilities
 *
 * Revision 1.9  2002/05/03 08:21:17  rogerh
 * Only use the 64 bit h.261 codec on little endian machines. (see dct.cxx)
 * Submitted by andi <andi@fischlustig.de>
 *
 * Revision 1.8  2002/04/26 04:57:41  dereks
 * Add Walter Whitlocks fixes, based on Victor Ivashim's suggestions to
 * improve the quality with Netmeeting. Thanks guys!!!!
 *
 * Revision 1.7  2002/04/05 00:53:19  dereks
 * Modify video frame encoding so that frame is encoded on an incremental basis.
 * Thanks to Walter Whitlock - good work.
 *
 * Revision 1.6  2001/09/26 02:00:09  robertj
 * Changed to use MSVC and GNU compiler built in 64 bit integer.
 *
 * Revision 1.5  2001/05/16 06:30:29  yurik
 * INT_64 is defined here
 *
 * Revision 1.4  2001/05/10 14:21:34  craigs
 * Added BYTE, as ptlib.h has been removed
 *
 * Revision 1.3  2001/05/10 05:25:44  robertj
 * Removed need for VIC code to use ptlib.
 *
 * Revision 1.2  2000/08/25 03:18:49  dereks
 * Add change log facility (Thanks Robert for the info on implementation)
 *
 *
 *
 ********/

#if defined(_WIN32) && !defined(WIN32)
#define WIN32
#endif

#include <ptlib.h>
#include <string.h>

typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned int  u_int;
typedef unsigned char BYTE;

/*
 * Largest (user-level) packet size generated by our rtp applications.
 * Individual video formats may use smaller mtu's.
 */
#define RTP_MTU 1024


#if defined(_MSC_VER)
//#define INT_64 __int64 // uncomment for 64 bit word machines
#elif defined(__GNUC__)
#if PBYTE_ORDER == PLITTLE_ENDIAN
// only use the 64 bit h.261 codec on little endian machines
#define INT_64 long long
#endif
#endif

