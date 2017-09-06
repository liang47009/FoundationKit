//
//
//#include <codecvt>
//#include <string>
//#include <locale>
//class basic_encoding
//{
//public:
//
//    static std::string wstring2string(const std::wstring& val)
//    {
//        std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
//        return conv.to_bytes(val);
//    }
//
//    static std::wstring string2wstring(const std::string& val)
//    {
//        std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
//        return conv.from_bytes(val);
//    }
//
//    static std::u16string UTF8ToUTF16(const std::string &s)
//    {
//        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> conv;
//        return conv.from_bytes(s);
//    }
//
//    static std::u32string UTF8ToUTF32(const std::string &s)
//    {
//        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
//        return conv.from_bytes(s);
//    }
//
//    static std::string    UTF16ToUTF8(const std::u16string &s)
//    {
//        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> conv;
//        return conv.to_bytes(s);
//    }
//
//    static std::u32string UTF16ToUTF32(const std::u16string &s)
//    {
//        const char16_t *pData = s.c_str();
//        std::wstring_convert<std::codecvt_utf16<char32_t>, char32_t> conv;
//        return conv.from_bytes(reinterpret_cast<const char*>(pData), reinterpret_cast<const char*>(pData + s.length()));
//    }
//
//    static std::string    UTF32ToUTF8(const std::u32string &s)
//    {
//        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
//        return conv.to_bytes(s);
//    }
//
//    static std::u16string UTF32ToUTF16(const std::u32string &s)
//    {
//        std::wstring_convert<std::codecvt_utf16<char32_t>, char32_t> conv;
//        std::string bytes = conv.to_bytes(s);
//        return std::u16string(reinterpret_cast<const char16_t*>(bytes.c_str()), bytes.length() / sizeof(char16_t));
//    }
//
//
//
//};
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
