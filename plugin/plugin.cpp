//   Copyright (C) 2005, 2006 Free Software Foundation, Inc.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "plugin.h"
#define MIME_TYPES_HANDLED  "application/x-shockwave-flash"
// The name must be this value to get flash movies that check the
// plugin version to load.
#define PLUGIN_NAME     "Shockwave Flash 7.0"
#define MIME_TYPES_DESCRIPTION  MIME_TYPES_HANDLED":swf:"PLUGIN_NAME
#define PLUGIN_DESCRIPTION PLUGIN_NAME

//"Copyright 2005, 2006 Free Software Foundation, Inc. This is free software with ABSOLUTELY NO WARRANTY. For details type see the file COPYING in the sources."

#include <GL/gl.h>              // Header File For The OpenGL32 Library
#include <GL/glu.h>             // Header File For The GLu32 Library

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>
#include <X11/Sunkeysym.h>
#include <SDL.h>
#include <SDL_thread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <string>

#include "ogl_sdl.h"
#include "player.h"
#include "xmlsocket.h"

#include "tu_file.h"
#include "tu_types.h"

using namespace std;

extern bool GLinitialized;
extern bool processing;

static int   streamfd = -1;
static float s_scale = 1.0f;
static bool  s_verbose = false;
static int   doneYet = 0;

const int INBUFSIZE = 1024;

#ifdef HAVE_LIBXML
extern int xml_fd;		// FIXME: this is the file descriptor
				// from XMLSocket::connect(). This
				// needs to be propogated up through
				// the layers properly, but first I
				// want to make sure it all works.
#endif // HAVE_LIBXML
static int eventThread(void *inst);
int playswf(nsPluginInstance *inst);

SDL_Thread *thread = NULL;

char*
NPP_GetMIMEDescription(void)
{
    return(MIME_TYPES_DESCRIPTION);
}

static void
log_callback(bool error, const char* message)
// Error callback for handling messages.
{
    if (error) {
        printf(message);
    }
}


/////////////////////////////////////
// general initialization and shutdown
//
NPError
NS_PluginInitialize()
{
    printf("%s(%d): Entering\n", __PRETTY_FUNCTION__, __LINE__);
    return NPERR_NO_ERROR;
}

void
NS_PluginShutdown()
{
    printf("%s(%d): Entering\n", __PRETTY_FUNCTION__, __LINE__);
}

// get values per plugin
NPError
NS_PluginGetValue(NPPVariable aVariable, void *aValue)
{
    NPError err = NPERR_NO_ERROR;
    switch (aVariable) {
      case NPPVpluginNameString:
          *((char **)aValue) = PLUGIN_NAME;
          break;
      case NPPVpluginDescriptionString:
          *((char **)aValue) = PLUGIN_DESCRIPTION;
          break;
      case NPPVpluginTimerInterval:
      case NPPVpluginNeedsXEmbed:
      case NPPVpluginKeepLibraryInMemory:
      default:
          err = NPERR_INVALID_PARAM;
          break;
    }
    return err;
}

/////////////////////////////////////////////////////////////
//
// construction and destruction of our plugin instance object
//
nsPluginInstanceBase *
NS_NewPluginInstance(nsPluginCreateData * aCreateDataStruct)
{
    printf("%s(%d): Entering\n", __PRETTY_FUNCTION__, __LINE__);
    if(!aCreateDataStruct)
        return NULL;

    nsPluginInstance * plugin = new nsPluginInstance(aCreateDataStruct->instance);
    
    return plugin;
}

void
NS_DestroyPluginInstance(nsPluginInstanceBase * aPlugin)
{
    printf("%s(%d): Entering\n", __PRETTY_FUNCTION__, __LINE__);
    if(aPlugin)
        delete (nsPluginInstance *)aPlugin;
}

