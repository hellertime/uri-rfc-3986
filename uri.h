#ifndef URI_H_INCLUDED
#define URI_H_INCLUDED

typedef enum
{
	URI_STATE_DONE
,	URI_STATE_INIT

	/* Transitional states */
,	URI_STATE_SEEK_SOLIDUS

	/* Top-level URI components */

,	URI_STATE_HAS_SCHEME
,	URI_STATE_HAS_AUTHORITY
,	URI_STATE HAS_PATH
,	URI_STATE_HAS_QUERY
,	URI_STATE_HAS_FRAGMENT

	/* Authority components */

,	URI_STATE_HAS_USER_INFO
,	URI_STATE_HAS_HOST
,	URI_STATE_HAS_PORT

	/* Path components */

,	URI_STATE_HAS_EMPTY_PATH
,	URI_STATE_HAS_ABSPATH
,	URI_STATE_HAS_RELPATH
,	URI_STATE_HAS_SEGMENT		/** allow for visiting each path segment if desired */

,	URI_STATE_ERROR
} uri_state_t;

typedef struct uri_t
{
	const char *start;
	const char *end;
	uri_state_t state;
} uri_t;

#endif
