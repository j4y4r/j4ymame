//http://forums.entechtaiwan.com/index.php?topic=5534.msg20902;topicseen#msg20902

// standard windows headers
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

// MAME headers
#include "emu.h"
#include "clifront.h"

// groovyMAME headers
#include "pstrip.h"

//============================================================
//  GLOBALS
//============================================================

static HWND hPSWnd;
static MonitorTiming timing_backup;

//============================================================
//  PROTOTYPES
//============================================================

int ps_init(int monitor_index);
int ps_reset(int monitor_index);
int ps_get_modeline(int monitor_index, ModeLine *modeline);
int ps_set_modeline(int monitor_index, ModeLine *modeline);
int ps_get_monitor_timing(int monitor_index, MonitorTiming *timing);
int ps_set_monitor_timing(int monitor_index, MonitorTiming *timing);
int ps_set_monitor_timing_string(int monitor_index, char *in);
int ps_set_refresh(int monitor_index, double vfreq);
int ps_create_resolution(int monitor_index, ModeLine *modeline);
static void read_timing_string(char *in, MonitorTiming *timing);
static void fill_timing_string(char *out, MonitorTiming *timing);
static int modeline_to_pstiming(ModeLine *modeline, MonitorTiming *timing);
static int pstiming_to_modeline(MonitorTiming *timing, ModeLine *modeline);
int ps_monitor_index (const char *display_name);

//============================================================
//  ps_init
//============================================================

int ps_init(int monitor_index)
{
	hPSWnd = FindWindowA("TPShidden", NULL);
	
	if (hPSWnd)
	{
		mame_printf_verbose("PStrip: PowerStrip found!\n");
		ps_get_monitor_timing(monitor_index, &timing_backup);
		return 1;
	}
	else
	{
		mame_printf_verbose("PStrip: Could not get PowerStrip API interface\n");
		return 0;
	}	
}

//============================================================
//  ps_reset
//============================================================

int ps_reset(int monitor_index)
{
	return ps_set_monitor_timing(monitor_index, &timing_backup);
}

//============================================================
//  ps_get_modeline
//============================================================

int ps_get_modeline(int monitor_index, ModeLine *modeline)
{
	MonitorTiming timing = {0};

	if (ps_get_monitor_timing(monitor_index, &timing))
	{
		pstiming_to_modeline(&timing, modeline);
		return 1;
	}
	else return 0;
}

//============================================================
//  ps_set_modeline
//============================================================

int ps_set_modeline(int monitor_index, ModeLine *modeline)
{
	MonitorTiming timing = {0};

	modeline_to_pstiming(modeline, &timing);

	if (ps_set_monitor_timing(monitor_index, &timing))
		return 1;
	else
		return 0;
}

//============================================================
//  ps_get_monitor_timing
//============================================================

int ps_get_monitor_timing(int monitor_index, MonitorTiming *timing)
{
	LRESULT lresult;
	char in[256];
	
	if (!hPSWnd) return 0;
	
	lresult = SendMessage(hPSWnd, UM_GETTIMING, monitor_index, 0);
	
	if (lresult == -1)
	{
		mame_printf_verbose("PStrip: Could not get PowerStrip timing string\n");
		return 0;
	}
	
	if (!GlobalGetAtomNameA(lresult, in, sizeof(in)))
	{
		mame_printf_verbose("PStrip: GlobalGetAtomName failed\n");
		return 0;
	}
	
	mame_printf_verbose("PStrip: ps_get_monitor_timing(%d): %s\n", monitor_index, in);
	
	read_timing_string(in, timing);
	
	GlobalDeleteAtom(lresult); // delete atom created by PowerStrip

	return 1;
}

//============================================================
//  ps_set_monitor_timing
//============================================================

