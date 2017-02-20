// Generated by Bisonc++ V4.13.01 on Fri, 28 Oct 2016 20:27:35 +0100

    // Include this file in the sources of the class JSONTokenizer.

// $insert class.h
#include <zapata/json/JSONTokenizer.h>
#include <zapata/exceptions/SyntaxErrorException.h>

// $insert namespace-open
namespace zpt
{

inline void JSONTokenizer::error(char const *msg)
{
	throw zpt::SyntaxErrorException(std::string("Syntax error in line ") + std::to_string(d_scanner.lineNr()));
}

// $insert lex
inline int JSONTokenizer::lex()
{
    return d_scanner.lex();
}

inline void JSONTokenizer::print()         
{
    print__();           // displays tokens if --print was specified
}

inline void JSONTokenizer::exceptionHandler__(std::exception const &exc)         
{
    throw;              // re-implement to handle exceptions thrown by actions
}

// $insert namespace-close
}

    // Add here includes that are only required for the compilation 
    // of JSONTokenizer's sources.


// $insert namespace-use
    // UN-comment the next using-declaration if you want to use
    // symbols from the namespace zpt without specifying zpt::
//using namespace zpt;

    // UN-comment the next using-declaration if you want to use
    // int JSONTokenizer's sources symbols from the namespace std without
    // specifying std::

//using namespace std;
