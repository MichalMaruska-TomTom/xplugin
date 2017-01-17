#define MMC_PIPELINE 1 /* fixme: This should be obsoleted. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/XKBlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XKB.h>
#include <X11/Xatom.h>

#include <X11/extensions/XKBstr.h>
#include <X11/XKBlib.h>

/* #define XKB_PLUGIN_DIR "{top-x-directory}/lib/modules/xkb-plugins/" */

int
main(int argc, char** argv)
{
    int major=1;
    int minor=0;
    int error_rtrn;
    int event_rtrn;
    int reason_rtrn;
    int device = XkbUseCoreKbd;

    Display* dpy;

    int res = False;
    int i = 1;

    if ((argc<2) || (0 == strcmp(argv[1],"-h")))
    {
        printf("usage: xplugin -d {number} [-]filename\nxplugin -[vl]\n"
               "   where filename is in the %s\n",
               "XKB_PLUGIN_DIR");
        exit(-1);
    }


    dpy = XkbOpenDisplay(getenv("DISPLAY"), &event_rtrn, &error_rtrn, &major, &minor, &reason_rtrn);
    if (dpy == NULL) {
        fprintf(stderr, "XkbOpenDisplay on %s failed: %d %s\n", getenv("DISPLAY"),
                reason_rtrn,
                (reason_rtrn == XkbOD_BadLibraryVersion)?"version":"??");
        perror ("XkbOpenDisplay");
        exit (-1);
    }

    if (0 == strcmp(argv[i],"-d")) {
        i++;
        device = atoi(argv[i++]);
    }

    if (0 == strcmp(argv[i],"-v"))
    {
        printf("dpy: xkb version: %d.%d reason: %d\n", major, minor, reason_rtrn);
        i++;
    }
    else if (0 == strcmp(argv[i],"-l"))
    {
        XkbPipelineListPtr p = XkbListPipeline(dpy, device);
        int j;
        i++;
        if (!p)
            exit(-1);

        printf("%d plugin%s in the pipeline\n",
               p->num_plugins,
               (p->num_plugins==0)?"":"s");
        for(j=0; j < p->num_plugins; j++)
        {                       /* flags ? */
            printf("%d: %s\n", p->plugins[j].id, p->plugins[j].name);
        }
    } else {
        if (argv[i][0] == '-') {
            printf ("removing %s\n", argv[i]);
            res = XkbSetPlugin(dpy, device, "", argv[i] +1, True); /* before */
            if (res == False)
                printf ("XkbSetPlugin failed\n");
        } else {
            /* Check if it's in! */
            /* XGrabServer(dpy); */
            char* module_name = argv[i];
            char* in_front_of = "xkb-auto-repeat";

            XkbPipelineListPtr p = XkbListPipeline(dpy, device);
            /* Is it already loaded? */
            int i;
            for(i= 0; i < p->num_plugins; i++)
                if (0 == strcmp(module_name, p->plugins[i].name))
                {
                    printf ("%s already loaded\n", module_name);
                    goto close;
                }
            /* i might test, if in_front_of is present! */
            printf ("loading %s\n", module_name);
            res = XkbSetPlugin(dpy, device, module_name, in_front_of, True); /* before */
            /* XUngrabServer(dpy); */
            if (res == False)
                printf ("XkbSetPlugin failed\n");
        }
    }
  close:
    XCloseDisplay(dpy);
    exit ((res == True)?0:1);
}
