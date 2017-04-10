// generated from http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/errno.h.html

#include <errno.h>

#include "common.hpp"

namespace dromozoa {
  void initialize_errno(lua_State* L) {
#ifdef E2BIG
    luaX_set_field(L, -1, "E2BIG", E2BIG);
#endif
#ifdef EACCES
    luaX_set_field(L, -1, "EACCES", EACCES);
#endif
#ifdef EADDRINUSE
    luaX_set_field(L, -1, "EADDRINUSE", EADDRINUSE);
#endif
#ifdef EADDRNOTAVAIL
    luaX_set_field(L, -1, "EADDRNOTAVAIL", EADDRNOTAVAIL);
#endif
#ifdef EAFNOSUPPORT
    luaX_set_field(L, -1, "EAFNOSUPPORT", EAFNOSUPPORT);
#endif
#ifdef EAGAIN
    luaX_set_field(L, -1, "EAGAIN", EAGAIN);
#endif
#ifdef EALREADY
    luaX_set_field(L, -1, "EALREADY", EALREADY);
#endif
#ifdef EBADF
    luaX_set_field(L, -1, "EBADF", EBADF);
#endif
#ifdef EBADMSG
    luaX_set_field(L, -1, "EBADMSG", EBADMSG);
#endif
#ifdef EBUSY
    luaX_set_field(L, -1, "EBUSY", EBUSY);
#endif
#ifdef ECANCELED
    luaX_set_field(L, -1, "ECANCELED", ECANCELED);
#endif
#ifdef ECHILD
    luaX_set_field(L, -1, "ECHILD", ECHILD);
#endif
#ifdef ECONNABORTED
    luaX_set_field(L, -1, "ECONNABORTED", ECONNABORTED);
#endif
#ifdef ECONNREFUSED
    luaX_set_field(L, -1, "ECONNREFUSED", ECONNREFUSED);
#endif
#ifdef ECONNRESET
    luaX_set_field(L, -1, "ECONNRESET", ECONNRESET);
#endif
#ifdef EDEADLK
    luaX_set_field(L, -1, "EDEADLK", EDEADLK);
#endif
#ifdef EDESTADDRREQ
    luaX_set_field(L, -1, "EDESTADDRREQ", EDESTADDRREQ);
#endif
#ifdef EDOM
    luaX_set_field(L, -1, "EDOM", EDOM);
#endif
#ifdef EDQUOT
    luaX_set_field(L, -1, "EDQUOT", EDQUOT);
#endif
#ifdef EEXIST
    luaX_set_field(L, -1, "EEXIST", EEXIST);
#endif
#ifdef EFAULT
    luaX_set_field(L, -1, "EFAULT", EFAULT);
#endif
#ifdef EFBIG
    luaX_set_field(L, -1, "EFBIG", EFBIG);
#endif
#ifdef EHOSTUNREACH
    luaX_set_field(L, -1, "EHOSTUNREACH", EHOSTUNREACH);
#endif
#ifdef EIDRM
    luaX_set_field(L, -1, "EIDRM", EIDRM);
#endif
#ifdef EILSEQ
    luaX_set_field(L, -1, "EILSEQ", EILSEQ);
#endif
#ifdef EINPROGRESS
    luaX_set_field(L, -1, "EINPROGRESS", EINPROGRESS);
#endif
#ifdef EINTR
    luaX_set_field(L, -1, "EINTR", EINTR);
#endif
#ifdef EINVAL
    luaX_set_field(L, -1, "EINVAL", EINVAL);
#endif
#ifdef EIO
    luaX_set_field(L, -1, "EIO", EIO);
#endif
#ifdef EISCONN
    luaX_set_field(L, -1, "EISCONN", EISCONN);
#endif
#ifdef EISDIR
    luaX_set_field(L, -1, "EISDIR", EISDIR);
#endif
#ifdef ELOOP
    luaX_set_field(L, -1, "ELOOP", ELOOP);
#endif
#ifdef EMFILE
    luaX_set_field(L, -1, "EMFILE", EMFILE);
#endif
#ifdef EMLINK
    luaX_set_field(L, -1, "EMLINK", EMLINK);
#endif
#ifdef EMSGSIZE
    luaX_set_field(L, -1, "EMSGSIZE", EMSGSIZE);
#endif
#ifdef EMULTIHOP
    luaX_set_field(L, -1, "EMULTIHOP", EMULTIHOP);
#endif
#ifdef ENAMETOOLONG
    luaX_set_field(L, -1, "ENAMETOOLONG", ENAMETOOLONG);
#endif
#ifdef ENETDOWN
    luaX_set_field(L, -1, "ENETDOWN", ENETDOWN);
#endif
#ifdef ENETRESET
    luaX_set_field(L, -1, "ENETRESET", ENETRESET);
#endif
#ifdef ENETUNREACH
    luaX_set_field(L, -1, "ENETUNREACH", ENETUNREACH);
#endif
#ifdef ENFILE
    luaX_set_field(L, -1, "ENFILE", ENFILE);
#endif
#ifdef ENOBUFS
    luaX_set_field(L, -1, "ENOBUFS", ENOBUFS);
#endif
#ifdef ENODATA
    luaX_set_field(L, -1, "ENODATA", ENODATA);
#endif
#ifdef ENODEV
    luaX_set_field(L, -1, "ENODEV", ENODEV);
#endif
#ifdef ENOENT
    luaX_set_field(L, -1, "ENOENT", ENOENT);
#endif
#ifdef ENOEXEC
    luaX_set_field(L, -1, "ENOEXEC", ENOEXEC);
#endif
#ifdef ENOLCK
    luaX_set_field(L, -1, "ENOLCK", ENOLCK);
#endif
#ifdef ENOLINK
    luaX_set_field(L, -1, "ENOLINK", ENOLINK);
#endif
#ifdef ENOMEM
    luaX_set_field(L, -1, "ENOMEM", ENOMEM);
#endif
#ifdef ENOMSG
    luaX_set_field(L, -1, "ENOMSG", ENOMSG);
#endif
#ifdef ENOPROTOOPT
    luaX_set_field(L, -1, "ENOPROTOOPT", ENOPROTOOPT);
#endif
#ifdef ENOSPC
    luaX_set_field(L, -1, "ENOSPC", ENOSPC);
#endif
#ifdef ENOSR
    luaX_set_field(L, -1, "ENOSR", ENOSR);
#endif
#ifdef ENOSTR
    luaX_set_field(L, -1, "ENOSTR", ENOSTR);
#endif
#ifdef ENOSYS
    luaX_set_field(L, -1, "ENOSYS", ENOSYS);
#endif
#ifdef ENOTCONN
    luaX_set_field(L, -1, "ENOTCONN", ENOTCONN);
#endif
#ifdef ENOTDIR
    luaX_set_field(L, -1, "ENOTDIR", ENOTDIR);
#endif
#ifdef ENOTEMPTY
    luaX_set_field(L, -1, "ENOTEMPTY", ENOTEMPTY);
#endif
#ifdef ENOTRECOVERABLE
    luaX_set_field(L, -1, "ENOTRECOVERABLE", ENOTRECOVERABLE);
#endif
#ifdef ENOTSOCK
    luaX_set_field(L, -1, "ENOTSOCK", ENOTSOCK);
#endif
#ifdef ENOTSUP
    luaX_set_field(L, -1, "ENOTSUP", ENOTSUP);
#endif
#ifdef ENOTTY
    luaX_set_field(L, -1, "ENOTTY", ENOTTY);
#endif
#ifdef ENXIO
    luaX_set_field(L, -1, "ENXIO", ENXIO);
#endif
#ifdef EOPNOTSUPP
    luaX_set_field(L, -1, "EOPNOTSUPP", EOPNOTSUPP);
#endif
#ifdef EOVERFLOW
    luaX_set_field(L, -1, "EOVERFLOW", EOVERFLOW);
#endif
#ifdef EOWNERDEAD
    luaX_set_field(L, -1, "EOWNERDEAD", EOWNERDEAD);
#endif
#ifdef EPERM
    luaX_set_field(L, -1, "EPERM", EPERM);
#endif
#ifdef EPIPE
    luaX_set_field(L, -1, "EPIPE", EPIPE);
#endif
#ifdef EPROTO
    luaX_set_field(L, -1, "EPROTO", EPROTO);
#endif
#ifdef EPROTONOSUPPORT
    luaX_set_field(L, -1, "EPROTONOSUPPORT", EPROTONOSUPPORT);
#endif
#ifdef EPROTOTYPE
    luaX_set_field(L, -1, "EPROTOTYPE", EPROTOTYPE);
#endif
#ifdef ERANGE
    luaX_set_field(L, -1, "ERANGE", ERANGE);
#endif
#ifdef EROFS
    luaX_set_field(L, -1, "EROFS", EROFS);
#endif
#ifdef ESPIPE
    luaX_set_field(L, -1, "ESPIPE", ESPIPE);
#endif
#ifdef ESRCH
    luaX_set_field(L, -1, "ESRCH", ESRCH);
#endif
#ifdef ESTALE
    luaX_set_field(L, -1, "ESTALE", ESTALE);
#endif
#ifdef ETIME
    luaX_set_field(L, -1, "ETIME", ETIME);
#endif
#ifdef ETIMEDOUT
    luaX_set_field(L, -1, "ETIMEDOUT", ETIMEDOUT);
#endif
#ifdef ETXTBSY
    luaX_set_field(L, -1, "ETXTBSY", ETXTBSY);
#endif
#ifdef EWOULDBLOCK
    luaX_set_field(L, -1, "EWOULDBLOCK", EWOULDBLOCK);
#endif
#ifdef EXDEV
    luaX_set_field(L, -1, "EXDEV", EXDEV);
#endif
  }
}
