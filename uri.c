#include <stddef.h>

#include "uri.h"

#define ALPHA       0x01
#define DIGIT       0x02
#define HEXIDECIMAL 0x04
#define SUB_DELIM   0x08
#define GEN_DELIM   0x10
#define UNRESERVED  0x20
#define PCHAR       0x40

#define A (unsigned char)ALPHA
#define D (unsigned char)DIGIT
#define H (unsigned char)HEXIDECIMAL
#define S (unsigned char)SUB_DELIM
#define G (unsigned char)GEN_DELIM
#define U (unsigned char)UNRESERVED
#define P (unsigned char)PCHAR

/* rfc 3986 definitions
 *
 * sub-delims = "!" / "$" / "&" / "'" / "(" / ")"
 *            / "*" / "+" / "," / ";" / "="
 * gen-delims = ":" / "/" / "?" / "#" / "[" / "]" / "@"
 * unreserved = ALPHA / DIGIT / "-" / "." / "_" / "~"
 * pchar = unreserved / pct-encoded / sub-delims / ":" / "@"
 */

static const unsigned char ascii_flags[256] = {
/* nul = 0x00, soh = 0x01, stx = 0x02, etx = 0x03, eot = 0x04, enq = 0x05, ack = 0x06, bel = 0x07 */
   0,          0,          0,          0,          0,          0,          0,          0,
/* bs  = 0x08, ht  = 0x09, lf  = 0x0a, vt  = 0x0b, ff  = 0x0c, cr  = 0x0d, so  = 0x0e, si = 0x0f  */ 
   0,          0,          0,          0,          0,          0,          0,          0,
/* dle = 0x10, dc1 = 0x11, dc2 = 0x12, dc3 = 0x13, dc4 = 0x14, nak = 0x15, syn = 0x16, etb = 0x17 */
   0,          0,          0,          0,          0,          0,          0,          0,
/* can = 0x18, em  = 0x19, sub = 0x1a, esc = 0x1b, fs  = 0x1c, gs = 0x1d,  rs  = 0x1e, us  = 0x1f */
   0,          0,          0,          0,          0,          0,          0,          0,
/* spc = 0x20, '!' = 0x21, '"' = 0x22, '#' = 0x23, '$' = 0x24, '%' = 0x25, '&' = 0x26, '\''= 0x27 */
   0,          P|S,        0,          G,          P|S,        0,          P|S,        P|S,
/* '(' = 0x28, ')' = 0x29, '*' = 0x2a, '+' = 0x2b, ',' = 0x2c, '-' = 0x2d, '.' = 0x2e, '/' = 0x2f */
   P|S,        P|S,        P|S,        P|S,        P|S,        P|U,        P|U,        G,
/* '0' = 0x30, '1' = 0x31, '2' = 0x32, '3' = 0x33, '4' = 0x34, '5' = 0x35, '6' = 0x36, '7' = 0x37 */
   P|U|H|D,    P|U|H|D,    P|U|H|D,    P|U|H|D,    P|U|H|D,    P|U|H|D,    P|U|H|D,    P|U|H|D,
/* '8' = 0x38, '9' = 0x39, ':' = 0x3a, ';' = 0x3b, '<' = 0x3c, '=' = 0x3d, '>' = 0x3e, '?' = 0x3f */
   P|U|H|D,    P|U|H|D,    P|G,        P|S,        0,          P|S,        0,          G,
/* '@' = 0x40, 'A' = 0x41, 'B' = 0x42, 'C' = 0x43, 'D' = 0x44, 'E' = 0x45, 'F' = 0x46, 'G' = 0x47 */
   P|G,        P|U|H|A,    P|U|H|A,    P|U|H|A,    P|U|H|A,    P|U|H|A,    P|U|H|A,    P|U|A,
/* 'H' = 0x48, 'I' = 0x49, 'J' = 0x4a, 'K' = 0x4b, 'L' = 0x4c, 'M' = 0x4d, 'N' = 0x4e, 'O' = 0x4f */
   P|U|A,      P|U|A,      P|U|A,      P|U|A,      P|U|A,      P|U|A,      P|U|A,      P|U|A,
/* 'P' = 0x50, 'Q' = 0x51, 'R' = 0x52, 'S' = 0x53, 'T' = 0x54, 'U' = 0x55, 'V' = 0x56, 'W' = 0x57 */
   P|U|A,      P|U|A,      P|U|A,      P|U|A,      P|U|A,      P|U|A,      P|U|A,      P|U|A,
/* 'X' = 0x58, 'Y' = 0x59, 'Z' = 0x5a, '[' = 0x5b, '\\'= 0x5c, ']' = 0x5d, '^' = 0x5e, '_' = 0x5f */
   P|U|A,      P|U|A,      P|U|A,      G,          0,          G,          0,          P|U,
/* '`' = 0x60, 'a' = 0x61, 'b' = 0x62, 'c' = 0x63, 'd' = 0x64, 'e' = 0x65, 'f' = 0x66, 'g' = 0x67 */
   0,          P|U|H|A,    P|U|H|A,    P|U|H|A,    P|U|H|A,    P|U|H|A,    P|U|H|A,    P|U|A,
/* 'h' = 0x68, 'i' = 0x69, 'j' = 0x6a, 'k' = 0x6b, 'l' = 0x6c, 'm' = 0x6d, 'n' = 0x6e, 'o' = 0x6f */
   P|U|A,      P|U|A,      P|U|A,      P|U|A,      P|U|A,      P|U|A,      P|U|A,      P|U|A,
/* 'p' = 0x70, 'q' = 0x71, 'r' = 0x72, 's' = 0x73, 't' = 0x74, 'u' = 0x75, 'v' = 0x76, 'w' = 0x77 */
   P|U|A,      P|U|A,      P|U|A,      P|U|A,      P|U|A,      P|U|A,      P|U|A,      P|U|A,
/* 'x' = 0x78, 'y' = 0x79, 'z' = 0x7a, '{' = 0x7b, '|' = 0x7c, '}' = 0x7d, '~' = 0x7e, del = 0x7f */
   P|U|A,      P|U|A,      P|U|A,      0,          0,          0,          P|U,        0          };

