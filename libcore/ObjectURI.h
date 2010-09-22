
#ifndef GNASH_OBJECTURI_H
#define GNASH_OBJECTURI_H

#include "string_table.h"
#include <string>
#include <ostream>
#include <sstream>

#define GNASH_DEBUG_OBJECT_URI_NOCASE 1

#ifdef GNASH_DEBUG_OBJECT_URI_NOCASE
# include <iostream>
#endif

namespace gnash {

/// A URI for describing as_objects.
//
/// This is used as a unique identifier for any object member, especially
/// prototypes, class, constructors.
struct ObjectURI
{

    class Logger;

    /// Default constructor, no name, no caseless name
    ObjectURI()
        :
        name(0),
        nameNoCase(0)
    {}

    /// Construct an ObjectURI from name and namespace.
    ObjectURI(string_table::key name)
        :
        name(name),
        nameNoCase(0)
    {}

/*
    operator const void*() const {
        return (name == 0) ? 0 : this;
    }
*/

    bool empty() const {
        return (name == 0);
    }

    const std::string&
    toString(string_table& st) const
    {
        return st.value(name);
    }


#ifdef GNASH_DEBUG_OBJECT_URI_NOCASE
    struct Counter {
        Counter(): skips(0), dos(0) {}
        int skips;
        int dos;
        void skip() { ++skips; }
        void doit() { ++dos; }
        ~Counter () {
            std::cerr << "Skipped " << skips << "/" << (skips+dos)
                << " (" << (double(skips)/double(skips+dos))
                << ") calls to noCase "<< std::endl;
        }
    };
#endif

    string_table::key noCase(string_table& st) const {
#ifdef GNASH_DEBUG_OBJECT_URI_NOCASE
        static Counter stat;
#endif
        if ( ! nameNoCase ) {
            nameNoCase = st.noCase(name);
#ifdef GNASH_DEBUG_OBJECT_URI_NOCASE
            stat.doit();
#endif
        }
#ifdef GNASH_DEBUG_OBJECT_URI_NOCASE
        else stat.skip();
#endif
        return nameNoCase;
    }

    string_table::key name;

    mutable string_table::key nameNoCase;
};

inline bool
equalsNoCase(string_table& st, const ObjectURI& a, const ObjectURI& b)
{
    return a.noCase(st) == b.noCase(st);
}

/// ObjectURIs are equal if name is equal
inline bool
operator==(const ObjectURI& a, const ObjectURI& b)
{
    return a.name == b.name;
}

inline bool
equals(string_table& st, const ObjectURI& a, const ObjectURI& b, bool caseless)
{
    if ( caseless ) return equalsNoCase(st, a, b);
    else return a == b;
}

/// Comparator for ObjectURI so it can serve as a key in stdlib containers.
inline bool
operator<(const ObjectURI& a, const ObjectURI& b)
{
    return a.name < b.name;
}

/// Get the name element of an ObjectURI
inline string_table::key
getName(const ObjectURI& o)
{
    return o.name;
}

class ObjectURI::Logger
{
public:
    Logger(string_table& st) : _st(st) {}

    std::string operator()(const ObjectURI& uri) const {
        const string_table::key name = getName(uri);
        return _st.value(name);
    }

    std::string debug(const ObjectURI& uri) const {
        std::stringstream ss;
        const string_table::key name = getName(uri);
        const string_table::key nameNoCase = uri.noCase(_st);
        ss << _st.value(name)
           << "(" << name << ")/"
           << _st.value(nameNoCase)
           << "(" << nameNoCase << ")";
        return ss.str();
    }

private:
    string_table& _st;
};

} // namespace gnash
#endif
