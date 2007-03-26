/*
 * uriparser - RFC 3986 URI parsing library
 *
 * Copyright (C) 2007, Weijia Song <songweijia@gmail.com>
 * Copyright (C) 2007, Sebastian Pipping <webmaster@hartwork.org>
 * All rights reserved.
 *
 * Redistribution  and use in source and binary forms, with or without
 * modification,  are permitted provided that the following conditions
 * are met:
 *
 *     * Redistributions   of  source  code  must  retain  the   above
 *       copyright  notice, this list of conditions and the  following
 *       disclaimer.
 *
 *     * Redistributions  in  binary  form must  reproduce  the  above
 *       copyright  notice, this list of conditions and the  following
 *       disclaimer   in  the  documentation  and/or  other  materials
 *       provided with the distribution.
 *
 *     * Neither  the name of the <ORGANIZATION> nor the names of  its
 *       contributors  may  be  used to endorse  or  promote  products
 *       derived  from  this software without specific  prior  written
 *       permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT  NOT
 * LIMITED  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND  FITNESS
 * FOR  A  PARTICULAR  PURPOSE ARE DISCLAIMED. IN NO EVENT  SHALL  THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL,    SPECIAL,   EXEMPLARY,   OR   CONSEQUENTIAL   DAMAGES
 * (INCLUDING,  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES;  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT  LIABILITY,  OR  TORT (INCLUDING  NEGLIGENCE  OR  OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file Uri.c
 * Holds the RFC 3986 %URI parser implementation.
 * NOTE: This source file includes itself twice.
 */

/* What encodings are enabled? */
#include <uriparser/UriDefsConfig.h>
#if (!defined(URI_PASS_ANSI) && !defined(URI_PASS_UNICODE))
/* Include SELF twice */
# define URI_PASS_ANSI 1
# include "Uri.c"
# undef URI_PASS_ANSI
# define URI_PASS_UNICODE 1
# include "Uri.c"
# undef URI_PASS_UNICODE
#else
# ifdef URI_PASS_ANSI
#  include <uriparser/UriDefsAnsi.h>
# else
#  include <uriparser/UriDefsUnicode.h>
# endif



#ifndef URI_DOXYGEN
# include <uriparser/Uri.h>
# include <uriparser/UriIp4.h>
#endif



/* Prototypes */
static const URI_CHAR * URI_FUNC(ParseAuthority)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast);
static const URI_CHAR * URI_FUNC(ParseAuthorityTwo)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast);
static const URI_CHAR * URI_FUNC(ParseHexZero)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast);
static const URI_CHAR * URI_FUNC(ParseHierPart)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast);
static const URI_CHAR * URI_FUNC(ParseIpFutLoop)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast);
static const URI_CHAR * URI_FUNC(ParseIpFutStopGo)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast);
static const URI_CHAR * URI_FUNC(ParseIpLit2)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast);
static const URI_CHAR * URI_FUNC(ParseIPv6address2)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast);
static const URI_CHAR * URI_FUNC(ParseMustBeSegmentNzNc)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast);
static const URI_CHAR * URI_FUNC(ParseOwnHost)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast);
static const URI_CHAR * URI_FUNC(ParseOwnHost2)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast);
static const URI_CHAR * URI_FUNC(ParseOwnHostUserInfo)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast);
static const URI_CHAR * URI_FUNC(ParseOwnHostUserInfoNz)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast);
static const URI_CHAR * URI_FUNC(ParseOwnPortUserInfo)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast);
static const URI_CHAR * URI_FUNC(ParseOwnUserInfo)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast);
static const URI_CHAR * URI_FUNC(ParsePartHelperTwo)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast);
static const URI_CHAR * URI_FUNC(ParsePathAbsEmpty)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast);
static const URI_CHAR * URI_FUNC(ParsePathAbsNoLeadSlash)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast);
static const URI_CHAR * URI_FUNC(ParsePathRootless)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast);
static const URI_CHAR * URI_FUNC(ParsePchar)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast);
static const URI_CHAR * URI_FUNC(ParsePctEncoded)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast);
static const URI_CHAR * URI_FUNC(ParsePctSubUnres)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast);
static const URI_CHAR * URI_FUNC(ParsePort)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast);
static const URI_CHAR * URI_FUNC(ParseQueryFrag)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast);
static const URI_CHAR * URI_FUNC(ParseSegment)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast);
static const URI_CHAR * URI_FUNC(ParseSegmentNz)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast);
static const URI_CHAR * URI_FUNC(ParseSegmentNzNcOrScheme2)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast);
static const URI_CHAR * URI_FUNC(ParseUriReference)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast);
static const URI_CHAR * URI_FUNC(ParseUriTail)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast);
static const URI_CHAR * URI_FUNC(ParseUriTailTwo)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast);
static const URI_CHAR * URI_FUNC(ParseZeroMoreSlashSegs)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast);

static void URI_FUNC(OnExitOwnHost2)(URI_TYPE(ParserState) * state, const URI_CHAR * first);
static void URI_FUNC(OnExitOwnHostUserInfo)(URI_TYPE(ParserState) * state, const URI_CHAR * first);
static void URI_FUNC(OnExitOwnPortUserInfo)(URI_TYPE(ParserState) * state, const URI_CHAR * first);
static void URI_FUNC(OnExitSegmentNzNcOrScheme2)(URI_TYPE(ParserState) * state, const URI_CHAR * first);
static void URI_FUNC(OnExitPartHelperTwo)(URI_TYPE(ParserState) * state);

static void URI_FUNC(ResetParserState)(URI_TYPE(ParserState) * state);
static void URI_FUNC(ResetUri)(URI_TYPE(Uri) * uri);
static void URI_FUNC(PushPathSegment)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast);
static void URI_FUNC(Stop)(URI_TYPE(ParserState) * state, const URI_CHAR * errorPos);
/*
static void URI_FUNC(StopEx)(URI_TYPE(ParserState) * state, const URI_CHAR * errorPos, int errorCode);
*/
unsigned char URI_FUNC(HexdigToInt)(URI_CHAR hexdig);



static URI_INLINE void URI_FUNC(Stop)(URI_TYPE(ParserState) * state,
		const URI_CHAR * errorPos) {
	URI_FUNC(FreeUriMembers)(state->uri);
	state->errorPos = errorPos;
	state->errorCode = URI_ERROR_SYNTAX;
}


/*
static URI_INLINE void URI_FUNC(StopEx)(URI_TYPE(ParserState) * state,
		const URI_CHAR * errorPos, int errorCode) {
	URI_FUNC(FreeUriMembers)(state->uri);
	state->errorPos = errorPos;
	state->errorCode = errorCode;
}
*/


/*
 * [authority]-><[>[ipLit2][authorityTwo]
 * [authority]->[ownHostUserInfoNz]
 * [authority]-><NULL>
 */
static URI_INLINE const URI_CHAR * URI_FUNC(ParseAuthority)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast) {
	if (first >= afterLast) {
		return afterLast;
	}

	switch (*first) {
	case _UT('['):
		{
			const URI_CHAR * const afterIpLit2
					= URI_FUNC(ParseIpLit2)(state, first + 1, afterLast);
			if (afterIpLit2 == NULL) {
				return NULL;
			}
			state->uri->hostText.first = first + 1; /* HOST BEGIN */
			return URI_FUNC(ParseAuthorityTwo)(state, afterIpLit2, afterLast);
		}

	case _UT('!'):
	case _UT('$'):
	case _UT('%'):
	case _UT('&'):
	case _UT('('):
	case _UT(')'):
	case _UT('-'):
	case _UT('*'):
	case _UT(','):
	case _UT('.'):
	case _UT(':'):
	case _UT(';'):
	case _UT('@'):
	case _UT('\''):
	case _UT('_'):
	case _UT('~'):
	case _UT('+'):
	case _UT('='):
	case _UT('0'):
	case _UT('1'):
	case _UT('2'):
	case _UT('3'):
	case _UT('4'):
	case _UT('5'):
	case _UT('6'):
	case _UT('7'):
	case _UT('8'):
	case _UT('9'):
	case _UT('a'):
	case _UT('A'):
	case _UT('b'):
	case _UT('B'):
	case _UT('c'):
	case _UT('C'):
	case _UT('d'):
	case _UT('D'):
	case _UT('e'):
	case _UT('E'):
	case _UT('f'):
	case _UT('F'):
	case _UT('g'):
	case _UT('G'):
	case _UT('h'):
	case _UT('H'):
	case _UT('i'):
	case _UT('I'):
	case _UT('j'):
	case _UT('J'):
	case _UT('k'):
	case _UT('K'):
	case _UT('l'):
	case _UT('L'):
	case _UT('m'):
	case _UT('M'):
	case _UT('n'):
	case _UT('N'):
	case _UT('o'):
	case _UT('O'):
	case _UT('p'):
	case _UT('P'):
	case _UT('q'):
	case _UT('Q'):
	case _UT('r'):
	case _UT('R'):
	case _UT('s'):
	case _UT('S'):
	case _UT('t'):
	case _UT('T'):
	case _UT('u'):
	case _UT('U'):
	case _UT('v'):
	case _UT('V'):
	case _UT('w'):
	case _UT('W'):
	case _UT('x'):
	case _UT('X'):
	case _UT('y'):
	case _UT('Y'):
	case _UT('z'):
	case _UT('Z'):
		state->uri->userInfo.first = first; /* USERINFO BEGIN */
		return URI_FUNC(ParseOwnHostUserInfoNz)(state, first, afterLast);

	default:
		return first;
	}
}

/*
 * [authorityTwo]-><:>[port]
 * [authorityTwo]-><NULL>
 */
static URI_INLINE const URI_CHAR * URI_FUNC(ParseAuthorityTwo)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast) {
	if (first >= afterLast) {
		return afterLast;
	}

	switch (*first) {
	case _UT(':'):
		{
			const URI_CHAR * const afterPort = URI_FUNC(ParsePort)(state, first + 1, afterLast);
			if (afterPort == NULL) {
				return NULL;
			}
			state->uri->portText.first = first + 1; /* PORT BEGIN */
			state->uri->portText.afterLast = afterPort; /* PORT END */
			return afterPort;
		}

	default:
		return first;
	}
}



/*
 * [hexZero]->[HEXDIG][hexZero]
 * [hexZero]-><NULL>
 */
static const URI_CHAR * URI_FUNC(ParseHexZero)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast) {
	if (first >= afterLast) {
		return afterLast;
	}

	switch (*first) {
	case _UT('0'):
	case _UT('1'):
	case _UT('2'):
	case _UT('3'):
	case _UT('4'):
	case _UT('5'):
	case _UT('6'):
	case _UT('7'):
	case _UT('8'):
	case _UT('9'):
	case _UT('a'):
	case _UT('A'):
	case _UT('b'):
	case _UT('B'):
	case _UT('c'):
	case _UT('C'):
	case _UT('d'):
	case _UT('D'):
	case _UT('e'):
	case _UT('E'):
	case _UT('f'):
	case _UT('F'):
		return URI_FUNC(ParseHexZero)(state, first + 1, afterLast);

	default:
		return first;
	}
}



/*
 * [hierPart]->[pathRootless]
 * [hierPart]-></>[partHelperTwo]
 * [hierPart]-><NULL>
 */
