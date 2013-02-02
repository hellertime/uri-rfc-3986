[![Build Status](https://travis-ci.org/hellertime/uri-rfc-3986.png?branch=master)](https://travis-ci.org/hellertime/uri-rfc-3986)

uri -- low overhead uri parsing routines
========================================

## SYNOPSIS

### Build and Install

```sh
make && cp uri.[ch] $YOUR_PROJECT
```

### Using

```c
#include "uri.h"
```

### Parsing a URI

```c
uri_t uri;

for (uri_state_t s = uri_init(&uri, "http://www.ics.uci.edu/pub/ietf/uri/#Related"); s != URI_PARSE_DONE || s != URI_PARSE_ERROR; )
{
	switch (s)
	{
	default:

		s = uri_parse_next_component(&uri);
		break;

	case URI_HAS_FRAGMENT:

		printf("frag: %.*s\n", uri_get_component_size(&uri), uri_get_component_pointer(&uri));
		s = uri_parse_next_component(&uri);
		break;

	}
}

printf("Parse status: %s\n" (uri_get_state(&uri) == URI_PARSE_DONE ? "DONE" : "ERROR"));
```

## DESCRIPTION

`uri` is a minimal approach to parsing URI information.

### FUNCTIONS

* `uri_init(uri_t *, const char *)`
* `uri_init_with_state(uri_t *, const char *, uri_state_t)`

Use these functions to initialize the URI parser (initial state is `URI_PARSE_RESET` if unspecified).

* `uri_get_bytes_parsed(const uri_t *)`
* `uri_get_component_pointer(const uri_t *)`
* `uri_get_component_size(const uri_t *)`
* `uri_get_state(const uri_t *)`

Use these functions to access the values of the current URI parse state.

* `uri_parse_next_component(uri_t *)`

Use this function to move the URI parser into the next state.

### STATES

* `URI_PARSE_DONE` parser has successully parsed a URI.
* `URI_PARSE_RESET` default state of parser when no initial state is specified.
* `URI_PARSE_ERROR` parser encountered an error while parsing the URI.
* `URI_HAS_SCHEME` parsed a URI scheme component.
* `URI_HAS_USERINFO` parsed a URI authority userinfo component.
* `URI_HAS_HOST` parsed a URI authority host component.
* `URI_HAS_PORT` parsed a URI authority port component.
* `URI_HAS_PATH` parsed a URI path component.
* `URI_HAS_EMPTY_PATH` parsed a URI empty-path component.
* `URI_HAS_QUERY` parsed a URI query component.
* `URI_HAS_FRAGMENT` parsed a URI fragment component.

## LICENSE

Licensed under the GPL v3, see COPYING for details.
