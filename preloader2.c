#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

// Function pointer typedefs for original X11 functions
typedef Window (*XCreateWindow_t)(Display *display, Window parent, int x, int y,
                                  unsigned int width, unsigned int height,
                                  unsigned int border_width, int depth,
                                  unsigned int class, Visual *visual,
                                  unsigned long valuemask,
                                  XSetWindowAttributes *attributes);
typedef int (*XMapWindow_t)(Display *display, Window w);
typedef int (*XSync_t)(Display *display, Bool discard);
typedef int (*XNextEvent_t)(Display *display, XEvent *event);
typedef int (*XDrawRectangle_t)(Display *display, Drawable d, GC gc, int x, int y,
                                unsigned int width, unsigned int height);

// Pointers to the original X11 functions
static XCreateWindow_t original_XCreateWindow = NULL;
static XMapWindow_t original_XMapWindow = NULL;
static XSync_t original_XSync = NULL;
static XNextEvent_t original_XNextEvent = NULL;
static XDrawRectangle_t original_XDrawRectangle = NULL;

// Our custom border width
const int BORDER_WIDTH = 5;

// Replacement XCreateWindow function
Window XCreateWindow(Display *display, Window parent, int x, int y,
                     unsigned int width, unsigned int height,
                     unsigned int border_width, int depth,
                     unsigned int class, Visual *visual,
                     unsigned long valuemask,
                     XSetWindowAttributes *attributes) {
    printf("Inside XCreateWindow replacement\n");
    if (!original_XCreateWindow) {
        printf("Error: XCreateWindow function pointer not initialized\n");
        exit(1);
    }
    // Call the original XCreateWindow function
    Window window = original_XCreateWindow(display, parent, x, y, width, height,
                                            border_width, depth, class, visual,
                                            valuemask, attributes);
    // Draw a border around the window
    XDrawRectangle(display, window, DefaultGC(display, 0), 0, 0, width - 1, height - 1);
    return window;
}

// Replacement XMapWindow function
int XMapWindow(Display *display, Window w) {
    printf("Inside XMapWindow replacement\n");
    if (!original_XMapWindow) {
        printf("Error: XMapWindow function pointer not initialized\n");
        exit(1);
    }
    // Call the original XMapWindow function
    int result = original_XMapWindow(display, w);
    XSync(display, False); // Ensure drawing is complete
    return result;
}

// Replacement XDrawRectangle function
int XDrawRectangle(Display *display, Drawable d, GC gc, int x, int y,
                   unsigned int width, unsigned int height) {
    printf("Inside XDrawRectangle replacement\n");
    if (!original_XDrawRectangle) {
        printf("Error: XDrawRectangle function pointer not initialized\n");
        exit(1);
    }
    // Call the original XDrawRectangle function
    int result = original_XDrawRectangle(display, d, gc, x, y, width, height);
    return result;
}

// Replacement XNextEvent function
int XNextEvent(Display *display, XEvent *event) {
    printf("Inside XNextEvent replacement\n");
    if (!original_XNextEvent) {
        printf("Error: XNextEvent function pointer not initialized\n");
        exit(1);
    }
    // Call the original XNextEvent function
    int result = original_XNextEvent(display, event);
    // If the window is mapped, draw the border
    if (event->type == MapNotify) {
        XWindowAttributes attr;
        XGetWindowAttributes(display, event->xmap.window, &attr);
        XDrawRectangle(display, event->xmap.window, DefaultGC(display, 0), 0, 0,
                       attr.width - 1, attr.height - 1);
    }
    return result;
}

// Initialize function pointers to original X11 functions
void initializeX11Functions() {
    printf("Initializing X11 function pointers\n");
    void *x11_lib = dlopen("libX11.so", RTLD_LAZY);
    if (!x11_lib) {
        fprintf(stderr, "Error: Failed to open libX11.so: %s\n", dlerror());
        exit(1);
    }

    original_XCreateWindow = (XCreateWindow_t)dlsym(x11_lib, "XCreateWindow");
    original_XMapWindow = (XMapWindow_t)dlsym(x11_lib, "XMapWindow");
    original_XSync = (XSync_t)dlsym(x11_lib, "XSync");
    original_XNextEvent = (XNextEvent_t)dlsym(x11_lib, "XNextEvent");
    original_XDrawRectangle = (XDrawRectangle_t)dlsym(x11_lib, "XDrawRectangle");

    if (!original_XCreateWindow || !original_XMapWindow || !original_XSync ||
        !original_XNextEvent || !original_XDrawRectangle) {
        fprintf(stderr, "Error: Failed to load X11 functions: %s\n", dlerror());
        exit(1);
    }

    dlclose(x11_lib);
}

int main() {
    printf("Preloader application started\n");

    // Initialize function pointers to original X11 functions
    initializeX11Functions();

    // Now you can proceed with your application logic here

    return 0;
}

