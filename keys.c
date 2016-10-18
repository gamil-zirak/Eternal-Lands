#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <ctype.h>
#include <SDL_keycode.h>
#include "errors.h"
#include "keys.h"
#include "asc.h"
#include "init.h"
#include "misc.h"
#ifdef FASTER_MAP_LOAD
#include "io/elfilewrapper.h"
#endif

// default definitions for keys
Uint32 K_QUIT=ALT|'x';
#ifdef WINDOWS
// Windows SDL reports [Alt Gr] as [Ctrl], which hinders German users typing '@',
// so don't use Ctrl-q as a symbol to exit
Uint32 K_QUIT_ALT=ALT|'x';
#elif OSX
Uint32 K_QUIT_ALT=KMOD_LGUI|'q';
#else
Uint32 K_QUIT_ALT=CTRL|'q';
#endif
Uint32 K_CAMERAUP=SDLK_UP;
Uint32 K_CAMERADOWN=SDLK_DOWN;
Uint32 K_ZOOMOUT=SDLK_PAGEDOWN;
Uint32 K_ZOOMIN=SDLK_PAGEUP;
Uint32 K_TURNLEFT=SDLK_INSERT;
Uint32 K_TURNRIGHT=SDLK_DELETE;
Uint32 K_ADVANCE=SDLK_HOME;
Uint32 K_HEALTHBAR=ALT|'h';
Uint32 K_VIEWNAMES=ALT|'n';
Uint32 K_VIEWHP=ALT|'b';
Uint32 K_STATS=CTRL|'a';
Uint32 K_QUESTLOG=CTRL|'g';
Uint32 K_SESSION=CTRL|'z';
Uint32 K_WALK=CTRL|'w';
Uint32 K_LOOK=CTRL|'l';
Uint32 K_USE=CTRL|'u';
Uint32 K_OPTIONS=CTRL|'o';
Uint32 K_REPEATSPELL=CTRL|'r';
Uint32 K_SIGILS=CTRL|'s';
Uint32 K_MANUFACTURE=CTRL|'m';
Uint32 K_ITEMS=CTRL|'i';
Uint32 K_MAP=SDLK_TAB;
Uint32 K_MINIMAP=ALT|'m';
Uint32 K_ROTATELEFT=SDLK_LEFT;
Uint32 K_ROTATERIGHT=SDLK_RIGHT;
Uint32 K_FROTATELEFT=SHIFT|SDLK_LEFT;
Uint32 K_FROTATERIGHT=SHIFT|SDLK_RIGHT;
Uint32 K_BROWSER=SDLK_F2;
Uint32 K_BROWSERWIN=CTRL|SDLK_F2;
Uint32 K_ESCAPE=SDLK_ESCAPE;
Uint32 K_CONSOLE=SDLK_F1;
Uint32 K_SHADOWS=SDLK_F3;
Uint32 K_KNOWLEDGE=CTRL|'k';
Uint32 K_ENCYCLOPEDIA=CTRL|'e';
Uint32 K_HELP=CTRL|'h';
Uint32 K_NOTEPAD=CTRL|'n';
Uint32 K_HIDEWINS=ALT|'d';
Uint32 K_ITEM1=CTRL|'1';
Uint32 K_ITEM2=CTRL|'2';
Uint32 K_ITEM3=CTRL|'3';
Uint32 K_ITEM4=CTRL|'4';
Uint32 K_ITEM5=CTRL|'5';
Uint32 K_ITEM6=CTRL|'6';
Uint32 K_ITEM7=CTRL|'7';
Uint32 K_ITEM8=CTRL|'8';
Uint32 K_ITEM9=CTRL|'9';
Uint32 K_ITEM10=CTRL|'0';
Uint32 K_ITEM11=CTRL|'-';
Uint32 K_ITEM12=CTRL|'=';
Uint32 K_SCREENSHOT=CTRL|'p';
Uint32 K_VIEWTEXTASOVERTEXT=ALT|'o';
Uint32 K_AFK=CTRL|ALT|'a';
Uint32 K_SIT=ALT|'s';
Uint32 K_RANGINGLOCK=ALT|'r';
Uint32 K_BUDDY=CTRL|'b';
Uint32 K_NEXT_CHAT_TAB=CTRL|SDLK_PAGEDOWN;
Uint32 K_PREV_CHAT_TAB=CTRL|SDLK_PAGEUP;
Uint32 K_RULES=SDLK_F5;
Uint32 K_SPELL1=ALT|'1';
Uint32 K_SPELL2=ALT|'2';
Uint32 K_SPELL3=ALT|'3';
Uint32 K_SPELL4=ALT|'4';
Uint32 K_SPELL5=ALT|'5';
Uint32 K_SPELL6=ALT|'6';
Uint32 K_SPELL7=ALT|'7';
Uint32 K_SPELL8=ALT|'8';
Uint32 K_SPELL9=ALT|'9';
Uint32 K_SPELL10=ALT|'0';
Uint32 K_SPELL11=ALT|'-';
Uint32 K_SPELL12=ALT|'=';
Uint32 K_TABCOMPLETE=CTRL|' ';
Uint32 K_WINDOWS_ON_TOP=ALT|'w';
Uint32 K_MARKFILTER=CTRL|'f';
Uint32 K_OPAQUEWIN=CTRL|'d';
Uint32 K_GRAB_MOUSE=ALT|'g';
Uint32 K_FIRST_PERSON=ALT|'f';
Uint32 K_EXTEND_CAM=ALT|'e';
#ifndef OSX
Uint32 K_CUT=CTRL|'x';
Uint32 K_COPY=CTRL|'c';
Uint32 K_PASTE=CTRL|'v';
#else
Uint32 K_CUT=KMOD_LGUI|'x';
Uint32 K_COPY=KMOD_LGUI|'c';
Uint32 K_PASTE=KMOD_LGUI|'v';
#endif
Uint32 K_COPY_ALT=CTRL|SDLK_INSERT;
Uint32 K_PASTE_ALT=SHIFT|SDLK_INSERT;
#ifdef ECDEBUGWIN
Uint32 K_ECDEBUGWIN=ALT|CTRL|'c';
#endif
Uint32 K_EMOTES=CTRL|'j';
Uint32 K_RANGINGWIN=CTRL|'t';