static URI_INLINE const URI_CHAR * URI_FUNC(ParseHierPart)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast) {
	if (first >= afterLast) {
		return afterLast;
	}

	switch (*first) {
	case _UT('!'):
	case _UT('$'):
	case _UT('%'):
	case _UT('&'):
	case _UT('('):
	case _UT(')'):
	case _UT('-'):
	case _UT('*'):
	case _UT(','):
	case _UT('.'):
	case _UT(':'):
	case _UT(';'):
	case _UT('@'):
	case _UT('\''):
	case _UT('_'):
	case _UT('~'):
	case _UT('+'):
	case _UT('='):
	case _UT('0'):
	case _UT('1'):
	case _UT('2'):
	case _UT('3'):
	case _UT('4'):
	case _UT('5'):
	case _UT('6'):
	case _UT('7'):
	case _UT('8'):
	case _UT('9'):
	case _UT('a'):
	case _UT('A'):
	case _UT('b'):
	case _UT('B'):
	case _UT('c'):
	case _UT('C'):
	case _UT('d'):
	case _UT('D'):
	case _UT('e'):
	case _UT('E'):
	case _UT('f'):
	case _UT('F'):
	case _UT('g'):
	case _UT('G'):
	case _UT('h'):
	case _UT('H'):
	case _UT('i'):
	case _UT('I'):
	case _UT('j'):
	case _UT('J'):
	case _UT('k'):
	case _UT('K'):
	case _UT('l'):
	case _UT('L'):
	case _UT('m'):
	case _UT('M'):
	case _UT('n'):
	case _UT('N'):
	case _UT('o'):
	case _UT('O'):
	case _UT('p'):
	case _UT('P'):
	case _UT('q'):
	case _UT('Q'):
	case _UT('r'):
	case _UT('R'):
	case _UT('s'):
	case _UT('S'):
	case _UT('t'):
	case _UT('T'):
	case _UT('u'):
	case _UT('U'):
	case _UT('v'):
	case _UT('V'):
	case _UT('w'):
	case _UT('W'):
	case _UT('x'):
	case _UT('X'):
	case _UT('y'):
	case _UT('Y'):
	case _UT('z'):
	case _UT('Z'):
		return URI_FUNC(ParsePathRootless)(state, first, afterLast);

	case _UT('/'):
		return URI_FUNC(ParsePartHelperTwo)(state, first + 1, afterLast);

	default:
		return first;
	}
}



/*
 * [ipFutLoop]->[subDelims][ipFutStopGo]
 * [ipFutLoop]->[unreserved][ipFutStopGo]
 * [ipFutLoop]-><:>[ipFutStopGo]
 */
static const URI_CHAR * URI_FUNC(ParseIpFutLoop)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast) {
	if (first >= afterLast) {
		URI_FUNC(Stop)(state, first);
		return NULL;
	}

	switch (*first) {
	case _UT('!'):
	case _UT('$'):
	case _UT('&'):
	case _UT('('):
	case _UT(')'):
	case _UT('-'):
	case _UT('*'):
	case _UT(','):
	case _UT('.'):
	case _UT(':'):
	case _UT(';'):
	case _UT('\''):
	case _UT('_'):
	case _UT('~'):
	case _UT('+'):
	case _UT('='):
	case _UT('0'):
	case _UT('1'):
	case _UT('2'):
	case _UT('3'):
	case _UT('4'):
	case _UT('5'):
	case _UT('6'):
	case _UT('7'):
	case _UT('8'):
	case _UT('9'):
	case _UT('a'):
	case _UT('A'):
	case _UT('b'):
	case _UT('B'):
	case _UT('c'):
	case _UT('C'):
	case _UT('d'):
	case _UT('D'):
	case _UT('e'):
	case _UT('E'):
	case _UT('f'):
	case _UT('F'):
	case _UT('g'):
	case _UT('G'):
	case _UT('h'):
	case _UT('H'):
	case _UT('i'):
	case _UT('I'):
	case _UT('j'):
	case _UT('J'):
	case _UT('k'):
	case _UT('K'):
	case _UT('l'):
	case _UT('L'):
	case _UT('m'):
	case _UT('M'):
	case _UT('n'):
	case _UT('N'):
	case _UT('o'):
	case _UT('O'):
	case _UT('p'):
	case _UT('P'):
	case _UT('q'):
	case _UT('Q'):
	case _UT('r'):
	case _UT('R'):
	case _UT('s'):
	case _UT('S'):
	case _UT('t'):
	case _UT('T'):
	case _UT('u'):
	case _UT('U'):
	case _UT('v'):
	case _UT('V'):
	case _UT('w'):
	case _UT('W'):
	case _UT('x'):
	case _UT('X'):
	case _UT('y'):
	case _UT('Y'):
	case _UT('z'):
	case _UT('Z'):
		return URI_FUNC(ParseIpFutStopGo)(state, first + 1, afterLast);

	default:
		URI_FUNC(Stop)(state, first);
		return NULL;
	}
}



/*
 * [ipFutStopGo]->[ipFutLoop]
 * [ipFutStopGo]-><NULL>
 */
static const URI_CHAR * URI_FUNC(ParseIpFutStopGo)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast) {
	if (first >= afterLast) {
		return afterLast;
	}

	switch (*first) {
	case _UT('!'):
	case _UT('$'):
	case _UT('&'):
	case _UT('('):
	case _UT(')'):
	case _UT('-'):
	case _UT('*'):
	case _UT(','):
	case _UT('.'):
	case _UT(':'):
	case _UT(';'):
	case _UT('\''):
	case _UT('_'):
	case _UT('~'):
	case _UT('+'):
	case _UT('='):
	case _UT('0'):
	case _UT('1'):
	case _UT('2'):
	case _UT('3'):
	case _UT('4'):
	case _UT('5'):
	case _UT('6'):
	case _UT('7'):
	case _UT('8'):
	case _UT('9'):
	case _UT('a'):
	case _UT('A'):
	case _UT('b'):
	case _UT('B'):
	case _UT('c'):
	case _UT('C'):
	case _UT('d'):
	case _UT('D'):
	case _UT('e'):
	case _UT('E'):
	case _UT('f'):
	case _UT('F'):
	case _UT('g'):
	case _UT('G'):
	case _UT('h'):
	case _UT('H'):
	case _UT('i'):
	case _UT('I'):
	case _UT('j'):
	case _UT('J'):
	case _UT('k'):
	case _UT('K'):
	case _UT('l'):
	case _UT('L'):
	case _UT('m'):
	case _UT('M'):
	case _UT('n'):
	case _UT('N'):
	case _UT('o'):
	case _UT('O'):
	case _UT('p'):
	case _UT('P'):
	case _UT('q'):
	case _UT('Q'):
	case _UT('r'):
	case _UT('R'):
	case _UT('s'):
	case _UT('S'):
	case _UT('t'):
	case _UT('T'):
	case _UT('u'):
	case _UT('U'):
	case _UT('v'):
	case _UT('V'):
	case _UT('w'):
	case _UT('W'):
	case _UT('x'):
	case _UT('X'):
	case _UT('y'):
	case _UT('Y'):
	case _UT('z'):
	case _UT('Z'):
		return URI_FUNC(ParseIpFutLoop)(state, first, afterLast);

	default:
		return first;
	}
}



/*
 * [ipFuture]-><v>[HEXDIG][hexZero]<.>[ipFutLoop]
 */
static const URI_CHAR * URI_FUNC(ParseIpFuture)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast) {
	if (first >= afterLast) {
		URI_FUNC(Stop)(state, first);
		return NULL;
	}

	/*
	First character has already been
	checked before entering this rule.

	switch (*first) {
	case _UT('v'):
	*/
		if (first + 1 >= afterLast) {
			URI_FUNC(Stop)(state, first + 1);
			return NULL;
		}

		switch (first[1]) {
		case _UT('0'):
		case _UT('1'):
		case _UT('2'):
		case _UT('3'):
		case _UT('4'):
		case _UT('5'):
		case _UT('6'):
		case _UT('7'):
		case _UT('8'):
		case _UT('9'):
		case _UT('a'):
		case _UT('A'):
		case _UT('b'):
		case _UT('B'):
		case _UT('c'):
		case _UT('C'):
		case _UT('d'):
		case _UT('D'):
		case _UT('e'):
		case _UT('E'):
		case _UT('f'):
		case _UT('F'):
			{
				const URI_CHAR * afterIpFutLoop;
				const URI_CHAR * const afterHexZero
						= URI_FUNC(ParseHexZero)(state, first + 2, afterLast);
				if (afterHexZero == NULL) {
					return NULL;
				}
				if ((afterHexZero >= afterLast)
						|| (*afterHexZero != _UT('.'))) {
					URI_FUNC(Stop)(state, afterHexZero);
					return NULL;
				}
				state->uri->hostText.first = first; /* HOST BEGIN */
				state->uri->hostData.ipFuture.first = first; /* IPFUTURE BEGIN */
				afterIpFutLoop = URI_FUNC(ParseIpFutLoop)(state, afterHexZero + 1, afterLast);
				if (afterIpFutLoop == NULL) {
					return NULL;
				}
				state->uri->hostText.afterLast = afterIpFutLoop; /* HOST END */
				state->uri->hostData.ipFuture.afterLast = afterIpFutLoop; /* IPFUTURE END */
				return afterIpFutLoop;
			}

		default:
			URI_FUNC(Stop)(state, first + 1);
			return NULL;
		}

	/*
	default:
		URI_FUNC(Stop)(state, first);
		return NULL;
	}
	*/
}



/*
 * [ipLit2]->[ipFuture]<]>
 * [ipLit2]->[IPv6address2]
 */
static URI_INLINE const URI_CHAR * URI_FUNC(ParseIpLit2)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast) {
	if (first >= afterLast) {
		URI_FUNC(Stop)(state, first);
		return NULL;
	}

	switch (*first) {
	case _UT('v'):
		{
			const URI_CHAR * const afterIpFuture
					= URI_FUNC(ParseIpFuture)(state, first, afterLast);
			if (afterIpFuture == NULL) {
				return NULL;
			}
			if ((afterIpFuture >= afterLast)
					|| (*afterIpFuture != _UT(']'))) {
				URI_FUNC(Stop)(state, first);
				return NULL;
			}
			return afterIpFuture + 1;
		}

	case _UT(':'):
	case _UT(']'):
	case _UT('0'):
	case _UT('1'):
	case _UT('2'):
	case _UT('3'):
	case _UT('4'):
	case _UT('5'):
	case _UT('6'):
	case _UT('7'):
	case _UT('8'):
	case _UT('9'):
	case _UT('a'):
	case _UT('A'):
	case _UT('b'):
	case _UT('B'):
	case _UT('c'):
	case _UT('C'):
	case _UT('d'):
	case _UT('D'):
	case _UT('e'):
	case _UT('E'):
	case _UT('f'):
	case _UT('F'):
		state->uri->hostData.ip6 = malloc(1 * sizeof(UriIp6)); /* Freed when stopping on parse error */
		return URI_FUNC(ParseIPv6address2)(state, first, afterLast);

	default:
		URI_FUNC(Stop)(state, first);
		return NULL;
	}
}



/*
 * [IPv6address2]->..<]>
 */
