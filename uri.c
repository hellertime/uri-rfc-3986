#include "uri.h"

#include <stddef.h>

#define LOWER(c) (unsigned char)(c | 0x20)
#define IS_ALPHA(c) (LOWER(c) >= 'a' && LOWER(c) <= 'z')
#define IS_DIGIT(c) ((c) >= '0' && (c) <= '9')
#define IS_HEX(c) (IS_DIGIT(c) || (LOWER(c) >= 'a' && LOWER(c) <= 'f'))

/* rfc 3986 definitions */

/*
 * sub-delims = "!" / "$" / "&" / "'" / "(" / ")"
 *            / "*" / "+" / "," / ";" / "="
 */
#define IS_SUB_DELIM(c) (((c) >= '!' && (c) <= '=') && (((c) >= '&' && (c) <= ',') || (c) == '!' || (c) == '$' || (c) == ';' || (c) == '='))

/*
 * gen-delims = ":" / "/" / "?" / "#" / "[" / "]" / "@"
 */
#define IS_GEN_DELIM(c) ((c) == ':' || (c) == '/' || (c) == '?' || c == '#' || c == '[' || c == ']' || c == '@')

/*
 * reserved = gen-delims / sub-delims
 */
#define IS_RESERVED(c) (IS_GEN_DELIM(c) || IS_SUB_DELIM(c))

/*
 * unreserved = ALPHA / DIGIT / "-" / "." / "_" / "~"
 */
#define IS_UNRESERVED(c) (IS_ALPHA(c) || IS_DIGIT(c) || (c) == '-' || (c) == '.' || (c) == '_' || (c) == '~')

/*
 * pct-encoded = "%" HEXDIG HEXDIG
 */
static const char* scout_pct_encoded(const char *c)
{
	return (*c == '%' && IS_HEX(*(c + 1)) && IS_HEX(*(c + 2))) ? (c + 2) : NULL;
}

/*
 * pchar = unreserved / pct-encoded / sub-delims / ":" / "@"
 */
static const char* scout_pchar(const char *c)
{
	return (IS_UNRESERVED(*c) || IS_SUB_DELIM(*c) || *c == ':' || *c == '@') ? c : scout_pct_encoded(c);
}

/*
 * query = *( pchar / "/" / "?" )
 */
static const char* scout_query(const char *c)
{
	const char *p = NULL;
	do
	{
		switch (*c)
		{
		default:

			c = scout_pchar(c);
			if (c != NULL) p = c++;
			break;

		case '/':
		case '?':

			p = c++;
			break;
		}
	} while (c != NULL);

	return p;
}

/*
 * fragment = *( pchar / "/" / "?" )
 */
#define scout_fragment scout_query

/*
 * segment = *pchar
 * segment-nz = 1*pchar
 * segment-nz-nc = 1*( unreserved / pct-encoded / sub-delims / "@" )
 */
static const char* scout_any_segment(const char *c, char exclude)
{
	const char *p = NULL;

	c = scout_pchar(c);
	while (c != NULL && *c != exclude)
	{
		p = c++;
		c = scout_pchar(c);
	}

	return p;
}

#define scout_segment(c) scout_any_segment(c, 0)
#define scout_segment_nz(c) scout_any_segment(c, 0)
#define scout_segment_nz_nc(c) scout_any_segment(c, ':')

/*
 * userinfo = *( unreserved / pct-encoded / sub-delims / ":" )
 */
#define scout_user_info(c) scout_any_segment(c, '@')

/*
 * reg-name = *( unreserved / pct-encoded / sub-delims )
 */
static const char* scout_reg_name(const char *c)
{
	const char *p = NULL;

	do
	{
		if (IS_UNRESERVED(*c) || IS_SUB_DELIM(*c)) p = c++;
		else if (scout_pct_encoded(c) != NULL) {
			p = c + 2;
			c += 3;
		}
		else c = NULL;
	} while (c != NULL);

	return p;
}

/*
 * path-abempty = *( "/" segment )
 */
