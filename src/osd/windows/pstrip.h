/* 

UM_SETCUSTOMTIMING = WM_USER+200;  
wparam = monitor number, zero-based  
lparam = atom for string pointer  
lresult = -1 for failure else current pixel clock (integer in Hz)  
Note: pass full PowerStrip timing string*  
 
UM_SETREFRESHRATE = WM_USER+201;  
wparam = monitor number, zero-based  
lparam = refresh rate (integer in Hz), or 0 for read-only  
lresult = -1 for failure else current refresh rate (integer in Hz)  
 
UM_SETPOLARITY = WM_USER+202;  
wparam = monitor number, zero-based  
lparam = polarity bits  
lresult = -1 for failure else current polarity bits+1  
 
UM_REMOTECONTROL = WM_USER+210;  
wparam = 99  
lparam =  
    0 to hide tray icon  
    1 to show tray icon,  
    2 to get build number  
   10 to show Performance profiles  
   11 to show Color profiles  
   12 to show Display profiles  
   13 to show Application profiles  
   14 to show Adapter information  
   15 to show Monitor information  
   16 to show Hotkey manager  
   17 to show Resource manager  
   18 to show Preferences  
   19 to show Online services  
   20 to show About screen  
   21 to show Tip-of-the-day  
   22 to show Setup wizard  
   23 to show Screen fonts  
   24 to show Advanced timing options  
   25 to show Custom resolutions  
   99 to close PS  
lresult = -1 for failure else lparam+1 for success or build number (e.g., 335) 
if lparam was 2  
 
UM_SETGAMMARAMP = WM_USER+203;  
wparam = monitor number, zero-based  
lparam = atom for string pointer  
lresult = -1 for failure, 1 for success  
 
UM_CREATERESOLUTION = WM_USER+204;  
wparam = monitor number, zero-based  
lparam = atom for string pointer  
lresult = -1 for failure, 1 for success  
Note: pass full PowerStrip timing string*; reboot is usually necessary to see if
the resolution is accepted by the display driver  
 
UM_GETTIMING = WM_USER+205;  
wparam = monitor number, zero-based  
lresult = -1 for failure else GlobalAtom number identifiying the timing string*  
Note: be sure to call GlobalDeleteAtom after reading the string associated with 
the atom  
 
UM_GETSETCLOCKS = WM_USER+206;  
wparam = monitor number, zero-based  
lparam = atom for string pointer  
lresult = -1 for failure else GlobalAtom number identifiying the performance 
string**  
Note: pass full PowerStrip performance string** to set the clocks, and ull to 
get clocks; be sure to call GlobalDeleteAtom after reading the string associated 
with the atom  
 
NegativeHorizontalPolarity = 0x02;
NegativeVerticalPolarity = 0x04;
 
*Timing string parameter definition:  
 1 = horizontal active pixels  
 2 = horizontal front porch  
 3 = horizontal sync width  
 4 = horizontal back porch  
 5 = vertical active pixels  
 6 = vertical front porch  
 7 = vertical sync width  
 8 = vertical back porch  
 9 = pixel clock in hertz  
10 = timing flags, where bit:  
     1 = negative horizontal porlarity  
     2 = negative vertical polarity  
     3 = interlaced  
     5 = composite sync  
     7 = sync-on-green  
     all other bits reserved  
 
**Performance string parameter definition:  
 1 = memory clock in hertz  
 2 = engine clock in hertz  
 3 = reserved  
 4 = reserved  
 5 = reserved  
 6 = reserved  
 7 = reserved  
 8 = reserved  
 9 = 2D memory clock in hertz (if different from 3D)  
10 = 2D engine clock in hertz (if different from 3D) 
 
*/
 
#define UM_SETCUSTOMTIMING      (WM_USER+200)  
#define UM_SETREFRESHRATE       (WM_USER+201)  
#define UM_SETPOLARITY          (WM_USER+202)  
#define UM_REMOTECONTROL        (WM_USER+210)  
#define UM_SETGAMMARAMP         (WM_USER+203)  
#define UM_CREATERESOLUTION     (WM_USER+204)  
#define UM_GETTIMING            (WM_USER+205) 
#define UM_GETSETCLOCKS         (WM_USER+206)
#define UM_SETCUSTOMTIMINGFAST  (WM_USER+211) // glitches vertical sync with PS 3.65 build 568
 
#define NegativeHorizontalPolarity      0x02
#define NegativeVerticalPolarity        0x04
#define Interlace			0x08

#define HideTrayIcon                    0x00
#define ShowTrayIcon                    0x01
#define ClosePowerStrip                 0x63

typedef struct
{
    int HorizontalActivePixels;
    int HorizontalFrontPorch;
    int HorizontalSyncWidth;
    int HorizontalBackPorch;
    int VerticalActivePixels;
    int VerticalFrontPorch;
    int VerticalSyncWidth;
    int VerticalBackPorch;
    int PixelClockInKiloHertz;
    union
    {
        int w;
        struct
        {
            unsigned :1;
            unsigned HorizontalPolarityNegative:1;
            unsigned VerticalPolarityNegative:1;
            unsigned :29;
        } b;
    } TimingFlags;
} MonitorTiming;