#undef A
#undef D
#undef H
#undef S
#undef G
#undef U
#undef P

static inline const char* scout_dec_octet(const char*) __pure;
static inline const char* scout_ipv4address(const char *c) __pure;
static inline const char* scout_h16(const char *c) __pure;
static inline const char* scout_ls32(const char *c) __pure;
static inline const char* scout_ipv6address_rh1(const char *c) __pure;
static inline const char* scout_ipv6address_rh2(const char *c) __pure;
static inline const char* scout_ipv6address_rh3(const char *c) __pure;
static inline const char* scout_ipv6address_rh4(const char *c) __pure;
static inline const char* scout_ipv6address_rh5(const char *c) __pure;
static inline const char* scout_ipv6address_rh6(const char *c) __pure;
static inline const char* scout_ipv6address(const char *c) __pure;
static inline const char* scout_ipvfuture(const char *c) __pure;
static inline const char* scout_ip_literal(const char *c) __pure;
static inline const char* scout_pct_encoded(const char*) __pure;
static inline const char* scout_pchar(const char*) __pure;
static inline const char* scout_query(const char*) __pure;
static inline const char* scout_any_segment(const char*, char) __pure;
static inline const char* scout_reg_name(const char*) __pure;
static inline const char* scout_host(const char *c) __pure;
static inline const char* scout_path_abempty(const char*) __pure;
static inline const char* scout_path_rootless(const char*) __pure;
static inline const char* scout_path_noscheme(const char*) __pure;
static inline const char* scout_path_absolute(const char*) __pure;
static inline const char* scout_path_empty(const char*) __pure;
static inline const char* scout_userinfo(const char*) __pure;
static inline const char* scout_port(const char*) __pure;
static inline const char* scout_scheme(const char*) __pure;

