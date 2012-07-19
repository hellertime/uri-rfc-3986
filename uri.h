#ifndef URI_H_INCLUDED
#define URI_H_INCLUDED

typedef enum
{
	URI_STATE_DONE
,	URI_STATE_INIT


	/* Top-level URI components */

,	URI_STATE_HAS_SCHEME
,	URI_STATE_HAS_AUTHORITY
,	URI_STATE HAS_PATH
,	URI_STATE_HAS_QUERY
,	URI_STATE_HAS_FRAGMENT

,	URI_STATE_ERROR
} uri_state_t;

typedef struct uri_t
{
	const char *start;
	const char *end;
	uri_state_t state;
} uri_t;

#endif