static const URI_CHAR * URI_FUNC(ParseIPv6address2)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast) {
	int zipperEver = 0;
	int quadsDone = 0;
	int digitCount = 0;
	unsigned char digitHistory[4];
	int ip4OctetsDone = 0;

	unsigned char quadsAfterZipper[14];
	int quadsAfterZipperCount = 0;


	for (;;) {
		if (first >= afterLast) {
			URI_FUNC(Stop)(state, first);
			return NULL;
		}

		/* Inside IPv4 part? */
		if (ip4OctetsDone > 0) {
			/* Eat rest of IPv4 address */
			for (;;) {
				switch (*first) {
				case _UT('0'):
				case _UT('1'):
				case _UT('2'):
				case _UT('3'):
				case _UT('4'):
				case _UT('5'):
				case _UT('6'):
				case _UT('7'):
				case _UT('8'):
				case _UT('9'):
					if (digitCount == 4) {
						URI_FUNC(Stop)(state, first);
						return NULL;
					}
					digitHistory[digitCount++] = (unsigned char)(9 + *first - _UT('9'));
					break;

				case _UT('.'):
					if ((ip4OctetsDone == 4) /* NOTE! */
							|| (digitCount == 0)
							|| (digitCount == 4)) {
						/* Invalid digit or octet count */
						URI_FUNC(Stop)(state, first);
						return NULL;
					} else if ((digitCount > 1)
							&& (digitHistory[0] == 0)) {
						/* Leading zero */
						URI_FUNC(Stop)(state, first - digitCount);
						return NULL;
					} else if ((digitCount > 2)
							&& (digitHistory[1] == 0)) {
						/* Leading zero */
						URI_FUNC(Stop)(state, first - digitCount + 1);
						return NULL;
					} else if ((digitCount == 3)
							&& (100 * digitHistory[0]
								+ 10 * digitHistory[1]
								+ digitHistory[2] > 255)) {
						/* Octet value too large */
						if (digitHistory[0] > 2) {
							URI_FUNC(Stop)(state, first - 3);
						} else if (digitHistory[1] > 5) {
							URI_FUNC(Stop)(state, first - 2);
						} else {
							URI_FUNC(Stop)(state, first - 1);
						}
						return NULL;
					}

					/* Copy IPv4 octet */
					state->uri->hostData.ip6->data[16 - 4 + ip4OctetsDone] = uriGetOctetValue(digitHistory, digitCount);
					digitCount = 0;
					ip4OctetsDone++;
					break;

				case _UT(']'):
					if ((ip4OctetsDone != 3) /* NOTE! */
							|| (digitCount == 0)
							|| (digitCount == 4)) {
						/* Invalid digit or octet count */
						URI_FUNC(Stop)(state, first);
						return NULL;
					} else if ((digitCount > 1)
							&& (digitHistory[0] == 0)) {
						/* Leading zero */
						URI_FUNC(Stop)(state, first - digitCount);
						return NULL;
					} else if ((digitCount > 2)
							&& (digitHistory[1] == 0)) {
						/* Leading zero */
						URI_FUNC(Stop)(state, first - digitCount + 1);
						return NULL;
					} else if ((digitCount == 3)
							&& (100 * digitHistory[0]
								+ 10 * digitHistory[1]
								+ digitHistory[2] > 255)) {
						/* Octet value too large */
						if (digitHistory[0] > 2) {
							URI_FUNC(Stop)(state, first - 3);
						} else if (digitHistory[1] > 5) {
							URI_FUNC(Stop)(state, first - 2);
						} else {
							URI_FUNC(Stop)(state, first - 1);
						}
						return NULL;
					}

					state->uri->hostText.afterLast = first; /* HOST END */

					/* Copy missing quads right before IPv4 */
					memcpy(state->uri->hostData.ip6->data + 16 - 4 - 2 * quadsAfterZipperCount,
								quadsAfterZipper, 2 * quadsAfterZipperCount);

					/* Copy last IPv4 octet */
					state->uri->hostData.ip6->data[16 - 4 + 3] = uriGetOctetValue(digitHistory, digitCount);

					return first + 1;

				default:
					URI_FUNC(Stop)(state, first);
					return NULL;
				}
				first++;
			}
		} else {
			/* Eat while no dot in sight */
			int letterAmong = 0;
			int walking = 1;
			do {
				switch (*first) {
				case _UT('a'):
				case _UT('b'):
				case _UT('c'):
				case _UT('d'):
				case _UT('e'):
				case _UT('f'):
					letterAmong = 1;
					if (digitCount == 4) {
						URI_FUNC(Stop)(state, first);
						return NULL;
					}
					digitHistory[digitCount] = (unsigned char)(15 + *first - _UT('f'));
					digitCount++;
					break;

				case _UT('A'):
				case _UT('B'):
				case _UT('C'):
				case _UT('D'):
				case _UT('E'):
				case _UT('F'):
					letterAmong = 1;
					if (digitCount == 4) {
						URI_FUNC(Stop)(state, first);
						return NULL;
					}
					digitHistory[digitCount] = (unsigned char)(15 + *first - _UT('F'));
					digitCount++;
					break;

				case _UT('0'):
				case _UT('1'):
				case _UT('2'):
				case _UT('3'):
				case _UT('4'):
				case _UT('5'):
				case _UT('6'):
				case _UT('7'):
				case _UT('8'):
				case _UT('9'):
					if (digitCount == 4) {
						URI_FUNC(Stop)(state, first);
						return NULL;
					}
					digitHistory[digitCount] = (unsigned char)(9 + *first - _UT('9'));
					digitCount++;
					break;

				case _UT(':'):
					{
						int setZipper = 0;

						/* Too many quads? */
						if (quadsDone > 8 - zipperEver) {
							URI_FUNC(Stop)(state, first);
							return NULL;
						}

						/* "::"? */
						if (first + 1 >= afterLast) {
							URI_FUNC(Stop)(state, first + 1);
							return NULL;
						}
						if (first[1] == _UT(':')) {
							const int resetOffset = 2 * (quadsDone + (digitCount > 0));

							first++;
							if (zipperEver) {
								URI_FUNC(Stop)(state, first);
								return NULL; /* "::.+::" */
							}

							/* Zero everything after zipper */
							memset(state->uri->hostData.ip6->data + resetOffset, 0, 16 - resetOffset);
							setZipper = 1;

							/* ":::+"? */
							if (first + 1 >= afterLast) {
								URI_FUNC(Stop)(state, first + 1);
								return NULL; /* No ']' yet */
							}
							if (first[1] == _UT(':')) {
								URI_FUNC(Stop)(state, first + 1);
								return NULL; /* ":::+ "*/
							}
						}
						if (digitCount > 0) {
							if (zipperEver) {
								uriWriteQuadToDoubleByte(digitHistory, digitCount, quadsAfterZipper + 2 * quadsAfterZipperCount);
								quadsAfterZipperCount++;
							} else {
								uriWriteQuadToDoubleByte(digitHistory, digitCount, state->uri->hostData.ip6->data + 2 * quadsDone);
							}
							quadsDone++;
							digitCount = 0;
						}
						letterAmong = 0;

						if (setZipper) {
							zipperEver = 1;
						}
					}
					break;

				case _UT('.'):
					if ((quadsDone > 6) /* NOTE */
							|| (!zipperEver && (quadsDone < 6))
							|| letterAmong
							|| (digitCount == 0)
							|| (digitCount == 4)) {
						/* Invalid octet before */
						URI_FUNC(Stop)(state, first);
						return NULL;
					} else if ((digitCount > 1)
							&& (digitHistory[0] == 0)) {
						/* Leading zero */
						URI_FUNC(Stop)(state, first - digitCount);
						return NULL;
					} else if ((digitCount > 2)
							&& (digitHistory[1] == 0)) {
						/* Leading zero */
						URI_FUNC(Stop)(state, first - digitCount + 1);
						return NULL;
					} else if ((digitCount == 3)
							&& (100 * digitHistory[0]
								+ 10 * digitHistory[1]
								+ digitHistory[2] > 255)) {
						/* Octet value too large */
						if (digitHistory[0] > 2) {
							URI_FUNC(Stop)(state, first - 3);
						} else if (digitHistory[1] > 5) {
							URI_FUNC(Stop)(state, first - 2);
						} else {
							URI_FUNC(Stop)(state, first - 1);
						}
						return NULL;
					}

					/* Copy first IPv4 octet */
					state->uri->hostData.ip6->data[16 - 4] = uriGetOctetValue(digitHistory, digitCount);
					digitCount = 0;

					/* Switch over to IPv4 loop */
					ip4OctetsDone = 1;
					walking = 0;
					break;

				case _UT(']'):
					/* Too little quads? */
					if (!zipperEver && !((quadsDone == 7) && (digitCount > 0))) {
						URI_FUNC(Stop)(state, first);
						return NULL;
					}

					if (digitCount > 0) {
						if (zipperEver) {
							uriWriteQuadToDoubleByte(digitHistory, digitCount, quadsAfterZipper + 2 * quadsAfterZipperCount);
							quadsAfterZipperCount++;
						} else {
							uriWriteQuadToDoubleByte(digitHistory, digitCount, state->uri->hostData.ip6->data + 2 * quadsDone);
						}
						/*
						quadsDone++;
						digitCount = 0;
						*/
					}

					/* Copy missing quads to the end */
					memcpy(state->uri->hostData.ip6->data + 16 - 2 * quadsAfterZipperCount,
								quadsAfterZipper, 2 * quadsAfterZipperCount);

					state->uri->hostText.afterLast = first; /* HOST END */
					return first + 1; /* Fine */

				default:
					URI_FUNC(Stop)(state, first);
					return NULL;
				}
				first++;

				if (first >= afterLast) {
					URI_FUNC(Stop)(state, first);
					return NULL; /* No ']' yet */
				}
			} while (walking);
		}
	}

	/* We should never get here */
	URI_FUNC(Stop)(state, first);
	return NULL;
}



/*
 * [mustBeSegmentNzNc]->[pctEncoded][mustBeSegmentNzNc]
 * [mustBeSegmentNzNc]->[subDelims][mustBeSegmentNzNc]
 * [mustBeSegmentNzNc]->[unreserved][mustBeSegmentNzNc]
 * [mustBeSegmentNzNc]->[uriTail] // can take <NULL>
 * [mustBeSegmentNzNc]-></>[segment][zeroMoreSlashSegs][uriTail]
 * [mustBeSegmentNzNc]-><@>[mustBeSegmentNzNc]
 */
static const URI_CHAR * URI_FUNC(ParseMustBeSegmentNzNc)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast) {
	if (first >= afterLast) {
		return afterLast;
	}

	switch (*first) {
	case _UT('%'):
		{
			const URI_CHAR * const afterPctEncoded
					= URI_FUNC(ParsePctEncoded)(state, first, afterLast);
			if (afterPctEncoded == NULL) {
				return NULL;
			}
			return URI_FUNC(ParseMustBeSegmentNzNc)(state, afterPctEncoded, afterLast);
		}

	case _UT('@'):
	case _UT('!'):
	case _UT('$'):
	case _UT('&'):
	case _UT('('):
	case _UT(')'):
	case _UT('*'):
	case _UT(','):
	case _UT(';'):
	case _UT('\''):
	case _UT('+'):
	case _UT('='):
	case _UT('-'):
	case _UT('.'):
	case _UT('_'):
	case _UT('~'):
	case _UT('0'):
	case _UT('1'):
	case _UT('2'):
	case _UT('3'):
	case _UT('4'):
	case _UT('5'):
	case _UT('6'):
	case _UT('7'):
	case _UT('8'):
	case _UT('9'):
	case _UT('a'):
	case _UT('A'):
	case _UT('b'):
	case _UT('B'):
	case _UT('c'):
	case _UT('C'):
	case _UT('d'):
	case _UT('D'):
	case _UT('e'):
	case _UT('E'):
	case _UT('f'):
	case _UT('F'):
	case _UT('g'):
	case _UT('G'):
	case _UT('h'):
	case _UT('H'):
	case _UT('i'):
	case _UT('I'):
	case _UT('j'):
	case _UT('J'):
	case _UT('k'):
	case _UT('K'):
	case _UT('l'):
	case _UT('L'):
	case _UT('m'):
	case _UT('M'):
	case _UT('n'):
	case _UT('N'):
	case _UT('o'):
	case _UT('O'):
	case _UT('p'):
	case _UT('P'):
	case _UT('q'):
	case _UT('Q'):
	case _UT('r'):
	case _UT('R'):
	case _UT('s'):
	case _UT('S'):
	case _UT('t'):
	case _UT('T'):
	case _UT('u'):
	case _UT('U'):
	case _UT('v'):
	case _UT('V'):
	case _UT('w'):
	case _UT('W'):
	case _UT('x'):
	case _UT('X'):
	case _UT('y'):
	case _UT('Y'):
	case _UT('z'):
	case _UT('Z'):
		return URI_FUNC(ParseMustBeSegmentNzNc)(state, first + 1, afterLast);

	case _UT('/'):
		{
			const URI_CHAR * afterZeroMoreSlashSegs;
			const URI_CHAR * const afterSegment
					= URI_FUNC(ParseSegment)(state, first + 1, afterLast);
			if (afterSegment == NULL) {
				return NULL;
			}
			URI_FUNC(PushPathSegment)(state, first + 1, afterSegment); /* SEGMENT BOTH */
			afterZeroMoreSlashSegs
					= URI_FUNC(ParseZeroMoreSlashSegs)(state, afterSegment, afterLast);
			if (afterZeroMoreSlashSegs == NULL) {
				return NULL;
			}
			return URI_FUNC(ParseUriTail)(state, afterZeroMoreSlashSegs, afterLast);
		}

	default:
		return URI_FUNC(ParseUriTail)(state, first, afterLast);
	}
}



