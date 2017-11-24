/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_STRING_BUILDER_HPP
#define FOUNDATIONKIT_STRING_BUILDER_HPP
#include "FoundationKit/GenericPlatformMacros.hpp"
#include <string>
#include <list>
#include <numeric>
NS_FK_BEGIN

// Subset of http://msdn.microsoft.com/en-us/library/system.text.stringbuilder.aspx
template <typename _Elem>
class string_builder 
{
    typedef std::basic_string<_Elem> string_t;
 	typedef std::list<string_t> container_t; // Reasons not to use vector below. 
    //typedef std::deque<string_t> container_t; // Reasons not to use vector below. 
	typedef typename string_t::size_type size_type; // Reuse the size type in the string.
	container_t m_Data;
	size_type   m_totalSize;
	void _append(const string_t &src) 
    {
		m_Data.push_back(src);
		m_totalSize += src.size();
	}
	// No copy constructor, no assignement.
    string_builder(const string_builder &);
    string_builder & operator = (const string_builder &);
public:
    string_builder(const string_t &src)
    {
		if (!src.empty()) {
			m_Data.push_back(src);
		}
		m_totalSize = src.size();
	}
    string_builder()
    {
		m_totalSize = 0;
	}

    void clear()
    {
        m_Data.clear();
        m_totalSize = 0;
    }

    string_builder & append(const string_t &src)
    {
        _append(src);
		return *this; // allow chaining.
	}

    // This one lets you add any STL container to the string builder. 
    template<class _Iter>
    string_builder & append(const _Iter &first, const _Iter &afterLast)
    {
        for (_Iter f = first; f != afterLast; ++f)
        {
            _append(*f);
		}
		return *this; // allow chaining.
	}

   /** 
    * Append a string and '\n' to string_builder.
    */
    string_builder & append_line(const string_t &src)
    {
        static _Elem lineFeed[] { 10, 0 }; // C++ 11. Feel the love!
		m_Data.push_back(src + lineFeed);
		m_totalSize += 1 + src.size();
		return *this; // allow chaining.
	}

   /**
    * Append a '\n' to string_builder.
    */
    string_builder & append_line()
    {
        static _Elem lineFeed[] { 10, 0 };
		m_Data.push_back(lineFeed);
		++m_totalSize;
		return *this; // allow chaining.
	}

	string_t to_string() const 
    {
		string_t result;
		result.reserve(m_totalSize + 1);
		for (auto iter = m_Data.begin(); iter != m_Data.end(); ++iter) 
        { 
			result += *iter;
		}
		return result;
	}


	string_t join(const string_t &delim) const
    {
		if (delim.empty())
        {
            return to_string();
		}
		string_t result;
		if (m_Data.empty())
        {
			return result;
		}
		// Hope we don't overflow the size type.
		size_type st = (delim.size() * (m_Data.size() - 1)) + m_totalSize + 1;
		result.reserve(st);
        // If you need reasons to love C++11, here is one.
		struct adder
        {
			string_t m_Joiner;
			adder(const string_t &s): m_Joiner(s) 
            {
				// This constructor is NOT empty.
			}
                        // This functor runs under accumulate() without reallocations, if 'l' has reserved enough memory. 
			string_t operator()(string_t &l, const string_t &r) 
            {
				l += m_Joiner;
				l += r;
				return l;
			}
		} adr(delim);

		auto iter = m_Data.begin(); 
                // Skip the delimiter before the first element in the container.
		result += *iter; 
		return std::accumulate(++iter, m_Data.end(), result, adr);
	}

}; // class string_builder

NS_FK_END

#endif // FOUNDATIONKIT_STRING_BUILDER_HPP


