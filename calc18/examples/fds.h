#ifndef _FDS_H
#define _FDS_H

#define BYTES(x)        (2*(x))

#define FDE_DISKFULL    1
#define FDE_DIRFULL     2
#define FDE_EXISTS      3
#define FDE_NOTFOUND    4
#define FDE_SYNTAX      5
#define FDE_DISK        6
#define FDE_IOCB        7
#define FDE_LOAD        8


#define MAXNAME   10
#define MAXNAME_W  5

/*
   struct DIRENT
      0 name   (MAXNAME / 2)
      5 tracks 1  packed as (ntracks,track)
      6 size   1
      7 type   1  0 - ASCII, 1 - binary
*/

#define DIRENT_tracks   5
#define DIRENT_size     6
#define DIRENT_type     7
#define sizeof_DIRENT   8

#define DE_EMPTY        (0000)
#define DE_DELETED      (0200)

#define MAXDIRENT  8
/*
 * struct DIR
 *    dirent[MAXDIRENT]
 */

#define MAXBUF  128
#define MAXBUF_W 64


#define DC3    023

/*
 * struct IOCB
 *    0 nm     5
 *    5 pos    1
 *    6 size   1
 *    7 eof    1
 *    8 rw     1
 *    9 bufpos 1
 *   10 dcb    2
 *   12 buf    1
 *   13 rsvd   3
 */

#define IOCB_pos      5
#define IOCB_size     6
#define IOCB_eof      7
#define IOCB_rw       8
#define IOCB_bufpos   9
#define IOCB_dcb     10
#define IOCB_buf     12
#define sizeof_IOCB  16

#define EOT DC3

#endif

/* vim: set ts=3 sw=3 et: */
