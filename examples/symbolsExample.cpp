//
//  symbolsExample.cpp
//  madronalib
//
//  Created by Randy Jones on 5/17/16.
//
//

#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <chrono>
#include <thread>

#include "../include/madronalib.h"
#include "MLTextUtils.h"

#include <iostream>

#include "MLText.h"
#include "MLPath.h"

using namespace ml;
using namespace ml::textUtils;


#define DUMP(s, qq, buf, sz)  {printf(s);                   \
for (qq = 0; qq < (sz);qq++)    \
printf("%02x ", buf[qq]); \
printf("\n");}




// This example class shows how to write a method like setParam() that can accept string literals
// as parameters so that the symbols are hashed at compile time. 
class TestProc
{
public:
	
	TestProc(){}
	~TestProc(){}
	
	// NOTE: template syntax here is needed to get the string literal length N at compile time.
	template<size_t N>
	inline void setParam(const char(&name)[N], float val)
	{
		std::cout << "setParam - HSL\n";
		HashedCharArray hsl(name);
		Symbol m(hsl);
		map[m] = val;
	}
	
	inline void setParam(const Symbol name, float val)
	{
		std::cout << "setParam - Symbol\n";
		map[name] = val;
	}
	
	template<size_t N>
	inline float getParam(const char(&name)[N])
	{
		std::cout << "getParam - HSL\n";
		HashedCharArray hsl(name);
		Symbol m(hsl);
		return map[m];
	}
	
	inline float getParam(const Symbol name)
	{
		std::cout << "getParam - Symbol\n";
		return map[name];
	}
	
	std::map< Symbol, float > map;
	
};


const char* b = "fff";
const char* c = "fff";
const char* f() { return b; }

