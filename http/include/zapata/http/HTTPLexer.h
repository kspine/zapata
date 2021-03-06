// Generated by Flexc++ V2.05.00 on Fri, 11 Aug 2017 14:04:23 +0100

#ifndef zptHTTPLexer_H_INCLUDED_
#define zptHTTPLexer_H_INCLUDED_

// $insert baseclass_h
#include "HTTPLexerbase.h"

// $insert namespace-open
namespace zpt {

// $insert classHead
class HTTPLexer : public HTTPLexerBase {
      public:
	explicit HTTPLexer(std::istream& in = std::cin, std::ostream& out = std::cout);

	HTTPLexer(std::string const& infile, std::string const& outfile);

	// $insert lexFunctionDecl
	int lex();

      private:
	int lex__();
	int executeAction__(size_t ruleNr);

	void print();
	void preCode(); // re-implement this function for code that must
			// be exec'ed before the patternmatching starts

	void postCode(PostEnum__ type);
	// re-implement this function for code that must
	// be exec'ed after the rules's actions.
};

// $insert scannerConstructors
inline HTTPLexer::HTTPLexer(std::istream& in, std::ostream& out) : HTTPLexerBase(in, out) {}

inline HTTPLexer::HTTPLexer(std::string const& infile, std::string const& outfile) : HTTPLexerBase(infile, outfile) {}

// $insert inlineLexFunction
inline int HTTPLexer::lex() { return lex__(); }

inline void HTTPLexer::preCode() {
	// optionally replace by your own code
}

inline void HTTPLexer::postCode(PostEnum__ type) {
	// optionally replace by your own code
}

inline void HTTPLexer::print() { print__(); }

// $insert namespace-close
}

#endif // HTTPLexer_H_INCLUDED_
