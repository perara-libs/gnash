// 
//   Copyright (C) 2005, 2006 Free Software Foundation, Inc.
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "check.h"
#include "URL.h"
#include <iostream>
#include <sstream>
#include <cassert>

using namespace std;
using namespace gnash;

int
main(int /*argc*/, char** /*argv*/)
{

	std::string label;


	/// Test absolute filename
	URL u("/etc/hosts");
	label = "abs fname";
	check_equals_label(label, u.protocol(), "file");
	check_equals_label(label, u.hostname(), "");
	check_equals_label(label, u.path(), "/etc/hosts");
	check_equals_label(label, u.str(), "file:///etc/hosts");

	/// Test relative filename
	URL u1("passwd", u);
	label = "rel fname";
	check_equals_label(label, u1.protocol(), "file" );
	check_equals_label(label, u1.hostname(), "" );
	check_equals_label(label, u1.path(), "/etc/passwd" );
	check_equals_label(label, u1.str(), "file:///etc/passwd" );

	/// Test proto-host relative filename
	URL u2("/", u);
	label = "rel path";
	check_equals_label (label, u2.protocol() , "file" );
	check_equals_label (label, u2.hostname() , "" );
	check_equals_label (label, u2.path() , "/" );
	check_equals_label (label, u2.str() , "file:///" );

	/// Test https url 
	URL u3("https://www.fake.it/path.swf");

	label = "https url";
	check_equals_label(label, u3.protocol(), "https");
	check_equals_label(label, u3.hostname(), "www.fake.it");
	check_equals_label(label, u3.path(), "/path.swf");
	check_equals_label(label, u3.str(), "https://www.fake.it/path.swf");

	/// Test http url with root path
	URL u4("http://www.fake.it/");
	label = "http root path";
	check_equals_label (label, u4.protocol() , "http" );
	check_equals_label (label, u4.hostname() , "www.fake.it" );
	check_equals_label (label, u4.path() , "/" );
	check_equals_label (label, u4.str() , "http://www.fake.it/" );

	/// Test path-absolute proto-host-relative http url 
	URL u5("/index.html", u4);
	label = "rel root path";
	check_equals_label (label, u5.protocol(), "http" );
	check_equals_label (label, u5.hostname(), "www.fake.it" );
	check_equals_label (label, u5.path(), "/index.html" );
	check_equals_label (label, u5.str(), "http://www.fake.it/index.html");

	/// Test back-seek path
	URL u6("/usr/local/include/curl.h");
	check_equals(u6.protocol() , "file" );
	check_equals( u6.path() , "/usr/local/include/curl.h" );

	URL u7("../../include/curl.h", u6);
	check_equals( u7.protocol() , "file" );
	check_equals( u7.path() , "/usr/include/curl.h" );

	URL u8("../..//../../../../tmp//curl.h", u6);
	check_equals ( u8.protocol() , "file" );
	check_equals ( u8.path() , "/tmp/curl.h" );
	check_equals ( "/tmp/curl.h", "/tmp/curl.h" );

	/// Test path normalization 
	check_equals (URL("/hello/world/../file").path(), "/hello/file");
	check_equals (URL("/dir/./file").path(), "/dir/file");
	check_equals (URL("/dir/./1/2/3/../../../...file").path(), "/dir/...file");

	/// Test url with anchors
	URL u9("/the/path#the_anchor");
	check_equals (u9.path(), "/the/path");
	check_equals (u9.anchor(), "the_anchor");
	check_equals (u9.str(), "file:///the/path#the_anchor");
	URL u10("http://host/the/path#the_anchor");
	check_equals (u10.hostname(), "host");
	check_equals (u10.path(), "/the/path");
	check_equals (u10.anchor(), "the_anchor");
	check_equals (u10.str(), "http://host/the/path#the_anchor");
	URL u11("#another_anchor", u10);
	check_equals (u11.hostname(), "host");
	check_equals (u11.path(), "/the/path");
	check_equals (u11.anchor(), "another_anchor");
	check_equals (u11.str(), "http://host/the/path#another_anchor");
	URL u12("#", u10);
	check_equals (u12.hostname(), "host");
	check_equals (u12.path(), "/the/path");
	check_equals (u12.anchor(), "");
	check_equals (u12.str(), "http://host/the/path");

	/// Test url with QUERY STRING
	URL u13("http://localhost/?M=A");
	check_equals (u13.hostname(), "localhost");
	check_equals (u13.path(), "/");
	check_equals (u13.protocol(), "http");
	check_equals (u13.anchor(), "");
	check_equals (u13.querystring(), "M=A");
	check_equals (u13.str(), "http://localhost/?M=A");
	URL u14("/?M=A&C=D");
	check_equals (u14.querystring(), "M=A&C=D");
	check_equals (u14.str(), "file:///?M=A&C=D");
	URL u15("/?M=A&C=D#anchor");
	check_equals (u15.querystring(), "M=A&C=D");
	check_equals (u15.anchor(), "anchor");
	check_equals (u15.str(), "file:///?M=A&C=D#anchor");
	URL u16("/my/path/?option1=23&option2=65#anchor");
	check_equals (u16.querystring(), "option1=23&option2=65");
	check_equals (u16.anchor(), "anchor");
	check_equals (u16.str(), "file:///my/path/?option1=23&option2=65#anchor");
	
	URL u17("/test?.swf");
	check_equals (u17.protocol() , "file" );
	check_equals (u17.hostname() , "" );
	check_equals (u17.path() , "/test?.swf" );
	check_equals (u17.str() , "file:///test?.swf" );
	check_equals (u17.querystring() , "" );


	// Test query_string parsing
	map<string, string> qs;
	URL::parse_querystring(u13.querystring(), qs);
	check_equals (qs["M"], "A");
	check_equals (qs["C"], "");
	URL::parse_querystring(u14.querystring(), qs);
	check_equals (qs["M"], "A");
	check_equals (qs["C"], "D");
	URL::parse_querystring(u16.querystring(), qs);
	check_equals (qs["option1"], "23");
	check_equals (qs["option2"], "65");


	// TODO: Samba paths
}