int main()
{
	{
		TextFragment first("test");
		std::cout << first << "\n";
	}
	
	
	{
		Symbol first("first");
	}
	
	std::cout << "hi\n";
	
	{
		Symbol second("second");
		Symbol third("third");
		
		Symbol sum = second + third;
		std::cout << sum << "\n";
	}
	
	{
		const char * letters("abcd");
		
		uint32_t hashTest = krHash0(letters, strlen(letters));	
		
		std::cout << std::hex << hashTest << std::dec << "\n";
		
		std::cout << "Symbols example:\n";
		
		std::cout << "\n\nTESTING sym param name ";
		
		TestProc p;
		
		p.setParam("9xo", 880.f);
		p.setParam("frequencies", 440.f);
		p.setParam("frequency", 440.f);
		p.setParam("fxo", 110.f);
		p.setParam("fyo", 909);
		
		char name1[4] = "foo";
		name1[1] = 'x';

		const char name2[4] = "fyo";
		
		// Foo::Foo(const char (&)[N]) [N = 2]
		std::cout << "frequency " << p.getParam("frequency") << "\n";
		
		// Foo::Foo(T, typename IsCharPtr<T>::Type) [T = char *]
		std::cout << "name1 (not const) " << p.getParam(name1) << "\n";
		
		// Foo::Foo(const char (&)[N]) [N = 2]
		std::cout << "name2 (const) " << p.getParam(name2) << "\n";
		
		// Foo::Foo(T, typename IsCharPtr<T>::Type) [T = const char *]
		std::cout << "ff " << p.getParam(b) << "\n";
			
		theSymbolTable().dump();
		
		TextFragment test1 ("hello.exe");
		std::cout << "findLast loc: " << textUtils::findLast(test1, 'l') << "\n"; // 3
		std::cout << "findLast loc: " << textUtils::findLast(test1, [](utf::codepoint_type c){ return c == 'o'; }) << "\n"; // 4

	#if HAVE_U8_LITERALS
		TextFragment kobayashi(u8"小林 尊");
	#else
		TextFragment kobayashi("\xE5\xB0\x8F\xE6\x9E\x97\x20\xE5\xB0\x8A");
	#endif	
		
		// iterate a UTF-8 text as code points
		auto first = utf::codepoint_iterator<const char*>(kobayashi.getText());
		auto last = utf::codepoint_iterator<const char*>(kobayashi.getText() + kobayashi.lengthInBytes());
		for (auto it = first; it != last; ++it) 
		{
			std::cout << std::hex << *it << " "; // code points: 5c0f 6797 20 5c0a 
		}	
		std::cout << "\n";
		
		// find a code point in a UTF-8 text
		utf::codepoint_type hayashi[1]{0x6797};
		std::cout << "hayashi loc: " << textUtils::findFirst(kobayashi, hayashi[0]) << "\n"; // 1
		
		// UTF-8 encode a single codepoint to preallocated buffer
		const int kBufSize = 4;
		unsigned char buf[kBufSize];
		auto hv = utf::make_stringview(hayashi);
		unsigned char* pb = buf;
		for (utf::codepoint_iterator<const char32_t *> it = hv.begin(); it != hv.end(); ++it) 
		{
			pb = utf::internal::utf_traits<utf::utf8>::encode(*it, pb);
			if((pb - buf) >= kBufSize) break;
		}
		std::cout << buf << "\n"; // 林
		
		TextFragment hello1("hi, how are you?");
		std::cout << hello1 << " [" << textUtils::subText(hello1, 4, 7) << "] \n"; // hi, how are you? [how] 
		std::cout << textUtils::stripFileExtension("example.txt") << "\n"; // example
		std::cout << textUtils::getShortFileName("golly/gee/whiz.txt") << "\n"; // whiz.txt
		std::cout << textUtils::getPath("golly/locks/file.txt") << "\n"; // golly/locks
		
		TextFragment space("林");
		TextFragment hello2("good?");
		TextFragment hello3(space, hello2);
		std::cout << "\n\n";
		
		std::cout << hello1 << "\n";
		std::cout << hello2 << "\n";
		std::cout << hello3 << "\n";
		
		
		std::cout << "*" << TextFragment("hello").beginsWith("hell") << "*" << "\n";
		std::cout << "*" << TextFragment("hello").beginsWith("hh") << "*" << "\n";
		std::cout << "*" << TextFragment("hello").endsWith("lo") << "*" << "\n";
		std::cout << "*" << TextFragment("hello").endsWith("lxo") << "*" << "\n";
		
		std::cout << "*" << textUtils::stripFinalNumber("xx林2") << "*" << "\n";
		
		std::cout << "*" << textUtils::addFinalNumber("林asd1", 23) << "*" << "\n";
		
		std::cout << "*" << std::dec << textUtils::getFinalNumber("林a1sd531x") << "*" << "\n";
		
	//	theTextFragmentPool().dump();
		
		ml::Path newPath("hello/小林/it's/nice/in/the/café/here");
		std::cout << "path: " << newPath << " (" << sizeof(newPath) << " bytes) \n";
	}
	
	{
		TextFragment t ("Hello, I'm Rags. ");
		TextFragment u (t, ("This "), ("林 "));
		TextFragment v (u, ("is "), ("nice! "));
		std::cout << "new text: " << v << "\n";		
		std::cout << TextFragment(v, "Hello, world!") << "\n";
		TextFragment w (v, "Hello, universe!");
		std::cout << "$" << std::string(w.getText()) << "$\n";
		std::cout << w.lengthInCodePoints() << " code points, " << w.lengthInBytes() << " bytes.\n";
		
		std::cout << "code points: \n";
		for (const utf::codepoint_type c : w) 
		{
			std::cout << c << " ";
		}
		std::cout << "\n";
		
	}
	
	{
		ml::RandomSource r;
		int len = fabs(r.getSample()*29.);
		std::cout << len << "\n";
		
		char test1[100];
		std::cout << std::hex << (unsigned long *)test1 << std::dec << "\n";
		char test2[100];
		std::cout << std::hex << (unsigned long *)test2 << std::dec << "\n";
		
		std::vector< ml::Symbol > tv1;
		std::vector< ml::Symbol > tv2;
		tv1.emplace_back("hello");
		tv1.emplace_back("again");
		std::cout << std::hex << (unsigned long *)(&tv1) << std::dec << " (" << sizeof(tv1) << " bytes) \n";
		std::cout << std::hex << (unsigned long *)(&tv2) << std::dec << " (" << sizeof(tv2) << " bytes) \n";
		std::cout << std::hex << (unsigned long *)(&tv1[0]) << std::dec << "\n";
		std::cout << std::hex << (unsigned long *)(&tv1[1]) << std::dec << "\n";
	}
	{
		Symbol p("please");
		std::cout << p << " " << ml::textUtils::stripFinalCharacter(p) << "\n";
	}
	
	{
		// stripping and base64
		TextFragment input("  \n \n\nabcdEFGHijklMNOP  \t\t ");
		TextFragment stripped(ml::textUtils::stripWhitespace(input));
		std::cout << "stripped: *" << stripped << "*\n";
		std::vector<uint8_t> decoded = ml::textUtils::base64Decode(stripped);
		std::cout << "decoded: ";
		for(uint8_t c : decoded)
		{
			std::cout << std::hex << (int)c << " ";
		}
		std::cout << "\n";
		ml::TextFragment encoded = ml::textUtils::base64Encode(decoded);
		std::cout << "re-encoded: *" << encoded << "*\n";
	}	

	{
		ml::TextFragment testFrag1("hello/小林/it's/nice/in/the/café/here");
		std::cout << byteVectorToText(textToByteVector(testFrag1)) << "\n";

		// AES
		ml::TextFragment plaintextFrag("It was the best of times, it was the würst of times.");
		std::vector<uint8_t> plaintext(ml::textUtils::textToByteVector(plaintextFrag));
		
		std::vector<uint8_t> key(32, 'a');
		std::vector<uint8_t> iv(32, 'a');
		
		std::vector<uint8_t> ciphertext = ml::textUtils::AES256ECBEncode(plaintext, key);
		
		std::vector<uint8_t> decodedtext = ml::textUtils::AES256CBCDecode(ciphertext, key, iv);
		
		ml::TextFragment decodedFrag(ml::textUtils::byteVectorToText(decodedtext));

		std::cout << "decoded: " << decodedFrag << "\n";
	}

	{
		const char* keyStr("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
//		std::vector<uint8_t> key(keyStr, keyStr + 32);
		std::vector<uint8_t> key(32, 'a');
		ml::TextFragment keyFrag(ml::textUtils::byteVectorToText(key));
		
		///= {"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"};
		
		TextFragment helloFrag("abcdabcdabcdabc");
		//std::vector<uint8_t> plaintext(ml::textUtils::textToByteVector(helloFrag));
		
		std::vector<uint8_t> plaintext(16, 0);
				
		
		
		/*
		
		std::cout << "plain: ";
		for(uint8_t byte : plaintext)
		{
			std::cout << std::hex << (int)byte << " " << std::dec ;
		}
		std::cout << "\n";
		std::cout << "crypt: ";
		for(uint8_t byte : encrypted)
		{
			std::cout << std::hex << (int)byte << " " << std::dec ;
		}
		std::cout << "\n";
		
		std::vector<uint8_t> xx(ml::textUtils::base64Decode("YWJjZGVmZ2g="));
		ml::TextFragment decodedFrag(ml::textUtils::byteVectorToText(xx));
		std::cout << "decoded: " << decodedFrag << "\n";
		
			
		TextFragment licenseFrag ("ejFfaLrg+8fLWUO5vpaV2YpWJ9TYJr5s/pKi3aKYsks=");
		std::vector<uint8_t> license = ml::textUtils::base64Decode(licenseFrag);
		std::vector<uint8_t> decodedtext = ml::textUtils::AES256ECBDecode(license, key);

		ml::TextFragment decodedFrag(ml::textUtils::byteVectorToText(decodedtext));
		std::cout << "decoded: " << decodedFrag << "\n";
		 */
		
		std::cout << "\n\n\n\n\n";
		{

			TextFragment encoded = "JwB0eXieG2RPATn7EHe4HpdRIUX958bEyTVQFXsnEwY=";			
			
			std::cout << "encoded: " << encoded << "\n";
			
			TextFragment stripped(ml::textUtils::stripWhitespace(encoded));
			std::cout << "stripped: *" << stripped << "*\n";
			
			std::vector<uint8_t> cipher = ml::textUtils::base64Decode(stripped);
			
			std::vector<uint8_t> key(32, 'a');
			std::vector<uint8_t> iv(32, 'a');
			
			std::vector<uint8_t> decodedtext = ml::textUtils::AES256CBCDecode(cipher, key, iv);

						
			// remove PKCS padding
	//		int padBytes = cipher[cipher.size() - 1];
	//		cipher.resize(cipher.size() - padBytes);

			TextFragment out = byteVectorToText(decodedtext);
			std::cout << out << "\n";
			
			
			return 0;
		} /* main */
		
	}
	
//	theSymbolTable().dump();

	return 0;
}