/*
 * [ownHost]-><[>[ipLit2][authorityTwo]
 * [ownHost]->[ownHost2] // can take <NULL>
 */
static URI_INLINE const URI_CHAR * URI_FUNC(ParseOwnHost)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast) {
	if (first >= afterLast) {
		return afterLast;
	}

	switch (*first) {
	case _UT('['):
		{
			const URI_CHAR * const afterIpLit2
					= URI_FUNC(ParseIpLit2)(state, first + 1, afterLast);
			if (afterIpLit2 == NULL) {
				return NULL;
			}
			state->uri->hostText.first = first + 1; /* HOST BEGIN */
			return URI_FUNC(ParseAuthorityTwo)(state, afterIpLit2, afterLast);
		}

	default:
		return URI_FUNC(ParseOwnHost2)(state, first, afterLast);
	}
}



static URI_INLINE void URI_FUNC(OnExitOwnHost2)(URI_TYPE(ParserState) * state, const URI_CHAR * first) {
	state->uri->hostText.afterLast = first; /* HOST END */

	/* Valid IPv4 or just a regname? */
	state->uri->hostData.ip4 = malloc(1 * sizeof(UriIp4)); /* Freed when stopping on parse error */
	if (URI_FUNC(ParseIpFourAddress)(state->uri->hostData.ip4->data,
			state->uri->hostText.first, state->uri->hostText.afterLast)) {
		/* Not IPv4 */
		free(state->uri->hostData.ip4);
		state->uri->hostData.ip4 = NULL;
	}
}



/*
 * [ownHost2]->[authorityTwo] // can take <NULL>
 * [ownHost2]->[pctSubUnres][ownHost2]
 */
static const URI_CHAR * URI_FUNC(ParseOwnHost2)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast) {
	if (first >= afterLast) {
		URI_FUNC(OnExitOwnHost2)(state, first);
		return afterLast;
	}

	switch (*first) {
	case _UT('!'):
	case _UT('$'):
	case _UT('%'):
	case _UT('&'):
	case _UT('('):
	case _UT(')'):
	case _UT('-'):
	case _UT('*'):
	case _UT(','):
	case _UT('.'):
	case _UT(';'):
	case _UT('\''):
	case _UT('_'):
	case _UT('~'):
	case _UT('+'):
	case _UT('='):
	case _UT('0'):
	case _UT('1'):
	case _UT('2'):
	case _UT('3'):
	case _UT('4'):
	case _UT('5'):
	case _UT('6'):
	case _UT('7'):
	case _UT('8'):
	case _UT('9'):
	case _UT('a'):
	case _UT('A'):
	case _UT('b'):
	case _UT('B'):
	case _UT('c'):
	case _UT('C'):
	case _UT('d'):
	case _UT('D'):
	case _UT('e'):
	case _UT('E'):
	case _UT('f'):
	case _UT('F'):
	case _UT('g'):
	case _UT('G'):
	case _UT('h'):
	case _UT('H'):
	case _UT('i'):
	case _UT('I'):
	case _UT('j'):
	case _UT('J'):
	case _UT('k'):
	case _UT('K'):
	case _UT('l'):
	case _UT('L'):
	case _UT('m'):
	case _UT('M'):
	case _UT('n'):
	case _UT('N'):
	case _UT('o'):
	case _UT('O'):
	case _UT('p'):
	case _UT('P'):
	case _UT('q'):
	case _UT('Q'):
	case _UT('r'):
	case _UT('R'):
	case _UT('s'):
	case _UT('S'):
	case _UT('t'):
	case _UT('T'):
	case _UT('u'):
	case _UT('U'):
	case _UT('v'):
	case _UT('V'):
	case _UT('w'):
	case _UT('W'):
	case _UT('x'):
	case _UT('X'):
	case _UT('y'):
	case _UT('Y'):
	case _UT('z'):
	case _UT('Z'):
		{
			const URI_CHAR * const afterPctSubUnres
					= URI_FUNC(ParsePctSubUnres)(state, first, afterLast);
			if (afterPctSubUnres == NULL) {
				return NULL;
			}
			return URI_FUNC(ParseOwnHost2)(state, afterPctSubUnres, afterLast);
		}

	default:
		URI_FUNC(OnExitOwnHost2)(state, first);
		return URI_FUNC(ParseAuthorityTwo)(state, first, afterLast);
	}
}



static URI_INLINE void URI_FUNC(OnExitOwnHostUserInfo)(URI_TYPE(ParserState) * state, const URI_CHAR * first) {
	state->uri->hostText.first = state->uri->userInfo.first; /* Host instead of userInfo, update */
	state->uri->userInfo.first = NULL; /* Not a userInfo, reset */
	state->uri->hostText.afterLast = first; /* HOST END */

	/* Valid IPv4 or just a regname? */
	state->uri->hostData.ip4 = malloc(1 * sizeof(UriIp4)); /* Freed when stopping on parse error */
	if (URI_FUNC(ParseIpFourAddress)(state->uri->hostData.ip4->data,
			state->uri->hostText.first, state->uri->hostText.afterLast)) {
		/* Not IPv4 */
		free(state->uri->hostData.ip4);
		state->uri->hostData.ip4 = NULL;
	}
}



/*
 * [ownHostUserInfo]->[ownHostUserInfoNz]
 * [ownHostUserInfo]-><NULL>
 */
static URI_INLINE const URI_CHAR * URI_FUNC(ParseOwnHostUserInfo)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast) {
	if (first >= afterLast) {
		URI_FUNC(OnExitOwnHostUserInfo)(state, first);
		return afterLast;
	}

	switch (*first) {
	case _UT('!'):
	case _UT('$'):
	case _UT('%'):
	case _UT('&'):
	case _UT('('):
	case _UT(')'):
	case _UT('-'):
	case _UT('*'):
	case _UT(','):
	case _UT('.'):
	case _UT(':'):
	case _UT(';'):
	case _UT('@'):
	case _UT('\''):
	case _UT('_'):
	case _UT('~'):
	case _UT('+'):
	case _UT('='):
	case _UT('0'):
	case _UT('1'):
	case _UT('2'):
	case _UT('3'):
	case _UT('4'):
	case _UT('5'):
	case _UT('6'):
	case _UT('7'):
	case _UT('8'):
	case _UT('9'):
	case _UT('a'):
	case _UT('A'):
	case _UT('b'):
	case _UT('B'):
	case _UT('c'):
	case _UT('C'):
	case _UT('d'):
	case _UT('D'):
	case _UT('e'):
	case _UT('E'):
	case _UT('f'):
	case _UT('F'):
	case _UT('g'):
	case _UT('G'):
	case _UT('h'):
	case _UT('H'):
	case _UT('i'):
	case _UT('I'):
	case _UT('j'):
	case _UT('J'):
	case _UT('k'):
	case _UT('K'):
	case _UT('l'):
	case _UT('L'):
	case _UT('m'):
	case _UT('M'):
	case _UT('n'):
	case _UT('N'):
	case _UT('o'):
	case _UT('O'):
	case _UT('p'):
	case _UT('P'):
	case _UT('q'):
	case _UT('Q'):
	case _UT('r'):
	case _UT('R'):
	case _UT('s'):
	case _UT('S'):
	case _UT('t'):
	case _UT('T'):
	case _UT('u'):
	case _UT('U'):
	case _UT('v'):
	case _UT('V'):
	case _UT('w'):
	case _UT('W'):
	case _UT('x'):
	case _UT('X'):
	case _UT('y'):
	case _UT('Y'):
	case _UT('z'):
	case _UT('Z'):
		return URI_FUNC(ParseOwnHostUserInfoNz)(state, first, afterLast);

	default:
		URI_FUNC(OnExitOwnHostUserInfo)(state, first);
		return first;
	}
}



/*
 * [ownHostUserInfoNz]->[pctSubUnres][ownHostUserInfo]
 * [ownHostUserInfoNz]-><:>[ownPortUserInfo]
 * [ownHostUserInfoNz]-><@>[ownHost]
 */
static const URI_CHAR * URI_FUNC(ParseOwnHostUserInfoNz)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast) {
	if (first >= afterLast) {
		URI_FUNC(Stop)(state, first);
		return NULL;
	}

	switch (*first) {
	case _UT('!'):
	case _UT('$'):
	case _UT('%'):
	case _UT('&'):
	case _UT('('):
	case _UT(')'):
	case _UT('-'):
	case _UT('*'):
	case _UT(','):
	case _UT('.'):
	case _UT(';'):
	case _UT('\''):
	case _UT('_'):
	case _UT('~'):
	case _UT('+'):
	case _UT('='):
	case _UT('0'):
	case _UT('1'):
	case _UT('2'):
	case _UT('3'):
	case _UT('4'):
	case _UT('5'):
	case _UT('6'):
	case _UT('7'):
	case _UT('8'):
	case _UT('9'):
	case _UT('a'):
	case _UT('A'):
	case _UT('b'):
	case _UT('B'):
	case _UT('c'):
	case _UT('C'):
	case _UT('d'):
	case _UT('D'):
	case _UT('e'):
	case _UT('E'):
	case _UT('f'):
	case _UT('F'):
	case _UT('g'):
	case _UT('G'):
	case _UT('h'):
	case _UT('H'):
	case _UT('i'):
	case _UT('I'):
	case _UT('j'):
	case _UT('J'):
	case _UT('k'):
	case _UT('K'):
	case _UT('l'):
	case _UT('L'):
	case _UT('m'):
	case _UT('M'):
	case _UT('n'):
	case _UT('N'):
	case _UT('o'):
	case _UT('O'):
	case _UT('p'):
	case _UT('P'):
	case _UT('q'):
	case _UT('Q'):
	case _UT('r'):
	case _UT('R'):
	case _UT('s'):
	case _UT('S'):
	case _UT('t'):
	case _UT('T'):
	case _UT('u'):
	case _UT('U'):
	case _UT('v'):
	case _UT('V'):
	case _UT('w'):
	case _UT('W'):
	case _UT('x'):
	case _UT('X'):
	case _UT('y'):
	case _UT('Y'):
	case _UT('z'):
	case _UT('Z'):
		{
			const URI_CHAR * const afterPctSubUnres
					= URI_FUNC(ParsePctSubUnres)(state, first, afterLast);
			if (afterPctSubUnres == NULL) {
				return NULL;
			}
			return URI_FUNC(ParseOwnHostUserInfo)(state, afterPctSubUnres, afterLast);
		}

	case _UT(':'):
		state->uri->hostText.afterLast = first; /* HOST END */
		state->uri->portText.first = first + 1; /* PORT BEGIN */
		return URI_FUNC(ParseOwnPortUserInfo)(state, first + 1, afterLast);

	case _UT('@'):
		state->uri->userInfo.afterLast = first; /* USERINFO END */
		state->uri->hostText.first = first + 1; /* HOST BEGIN */
		return URI_FUNC(ParseOwnHost)(state, first + 1, afterLast);

	default:
		URI_FUNC(Stop)(state, first);
		return NULL;
	}
}