/*
 * dec-octet = DIGIT             ;   0-9
 *           / "1"-"9" DIGIT     ;  10-99
 *           / "1" 2DIGIT        ; 100-199
 *           / "2" "0"-"4" DIGIT ; 200-249
 *           / "25" "0"-"5"      ; 250-255
 */
static inline const char* scout_dec_octet(const char *c)
{
	if (ascii_flags[(unsigned char)*c] & DIGIT) {
		if (*c == '2') {
			if ((*(c + 1) - '0') <= ('5' - '0')) {
				if ((*(c + 1) == '5') && ((*(c + 2) - '0') <= ('5' - '0'))) return c + 2;
				else if (ascii_flags[(unsigned char)*(c + 2)] & DIGIT) return c + 2;
				else return NULL; 
			}
			else return NULL;
		}
		else if (*c == '1') {
			if ((ascii_flags[(unsigned char)*(c + 1)] & DIGIT) && (ascii_flags[(unsigned char)*(c + 2)] & DIGIT)) return c + 2;
			else if (ascii_flags[(unsigned char)*(c + 1)] & DIGIT) return c + 1;
			else return NULL;
		}
		else if (((*c - '3') <= ('9' - '3')) && (ascii_flags[(unsigned char)*(c + 1)] & DIGIT)) return c + 1;
		else return c;
	}
	else return NULL;
}

/*
 * IPv4address = dec-octet '.' dec-octet '.' dec-octet '.' dec-octet
 */
static inline const char* scout_ipv4address(const char *c)
{
	if ((c = scout_dec_octet(c)) == NULL) return NULL;
	if (*(++c) != '.') return NULL;
	if ((c = scout_dec_octet(c + 1)) == NULL) return NULL;
	if (*(++c) != '.') return NULL;
	if ((c = scout_dec_octet(c + 1)) == NULL) return NULL;
	if (*(++c) != '.') return NULL;
	return scout_dec_octet(c + 1);
}

/*
 * h16 = 1*4HEXDIG
 */
static inline const char* scout_h16(const char *c)
{
	const char *p = NULL, *c0 = c;

	while (ascii_flags[(unsigned char)*c] & HEXIDECIMAL)
		p = c++;

	return (p == NULL || p - c0 < 4) ? p : NULL; 
}

/*
 * ls32 = ( h16 ":" h16 ) / IPv4address
 */
static inline const char* scout_ls32(const char *c)
{
	const char *p = scout_ipv4address(c);

	if (p == NULL) {
		p = scout_h16(c);
		if ( p != NULL) {
			c = p + 1;
			if (*c == ':') return scout_h16(c + 1);
			else return NULL;
		}
		else return NULL;
	}

	return p;
}


/*
 * h16 ":" ls32
 */
static inline const char* scout_ipv6address_rh1(const char *c)
{
	if ((c = scout_h16(c)) == NULL) return NULL;
	if (*(c + 1) == ':') return scout_ls32(c + 2);
	return NULL;
}

/*
 * N( h16 ":" ) ls32
 */
#define SCOUT_IPV6ADDRESS_RH(N,M) static inline const char* scout_ipv6address_rh##N (const char *c) { \
	if ((c = scout_h16(c)) == NULL) return NULL;                                                  \
	if (*(c + 1) == ':') return scout_ipv6address_rh##M (c + 2);                                  \
	return NULL;                                                                                  \
}

SCOUT_IPV6ADDRESS_RH(2,1)
SCOUT_IPV6ADDRESS_RH(3,2)
SCOUT_IPV6ADDRESS_RH(4,3)
SCOUT_IPV6ADDRESS_RH(5,4)
SCOUT_IPV6ADDRESS_RH(6,5)

