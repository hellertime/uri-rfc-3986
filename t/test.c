#include <stdio.h>

#include "uri.h"

static const char *uri_state_strings[] =
{
#define F(id, symbol, string) #string,
	URI_STATE_MAP(F)
#undef F
};

static const struct {
	const char *uri;
	const unsigned int n_states;
	const uri_state_t expected_states[16];
} uri_tests[] = {
	{ "ftp://example.org/resource.txt", 5, { URI_PARSE_RESET, URI_HAS_SCHEME, URI_HAS_HOST, URI_HAS_PATH, URI_PARSE_DONE } }
,	{ "urn:issn:1535-3613", 4, { URI_PARSE_RESET, URI_HAS_SCHEME, URI_HAS_PATH, URI_PARSE_DONE } }
,	{ "http://en.wikipedia.org/wiki/URI#Examples_of_URI_references", 6, { URI_PARSE_RESET, URI_HAS_SCHEME, URI_HAS_HOST, URI_HAS_PATH, URI_HAS_FRAGMENT, URI_PARSE_DONE } }
,	{ "//example.org/scheme-relative/URI/with/absolute/path/to/resource.txt", 4, { URI_PARSE_RESET, URI_HAS_HOST, URI_HAS_PATH, URI_PARSE_DONE } }
,	{ "/relative/URI/with/absolute/path/to/resource.txt", 3, { URI_PARSE_RESET, URI_HAS_PATH, URI_PARSE_DONE } }
,	{ "relative/path/to/resource.txt", 3, { URI_PARSE_RESET, URI_HAS_PATH, URI_PARSE_DONE } }
,	{ "../../../resource.txt", 3, { URI_PARSE_RESET, URI_HAS_PATH, URI_PARSE_DONE } }
,	{ "./resource.txt#frag01", 4, { URI_PARSE_RESET, URI_HAS_PATH, URI_HAS_FRAGMENT, URI_PARSE_DONE } }
,	{ "resource.txt", 3, { URI_PARSE_RESET, URI_HAS_PATH, URI_PARSE_DONE } }
,	{ "#frag01", 4, { URI_PARSE_RESET, URI_HAS_EMPTY_PATH, URI_HAS_FRAGMENT, URI_PARSE_DONE } }
,	{ "", 3, { URI_PARSE_RESET, URI_HAS_EMPTY_PATH, URI_PARSE_DONE } }
};

int main(void)
{
	uri_t uri;
	uri_state_t s;
	
	for (unsigned int i = 0; i < sizeof(uri_tests)/sizeof(uri_tests[0]); i++)
	{
		printf("[%04d] '%s': ", i, uri_tests[i].uri);

		s = uri_init(&uri, uri_tests[i].uri);
		for (unsigned int j = 0;  j < uri_tests[i].n_states && s != URI_PARSE_DONE; s = uri_parse_next_component(&uri), j++)
		{
			if (s != uri_tests[i].expected_states[j])
			{
				printf("In state '%s', expected state '%s'\n", uri_state_strings[s], uri_state_strings[uri_tests[i].expected_states[j]]);
				break;
			}
		}

		if (s == URI_PARSE_DONE)
		{
			printf("OK\n");
		}
	}

	return 0;
}
