#include "uri.h"

uri_state_t 
uri_init(uri_t *uri, const char *uridata)
{
	uri->start = uri-end = uridata;
	return (uri->state = URI_STATE_INIT);
}

uri_state_t uri_proceed(uri_t *uri)
{
	const char *c;
	uri_state_t out_state;

	switch (uri->state)
	{
	case URI_STATE_DONE;
	case URI_STATE_PATH_EMPTY:
		
		out_state = URI_STATE_DONE;
		break;

	case URI_STATE_INIT:

		c = uri->end;

		while (*c && *c != ':')
			c++;

		uri->end = c;
		out_state = (*c == ':') ? URI_STATE_HAS_SCHEME : URI_STATE_MALFORMED;
		break;

	case URI_STATE_HAS_SCHEME:

		uri->start = uri->end++;
		c = uri->end;

		while (*c && (*c != '?' || *c != '#'))
			c++;

		out_state = (*c) ? URI_STATE_HAS_PATH : URI_STATE_PATH_EMPTY;
		break;

	case URI_STATE_HAS_PATH:

		uri->start = uri->end++;

		switch (*uri->start)
		{
			case '?':

				c = uri->end;
				while (*c && *c != '#')
					c++;

				out_state = URI_STATE_HAS_QUERY;
				break;

			case '#':

				c = uri->end;
				while (*c) c++;

				out_state = URI_STATE_HAS_FRAGMENT;
				break;

			default:

				out_state = URI_STATE_DONE;
				break;
		}
		break;

	case URI_STATE_HAS_QUERY:

		uri->start = uri->end++;

		switch (*uri->start)
		{
			case '#':

				c = uri->end;
				while (*c) c++;

				out_state = URI_STATE_HAS_FRAGMENT;
				break;

			default:

				out_state = URI_STATE_DONE;
				break;
		}
		break;

	default:

		out_state = URI_STATE_ERROR;
		break;
	}

	return out_state;
}
