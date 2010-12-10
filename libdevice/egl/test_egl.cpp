// 
//   Copyright (C) 2010 Free Software Foundation, Inc
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

#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include <map>
#include <cassert>
#include <regex.h>
#include <boost/assign/list_of.hpp>

#ifdef HAVE_EGL_EGL_H
# include <EGL/egl.h>
#else
# error "This file needs EGL, which is part of OpenGL-ES"
#endif

#include "log.h"
#include "dejagnu.h"
#include "eglDevice.h"
#include "GnashDevice.h"

TestState runtest;

using namespace gnash;
using namespace std;
using namespace renderer;

void test_egl(EGLDevice &egl, GnashDevice::rtype_t rtype, int argc, char *argv[]);

// The debug log used by all the gnash libraries.
static LogFile& dbglogfile = LogFile::getDefaultInstance();

const char *estrs[] = {
    "OpenVG",
    "OpenGLES1",
    "OpenGLES2",
    "X11",
    "VAAPI"
};
    
int
main(int argc, char *argv[])
{
    // FIXME: for now, always run verbose till this supports command line args
    dbglogfile.setVerbosity();
    
    EGLDevice egl1, egl2, egl3;

#ifdef RENDERER_OPENVG
    std::cerr << "== OpenVG tests ==" << std::endl;
    test_egl(egl1, GnashDevice::OPENVG, argc, argv);
    egl1.printEGLConfig();
    egl1.printEGLContext();
    egl1.printEGLSurface();
#endif
    
#ifdef RENDERER_GLES1
    std::cerr << "== OpenGLES1 tests ==" << std::endl;
    test_egl(egl2, GnashDevice::OPENGLES1, argc, argv);
//    egl2.printEGLConfig();
#endif
    
#ifdef RENDERER_GLES2
    std::cerr << "== OpenGLES2 tests ==" << std::endl;
    test_egl(egl3, GnashDevice::OPENGLES2, argc, argv);
//    egl3.printEGLConfig();
#endif
}