// Remaining keys are not assigned to the keyboard but
// can be redefined or used by the #keypress command.
// They will get values at startup along with any keys
// undefined in key.ini
Uint32 K_COUNTERS=SDLK_UNKNOWN;
Uint32 K_HELPSKILLS=SDLK_UNKNOWN;

typedef struct
{
	char name[25];
	Uint32 *value;
} key_store_entry;

static key_store_entry key_store[] =
{
	{ "#K_QUIT", &K_QUIT },
	{ "#K_QUIT_ALT", &K_QUIT_ALT },
	{ "#K_CAMERAUP", &K_CAMERAUP },
	{ "#K_CAMERADOWN", &K_CAMERADOWN },
	{ "#K_ZOOMOUT", &K_ZOOMOUT },
	{ "#K_ZOOMIN", &K_ZOOMIN },
	{ "#K_TURNLEFT", &K_TURNLEFT },
	{ "#K_TURNRIGHT", &K_TURNRIGHT },
	{ "#K_ADVANCE", &K_ADVANCE },
	{ "#K_HEALTHBAR", &K_HEALTHBAR },
	{ "#K_VIEWNAMES", &K_VIEWNAMES },
	{ "#K_VIEWHP", &K_VIEWHP },
	{ "#K_STATS", &K_STATS },
	{ "#K_QUESTLOG", &K_QUESTLOG },
	{ "#K_SESSION", &K_SESSION },
	{ "#K_WALK", &K_WALK },
	{ "#K_LOOK", &K_LOOK },
	{ "#K_USE", &K_USE },
	{ "#K_OPTIONS", &K_OPTIONS },
	{ "#K_REPEATSPELL", &K_REPEATSPELL },
	{ "#K_SIGILS", &K_SIGILS },
	{ "#K_MANUFACTURE", &K_MANUFACTURE },
	{ "#K_ITEMS", &K_ITEMS },
	{ "#K_MAP", &K_MAP },
	{ "#K_MINIMAP", &K_MINIMAP },
	{ "#K_ROTATELEFT", &K_ROTATELEFT },
	{ "#K_ROTATERIGHT", &K_ROTATERIGHT },
	{ "#K_FROTATELEFT", &K_FROTATELEFT },
	{ "#K_FROTATERIGHT", &K_FROTATERIGHT },
	{ "#K_BROWSER", &K_BROWSER },
	{ "#K_BROWSERWIN", &K_BROWSERWIN },
	{ "#K_ESCAPE", &K_ESCAPE },
	{ "#K_CONSOLE", &K_CONSOLE },
	{ "#K_SHADOWS", &K_SHADOWS },
	{ "#K_KNOWLEDGE", &K_KNOWLEDGE },
	{ "#K_ENCYCLOPEDIA", &K_ENCYCLOPEDIA },
	{ "#K_HELP", &K_HELP },
	{ "#K_NOTEPAD", &K_NOTEPAD },
	{ "#K_HIDEWINS", &K_HIDEWINS },
	{ "#K_ITEM1", &K_ITEM1 },
	{ "#K_ITEM2", &K_ITEM2 },
	{ "#K_ITEM3", &K_ITEM3 },
	{ "#K_ITEM4", &K_ITEM4 },
	{ "#K_ITEM5", &K_ITEM5 },
	{ "#K_ITEM6", &K_ITEM6 },
	{ "#K_ITEM7", &K_ITEM7 },
	{ "#K_ITEM8", &K_ITEM8 },
	{ "#K_ITEM9", &K_ITEM9 },
	{ "#K_ITEM10", &K_ITEM10 },
	{ "#K_ITEM11", &K_ITEM11 },
	{ "#K_ITEM12", &K_ITEM12 },
	{ "#K_SCREENSHOT", &K_SCREENSHOT },
	{ "#K_VIEWTEXTASOVERTEXT", &K_VIEWTEXTASOVERTEXT },
	{ "#K_AFK", &K_AFK },
	{ "#K_SIT", &K_SIT },
	{ "#K_RANGINGLOCK", &K_RANGINGLOCK },
	{ "#K_BUDDY", &K_BUDDY },
	{ "#K_NEXT_CHAT_TAB", &K_NEXT_CHAT_TAB },
	{ "#K_PREV_CHAT_TAB", &K_PREV_CHAT_TAB },
	{ "#K_RULES", &K_RULES },
	{ "#K_SPELL1", &K_SPELL1 },
	{ "#K_SPELL2", &K_SPELL2 },
	{ "#K_SPELL3", &K_SPELL3 },
	{ "#K_SPELL4", &K_SPELL4 },
	{ "#K_SPELL5", &K_SPELL5 },
	{ "#K_SPELL6", &K_SPELL6 },
	{ "#K_SPELL7", &K_SPELL7 },
	{ "#K_SPELL8", &K_SPELL8 },
	{ "#K_SPELL9", &K_SPELL9 },
	{ "#K_SPELL10", &K_SPELL10 },
	{ "#K_SPELL11", &K_SPELL11 },
	{ "#K_SPELL12", &K_SPELL12 },
	{ "#K_TABCOMPLETE", &K_TABCOMPLETE },
	{ "#K_WINDOWS_ON_TOP", &K_WINDOWS_ON_TOP },
	{ "#K_MARKFILTER", &K_MARKFILTER },
	{ "#K_OPAQUEWIN", &K_OPAQUEWIN },
	{ "#K_GRAB_MOUSE", &K_GRAB_MOUSE },
	{ "#K_FIRST_PERSON", &K_FIRST_PERSON },
	{ "#K_EXTEND_CAM", &K_EXTEND_CAM },
	{ "#K_CUT", &K_CUT },
	{ "#K_COPY", &K_COPY },
	{ "#K_PASTE", &K_PASTE },
	{ "#K_COPY_ALT", &K_COPY_ALT },
	{ "#K_PASTE_ALT", &K_PASTE_ALT },
#ifdef ECDEBUGWIN
	{ "#K_ECDEBUGWIN", &K_ECDEBUGWIN },
#endif
	{ "#K_EMOTES", &K_EMOTES },
	{ "#K_RANGINGWIN", &K_RANGINGWIN },
	{ "#K_COUNTERS", &K_COUNTERS },
	{ "#K_HELPSKILLS", &K_HELPSKILLS }
};


Uint32 get_key_value(const char* name)
{
	size_t num_keys = sizeof(key_store)/sizeof(key_store_entry);
	size_t i;
	if ((name == NULL) || strlen(name) == 0)
	{
		LOG_ERROR("%s() empty name\n", __FUNCTION__);
		return 0;
	}
	for (i=0; i<num_keys; i++)
	{
		if (strcasecmp(name, key_store[i].name) == 0)
			return *key_store[i].value;
	}
	return 0;
}

static void add_key(Uint32 *key, Uint32 n)
{
	switch (n)
	{
		case SDLK_LSHIFT:
		case SDLK_RSHIFT:
			*key |= SHIFT;
			break;
		case SDLK_LCTRL:
		case SDLK_RCTRL:
			*key |= CTRL;
			break;
		case SDLK_LALT:
		case SDLK_RALT:
			*key |= ALT;
			break;
		default:
//			*key = (n & 0xFFFF) | (*key &0xFFFF0000);
			*key = n | (*key &0xFFFF0000);
        }
}

static Uint32 CRC32(const char *data, int len)
{
	unsigned int result = 0;
	int i, j;
	unsigned char octet;

	for (i = 0; i < len; i++)
	{
		octet = *(data++);
		for (j = 0; j < 8; j++)
		{
			if ((octet >> 7) ^ (result >> 31))
				result = (result << 1) ^ 0x04c11db7;
			else
				result = (result << 1);
			octet <<= 1;
		}
	}

	return ~result;
}

