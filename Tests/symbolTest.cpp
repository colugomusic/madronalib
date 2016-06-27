
//
// symbolTest
// a unit test made using the Catch framework in catch.hpp / tests.cpp.
//

#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <chrono>
#include <thread>

#include "catch.hpp"
#include "../include/madronalib.h"
#include "MLTextUtils.h"


#if _WIN32
#define HAVE_U8_LITERALS 0
#else
#define HAVE_U8_LITERALS 1
#endif

static const int kThreadTestSize = 1024; 

using namespace ml;

void threadTest(int threadID)
{
	textUtils::NameMaker namer;
	for(int i=0; i<kThreadTestSize; ++i)
	{
		Symbol sym(namer.nextName());
		std::this_thread::yield();
	}
}

TEST_CASE("madronalib/core/symbol/threads", "[symbol][threads]")
{
	// multithreaded test. multiple nameMakers will try to make duplicate names at about the same time,
	// which will almost certainly lead to problems unless the symbol code is properly thread-safe.
	
	theSymbolTable().clear();
	int nThreads = 16;
	std::vector< std::thread > threads;
	for(int i=0; i < nThreads; ++i)
	{
		threads.push_back(std::thread(threadTest, i));
	}
	for(int i=0; i < nThreads; ++i)
	{
		threads[i].join();
	}
	
	REQUIRE(theSymbolTable().audit());
	REQUIRE(theSymbolTable().getSize() == kThreadTestSize + 1);
}

TEST_CASE("madronalib/core/collision", "[collision]")
{
	// nothing is checked here - these are two pairs of colliding symbols for reference
	Symbol a("mse");
	Symbol aa("KL");
	Symbol b("AAAAAAAAAAAAAAwwdmbs");
	Symbol bb("AAAAAAAAAAAAAAjntepetnj");
}

template<size_t N>
constexpr int hashTest1(const char (&sym)[N]) { return krHash1<N>(sym); }

TEST_CASE("madronalib/core/hashes", "[hashes]")
{
	// the compile time and runtime hashes need to be equivalent.
	const char* str1("hello");
	const char* str2(u8"محمد بن سعيد");
	
	constexpr int a1 = hashTest1("hello");
	constexpr int a2 = hashTest1(u8"محمد بن سعيد");
	
	int b1 = krHash0(str1, strlen(str1));
	int b2 = krHash0(str2, strlen(str2));
	
	REQUIRE(a1 == b1);
	REQUIRE(a2 == b2);
}

const char letters[24] = "abcdefghjklmnopqrstuvw";