static const char* scout_path_abempty(const char *c)
{
	const char *p = NULL;

	do
	{
		if (*c != '/') break;
		p = c++;
		c = scout_segment(c);
		if (c != NULL) p = c++;
	} while (c != NULL);

	return p;
}

/*
 * path-rootless = segment-nz *( "/" segment )
 */
static const char* scout_path_rootless(const char *c)
{
	const char *p = NULL;

	c = scout_segment_nz(c);
	if (c != NULL) {
		p = c++;

		do
		{
			c = scout_path_abempty(c);
			if (c != NULL) p = c++;
		} while (c != NULL);
	}

	return p;
}

/*
 * path-noscheme = segment-nz-nc *( "/" segment )
 */
static const char* scout_path_noscheme(const char *c)
{
	const char *p = NULL;

	c = scout_segment_nz_nc(c);
	if (c != NULL) {
		p = c++;

		do
		{
			c = scout_path_abempty(c);
			if (c != NULL) p = c++;
		} while (c != NULL);
	}

	return p;
}

/*
 * path-absolute = "/" [ segment-nz *( "/" segment ) ]
 */
static const char* scout_path_absolute(const char *c)
{
	const char *p = NULL;

	p = c++;
	c = scout_path_rootless(c);
	if (c != NULL) p = c;

	return p;
}

/*
 * path-empty = 0<pchar>
 */
static const char* scout_path_empty(const char *c)
{
	return (scout_pchar(c) == NULL) ? c : NULL;
}

/*
 * userinfo = *( unreserved / pct-encoded / sub-delims / ":" )
 */
static const char* scout_userinfo(const char *c)
{
	const char *p = c;

	do
	{
		if (IS_UNRESERVED(*c) || IS_SUB_DELIM(*c) || *c == ':') p = c++;
		else if (scout_pct_encoded(c) != NULL) {
			p = c + 2;
			c += 3;
		}
		else if (c != NULL && *c == '@') return p;
		else return NULL;
	} while (c != NULL);

	return NULL;
}

/*
 * host = IP-literal / IPv4address / reg-name
 *
 * TODO: implement this fully
 */
#define scout_host scout_reg_name

/*
 * port = *DIGIT
 */
static const char* scout_port(const char *c)
{
	const char *p = NULL;

	while (IS_DIGIT(*c))
	{
		p = c++;
	}

	return p;
}

/*
 * scheme = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
 */
static const char* scout_scheme(const char *c)
{
	const char *p = (IS_ALPHA(*c) ? c++ : NULL);

	while (p != NULL)
	{
		switch (*c)
		{
		default:

			if (IS_ALPHA(*c) || IS_DIGIT(*c)) p = c++;
			else goto exit;
			break;

		case '+':
		case '-':
		case '.':

			p = c++;
			break;
		}
	}

exit:
	return p;
}

static uri_state_t proceed(const char ** const start, const char ** const end, uri_state_t in_state)
{
	int relative_ref = 0;

	switch (in_state)
	{
	case URI_PARSE_DONE:

		*start = *end;
		return URI_PARSE_DONE;

	case URI_PARSE_RESET:

		if ((*end = scout_scheme(*start)) != NULL) {
			if (*(*end + 1) == ':') {
				(*end)++;
				return URI_HAS_SCHEME;
			}
			else {
				relative_ref = 1;
				*end = *start;
				goto proceed_relative_ref;
			}
		}
		else {
			relative_ref = 1;
			*end = *start;
			goto proceed_relative_ref;
		}

	case URI_HAS_SCHEME:

		if (**end != ':') return URI_PARSE_ERROR;

		*start = ++(*end);

proceed_relative_ref:

		switch (**start)
		{
		case '/':

			(*start)++;
			if (**start == '/') {
				(*start)++;
				if ((*end = scout_userinfo(*start)) != NULL) {
					(*end)++; 
					return URI_HAS_USERINFO;
				}
				else if ((*end = scout_host(*start)) != NULL) {
					(*end)++; 
					return URI_HAS_HOST;
				}
				else if ((**start == ':') && ((*end = scout_port(*start + 1)) != NULL)) {
					(*start)++; 
					(*end)++;
					return URI_HAS_PORT;
				}
				else if ((*end = scout_path_abempty(*start)) != NULL) {
					(*end)++;
					return URI_HAS_PATH;
				}
				else {
					*end = *start;
					return URI_HAS_PATH;
				}
			}
			else if ((*end = scout_path_absolute(*start)) != NULL) {
				(*end)++;
				return URI_HAS_PATH;
			}
			else return URI_HAS_PATH;

		default:
			if (relative_ref && ((*end = scout_path_noscheme(*start)) != NULL)) {
				(*end)++;
				return URI_HAS_PATH;
			}
			else if ((*end = scout_path_rootless(*start)) != NULL) {
				(*end)++;
				return URI_HAS_PATH;
			}
			else if ((*end = scout_path_empty(*start)) != NULL) {
				(*end)++;
				return URI_HAS_PATH;
			}
			else return URI_PARSE_ERROR;
		}

	case URI_HAS_USERINFO:

		if (**end != '@') return URI_PARSE_ERROR;

		*start = ++(*end);

		if ((*end = scout_host(*start)) != NULL) {
			(*end)++;
			return URI_HAS_HOST;
		}
		else if ((**start == ':') && ((*end = scout_port(*start + 1)) != NULL)) {
			(*start)++;
			(*end)++;
			return URI_HAS_PORT;
		}
		else if ((*end = scout_path_abempty(*start)) != NULL) {
			(*end)++;
			return URI_HAS_PATH;
		}
		else {
			*end = *start;
			return URI_HAS_PATH;
		}

	case URI_HAS_HOST:

		*start = *end;

		if ((**start == ':') && ((*end = scout_port(*start + 1)) != NULL)) {
			(*start)++;
			(*end)++;
			return URI_HAS_PORT;
		}
		else if ((*end = scout_path_abempty(*start)) != NULL) {
			(*end)++;
			return URI_HAS_PATH;
		}
		else {
			*end = *start;
			return URI_HAS_PATH;
		}

	case URI_HAS_PORT:

		*start = *end;

		if ((*end = scout_path_abempty(*start)) != NULL) {
			(*end)++;
			return URI_HAS_PATH;
		}
		else {
			*end = *start;
			return URI_HAS_PATH;
		}

	case URI_HAS_PATH:

		*start = *end;

		switch (**start)
		{
		case '?':

			(*start)++;
			if ((*end = scout_query(*start)) != NULL) {
				(*end)++;
				return URI_HAS_QUERY;
			}
			else {
				*end = *start;
				return URI_PARSE_DONE;
			}

		case '#':

			(*start)++;
			if ((*end = scout_fragment(*start)) != NULL) {
				(*end)++;
				return URI_HAS_FRAGMENT;
			}
			else {
				*end = *start;
				return URI_PARSE_DONE;
			}

		default:

			*end = *start;
			return URI_PARSE_DONE;
		}

	case URI_HAS_QUERY:

		*start = *end;

		switch (**start)
		{
		case '#':

			(*start)++;
			if ((*end = scout_fragment(*start)) != NULL) {
				(*end)++;
				return URI_HAS_FRAGMENT;
			}
			else {
				*end = *start;
				return URI_PARSE_DONE;
			}

		default:

			*end = *start;
			return URI_PARSE_DONE;
		}

	case URI_HAS_FRAGMENT:

		*start = *end;
		return URI_PARSE_DONE;

	default:

		return URI_PARSE_ERROR;
	}
}

uri_state_t uri_init(uri_t *uri, const char *uridata)
{
	uri->start = uri->end = uridata;
	scout_path_noscheme("");
	return (uri->state = URI_PARSE_RESET);
}

uri_state_t uri_proceed(uri_t *uri)
{
	return (uri->state = proceed(&uri->start, &uri->end, uri->state));
}