static Uint32 get_key_code(const char *key)
{
	int len = strlen(key);

	if (len==1)
	{
		return tolower(key[0]);
	}
	else
	{
		Uint32 crc = CRC32(key,len);
		switch(crc){
			case 0x414243d2: //UP
				return SDLK_UP;
			case 0x8b9c5c32: //F1
				return SDLK_F1;
			case 0x86df7aeb: //F2
				return SDLK_F2;
			case 0x821e675c: //F3
				return SDLK_F3;
			case 0x9c593759: //F4
				return SDLK_F4;
			case 0x98982aee: //F5
				return SDLK_F5;
			case 0x95db0c37: //F6
				return SDLK_F6;
			case 0x911a1180: //F7
				return SDLK_F7;
			case 0xa955ac3d: //F8
				return SDLK_F8;
			case 0xad94b18a: //F9
				return SDLK_F9;
			case 0xbbde3454: //F10
				return SDLK_F10;
			case 0xbf1f29e3: //F11
				return SDLK_F11;
			case 0xb25c0f3a: //F12
				return SDLK_F12;
			case 0xb69d128d: //F13
				return SDLK_F13;
			case 0xa8da4288: //F14
				return SDLK_F14;
			case 0xac1b5f3f: //F15
				return SDLK_F15;
			case 0xe5b332af: //BACKSPACE
				return SDLK_BACKSPACE;
			case 0x3d6742da: //TAB
				return SDLK_TAB;
			case 0xe4f512ce: //CLEAR
				return SDLK_CLEAR;
			case 0xe5c642f: //RETURN
				return SDLK_RETURN;
			case 0x1a3dbcf4: //PAUSE
				return SDLK_PAUSE;
			case 0xb23e322f: //ESCAPE
				return SDLK_ESCAPE;
			case 0xe0ea4208: //SPACE
				return SDLK_SPACE;
			case 0x3f048816: //DELETE
				return SDLK_DELETE;
			case 0x5dd541: //KP0
				return SDLK_KP_0;
			case 0x49cc8f6: //KP1
				return SDLK_KP_1;
			case 0x9dfee2f: //KP2
				return SDLK_KP_2;
			case 0xd1ef398: //KP3
				return SDLK_KP_3;
			case 0x1359a39d: //KP4
				return SDLK_KP_4;
			case 0x1798be2a: //KP5
				return SDLK_KP_5;
			case 0x1adb98f3: //KP6
				return SDLK_KP_6;
			case 0x1e1a8544: //KP7
				return SDLK_KP_7;
			case 0x265538f9: //KP8
				return SDLK_KP_8;
			case 0x2294254e: //KP9
				return SDLK_KP_9;
			case 0xc9681663: //KP_PERIOD
				return SDLK_KP_PERIOD;
			case 0xf2032002: //KP_DIVIDE
				return SDLK_KP_DIVIDE;
			case 0xc69c9177: //KP_MULTIPLY
				return SDLK_KP_MULTIPLY;
			case 0xe05a3b75: //KP_MINUS
				return SDLK_KP_MINUS;
			case 0x7a14ede0: //KP_PLUS
				return SDLK_KP_PLUS;
			case 0xb95fb1fa: //KP_ENTER
				return SDLK_KP_ENTER;
			case 0x997d27b6: //KP_EQUALS
				return SDLK_KP_EQUALS;
			case 0x412c789a: //DOWN
				return SDLK_DOWN;
			case 0xcfd43bcf: //RIGHT
				return SDLK_RIGHT;
			case 0x14618acf: //LEFT
				return SDLK_LEFT;
			case 0xb448467c: //INSERT
				return SDLK_INSERT;
			case 0xd59321ba: //HOME
				return SDLK_HOME;
			case 0x863456b7: //END
				return SDLK_END;
			case 0xd541afe1: //PAGEUP
				return SDLK_PAGEUP;
			case 0x77a53c61: //PAGEDOWN
				return SDLK_PAGEDOWN;
			case 0x8563dfd4: //NUMLOCK
				return SDLK_NUMLOCKCLEAR;
			case 0x4b601de5: //CAPSLOCK
				return SDLK_CAPSLOCK;
			case 0x7b642f: //SCROLLOCK
				return SDLK_SCROLLLOCK;
			case 0x6fa8765e: //RSHIFT
				return SDLK_RSHIFT;
			case 0x5a59f8b9: //LSHIFT
				return SDLK_LSHIFT;
			case 0xc535c663: //RCTRL
				return SDLK_RCTRL;
			case 0xb5e083f0: //LCTRL
				return SDLK_LCTRL;
			case 0xf7a834fb: //RALT
				return SDLK_RALT;
			case 0x39b9e58e: //LALT
				return SDLK_LALT;
			case 0x34796737: //RMETA
				return SDLK_RGUI;
			case 0x44ac22a4: //LMETA
				return SDLK_LGUI;
			case 0x2d5a7586: //MODE
				return SDLK_MODE;
			case 0x512a6d4b: //HELP
				return SDLK_HELP;
			case 0xdc87c39e: //PRINT
				return SDLK_PRINTSCREEN;
			case 0xbdf2d984: //SYSREQ
				return SDLK_SYSREQ;
			case 0x46854e9d: //MENU
				return SDLK_MENU;
			case 0x8758b6ec: //POWER
				return SDLK_POWER;
			case 0xdf6ba7e: //UNDO
				return SDLK_UNDO;
			default:
				return SDLK_UNKNOWN;
		}
	}
}

static void parse_key_line(const char *line)
{
	char kstr[100], t1[100], t2[100], t3[100], t4[100];
	Uint32 key = 0;
	int nkey = sscanf(line, " #K_%99s = %99s %99s %99s %99s", kstr,
		t1, t2, t3, t4);
	size_t num_keys = sizeof(key_store)/sizeof(key_store_entry);
	size_t i;

	if (nkey <= 1)
		return;

	add_key(&key, get_key_code(t1));
	if (nkey > 2 && t2[0] != '#')
	{
		add_key(&key, get_key_code(t2));
		if (nkey > 3 && t3[0] != '#')
		{
			add_key(&key, get_key_code(t3));
			if (nkey > 4 && t4[0] != '#')
			{
				add_key(&key, get_key_code(t4));
			}
		}
	}

	for (i=0; i<num_keys; i++)
		if (strcasecmp(kstr, &key_store[i].name[3]) == 0) // skip "#K_"
		{
			*key_store[i].value = key;
			break;
		}
}

// load the dynamic definitions for keys
void read_key_config()
{
	char line[512];
	el_file_ptr f;
	size_t num_keys = sizeof(key_store)/sizeof(key_store_entry);
	Uint32 last_key_value = SDLK_SLEEP;
	size_t i;

	f = el_open_custom("key.ini");
	if (f)
	{
		while (el_fgets(line, sizeof(line), f))
			parse_key_line(line);
		el_close(f);
	}

	// look for unassigned keys and assign one up from SDLK_SLEEP (SDLK_LAST no longer exists, SDLK_SLEEP is the last in the list)
	for (i=0; i<num_keys; i++)
		if (*key_store[i].value == 0)
			*key_store[i].value = ++last_key_value;
}

// Returns (in the buffer provided) a string describing the specified keydef.
const char *get_key_string(Uint32 keydef, char *buf, size_t buflen)
{
	char base = keydef & 0xFF;
	char *mod = "";
	if (keydef & CTRL)
		mod = "ctrl-";
	else if (keydef & ALT)
		mod = "alt-";
	else if (keydef & SHIFT)
		mod = "shift-";
	safe_snprintf(buf, buflen, "%s%c", mod, base);
	return buf;
}
