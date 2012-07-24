#ifndef URI_H_INCLUDED
#define URI_H_INCLUDED

#define URI_STATE_MAP(F)			\
	F(0,	PARSE_DONE,	PARSE_DONE)	\
	F(1,	PARSE_RESET,	PARSE_RESET)	\
	F(2,	PARSE_ERROR,	PARSE_ERROR)	\
	/* top-level states */			\
	F(3,	HAS_SCHEME,	HAS_SCHEME)	\
	F(4,	HAS_USERINFO,	HAS_USERINFO)	\
	F(5,	HAS_HOST,	HAS_HOST)	\
	F(6,	HAS_PORT,	HAS_PORT)	\
	F(7,	HAS_PATH,	HAS_PATH)	\
	F(8,	HAS_EMPTY_PATH,	HAS_EMPTY_PATH)	\
	F(9,	HAS_QUERY,	HAS_QUERY)	\
	F(10,	HAS_FRAGMENT,	HAS_FRAGMENT)	\

typedef enum
{
#define F(id, symbol, _) URI_##symbol = id,
	URI_STATE_MAP(F)
#undef F
} uri_state_t;

typedef struct uri_t
{
	const char *data;
	const char *start;
	const char *end;
	uri_state_t state;
} uri_t;

uri_state_t uri_init(uri_t *, const char *);
uri_state_t uri_init_with_state(uri_t *, const char *, uri_state_t);

const char* uri_get_component_pointer(const uri_t *);
size_t uri_get_component_size(const uri_t *);

size_t uri_get_bytes_parsed(const uri_t *);

uri_state_t uri_parse_next_component(uri_t *);

#endif
