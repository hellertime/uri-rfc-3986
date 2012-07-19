uri -- low overhead uri parsing routines
========================================

## SYNOPSIS

### Build and Install

```sh
make test && cp uri.[ch] $YOUR_PROJECT
```

### Using

```c
#include "uri.h"
```

### Parsing a URI

```c
uri_t uri;

uri_init(&uri, "http://www.ics.uci.edu/pub/ietf/uri/#Related");

while (uri_okay(&uri))
{
    switch (uri_getstate(&uri)) {
    default:
      uri_proceed(&uri); break;
    case URI_HAS_SCHEME:
      printf("scheme: %.*s\n", uri_get_byte_count(&uri), uri_get_pointer(&uri));
      uri_proceed(&uri);
      break;
    case URI_HAS_AUTHORITY:
    }
}
```

## DESCRIPTION

`uri` is a minimal approach to parsing URI information.

## FUNCTIONS

## LIMITATIONS
