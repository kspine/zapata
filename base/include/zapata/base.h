/*
The MIT License (MIT)

Copyright (c) 2014 n@zgul <naazgull@dfz.pt>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include <zapata/base/assert.h>
#include <zapata/base/config.h>
#include <zapata/base/Job.h>

#include <zapata/text/convert.h>
#include <zapata/text/manip.h>
#include <zapata/text/html.h>

#include <zapata/log/log.h>

#include <zapata/mem/usage.h>

#include <zapata/exceptions/CastException.h>
#include <zapata/exceptions/ClosedException.h>
#include <zapata/exceptions/ParserEOF.h>
#include <zapata/exceptions/NoAttributeNameException.h>
#include <zapata/exceptions/InterruptedException.h>
#include <zapata/exceptions/SyntaxErrorException.h>

#include <zapata/file/manip.h>

#include <zapata/mail/manip.h>