////////////////////////////////////////
//
// nsPluginInstance class implementation
//
nsPluginInstance::nsPluginInstance(NPP aInstance) : nsPluginInstanceBase(),
                                                    mInstance(aInstance),
                                                    mInitialized(FALSE),
                                                    mWindow(0),
                                                    mXtwidget(0),
                                                    mFontInfo(0),
                                                    thr_count(0)
{
    printf("%s(%d): Entering\n", __PRETTY_FUNCTION__, __LINE__);
}

// Cleanup resources
nsPluginInstance::~nsPluginInstance()
{
    printf("%s(%d): Entering\n", __PRETTY_FUNCTION__, __LINE__);

#if 0
    if (cond) {
        SDL_DestroyCond(cond);
    }
    if (mutex) {
        SDL_DestroyMutex(mutex);
    }
    if (thread) {
        SDL_KillThread(thread);
    }    
    SDL_Quit();
#endif
}

static void
xt_event_handler(Widget xtwidget, nsPluginInstance *plugin, XEvent *xevent, Boolean *b)
{
    int        keycode;
    KeySym     keysym;
#if 0
    SDL_Event  sdl_event;
    SDL_keysym sdl_keysym;

    //    handleKeyPress((SDL_keysym)keysym);
    printf("Peep Event returned %d\n", SDL_PeepEvents(&sdl_event, 1, SDL_PEEKEVENT, SDL_USEREVENT|SDL_ACTIVEEVENT|SDL_KEYDOWN|SDL_KEYUP|SDL_MOUSEBUTTONUP|SDL_MOUSEBUTTONDOWN));
  
    if (SDL_PollEvent(&sdl_event)) {
        switch(sdl_event.type) {
          case SDL_ACTIVEEVENT:
          case SDL_VIDEORESIZE:
          case SDL_KEYDOWN:
              /* handle key presses */
              handleKeyPress( &sdl_event.key.keysym );
              break;
          default:
              break;
      
        }
    }
#endif
  
    switch (xevent->type) {
      case Expose:
          // get rid of all other exposure events
          if (plugin) {
              if (GLinitialized) {
//                   drawGLScene();
                  printf("HACK ALERT! ignoring expose event!\n");
              } else {
                  printf("GL Surface not initialized yet, ignoring expose event!\n");
	      }
          }
          break;
      case ButtonPress:
//     fe.type = FeButtonPress;
          printf("Button Press\n");
          break;
      case ButtonRelease:
          //     fe.type = FeButtonRelease;
          printf("Button Release\n");
          break;
      case KeyPress:
          keycode = xevent->xkey.keycode;
          keysym = XLookupKeysym((XKeyEvent*)xevent, 0);
          printf ("%s(%d): Keysym is %s\n", __PRETTY_FUNCTION__, __LINE__,
                  XKeysymToString(keysym));

          switch (keysym) {
            case XK_Up:
                printf("Key Up\n");
                break;
            case XK_Down:
                printf("Key Down\n");
                break;
            case XK_Left:
                printf("Key Left\n");
                break;
            case XK_Right:
                printf("Key Right\n");
                break;
            case XK_Return:
                printf("Key Return\n");
                break;
      
            default:
                break;
          }
    }
}

// This draws a box weith some text using the X intrinsics.
void
nsPluginInstance::draw()
{
    unsigned int h = mHeight/2;
    unsigned int w = 3 * mWidth/4;
    int x = (mWidth - w)/2; // center
    int y = h/2;
    if (x >= 0 && y >= 0) {
        GC gc = XCreateGC(mDisplay, mWindow, 0, NULL);
        if (!gc) 
            return;
        XDrawRectangle(mDisplay, mWindow, gc, x, y, w, h);
        const char *string = getVersion();
        if (string && *string) {
            int l = strlen(string);
            int fmba = mFontInfo->max_bounds.ascent;
            int fmbd = mFontInfo->max_bounds.descent;
            int fh = fmba + fmbd;
            y += fh;
            x += 32;
            XDrawString(mDisplay, mWindow, gc, x, y, string, l); 
        }
        XFreeGC(mDisplay, gc);
    }
}

