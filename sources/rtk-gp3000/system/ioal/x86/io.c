#if defined(CYGWIN_MDIO_IO)

#include <ioal/x86/io.h>
#include <osal/print.h>


PORTOUT PortOut;
PORTWORDOUT PortWordOut;
PORTDWORDOUT PortDWordOut;
PORTIN PortIn;
PORTWORDIN PortWordIn;
PORTDWORDIN PortDWordIn;
SETPORTBIT SetPortBit;
CLRPORTBIT ClrPortBit;
NOTPORTBIT NotPortBit;
GETPORTBIT GetPortBit;
RIGHTPORTSHIFT RightPortShift;
LEFTPORTSHIFT LeftPortShift;
ISDRIVERINSTALLED IsDriverInstalled;

HMODULE hio;


void UnloadIODLL(void);
int LoadIODLL(void);


void UnloadIODLL(void) {
	FreeLibrary(hio);
}

int LoadIODLL(void) {
    /* Load iopout32.dll, for Win7, vista, XP usage */
    hio = LoadLibrary("inpout32.dll");
   
    if (hio == NULL)
        osal_printf("\nload inpout32.dll failure!\n");


	PortOut         = (PORTOUT)GetProcAddress(hio, "Out32");
	PortWordOut     = (PORTWORDOUT)NULL;
	PortDWordOut    = (PORTDWORDOUT)NULL;
	PortIn          = (PORTIN)GetProcAddress(hio, "Inp32");
	PortWordIn      = (PORTWORDIN)NULL;
	PortDWordIn     = (PORTDWORDIN)NULL;
	SetPortBit      = (SETPORTBIT)NULL;
	ClrPortBit      = (CLRPORTBIT)NULL;
	NotPortBit      = (NOTPORTBIT)NULL;
	GetPortBit      = (GETPORTBIT)NULL;
	RightPortShift  = (RIGHTPORTSHIFT)NULL;
	LeftPortShift   = (LEFTPORTSHIFT)NULL;
	IsDriverInstalled = (ISDRIVERINSTALLED)NULL;

	atexit(UnloadIODLL);

	return 0;
}

#endif /*defined(CYGWIN_MDIO_IO)*/
