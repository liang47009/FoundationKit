#include "FoundationKit/Base/Buffer.h"

USING_NS_FK;

static void BufferTest()
{
    // ========= Test Constructor ========= 
    Buffer<char>  BufferChar;
    Buffer<char>  BufferChar1(64);
    Buffer<char>  BufferChar2(std::move(BufferChar1));
    Buffer<char>  BufferChar3(BufferChar2);
    Buffer<char>  BufferChar4("This is three constructor.", 26);
    Buffer<char>  BufferChar5(8, 'A');

    Buffer<char>  BufferChar6 = BufferChar4.append('A');
    Buffer<char>  BufferChar7 = BufferChar4.append(3, 'S');
    Buffer<char>  BufferChar8 = BufferChar4.append("-Lose", 5);
    Buffer<char>  BufferChar9 = BufferChar4.append(BufferChar5);

    Buffer<char>  BufferChar10 = BufferChar4.assign(5, 'L');
    Buffer<char>  BufferChar11 = BufferChar4.assign("Libo", 4);
    Buffer<char>  BufferChar12 = BufferChar4.assign(BufferChar4);
    Buffer<char>  BufferChar13 = BufferChar4.assign(std::move(BufferChar4));

    Buffer<int>   BufferInt0();
    Buffer<int>   BufferInt1(2);
    BufferInt1.append(10);
    BufferInt1.append(100);
    
    int a = BufferInt1.read();
    int b = BufferInt1.read();

    Buffer<int> BufferInt2 = BufferInt1;

    bool isSame = (BufferInt2 == BufferInt1);

    int c = BufferInt1[0];
    int d = BufferInt1[1];
 


}