static URI_INLINE void URI_FUNC(OnExitOwnPortUserInfo)(URI_TYPE(ParserState) * state, const URI_CHAR * first) {
	state->uri->hostText.first = state->uri->userInfo.first; /* Host instead of userInfo, update */
	state->uri->userInfo.first = NULL; /* Not a userInfo, reset */
	state->uri->portText.afterLast = first; /* PORT END */

	/* Valid IPv4 or just a regname? */
	state->uri->hostData.ip4 = malloc(1 * sizeof(UriIp4)); /* Freed when stopping on parse error */
	if (URI_FUNC(ParseIpFourAddress)(state->uri->hostData.ip4->data,
			state->uri->hostText.first, state->uri->hostText.afterLast)) {
		/* Not IPv4 */
		free(state->uri->hostData.ip4);
		state->uri->hostData.ip4 = NULL;
	}
}



/*
 * [ownPortUserInfo]->[ALPHA][ownUserInfo]
 * [ownPortUserInfo]->[DIGIT][ownPortUserInfo]
 * [ownPortUserInfo]-><.>[ownUserInfo]
 * [ownPortUserInfo]-><_>[ownUserInfo]
 * [ownPortUserInfo]-><~>[ownUserInfo]
 * [ownPortUserInfo]-><->[ownUserInfo]
 * [ownPortUserInfo]-><NULL>
 */
static const URI_CHAR * URI_FUNC(ParseOwnPortUserInfo)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast) {
	if (first >= afterLast) {
		URI_FUNC(OnExitOwnPortUserInfo)(state, first);
		return afterLast;
	}

	switch (*first) {
	case _UT('.'):
	case _UT('_'):
	case _UT('~'):
	case _UT('-'):
	case _UT('a'):
	case _UT('A'):
	case _UT('b'):
	case _UT('B'):
	case _UT('c'):
	case _UT('C'):
	case _UT('d'):
	case _UT('D'):
	case _UT('e'):
	case _UT('E'):
	case _UT('f'):
	case _UT('F'):
	case _UT('g'):
	case _UT('G'):
	case _UT('h'):
	case _UT('H'):
	case _UT('i'):
	case _UT('I'):
	case _UT('j'):
	case _UT('J'):
	case _UT('k'):
	case _UT('K'):
	case _UT('l'):
	case _UT('L'):
	case _UT('m'):
	case _UT('M'):
	case _UT('n'):
	case _UT('N'):
	case _UT('o'):
	case _UT('O'):
	case _UT('p'):
	case _UT('P'):
	case _UT('q'):
	case _UT('Q'):
	case _UT('r'):
	case _UT('R'):
	case _UT('s'):
	case _UT('S'):
	case _UT('t'):
	case _UT('T'):
	case _UT('u'):
	case _UT('U'):
	case _UT('v'):
	case _UT('V'):
	case _UT('w'):
	case _UT('W'):
	case _UT('x'):
	case _UT('X'):
	case _UT('y'):
	case _UT('Y'):
	case _UT('z'):
	case _UT('Z'):
		state->uri->hostText.afterLast = NULL; /* Not a host, reset */
		state->uri->portText.first = NULL; /* Not a port, reset */
		return URI_FUNC(ParseOwnUserInfo)(state, first + 1, afterLast);

	case _UT('0'):
	case _UT('1'):
	case _UT('2'):
	case _UT('3'):
	case _UT('4'):
	case _UT('5'):
	case _UT('6'):
	case _UT('7'):
	case _UT('8'):
	case _UT('9'):
		return URI_FUNC(ParseOwnPortUserInfo)(state, first + 1, afterLast);

	default:
		URI_FUNC(OnExitOwnPortUserInfo)(state, first);
		return first;
	}
}



/*
 * [ownUserInfo]->[pctSubUnres][ownUserInfo]
 * [ownUserInfo]-><:>[ownUserInfo]
 * [ownUserInfo]-><@>[ownHost]
 */
static const URI_CHAR * URI_FUNC(ParseOwnUserInfo)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast) {
	if (first >= afterLast) {
		URI_FUNC(Stop)(state, first);
		return NULL;
	}

	switch (*first) {
	case _UT('!'):
	case _UT('$'):
	case _UT('%'):
	case _UT('&'):
	case _UT('('):
	case _UT(')'):
	case _UT('-'):
	case _UT('*'):
	case _UT(','):
	case _UT('.'):
	case _UT(';'):
	case _UT('\''):
	case _UT('_'):
	case _UT('~'):
	case _UT('+'):
	case _UT('='):
	case _UT('0'):
	case _UT('1'):
	case _UT('2'):
	case _UT('3'):
	case _UT('4'):
	case _UT('5'):
	case _UT('6'):
	case _UT('7'):
	case _UT('8'):
	case _UT('9'):
	case _UT('a'):
	case _UT('A'):
	case _UT('b'):
	case _UT('B'):
	case _UT('c'):
	case _UT('C'):
	case _UT('d'):
	case _UT('D'):
	case _UT('e'):
	case _UT('E'):
	case _UT('f'):
	case _UT('F'):
	case _UT('g'):
	case _UT('G'):
	case _UT('h'):
	case _UT('H'):
	case _UT('i'):
	case _UT('I'):
	case _UT('j'):
	case _UT('J'):
	case _UT('k'):
	case _UT('K'):
	case _UT('l'):
	case _UT('L'):
	case _UT('m'):
	case _UT('M'):
	case _UT('n'):
	case _UT('N'):
	case _UT('o'):
	case _UT('O'):
	case _UT('p'):
	case _UT('P'):
	case _UT('q'):
	case _UT('Q'):
	case _UT('r'):
	case _UT('R'):
	case _UT('s'):
	case _UT('S'):
	case _UT('t'):
	case _UT('T'):
	case _UT('u'):
	case _UT('U'):
	case _UT('v'):
	case _UT('V'):
	case _UT('w'):
	case _UT('W'):
	case _UT('x'):
	case _UT('X'):
	case _UT('y'):
	case _UT('Y'):
	case _UT('z'):
	case _UT('Z'):
		{
			const URI_CHAR * const afterPctSubUnres
					= URI_FUNC(ParsePctSubUnres)(state, first, afterLast);
			if (afterPctSubUnres == NULL) {
				return NULL;
			}
			return URI_FUNC(ParseOwnUserInfo)(state, afterPctSubUnres, afterLast);
		}

	case _UT(':'):
		return URI_FUNC(ParseOwnUserInfo)(state, first + 1, afterLast);

	case _UT('@'):
		/* SURE */
		state->uri->userInfo.afterLast = first; /* USERINFO END */
		state->uri->hostText.first = first + 1; /* HOST BEGIN */
		return URI_FUNC(ParseOwnHost)(state, first + 1, afterLast);

	default:
		URI_FUNC(Stop)(state, first);
		return NULL;
	}
}



static URI_INLINE void URI_FUNC(OnExitPartHelperTwo)(URI_TYPE(ParserState) * state) {
	state->uri->absolutePath = URI_TRUE;
}



/*
 * [partHelperTwo]->[pathAbsNoLeadSlash] // can take <NULL>
 * [partHelperTwo]-></>[authority][pathAbsEmpty]
 */
static URI_INLINE const URI_CHAR * URI_FUNC(ParsePartHelperTwo)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast) {
	if (first >= afterLast) {
		URI_FUNC(OnExitPartHelperTwo)(state);
		return afterLast;
	}

	switch (*first) {
	case _UT('/'):
		{
			const URI_CHAR * const afterAuthority
					= URI_FUNC(ParseAuthority)(state, first + 1, afterLast);
			if (afterAuthority == NULL) {
				return NULL;
			}
			return URI_FUNC(ParsePathAbsEmpty)(state, afterAuthority, afterLast);
		}

	default:
		URI_FUNC(OnExitPartHelperTwo)(state);
		return URI_FUNC(ParsePathAbsNoLeadSlash)(state, first, afterLast);
	}
}



/*
 * [pathAbsEmpty]-></>[segment][pathAbsEmpty]
 * [pathAbsEmpty]-><NULL>
 */
static const URI_CHAR * URI_FUNC(ParsePathAbsEmpty)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast) {
	if (first >= afterLast) {
		return afterLast;
	}

	switch (*first) {
	case _UT('/'):
		{
			const URI_CHAR * const afterSegment
					= URI_FUNC(ParseSegment)(state, first + 1, afterLast);
			if (afterSegment == NULL) {
				return NULL;
			}
			URI_FUNC(PushPathSegment)(state, first + 1, afterSegment); /* SEGMENT BOTH */
			return URI_FUNC(ParsePathAbsEmpty)(state, afterSegment, afterLast);
		}


	default:
		return first;
	}
}



/*
 * [pathAbsNoLeadSlash]->[segmentNz][zeroMoreSlashSegs]
 * [pathAbsNoLeadSlash]-><NULL>
 */
static URI_INLINE const URI_CHAR * URI_FUNC(ParsePathAbsNoLeadSlash)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast) {
	if (first >= afterLast) {
		return afterLast;
	}

	switch (*first) {
	case _UT('!'):
	case _UT('$'):
	case _UT('%'):
	case _UT('&'):
	case _UT('('):
	case _UT(')'):
	case _UT('-'):
	case _UT('*'):
	case _UT(','):
	case _UT('.'):
	case _UT(':'):
	case _UT(';'):
	case _UT('@'):
	case _UT('\''):
	case _UT('_'):
	case _UT('~'):
	case _UT('+'):
	case _UT('='):
	case _UT('0'):
	case _UT('1'):
	case _UT('2'):
	case _UT('3'):
	case _UT('4'):
	case _UT('5'):
	case _UT('6'):
	case _UT('7'):
	case _UT('8'):
	case _UT('9'):
	case _UT('a'):
	case _UT('A'):
	case _UT('b'):
	case _UT('B'):
	case _UT('c'):
	case _UT('C'):
	case _UT('d'):
	case _UT('D'):
	case _UT('e'):
	case _UT('E'):
	case _UT('f'):
	case _UT('F'):
	case _UT('g'):
	case _UT('G'):
	case _UT('h'):
	case _UT('H'):
	case _UT('i'):
	case _UT('I'):
	case _UT('j'):
	case _UT('J'):
	case _UT('k'):
	case _UT('K'):
	case _UT('l'):
	case _UT('L'):
	case _UT('m'):
	case _UT('M'):
	case _UT('n'):
	case _UT('N'):
	case _UT('o'):
	case _UT('O'):
	case _UT('p'):
	case _UT('P'):
	case _UT('q'):
	case _UT('Q'):
	case _UT('r'):
	case _UT('R'):
	case _UT('s'):
	case _UT('S'):
	case _UT('t'):
	case _UT('T'):
	case _UT('u'):
	case _UT('U'):
	case _UT('v'):
	case _UT('V'):
	case _UT('w'):
	case _UT('W'):
	case _UT('x'):
	case _UT('X'):
	case _UT('y'):
	case _UT('Y'):
	case _UT('z'):
	case _UT('Z'):
		{
			const URI_CHAR * const afterSegmentNz
					= URI_FUNC(ParseSegmentNz)(state, first, afterLast);
			if (afterSegmentNz == NULL) {
				return NULL;
			}
			URI_FUNC(PushPathSegment)(state, first, afterSegmentNz); /* SEGMENT BOTH */
			return URI_FUNC(ParseZeroMoreSlashSegs)(state, afterSegmentNz, afterLast);
		}

	default:
		return first;
	}
}