NPBool nsPluginInstance::init(NPWindow* aWindow)
{
    printf("%s(%d): Entering\n", __PRETTY_FUNCTION__, __LINE__);

//      initGL(this);
//      surface_activated = true;

    mutex = SDL_CreateMutex();
    
    if(aWindow == NULL) {
        return FALSE;
    }
  
    if (SetWindow(aWindow)) {
        mInitialized = TRUE;
    }
	
    return mInitialized;
}

void nsPluginInstance::shut()
{
    printf("%s(%d): Entering. Thread_count is %d\n", __PRETTY_FUNCTION__, __LINE__, thr_count);
    mInitialized = FALSE;


    GLinitialized = false;
    
    thr_count--;
    
#if 0
    if (cond) {
        SDL_DestroyCond(cond);
    }
    if (thread) {
        SDL_KillThread(thread);
    }
    
    if (mutex) {
        SDL_DestroyMutex(mutex);
    }
#endif
    if (thr_count <= 1) {
        SDL_Quit();
    }
}

const char * nsPluginInstance::getVersion()
{
    return NPN_UserAgent(mInstance);
}

NPError nsPluginInstance::GetValue(NPPVariable aVariable, void *aValue)
{
    NPError err = NPERR_NO_ERROR;
    switch (aVariable) {
      case NPPVpluginNameString:
      case NPPVpluginDescriptionString:
          return NS_PluginGetValue(aVariable, aValue) ;
          break;
      default:
          err = NPERR_INVALID_PARAM;
          break;
    }
    return err;

}

NPError nsPluginInstance::SetWindow(NPWindow* aWindow)
{
    if(aWindow == NULL)
        return FALSE;

    mX = aWindow->x;
    mY = aWindow->y;
    mWidth = aWindow->width;
    mHeight = aWindow->height;

    printf("%s: X origin = %d, Y Origin = %d, Width = %d, Height = %d\n",
           __PRETTY_FUNCTION__, mX, mY, mWidth, mHeight);
  
    if (mWindow == (Window) aWindow->window) {
        // The page with the plugin is being resized.
        // Save any UI information because the next time
        // around expect a SetWindow with a new window id.
    } else {
        mWindow = (Window) aWindow->window;
        NPSetWindowCallbackStruct *ws_info = (NPSetWindowCallbackStruct *)aWindow->ws_info;
        mDisplay = ws_info->display;
        mVisual = ws_info->visual;
        mDepth = ws_info->depth;
        mColormap = ws_info->colormap;

        if (!mFontInfo) {
            if (!(mFontInfo = XLoadQueryFont(mDisplay, "9x15")))
                printf("Cannot open 9X15 font\n");
        }

        // add xt event handler
        Widget xtwidget = XtWindowToWidget(mDisplay, mWindow);
        if (xtwidget && mXtwidget != xtwidget) {
            mXtwidget = xtwidget;
            // mask values are:
            // KeyPress, KeyRelease, ButtonPress, ButtonRelease,
            // PointerMotion, Button1Motion, Button2Motion, Button3Motion,
            // Button4Motion, Button5Motion 
            long event_mask = ExposureMask|KeyPress|KeyRelease|ButtonPress|ButtonRelease;
            XSelectInput(mDisplay, mWindow, event_mask);
            XtAddEventHandler(xtwidget, event_mask, False, (XtEventHandler)xt_event_handler, this);
        }
    }
  
#if 0
    if (aWindow->type == NPWindowTypeWindow) {
        WriteStatus("Window type is \"Windowed\"");
    }
    if (aWindow->type == NPWindowTypeDrawable) {
        WriteStatus("Window type is \"Drawable\"");
    }
#endif
    
    return TRUE;
}

// Write a status message to the status line and the console.
NPError
nsPluginInstance::WriteStatus(char *msg) const
{
    NPN_Status(mInstance, msg);
    printf("%s\n", msg);
}