/*
 * IPv6address =                            6( h16 ":" ) ls32
 *             /                       "::" 5( h16 ":" ) ls32
 *             / [               h16 ] "::" 4( h16 ":" ) ls32
 *             / [ *1( h16 ":" ) h16 ] "::" 3( h16 ":" ) ls32
 *             / [ *2( h16 ":" ) h16 ] "::" 2( h16 ":" ) ls32
 *             / [ *3( h16 ":" ) h16 ] "::"    h16 ":"   ls32
 *             / [ *4( h16 ":" ) h16 ] "::"              ls32
 *             / [ *5( h16 ":" ) h16 ] "::"              h16
 *             / [ *6( h16 ":" ) h16 ] "::"
 */
static inline const char* scout_ipv6address(const char *c)
{
	const char *p = NULL;
	int left_hand_count = 0;

	if ((p = scout_ipv6address_rh6(c)) != NULL) return p;
	else {
		p = c;
		c = scout_h16(c);
		while (c != NULL)
		{
			p = c++;
			left_hand_count++;
			if (*c == ':') c = scout_h16(c + 1);
		}

		p += (left_hand_count == 0) ? 2 : 3;

		switch (left_hand_count)
		{
		case 0: if ((c = scout_ipv6address_rh5(p)) != NULL) return c;
		case 1: if ((c = scout_ipv6address_rh4(p)) != NULL) return c;
		case 2: if ((c = scout_ipv6address_rh3(p)) != NULL) return c;
		case 3: if ((c = scout_ipv6address_rh2(p)) != NULL) return c;
		case 4: if ((c = scout_ipv6address_rh1(p)) != NULL) return c;
		case 5: if ((c = scout_ls32(p)) != NULL) return c;
		case 6: if ((c = scout_h16(p)) != NULL) return c;
		case 7: return p;
		default: return NULL;
		}
	}
}

/*
 * IPvFuture = "v" 1*HEXDIG "." 1*( unreserved / sub-delims / ":" )
 */
static inline const char* scout_ipvfuture(const char *c)
{
	if (*c == 'v') {
		while (ascii_flags[(unsigned char)*c] & HEXIDECIMAL)
			c++;

		if (*c == '.') {
			while ((ascii_flags[(unsigned char)*c] & (UNRESERVED | SUB_DELIM)) || *c == ':')
				c++;

			return c;
		}
	}

	return NULL;
}

/*
 * IP-literal = '[' ( IPv6address / IPvFuture ) ']'
 */
static inline const char* scout_ip_literal(const char *c)
{
	if (*c == '[') {
		const char *p = scout_ipv6address(c + 1);
		if (p != NULL && *(p + 1) == ']') return p + 1;
		else if ((p = scout_ipvfuture(c + 1)) != NULL && *(p + 1) == ']') return p + 1;
		else return NULL;
	}

	return NULL;
}

/*
 * pct-encoded = "%" HEXDIG HEXDIG
 */
static inline const char* scout_pct_encoded(const char *c)
{
	return (*c == '%' && (ascii_flags[(unsigned char)*(c + 1)] & HEXIDECIMAL) && (ascii_flags[(unsigned char)*(c + 2)] & HEXIDECIMAL)) ? (c + 2) : NULL;
}

static inline const char* scout_pchar(const char *c)
{
	return (ascii_flags[(unsigned char)*c] & PCHAR) ? c : scout_pct_encoded(c);
}

/*
 * query = *( pchar / "/" / "?" )
 */
static inline const char* scout_query(const char *c)
{
	const char *p = NULL;
	do
	{
		if (*c == '/' || *c == '?') p = c++;
		else if ((c = scout_pchar(c)) != NULL) p = c++; 
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
static inline const char* scout_any_segment(const char *c, char exclude)
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
static inline const char* scout_reg_name(const char *c)
{
	const char *p = NULL;

	do
	{
		if (ascii_flags[(unsigned char)*c] & (UNRESERVED | SUB_DELIM)) p = c++;
		else if ((c = scout_pct_encoded(c)) != NULL) p = c++;
	} while (c != NULL);

	return p;
}

/*
 * path-abempty = *( "/" segment )
 */
static inline const char* scout_path_abempty(const char *c)
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
static inline const char* scout_path_rootless(const char *c)
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
static inline const char* scout_path_noscheme(const char *c)
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
static inline const char* scout_path_absolute(const char *c)
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
static inline const char* scout_path_empty(const char *c)
{
	return (scout_pchar(c) == NULL) ? c : NULL;
}

/*
 * userinfo = *( unreserved / pct-encoded / sub-delims / ":" )
 */
static inline const char* scout_userinfo(const char *c)
{
	const char *p = c;

	do
	{
		if ((ascii_flags[(unsigned char)*c] & (UNRESERVED | SUB_DELIM)) || *c == ':') p = c++;
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
 */
static inline const char* scout_host(const char *c)
{
	const char *p = scout_reg_name(c);
	if (p == NULL) {
		if ((p = scout_ipv4address(c)) == NULL)
			return scout_ip_literal(c);
	}

	return p;
}

/*
 * port = *DIGIT
 */
static inline const char* scout_port(const char *c)
{
	const char *p = NULL;

	while (ascii_flags[(unsigned char)*c] & DIGIT)
	{
		p = c++;
	}

	return p;
}

/*
 * scheme = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
 */
static inline const char* scout_scheme(const char *c)
{
	const char *p;

	if (ascii_flags[(unsigned char)*c] & ALPHA)
		p = c++;
	else
		return NULL;

	while (c != NULL)
	{
		switch (*c)
		{
		default:

			if (ascii_flags[(unsigned char)*c] & (ALPHA | DIGIT)) p = c++;
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

				goto proceed_host;
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
				return URI_HAS_EMPTY_PATH;
			}
			else return URI_PARSE_ERROR;
		}

	case URI_HAS_USERINFO:

		if (**end != '@') return URI_PARSE_ERROR;

		*start = ++(*end);

proceed_host:

		if ((*end = scout_host(*start)) != NULL) {
			(*end)++;
			return URI_HAS_HOST;
		}

		goto proceed_port;

	case URI_HAS_HOST:

		*start = *end;

proceed_port:

		if ((**start == ':') && ((*end = scout_port(*start + 1)) != NULL)) {
			(*start)++;
			(*end)++;
			return URI_HAS_PORT;
		}

		goto proceed_path_abempty;

	case URI_HAS_PORT:

		*start = *end;

proceed_path_abempty:

		if ((*end = scout_path_abempty(*start)) != NULL) {
			(*end)++;
			return URI_HAS_PATH;
		}
		else {
			*end = *start;
			return URI_HAS_EMPTY_PATH;
		}

	case URI_HAS_PATH:
	case URI_HAS_EMPTY_PATH:

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

			goto proceed_fragment;

		default:

			*end = *start;
			return URI_PARSE_DONE;
		}

	case URI_HAS_QUERY:

		*start = *end;

		switch (**start)
		{
		case '#':

proceed_fragment:

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

uri_state_t uri_init_with_state(uri_t *uri, const char *uridata, uri_state_t in_state)
{
	uri->start = uri->end = uri->data = uridata;
	return (uri->state = in_state);
}

uri_state_t uri_init(uri_t *uri, const char *uridata)
{
	return uri_init_with_state(uri, uridata, URI_PARSE_RESET);

}

size_t uri_get_bytes_parsed(const uri_t *uri)
{
	return (uri->end - uri->data);
}

const char* uri_get_component_pointer(const uri_t *uri)
{
	return uri->start;
}

size_t uri_get_component_size(const uri_t *uri)
{
	return (uri->end - uri->start);
}

uri_state_t uri_get_state(const uri_t *uri)
{
	return uri->state;
}

uri_state_t uri_parse_next_component(uri_t *uri)
{
	return (uri->state = proceed(&uri->start, &uri->end, uri->state));
}