int ps_set_monitor_timing(int monitor_index, MonitorTiming *timing)
{
	LRESULT lresult;
	ATOM atom;
	char out[256];
	
	if (!hPSWnd) return 0;
		
	fill_timing_string(out, timing);
	atom = GlobalAddAtomA(out);
	
	if (atom)
	{
		lresult = SendMessage(hPSWnd, UM_SETCUSTOMTIMING, monitor_index, atom);
		
		if (lresult < 0)
		{
			mame_printf_verbose("PStrip: SendMessage failed\n");
			GlobalDeleteAtom(atom);
		}
		else
		{
			mame_printf_verbose("PStrip: ps_set_monitor_timing(%d): %s\n", monitor_index, out);
			return 1;
		}
	}
	else mame_printf_verbose("PStrip: ps_set_monitor_timing atom creation failed\n");

	return 0;
}

//============================================================
//  ps_set_monitor_timing_string
//============================================================

int ps_set_monitor_timing_string(int monitor_index, char *in)
{
	MonitorTiming timing;

	read_timing_string(in, &timing);
	return ps_set_monitor_timing(monitor_index, &timing);
}

//============================================================
//  ps_set_refresh
//============================================================

int ps_set_refresh(int monitor_index, double vfreq)
{
	MonitorTiming timing = {0};
	int i, hht, vvt, new_vvt;
	int desired_pClock;
	int best_pClock = 0;
	
	memcpy(&timing, &timing_backup, sizeof(MonitorTiming));

	hht = timing.HorizontalActivePixels
		+ timing.HorizontalFrontPorch
		+ timing.HorizontalSyncWidth
		+ timing.HorizontalBackPorch;
	
	vvt = timing.VerticalActivePixels
		+ timing.VerticalFrontPorch
		+ timing.VerticalSyncWidth
		+ timing.VerticalBackPorch;

	desired_pClock = hht * vvt * vfreq / 1000;

	mame_printf_verbose("PStrip: ps_set_refresh(%d) %f Hz, getting stable dotclocks for %d...\n", monitor_index, vfreq, desired_pClock);
		
	for (i = -50; i <= 50; i += 25)
	{
		timing.PixelClockInKiloHertz = desired_pClock + i;
	
		ps_set_monitor_timing(monitor_index, &timing);
		ps_get_monitor_timing(monitor_index, &timing);
		
		if (abs(timing.PixelClockInKiloHertz - desired_pClock) < abs(desired_pClock - best_pClock))
			
			best_pClock = timing.PixelClockInKiloHertz;
	}
	
	mame_printf_verbose("PStrip: ps_set_refresh(%d), new dotclock: %d\n", monitor_index, best_pClock);

	new_vvt = best_pClock * 1000 / (vfreq * hht);
	
	timing.VerticalBackPorch += (new_vvt - vvt);
	timing.PixelClockInKiloHertz = best_pClock;
	
	ps_set_monitor_timing(monitor_index, &timing);
	ps_get_monitor_timing(monitor_index, &timing);
	
	return 1;
}

//============================================================
//  ps_create_resolution
//============================================================

int ps_create_resolution(int monitor_index, ModeLine *modeline)
{
	LRESULT     lresult;
	ATOM        atom;
	char        out[256];
	MonitorTiming timing = {0};
	
	if (!hPSWnd) return 0;
	
	modeline_to_pstiming(modeline, &timing);
	
	fill_timing_string(out, &timing);
	atom = GlobalAddAtomA(out);
	
	if (atom)
	{
		lresult = SendMessage(hPSWnd, UM_CREATERESOLUTION, monitor_index, atom);
		
		if (lresult < 0)
        	{
        		mame_printf_verbose("PStrip: SendMessage failed\n");
        		GlobalDeleteAtom(atom);
        	}
        	else
        	{
        		mame_printf_verbose("PStrip: ps_create_resolution(%d): %dx%d succeded \n",
        			modeline->a_width, modeline->a_height, monitor_index);
        		return 1;
        	}
        }
        else mame_printf_verbose("PStrip: ps_create_resolution atom creation failed\n");
	
	return 0;
}

//============================================================
//  read_timing_string
//============================================================

static void read_timing_string(char *in, MonitorTiming *timing)
{
	sscanf(in,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
		&timing->HorizontalActivePixels,
		&timing->HorizontalFrontPorch,
		&timing->HorizontalSyncWidth,
		&timing->HorizontalBackPorch,
		&timing->VerticalActivePixels,
		&timing->VerticalFrontPorch,
		&timing->VerticalSyncWidth,
		&timing->VerticalBackPorch,
		&timing->PixelClockInKiloHertz,
		&timing->TimingFlags.w);
}

//============================================================
//  fill_timing_string
//============================================================

static void fill_timing_string(char *out, MonitorTiming *timing)
{
	sprintf(out, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
		timing->HorizontalActivePixels,
		timing->HorizontalFrontPorch,
		timing->HorizontalSyncWidth,
		timing->HorizontalBackPorch,
		timing->VerticalActivePixels,
		timing->VerticalFrontPorch,
		timing->VerticalSyncWidth,
		timing->VerticalBackPorch,
		timing->PixelClockInKiloHertz,
		timing->TimingFlags.w);
}

//============================================================
//  modeline_to_pstiming
//============================================================

static int modeline_to_pstiming(ModeLine *modeline, MonitorTiming *timing)
{
	timing->HorizontalActivePixels = modeline->hactive;
	timing->HorizontalFrontPorch = modeline->hbegin - modeline->hactive;
	timing->HorizontalSyncWidth = modeline->hend - modeline->hbegin;
	timing->HorizontalBackPorch = modeline->htotal - modeline->hend;
	
	timing->VerticalActivePixels = modeline->vactive;
	timing->VerticalFrontPorch = modeline->vbegin - modeline->vactive;
	timing->VerticalSyncWidth = modeline->vend - modeline->vbegin;
	timing->VerticalBackPorch = modeline->vtotal - modeline->vend;
	
	timing->PixelClockInKiloHertz = modeline->pclock / 1000;
	
	if (modeline->hsync == 0)
		timing->TimingFlags.w |= NegativeHorizontalPolarity;
	if (modeline->vsync == 0)
		timing->TimingFlags.w |= NegativeVerticalPolarity;
	if (modeline->interlace)
		timing->TimingFlags.w |= Interlace;

	return 0;
}

//============================================================
//  pstiming_to_modeline
//============================================================

static int pstiming_to_modeline(MonitorTiming *timing, ModeLine *modeline)
{
	modeline->hactive = timing->HorizontalActivePixels;
	modeline->hbegin = modeline->hactive + timing->HorizontalFrontPorch;
	modeline->hend = modeline->hbegin + timing->HorizontalSyncWidth;
	modeline->htotal = modeline->hend + timing->HorizontalBackPorch;
	
	modeline->vactive = timing->VerticalActivePixels;
	modeline->vbegin = modeline->vactive + timing->VerticalFrontPorch;
	modeline->vend = modeline->vbegin + timing->VerticalSyncWidth;
	modeline->vtotal = modeline->vend + timing->VerticalBackPorch;
	
	modeline->a_width = modeline->hactive;
	modeline->a_height = modeline->vactive;
	
	sprintf(modeline->name, "Modeline %dx%d", modeline->hactive, modeline->vactive);
	
	modeline->pclock = timing->PixelClockInKiloHertz * 1000;
	
	if (!(timing->TimingFlags.w & NegativeHorizontalPolarity))
		modeline->hsync = 1;
		
	if (!(timing->TimingFlags.w & NegativeVerticalPolarity))
		modeline->vsync = 1;
		
	if ((timing->TimingFlags.w & Interlace))
		modeline->interlace = 1;	

	return 0;
}

//============================================================
//  pstiming_to_modeline
//============================================================

int ps_monitor_index (const char *display_name)
{
	int monitor_index = 0;
	char sub_index[2];
	
	sub_index[0] = display_name[strlen(display_name)-1];
	sub_index[1] = 0;
	if (sscanf(sub_index,"%d", &monitor_index) == 1)
		monitor_index --;
		
	return monitor_index;
}