// Open a new incoming data stream, which is the flash movie we want to play.
// A URL can be pretty ugly, like in this example:
// http://www.shockwave.com/swf/navbar/navbar_sw.swf?atomfilms=http%3a//www.atomfilms.com/af/home/&shockwave=http%3a//www.shockwave.com&gameblast=http%3a//gameblast.shockwave.com/gb/gbHome.jsp&known=0
NPError
nsPluginInstance::NewStream(NPMIMEType type, NPStream * stream,
                            NPBool seekable, uint16 * stype)
{
    char tmp[100];
    memset(tmp, 0, 100);
    string url = stream->url;
    string fname;
    int start, end;

    end   = url.find(".swf", 0) + 4;
    start = url.rfind("/", end) + 1;
    fname = "/tmp/";
    fname += url.substr(start, end - start);

    //  printf("%s: URL is %s\n", __PRETTY_FUNCTION__, url.c_str());
    printf("%s: Open stream for %s (%d, %d)\n", __PRETTY_FUNCTION__, fname.c_str(), start, end);

    sprintf(tmp, "Loading Shockwave file %s", fname.c_str());
    WriteStatus(tmp);
  
    streamfd = open(fname.c_str(), O_CREAT | O_WRONLY, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
    if (streamfd < 0) {
        sprintf(tmp,"%s can't be opened, check your permissions!\n", fname.c_str());
        WriteStatus(tmp);
        streamfd = open(fname.c_str(), O_TRUNC | O_WRONLY, S_IRUSR|S_IRGRP|S_IROTH);
        if (streamfd < 0) {
            sprintf(tmp,"%s can't be created, check your permissions!\n", fname.c_str());
            WriteStatus(tmp);
        }
    }

    swf_file = fname;
    processing = true;
    return NPERR_NO_ERROR;
}

NPError
nsPluginInstance::DestroyStream(NPStream * stream, NPError reason)
{
    printf("%s (%i): %s\n", __PRETTY_FUNCTION__, reason, stream->url);
    processing = false;

    if (streamfd) {
        close(streamfd);
        streamfd = -1;
    }

//     cond = SDL_CreateCond();
    thr_count++;
//    thread = SDL_CreateThread(playerThread, (void *)this);
    thread = SDL_CreateThread(eventThread, (void *)this);
  
//     SDL_mutexP(mutex);
//     SDL_CondSignal(cond);

}

void
nsPluginInstance::URLNotify(const char *url, NPReason reason,
                            void *notifyData)
{
    printf("%s(%d): Entering\n", __PRETTY_FUNCTION__, __LINE__);
    printf("URL: %s\nReason %i\n", url, reason);
}

// Return how many bytes we can read into the buffer
int32
nsPluginInstance::WriteReady(NPStream * stream)
{
//   printf("%s(%d): Entering\n", __PRETTY_FUNCTION__, __LINE__);
//   printf("Stream for %s is ready\n", stream->url);

    return INBUFSIZE;
}

// Read the daat stream from Mozilla/Firefox
int32
nsPluginInstance::Write(NPStream * stream, int32 offset, int32 len,
                        void *buffer)
{
//   printf("%s(%d): Entering\n", __PRETTY_FUNCTION__, __LINE__);
//   printf("Reading Stream %s, offset is %d, length = %d \n",
//          stream->url, offset, len);

    write(streamfd, buffer, len);
}

static int
eventThread(void *arg)
{
    printf("%s: \n", __PRETTY_FUNCTION__);
    int retries = 0;
  
    nsPluginInstance *inst = (nsPluginInstance *)arg;

    if (!GLinitialized) {
        initGL(inst);
        GLinitialized = true;
    }
    
    while (retries++ < 2) {
#if 1
        drawGLScene();
#else
//        playswf(inst);
        main_loop(inst);
#endif
        SDL_Delay(20);      // don't trash the CPU
        // So we don't run forever for now.
        printf("%s(%d): FIXME: loop timed out\n",
               __PRETTY_FUNCTION__, __LINE__);
        break;
    }     
  
    return 0;
}

// Local Variables:
// mode: C++
// indent-tabs-mode: t
// End:
