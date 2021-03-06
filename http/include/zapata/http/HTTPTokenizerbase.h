// Generated by Bisonc++ V5.02.00 on Sat, 25 Mar 2017 16:02:37 +0000

#ifndef zptHTTPTokenizerBase_h_included
#define zptHTTPTokenizerBase_h_included

#include <exception>
#include <vector>
#include <iostream>

// $insert preincludes
#include "HTTPinc.h"

namespace // anonymous
{
struct PI__;
}

// $insert namespace-open
namespace zpt {

class HTTPTokenizerBase {
      public:
	enum DebugMode__ { OFF = 0, ON = 1 << 0, ACTIONCASES = 1 << 1 };

// $insert tokens

#ifdef VERSION
#undef VERSION
#endif

	// Symbolic tokens:
	enum Tokens__ {
		METHOD = 257,
		VERSION,
		URL,
		STATUS,
		CR_LF,
		COLON,
		STRING,
		SPACE,
		BODY,
		QMARK,
		EQ,
		E,
	};

	// $insert STYPE
	typedef int STYPE__;

      private:
	int d_stackIdx__ = -1;
	std::vector<size_t> d_stateStack__;
	std::vector<STYPE__> d_valueStack__;

      protected:
	enum Return__ {
		PARSE_ACCEPT__ = 0, // values used as parse()'s return values
		PARSE_ABORT__ = 1
	};
	enum ErrorRecovery__ {
		DEFAULT_RECOVERY_MODE__,
		UNEXPECTED_TOKEN__,
	};
	bool d_actionCases__ = false;
	bool d_debug__ = true;
	size_t d_nErrors__ = 0;
	size_t d_requiredTokens__;
	size_t d_acceptedTokens__;
	int d_token__;
	int d_nextToken__;
	size_t d_state__;
	STYPE__* d_vsp__;
	STYPE__ d_val__;
	STYPE__ d_nextVal__;

	HTTPTokenizerBase();

	void ABORT() const;
	void ACCEPT() const;
	void ERROR() const;
	void clearin();
	bool actionCases() const;
	bool debug() const;
	void pop__(size_t count = 1);
	void push__(size_t nextState);
	void popToken__();
	void pushToken__(int token);
	void reduce__(PI__ const& productionInfo);
	void errorVerbose__();
	size_t top__() const;

      public:
	void setDebug(bool mode);
	void setDebug(DebugMode__ mode);
};

inline HTTPTokenizerBase::DebugMode__ operator|(HTTPTokenizerBase::DebugMode__ lhs,
						HTTPTokenizerBase::DebugMode__ rhs) {
	return static_cast<HTTPTokenizerBase::DebugMode__>(static_cast<int>(lhs) | rhs);
};

inline bool HTTPTokenizerBase::debug() const { return d_debug__; }

inline bool HTTPTokenizerBase::actionCases() const { return d_actionCases__; }

inline void HTTPTokenizerBase::ABORT() const { throw PARSE_ABORT__; }

inline void HTTPTokenizerBase::ACCEPT() const { throw PARSE_ACCEPT__; }

inline void HTTPTokenizerBase::ERROR() const { throw UNEXPECTED_TOKEN__; }

inline HTTPTokenizerBase::DebugMode__ operator&(HTTPTokenizerBase::DebugMode__ lhs,
						HTTPTokenizerBase::DebugMode__ rhs) {
	return static_cast<HTTPTokenizerBase::DebugMode__>(static_cast<int>(lhs) & rhs);
}

// For convenience, when including ParserBase.h its symbols are available as
// symbols in the class Parser, too.
#define HTTPTokenizer HTTPTokenizerBase

// $insert namespace-close
}

#endif
