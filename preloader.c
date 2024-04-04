#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <unistd.h> // for usleep

// Define your decoration size
const int BORDER_WIDTH = 10;

Display *display;

// Function to draw custom decoration around the window
void drawBorder(Window window) {
    if (window == None) {
        fprintf(stderr, "Invalid window parameter\n");
        return;
    }

    XWindowAttributes attr;
    if (!XGetWindowAttributes(display, window, &attr)) {
        fprintf(stderr, "Failed to get window attributes\n");
        return;
    }
    if (attr.map_state != IsViewable) {
        // Window is not mapped, do not draw border
        return;
    }

    XSetForeground(display, DefaultGC(display, 0), BlackPixel(display, DefaultScreen(display)));
    XDrawRectangle(display, window, DefaultGC(display, 0), 0, 0, attr.width - 1, attr.height - 1);
}

// X Error handler function
int handleError(Display *d, XErrorEvent *ev) {
    char err_buf[256];
    XGetErrorText(d, ev->error_code, err_buf, 256);
    fprintf(stderr, "X Error: %s\n", err_buf);
    return 0;
}

// Event handler function
void handleEvents() {
    XEvent event;
    while (1) {
        XNextEvent(display, &event);
        switch (event.type) {
            case CreateNotify:
                // Delay slightly before drawing the border to ensure window initialization
                usleep(10000); // 10ms delay
                drawBorder(event.xcreatewindow.window);
                break;
            case Expose:
                drawBorder(event.xexpose.window);
                break;
        }
    }
}

// Constructor function that is called when the library is loaded
__attribute__((constructor))
void preload() {
    display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "Error: Unable to open display\n");
        return;
    }

    XSetErrorHandler(handleError);

    XSelectInput(display, DefaultRootWindow(display), SubstructureNotifyMask | ExposureMask);

    handleEvents();
}

