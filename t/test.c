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
,	{ "https://www.google.com/?q=URI+percent+encoding+!*'()%3B%3A%40%26%3D%2B%24%2C%2F%3F%23%5B%5D", 6, { URI_PARSE_RESET, URI_HAS_SCHEME, URI_HAS_HOST, URI_HAS_PATH, URI_HAS_QUERY, URI_PARSE_DONE } }
,	{ "?q=URI+percent+encoding+!*'()%3B%3A%40%26%3D%2B%24%2C%2F%3F%23%5B%5D", 2, { URI_HAS_QUERY, URI_PARSE_DONE } }
,	{ "sip:biloxi.com;method=REGISTER;transport=tcp?to=sip:bob%40biloxi.com", 5, { URI_PARSE_RESET, URI_HAS_SCHEME, URI_HAS_PATH, URI_HAS_QUERY, URI_PARSE_DONE } }
,	{ "mailto:?to=joe@xyz.com&amp;cc=bob@xyz.com&amp;body=hello", 5, { URI_PARSE_RESET, URI_HAS_SCHEME, URI_HAS_EMPTY_PATH, URI_HAS_QUERY, URI_PARSE_DONE } }
,	{ "file:///c:/WINDOWS/clock.avi", 4, { URI_PARSE_RESET, URI_HAS_SCHEME, URI_HAS_PATH, URI_PARSE_DONE } }
,	{ "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAUAAAAFCAYAAACNbyblAAAAHElEQVQI12P4//8/w38GIAXDIBKE0DHxgljNBAAO9TXL0Y4OHwAAAABJRU5ErkJggg==", 4, { URI_PARSE_RESET, URI_HAS_SCHEME, URI_HAS_PATH, URI_PARSE_DONE } }
,	{ "ftp://me@you.com/my%20test.asp?name=st%C3%A5le&car=saab", 7, { URI_PARSE_RESET, URI_HAS_SCHEME, URI_HAS_USERINFO, URI_HAS_HOST, URI_HAS_PATH, URI_HAS_QUERY, URI_PARSE_DONE } }
,	{ "tag:example.com,2004:fred:", 4, { URI_PARSE_RESET, URI_HAS_SCHEME, URI_HAS_PATH, URI_PARSE_DONE } }
,	{ "qemu+unix:///system?socket=/opt/libvirt/run/libvirt/libvirt-sock", 5, { URI_PARSE_RESET, URI_HAS_SCHEME, URI_HAS_PATH, URI_HAS_QUERY, URI_PARSE_DONE } }
,	{ "test+tcp://node.example.com:5000/default", 6, { URI_PARSE_RESET, URI_HAS_SCHEME, URI_HAS_HOST, URI_HAS_PORT, URI_HAS_PATH, URI_PARSE_DONE } }
,	{ "news://server.example/ab.cd@example.com", 5, { URI_PARSE_RESET, URI_HAS_SCHEME, URI_HAS_HOST, URI_HAS_PATH, URI_PARSE_DONE } }
,	{ "geo:66,30;u=6.500;FOo=this%2dthat", 4, { URI_PARSE_RESET, URI_HAS_SCHEME, URI_HAS_PATH, URI_PARSE_DONE } }
,	{ "magnet:?xt.1=urn:sha1:YNCKHTQCWBTRNJIV4WNAE52SJUQCZO5C&xt.2=urn:sha1:TXGCZQTH26NL6OUQAJJPFALHG2LTGBC7", 4, { URI_PARSE_RESET, URI_HAS_SCHEME, URI_HAS_EMPTY_PATH, URI_HAS_QUERY, URI_PARSE_DONE } }
,	{ "tel:863-1234;phone-context=+1-914-555", 4, { URI_PARSE_RESET, URI_HAS_SCHEME, URI_HAS_PATH, URI_PARSE_DONE } }
};

int main(void)
{
	uri_t uri;
	
	for (unsigned int i = 0; i < sizeof(uri_tests)/sizeof(uri_tests[0]); i++)
	{
		printf("[%04d] '%s': ", i, uri_tests[i].uri);
		uri_state_t s = uri_init_with_state(&uri, uri_tests[i].uri, uri_tests[i].expected_states[0]);
		for (unsigned int j = 0; j < uri_tests[i].n_states && s != URI_PARSE_DONE; s = uri_parse_next_component(&uri), j++)
		{
			if (s != uri_tests[i].expected_states[j])
			{
				printf("In state '%s', expected state '%s'\n", uri_state_strings[s], uri_state_strings[uri_tests[i].expected_states[j]]);
				break;
			}
		}

		if (uri_get_state(&uri) == URI_PARSE_DONE)
		{
			printf("OK\n");
		}
	}

	return 0;
}
