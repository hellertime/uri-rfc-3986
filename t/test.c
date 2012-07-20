#include <stdio.h>

#include "uri.h"

static const char *uri_state_strings[] =
{
#define F(id, symbol, string) #string,
	URI_STATE_MAP(F)
#undef F
};

static const char *uri_examples[] =
{
	"http://a"
,	"http://a:1000"
,	"http://a/b"
,	"http://a:1000/b"
,	"http://a@:1000"
,	"http://a@:1000/c"
,	"http://a@b/c"
,	"http://a@b:1000/c"
};

int main(void)
{
	uri_t uri;
	uri_state_t s;
	
	for (unsigned int i = 0; i < sizeof(uri_examples)/sizeof(uri_examples[0]); i++)
	{
		printf("URI Test #%04d: '%s'\n", i, uri_examples[i]);
		for (s = uri_init(&uri, uri_examples[i]); s != URI_PARSE_DONE && s != URI_PARSE_ERROR; s = uri_proceed(&uri))
		{
			printf(".... (%s): %.*s\n", uri_state_strings[s], uri.end - uri.start, uri.start);
		}

		if (s == URI_PARSE_ERROR)
		{
			printf(".... (%s): %s\n", uri_state_strings[s], uri.end);
		}
	}

	return 0;
}