void
test_egl(EGLDevice &egl, GnashDevice::rtype_t rtype, int argc, char *argv[])
{
    bool hwinit = false;

    // This is a utility method for converting integer error codes to
    // something human readable for debugging.
    string result = "EGL_BAD_CONFIG";
    if (egl.getErrorString(EGL_BAD_CONFIG) == result) {
        runtest.pass("EGLDevice::getErrorString()");
    } else {
        runtest.fail("EGLDevice::getErrorString()");
    }

    if (egl.initDevice(argc, argv)) {
        runtest.pass("EGLDevice::initDevice()");
        hwinit = true;
    } else {
        runtest.fail("EGLDevice::initDevice()");
    }

    if (hwinit) {
        if (egl.bindClient(rtype)) {
            runtest.pass("EGLDevice::bindClient()");
        } else {
            runtest.fail("EGLDevice::bindClient()");
        }
    } else {
        runtest.untested("EGLDevice::bindClient()");
    }

//    egl.printEGLConfig();
    
    // If there are more than zero configurations, something beyond
    // initializing is working
    if (hwinit) {
        if (egl.queryEGLConfig()) {
            runtest.pass("EGLDevice::queryEGLConfig()");
        } else {
            runtest.fail("EGLDevice::queryEGLConfig()");
        }
    } else {
        runtest.untested("EGLDevice::queryEGLConfig()");
    }

#if 0
    // Init'ing to zero uses the root screen as the display. Otherwise
    // the argument should be an EGLNativeWindowType.
    if (egl.attachWindow(0)) {
        runtest.pass("EGLDevice::attachWindow(0)");
    } else {
        runtest.fail("EGLDevice::attachWindow(0)");
    }
#endif
    
    if (hwinit) {
        if (egl.supportsRenderer(rtype)) {
            runtest.pass("EGLDevice::supportsRenderer()");
        } else {
            runtest.fail("EGLDevice::supportsRenderer()");
        }
    } else {
        runtest.untested("EGLDevice::supportsRenderer()");
    }
    
    if (hwinit) {    
        if (egl.getRedSize() > 0) {
            runtest.pass("EGLDevice::getRedSize()");
        } else {
            runtest.fail("EGLDevice::getRedSize()");
        }    
    } else {
        runtest.untested("EGLDevice::getRedSize()");
    }
    
    if (hwinit) {
        if (egl.getGreenSize() > 0) {
            runtest.pass("EGLDevice::getGreenSize()");
        } else {
            runtest.fail("EGLDevice::getGreenSize()");
        }    
    } else {
        runtest.untested("EGLDevice::getGreenSize()");
    }
    
    if (hwinit) {
        if (egl.getBlueSize() > 0) {
            runtest.pass("EGLDevice::getBlueSize()");
        } else {
            runtest.fail("EGLDevice::getBlueSize()");
        }
    } else {
        runtest.untested("EGLDevice::getBlueSize()");
    }

    // Surface config info tests
    if (hwinit) {
        if (egl.getSurfaceID()) {
            runtest.pass("EGLDevice::getSurfaceID()");
        } else {
            runtest.fail("EGLDevice::getSurfaceID()");
        }
    } else {
        runtest.untested("EGLDevice::getSurfaceID()");
    }
    
    if (hwinit) {
        if (egl.getWidth()) {
            runtest.pass("EGLDevice::getWidth()");
        } else {
            runtest.fail("EGLDevice::getWidth()");
        }
    } else {
        runtest.untested("EGLDevice::getWidth()");
    }
    
    if (hwinit) {
        if (egl.getHeight()) {
            runtest.pass("EGLDevice::getHeigth()");
        } else {
            runtest.fail("EGLDevice::getHeigth()");
        }
    } else {
        runtest.untested("EGLDevice::getHeigth()");
    }
    
    if (hwinit) {
        if (egl.getVerticalRes()) {
            runtest.pass("EGLDevice::getVerticalRes()");
        } else {
            runtest.fail("EGLDevice::getVerticalRes()");
        }
    } else {
        runtest.untested("EGLDevice::getVerticalRes()");
    }
    
    if (hwinit) {
        if (egl.getHorzRes()) {
            runtest.pass("EGLDevice::getHorzRes()");
        } else {
            runtest.fail("EGLDevice::getHorzRes()");
        }
    } else {
        runtest.untested("EGLDevice::getHorzRes()");
    }

    if (hwinit) {
        if (egl.isSingleBuffered() != egl.isBackBuffered()) {
            runtest.pass("EGLDevice::is*Buffered()");
        } else {
            runtest.fail("EGLDevice::is*Buffered()");
        }
    } else {
        runtest.untested("EGLDevice::is*Buffered()");
    }

    if (hwinit) {
        if (egl.isBufferDestroyed()) {
            runtest.pass("EGLDevice::isBufferDestroyed()");
        } else {
            runtest.fail("EGLDevice::isBufferDestroyed()");
        }
    } else {
        runtest.untested("EGLDevice::isBufferDestroyed()");
    }
    
    if (hwinit) {
        if (!egl.isMultiSample()) {
            runtest.pass("EGLDevice::isMultiSample()");
        } else {
            runtest.fail("EGLDevice::isMultiSample()");
        }
    } else {
        runtest.untested("EGLDevice::isMultiSample()");
    }

    // Context accessor tests
    if (hwinit) {
        if (egl.getContextID()) {
            runtest.pass("EGLDevice::getContextID()");
        } else {
            runtest.fail("EGLDevice::getContextID()");
        }
    } else {
        runtest.untested("EGLDevice::getContextID()");
    }

#if 0
    if (egl.supportsClient(rtype)) {
        runtest.pass("EGLDevice::supportsClient()");
    } else {
        runtest.fail("EGLDevice::supportsClient()");
    }
#endif
    
    if (hwinit) {
        if (egl.isContextSingleBuffered() != egl.isContextBackBuffered()) {
            runtest.pass("EGLDevice::isContext*Buffered()");
        } else {
            runtest.fail("EGLDevice::isContext*Buffered()");
        }
    } else {
        runtest.untested("EGLDevice::isContext*Buffered()");
    }
    
    if (hwinit) {
        if (egl.isNativeRender()) {
            runtest.pass("EGLDevice::isNativeRender()");
        } else {
            runtest.fail("EGLDevice::isNativeRender()");
        }
    } else {
        runtest.untested("EGLDevice::isNativeRender()");
    }

    if (hwinit) {
        if (egl.getSamples() == 0) {
            runtest.pass("EGLDevice::getSamples()");
        } else {
            runtest.fail("EGLDevice::getSamples()");
        }
    } else {
        runtest.untested("EGLDevice::getSamples()");
    }
    
    if (hwinit) {
        if (egl.getSampleBuffers() == 0) {
            runtest.pass("EGLDevice::getSampleBuffers()");
        } else {
            runtest.fail("EGLDevice::getSampleBuffers()");
        }
    } else {
        runtest.untested("EGLDevice::getSampleBuffers()");
    }
    
    if (hwinit) {
        if (egl.getDepth()) {
            runtest.pass("EGLDevice::getDepth()");
        } else {
            runtest.fail("EGLDevice::getDepth()");
        }
    } else {
        runtest.untested("EGLDevice::getDepth()");
    }
    
    if (hwinit) {
        if (egl.getMaxSwapInterval() == 0) {
            runtest.pass("EGLDevice::getMaxSwapInterval()");
        } else {
            runtest.fail("EGLDevice::getMaxSwapInterval()");
        }
    } else {
        runtest.untested("EGLDevice::getMaxSwapInterval()");
    }
    
    if (hwinit) {
        if (egl.getMinSwapInterval() == 0) {
            runtest.pass("EGLDevice::getMinSwapInterval()");
        } else {
            runtest.fail("EGLDevice::getMinSwapInterval()");
        }
    } else {
        runtest.untested("EGLDevice::getMinSwapInterval()");
    }

    // Test Pbuffers
    if (hwinit) {
        EGLSurface surf = egl.createPbuffer(200, 200);
        if ((surf != EGL_NO_SURFACE) && (egl.getWidth(surf) == 200)) {
            runtest.pass("EGLDevice::createPbuffer(int, int)");
        } else {
            runtest.fail("EGLDevice::createPbuffer(int, int)");
        }
    } else {
        runtest.untested("EGLDevice::createPbuffer(int, int)");
    }
    
    // EGLSurface surf1 = egl[0];
    // if ((surf1 != EGL_NO_SURFACE) && (egl.getWidth(surf1) == 200)
    //     && (egl.getHeight(surf1) == 200)) {
    //     runtest.pass("EGLDevice::operator[]()");
    // } else {
    //     runtest.fail("EGLDevice::operator[]()");
    // }

    EGLSurface surf2 = egl.createPbuffer(300, 300);
    EGLSurface surf3 = egl.createPbuffer(400, 400);

    if (hwinit) {
        if (egl.totalPbuffers() == 3) {
            runtest.pass("EGLDevice::totalPbuffers(2)");
        } else {
            runtest.fail("EGLDevice::totalPbuffers(2)");
        }
    }

    // Since we're EGL_SINGLE_BUFFER'd, this is a nop
    if (hwinit) {
        if (egl.swapPbuffers()) {
            runtest.pass("EGLDevice::swapPbuffers()");
        } else {
            runtest.fail("EGLDevice::swapPbuffers()");
        }
    } else {
        runtest.untested("EGLDevice::swapPbuffers()");
    }

    egl.makePbufferCurrent(1);
    EGLSurface surf4 = eglGetCurrentSurface(EGL_DRAW);
    if (hwinit) {
        if ((egl.getWidth(surf4) == 300) && ((egl.getHeight(surf4) == 300))) {
            runtest.pass("EGLDevice::makePbufferCurrent(int)");
        } else {
            runtest.fail("EGLDevice::makePbufferCurrent(int)");
        }
    } else {
        runtest.untested("EGLDevice::makePbufferCurrent(int)");
    }

    // This should trigger an error as the number is more than we
    // have created
    if (hwinit) {
        if (!egl.makePbufferCurrent(10)) {
            runtest.pass("EGLDevice::makePbufferCurrent(maxed)");
        } else {
            runtest.fail("EGLDevice::makePbufferCurrent(maxed)");
        }
    } else {
        runtest.untested("EGLDevice::makePbufferCurrent(maxed)");
    }
    
#if 0
    if (!egl.copyPbuffers(1)) {
        runtest.pass("EGLDevice::copyPbuffers()");
    } else {
        runtest.fail("EGLDevice::copyPbuffers()");
    }
#endif
    
    // EGLSurface surf5 = eglGetCurrentSurface(EGL_DRAW);
    // egl.printEGLSurface(surf5);
}

// Local Variables:
// mode: C++
// indent-tabs-mode: nil
// End:
