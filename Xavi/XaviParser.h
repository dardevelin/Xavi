/*
 * XaviParser.h: Parser for Xavi.
 * Copyright 2014 Vincent Damewood
 *
 * This file is part of Xavi.
 *
 * Xavi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * Xavi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Xavi. If not, see <http://www.gnu.org/licenses/>.
 */

#if !defined XAVI_PARSER_H
#define XAVI_PARSER_H

#if !defined USE_BISON
#define USE_BISON 0
#endif /* USE_BISON */


#if USE_BISON
#include "XaviYyParser.h"
typedef YYSTYPE XaviTokenValue;
#else
#include "Xavi.h"
#include "XaviStructs.h"

enum yytokentype
{
     EOL = 0,
     INTEGER = 258,
     FLOAT = 259,
     ID = 260,
     ERROR = 261
};


union XaviTokenValue
{
        XaviTree * t;
        XaviArglist * a;
        char * s;
        int i;
        float f;
};
typedef union XaviTokenValue XaviTokenValue;

#include "XaviLexer.h"

int XaviInternalParse(XaviNumber *, XaviMemoryPool *, XaviLexer *);
#endif /* !USE_BISON */

#endif /* XAVI_PARSER_H */
