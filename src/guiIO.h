#ifndef GUI_STR
#define GUI_STR

#include <windows.h>
#include <iostream.h>
#include "guiClass.h"

namespace N_Gui{

/*
 * GUI -> CASTER
 */
class ginClass{
public:
	ginClass();
	ginClass& operator>>(WORD&);
	ginClass& operator>>(char*);
	ginClass& getline(char*,int);
	void clear(){
		//TODO
	}
	void ignore(int n, char c){
		//TODO
		if(n){}
		if(c){}
	}
	bool fail(){
		//dummy‚Íí‚Éfalse
		//ƒ[ƒjƒ“ƒOÁ‹Ž‚Ì‚½‚ß
		return dummy;
	}
	
	int blockFlg;
	bool dummy;
	void enableBlock();
	void disableBlock();
};

/*
 * CASTER -> GUI
 *
 * Stream Buffer
 * Refs http://www.jah.ne.jp/~naoyuki/Writings/ExtIos.html
 */
template <class Ch,class Tr=std::char_traits<Ch> >
class basic_gout_streambuf : public std::basic_streambuf<Ch,Tr> {
public:
	basic_gout_streambuf(void)
	{
		setbuf(0,0);
	}
	~basic_gout_streambuf(void)
	{
	}

protected:
	//TODO
	//Do we need create Mutex?

	std::streampos seekoff( 
		std::streamoff off, 
		std::ios::seek_dir dir, 
		int nMode = std::ios::in | std::ios::out )
	{
		return EOF;
	}
	std::streampos seekpos( 
		std::streampos pos, 
		int nMode = std::ios::in | std::ios::out )
	{
		if(pos){}
		if(nMode){}
		return EOF;
	}

	int overflow(int);

	int underflow(void)
	{
		return EOF;
	}

private:
};

/*
 * CASTER -> GUI
 *
 * Stream
 * Refs http://www.jah.ne.jp/~naoyuki/Writings/ExtIos.html
 */
template <class Ch,class Tr=std::char_traits<Ch> >
class basic_gout_stream : public std::basic_iostream<Ch,Tr> {
public:
	basic_gout_stream(void) 
		: std::basic_iostream<Ch,Tr>(new basic_gout_streambuf<Ch,Tr>())
	{
	}

	~basic_gout_stream(void)
	{
	}

private:
};

/*
 * for easier use
 */
typedef basic_gout_streambuf<char> gout_streambuf;
typedef basic_gout_stream<char> goutStream;
extern ginClass gin;
extern goutStream gout;


}

#endif