/*
 * [pathRootless]->[segmentNz][zeroMoreSlashSegs]
 */
static URI_INLINE const URI_CHAR * URI_FUNC(ParsePathRootless)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast) {
	const URI_CHAR * const afterSegmentNz
			= URI_FUNC(ParseSegmentNz)(state, first, afterLast);
	if (afterSegmentNz == NULL) {
		return NULL;
	}
	return URI_FUNC(ParseZeroMoreSlashSegs)(state, afterSegmentNz, afterLast);
}



/*
 * [pchar]->[pctEncoded]
 * [pchar]->[subDelims]
 * [pchar]->[unreserved]
 * [pchar]-><:>
 * [pchar]-><@>
 */
static const URI_CHAR * URI_FUNC(ParsePchar)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast) {
	if (first >= afterLast) {
		URI_FUNC(Stop)(state, first);
		return NULL;
	}

	switch (*first) {
	case _UT('%'):
		return URI_FUNC(ParsePctEncoded)(state, first, afterLast);

	case _UT(':'):
	case _UT('@'):
	case _UT('!'):
	case _UT('$'):
	case _UT('&'):
	case _UT('('):
	case _UT(')'):
	case _UT('*'):
	case _UT(','):
	case _UT(';'):
	case _UT('\''):
	case _UT('+'):
	case _UT('='):
	case _UT('-'):
	case _UT('.'):
	case _UT('_'):
	case _UT('~'):
	case _UT('0'):
	case _UT('1'):
	case _UT('2'):
	case _UT('3'):
	case _UT('4'):
	case _UT('5'):
	case _UT('6'):
	case _UT('7'):
	case _UT('8'):
	case _UT('9'):
	case _UT('a'):
	case _UT('A'):
	case _UT('b'):
	case _UT('B'):
	case _UT('c'):
	case _UT('C'):
	case _UT('d'):
	case _UT('D'):
	case _UT('e'):
	case _UT('E'):
	case _UT('f'):
	case _UT('F'):
	case _UT('g'):
	case _UT('G'):
	case _UT('h'):
	case _UT('H'):
	case _UT('i'):
	case _UT('I'):
	case _UT('j'):
	case _UT('J'):
	case _UT('k'):
	case _UT('K'):
	case _UT('l'):
	case _UT('L'):
	case _UT('m'):
	case _UT('M'):
	case _UT('n'):
	case _UT('N'):
	case _UT('o'):
	case _UT('O'):
	case _UT('p'):
	case _UT('P'):
	case _UT('q'):
	case _UT('Q'):
	case _UT('r'):
	case _UT('R'):
	case _UT('s'):
	case _UT('S'):
	case _UT('t'):
	case _UT('T'):
	case _UT('u'):
	case _UT('U'):
	case _UT('v'):
	case _UT('V'):
	case _UT('w'):
	case _UT('W'):
	case _UT('x'):
	case _UT('X'):
	case _UT('y'):
	case _UT('Y'):
	case _UT('z'):
	case _UT('Z'):
		return first + 1;

	default:
		URI_FUNC(Stop)(state, first);
		return NULL;
	}
}



/*
 * [pctEncoded]-><%>[HEXDIG][HEXDIG]
 */
static const URI_CHAR * URI_FUNC(ParsePctEncoded)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast) {
	if (first >= afterLast) {
		URI_FUNC(Stop)(state, first);
		return NULL;
	}

	/*
	First character has already been
	checked before entering this rule.

	switch (*first) {
	case _UT('%'):
	*/
		if (first + 1 >= afterLast) {
			URI_FUNC(Stop)(state, first + 1);
			return NULL;
		}

		switch (first[1]) {
		case _UT('0'):
		case _UT('1'):
		case _UT('2'):
		case _UT('3'):
		case _UT('4'):
		case _UT('5'):
		case _UT('6'):
		case _UT('7'):
		case _UT('8'):
		case _UT('9'):
		case _UT('a'):
		case _UT('A'):
		case _UT('b'):
		case _UT('B'):
		case _UT('c'):
		case _UT('C'):
		case _UT('d'):
		case _UT('D'):
		case _UT('e'):
		case _UT('E'):
		case _UT('f'):
		case _UT('F'):
			if (first + 2 >= afterLast) {
				URI_FUNC(Stop)(state, first + 2);
				return NULL;
			}

			switch (first[2]) {
			case _UT('0'):
			case _UT('1'):
			case _UT('2'):
			case _UT('3'):
			case _UT('4'):
			case _UT('5'):
			case _UT('6'):
			case _UT('7'):
			case _UT('8'):
			case _UT('9'):
			case _UT('a'):
			case _UT('A'):
			case _UT('b'):
			case _UT('B'):
			case _UT('c'):
			case _UT('C'):
			case _UT('d'):
			case _UT('D'):
			case _UT('e'):
			case _UT('E'):
			case _UT('f'):
			case _UT('F'):
				return first + 3;

			default:
				URI_FUNC(Stop)(state, first + 2);
				return NULL;
			}

		default:
			URI_FUNC(Stop)(state, first + 1);
			return NULL;
		}

	/*
	default:
		URI_FUNC(Stop)(state, first);
		return NULL;
	}
	*/
}



/*
 * [pctSubUnres]->[pctEncoded]
 * [pctSubUnres]->[subDelims]
 * [pctSubUnres]->[unreserved]
 */
static const URI_CHAR * URI_FUNC(ParsePctSubUnres)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast) {
	if (first >= afterLast) {
		URI_FUNC(Stop)(state, first);
		return NULL;
	}

	switch (*first) {
	case _UT('%'):
		return URI_FUNC(ParsePctEncoded)(state, first, afterLast);

	case _UT('!'):
	case _UT('$'):
	case _UT('&'):
	case _UT('('):
	case _UT(')'):
	case _UT('*'):
	case _UT(','):
	case _UT(';'):
	case _UT('\''):
	case _UT('+'):
	case _UT('='):
	case _UT('-'):
	case _UT('.'):
	case _UT('_'):
	case _UT('~'):
	case _UT('0'):
	case _UT('1'):
	case _UT('2'):
	case _UT('3'):
	case _UT('4'):
	case _UT('5'):
	case _UT('6'):
	case _UT('7'):
	case _UT('8'):
	case _UT('9'):
	case _UT('a'):
	case _UT('A'):
	case _UT('b'):
	case _UT('B'):
	case _UT('c'):
	case _UT('C'):
	case _UT('d'):
	case _UT('D'):
	case _UT('e'):
	case _UT('E'):
	case _UT('f'):
	case _UT('F'):
	case _UT('g'):
	case _UT('G'):
	case _UT('h'):
	case _UT('H'):
	case _UT('i'):
	case _UT('I'):
	case _UT('j'):
	case _UT('J'):
	case _UT('k'):
	case _UT('K'):
	case _UT('l'):
	case _UT('L'):
	case _UT('m'):
	case _UT('M'):
	case _UT('n'):
	case _UT('N'):
	case _UT('o'):
	case _UT('O'):
	case _UT('p'):
	case _UT('P'):
	case _UT('q'):
	case _UT('Q'):
	case _UT('r'):
	case _UT('R'):
	case _UT('s'):
	case _UT('S'):
	case _UT('t'):
	case _UT('T'):
	case _UT('u'):
	case _UT('U'):
	case _UT('v'):
	case _UT('V'):
	case _UT('w'):
	case _UT('W'):
	case _UT('x'):
	case _UT('X'):
	case _UT('y'):
	case _UT('Y'):
	case _UT('z'):
	case _UT('Z'):
		return first + 1;

	default:
		URI_FUNC(Stop)(state, first);
		return NULL;
	}
}



/*
 * [port]->[DIGIT][port]
 * [port]-><NULL>
 */
static const URI_CHAR * URI_FUNC(ParsePort)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast) {
	if (first >= afterLast) {
		return afterLast;
	}

	switch (*first) {
	case _UT('0'):
	case _UT('1'):
	case _UT('2'):
	case _UT('3'):
	case _UT('4'):
	case _UT('5'):
	case _UT('6'):
	case _UT('7'):
	case _UT('8'):
	case _UT('9'):
		return URI_FUNC(ParsePort)(state, first + 1, afterLast);

	default:
		return first;
	}
}



/*
 * [queryFrag]->[pchar][queryFrag]
 * [queryFrag]-></>[queryFrag]
 * [queryFrag]-><?>[queryFrag]
 * [queryFrag]-><NULL>
 */
static const URI_CHAR * URI_FUNC(ParseQueryFrag)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast) {
	if (first >= afterLast) {
		return afterLast;
	}

	switch (*first) {
	case _UT('!'):
	case _UT('$'):
	case _UT('%'):
	case _UT('&'):
	case _UT('('):
	case _UT(')'):
	case _UT('-'):
	case _UT('*'):
	case _UT(','):
	case _UT('.'):
	case _UT(':'):
	case _UT(';'):
	case _UT('@'):
	case _UT('\''):
	case _UT('_'):
	case _UT('~'):
	case _UT('+'):
	case _UT('='):
	case _UT('0'):
	case _UT('1'):
	case _UT('2'):
	case _UT('3'):
	case _UT('4'):
	case _UT('5'):
	case _UT('6'):
	case _UT('7'):
	case _UT('8'):
	case _UT('9'):
	case _UT('a'):
	case _UT('A'):
	case _UT('b'):
	case _UT('B'):
	case _UT('c'):
	case _UT('C'):
	case _UT('d'):
	case _UT('D'):
	case _UT('e'):
	case _UT('E'):
	case _UT('f'):
	case _UT('F'):
	case _UT('g'):
	case _UT('G'):
	case _UT('h'):
	case _UT('H'):
	case _UT('i'):
	case _UT('I'):
	case _UT('j'):
	case _UT('J'):
	case _UT('k'):
	case _UT('K'):
	case _UT('l'):
	case _UT('L'):
	case _UT('m'):
	case _UT('M'):
	case _UT('n'):
	case _UT('N'):
	case _UT('o'):
	case _UT('O'):
	case _UT('p'):
	case _UT('P'):
	case _UT('q'):
	case _UT('Q'):
	case _UT('r'):
	case _UT('R'):
	case _UT('s'):
	case _UT('S'):
	case _UT('t'):
	case _UT('T'):
	case _UT('u'):
	case _UT('U'):
	case _UT('v'):
	case _UT('V'):
	case _UT('w'):
	case _UT('W'):
	case _UT('x'):
	case _UT('X'):
	case _UT('y'):
	case _UT('Y'):
	case _UT('z'):
	case _UT('Z'):
		{
			const URI_CHAR * const afterPchar
					= URI_FUNC(ParsePchar)(state, first, afterLast);
			if (afterPchar == NULL) {
				return NULL;
			}
			return URI_FUNC(ParseQueryFrag)(state, afterPchar, afterLast);
		}

	case _UT('/'):
	case _UT('?'):
		return URI_FUNC(ParseQueryFrag)(state, first + 1, afterLast);

	default:
		return first;
	}
}



