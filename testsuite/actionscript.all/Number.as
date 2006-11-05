// 
//   Copyright (C) 2005, 2006 Free Software Foundation, Inc.
//
// This program is free software; you can redistribute it and/or modchecky
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
//
// along with this program; check not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fcheckth Floor, Boston, MA  02110-1301  USA
// Test case for Number ActionScript class
// compile this test case with Ming makeswf, and then
// execute it like this gnash -1 -r 0 -v out.swf

//
// TODO: have ming output ACTION_EQUAL
//       it seems it will only do it for SWF4 output
//
// TODO: test with SWF target != 6 (the only one tested so far)
//	

rcsid="$Id: Number.as,v 1.8 2006/11/05 00:45:27 rsavoye Exp $";

#include "check.as"

var n1=new Number(268);

// strict-equality operator was introduced in SWF6
#if OUTPUT_VERSION > 5
check ( ! (n1 === 268) );
// They are not the same object !
check ( ! (n1 === Number(268)) );
#endif

// but they have the same numeric value
check_equals (n1 , 268 );
check_equals (268 , n1 );