TEST_CASE("madronalib/core/symbol/maps", "[symbol]")
{
	const int kTableSize = 100;	
	const int kTestLength = 100000;
	
	// main maps for testing
	std::map<Symbol, float> testMapOrderedSym;
	std::map<std::string, float> testMapOrderedStr;
	std::unordered_map<Symbol, float> testMapUnorderedSym;
	std::unordered_map<std::string, float> testMapUnorderedStr;
	
	// make dictionaries of symbols, strings and chars for testing
	std::vector<Symbol> symbolDict;
	std::vector<std::string> stringDict;
	std::vector<const char *> charDict;
	int p = 0;
	for(int i=0; i<kTableSize; ++i)
	{
		// make procedural gibberish
		std::string newString;
		int length = 3 + (p%8);
		for(int j=0; j<length; ++j)
		{
			p += (i*j + 1);
			p += i%37;
			p += j%23;
			p = abs(p);
			newString += letters[p % 22];
		}		
		
		stringDict.push_back(newString);
		
		// add it to symbol table
		Symbol newSym(newString.c_str());
		symbolDict.push_back(newSym);
		
		// add an entry to each map
		float val = i;
		testMapOrderedSym[newSym] = val;
		testMapOrderedStr[newString] = val;
		testMapUnorderedSym[newSym] = val;
		testMapUnorderedStr[newString] = val;
	}
	
	// make char dict after string dict is complete, otherwise ptrs may change!
	for(int i=0; i<stringDict.size(); ++i)
	{
		charDict.push_back(stringDict[i].c_str());
	}
	
	SECTION("test maps")
	{
		std::chrono::time_point<std::chrono::system_clock> start, end;
		std::chrono::duration<double> elapsed;
		double symbolSum, stringSum;
		int idx;
		
		// lookup from existing std::strings
		start = std::chrono::system_clock::now();
		stringSum = 0.f;
		idx = 0;
		for(int i=0; i<kTestLength; ++i)
		{
			if(++idx >= kTableSize) idx = 0;	
			stringSum += testMapOrderedStr[stringDict[idx]];
		}	
		end = std::chrono::system_clock::now();
		elapsed = end-start;
		std::cout << "existing strings, elapsed time: " << elapsed.count() << "s\n";
		
		// lookup from existing MLSymbols
		start = std::chrono::system_clock::now();
		symbolSum = 0.f;
		idx = 0;
		for(int i=0; i<kTestLength; ++i)
		{
			if(++idx >= kTableSize) idx = 0;	
			symbolSum += testMapOrderedSym[symbolDict[idx]];
		}
		end = std::chrono::system_clock::now();
		elapsed = end-start;
		std::cout << "existing symbols, elapsed time: " << elapsed.count() << "s\n";
		
		REQUIRE(stringSum == symbolSum);
		
		// lookup from existing std::strings
		start = std::chrono::system_clock::now();
		stringSum = 0.f;
		idx = 0;
		for(int i=0; i<kTestLength; ++i)
		{
			if(++idx >= kTableSize) idx = 0;	
			stringSum += testMapUnorderedStr[stringDict[idx]];
		}	
		end = std::chrono::system_clock::now();
		elapsed = end-start;
		std::cout << "existing strings, unordered, elapsed time: " << elapsed.count() << "s\n";
		
		// lookup from existing MLSymbols
		start = std::chrono::system_clock::now();
		symbolSum = 0.f;
		idx = 0;
		for(int i=0; i<kTestLength; ++i)
		{
			if(++idx >= kTableSize) idx = 0;	
			symbolSum += testMapUnorderedSym[symbolDict[idx]];
		}
		end = std::chrono::system_clock::now();
		elapsed = end-start;
		std::cout << "existing symbols, unordered, elapsed time: " << elapsed.count() << "s\n";

		REQUIRE(stringSum == symbolSum);
		
		// lookup from newly made std::strings
		start = std::chrono::system_clock::now();
		stringSum = 0.f;
		idx = 0;
		for(int i=0; i<kTestLength; ++i)
		{
			if(++idx >= kTableSize) idx = 0;	
			stringSum += testMapOrderedStr[charDict[idx]];
		}	
		end = std::chrono::system_clock::now();
		elapsed = end-start;
		std::cout << "constructing strings, elapsed time: " << elapsed.count() << "s\n";
		
		// lookup from new MLSymbols made from char * 
		start = std::chrono::system_clock::now();
		symbolSum = 0.f;
		idx = 0;
		for(int i=0; i<kTestLength; ++i)
		{
			if(++idx >= kTableSize) idx = 0;	
			symbolSum += testMapOrderedSym[charDict[idx]];
		}
		end = std::chrono::system_clock::now();
		elapsed = end-start;
		std::cout << "constructing symbols, elapsed time: " << elapsed.count() << "s\n";

		REQUIRE(stringSum == symbolSum);
				
		// lookup from newly made std::strings
		start = std::chrono::system_clock::now();
		stringSum = 0.f;
		idx = 0;
		for(int i=0; i<kTestLength; ++i)
		{
			if(++idx >= kTableSize) idx = 0;	
			stringSum += testMapUnorderedStr[charDict[idx]];
		}	
		end = std::chrono::system_clock::now();
		elapsed = end-start;
		std::cout << "constructing strings, unordered, elapsed time: " << elapsed.count() << "s\n";
		
		// unordered lookup from new MLSymbols made from char * 
		start = std::chrono::system_clock::now();
		symbolSum = 0.f;
		idx = 0;
		for(int i=0; i<kTestLength; ++i)
		{
			if(++idx >= kTableSize) idx = 0;	
			symbolSum += testMapUnorderedSym[charDict[idx]];
		}
		end = std::chrono::system_clock::now();
		elapsed = end-start;
		std::cout << "constructing symbols, unordered, elapsed time: " << elapsed.count() << "s\n";
				
		REQUIRE(stringSum == symbolSum);
		
		REQUIRE(theSymbolTable().audit());
	}
}

TEST_CASE("madronalib/core/symbol/numbers", "[symbol]")
{
	textUtils::NameMaker namer;
	for(int i=0; i<10; ++i)
	{
		Symbol testSym = namer.nextName();
		Symbol testSymWithNum = textUtils::addFinalNumber(testSym, i);
		Symbol testSymWithoutNum = textUtils::stripFinalNumber(testSym);
		int j = textUtils::getFinalNumber(testSymWithNum);
		
		REQUIRE(testSym == testSymWithoutNum);
		REQUIRE(i == j);
	}
	REQUIRE(theSymbolTable().audit());
}

TEST_CASE("madronalib/core/symbol/identity", "[symbol][identity]")
{
	// things that should and shouldn't be the same as one another.
	theSymbolTable().clear();
	Symbol a("xxx_yyy");
	Symbol b("xxx");
	REQUIRE(a != b);
}

// hex char printing
struct HexCharStruct
{
	unsigned char c;
	HexCharStruct(unsigned char _c) : c(_c) { }
};

inline std::ostream& operator<<(std::ostream& o, const HexCharStruct& hs)
{
	return (o << std::hex << (int)hs.c << std::dec );
}

inline HexCharStruct hexchar(unsigned char _c)
{
	return HexCharStruct(_c);
}

TEST_CASE("madronalib/core/symbol/UTF8", "[symbol][UTF8]")
{
	theSymbolTable().clear();
	std::map<Symbol, int> sortedMap;
	const int sortTestSize = 10;
	int p = 0;

#if HAVE_U8_LITERALS
	std::vector< std::string > strings = { u8"Федор", u8"小林 尊", u8"محمد بن سعيد" };
#else
	const char* fedor("\xD0\xA4\xD0\xB5\xD0\xB4\xD0\xBE\xD1\x80");
	const char* kobayashi("\xE5\xB0\x8F\xE6\x9E\x97\x20\xE5\xB0\x8A");
	const char* muhammad("\xD9\x85\xD8\xAD\xD9\x85\xD8\xAF\x20\xD8\xA8\xD9\x86\x20\xD8\xB3\xD8\xB9\xD9\x8A\xD8\xAF");
	std::vector< std::string > strings = { fedor, kobayashi, muhammad };
#endif	
	
	int totalPoints = 0;
	for(auto testString : strings)
	{
		totalPoints += TextFragment(testString.c_str()).lengthInCodePoints();
	}
	REQUIRE(totalPoints == 21);
}