/*
 * [segment]->[pchar][segment]
 * [segment]-><NULL>
 */
static const URI_CHAR * URI_FUNC(ParseSegment)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast) {
	if (first >= afterLast) {
		return afterLast;
	}

	switch (*first) {
	case _UT('!'):
	case _UT('$'):
	case _UT('%'):
	case _UT('&'):
	case _UT('('):
	case _UT(')'):
	case _UT('-'):
	case _UT('*'):
	case _UT(','):
	case _UT('.'):
	case _UT(':'):
	case _UT(';'):
	case _UT('@'):
	case _UT('\''):
	case _UT('_'):
	case _UT('~'):
	case _UT('+'):
	case _UT('='):
	case _UT('0'):
	case _UT('1'):
	case _UT('2'):
	case _UT('3'):
	case _UT('4'):
	case _UT('5'):
	case _UT('6'):
	case _UT('7'):
	case _UT('8'):
	case _UT('9'):
	case _UT('a'):
	case _UT('A'):
	case _UT('b'):
	case _UT('B'):
	case _UT('c'):
	case _UT('C'):
	case _UT('d'):
	case _UT('D'):
	case _UT('e'):
	case _UT('E'):
	case _UT('f'):
	case _UT('F'):
	case _UT('g'):
	case _UT('G'):
	case _UT('h'):
	case _UT('H'):
	case _UT('i'):
	case _UT('I'):
	case _UT('j'):
	case _UT('J'):
	case _UT('k'):
	case _UT('K'):
	case _UT('l'):
	case _UT('L'):
	case _UT('m'):
	case _UT('M'):
	case _UT('n'):
	case _UT('N'):
	case _UT('o'):
	case _UT('O'):
	case _UT('p'):
	case _UT('P'):
	case _UT('q'):
	case _UT('Q'):
	case _UT('r'):
	case _UT('R'):
	case _UT('s'):
	case _UT('S'):
	case _UT('t'):
	case _UT('T'):
	case _UT('u'):
	case _UT('U'):
	case _UT('v'):
	case _UT('V'):
	case _UT('w'):
	case _UT('W'):
	case _UT('x'):
	case _UT('X'):
	case _UT('y'):
	case _UT('Y'):
	case _UT('z'):
	case _UT('Z'):
		{
			const URI_CHAR * const afterPchar
					= URI_FUNC(ParsePchar)(state, first, afterLast);
			if (afterPchar == NULL) {
				return NULL;
			}
			return URI_FUNC(ParseSegment)(state, afterPchar, afterLast);
		}

	default:
		return first;
	}
}



/*
 * [segmentNz]->[pchar][segment]
 */
static URI_INLINE const URI_CHAR * URI_FUNC(ParseSegmentNz)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast) {
	const URI_CHAR * const afterPchar
			= URI_FUNC(ParsePchar)(state, first, afterLast);
	if (afterPchar == NULL) {
		return NULL;
	}
	return URI_FUNC(ParseSegment)(state, afterPchar, afterLast);
}



static URI_INLINE void URI_FUNC(OnExitSegmentNzNcOrScheme2)(URI_TYPE(ParserState) * state, const URI_CHAR * first) {
	URI_FUNC(PushPathSegment)(state, state->uri->scheme.first, first); /* SEGMENT BOTH */
	state->uri->scheme.first = NULL; /* Not a scheme, reset */
}



/*
 * [segmentNzNcOrScheme2]->[ALPHA][segmentNzNcOrScheme2]
 * [segmentNzNcOrScheme2]->[DIGIT][segmentNzNcOrScheme2]
 * [segmentNzNcOrScheme2]->[pctEncoded][mustBeSegmentNzNc]
 * [segmentNzNcOrScheme2]->[uriTail] // can take <NULL>
 * [segmentNzNcOrScheme2]-><!>[mustBeSegmentNzNc]
 * [segmentNzNcOrScheme2]-><$>[mustBeSegmentNzNc]
 * [segmentNzNcOrScheme2]-><&>[mustBeSegmentNzNc]
 * [segmentNzNcOrScheme2]-><(>[mustBeSegmentNzNc]
 * [segmentNzNcOrScheme2]-><)>[mustBeSegmentNzNc]
 * [segmentNzNcOrScheme2]-><*>[mustBeSegmentNzNc]
 * [segmentNzNcOrScheme2]-><,>[mustBeSegmentNzNc]
 * [segmentNzNcOrScheme2]-><.>[segmentNzNcOrScheme2]
 * [segmentNzNcOrScheme2]-></>[segment][zeroMoreSlashSegs][uriTail]
 * [segmentNzNcOrScheme2]-><:>[hierPart][uriTail]
 * [segmentNzNcOrScheme2]-><;>[mustBeSegmentNzNc]
 * [segmentNzNcOrScheme2]-><@>[mustBeSegmentNzNc]
 * [segmentNzNcOrScheme2]-><_>[mustBeSegmentNzNc]
 * [segmentNzNcOrScheme2]-><~>[mustBeSegmentNzNc]
 * [segmentNzNcOrScheme2]-><+>[segmentNzNcOrScheme2]
 * [segmentNzNcOrScheme2]-><=>[mustBeSegmentNzNc]
 * [segmentNzNcOrScheme2]-><'>[mustBeSegmentNzNc]
 * [segmentNzNcOrScheme2]-><->[segmentNzNcOrScheme2]
 */
static const URI_CHAR * URI_FUNC(ParseSegmentNzNcOrScheme2)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast) {
	if (first >= afterLast) {
		URI_FUNC(OnExitSegmentNzNcOrScheme2)(state, first);
		return afterLast;
	}

	switch (*first) {
	case _UT('.'):
	case _UT('+'):
	case _UT('-'):
	case _UT('a'):
	case _UT('A'):
	case _UT('b'):
	case _UT('B'):
	case _UT('c'):
	case _UT('C'):
	case _UT('d'):
	case _UT('D'):
	case _UT('e'):
	case _UT('E'):
	case _UT('f'):
	case _UT('F'):
	case _UT('g'):
	case _UT('G'):
	case _UT('h'):
	case _UT('H'):
	case _UT('i'):
	case _UT('I'):
	case _UT('j'):
	case _UT('J'):
	case _UT('k'):
	case _UT('K'):
	case _UT('l'):
	case _UT('L'):
	case _UT('m'):
	case _UT('M'):
	case _UT('n'):
	case _UT('N'):
	case _UT('o'):
	case _UT('O'):
	case _UT('p'):
	case _UT('P'):
	case _UT('q'):
	case _UT('Q'):
	case _UT('r'):
	case _UT('R'):
	case _UT('s'):
	case _UT('S'):
	case _UT('t'):
	case _UT('T'):
	case _UT('u'):
	case _UT('U'):
	case _UT('v'):
	case _UT('V'):
	case _UT('w'):
	case _UT('W'):
	case _UT('x'):
	case _UT('X'):
	case _UT('y'):
	case _UT('Y'):
	case _UT('z'):
	case _UT('Z'):
	case _UT('0'):
	case _UT('1'):
	case _UT('2'):
	case _UT('3'):
	case _UT('4'):
	case _UT('5'):
	case _UT('6'):
	case _UT('7'):
	case _UT('8'):
	case _UT('9'):
		return URI_FUNC(ParseSegmentNzNcOrScheme2)(state, first + 1, afterLast);

	case _UT('%'):
		{
			const URI_CHAR * const afterPctEncoded
					= URI_FUNC(ParsePctEncoded)(state, first, afterLast);
			if (afterPctEncoded == NULL) {
				return NULL;
			}
			return URI_FUNC(ParseMustBeSegmentNzNc)(state, afterPctEncoded, afterLast);
		}

	case _UT('!'):
	case _UT('$'):
	case _UT('&'):
	case _UT('('):
	case _UT(')'):
	case _UT('*'):
	case _UT(','):
	case _UT(';'):
	case _UT('@'):
	case _UT('_'):
	case _UT('~'):
	case _UT('='):
	case _UT('\''):
		return URI_FUNC(ParseMustBeSegmentNzNc)(state, first + 1, afterLast);

	case _UT('/'):
		{
			const URI_CHAR * afterZeroMoreSlashSegs;
			const URI_CHAR * const afterSegment
					= URI_FUNC(ParseSegment)(state, first + 1, afterLast);
			if (afterSegment == NULL) {
				return NULL;
			}
			URI_FUNC(PushPathSegment)(state, state->uri->scheme.first, first); /* SEGMENT BOTH */
			state->uri->scheme.first = NULL; /* Not a scheme, reset */
			URI_FUNC(PushPathSegment)(state, first + 1, afterSegment); /* SEGMENT BOTH */
			afterZeroMoreSlashSegs
					= URI_FUNC(ParseZeroMoreSlashSegs)(state, afterSegment, afterLast);
			if (afterZeroMoreSlashSegs == NULL) {
				return NULL;
			}
			return URI_FUNC(ParseUriTail)(state, afterZeroMoreSlashSegs, afterLast);
		}

	case _UT(':'):
		{
			const URI_CHAR * const afterHierPart
					= URI_FUNC(ParseHierPart)(state, first + 1, afterLast);
			state->uri->scheme.afterLast = first; /* SCHEME END */
			if (afterHierPart == NULL) {
				return NULL;
			}
			return URI_FUNC(ParseUriTail)(state, afterHierPart, afterLast);
		}

	default:
		URI_FUNC(OnExitSegmentNzNcOrScheme2)(state, first);
		return URI_FUNC(ParseUriTail)(state, first, afterLast);
	}
}



/*
 * [uriReference]->[ALPHA][segmentNzNcOrScheme2]
 * [uriReference]->[DIGIT][mustBeSegmentNzNc]
 * [uriReference]->[pctEncoded][mustBeSegmentNzNc]
 * [uriReference]->[subDelims][mustBeSegmentNzNc]
 * [uriReference]->[uriTail] // can take <NULL>
 * [uriReference]-><.>[mustBeSegmentNzNc]
 * [uriReference]-></>[partHelperTwo][uriTail]
 * [uriReference]-><@>[mustBeSegmentNzNc]
 * [uriReference]-><_>[mustBeSegmentNzNc]
 * [uriReference]-><~>[mustBeSegmentNzNc]
 * [uriReference]-><->[mustBeSegmentNzNc]
 */
