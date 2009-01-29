// StreamProvider.cpp:  ActionScript file: or http: stream reader, for Gnash.
// 
//   Copyright (C) 2005, 2006, 2007, 2008 Free Software Foundation, Inc.
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#ifdef HAVE_CONFIG_H
#include "gnashconfig.h"
#endif


#include "GnashFileUtilities.h"
#include "StreamProvider.h"
#include "URL.h"
#include "tu_file.h"
#include "NetworkAdapter.h"
#include "URLAccessManager.h"
#include "log.h"
#include "rc.h" // for rcfile

#include <cstdio>
#include <map>
#include <string>
#include <vector>
#include <boost/algorithm/string/replace.hpp>

namespace gnash {

namespace {
    
    std::string urlToDirectory(const std::string& path);
    
    /// Make a unique cachefile name from the supplied name.
    /// If all possible filenames are taken, return an empty string.
    std::string incrementalRename(const URL& url);
    
    /// Make a non-unique cachefile name from the supplied name.
    /// If the directory cannot be created, return an empty string.
    std::string overwriteExisting(const URL& url);
}

StreamProvider&
StreamProvider::getDefaultInstance()
{
	static StreamProvider inst;
	return inst;
}

StreamProvider::NamingPolicy
StreamProvider::currentNamingPolicy() const
{
    //return overwriteExisting;
    return incrementalRename;
}

std::auto_ptr<IOChannel>
StreamProvider::getStream(const URL& url, NamingPolicy np)
{

    std::auto_ptr<IOChannel> stream;

	if (url.protocol() == "file")
	{
		std::string path = url.path();
		if ( path == "-" )
		{
            // TODO: only allow this as the *very first* call ?
            //       Rationale is a movie might request load of
            //       standard input, being a security issue.
            //       Note also that the FB gui will use stdin
            //       for key events.
            //
			FILE *newin = fdopen(dup(0), "rb");

			// Close on destruction.
			stream.reset(new tu_file(newin, true));
			return stream;
		}
		else
		{
            // check security here !!
		    if (!URLAccessManager::allow(url)) return stream;

			FILE *newin = std::fopen(path.c_str(), "rb");
			if (!newin)  { 
				return stream;
			}
			// Close on destruction
			stream.reset(new tu_file(newin, true));
			return stream;
		}
	}
	else
	{
		if (URLAccessManager::allow(url)) {
			stream = NetworkAdapter::makeStream(url.str(),
                    np ?  np(url) : std::string());
		}

        // Will return 0 auto_ptr if not allowed.
		return stream;
	}
}

std::auto_ptr<IOChannel>
StreamProvider::getStream(const URL& url, const std::string& postdata,
        const NetworkAdapter::RequestHeaders& headers, NamingPolicy np)
{

    if (url.protocol() == "file")
    {
        if (!headers.empty())
        {
            log_error("Request Headers discarded while getting stream "
                    "from file: uri");
        }
        return getStream(url, postdata);
    }

	if ( URLAccessManager::allow(url) ) {
		return NetworkAdapter::makeStream(url.str(), postdata, headers,
                np ? np(url) : std::string());
	}

	return std::auto_ptr<IOChannel>();

}

std::auto_ptr<IOChannel>
StreamProvider::getStream(const URL& url, const std::string& postdata,
       NamingPolicy np)
{

    std::auto_ptr<IOChannel> stream;

	if (url.protocol() == "file")
	{
        if (!postdata.empty())
        {    
		    log_error(_("POST data discarded while getting a stream "
                        "from file: uri"));
        }
		std::string path = url.path();
		if ( path == "-" )
		{
			FILE *newin = fdopen(dup(0), "rb");
			stream.reset(new tu_file(newin, false));
			return stream;
		}
		else
		{
			if ( ! URLAccessManager::allow(url) ) return stream;

			FILE *newin = std::fopen(path.c_str(), "rb");
			if (!newin)  { 
				return stream;
			}
			stream.reset(new tu_file(newin, false));
			return stream;
		}
	}
	else
	{
		if (URLAccessManager::allow(url)) {
			stream = NetworkAdapter::makeStream(url.str(), postdata,
                    np ? np(url) : std::string());
		}
        // Will return 0 auto_ptr if not allowed.
		return stream;		

	}
}


namespace {

/// Transform a URL into a directory and create it.
//
/// @return     an empty string if the directory cannot be created, otherwise
///             the name of the created directory with a trailing slash.
/// @param url  The path to transform. Anything after the last '/' is ignored.
std::string
urlToDirectory(const std::string& path)
{

    const RcInitFile& rcfile = RcInitFile::getDefaultInstance();
    const std::string& dir = rcfile.getMediaDir() + "/" + path;
 
    // Create the user-specified directory if possible.
    // An alternative would be to use the 'host' part and create a 
    // directory tree.
    if (!mkdirRecursive(dir)) {
        // Error
        return std::string();
    }

    return dir;

}

std::string
overwriteExisting(const URL& url)
{
    std::string path = url.path().substr(1);
    
    // Replace all slashes with a _ for a flat directory structure.
    boost::replace_all(path, "/", "_");

    const std::string& dir = urlToDirectory(url.hostname() + "/");

    if (dir.empty()) return std::string();

    return dir + path;
}

std::string
incrementalRename(const URL& url)
{

    const std::string& path = url.path();
    assert(!path.empty());
    assert(path[0] == '/');
    
    // Find the last dot, but not if it's first in the path (after the
    // initial '/').
    std::string::size_type dot = path.rfind('.');
    if (dot == 1) dot = std::string::npos;

    // Take the path from after the initial '/' to the last '.' for
    // manipulation. It doesn't matter if dot is npos.
    std::string pre = path.substr(1, dot - 1);

    // Replace all slashes with a _ for a flat directory structure.
    boost::replace_all(pre, "/", "_");

    const std::string& suffix = (dot == std::string::npos) ? "" : 
        path.substr(dot);

    // Add a trailing slash.
    const std::string& dir = urlToDirectory(url.hostname() + "/");
    if (dir.empty()) return std::string();

    std::ostringstream s(dir + pre + suffix);

    size_t i = 0;

    const size_t m = std::numeric_limits<size_t>::max();

    struct stat st;
    while (stat(s.str().c_str(), &st) >= 0 && i < m) {
        s.str("");
        s << dir << pre << i << suffix;
        ++i;
    }

    // If there are no options left, return an empty string.
    if (i == m) {
        return std::string();
    }

    return s.str();

}

} // anonymous namespace
} // namespace gnash