static const URI_CHAR * URI_FUNC(ParseUriReference)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast) {
	if (first >= afterLast) {
		return afterLast;
	}

	switch (*first) {
	case _UT('a'):
	case _UT('A'):
	case _UT('b'):
	case _UT('B'):
	case _UT('c'):
	case _UT('C'):
	case _UT('d'):
	case _UT('D'):
	case _UT('e'):
	case _UT('E'):
	case _UT('f'):
	case _UT('F'):
	case _UT('g'):
	case _UT('G'):
	case _UT('h'):
	case _UT('H'):
	case _UT('i'):
	case _UT('I'):
	case _UT('j'):
	case _UT('J'):
	case _UT('k'):
	case _UT('K'):
	case _UT('l'):
	case _UT('L'):
	case _UT('m'):
	case _UT('M'):
	case _UT('n'):
	case _UT('N'):
	case _UT('o'):
	case _UT('O'):
	case _UT('p'):
	case _UT('P'):
	case _UT('q'):
	case _UT('Q'):
	case _UT('r'):
	case _UT('R'):
	case _UT('s'):
	case _UT('S'):
	case _UT('t'):
	case _UT('T'):
	case _UT('u'):
	case _UT('U'):
	case _UT('v'):
	case _UT('V'):
	case _UT('w'):
	case _UT('W'):
	case _UT('x'):
	case _UT('X'):
	case _UT('y'):
	case _UT('Y'):
	case _UT('z'):
	case _UT('Z'):
		state->uri->scheme.first = first; /* SCHEME BEGIN */
		return URI_FUNC(ParseSegmentNzNcOrScheme2)(state, first + 1, afterLast);

	case _UT('0'):
	case _UT('1'):
	case _UT('2'):
	case _UT('3'):
	case _UT('4'):
	case _UT('5'):
	case _UT('6'):
	case _UT('7'):
	case _UT('8'):
	case _UT('9'):
	case _UT('!'):
	case _UT('$'):
	case _UT('&'):
	case _UT('('):
	case _UT(')'):
	case _UT('*'):
	case _UT(','):
	case _UT(';'):
	case _UT('\''):
	case _UT('+'):
	case _UT('='):
	case _UT('.'):
	case _UT('_'):
	case _UT('~'):
	case _UT('-'):
	case _UT('@'):
		state->uri->scheme.first = first; /* SCHEME BEGIN */
		return URI_FUNC(ParseMustBeSegmentNzNc)(state, first + 1, afterLast);

	case _UT('%'):
		{
			const URI_CHAR * const afterPctEncoded
					= URI_FUNC(ParsePctEncoded)(state, first, afterLast);
			if (afterPctEncoded == NULL) {
				return NULL;
			}
			state->uri->scheme.first = first; /* SCHEME BEGIN */
			return URI_FUNC(ParseMustBeSegmentNzNc)(state, afterPctEncoded, afterLast);
		}

	case _UT('/'):
		{
			const URI_CHAR * const afterPartHelperTwo
					= URI_FUNC(ParsePartHelperTwo)(state, first + 1, afterLast);
			if (afterPartHelperTwo == NULL) {
				return NULL;
			}
			return URI_FUNC(ParseUriTail)(state, afterPartHelperTwo, afterLast);
		}

	default:
		return URI_FUNC(ParseUriTail)(state, first, afterLast);
	}
}



/*
 * [uriTail]-><#>[queryFrag]
 * [uriTail]-><?>[queryFrag][uriTailTwo]
 * [uriTail]-><NULL>
 */
static URI_INLINE const URI_CHAR * URI_FUNC(ParseUriTail)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast) {
	if (first >= afterLast) {
		return afterLast;
	}

	switch (*first) {
	case _UT('#'):
		{
			const URI_CHAR * const afterQueryFrag = URI_FUNC(ParseQueryFrag)(state, first + 1, afterLast);
			if (afterQueryFrag == NULL) {
				return NULL;
			}
			state->uri->fragment.first = first + 1; /* FRAGMENT BEGIN */
			state->uri->fragment.afterLast = afterQueryFrag; /* FRAGMENT END */
			return afterQueryFrag;
		}

	case _UT('?'):
		{
			const URI_CHAR * const afterQueryFrag
					= URI_FUNC(ParseQueryFrag)(state, first + 1, afterLast);
			if (afterQueryFrag == NULL) {
				return NULL;
			}
			state->uri->query.first = first + 1; /* QUERY BEGIN */
			state->uri->query.afterLast = afterQueryFrag; /* QUERY END */
			return URI_FUNC(ParseUriTailTwo)(state, afterQueryFrag, afterLast);
		}

	default:
		return first;
	}
}



/*
 * [uriTailTwo]-><#>[queryFrag]
 * [uriTailTwo]-><NULL>
 */
static URI_INLINE const URI_CHAR * URI_FUNC(ParseUriTailTwo)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast) {
	if (first >= afterLast) {
		return afterLast;
	}

	switch (*first) {
	case _UT('#'):
		{
			const URI_CHAR * const afterQueryFrag = URI_FUNC(ParseQueryFrag)(state, first + 1, afterLast);
			if (afterQueryFrag == NULL) {
				return NULL;
			}
			state->uri->fragment.first = first + 1; /* FRAGMENT BEGIN */
			state->uri->fragment.afterLast = afterQueryFrag; /* FRAGMENT END */
			return afterQueryFrag;
		}

	default:
		return first;
	}
}



/*
 * [zeroMoreSlashSegs]-></>[segment][zeroMoreSlashSegs]
 * [zeroMoreSlashSegs]-><NULL>
 */
static const URI_CHAR * URI_FUNC(ParseZeroMoreSlashSegs)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast) {
	if (first >= afterLast) {
		return afterLast;
	}

	switch (*first) {
	case _UT('/'):
		{
			const URI_CHAR * const afterSegment
					= URI_FUNC(ParseSegment)(state, first + 1, afterLast);
			if (afterSegment == NULL) {
				return NULL;
			}
			URI_FUNC(PushPathSegment)(state, first + 1, afterSegment); /* SEGMENT BOTH */
			return URI_FUNC(ParseZeroMoreSlashSegs)(state, afterSegment, afterLast);
		}

	default:
		return first;
	}
}



static URI_INLINE void URI_FUNC(ResetParserState)(URI_TYPE(ParserState) * state) {
	URI_TYPE(Uri) * const uriBackup = state->uri;
	memset(state, 0, sizeof(URI_TYPE(ParserState)));
	state->uri = uriBackup;
}



static URI_INLINE void URI_FUNC(ResetUri)(URI_TYPE(Uri) * uri) {
	memset(uri, 0, sizeof(URI_TYPE(Uri)));
}



static URI_INLINE void URI_FUNC(PushPathSegment)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast) {
	URI_TYPE(PathSegment) * segment = malloc(1 * sizeof(URI_TYPE(PathSegment)));
	memset(segment, 0, sizeof(URI_TYPE(PathSegment)));
	segment->text.first = first;
	segment->text.afterLast = afterLast;

	if (state->uri->pathHead == NULL) {
		state->uri->pathHead = segment;
		state->uri->pathTail = segment;
	} else {
		state->uri->pathTail->next = segment;
		state->uri->pathTail = segment;
	}
}



int URI_FUNC(ParseUriEx)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast) {
	const URI_CHAR * afterUriReference;
	URI_TYPE(Uri) * uri;

	/* Check params */
	if ((state == NULL) || (state == NULL) || (state == NULL)) {
		return URI_ERROR_NULL;
	}
	uri = state->uri;

	/* Init parser */
	URI_FUNC(ResetParserState)(state);
	URI_FUNC(ResetUri)(uri);

	/* Parse */
	afterUriReference = URI_FUNC(ParseUriReference)(state, first, afterLast);
	if (afterUriReference == NULL) {
		return state->errorCode;
	}
	if (afterUriReference != afterLast) {
		return URI_ERROR_SYNTAX;
	}
	return URI_SUCCESS;
}



int URI_FUNC(ParseUri)(URI_TYPE(ParserState) * state, const URI_CHAR * text) {
	if ((state == NULL) || (text == NULL)) {
		return URI_ERROR_NULL;
	}
	return URI_FUNC(ParseUriEx)(state, text, text + URI_STRLEN(text));
}



void URI_FUNC(FreeUriMembers)(URI_TYPE(Uri) * uri) {
	if (uri == NULL) {
		return;
	}
	if (uri->hostData.ip4 != NULL) {
		free(uri->hostData.ip4);
		uri->hostData.ip4 = NULL;
	}
	if (uri->hostData.ip6 != NULL) {
		free(uri->hostData.ip6);
		uri->hostData.ip6 = NULL;
	}
	if (uri->pathHead != NULL) {
		URI_TYPE(PathSegment) * segWalk = uri->pathHead;
		while (segWalk != NULL) {
			URI_TYPE(PathSegment) * const next = segWalk->next;
			free(segWalk);
			segWalk = next;
		}
		uri->pathHead = NULL;
		uri->pathTail = NULL;
	}
}



unsigned char URI_FUNC(HexdigToInt)(URI_CHAR hexdig) {
	switch (hexdig) {
	case _UT('0'):
	case _UT('1'):
	case _UT('2'):
	case _UT('3'):
	case _UT('4'):
	case _UT('5'):
	case _UT('6'):
	case _UT('7'):
	case _UT('8'):
	case _UT('9'):
		return (unsigned char)(9 + hexdig - _UT('9'));

	case _UT('a'):
	case _UT('b'):
	case _UT('c'):
	case _UT('d'):
	case _UT('e'):
	case _UT('f'):
		return (unsigned char)(15 + hexdig - _UT('f'));

	case _UT('A'):
	case _UT('B'):
	case _UT('C'):
	case _UT('D'):
	case _UT('E'):
	case _UT('F'):
		return (unsigned char)(15 + hexdig - _UT('F'));

	default:
		return 0;
	}
}



const URI_CHAR * URI_FUNC(UnescapeInPlace)(URI_CHAR * inout) {
	URI_CHAR * read = inout;
	URI_CHAR * write = inout;

	if (inout == NULL) {
		return NULL;
	}

	for (;;) {
		switch (read[0]) {
		case _UT('\0'):
			if (read > write) {
				write[0] = _UT('\0');
			}
			return write;

		case _UT('%'):
			switch (read[1]) {
			case _UT('0'):
			case _UT('1'):
			case _UT('2'):
			case _UT('3'):
			case _UT('4'):
			case _UT('5'):
			case _UT('6'):
			case _UT('7'):
			case _UT('8'):
			case _UT('9'):
			case _UT('a'):
			case _UT('b'):
			case _UT('c'):
			case _UT('d'):
			case _UT('e'):
			case _UT('f'):
			case _UT('A'):
			case _UT('B'):
			case _UT('C'):
			case _UT('D'):
			case _UT('E'):
			case _UT('F'):
				switch (read[2]) {
				case _UT('0'):
				case _UT('1'):
				case _UT('2'):
				case _UT('3'):
				case _UT('4'):
				case _UT('5'):
				case _UT('6'):
				case _UT('7'):
				case _UT('8'):
				case _UT('9'):
				case _UT('a'):
				case _UT('b'):
				case _UT('c'):
				case _UT('d'):
				case _UT('e'):
				case _UT('f'):
				case _UT('A'):
				case _UT('B'):
				case _UT('C'):
				case _UT('D'):
				case _UT('E'):
				case _UT('F'):
					{
						/* Percent group found, make single char of that */
						const unsigned char left = URI_FUNC(HexdigToInt)(read[1]);
						const unsigned char right = URI_FUNC(HexdigToInt)(read[2]);
						write[0] = (URI_CHAR)(16 * left + right);
						read += 3;
						write++;
					}
					break;

				default:
					/* Copy two chars unmodified and */
					/* look at this char again */
					if (read > write) {
						write[0] = read[0];
						write[1] = read[1];
					}
					read += 2;
					write += 2;
				}
				break;

			default:
				/* Copy one char unmodified and */
				/* look at this char again */
				if (read > write) {
					write[0] = read[0];
				}
				read++;
				write++;
			}
			break;

		default:
			/* Copy one char unmodified */
			if (read > write) {
				write[0] = read[0];
			}
			read++;
			write++;
		}
	}
}



/* TODO */
UriBool URI_FUNC(ParseIpSix)(const URI_CHAR * text) {
	URI_TYPE(Uri) uri;
	URI_TYPE(ParserState) parser;
	const URI_CHAR * const afterIpSix = text + URI_STRLEN(text);
	const URI_CHAR * res;

	URI_FUNC(ResetParserState)(&parser);
	URI_FUNC(ResetUri)(&uri);
	parser.uri = &uri;
	parser.uri->hostData.ip6 = malloc(1 * sizeof(UriIp6));
	res = URI_FUNC(ParseIPv6address2)(&parser, text, afterIpSix);
	URI_FUNC(FreeUriMembers)(&uri);
	return res == afterIpSix ? URI_TRUE : URI_FALSE;
}



#endif
