#include <ultra64.h>
#include "n_libaudio.h"
#include "constants.h"
#include "game/file.h"
#include "game/lv.h"
#include "game/music.h"
#include "bss.h"
#include "lib/rzip.h"
#include "lib/args.h"
#include "lib/dma.h"
#include "lib/snd.h"
#include "lib/memp.h"
#include "lib/rng.h"
#include "lib/lib_2fc60.h"
#include "lib/lib_317f0.h"
#include "lib/mp3.h"
#include "lib/lib_39c80.h"
#include "data.h"
#include "types.h"

struct curmp3 {
	union soundnumhack sfxref;
	u32 playing;
	u32 unk08;
	s32 responsetimer240;
	s32 prevwhisper;
	s32 prevacknowledge;
	s32 prevgreeting;
	u32 romaddr;
	u32 romsize;
	s32 responsetype;
};

s32 g_NumSounds;
u32 *g_ALSoundRomOffsets;
s32 g_SndMaxFxBusses;
u32 var80094eac;
struct curmp3 g_SndCurMp3;
struct seqinstance g_SeqInstances[3];
ALHeap g_SndHeap;
u32 var80095200;
ALBank *var80095204;
struct seqtable *g_SeqTable;
u32 g_SeqBufferSize;
struct sndcache g_SndCache;

const char g_SndGuardString[] = "RUSSES SOUND GUARD STRING";
const char var70053b3c[] = "Snd: SoundHeaderCacheInit\n";

bool g_SndDisabled = false;
u32 var8005dda4 = 0x00000000;

s32 g_SndNosediveVolume = 0;
s32 g_SndNosediveAge240 = -1;
s32 g_SndNosediveDuration240 = 0;

s32 g_SndUfoVolume = 0;
s32 g_SndUfoAge240 = -1;
s32 g_SndUfoDuration240 = 0;

struct sndstate *g_SndNosediveHandle = NULL;
struct sndstate *g_SndUfoHandle = NULL;

u16 g_SfxVolume = 32767;
s32 g_SoundMode = (VERSION >= VERSION_NTSC_1_0 ? SOUNDMODE_STEREO : SOUNDMODE_SURROUND);
bool g_SndMp3Enabled = false;

#if VERSION >= VERSION_NTSC_1_0
s32 var8005ddd4 = 0;
s32 var8005ddd8 = 0;
s32 var8005dddc = 0;
#endif

u8 *g_SndGuardStringPtr = NULL;

enum audioconfig_e {
	AUDIOCONFIG_00,
	AUDIOCONFIG_01,
	AUDIOCONFIG_02,
	AUDIOCONFIG_03,
	AUDIOCONFIG_04,
	AUDIOCONFIG_05,
	AUDIOCONFIG_06,
	AUDIOCONFIG_07,
	AUDIOCONFIG_08,
	AUDIOCONFIG_09,
	AUDIOCONFIG_10,
	AUDIOCONFIG_11,
	AUDIOCONFIG_12,
	AUDIOCONFIG_13,
	AUDIOCONFIG_14,
	AUDIOCONFIG_15,
	AUDIOCONFIG_16,
	AUDIOCONFIG_17,
	AUDIOCONFIG_18,
	AUDIOCONFIG_19,
	AUDIOCONFIG_20,
	AUDIOCONFIG_21,
	AUDIOCONFIG_22,
	AUDIOCONFIG_23,
	AUDIOCONFIG_24,
	AUDIOCONFIG_25,
	AUDIOCONFIG_26,
	AUDIOCONFIG_27,
	AUDIOCONFIG_28,
	AUDIOCONFIG_29,
	AUDIOCONFIG_30,
	AUDIOCONFIG_31,
	AUDIOCONFIG_32,
	AUDIOCONFIG_33,
	AUDIOCONFIG_34,
	AUDIOCONFIG_35,
	AUDIOCONFIG_36,
	AUDIOCONFIG_37,
	AUDIOCONFIG_38,
	AUDIOCONFIG_39,
	AUDIOCONFIG_40,
	AUDIOCONFIG_41,
	AUDIOCONFIG_42,
	AUDIOCONFIG_43,
	AUDIOCONFIG_44,
	AUDIOCONFIG_45,
	AUDIOCONFIG_46,
	AUDIOCONFIG_47,
	AUDIOCONFIG_48,
	AUDIOCONFIG_49,
	AUDIOCONFIG_50,
	AUDIOCONFIG_51,
	AUDIOCONFIG_52,
	AUDIOCONFIG_53,
#if VERSION >= VERSION_NTSC_1_0
	AUDIOCONFIG_54,
#endif
	AUDIOCONFIG_55,
	AUDIOCONFIG_56,
	AUDIOCONFIG_57,
	AUDIOCONFIG_58,
	AUDIOCONFIG_59,
#if VERSION >= VERSION_NTSC_1_0
	AUDIOCONFIG_60,
	AUDIOCONFIG_61,
	AUDIOCONFIG_62,
	AUDIOCONFIG_63
#else
	AUDIOCONFIG_60
#endif
};

// 3e04
struct audiorussmapping g_AudioRussMappings[] = {
	/*0x0000*/ { 0x85ba, AUDIOCONFIG_59 },
	/*0x0001*/ { 0x85a5, AUDIOCONFIG_15 },
	/*0x0002*/ { 0x85a6, AUDIOCONFIG_15 },
	/*0x0003*/ { 0x80bb, AUDIOCONFIG_14 },
	/*0x0004*/ { 0x80bc, AUDIOCONFIG_14 },
	/*0x0005*/ { 0x8007, AUDIOCONFIG_14 },
	/*0x0006*/ { 0x8007, AUDIOCONFIG_14 },
	/*0x0007*/ { 0x80be, AUDIOCONFIG_14 },
	/*0x0008*/ { 0x80bf, AUDIOCONFIG_14 },
	/*0x0009*/ { 0x80bf, AUDIOCONFIG_14 },
	/*0x000a*/ { 0x80c0, AUDIOCONFIG_14 },
	/*0x000b*/ { 0x8007, AUDIOCONFIG_14 },
	/*0x000c*/ { 0x80c2, AUDIOCONFIG_14 },
	/*0x000d*/ { 0x80c3, AUDIOCONFIG_14 },
	/*0x000e*/ { 0x80c4, AUDIOCONFIG_14 },
	/*0x000f*/ { 0x80c5, AUDIOCONFIG_14 },
	/*0x0010*/ { 0x8007, AUDIOCONFIG_14 },
	/*0x0011*/ { 0x80c7, AUDIOCONFIG_14 },
	/*0x0012*/ { 0x80c8, AUDIOCONFIG_14 },
	/*0x0013*/ { 0x80c9, AUDIOCONFIG_14 },
	/*0x0014*/ { 0x80ca, AUDIOCONFIG_15 },
	/*0x0015*/ { 0x80cb, AUDIOCONFIG_15 },
	/*0x0016*/ { 0x80cc, AUDIOCONFIG_15 },
	/*0x0017*/ { 0x8007, AUDIOCONFIG_15 },
	/*0x0018*/ { 0x8007, AUDIOCONFIG_15 },
	/*0x0019*/ { 0x8007, AUDIOCONFIG_15 },
	/*0x001a*/ { 0x80d2, AUDIOCONFIG_15 },
	/*0x001b*/ { 0x80d3, AUDIOCONFIG_15 },
	/*0x001c*/ { 0x8007, AUDIOCONFIG_15 },
	/*0x001d*/ { 0x80c6, AUDIOCONFIG_14 },
	/*0x001e*/ { 0x80d6, AUDIOCONFIG_15 },
	/*0x001f*/ { 0x80d7, AUDIOCONFIG_15 },
	/*0x0020*/ { 0x80d8, AUDIOCONFIG_15 },
	/*0x0021*/ { 0x80db, AUDIOCONFIG_14 },
	/*0x0022*/ { 0x80da, AUDIOCONFIG_14 },
	/*0x0023*/ { 0x80bf, AUDIOCONFIG_14 },
	/*0x0024*/ { 0x80bf, AUDIOCONFIG_14 },
	/*0x0025*/ { 0x80de, AUDIOCONFIG_14 },
	/*0x0026*/ { 0x80e1, AUDIOCONFIG_15 },
	/*0x0027*/ { 0x80e2, AUDIOCONFIG_15 },
	/*0x0028*/ { 0x977a, AUDIOCONFIG_60 }, // Carrington: "Hello Joanna"
	/*0x0029*/ { 0x97b0, AUDIOCONFIG_60 }, // Carrington: "Joanna, it's good to see you"
	/*0x002a*/ { 0x97b1, AUDIOCONFIG_60 }, // Carrington: "Good day to you, Jo"
	/*0x002b*/ { 0x977c, AUDIOCONFIG_60 }, // Female: "Hi there"
	/*0x002c*/ { 0x9786, AUDIOCONFIG_60 }, // Grimshaw: "Welcome to hacker central"
	/*0x002d*/ { 0x9787, AUDIOCONFIG_60 }, // Grimshaw: "Hi there"
	/*0x002e*/ { 0x9788, AUDIOCONFIG_60 }, // Grimshaw: "Um, er, hi"
	/*0x002f*/ { 0x978c, AUDIOCONFIG_60 }, // Holo girl: "Hi. What can I do for you?"
	/*0x0030*/ { 0x9794, AUDIOCONFIG_60 }, // Hangar guy: "What do you want?"
	/*0x0031*/ { 0x9784, AUDIOCONFIG_60 }, // Foster: "Ah, our star agent"
	/*0x0032*/ { 0x978e, AUDIOCONFIG_60 }, // Male: "Hey there"
	/*0x0033*/ { 0x978f, AUDIOCONFIG_60 }, // Male: "Hi"
	/*0x0034*/ { 0x9790, AUDIOCONFIG_60 }, // Male "How's it going?"
	/*0x0035*/ { 0x977e, AUDIOCONFIG_60 }, // Female: "Hello"
	/*0x0036*/ { 0x977f, AUDIOCONFIG_60 }, // Holo girl: "Hi Jo"
	/*0x0037*/ { 0x9780, AUDIOCONFIG_60 }, // Holo girl: "How's it going?"
	/*0x0038*/ { 0x802e, AUDIOCONFIG_32 },
	/*0x0039*/ { 0x8037, AUDIOCONFIG_35 },
	/*0x003a*/ { 0x8037, AUDIOCONFIG_35 },
	/*0x003b*/ { 0x806a, AUDIOCONFIG_33 },
	/*0x003c*/ { 0x8037, AUDIOCONFIG_35 },
	/*0x003d*/ { 0x8037, AUDIOCONFIG_35 },
	/*0x003e*/ { 0x8037, AUDIOCONFIG_35 },
	/*0x003f*/ { 0x806d, AUDIOCONFIG_33 },
	/*0x0040*/ { 0x806e, AUDIOCONFIG_33 },
	/*0x0041*/ { 0x8037, AUDIOCONFIG_35 },
	/*0x0042*/ { 0x8037, AUDIOCONFIG_35 },
	/*0x0043*/ { 0x8070, AUDIOCONFIG_33 },
	/*0x0044*/ { 0x8071, AUDIOCONFIG_33 },
	/*0x0045*/ { 0x8037, AUDIOCONFIG_35 },
	/*0x0046*/ { 0x8037, AUDIOCONFIG_35 },
	/*0x0047*/ { 0x8037, AUDIOCONFIG_35 },
	/*0x0048*/ { 0x8037, AUDIOCONFIG_35 },
	/*0x0049*/ { 0x8075, AUDIOCONFIG_33 },
	/*0x004a*/ { 0x8037, AUDIOCONFIG_35 },
	/*0x004b*/ { 0x8037, AUDIOCONFIG_35 },
	/*0x004c*/ { 0x8079, AUDIOCONFIG_31 },
	/*0x004d*/ { 0x8066, AUDIOCONFIG_31 },
	/*0x004e*/ { 0x8037, AUDIOCONFIG_35 },
	/*0x004f*/ { 0x8032, AUDIOCONFIG_31 },
	/*0x0050*/ { 0x8037, AUDIOCONFIG_35 },
	/*0x0051*/ { 0x807a, AUDIOCONFIG_35 },
	/*0x0052*/ { 0x8059, AUDIOCONFIG_31 },
	/*0x0053*/ { 0x8001, AUDIOCONFIG_35 },
	/*0x0054*/ { 0x847e, AUDIOCONFIG_32 },
	/*0x0055*/ { 0x847d, AUDIOCONFIG_33 },
	/*0x0056*/ { 0x84f1, AUDIOCONFIG_32 },
	/*0x0057*/ { 0x84f2, AUDIOCONFIG_32 },
	/*0x0058*/ { 0x84f3, AUDIOCONFIG_32 },
	/*0x0059*/ { 0x84f4, AUDIOCONFIG_33 },
	/*0x005a*/ { 0x84f6, AUDIOCONFIG_33 },
	/*0x005b*/ { 0x8506, AUDIOCONFIG_31 },
	/*0x005c*/ { 0x84fd, AUDIOCONFIG_33 },
	/*0x005d*/ { 0x8504, AUDIOCONFIG_33 },
	/*0x005e*/ { 0x84fe, AUDIOCONFIG_33 },
	/*0x005f*/ { 0x8503, AUDIOCONFIG_33 },
	/*0x0060*/ { 0x8500, AUDIOCONFIG_32 },
	/*0x0061*/ { 0x8501, AUDIOCONFIG_32 },
	/*0x0062*/ { 0x8502, AUDIOCONFIG_32 },
	/*0x0063*/ { 0x85ca, AUDIOCONFIG_31 },
	/*0x0064*/ { 0x85cb, AUDIOCONFIG_31 },
	/*0x0065*/ { 0x85d2, AUDIOCONFIG_31 },
	/*0x0066*/ { 0x8074, AUDIOCONFIG_33 },
	/*0x0067*/ { 0x85eb, AUDIOCONFIG_35 },
	/*0x0068*/ { 0x85ec, AUDIOCONFIG_35 },
	/*0x0069*/ { 0x85ed, AUDIOCONFIG_31 },
	/*0x006a*/ { 0x85ee, AUDIOCONFIG_31 },
	/*0x006b*/ { 0x85ef, AUDIOCONFIG_33 },
	/*0x006c*/ { 0x85f0, AUDIOCONFIG_33 },
	/*0x006d*/ { 0x85f1, AUDIOCONFIG_33 },
	/*0x006e*/ { 0x85f2, AUDIOCONFIG_33 },
	/*0x006f*/ { 0x85f3, AUDIOCONFIG_33 },
	/*0x0070*/ { 0x85f4, AUDIOCONFIG_33 },
	/*0x0071*/ { 0x8508, AUDIOCONFIG_31 },
	/*0x0072*/ { 0x850a, AUDIOCONFIG_31 },
	/*0x0073*/ { 0x8076, AUDIOCONFIG_33 },
	/*0x0074*/ { 0x85f5, AUDIOCONFIG_30 },
	/*0x0075*/ { 0x8509, AUDIOCONFIG_33 },
	/*0x0076*/ { 0x8045, AUDIOCONFIG_45 },
	/*0x0077*/ { 0x8046, AUDIOCONFIG_45 },
	/*0x0078*/ { 0x8047, AUDIOCONFIG_25 },
	/*0x0079*/ { 0x8048, AUDIOCONFIG_45 },
	/*0x007a*/ { 0x80f0, AUDIOCONFIG_45 },
	/*0x007b*/ { 0x80ef, AUDIOCONFIG_45 },
	/*0x007c*/ { 0x8049, AUDIOCONFIG_45 },
	/*0x007d*/ { 0x804a, AUDIOCONFIG_45 },
	/*0x007e*/ { 0x804b, AUDIOCONFIG_45 },
	/*0x007f*/ { 0x80d9, AUDIOCONFIG_45 },
	/*0x0080*/ { 0x804c, AUDIOCONFIG_45 },
	/*0x0081*/ { 0x80e7, AUDIOCONFIG_45 },
	/*0x0082*/ { 0x80e6, AUDIOCONFIG_45 },
	/*0x0083*/ { 0x80e0, AUDIOCONFIG_45 },
	/*0x0084*/ { 0x80dc, AUDIOCONFIG_45 },
	/*0x0085*/ { 0x80b9, AUDIOCONFIG_45 },
	/*0x0086*/ { 0x80d4, AUDIOCONFIG_45 },
	/*0x0087*/ { 0x80d0, AUDIOCONFIG_45 },
	/*0x0088*/ { 0x80d1, AUDIOCONFIG_45 },
	/*0x0089*/ { 0x8078, AUDIOCONFIG_45 },
	/*0x008a*/ { 0x80dd, AUDIOCONFIG_45 },
	/*0x008b*/ { 0x8003, AUDIOCONFIG_45 },
	/*0x008c*/ { 0x802d, AUDIOCONFIG_21 },
	/*0x008d*/ { 0x807b, AUDIOCONFIG_21 },
	/*0x008e*/ { 0x807c, AUDIOCONFIG_21 },
	/*0x008f*/ { 0x807d, AUDIOCONFIG_21 },
	/*0x0090*/ { 0x807e, AUDIOCONFIG_21 },
	/*0x0091*/ { 0x807f, AUDIOCONFIG_21 },
	/*0x0092*/ { 0x8080, AUDIOCONFIG_21 },
	/*0x0093*/ { 0x8081, AUDIOCONFIG_21 },
	/*0x0094*/ { 0x8082, AUDIOCONFIG_21 },
	/*0x0095*/ { 0x8083, AUDIOCONFIG_21 },
	/*0x0096*/ { 0x8084, AUDIOCONFIG_21 },
	/*0x0097*/ { 0x8085, AUDIOCONFIG_21 },
	/*0x0098*/ { 0x80b4, AUDIOCONFIG_18 },
	/*0x0099*/ { 0x80b5, AUDIOCONFIG_18 },
	/*0x009a*/ { 0x80b6, AUDIOCONFIG_17 },
	/*0x009b*/ { 0x80a9, AUDIOCONFIG_18 },
	/*0x009c*/ { 0x80aa, AUDIOCONFIG_18 },
	/*0x009d*/ { 0x80ab, AUDIOCONFIG_17 },
	/*0x009e*/ { 0x80ac, AUDIOCONFIG_18 },
	/*0x009f*/ { 0x80ad, AUDIOCONFIG_18 },
	/*0x00a0*/ { 0x80ae, AUDIOCONFIG_17 },
	/*0x00a1*/ { 0x80b1, AUDIOCONFIG_18 },
	/*0x00a2*/ { 0x80b2, AUDIOCONFIG_18 },
	/*0x00a3*/ { 0x80b3, AUDIOCONFIG_18 },
	/*0x00a4*/ { 0x818a, AUDIOCONFIG_19 },
	/*0x00a5*/ { VERSION >= VERSION_NTSC_1_0 ? 0xefdd : 0x8190, AUDIOCONFIG_19 },
	/*0x00a6*/ { 0x805f, AUDIOCONFIG_32 },
	/*0x00a7*/ { 0x8060, AUDIOCONFIG_32 },
	/*0x00a8*/ { 0x8061, AUDIOCONFIG_32 },
	/*0x00a9*/ { 0x8004, AUDIOCONFIG_32 },
	/*0x00aa*/ { 0x80f1, AUDIOCONFIG_32 },
	/*0x00ab*/ { 0x80f3, AUDIOCONFIG_32 },
	/*0x00ac*/ { 0x8055, AUDIOCONFIG_16 },
	/*0x00ad*/ { 0x8056, AUDIOCONFIG_16 },
	/*0x00ae*/ { 0x8057, AUDIOCONFIG_16 },
	/*0x00af*/ { 0x85f7, AUDIOCONFIG_52 },
	/*0x00b0*/ { 0x8127, AUDIOCONFIG_29 },
	/*0x00b1*/ { 0x8128, AUDIOCONFIG_29 },
	/*0x00b2*/ { 0x8129, AUDIOCONFIG_29 },
	/*0x00b3*/ { 0x812a, AUDIOCONFIG_29 },
	/*0x00b4*/ { 0x812b, AUDIOCONFIG_29 },
	/*0x00b5*/ { 0x812c, AUDIOCONFIG_29 },
	/*0x00b6*/ { 0x812d, AUDIOCONFIG_50 },
	/*0x00b7*/ { 0x812e, AUDIOCONFIG_50 },
	/*0x00b8*/ { 0x8138, AUDIOCONFIG_50 },
	/*0x00b9*/ { 0x8138, AUDIOCONFIG_28 },
	/*0x00ba*/ { 0x812f, AUDIOCONFIG_51 },
	/*0x00bb*/ { 0x8151, AUDIOCONFIG_51 },
	/*0x00bc*/ { 0x8154, AUDIOCONFIG_51 },
	/*0x00bd*/ { 0x8157, AUDIOCONFIG_51 },
	/*0x00be*/ { 0x815a, AUDIOCONFIG_51 },
	/*0x00bf*/ { 0x8132, AUDIOCONFIG_51 },
	/*0x00c0*/ { 0x8139, AUDIOCONFIG_51 },
	/*0x00c1*/ { 0x813f, AUDIOCONFIG_51 },
	/*0x00c2*/ { 0x8145, AUDIOCONFIG_51 },
	/*0x00c3*/ { 0x814b, AUDIOCONFIG_51 },
	/*0x00c4*/ { 0x8161, AUDIOCONFIG_04 },
	/*0x00c5*/ { 0x8162, AUDIOCONFIG_04 },
	/*0x00c6*/ { 0x8163, AUDIOCONFIG_04 },
	/*0x00c7*/ { 0x8164, AUDIOCONFIG_04 },
	/*0x00c8*/ { 0x8165, AUDIOCONFIG_04 },
	/*0x00c9*/ { 0x8166, AUDIOCONFIG_04 },
	/*0x00ca*/ { 0x8167, AUDIOCONFIG_04 },
	/*0x00cb*/ { 0x8168, AUDIOCONFIG_04 },
	/*0x00cc*/ { 0x8169, AUDIOCONFIG_04 },
	/*0x00cd*/ { 0x816a, AUDIOCONFIG_04 },
	/*0x00ce*/ { 0x816b, AUDIOCONFIG_04 },
	/*0x00cf*/ { 0x816c, AUDIOCONFIG_04 },
	/*0x00d0*/ { 0x816d, AUDIOCONFIG_04 },
	/*0x00d1*/ { 0x816e, AUDIOCONFIG_04 },
	/*0x00d2*/ { 0x816f, AUDIOCONFIG_04 },
	/*0x00d3*/ { 0x8170, AUDIOCONFIG_04 },
	/*0x00d4*/ { 0x8171, AUDIOCONFIG_04 },
	/*0x00d5*/ { 0x8172, AUDIOCONFIG_04 },
	/*0x00d6*/ { 0x8173, AUDIOCONFIG_04 },
	/*0x00d7*/ { 0x8174, AUDIOCONFIG_04 },
	/*0x00d8*/ { 0x8175, AUDIOCONFIG_04 },
	/*0x00d9*/ { 0x8176, AUDIOCONFIG_04 },
	/*0x00da*/ { 0x8177, AUDIOCONFIG_04 },
	/*0x00db*/ { 0x8178, AUDIOCONFIG_04 },
	/*0x00dc*/ { 0x8179, AUDIOCONFIG_04 },
	/*0x00dd*/ { 0x817a, AUDIOCONFIG_04 },
	/*0x00de*/ { 0x817b, AUDIOCONFIG_04 },
	/*0x00df*/ { 0x817c, AUDIOCONFIG_04 },
	/*0x00e0*/ { 0x817d, AUDIOCONFIG_04 },
	/*0x00e1*/ { 0x817e, AUDIOCONFIG_04 },
	/*0x00e2*/ { 0x817f, AUDIOCONFIG_04 },
	/*0x00e3*/ { 0x8180, AUDIOCONFIG_04 },
	/*0x00e4*/ { 0x804d, AUDIOCONFIG_04 },
	/*0x00e5*/ { 0x804e, AUDIOCONFIG_04 },
	/*0x00e6*/ { 0x85f8, AUDIOCONFIG_05 },
	/*0x00e7*/ { 0x85f9, AUDIOCONFIG_05 },
	/*0x00e8*/ { 0x85fa, AUDIOCONFIG_05 },
	/*0x00e9*/ { 0x85fb, AUDIOCONFIG_05 },
	/*0x00ea*/ { 0x85fc, AUDIOCONFIG_06 },
	/*0x00eb*/ { 0x85fd, AUDIOCONFIG_06 },
	/*0x00ec*/ { 0x85fe, AUDIOCONFIG_06 },
	/*0x00ed*/ { 0x85ff, AUDIOCONFIG_06 },
	/*0x00ee*/ { 0x8600, AUDIOCONFIG_04 },
	/*0x00ef*/ { 0x8601, AUDIOCONFIG_04 },
	/*0x00f0*/ { 0x8602, AUDIOCONFIG_04 },
	/*0x00f1*/ { 0x8603, AUDIOCONFIG_04 },
	/*0x00f2*/ { 0x8604, AUDIOCONFIG_04 },
	/*0x00f3*/ { 0x8605, AUDIOCONFIG_04 },
	/*0x00f4*/ { 0x8606, AUDIOCONFIG_04 },
	/*0x00f5*/ { 0x8606, AUDIOCONFIG_04 },
#if VERSION >= VERSION_NTSC_1_0
	/*0x00f6*/ { 0x81d9, AUDIOCONFIG_61 },
#endif
	/*0x00f7*/ { 0x8267, AUDIOCONFIG_02 },
	/*0x00f8*/ { 0x8268, AUDIOCONFIG_02 },
	/*0x00f9*/ { 0x8286, AUDIOCONFIG_02 },
	/*0x00fa*/ { 0x8288, AUDIOCONFIG_02 },
	/*0x00fb*/ { 0x8291, AUDIOCONFIG_02 },
	/*0x00fc*/ { 0x829f, AUDIOCONFIG_02 },
	/*0x00fd*/ { 0x82a1, AUDIOCONFIG_02 },
#if VERSION >= VERSION_NTSC_1_0
	/*0x00fe*/ { 0x8af9, AUDIOCONFIG_47 }, // Cass: "You've become quite an annoyance"
	/*0x00ff*/ { 0x8afa, AUDIOCONFIG_47 }, // Cass: "We meet again, girl"
	/*0x0100*/ { 0x8afb, AUDIOCONFIG_47 }, // Cass: "Good night, Miss Dark"
#else
	/*0x00fe*/ { 0x92f9, AUDIOCONFIG_47 }, // Cass: "You've become quite an annoyance"
	/*0x00ff*/ { 0x92fa, AUDIOCONFIG_47 }, // Cass: "We meet again, girl"
	/*0x0100*/ { 0x92fb, AUDIOCONFIG_47 }, // Cass: "Good night, Miss Dark"
#endif
	/*0x0101*/ { 0x92fc, AUDIOCONFIG_47 }, // Cass: "Get her!"
	/*0x0102*/ { 0x92fd, AUDIOCONFIG_47 }, // Cass: (laughing)
	/*0x0103*/ { 0x9285, AUDIOCONFIG_02 }, // Scientist: "Who the hell are you?"
	/*0x0104*/ { 0x9306, AUDIOCONFIG_48 }, // Programmer: "Yes, yes, I agree. Personality..."
	/*0x0105*/ { 0x9307, AUDIOCONFIG_48 }, // Programmer: "No. I can start the process..."
	/*0x0106*/ { 0x9308, AUDIOCONFIG_48 }, // Programmer: "I'll begin at once. Goodbye"
	/*0x0107*/ { 0x9309, AUDIOCONFIG_48 }, // Programmer: "Security, help me!"
	/*0x0108*/ { 0x930a, AUDIOCONFIG_48 }, // Programmer: "Help! Help!"
	/*0x0109*/ { 0x8435, AUDIOCONFIG_49 },
	/*0x010a*/ { 0x81b5, AUDIOCONFIG_07 },
	/*0x010b*/ { 0x81b6, AUDIOCONFIG_08 },
	/*0x010c*/ { 0x81b5, AUDIOCONFIG_09 },
	/*0x010d*/ { 0x81b6, AUDIOCONFIG_10 },
	/*0x010e*/ { 0x85f3, AUDIOCONFIG_11 },
	/*0x010f*/ { 0x81b5, AUDIOCONFIG_12 },
	/*0x0110*/ { 0x81b6, AUDIOCONFIG_13 },
	/*0x0111*/ { 0x81b7, AUDIOCONFIG_20 },
	/*0x0112*/ { 0x81b8, AUDIOCONFIG_20 },
	/*0x0113*/ { 0x81c7, AUDIOCONFIG_20 },
	/*0x0114*/ { 0x8037, AUDIOCONFIG_20 },
	/*0x0115*/ { 0x81b7, AUDIOCONFIG_56 },
	/*0x0116*/ { 0x81b9, AUDIOCONFIG_00 },
	/*0x0117*/ { 0x81ba, AUDIOCONFIG_00 },
	/*0x0118*/ { 0x81bb, AUDIOCONFIG_00 },
	/*0x0119*/ { 0x81cb, AUDIOCONFIG_36 },
	/*0x011a*/ { 0x81cb, AUDIOCONFIG_37 },
	/*0x011b*/ { 0x81cb, AUDIOCONFIG_38 },
#if VERSION >= VERSION_NTSC_1_0
	/*0x011c*/ { 0x81c6, AUDIOCONFIG_55 },
#else
	/*0x011c*/ { 0x81c6, AUDIOCONFIG_55 },
#endif
	/*0x011d*/ { 0x9278, AUDIOCONFIG_47 }, // "That's not how it goes"
	/*0x011e*/ { 0x9279, AUDIOCONFIG_47 }, // "Give it to me. You're doing it wrong"
	/*0x011f*/ { 0x927a, AUDIOCONFIG_47 }, // "Oh no, we're too late"
	/*0x0120*/ { 0x927b, AUDIOCONFIG_47 }, // "Yes it is"
	/*0x0121*/ { 0x927c, AUDIOCONFIG_47 }, // "Stop annoying me"
	/*0x0122*/ { 0x927d, AUDIOCONFIG_47 }, // "She's here!"
	/*0x0123*/ { 0x9277, AUDIOCONFIG_47 }, // "Will you just hurry up?"
	/*0x0124*/ { 0x82fd, AUDIOCONFIG_02 }, // "Holy..."
	/*0x0125*/ { 0x82fe, AUDIOCONFIG_02 }, // "What the hell?"
	/*0x0126*/ { 0x8300, AUDIOCONFIG_02 }, // "Oh my god"
	/*0x0127*/ { 0x8303, AUDIOCONFIG_02 }, // "Oh god I'm hit"
	/*0x0128*/ { 0x830a, AUDIOCONFIG_02 }, // "Oh my god"
#if VERSION >= VERSION_NTSC_1_0
	/*0x0129*/ { 0x8313, AUDIOCONFIG_62 }, // "Noooo!"
#endif
	/*0x012a*/ { 0x8314, AUDIOCONFIG_02 }, // "Oh god he's dead!"
	/*0x012b*/ { 0x832e, AUDIOCONFIG_02 }, // "Bloody stupid gun"
	/*0x012c*/ { 0x8330, AUDIOCONFIG_02 }, // "Damn it"
	/*0x012d*/ { 0x8333, AUDIOCONFIG_02 }, // "Damn, she's good"
	/*0x012e*/ { 0x8334, AUDIOCONFIG_02 }, // "Cover my ass"
#if VERSION >= VERSION_NTSC_1_0
	/*0x012f*/ { 0x834c, AUDIOCONFIG_62 }, // Death scream
#endif
	/*0x0130*/ { 0x83a3, AUDIOCONFIG_02 }, // Female: "Damn she's good"
	/*0x0131*/ { 0x83b9, AUDIOCONFIG_02 }, // Female: "My god"
	/*0x0132*/ { 0x83c7, AUDIOCONFIG_02 }, // "Geez, that hurt"
	/*0x0133*/ { 0x83cf, AUDIOCONFIG_02 }, // "Damn it, I'm taking fire"
	/*0x0134*/ { 0x83d2, AUDIOCONFIG_02 }, // "God damn it"
	/*0x0135*/ { 0x83d4, AUDIOCONFIG_02 }, // "Holy moley"
	/*0x0136*/ { 0x83f3, AUDIOCONFIG_02 }, // "Damn, missed again"
	/*0x0137*/ { 0x83f5, AUDIOCONFIG_02 }, // "Damn"
	/*0x0138*/ { 0x83f6, AUDIOCONFIG_02 }, // "Hell, she's good"
	/*0x0139*/ { 0x8400, AUDIOCONFIG_02 }, // "Let's get the hell out of here"
#if VERSION >= VERSION_NTSC_1_0
	/*0x013a*/ { 0x8411, AUDIOCONFIG_62 }, // "Noooo!"
#endif
	/*0x013b*/ { 0x8413, AUDIOCONFIG_02 }, // "Shoot, damn it"
	/*0x013c*/ { 0x8414, AUDIOCONFIG_02 }, // "Oh god, I'm dying"
	/*0x013d*/ { 0x8422, AUDIOCONFIG_02 }, // "God, run!"
	/*0x013e*/ { 0x8432, AUDIOCONFIG_33 },
	/*0x013f*/ { 0x843c, AUDIOCONFIG_58 },
	/*0x0140*/ { 0x843d, AUDIOCONFIG_58 },
	/*0x0141*/ { 0x84ab, AUDIOCONFIG_58 },
	/*0x0142*/ { 0x843d, AUDIOCONFIG_58 },
	/*0x0143*/ { 0x81b7, AUDIOCONFIG_58 },
	/*0x0144*/ { 0x8477, AUDIOCONFIG_57 },
	/*0x0145*/ { 0x8478, AUDIOCONFIG_57 },
#if VERSION >= VERSION_NTSC_1_0
	/*0x0146*/ { 0x81c2, AUDIOCONFIG_54 },
	/*0x0147*/ { 0x8479, AUDIOCONFIG_54 },
	/*0x0148*/ { 0x81c3, AUDIOCONFIG_54 },
	/*0x0149*/ { 0x81c4, AUDIOCONFIG_54 },
#else
	/*0x0146*/ { 0x81c2, AUDIOCONFIG_55 },
	/*0x0147*/ { 0x8479, AUDIOCONFIG_55 },
	/*0x0148*/ { 0x81c3, AUDIOCONFIG_55 },
	/*0x0149*/ { 0x81c4, AUDIOCONFIG_55 },
#endif
	/*0x014a*/ { 0x8443, AUDIOCONFIG_02 }, // Civilian: "Oh my god"
	/*0x014b*/ { 0x844e, AUDIOCONFIG_02 }, // "What the hell was that?"
#if VERSION >= VERSION_NTSC_1_0
	/*0x014c*/ { 0x843c, AUDIOCONFIG_63 },
	/*0x014d*/ { 0x847c, AUDIOCONFIG_63 },
	/*0x014e*/ { 0x843d, AUDIOCONFIG_63 },
#endif
	/*0x014f*/ { 0xf2ad, AUDIOCONFIG_01 }, // "Joanna, this is Jonathan. You're in grave danger"
	/*0x0150*/ { 0xf2ae, AUDIOCONFIG_01 }, // "They've flooded the area with nerge gas"
	/*0x0151*/ { 0xf2af, AUDIOCONFIG_01 }, // "Get our friend to the containment lab. There's a hiding place there"
	/*0x0152*/ { 0xd2b0, AUDIOCONFIG_01 }, // "I'll contact you when you get there. Good luck"
	/*0x0153*/ { 0xf2b1, AUDIOCONFIG_01 }, // "Good work Jo. We need to meet up. They're getting suspicious of me"
	/*0x0154*/ { 0xd2b2, AUDIOCONFIG_01 }, // "Use the maintenance hatch I've opened in one of the containment labs. Hurry"
	/*0x0155*/ { 0x92b8, AUDIOCONFIG_02 }, // "Damn it Joanna, cover me!"
	/*0x0156*/ { 0x9b30, AUDIOCONFIG_02 }, // "Oh my god, a gun?!"
	/*0x0157*/ { 0x9b27, AUDIOCONFIG_47 }, // "So you got here at last"
	/*0x0158*/ { 0x9b28, AUDIOCONFIG_47 }, // "Hey, you're not, uh, security alert!"
	/*0x0159*/ { 0x9b29, AUDIOCONFIG_47 }, // "No guns allowed in here. Put your weapon away"
	/*0x015a*/ { 0x9b2a, AUDIOCONFIG_47 }, // "I won't tell you again. Lose the weapon"
	/*0x015b*/ { 0x9b2b, AUDIOCONFIG_47 }, // "Right. I'm calling security"
	/*0x015c*/ { 0x9b2c, AUDIOCONFIG_47 }, // "Everyone's been waiting for you"
	/*0x015d*/ { 0x9a9d, AUDIOCONFIG_60 }, // "How's things?"
	/*0x015e*/ { 0x9a9e, AUDIOCONFIG_60 }, // "Hey there"
	/*0x015f*/ { 0x9a9f, AUDIOCONFIG_60 }, // "Hi. How are you?"
	/*0x0160*/ { 0x8037, AUDIOCONFIG_04 },
	/*0x0161*/ { 0x9271, AUDIOCONFIG_60 }, // Female: "Good afternoon"
	/*0x0162*/ { 0x9275, AUDIOCONFIG_02 }, // Female: "Oh my god!"
	/*0x0163*/ { 0x9acc, AUDIOCONFIG_60 }, // "Hi there"
	/*0x0164*/ { 0x9acd, AUDIOCONFIG_60 }, // "How's things"
	/*0x0165*/ { 0x9ad4, AUDIOCONFIG_60 }, // "Hello"
	/*0x0166*/ { 0x9ad5, AUDIOCONFIG_60 }, // "Hey, what's up?"
	/*0x0167*/ { 0x84c8, AUDIOCONFIG_02 }, // "What the hell?"
	/*0x0168*/ { 0x84cd, AUDIOCONFIG_02 }, // "Geez"
	/*0x0169*/ { 0x84ad, AUDIOCONFIG_46 },
	/*0x016a*/ { 0x8507, AUDIOCONFIG_27 },
	/*0x016b*/ { 0x84ee, AUDIOCONFIG_16 },
	/*0x016c*/ { 0x84ef, AUDIOCONFIG_16 },
	/*0x016d*/ { 0x84f0, AUDIOCONFIG_16 },
	/*0x016e*/ { 0x92c8, AUDIOCONFIG_02 }, // President: "What the hell was that?"
	/*0x016f*/ { 0x92c9, AUDIOCONFIG_02 }, // Trent: "How the hell did you get on board?"
	/*0x0170*/ { 0xb3a0, AUDIOCONFIG_01 }, // Carrington: "We're getting a positive reading. The internal comms hub is nearby"
	/*0x0171*/ { 0xb3a1, AUDIOCONFIG_01 }, // Carrington: "You're on the same floor as Cassandra's office"
	/*0x0172*/ { 0xd3a3, AUDIOCONFIG_01 }, // Carrington: "The basement elevator must be around there somewhere"
	/*0x0173*/ { 0xd3a4, AUDIOCONFIG_01 }, // Carrington: "That's the highest security sector. Dr Caroll has got to be nearby"
	/*0x0174*/ { 0xd3a6, AUDIOCONFIG_01 }, // Grimshaw: "Reprogram the cleaning bot. It'll give you a way in"
	/*0x0175*/ { 0xd3a7, AUDIOCONFIG_01 }, // Carrington: "Get out Jo! The levels are too high. Use the camspy"
	/*0x0176*/ { 0xd3a8, AUDIOCONFIG_01 }, // Carrington: "They've locked down the ground floor. Get to the elevator"
	/*0x0177*/ { 0xd3a9, AUDIOCONFIG_01 }, // Carrington: "That copter needs to be taken out of the jumpship will get shut down"
	/*0x0178*/ { 0xd3ab, AUDIOCONFIG_01 }, // Carrington: "Don't keep the jumpship waiting. Get back to the landing pad"
	/*0x0179*/ { 0xd3b0, AUDIOCONFIG_01 }, // Carrington: "The only place we could secure the equipment was a storm drain"
	/*0x017a*/ { 0xd3b2, AUDIOCONFIG_01 }, // Grimshaw: "Reprogram that taxi. It's a mark 2 and should be compatible"
	/*0x017b*/ { 0xd3b3, AUDIOCONFIG_01 }, // Carrington: The carpark lift is your only point of ingress"
	/*0x017c*/ { 0xb3b6, AUDIOCONFIG_01 }, // Grimshaw: "The safe's heavily encrypted. The decoder's going to take a little longer"
	/*0x017d*/ { 0xd3b8, AUDIOCONFIG_01 }, // Carrington: "The air intercept radar is controlled from that bunker"
	/*0x017e*/ { 0xd3ba, AUDIOCONFIG_01 }, // Carrington: "The hangar lift is the other side of that huge door"
	/*0x017f*/ { 0xd3bb, AUDIOCONFIG_01 }, // Jon: "Careful with that trigger finger, Agent Dark. You're closing in on my position"
	/*0x0180*/ { 0xb3bd, AUDIOCONFIG_01 }, // Carrington: "The temperature's dropping. You must be close to the cryo room"
	/*0x0181*/ { 0xd3bf, AUDIOCONFIG_01 }, // Carrington: "There he is Jo. Hurry or you'll be too late"
	/*0x0182*/ { 0xb3c4, AUDIOCONFIG_01 }, // Carrington: "Now's your chance. Grab the stewardess's bag and get her uniform"
	/*0x0183*/ { 0xb3c5, AUDIOCONFIG_01 }, // Carrington: "Use that case to conceal your equipment"
	/*0x0184*/ { 0xb3c6, AUDIOCONFIG_01 }, // Carrington: "The case will get stopped unless you do something"
	/*0x0185*/ { 0xb3c9, AUDIOCONFIG_01 }, // Carrington: "The president's room is on this level, Joanna"
	/*0x0186*/ { 0xb3ca, AUDIOCONFIG_01 }, // Carrington: "Something's wrong. You have to get to the cockpit"
	/*0x0187*/ { 0x8594, AUDIOCONFIG_04 }, // Footstep
	/*0x0188*/ { 0x8595, AUDIOCONFIG_04 }, // Footstep
	/*0x0189*/ { 0x8596, AUDIOCONFIG_04 }, // Footstep
	/*0x018a*/ { 0x8597, AUDIOCONFIG_04 }, // Footstep
	/*0x018b*/ { 0x8598, AUDIOCONFIG_04 }, // Footstep
	/*0x018c*/ { 0x8599, AUDIOCONFIG_04 }, // Footstep
	/*0x018d*/ { 0x859a, AUDIOCONFIG_04 }, // Footstep
	/*0x018e*/ { 0x859b, AUDIOCONFIG_04 }, // Footstep
	/*0x018f*/ { 0x85a0, AUDIOCONFIG_04 }, // Footstep
	/*0x0190*/ { 0x85a1, AUDIOCONFIG_04 }, // Footstep
	/*0x0191*/ { 0x85a2, AUDIOCONFIG_04 }, // Footstep
	/*0x0192*/ { 0x85a3, AUDIOCONFIG_04 }, // Footstep
	/*0x0193*/ { 0xf310, AUDIOCONFIG_01 }, // Carrington: "Joanna. We're under attack"
	/*0x0194*/ { 0xd311, AUDIOCONFIG_01 }, // Carrington: "Get the guns back online. Hurry"
	/*0x0195*/ { 0xf312, AUDIOCONFIG_01 }, // Carrington: "The Skedar have taken hostages"
	/*0x0196*/ { 0xd313, AUDIOCONFIG_01 }, // Carrington: "Get up to the offices and save them"
	/*0x0197*/ { 0xf314, AUDIOCONFIG_01 }, // Carrington: "They're using a new form of shield technology"
	/*0x0198*/ { 0xd315, AUDIOCONFIG_01 }, // Carrington: "Foster was working on a new weapon which may be useful"
	/*0x0199*/ { 0xf316, AUDIOCONFIG_03 }, // Carrington: "Damn it, my office. If they get access..."
	/*0x019a*/ { 0xd317, AUDIOCONFIG_01 }, // Carrington: "Get there first, Jo and destroy the files"
	/*0x019b*/ { 0xf318, AUDIOCONFIG_01 }, // Carrington: "Things are desparate. They've planted a bomb"
	/*0x019c*/ { 0xd319, AUDIOCONFIG_01 }, // Carrington: "Find it and get it out of the building"
	/*0x019d*/ { 0xf31a, AUDIOCONFIG_01 }, // Carrington: "Well done Joanna. We're nearly clear"
	/*0x019e*/ { 0xd31b, AUDIOCONFIG_01 }, // Carrington: "The last dropship is waiting for you. Hurry"
	/*0x019f*/ { 0xf293, AUDIOCONFIG_01 }, // Grimshaw: "Joanna, it's Grimshaw. We've got hackers at..."
	/*0x01a0*/ { 0xf2df, AUDIOCONFIG_01 }, // Elvis: "We're outside, Joanna. Get the shields down and we can help you"
	/*0x01a1*/ { 0xf2e0, AUDIOCONFIG_01 }, // Elvis: "You've got to open the hanger doors so we can dock"
	/*0x01a2*/ { 0xd4ea, AUDIOCONFIG_01 }, // Carrington: "Stand back Joanna. We'll create your escape route"
	/*0x01a3*/ { VERSION >= VERSION_NTSC_1_0 ? 0x8d24 : 0x9524, AUDIOCONFIG_02 }, // Cass: "Get the hell out of my office"
	/*0x01a4*/ { 0x9d62, AUDIOCONFIG_47 }, // Elvis: "Joanna, what took you so long? Follow me. Let's get off this old tub"
	/*0x01a5*/ { 0x84aa, AUDIOCONFIG_43 },
	/*0x01a6*/ { 0x81cf, AUDIOCONFIG_41 },
	/*0x01a7*/ { 0x858f, AUDIOCONFIG_40 },
	/*0x01a8*/ { 0x80d5, AUDIOCONFIG_39 }, // Lift mechanical sound
	/*0x01a9*/ { 0x80ec, AUDIOCONFIG_39 }, // Lift mechanical sound
	/*0x01aa*/ { 0x80ed, AUDIOCONFIG_40 }, // Lift mechanical sound
	/*0x01ab*/ { 0x80d5, AUDIOCONFIG_41 }, // Lift mechanical sound
	/*0x01ac*/ { 0x80ec, AUDIOCONFIG_41 }, // Lift mechanical sound
	/*0x01ad*/ { 0x80ed, AUDIOCONFIG_42 }, // Lift mechanical sound
	/*0x01ae*/ { 0x80d5, AUDIOCONFIG_41 }, // Lift mechanical sound
	/*0x01af*/ { 0x80ec, AUDIOCONFIG_41 }, // Lift mechanical sound
	/*0x01b0*/ { 0x803a, AUDIOCONFIG_41 }, // Lift call
	/*0x01b1*/ { 0x84e8, AUDIOCONFIG_43 }, // Lift mechanical sound
	/*0x01b2*/ { 0x84e9, AUDIOCONFIG_43 }, // Lift mechanical sound
	/*0x01b3*/ { 0x80cd, AUDIOCONFIG_42 }, // Lift mechanical sound
	/*0x01b4*/ { 0x80ce, AUDIOCONFIG_42 }, // Lift mechanical sound
	/*0x01b5*/ { 0x8039, AUDIOCONFIG_42 }, // Lift mechanical sound
	/*0x01b6*/ { 0x80cd, AUDIOCONFIG_44 }, // Lift mechanical sound
	/*0x01b7*/ { 0x80db, AUDIOCONFIG_16 }, // Lift roller door
	/*0x01b8*/ { 0x80da, AUDIOCONFIG_16 }, // Lift roller door
	/*0x01b9*/ { 0xf41c, AUDIOCONFIG_02 }, // Joanna: "Oh my god"
	/*0x01ba*/ { 0xf434, AUDIOCONFIG_02 }, // Lab guy: "What the hell do you think you're doing? This is supposed..."
	/*0x01bb*/ { 0xf44f, AUDIOCONFIG_02 }, // President: "Damn it man. I say no and I mean no..."
	/*0x01bc*/ { 0x0000, AUDIOCONFIG_00 },
};

struct audioconfig g_AudioConfigs[] = {
	{ /* 0*/  200, 1000, 1200, -1, 100, -1,   0, 0 },
	{ /* 1*/  400, 2500, 3000, -1, 100, 48,   0, AUDIOCONFIGFLAG_20 },
	{ /* 2*/  400, 2500, 3000, -1, 100, -1,   0, AUDIOCONFIGFLAG_OFFENSIVE },
	{ /* 3*/  400, 2500, 3000, -1, 100, 48,   0, AUDIOCONFIGFLAG_OFFENSIVE | AUDIOCONFIGFLAG_20 },
	{ /* 4*/  200, 1000, 1200, -1, 100, -1,   0, 0 },
	{ /* 5*/  200, 1000, 1200, -1, 100, -1,   0, 0 },
	{ /* 6*/  200, 1000, 1200, -1, 100, -1,   0, 0 },
	{ /* 7*/ 1000, 3500, 4000, -1, 100, -1, 120, AUDIOCONFIGFLAG_01 | AUDIOCONFIGFLAG_08 },
	{ /* 8*/ 1000, 2500, 3000, -1, 100, -1,  30, AUDIOCONFIGFLAG_01 | AUDIOCONFIGFLAG_08 },
	{ /* 9*/ 1000, 3500, 4000, -1, 100, -1, 120, AUDIOCONFIGFLAG_01 | AUDIOCONFIGFLAG_08 },
	{ /*10*/ 1000, 2500, 3000, -1, 100, -1,  30, AUDIOCONFIGFLAG_01 | AUDIOCONFIGFLAG_08 },
	{ /*11*/ 1600, 3000, 3500, -1, 100, -1,   0, AUDIOCONFIGFLAG_01 | AUDIOCONFIGFLAG_08 },
	{ /*12*/  400, 1100, 1400, -1, 100, -1, 140, AUDIOCONFIGFLAG_01 | AUDIOCONFIGFLAG_08 },
	{ /*13*/  400, 1100, 1400, -1, 100, -1,   0, AUDIOCONFIGFLAG_01 | AUDIOCONFIGFLAG_08 },
	{ /*14*/  300, 1200, 1250, -1, 100, -1,   0, AUDIOCONFIGFLAG_01 },
	{ /*15*/  500, 1800, 1900, -1, 100, -1,   0, AUDIOCONFIGFLAG_01 },
	{ /*16*/ 1000, 2700, 2900, -1, 100, -1,   0, AUDIOCONFIGFLAG_01 },
	{ /*17*/ 1500, 3000, 4000, -1, 100, -1,   0, AUDIOCONFIGFLAG_01 },
	{ /*18*/ 2500, 4900, 5000, -1, 100, -1,   0, AUDIOCONFIGFLAG_01 },
	{ /*19*/ 5500, 5600, 5700, -1, 100, -1,   0, AUDIOCONFIGFLAG_01 | AUDIOCONFIGFLAG_20 },
	{ /*20*/   20,  400,  500, -1, 100, -1,   0, 0 },
	{ /*21*/  300, 1700, 2000, -1, 100, -1,   0, 0 },
	{ /*22*/  600, 3500, 5000, -1, 100, -1,   0, 0 },
	{ /*23*/  600, 3500, 5000, -1, 100, -1,   0, 0 },
	{ /*24*/  600, 3500, 5000, -1, 100, -1,   0, 0 },
	{ /*25*/  600, 2500, 3000, -1, 100, -1,   0, 0 },
	{ /*26*/ 1500, 4900, 5500, -1, 100, -1, 100, AUDIOCONFIGFLAG_01 },
	{ /*27*/ 5500, 5740, 5800, -1, 100, -1, 100, AUDIOCONFIGFLAG_01 },
	{ /*28*/  250, 1000, 1300,  2, 100, -1, 120, AUDIOCONFIGFLAG_01 },
	{ /*29*/  200, 2600, 3000, -1, 100, -1,   0, 0 },
	{ /*30*/  200,  750,  850, -1, 100, -1,   0, 0 },
	{ /*31*/  600, 2000, 3000, -1, 100, -1,   0, 0 },
	{ /*32*/  500, 1500, 1800, -1, 100, -1,   0, 0 },
	{ /*33*/  800, 2500, 3000, -1, 100, -1,   0, 0 },
	{ /*34*/  400, 1500, 2000, -1, 100, -1,   0, 0 },
	{ /*35*/ 1000, 3500, 5000, -1, 100, -1,   0, AUDIOCONFIGFLAG_01 },
	{ /*36*/   50,  225,  250, -1, 100, -1,   0, AUDIOCONFIGFLAG_01 },
	{ /*37*/   50,  225,  250, -1, 100, -1,   0, AUDIOCONFIGFLAG_01 },
	{ /*38*/ 2000, 2500, 2700, -1,  50, -1,   0, AUDIOCONFIGFLAG_01 | AUDIOCONFIGFLAG_08 },
	{ /*39*/  300, 1000, 1200, -1, 100, -1,   0, AUDIOCONFIGFLAG_01 },
	{ /*40*/  300, 1000, 1200, -1, 100, -1, 100, AUDIOCONFIGFLAG_01 },
	{ /*41*/  600, 1500, 1800, -1, 100, -1,   0, AUDIOCONFIGFLAG_01 },
	{ /*42*/  600, 1500, 1800, -1, 100, -1, 100, AUDIOCONFIGFLAG_01 },
	{ /*43*/  800, 2600, 3500, -1, 100, -1,   0, AUDIOCONFIGFLAG_01 },
	{ /*44*/  800, 2600, 3500, -1, 100, -1, 100, AUDIOCONFIGFLAG_01 },
	{ /*45*/  200,  800, 1000, -1, 100, -1,   0, 0 },
	{ /*46*/  200,  700, 1000, -1, 100, -1,   0, 0 },
	{ /*47*/ 1000, 2500, 3000, -1, 100, -1,   0, 0 },
	{ /*48*/  500, 1400, 1500, -1, 100, -1,   0, 0 },
	{ /*49*/  500, 2000, 2200, -1, 100, -1,   0, 0 },
	{ /*50*/  400, 2500, 3000, -1,  50, -1,   0, 0 },
	{ /*51*/  400, 2500, 3000, -1,  75, -1,   0, 0 },
	{ /*52*/  200,  500,  800, -1, 100, -1, 100, 0 },
	{ /*53*/ 1000, 1100, 1600, -1, 100, -1,   0, 0 },
#if VERSION >= VERSION_NTSC_1_0
	{ /*54*/  300, 1300, 1500, -1, 100, -1,   0, AUDIOCONFIGFLAG_01 },
#endif
	{ /*55*/  300, 1300, 1500, -1, 100, -1, 200, AUDIOCONFIGFLAG_01 },
	{ /*56*/  250,  600,  900, -1, 100, -1, 200, AUDIOCONFIGFLAG_01 },
	{ /*57*/   50,  180,  250, -1, 100, -1, 200, AUDIOCONFIGFLAG_01 },
	{ /*58*/  100,  400,  600, -1, 100, -1, 200, AUDIOCONFIGFLAG_01 },
	{ /*59*/  700, 1100, 1400, -1, 100, -1, 200, AUDIOCONFIGFLAG_01 },
	{ /*60*/  400, 2500, 3000, -1, 100, -1,   0, AUDIOCONFIGFLAG_04 },
#if VERSION >= VERSION_NTSC_1_0
	{ /*61*/  300,  900, 1100, -1, 100, -1,   0, 0 },
	{ /*62*/ 1000, 2500, 3000, -1, 100, -1,   0, AUDIOCONFIGFLAG_08 },
	{ /*63*/  400, 1000, 1200, -1, 100, -1,   0, 0 },
#endif
};

u32 var8005ecd8 = 0x00000000;
u32 var8005ecdc = 0x00000000;
u32 var8005ece0 = 0x00000000;
u32 var8005ece4 = 0x00000000;
u32 var8005ece8 = 0x00000000;
u32 var8005ecec = 0x00000000;
u32 var8005ecf0 = 0x00000000;
u32 var8005ecf4 = 0x00000000;

s16 var8005ecf8[] = {
	0x6665,
	0x5998,
	0x5fff,
	0x4ccc,
	0x5fff,
	0x2ccc,
	0x5998,
	0x6665,
	0x4ccc,
	0x5998,
	0x5998,
	0x5998,
	0x5fff,
	0x6665,
	0x6665,
	0x5fff,
	0x5998,
	0x6665,
	0x5fff,
	0x5fff,
	0x5fff,
	0x6665,
	0x5fff,
	0x5fff,
	0x5fff,
	0x7332,
	0x5fff,
	0x6665,
	0x5fff,
#if VERSION >= VERSION_NTSC_1_0
	0x5998,
	0x5998,
#else
	0x5fff,
	0x5fff,
#endif
	0x5332,
	0x5332,
	0x5fff,
	0x3332,
#if VERSION >= VERSION_NTSC_1_0
	0x4ccc,
#else
	0x3fff,
#endif
	0x5fff,
	0x4ccc,
	0x3fff,
	0x5fff,
	0x5fff,
	0x5fff,
	0x5fff,
	0x4ccc,
	0x4ccc,
	0x4ccc,
	0x4ccc,
	0x4ccc,
	0x4ccc,
	0x4ccc,
	0x5fff,
	0x5fff,
	0x5fff,
	0x5fff,
	0x5fff,
	0x5fff,
	0x5fff,
	0x4ccc,
	0x5fff,
	0x5fff,
	0x5fff,
	0x5fff,
	0x5fff,
	0x5fff,
	0x5fff,
	0x4ccc,
	0x4ccc,
	0x4ccc,
	0x4ccc,
	0x4ccc,
	0x4ccc,
	0x7332,
	0x7332,
	0x7332,
	0x4ccc,
#if VERSION >= VERSION_NTSC_1_0
	0x3c28,
#else
	0x4ccc,
#endif
	0x4ccc,
	0x4ccc,
	0x4ccc,
	0x4ccc,
	0x4ccc,
	0x4ccc,
	0x4ccc,
	0x4ccc,
	0x4ccc,
	0x4ccc,
	0x4ccc,
	0x4ccc,
	0x7332,
	0x7332,
	0x4ccc,
	0x4ccc,
	0x4ccc,
	0x4ccc,
	0x4ccc,
	0x6665,
	0x4ccc,
	0x4ccc,
	0x4ccc,
	0x4ccc,
	0x6ccb,
	0x5fff,
	0x5998,
	0x7998,
	0x5998,
	0x5998,
	0x6665,
	0x5fff,
	0x5fff,
	0x7332,
	0x7332,
	0x7332,
	0x7332,
	0x7332,
	0x4ccc,
	0x6665,
	0x6665,
	0x6665,
	0x4ccc,
	0x6ccb,
	0xffff,
	0x0000,
};

extern u8 _sfxctlSegmentRomStart;
extern u8 _sfxtblSegmentRomStart;
extern u8 _seqctlSegmentRomStart;
extern u8 _seqctlSegmentRomEnd;
extern u8 _seqtblSegmentRomStart;
extern u8 _sequencesSegmentRomStart;

bool sndIsPlayingMp3(void)
{
	return g_SndCurMp3.playing;
}

u16 snd0000e9dc(void)
{
#if VERSION >= VERSION_NTSC_1_0
	s32 result;

	if (func00033ec4(0) < 0x5000) {
		result = func00033ec4(0);
	} else {
		result = 0x5000;
	}

	return result;
#else
	return func00033ec4(0);
#endif
}

void sndSetSfxVolume(u16 volume)
{
	u8 i;

#if VERSION >= VERSION_NTSC_1_0
	if (volume > 0x5000) {
		volume = 0x5000;
	}
#endif

	for (i = 0; i < 9; i++) {
		func00033f44(i, volume);
	}

	g_SfxVolume = volume;
}

void snd0000ea80(u16 volume)
{
	u8 i;

#if VERSION >= VERSION_NTSC_1_0
	if (volume > 0x5000) {
		volume = 0x5000;
	}
#endif

	for (i = 0; i < 9; i++) {
		func00033f44(i, volume);
	}
}

void sndResetCurMp3(void)
{
	g_SndCurMp3.sfxref.id = 0;
	g_SndCurMp3.sfxref.mp3priority = 0;
	g_SndCurMp3.sfxref.unk02 = 0;
	g_SndCurMp3.sfxref.hasconfig = false;
	g_SndCurMp3.playing = false;
	g_SndCurMp3.unk08 = 0;
	g_SndCurMp3.responsetimer240 = -1;
	g_SndCurMp3.prevwhisper = -1;
	g_SndCurMp3.prevacknowledge = -1;
	g_SndCurMp3.prevgreeting = -1;
}

#if VERSION >= VERSION_NTSC_1_0
void sndLoadSfxCtl(void)
{
	s32 i;
	u8 unalignedbuffer[256 + 16];
	u8 *buffer;
	ALBankFile *file;
	ALBank *bank;
	u32 romaddr;
	u32 size;

	g_ALSoundRomOffsets = NULL;
	buffer = (u8 *) ALIGN16((u32)unalignedbuffer);

	// Load the first 256 bytes of the ctl file.
	size = 256;
	dmaExec(buffer, (u32) &_sfxctlSegmentRomStart, size);

	// Get the ROM address of the first (and only) bank,
	// then load the first 256 bytes of the bank.
	file = (ALBankFile *) buffer;
	romaddr = (u32)&_sfxctlSegmentRomStart;
	romaddr += (u32)file->bankArray[0];
	dmaExec(buffer, romaddr, size);

	// Get the ROM address of the first (and only) instrument,
	// then load the first 256 bytes of the instrument.
	bank = (ALBank *) buffer;
	romaddr = (u32)&_sfxctlSegmentRomStart;
	romaddr += (u32)bank->instArray[0];
	dmaExec(buffer, romaddr, size);

	// Get the soundCount (spoiler: there's 1545+1).
	// The final one might be a null terminator?
	// Or accounting for 1-based indexing of soundnums.
	g_NumSounds = ((ALInstrument *)buffer)->soundCount + 1;

	// Calculate the size of the ALInstrument and load it. The pointer is then
	// shifted forward to point to the instrument's ALSound array. This leaks
	// some memory but this is initialisation code so it's not much of an issue.
	size = g_NumSounds * 4 + 20;
	size = ALIGN16(size);
	g_ALSoundRomOffsets = alHeapAlloc(&g_SndHeap, 1, size);
	dmaExec(g_ALSoundRomOffsets, romaddr, size);

	*(u32 *)&g_ALSoundRomOffsets += 0x10;

	// Convert ctl-local offsets to ROM offsets
	for (i = 0; i < g_NumSounds; i++) {
		g_ALSoundRomOffsets[i] += (u32) &_sfxctlSegmentRomStart;
	}

	// Allocate and initialise cache
	g_SndCache.indexes = alHeapAlloc(&g_SndHeap, sizeof(u16), g_NumSounds);

	for (i = 0; i < (u32)g_NumSounds; i++) {
		g_SndCache.indexes[i] = -1;
	}

	for (i = 0; i < 45; i++) {
		g_SndCache.ages[i] = 1;
		g_SndCache.permanent[i] = 0;
	}
}
#else
GLOBAL_ASM(
glabel sndLoadSfxCtl
/*     f2d4:	27bdfe90 */ 	addiu	$sp,$sp,-368
/*     f2d8:	afb20028 */ 	sw	$s2,0x28($sp)
/*     f2dc:	03a09025 */ 	or	$s2,$sp,$zero
/*     f2e0:	afb10024 */ 	sw	$s1,0x24($sp)
/*     f2e4:	afb00020 */ 	sw	$s0,0x20($sp)
/*     f2e8:	2652006b */ 	addiu	$s2,$s2,0x6b
/*     f2ec:	3c118009 */ 	lui	$s1,0x8009
/*     f2f0:	364e000f */ 	ori	$t6,$s2,0xf
/*     f2f4:	3c10007c */ 	lui	$s0,0x7c
/*     f2f8:	26317e24 */ 	addiu	$s1,$s1,0x7e24
/*     f2fc:	afbf0034 */ 	sw	$ra,0x34($sp)
/*     f300:	2610e940 */ 	addiu	$s0,$s0,-5824
/*     f304:	39c4000f */ 	xori	$a0,$t6,0xf
/*     f308:	afb40030 */ 	sw	$s4,0x30($sp)
/*     f30c:	afb3002c */ 	sw	$s3,0x2c($sp)
/*     f310:	ae200000 */ 	sw	$zero,0x0($s1)
/*     f314:	00809025 */ 	or	$s2,$a0,$zero
/*     f318:	02002825 */ 	or	$a1,$s0,$zero
/*     f31c:	0c003664 */ 	jal	dmaExec
/*     f320:	24060100 */ 	addiu	$a2,$zero,0x100
/*     f324:	8e580004 */ 	lw	$t8,0x4($s2)
/*     f328:	02402025 */ 	or	$a0,$s2,$zero
/*     f32c:	24060100 */ 	addiu	$a2,$zero,0x100
/*     f330:	0c003664 */ 	jal	dmaExec
/*     f334:	02182821 */ 	addu	$a1,$s0,$t8
/*     f338:	8e59000c */ 	lw	$t9,0xc($s2)
/*     f33c:	02402025 */ 	or	$a0,$s2,$zero
/*     f340:	24060100 */ 	addiu	$a2,$zero,0x100
/*     f344:	0219a021 */ 	addu	$s4,$s0,$t9
/*     f348:	0c003664 */ 	jal	dmaExec
/*     f34c:	02802825 */ 	or	$a1,$s4,$zero
/*     f350:	864a000e */ 	lh	$t2,0xe($s2)
/*     f354:	3c098009 */ 	lui	$t1,0x8009
/*     f358:	3c018009 */ 	lui	$at,0x8009
/*     f35c:	254b0001 */ 	addiu	$t3,$t2,0x1
/*     f360:	25297e20 */ 	addiu	$t1,$t1,0x7e20
/*     f364:	ac2b7e20 */ 	sw	$t3,0x7e20($at)
/*     f368:	8d330000 */ 	lw	$s3,0x0($t1)
/*     f36c:	3c06800a */ 	lui	$a2,0x800a
/*     f370:	24c68170 */ 	addiu	$a2,$a2,-32400
/*     f374:	00136080 */ 	sll	$t4,$s3,0x2
/*     f378:	01809825 */ 	or	$s3,$t4,$zero
/*     f37c:	26730023 */ 	addiu	$s3,$s3,0x23
/*     f380:	366d000f */ 	ori	$t5,$s3,0xf
/*     f384:	39b3000f */ 	xori	$s3,$t5,0xf
/*     f388:	afb30010 */ 	sw	$s3,0x10($sp)
/*     f38c:	00002025 */ 	or	$a0,$zero,$zero
/*     f390:	00002825 */ 	or	$a1,$zero,$zero
/*     f394:	0c00c499 */ 	jal	alHeapDBAlloc
/*     f398:	24070001 */ 	addiu	$a3,$zero,0x1
/*     f39c:	ae220000 */ 	sw	$v0,0x0($s1)
/*     f3a0:	00402025 */ 	or	$a0,$v0,$zero
/*     f3a4:	02802825 */ 	or	$a1,$s4,$zero
/*     f3a8:	0c003664 */ 	jal	dmaExec
/*     f3ac:	02603025 */ 	or	$a2,$s3,$zero
/*     f3b0:	3c098009 */ 	lui	$t1,0x8009
/*     f3b4:	8e2f0000 */ 	lw	$t7,0x0($s1)
/*     f3b8:	25297e20 */ 	addiu	$t1,$t1,0x7e20
/*     f3bc:	8d280000 */ 	lw	$t0,0x0($t1)
/*     f3c0:	25f80010 */ 	addiu	$t8,$t7,0x10
/*     f3c4:	ae380000 */ 	sw	$t8,0x0($s1)
/*     f3c8:	1900000e */ 	blez	$t0,.NB0000f404
/*     f3cc:	00001825 */ 	or	$v1,$zero,$zero
/*     f3d0:	00002025 */ 	or	$a0,$zero,$zero
/*     f3d4:	8e390000 */ 	lw	$t9,0x0($s1)
.NB0000f3d8:
/*     f3d8:	24630001 */ 	addiu	$v1,$v1,0x1
/*     f3dc:	03241021 */ 	addu	$v0,$t9,$a0
/*     f3e0:	8c4a0000 */ 	lw	$t2,0x0($v0)
/*     f3e4:	24840004 */ 	addiu	$a0,$a0,0x4
/*     f3e8:	01505821 */ 	addu	$t3,$t2,$s0
/*     f3ec:	ac4b0000 */ 	sw	$t3,0x0($v0)
/*     f3f0:	8d280000 */ 	lw	$t0,0x0($t1)
/*     f3f4:	0068082a */ 	slt	$at,$v1,$t0
/*     f3f8:	5420fff7 */ 	bnezl	$at,.NB0000f3d8
/*     f3fc:	8e390000 */ 	lw	$t9,0x0($s1)
/*     f400:	00001825 */ 	or	$v1,$zero,$zero
.NB0000f404:
/*     f404:	3c06800a */ 	lui	$a2,0x800a
/*     f408:	24c68170 */ 	addiu	$a2,$a2,-32400
/*     f40c:	00002025 */ 	or	$a0,$zero,$zero
/*     f410:	00002825 */ 	or	$a1,$zero,$zero
/*     f414:	24070002 */ 	addiu	$a3,$zero,0x2
/*     f418:	afa80010 */ 	sw	$t0,0x10($sp)
/*     f41c:	0c00c499 */ 	jal	alHeapDBAlloc
/*     f420:	afa3016c */ 	sw	$v1,0x16c($sp)
/*     f424:	3c098009 */ 	lui	$t1,0x8009
/*     f428:	25297e20 */ 	addiu	$t1,$t1,0x7e20
/*     f42c:	8d2c0000 */ 	lw	$t4,0x0($t1)
/*     f430:	3c05800a */ 	lui	$a1,0x800a
/*     f434:	24a58190 */ 	addiu	$a1,$a1,-32368
/*     f438:	aca20000 */ 	sw	$v0,0x0($a1)
/*     f43c:	1180000c */ 	beqz	$t4,.NB0000f470
/*     f440:	8fa3016c */ 	lw	$v1,0x16c($sp)
/*     f444:	00001025 */ 	or	$v0,$zero,$zero
/*     f448:	3404ffff */ 	dli	$a0,0xffff
/*     f44c:	8cad0000 */ 	lw	$t5,0x0($a1)
.NB0000f450:
/*     f450:	24630001 */ 	addiu	$v1,$v1,0x1
/*     f454:	01a27021 */ 	addu	$t6,$t5,$v0
/*     f458:	a5c40000 */ 	sh	$a0,0x0($t6)
/*     f45c:	8d2f0000 */ 	lw	$t7,0x0($t1)
/*     f460:	24420002 */ 	addiu	$v0,$v0,0x2
/*     f464:	006f082b */ 	sltu	$at,$v1,$t7
/*     f468:	5420fff9 */ 	bnezl	$at,.NB0000f450
/*     f46c:	8cad0000 */ 	lw	$t5,0x0($a1)
.NB0000f470:
/*     f470:	3c18800a */ 	lui	$t8,0x800a
/*     f474:	27028190 */ 	addiu	$v0,$t8,-32368
/*     f478:	3c05800a */ 	lui	$a1,0x800a
/*     f47c:	3c04800a */ 	lui	$a0,0x800a
/*     f480:	248481bd */ 	addiu	$a0,$a0,-32323
/*     f484:	94a5ea1c */ 	lhu	$a1,-0x15e4($a1)
/*     f488:	00401825 */ 	or	$v1,$v0,$zero
.NB0000f48c:
/*     f48c:	24420001 */ 	addiu	$v0,$v0,0x1
/*     f490:	a4650032 */ 	sh	$a1,0x32($v1)
/*     f494:	24630002 */ 	addiu	$v1,$v1,0x2
/*     f498:	1444fffc */ 	bne	$v0,$a0,.NB0000f48c
/*     f49c:	a0400003 */ 	sb	$zero,0x3($v0)
/*     f4a0:	8fbf0034 */ 	lw	$ra,0x34($sp)
/*     f4a4:	8fb00020 */ 	lw	$s0,0x20($sp)
/*     f4a8:	8fb10024 */ 	lw	$s1,0x24($sp)
/*     f4ac:	8fb20028 */ 	lw	$s2,0x28($sp)
/*     f4b0:	8fb3002c */ 	lw	$s3,0x2c($sp)
/*     f4b4:	8fb40030 */ 	lw	$s4,0x30($sp)
/*     f4b8:	03e00008 */ 	jr	$ra
/*     f4bc:	27bd0170 */ 	addiu	$sp,$sp,0x170
);
#endif

#if VERSION >= VERSION_NTSC_1_0
void sndIncrementAges(void)
{
	s32 i;

	for (i = 0; i < 45; i++) {
		if (!g_SndCache.permanent[i] && g_SndCache.ages[i] < 32000) {
			g_SndCache.ages[i]++;
		}
	}
}
#endif

#if VERSION >= VERSION_NTSC_1_0
GLOBAL_ASM(
glabel sndLoadEnvelope
/*     ed68:	27bdff10 */ 	addiu	$sp,$sp,-240
/*     ed6c:	afb20020 */ 	sw	$s2,0x20($sp)
/*     ed70:	afb1001c */ 	sw	$s1,0x1c($sp)
/*     ed74:	03a08825 */ 	or	$s1,$sp,$zero
/*     ed78:	03a09025 */ 	or	$s2,$sp,$zero
/*     ed7c:	3c0e0081 */ 	lui	$t6,%hi(_sfxctlSegmentRomStart)
/*     ed80:	265200af */ 	addiu	$s2,$s2,0xaf
/*     ed84:	2631005f */ 	addiu	$s1,$s1,0x5f
/*     ed88:	afb40028 */ 	sw	$s4,0x28($sp)
/*     ed8c:	afb30024 */ 	sw	$s3,0x24($sp)
/*     ed90:	25cea250 */ 	addiu	$t6,$t6,%lo(_sfxctlSegmentRomStart)
/*     ed94:	3639000f */ 	ori	$t9,$s1,0xf
/*     ed98:	364f000f */ 	ori	$t7,$s2,0xf
/*     ed9c:	afbf002c */ 	sw	$ra,0x2c($sp)
/*     eda0:	afb00018 */ 	sw	$s0,0x18($sp)
/*     eda4:	afa500f4 */ 	sw	$a1,0xf4($sp)
/*     eda8:	008ea021 */ 	addu	$s4,$a0,$t6
/*     edac:	39f2000f */ 	xori	$s2,$t7,0xf
/*     edb0:	3b31000f */ 	xori	$s1,$t9,0xf
/*     edb4:	24130040 */ 	addiu	$s3,$zero,0x40
/*     edb8:	02402025 */ 	or	$a0,$s2,$zero
.L0000edbc:
/*     edbc:	02802825 */ 	or	$a1,$s4,$zero
/*     edc0:	0c003513 */ 	jal	dmaExecHighPriority
/*     edc4:	24060040 */ 	addiu	$a2,$zero,0x40
/*     edc8:	00008025 */ 	or	$s0,$zero,$zero
/*     edcc:	00001825 */ 	or	$v1,$zero,$zero
/*     edd0:	00001025 */ 	or	$v0,$zero,$zero
.L0000edd4:
/*     edd4:	02424821 */ 	addu	$t1,$s2,$v0
/*     edd8:	8d2a0000 */ 	lw	$t2,0x0($t1)
/*     eddc:	24630001 */ 	addiu	$v1,$v1,0x1
/*     ede0:	2c610010 */ 	sltiu	$at,$v1,0x10
/*     ede4:	24420004 */ 	addiu	$v0,$v0,0x4
/*     ede8:	1420fffa */ 	bnez	$at,.L0000edd4
/*     edec:	020a8021 */ 	addu	$s0,$s0,$t2
/*     edf0:	02202025 */ 	or	$a0,$s1,$zero
/*     edf4:	02802825 */ 	or	$a1,$s4,$zero
/*     edf8:	0c003513 */ 	jal	dmaExecHighPriority
/*     edfc:	24060040 */ 	addiu	$a2,$zero,0x40
/*     ee00:	00002025 */ 	or	$a0,$zero,$zero
/*     ee04:	00001025 */ 	or	$v0,$zero,$zero
.L0000ee08:
/*     ee08:	02225821 */ 	addu	$t3,$s1,$v0
/*     ee0c:	8d6c0000 */ 	lw	$t4,0x0($t3)
/*     ee10:	24420004 */ 	addiu	$v0,$v0,0x4
/*     ee14:	1453fffc */ 	bne	$v0,$s3,.L0000ee08
/*     ee18:	008c2021 */ 	addu	$a0,$a0,$t4
/*     ee1c:	5604ffe7 */ 	bnel	$s0,$a0,.L0000edbc
/*     ee20:	02402025 */ 	or	$a0,$s2,$zero
/*     ee24:	97ad00f6 */ 	lhu	$t5,0xf6($sp)
/*     ee28:	8e210000 */ 	lw	$at,0x0($s1)
/*     ee2c:	3c038009 */ 	lui	$v1,%hi(g_SndCache)
/*     ee30:	24635210 */ 	addiu	$v1,$v1,%lo(g_SndCache)
/*     ee34:	000d7100 */ 	sll	$t6,$t5,0x4
/*     ee38:	006e7821 */ 	addu	$t7,$v1,$t6
/*     ee3c:	ade1008c */ 	sw	$at,0x8c($t7)
/*     ee40:	8e390004 */ 	lw	$t9,0x4($s1)
/*     ee44:	adf90090 */ 	sw	$t9,0x90($t7)
/*     ee48:	8e210008 */ 	lw	$at,0x8($s1)
/*     ee4c:	ade10094 */ 	sw	$at,0x94($t7)
/*     ee50:	8e39000c */ 	lw	$t9,0xc($s1)
/*     ee54:	adf90098 */ 	sw	$t9,0x98($t7)
/*     ee58:	97a800f6 */ 	lhu	$t0,0xf6($sp)
/*     ee5c:	8fbf002c */ 	lw	$ra,0x2c($sp)
/*     ee60:	8fb40028 */ 	lw	$s4,0x28($sp)
/*     ee64:	00084900 */ 	sll	$t1,$t0,0x4
/*     ee68:	00691021 */ 	addu	$v0,$v1,$t1
/*     ee6c:	8fb30024 */ 	lw	$s3,0x24($sp)
/*     ee70:	8fb20020 */ 	lw	$s2,0x20($sp)
/*     ee74:	8fb1001c */ 	lw	$s1,0x1c($sp)
/*     ee78:	8fb00018 */ 	lw	$s0,0x18($sp)
/*     ee7c:	27bd00f0 */ 	addiu	$sp,$sp,0xf0
/*     ee80:	03e00008 */ 	jr	$ra
/*     ee84:	2442008c */ 	addiu	$v0,$v0,0x8c
);
#else
GLOBAL_ASM(
glabel sndLoadEnvelope
/*     f4c0:	27bdff90 */ 	addiu	$sp,$sp,-112
/*     f4c4:	3c0e007c */ 	lui	$t6,0x7c
/*     f4c8:	afa50074 */ 	sw	$a1,0x74($sp)
/*     f4cc:	25cee940 */ 	addiu	$t6,$t6,-5824
/*     f4d0:	afa40070 */ 	sw	$a0,0x70($sp)
/*     f4d4:	008e2821 */ 	addu	$a1,$a0,$t6
/*     f4d8:	03a02025 */ 	or	$a0,$sp,$zero
/*     f4dc:	2484002f */ 	addiu	$a0,$a0,0x2f
/*     f4e0:	348f000f */ 	ori	$t7,$a0,0xf
/*     f4e4:	afbf0014 */ 	sw	$ra,0x14($sp)
/*     f4e8:	39e4000f */ 	xori	$a0,$t7,0xf
/*     f4ec:	afa40018 */ 	sw	$a0,0x18($sp)
/*     f4f0:	0c00366e */ 	jal	dmaExecHighPriority
/*     f4f4:	24060040 */ 	addiu	$a2,$zero,0x40
/*     f4f8:	8fa40018 */ 	lw	$a0,0x18($sp)
/*     f4fc:	97a50076 */ 	lhu	$a1,0x76($sp)
/*     f500:	3c03800a */ 	lui	$v1,0x800a
/*     f504:	8c810000 */ 	lw	$at,0x0($a0)
/*     f508:	24638190 */ 	addiu	$v1,$v1,-32368
/*     f50c:	0005c900 */ 	sll	$t9,$a1,0x4
/*     f510:	00794021 */ 	addu	$t0,$v1,$t9
/*     f514:	ad01008c */ 	sw	$at,0x8c($t0)
/*     f518:	8c8b0004 */ 	lw	$t3,0x4($a0)
/*     f51c:	00056100 */ 	sll	$t4,$a1,0x4
/*     f520:	006c1021 */ 	addu	$v0,$v1,$t4
/*     f524:	ad0b0090 */ 	sw	$t3,0x90($t0)
/*     f528:	8c810008 */ 	lw	$at,0x8($a0)
/*     f52c:	2442008c */ 	addiu	$v0,$v0,0x8c
/*     f530:	ad010094 */ 	sw	$at,0x94($t0)
/*     f534:	8c8b000c */ 	lw	$t3,0xc($a0)
/*     f538:	ad0b0098 */ 	sw	$t3,0x98($t0)
/*     f53c:	8fbf0014 */ 	lw	$ra,0x14($sp)
/*     f540:	27bd0070 */ 	addiu	$sp,$sp,0x70
/*     f544:	03e00008 */ 	jr	$ra
/*     f548:	00000000 */ 	sll	$zero,$zero,0x0
);
#endif

#if VERSION >= VERSION_NTSC_1_0
GLOBAL_ASM(
glabel sndLoadKeymap
/*     ee88:	27bdff10 */ 	addiu	$sp,$sp,-240
/*     ee8c:	afb20020 */ 	sw	$s2,0x20($sp)
/*     ee90:	afb1001c */ 	sw	$s1,0x1c($sp)
/*     ee94:	03a08825 */ 	or	$s1,$sp,$zero
/*     ee98:	03a09025 */ 	or	$s2,$sp,$zero
/*     ee9c:	3c0e0081 */ 	lui	$t6,%hi(_sfxctlSegmentRomStart)
/*     eea0:	265200af */ 	addiu	$s2,$s2,0xaf
/*     eea4:	2631005f */ 	addiu	$s1,$s1,0x5f
/*     eea8:	afb40028 */ 	sw	$s4,0x28($sp)
/*     eeac:	afb30024 */ 	sw	$s3,0x24($sp)
/*     eeb0:	25cea250 */ 	addiu	$t6,$t6,%lo(_sfxctlSegmentRomStart)
/*     eeb4:	3639000f */ 	ori	$t9,$s1,0xf
/*     eeb8:	364f000f */ 	ori	$t7,$s2,0xf
/*     eebc:	afbf002c */ 	sw	$ra,0x2c($sp)
/*     eec0:	afb00018 */ 	sw	$s0,0x18($sp)
/*     eec4:	afa500f4 */ 	sw	$a1,0xf4($sp)
/*     eec8:	008ea021 */ 	addu	$s4,$a0,$t6
/*     eecc:	39f2000f */ 	xori	$s2,$t7,0xf
/*     eed0:	3b31000f */ 	xori	$s1,$t9,0xf
/*     eed4:	24130040 */ 	addiu	$s3,$zero,0x40
/*     eed8:	02402025 */ 	or	$a0,$s2,$zero
.L0000eedc:
/*     eedc:	02802825 */ 	or	$a1,$s4,$zero
/*     eee0:	0c003513 */ 	jal	dmaExecHighPriority
/*     eee4:	24060040 */ 	addiu	$a2,$zero,0x40
/*     eee8:	00008025 */ 	or	$s0,$zero,$zero
/*     eeec:	00001825 */ 	or	$v1,$zero,$zero
/*     eef0:	00001025 */ 	or	$v0,$zero,$zero
.L0000eef4:
/*     eef4:	02424821 */ 	addu	$t1,$s2,$v0
/*     eef8:	8d2a0000 */ 	lw	$t2,0x0($t1)
/*     eefc:	24630001 */ 	addiu	$v1,$v1,0x1
/*     ef00:	2c610010 */ 	sltiu	$at,$v1,0x10
/*     ef04:	24420004 */ 	addiu	$v0,$v0,0x4
/*     ef08:	1420fffa */ 	bnez	$at,.L0000eef4
/*     ef0c:	020a8021 */ 	addu	$s0,$s0,$t2
/*     ef10:	02202025 */ 	or	$a0,$s1,$zero
/*     ef14:	02802825 */ 	or	$a1,$s4,$zero
/*     ef18:	0c003513 */ 	jal	dmaExecHighPriority
/*     ef1c:	24060040 */ 	addiu	$a2,$zero,0x40
/*     ef20:	00002025 */ 	or	$a0,$zero,$zero
/*     ef24:	00001025 */ 	or	$v0,$zero,$zero
.L0000ef28:
/*     ef28:	02225821 */ 	addu	$t3,$s1,$v0
/*     ef2c:	8d6c0000 */ 	lw	$t4,0x0($t3)
/*     ef30:	24420004 */ 	addiu	$v0,$v0,0x4
/*     ef34:	1453fffc */ 	bne	$v0,$s3,.L0000ef28
/*     ef38:	008c2021 */ 	addu	$a0,$a0,$t4
/*     ef3c:	5604ffe7 */ 	bnel	$s0,$a0,.L0000eedc
/*     ef40:	02402025 */ 	or	$a0,$s2,$zero
/*     ef44:	97a300f6 */ 	lhu	$v1,0xf6($sp)
/*     ef48:	8a210000 */ 	lwl	$at,0x0($s1)
/*     ef4c:	9a210003 */ 	lwr	$at,0x3($s1)
/*     ef50:	00036880 */ 	sll	$t5,$v1,0x2
/*     ef54:	3c048009 */ 	lui	$a0,%hi(g_SndCache)
/*     ef58:	01a36823 */ 	subu	$t5,$t5,$v1
/*     ef5c:	24845210 */ 	addiu	$a0,$a0,%lo(g_SndCache)
/*     ef60:	000d6840 */ 	sll	$t5,$t5,0x1
/*     ef64:	008d7021 */ 	addu	$t6,$a0,$t5
/*     ef68:	a9c1035c */ 	swl	$at,0x35c($t6)
/*     ef6c:	b9c1035f */ 	swr	$at,0x35f($t6)
/*     ef70:	92210004 */ 	lbu	$at,0x4($s1)
/*     ef74:	008d1021 */ 	addu	$v0,$a0,$t5
/*     ef78:	2442035c */ 	addiu	$v0,$v0,0x35c
/*     ef7c:	a1c10360 */ 	sb	$at,0x360($t6)
/*     ef80:	92380005 */ 	lbu	$t8,0x5($s1)
/*     ef84:	a1d80361 */ 	sb	$t8,0x361($t6)
/*     ef88:	8fbf002c */ 	lw	$ra,0x2c($sp)
/*     ef8c:	8fb40028 */ 	lw	$s4,0x28($sp)
/*     ef90:	8fb30024 */ 	lw	$s3,0x24($sp)
/*     ef94:	8fb20020 */ 	lw	$s2,0x20($sp)
/*     ef98:	8fb1001c */ 	lw	$s1,0x1c($sp)
/*     ef9c:	8fb00018 */ 	lw	$s0,0x18($sp)
/*     efa0:	03e00008 */ 	jr	$ra
/*     efa4:	27bd00f0 */ 	addiu	$sp,$sp,0xf0
);
#else
GLOBAL_ASM(
glabel sndLoadKeymap
/*     f54c:	27bdff90 */ 	addiu	$sp,$sp,-112
/*     f550:	3c0e007c */ 	lui	$t6,0x7c
/*     f554:	afa50074 */ 	sw	$a1,0x74($sp)
/*     f558:	25cee940 */ 	addiu	$t6,$t6,-5824
/*     f55c:	afa40070 */ 	sw	$a0,0x70($sp)
/*     f560:	008e2821 */ 	addu	$a1,$a0,$t6
/*     f564:	03a02025 */ 	or	$a0,$sp,$zero
/*     f568:	2484002f */ 	addiu	$a0,$a0,0x2f
/*     f56c:	348f000f */ 	ori	$t7,$a0,0xf
/*     f570:	afbf0014 */ 	sw	$ra,0x14($sp)
/*     f574:	39e4000f */ 	xori	$a0,$t7,0xf
/*     f578:	afa40018 */ 	sw	$a0,0x18($sp)
/*     f57c:	0c00366e */ 	jal	dmaExecHighPriority
/*     f580:	24060040 */ 	addiu	$a2,$zero,0x40
/*     f584:	8fa40018 */ 	lw	$a0,0x18($sp)
/*     f588:	97a30076 */ 	lhu	$v1,0x76($sp)
/*     f58c:	3c05800a */ 	lui	$a1,0x800a
/*     f590:	88810000 */ 	lwl	$at,0x0($a0)
/*     f594:	0003c880 */ 	sll	$t9,$v1,0x2
/*     f598:	98810003 */ 	lwr	$at,0x3($a0)
/*     f59c:	0323c823 */ 	subu	$t9,$t9,$v1
/*     f5a0:	24a58190 */ 	addiu	$a1,$a1,-32368
/*     f5a4:	0019c840 */ 	sll	$t9,$t9,0x1
/*     f5a8:	00b94021 */ 	addu	$t0,$a1,$t9
/*     f5ac:	a901035c */ 	swl	$at,0x35c($t0)
/*     f5b0:	b901035f */ 	swr	$at,0x35f($t0)
/*     f5b4:	90810004 */ 	lbu	$at,0x4($a0)
/*     f5b8:	00b91021 */ 	addu	$v0,$a1,$t9
/*     f5bc:	2442035c */ 	addiu	$v0,$v0,0x35c
/*     f5c0:	a1010360 */ 	sb	$at,0x360($t0)
/*     f5c4:	908b0005 */ 	lbu	$t3,0x5($a0)
/*     f5c8:	a10b0361 */ 	sb	$t3,0x361($t0)
/*     f5cc:	8fbf0014 */ 	lw	$ra,0x14($sp)
/*     f5d0:	27bd0070 */ 	addiu	$sp,$sp,0x70
/*     f5d4:	03e00008 */ 	jr	$ra
/*     f5d8:	00000000 */ 	sll	$zero,$zero,0x0
);
#endif

// Mismatch:
// Goal calculates the address of g_SndCache.unk35c twice.
// The below demonstrates similar codegen by not adding arg1 to the return value.
//struct var80095210_35c *sndLoadKeymap(u32 segoffset, u16 arg1)
//{
//#if VERSION >= VERSION_NTSC_1_0
//	u8 spaf[0x50];
//	u8 sp5f[0x50];
//	u32 *s2 = (u32 *)ALIGN16((u32)spaf);
//	u32 *s1 = (u32 *)ALIGN16((u32)sp5f);
//	s32 i;
//	s32 sum1;
//	s32 sum2;
//
//	segoffset += (u32)&_sfxctlSegmentRomStart;
//
//	do {
//		dmaExecHighPriority(s2, segoffset, 0x40);
//		sum1 = 0;
//
//		for (i = 0; i < 16U; i++) {
//			sum1 += s2[i];
//		}
//
//		dmaExecHighPriority(s1, segoffset, 0x40);
//		sum2 = 0;
//
//		for (i = 0; i < 16U; i++) {
//			sum2 += s1[i];
//		}
//	} while (sum1 != sum2);
//#else
//	u8 sp5f[0x50];
//	u32 *s1 = (u32 *)ALIGN16((u32)sp5f);
//
//	segoffset += (u32)&_sfxctlSegmentRomStart;
//
//	dmaExecHighPriority(s1, segoffset, 0x40);
//#endif
//
//	g_SndCache.unk35c[arg1] = *(struct var80095210_35c *)s1;
//
//	//return &g_SndCache.unk35c[arg1];
//	return (struct var80095210_35c *)((u32)&g_SndCache + 0x35c);
//}

#if VERSION >= VERSION_NTSC_1_0
GLOBAL_ASM(
glabel snd0000efa8
/*     efa8:	27bdfd10 */ 	addiu	$sp,$sp,-752
/*     efac:	afb20020 */ 	sw	$s2,0x20($sp)
/*     efb0:	afb1001c */ 	sw	$s1,0x1c($sp)
/*     efb4:	03a08825 */ 	or	$s1,$sp,$zero
/*     efb8:	03a09025 */ 	or	$s2,$sp,$zero
/*     efbc:	3c0e0081 */ 	lui	$t6,%hi(_sfxctlSegmentRomStart)
/*     efc0:	265201af */ 	addiu	$s2,$s2,0x1af
/*     efc4:	2631005f */ 	addiu	$s1,$s1,0x5f
/*     efc8:	afb40028 */ 	sw	$s4,0x28($sp)
/*     efcc:	afb30024 */ 	sw	$s3,0x24($sp)
/*     efd0:	25cea250 */ 	addiu	$t6,$t6,%lo(_sfxctlSegmentRomStart)
/*     efd4:	3639000f */ 	ori	$t9,$s1,0xf
/*     efd8:	364f000f */ 	ori	$t7,$s2,0xf
/*     efdc:	afbf002c */ 	sw	$ra,0x2c($sp)
/*     efe0:	afb00018 */ 	sw	$s0,0x18($sp)
/*     efe4:	afa502f4 */ 	sw	$a1,0x2f4($sp)
/*     efe8:	008ea021 */ 	addu	$s4,$a0,$t6
/*     efec:	39f2000f */ 	xori	$s2,$t7,0xf
/*     eff0:	3b31000f */ 	xori	$s1,$t9,0xf
/*     eff4:	24130140 */ 	addiu	$s3,$zero,0x140
/*     eff8:	02402025 */ 	or	$a0,$s2,$zero
.L0000effc:
/*     effc:	02802825 */ 	or	$a1,$s4,$zero
/*     f000:	0c003513 */ 	jal	dmaExecHighPriority
/*     f004:	24060140 */ 	addiu	$a2,$zero,0x140
/*     f008:	00008025 */ 	or	$s0,$zero,$zero
/*     f00c:	00001825 */ 	or	$v1,$zero,$zero
/*     f010:	00001025 */ 	or	$v0,$zero,$zero
.L0000f014:
/*     f014:	02424821 */ 	addu	$t1,$s2,$v0
/*     f018:	8d2a0000 */ 	lw	$t2,0x0($t1)
/*     f01c:	24630001 */ 	addiu	$v1,$v1,0x1
/*     f020:	2c610050 */ 	sltiu	$at,$v1,0x50
/*     f024:	24420004 */ 	addiu	$v0,$v0,0x4
/*     f028:	1420fffa */ 	bnez	$at,.L0000f014
/*     f02c:	020a8021 */ 	addu	$s0,$s0,$t2
/*     f030:	02202025 */ 	or	$a0,$s1,$zero
/*     f034:	02802825 */ 	or	$a1,$s4,$zero
/*     f038:	0c003513 */ 	jal	dmaExecHighPriority
/*     f03c:	24060140 */ 	addiu	$a2,$zero,0x140
/*     f040:	00002025 */ 	or	$a0,$zero,$zero
/*     f044:	00001025 */ 	or	$v0,$zero,$zero
.L0000f048:
/*     f048:	02225821 */ 	addu	$t3,$s1,$v0
/*     f04c:	8d6c0000 */ 	lw	$t4,0x0($t3)
/*     f050:	24420004 */ 	addiu	$v0,$v0,0x4
/*     f054:	1453fffc */ 	bne	$v0,$s3,.L0000f048
/*     f058:	008c2021 */ 	addu	$a0,$a0,$t4
/*     f05c:	5604ffe7 */ 	bnel	$s0,$a0,.L0000effc
/*     f060:	02402025 */ 	or	$a0,$s2,$zero
/*     f064:	97a302f6 */ 	lhu	$v1,0x2f6($sp)
/*     f068:	3c048009 */ 	lui	$a0,%hi(g_SndCache)
/*     f06c:	24845210 */ 	addiu	$a0,$a0,%lo(g_SndCache)
/*     f070:	00036940 */ 	sll	$t5,$v1,0x5
/*     f074:	01a36821 */ 	addu	$t5,$t5,$v1
/*     f078:	000d18c0 */ 	sll	$v1,$t5,0x3
/*     f07c:	00837021 */ 	addu	$t6,$a0,$v1
/*     f080:	0220c825 */ 	or	$t9,$s1,$zero
/*     f084:	26380108 */ 	addiu	$t8,$s1,0x108
.L0000f088:
/*     f088:	8f210000 */ 	lw	$at,0x0($t9)
/*     f08c:	2739000c */ 	addiu	$t9,$t9,0xc
/*     f090:	25ce000c */ 	addiu	$t6,$t6,0xc
/*     f094:	adc107e4 */ 	sw	$at,0x7e4($t6)
/*     f098:	8f21fff8 */ 	lw	$at,-0x8($t9)
/*     f09c:	adc107e8 */ 	sw	$at,0x7e8($t6)
/*     f0a0:	8f21fffc */ 	lw	$at,-0x4($t9)
/*     f0a4:	1738fff8 */ 	bne	$t9,$t8,.L0000f088
/*     f0a8:	adc107ec */ 	sw	$at,0x7ec($t6)
/*     f0ac:	8fbf002c */ 	lw	$ra,0x2c($sp)
/*     f0b0:	00831021 */ 	addu	$v0,$a0,$v1
/*     f0b4:	8fb00018 */ 	lw	$s0,0x18($sp)
/*     f0b8:	8fb1001c */ 	lw	$s1,0x1c($sp)
/*     f0bc:	8fb20020 */ 	lw	$s2,0x20($sp)
/*     f0c0:	8fb30024 */ 	lw	$s3,0x24($sp)
/*     f0c4:	8fb40028 */ 	lw	$s4,0x28($sp)
/*     f0c8:	27bd02f0 */ 	addiu	$sp,$sp,0x2f0
/*     f0cc:	03e00008 */ 	jr	$ra
/*     f0d0:	244207f0 */ 	addiu	$v0,$v0,0x7f0
);
#else
GLOBAL_ASM(
glabel snd0000efa8
/*     f5dc:	27bdfe90 */ 	addiu	$sp,$sp,-368
/*     f5e0:	3c0e007c */ 	lui	$t6,0x7c
/*     f5e4:	afa50174 */ 	sw	$a1,0x174($sp)
/*     f5e8:	25cee940 */ 	addiu	$t6,$t6,-5824
/*     f5ec:	afa40170 */ 	sw	$a0,0x170($sp)
/*     f5f0:	008e2821 */ 	addu	$a1,$a0,$t6
/*     f5f4:	03a02025 */ 	or	$a0,$sp,$zero
/*     f5f8:	2484002f */ 	addiu	$a0,$a0,0x2f
/*     f5fc:	348f000f */ 	ori	$t7,$a0,0xf
/*     f600:	afbf0014 */ 	sw	$ra,0x14($sp)
/*     f604:	39e4000f */ 	xori	$a0,$t7,0xf
/*     f608:	afa40018 */ 	sw	$a0,0x18($sp)
/*     f60c:	0c00366e */ 	jal	dmaExecHighPriority
/*     f610:	24060140 */ 	addiu	$a2,$zero,0x140
/*     f614:	97a30176 */ 	lhu	$v1,0x176($sp)
/*     f618:	8fac0018 */ 	lw	$t4,0x18($sp)
/*     f61c:	3c05800a */ 	lui	$a1,0x800a
/*     f620:	0003c940 */ 	sll	$t9,$v1,0x5
/*     f624:	0323c821 */ 	addu	$t9,$t9,$v1
/*     f628:	24a58190 */ 	addiu	$a1,$a1,-32368
/*     f62c:	001918c0 */ 	sll	$v1,$t9,0x3
/*     f630:	00a34021 */ 	addu	$t0,$a1,$v1
/*     f634:	258b0108 */ 	addiu	$t3,$t4,0x108
.NB0000f638:
/*     f638:	8d810000 */ 	lw	$at,0x0($t4)
/*     f63c:	258c000c */ 	addiu	$t4,$t4,0xc
/*     f640:	2508000c */ 	addiu	$t0,$t0,0xc
/*     f644:	ad0107e4 */ 	sw	$at,0x7e4($t0)
/*     f648:	8d81fff8 */ 	lw	$at,-0x8($t4)
/*     f64c:	ad0107e8 */ 	sw	$at,0x7e8($t0)
/*     f650:	8d81fffc */ 	lw	$at,-0x4($t4)
/*     f654:	158bfff8 */ 	bne	$t4,$t3,.NB0000f638
/*     f658:	ad0107ec */ 	sw	$at,0x7ec($t0)
/*     f65c:	8fbf0014 */ 	lw	$ra,0x14($sp)
/*     f660:	00a31021 */ 	addu	$v0,$a1,$v1
/*     f664:	244207f0 */ 	addiu	$v0,$v0,0x7f0
/*     f668:	03e00008 */ 	jr	$ra
/*     f66c:	27bd0170 */ 	addiu	$sp,$sp,0x170
);
#endif

#if VERSION >= VERSION_NTSC_1_0
GLOBAL_ASM(
glabel snd0000f0d4
/*     f0d4:	27bdff10 */ 	addiu	$sp,$sp,-240
/*     f0d8:	afb40028 */ 	sw	$s4,0x28($sp)
/*     f0dc:	0080a025 */ 	or	$s4,$a0,$zero
/*     f0e0:	afbf002c */ 	sw	$ra,0x2c($sp)
/*     f0e4:	afb30024 */ 	sw	$s3,0x24($sp)
/*     f0e8:	afb20020 */ 	sw	$s2,0x20($sp)
/*     f0ec:	afb1001c */ 	sw	$s1,0x1c($sp)
/*     f0f0:	afb00018 */ 	sw	$s0,0x18($sp)
/*     f0f4:	14800003 */ 	bnez	$a0,.L0000f104
/*     f0f8:	afa500f4 */ 	sw	$a1,0xf4($sp)
/*     f0fc:	10000042 */ 	b	.L0000f208
/*     f100:	00001025 */ 	or	$v0,$zero,$zero
.L0000f104:
/*     f104:	03a09025 */ 	or	$s2,$sp,$zero
/*     f108:	03a08825 */ 	or	$s1,$sp,$zero
/*     f10c:	3c0e0081 */ 	lui	$t6,%hi(_sfxctlSegmentRomStart)
/*     f110:	2631005f */ 	addiu	$s1,$s1,0x5f
/*     f114:	265200af */ 	addiu	$s2,$s2,0xaf
/*     f118:	25cea250 */ 	addiu	$t6,$t6,%lo(_sfxctlSegmentRomStart)
/*     f11c:	364f000f */ 	ori	$t7,$s2,0xf
/*     f120:	3639000f */ 	ori	$t9,$s1,0xf
/*     f124:	028ea021 */ 	addu	$s4,$s4,$t6
/*     f128:	3b31000f */ 	xori	$s1,$t9,0xf
/*     f12c:	39f2000f */ 	xori	$s2,$t7,0xf
/*     f130:	24130040 */ 	addiu	$s3,$zero,0x40
/*     f134:	02402025 */ 	or	$a0,$s2,$zero
.L0000f138:
/*     f138:	02802825 */ 	or	$a1,$s4,$zero
/*     f13c:	0c003513 */ 	jal	dmaExecHighPriority
/*     f140:	24060040 */ 	addiu	$a2,$zero,0x40
/*     f144:	00008025 */ 	or	$s0,$zero,$zero
/*     f148:	00001825 */ 	or	$v1,$zero,$zero
/*     f14c:	00001025 */ 	or	$v0,$zero,$zero
.L0000f150:
/*     f150:	02424821 */ 	addu	$t1,$s2,$v0
/*     f154:	8d2a0000 */ 	lw	$t2,0x0($t1)
/*     f158:	24630001 */ 	addiu	$v1,$v1,0x1
/*     f15c:	2c610010 */ 	sltiu	$at,$v1,0x10
/*     f160:	24420004 */ 	addiu	$v0,$v0,0x4
/*     f164:	1420fffa */ 	bnez	$at,.L0000f150
/*     f168:	020a8021 */ 	addu	$s0,$s0,$t2
/*     f16c:	02202025 */ 	or	$a0,$s1,$zero
/*     f170:	02802825 */ 	or	$a1,$s4,$zero
/*     f174:	0c003513 */ 	jal	dmaExecHighPriority
/*     f178:	24060040 */ 	addiu	$a2,$zero,0x40
/*     f17c:	00002025 */ 	or	$a0,$zero,$zero
/*     f180:	00001025 */ 	or	$v0,$zero,$zero
.L0000f184:
/*     f184:	02225821 */ 	addu	$t3,$s1,$v0
/*     f188:	8d6c0000 */ 	lw	$t4,0x0($t3)
/*     f18c:	24420004 */ 	addiu	$v0,$v0,0x4
/*     f190:	1453fffc */ 	bne	$v0,$s3,.L0000f184
/*     f194:	008c2021 */ 	addu	$a0,$a0,$t4
/*     f198:	5604ffe7 */ 	bnel	$s0,$a0,.L0000f138
/*     f19c:	02402025 */ 	or	$a0,$s2,$zero
/*     f1a0:	97a300f6 */ 	lhu	$v1,0xf6($sp)
/*     f1a4:	3c048009 */ 	lui	$a0,%hi(g_SndCache)
/*     f1a8:	24845210 */ 	addiu	$a0,$a0,%lo(g_SndCache)
/*     f1ac:	00036880 */ 	sll	$t5,$v1,0x2
/*     f1b0:	01a36823 */ 	subu	$t5,$t5,$v1
/*     f1b4:	000d6880 */ 	sll	$t5,$t5,0x2
/*     f1b8:	01a36823 */ 	subu	$t5,$t5,$v1
/*     f1bc:	000d1880 */ 	sll	$v1,$t5,0x2
/*     f1c0:	00837021 */ 	addu	$t6,$a0,$v1
/*     f1c4:	0220c825 */ 	or	$t9,$s1,$zero
/*     f1c8:	26380024 */ 	addiu	$t8,$s1,0x24
.L0000f1cc:
/*     f1cc:	8f210000 */ 	lw	$at,0x0($t9)
/*     f1d0:	2739000c */ 	addiu	$t9,$t9,0xc
/*     f1d4:	25ce000c */ 	addiu	$t6,$t6,0xc
/*     f1d8:	adc1364c */ 	sw	$at,0x364c($t6)
/*     f1dc:	8f21fff8 */ 	lw	$at,-0x8($t9)
/*     f1e0:	adc13650 */ 	sw	$at,0x3650($t6)
/*     f1e4:	8f21fffc */ 	lw	$at,-0x4($t9)
/*     f1e8:	1738fff8 */ 	bne	$t9,$t8,.L0000f1cc
/*     f1ec:	adc13654 */ 	sw	$at,0x3654($t6)
/*     f1f0:	8f210000 */ 	lw	$at,0x0($t9)
/*     f1f4:	00831021 */ 	addu	$v0,$a0,$v1
/*     f1f8:	24423658 */ 	addiu	$v0,$v0,0x3658
/*     f1fc:	adc13658 */ 	sw	$at,0x3658($t6)
/*     f200:	8f380004 */ 	lw	$t8,0x4($t9)
/*     f204:	add8365c */ 	sw	$t8,0x365c($t6)
.L0000f208:
/*     f208:	8fbf002c */ 	lw	$ra,0x2c($sp)
/*     f20c:	8fb00018 */ 	lw	$s0,0x18($sp)
/*     f210:	8fb1001c */ 	lw	$s1,0x1c($sp)
/*     f214:	8fb20020 */ 	lw	$s2,0x20($sp)
/*     f218:	8fb30024 */ 	lw	$s3,0x24($sp)
/*     f21c:	8fb40028 */ 	lw	$s4,0x28($sp)
/*     f220:	03e00008 */ 	jr	$ra
/*     f224:	27bd00f0 */ 	addiu	$sp,$sp,0xf0
);
#else
GLOBAL_ASM(
glabel snd0000f0d4
/*     f670:	27bdff90 */ 	addiu	$sp,$sp,-112
/*     f674:	afbf0014 */ 	sw	$ra,0x14($sp)
/*     f678:	afa50074 */ 	sw	$a1,0x74($sp)
/*     f67c:	14800003 */ 	bnez	$a0,.NB0000f68c
/*     f680:	00803825 */ 	or	$a3,$a0,$zero
/*     f684:	10000025 */ 	beqz	$zero,.NB0000f71c
/*     f688:	00001025 */ 	or	$v0,$zero,$zero
.NB0000f68c:
/*     f68c:	03a02025 */ 	or	$a0,$sp,$zero
/*     f690:	2484002f */ 	addiu	$a0,$a0,0x2f
/*     f694:	3c0e007c */ 	lui	$t6,0x7c
/*     f698:	348f000f */ 	ori	$t7,$a0,0xf
/*     f69c:	25cee940 */ 	addiu	$t6,$t6,-5824
/*     f6a0:	39e4000f */ 	xori	$a0,$t7,0xf
/*     f6a4:	00ee2821 */ 	addu	$a1,$a3,$t6
/*     f6a8:	afa40018 */ 	sw	$a0,0x18($sp)
/*     f6ac:	0c00366e */ 	jal	dmaExecHighPriority
/*     f6b0:	24060040 */ 	addiu	$a2,$zero,0x40
/*     f6b4:	97a30076 */ 	lhu	$v1,0x76($sp)
/*     f6b8:	8fac0018 */ 	lw	$t4,0x18($sp)
/*     f6bc:	3c05800a */ 	lui	$a1,0x800a
/*     f6c0:	0003c880 */ 	sll	$t9,$v1,0x2
/*     f6c4:	0323c823 */ 	subu	$t9,$t9,$v1
/*     f6c8:	0019c880 */ 	sll	$t9,$t9,0x2
/*     f6cc:	0323c823 */ 	subu	$t9,$t9,$v1
/*     f6d0:	24a58190 */ 	addiu	$a1,$a1,-32368
/*     f6d4:	00191880 */ 	sll	$v1,$t9,0x2
/*     f6d8:	00a34021 */ 	addu	$t0,$a1,$v1
/*     f6dc:	258b0024 */ 	addiu	$t3,$t4,0x24
.NB0000f6e0:
/*     f6e0:	8d810000 */ 	lw	$at,0x0($t4)
/*     f6e4:	258c000c */ 	addiu	$t4,$t4,0xc
/*     f6e8:	2508000c */ 	addiu	$t0,$t0,0xc
/*     f6ec:	ad01364c */ 	sw	$at,0x364c($t0)
/*     f6f0:	8d81fff8 */ 	lw	$at,-0x8($t4)
/*     f6f4:	ad013650 */ 	sw	$at,0x3650($t0)
/*     f6f8:	8d81fffc */ 	lw	$at,-0x4($t4)
/*     f6fc:	158bfff8 */ 	bne	$t4,$t3,.NB0000f6e0
/*     f700:	ad013654 */ 	sw	$at,0x3654($t0)
/*     f704:	8d810000 */ 	lw	$at,0x0($t4)
/*     f708:	00a31021 */ 	addu	$v0,$a1,$v1
/*     f70c:	24423658 */ 	addiu	$v0,$v0,0x3658
/*     f710:	ad013658 */ 	sw	$at,0x3658($t0)
/*     f714:	8d8b0004 */ 	lw	$t3,0x4($t4)
/*     f718:	ad0b365c */ 	sw	$t3,0x365c($t0)
.NB0000f71c:
/*     f71c:	8fbf0014 */ 	lw	$ra,0x14($sp)
/*     f720:	27bd0070 */ 	addiu	$sp,$sp,0x70
/*     f724:	03e00008 */ 	jr	$ra
/*     f728:	00000000 */ 	sll	$zero,$zero,0x0
);
#endif

#if VERSION >= VERSION_NTSC_1_0
GLOBAL_ASM(
glabel sndLoadWavetable
/*     f228:	27bdff08 */ 	addiu	$sp,$sp,-248
/*     f22c:	afb20020 */ 	sw	$s2,0x20($sp)
/*     f230:	afb1001c */ 	sw	$s1,0x1c($sp)
/*     f234:	03a08825 */ 	or	$s1,$sp,$zero
/*     f238:	03a09025 */ 	or	$s2,$sp,$zero
/*     f23c:	3c0e0081 */ 	lui	$t6,%hi(_sfxctlSegmentRomStart)
/*     f240:	265200b7 */ 	addiu	$s2,$s2,0xb7
/*     f244:	26310067 */ 	addiu	$s1,$s1,0x67
/*     f248:	afb40028 */ 	sw	$s4,0x28($sp)
/*     f24c:	afb30024 */ 	sw	$s3,0x24($sp)
/*     f250:	25cea250 */ 	addiu	$t6,$t6,%lo(_sfxctlSegmentRomStart)
/*     f254:	3639000f */ 	ori	$t9,$s1,0xf
/*     f258:	364f000f */ 	ori	$t7,$s2,0xf
/*     f25c:	afbf002c */ 	sw	$ra,0x2c($sp)
/*     f260:	afb00018 */ 	sw	$s0,0x18($sp)
/*     f264:	afa500fc */ 	sw	$a1,0xfc($sp)
/*     f268:	008ea021 */ 	addu	$s4,$a0,$t6
/*     f26c:	39f2000f */ 	xori	$s2,$t7,0xf
/*     f270:	3b31000f */ 	xori	$s1,$t9,0xf
/*     f274:	24130040 */ 	addiu	$s3,$zero,0x40
/*     f278:	02402025 */ 	or	$a0,$s2,$zero
.L0000f27c:
/*     f27c:	02802825 */ 	or	$a1,$s4,$zero
/*     f280:	0c003513 */ 	jal	dmaExecHighPriority
/*     f284:	24060040 */ 	addiu	$a2,$zero,0x40
/*     f288:	00008025 */ 	or	$s0,$zero,$zero
/*     f28c:	00001825 */ 	or	$v1,$zero,$zero
/*     f290:	00001025 */ 	or	$v0,$zero,$zero
.L0000f294:
/*     f294:	02424821 */ 	addu	$t1,$s2,$v0
/*     f298:	8d2a0000 */ 	lw	$t2,0x0($t1)
/*     f29c:	24630001 */ 	addiu	$v1,$v1,0x1
/*     f2a0:	2c610010 */ 	sltiu	$at,$v1,0x10
/*     f2a4:	24420004 */ 	addiu	$v0,$v0,0x4
/*     f2a8:	1420fffa */ 	bnez	$at,.L0000f294
/*     f2ac:	020a8021 */ 	addu	$s0,$s0,$t2
/*     f2b0:	02202025 */ 	or	$a0,$s1,$zero
/*     f2b4:	02802825 */ 	or	$a1,$s4,$zero
/*     f2b8:	0c003513 */ 	jal	dmaExecHighPriority
/*     f2bc:	24060040 */ 	addiu	$a2,$zero,0x40
/*     f2c0:	00002025 */ 	or	$a0,$zero,$zero
/*     f2c4:	00001025 */ 	or	$v0,$zero,$zero
.L0000f2c8:
/*     f2c8:	02225821 */ 	addu	$t3,$s1,$v0
/*     f2cc:	8d6c0000 */ 	lw	$t4,0x0($t3)
/*     f2d0:	24420004 */ 	addiu	$v0,$v0,0x4
/*     f2d4:	1453fffc */ 	bne	$v0,$s3,.L0000f2c8
/*     f2d8:	008c2021 */ 	addu	$a0,$a0,$t4
/*     f2dc:	5604ffe7 */ 	bnel	$s0,$a0,.L0000f27c
/*     f2e0:	02402025 */ 	or	$a0,$s2,$zero
/*     f2e4:	97ad00fe */ 	lhu	$t5,0xfe($sp)
/*     f2e8:	3c0f8009 */ 	lui	$t7,%hi(g_SndCache)
/*     f2ec:	8e210000 */ 	lw	$at,0x0($s1)
/*     f2f0:	000d7080 */ 	sll	$t6,$t5,0x2
/*     f2f4:	01cd7021 */ 	addu	$t6,$t6,$t5
/*     f2f8:	000e7080 */ 	sll	$t6,$t6,0x2
/*     f2fc:	25ef5210 */ 	addiu	$t7,$t7,%lo(g_SndCache)
/*     f300:	01cf1021 */ 	addu	$v0,$t6,$t7
/*     f304:	2450046c */ 	addiu	$s0,$v0,0x46c
/*     f308:	ae010000 */ 	sw	$at,0x0($s0)
/*     f30c:	8e390004 */ 	lw	$t9,0x4($s1)
/*     f310:	3c090084 */ 	lui	$t1,%hi(_sfxtblSegmentRomStart)
/*     f314:	25299dd0 */ 	addiu	$t1,$t1,%lo(_sfxtblSegmentRomStart)
/*     f318:	ae190004 */ 	sw	$t9,0x4($s0)
/*     f31c:	8e210008 */ 	lw	$at,0x8($s1)
/*     f320:	ae010008 */ 	sw	$at,0x8($s0)
/*     f324:	8e39000c */ 	lw	$t9,0xc($s1)
/*     f328:	ae19000c */ 	sw	$t9,0xc($s0)
/*     f32c:	8e210010 */ 	lw	$at,0x10($s1)
/*     f330:	ae010010 */ 	sw	$at,0x10($s0)
/*     f334:	8c48046c */ 	lw	$t0,0x46c($v0)
/*     f338:	904b0474 */ 	lbu	$t3,0x474($v0)
/*     f33c:	01095021 */ 	addu	$t2,$t0,$t1
/*     f340:	15600009 */ 	bnez	$t3,.L0000f368
/*     f344:	ac4a046c */ 	sw	$t2,0x46c($v0)
/*     f348:	8e040010 */ 	lw	$a0,0x10($s0)
/*     f34c:	0c003bea */ 	jal	snd0000efa8
/*     f350:	97a500fe */ 	lhu	$a1,0xfe($sp)
/*     f354:	ae020010 */ 	sw	$v0,0x10($s0)
/*     f358:	8e04000c */ 	lw	$a0,0xc($s0)
/*     f35c:	0c003c35 */ 	jal	snd0000f0d4
/*     f360:	97a500fe */ 	lhu	$a1,0xfe($sp)
/*     f364:	ae02000c */ 	sw	$v0,0xc($s0)
.L0000f368:
/*     f368:	8fbf002c */ 	lw	$ra,0x2c($sp)
/*     f36c:	02001025 */ 	or	$v0,$s0,$zero
/*     f370:	8fb00018 */ 	lw	$s0,0x18($sp)
/*     f374:	8fb1001c */ 	lw	$s1,0x1c($sp)
/*     f378:	8fb20020 */ 	lw	$s2,0x20($sp)
/*     f37c:	8fb30024 */ 	lw	$s3,0x24($sp)
/*     f380:	8fb40028 */ 	lw	$s4,0x28($sp)
/*     f384:	03e00008 */ 	jr	$ra
/*     f388:	27bd00f8 */ 	addiu	$sp,$sp,0xf8
);
#else
GLOBAL_ASM(
glabel sndLoadWavetable
/*     f72c:	27bdff88 */ 	addiu	$sp,$sp,-120
/*     f730:	3c0e007c */ 	lui	$t6,0x7c
/*     f734:	afa5007c */ 	sw	$a1,0x7c($sp)
/*     f738:	25cee940 */ 	addiu	$t6,$t6,-5824
/*     f73c:	afa40078 */ 	sw	$a0,0x78($sp)
/*     f740:	008e2821 */ 	addu	$a1,$a0,$t6
/*     f744:	03a02025 */ 	or	$a0,$sp,$zero
/*     f748:	24840037 */ 	addiu	$a0,$a0,0x37
/*     f74c:	348f000f */ 	ori	$t7,$a0,0xf
/*     f750:	afbf0014 */ 	sw	$ra,0x14($sp)
/*     f754:	39e4000f */ 	xori	$a0,$t7,0xf
/*     f758:	afa4001c */ 	sw	$a0,0x1c($sp)
/*     f75c:	0c00366e */ 	jal	dmaExecHighPriority
/*     f760:	24060040 */ 	addiu	$a2,$zero,0x40
/*     f764:	97a5007e */ 	lhu	$a1,0x7e($sp)
/*     f768:	8fa4001c */ 	lw	$a0,0x1c($sp)
/*     f76c:	3c08800a */ 	lui	$t0,0x800a
/*     f770:	0005c880 */ 	sll	$t9,$a1,0x2
/*     f774:	0325c821 */ 	addu	$t9,$t9,$a1
/*     f778:	8c810000 */ 	lw	$at,0x0($a0)
/*     f77c:	0019c880 */ 	sll	$t9,$t9,0x2
/*     f780:	25088190 */ 	addiu	$t0,$t0,-32368
/*     f784:	03281021 */ 	addu	$v0,$t9,$t0
/*     f788:	2443046c */ 	addiu	$v1,$v0,0x46c
/*     f78c:	ac610000 */ 	sw	$at,0x0($v1)
/*     f790:	8c8b0004 */ 	lw	$t3,0x4($a0)
/*     f794:	3c0d007f */ 	lui	$t5,0x7f
/*     f798:	25ade4c0 */ 	addiu	$t5,$t5,-6976
/*     f79c:	ac6b0004 */ 	sw	$t3,0x4($v1)
/*     f7a0:	8c810008 */ 	lw	$at,0x8($a0)
/*     f7a4:	ac610008 */ 	sw	$at,0x8($v1)
/*     f7a8:	8c8b000c */ 	lw	$t3,0xc($a0)
/*     f7ac:	ac6b000c */ 	sw	$t3,0xc($v1)
/*     f7b0:	8c810010 */ 	lw	$at,0x10($a0)
/*     f7b4:	ac610010 */ 	sw	$at,0x10($v1)
/*     f7b8:	8c4c046c */ 	lw	$t4,0x46c($v0)
/*     f7bc:	904f0474 */ 	lbu	$t7,0x474($v0)
/*     f7c0:	018d7021 */ 	addu	$t6,$t4,$t5
/*     f7c4:	15e0000b */ 	bnez	$t7,.NB0000f7f4
/*     f7c8:	ac4e046c */ 	sw	$t6,0x46c($v0)
/*     f7cc:	8c640010 */ 	lw	$a0,0x10($v1)
/*     f7d0:	0c003d77 */ 	jal	snd0000efa8
/*     f7d4:	afa30018 */ 	sw	$v1,0x18($sp)
/*     f7d8:	8fa30018 */ 	lw	$v1,0x18($sp)
/*     f7dc:	97a5007e */ 	lhu	$a1,0x7e($sp)
/*     f7e0:	ac620010 */ 	sw	$v0,0x10($v1)
/*     f7e4:	0c003d9c */ 	jal	snd0000f0d4
/*     f7e8:	8c64000c */ 	lw	$a0,0xc($v1)
/*     f7ec:	8fa30018 */ 	lw	$v1,0x18($sp)
/*     f7f0:	ac62000c */ 	sw	$v0,0xc($v1)
.NB0000f7f4:
/*     f7f4:	8fbf0014 */ 	lw	$ra,0x14($sp)
/*     f7f8:	27bd0078 */ 	addiu	$sp,$sp,0x78
/*     f7fc:	00601025 */ 	or	$v0,$v1,$zero
/*     f800:	03e00008 */ 	jr	$ra
/*     f804:	00000000 */ 	sll	$zero,$zero,0x0
);
#endif

void sndSetSoundMode(s32 mode)
{
	s32 i;

	g_SoundMode = mode;

	switch (mode) {
	case SOUNDMODE_MONO:
		speakersSetMode(SPEAKERMODE_MONO);
		break;
	case SOUNDMODE_STEREO:
		speakersSetMode(SPEAKERMODE_STEREO);
		break;
	case SOUNDMODE_HEADPHONE:
		speakersSetMode(SPEAKERMODE_HEADPHONE);
		break;
	case SOUNDMODE_SURROUND:
		speakersSetMode(SPEAKERMODE_SURROUND);
		break;
	}

	speaker00034104(0, 4);

	for (i = 1; i < g_SndMaxFxBusses; i++) {
		switch (mode) {
		case SOUNDMODE_STEREO:
		case SOUNDMODE_HEADPHONE:
		case SOUNDMODE_SURROUND:
			speaker00034104(i, 4);
			break;
		}
	}
}

#if VERSION >= VERSION_NTSC_1_0
ALSound *sndLoadSound(s16 soundnum)
{
	union soundnumhack tmp;
	u16 cacheindex;
	u16 *ptr;
	ALSound *sound;
	s32 oldestindex;
	u32 oldestage;
	s32 i;
	s16 sfxnum;
	u8 sp47[0x58];

	sound = (ALSound *)ALIGN16((u32)sp47);

	tmp.packed = soundnum;
	sfxnum = tmp.id;

	if (sndIsMp3(tmp.packed)) {
		return NULL;
	}

	// If this sound doesn't exist in the cache
	cacheindex = g_SndCache.indexes[sfxnum];

	if (cacheindex == 0xffff) {
		// Find the oldest cache item
		oldestindex = -1;
		oldestage = 0;

		for (i = 0; i < 45; i++) {
			if (!g_SndCache.permanent[i] && oldestage < g_SndCache.ages[i]) {
				oldestage = g_SndCache.ages[i];
				oldestindex = i;
			}
		}

		cacheindex = oldestindex;

		// Remove the old sound from the cacheindexes list
		ptr = g_SndCache.indexes;

		for (i = 0; i < (u32)g_NumSounds; i++) {
			if (*ptr == cacheindex) {
				*ptr = 0xffff;
			}

			ptr++;
		}

		// DMA the ALSound data
		dmaExecHighPriority(sound, g_ALSoundRomOffsets[sfxnum - 1], 0x40);

		// Promote segment offsets to pointers and load their child data
		sound->envelope = sndLoadEnvelope((u32)sound->envelope, cacheindex);
		sound->wavetable = sndLoadWavetable((u32)sound->wavetable, cacheindex);
		sound->keyMap = sndLoadKeymap((u32)sound->keyMap, cacheindex);

		// Save the ALSound into the cache
		g_SndCache.sounds[cacheindex] = *sound;

		g_SndCache.indexes[sfxnum] = cacheindex;
	}

	// Reset this cache item's age, even if it already existed in the cache
	g_SndCache.ages[cacheindex] = 1;

	return &g_SndCache.sounds[cacheindex];
}
#else
GLOBAL_ASM(
glabel sndLoadSound
/*     f918:	27bdff50 */ 	addiu	$sp,$sp,-176
/*     f91c:	afb00014 */ 	sw	$s0,0x14($sp)
/*     f920:	00048400 */ 	sll	$s0,$a0,0x10
/*     f924:	00107403 */ 	sra	$t6,$s0,0x10
/*     f928:	afa400b0 */ 	sw	$a0,0xb0($sp)
/*     f92c:	000e2400 */ 	sll	$a0,$t6,0x10
/*     f930:	afbf001c */ 	sw	$ra,0x1c($sp)
/*     f934:	00047c03 */ 	sra	$t7,$a0,0x10
/*     f938:	01c08025 */ 	or	$s0,$t6,$zero
/*     f93c:	afb10018 */ 	sw	$s1,0x18($sp)
/*     f940:	0c00400e */ 	jal	sndIsMp3
/*     f944:	01e02025 */ 	or	$a0,$t7,$zero
/*     f948:	10400003 */ 	beqz	$v0,.NB0000f958
/*     f94c:	3c08800a */ 	lui	$t0,0x800a
/*     f950:	1000006b */ 	beqz	$zero,.NB0000fb00
/*     f954:	00001025 */ 	or	$v0,$zero,$zero
.NB0000f958:
/*     f958:	25088190 */ 	addiu	$t0,$t0,-32368
/*     f95c:	320b07ff */ 	andi	$t3,$s0,0x7ff
/*     f960:	8d060000 */ 	lw	$a2,0x0($t0)
/*     f964:	000bc400 */ 	sll	$t8,$t3,0x10
/*     f968:	00185c03 */ 	sra	$t3,$t8,0x10
/*     f96c:	000b6040 */ 	sll	$t4,$t3,0x1
/*     f970:	00cc6821 */ 	addu	$t5,$a2,$t4
/*     f974:	95aa0000 */ 	lhu	$t2,0x0($t5)
/*     f978:	3401ffff */ 	dli	$at,0xffff
/*     f97c:	2407ffff */ 	addiu	$a3,$zero,-1
/*     f980:	15410056 */ 	bne	$t2,$at,.NB0000fadc
/*     f984:	00002825 */ 	or	$a1,$zero,$zero
/*     f988:	3c03800a */ 	lui	$v1,0x800a
/*     f98c:	3c09800a */ 	lui	$t1,0x800a
/*     f990:	2529e6c0 */ 	addiu	$t1,$t1,-6464
/*     f994:	24638190 */ 	addiu	$v1,$v1,-32368
/*     f998:	00001025 */ 	or	$v0,$zero,$zero
.NB0000f99c:
/*     f99c:	906e0004 */ 	lbu	$t6,0x4($v1)
/*     f9a0:	0002c040 */ 	sll	$t8,$v0,0x1
/*     f9a4:	0118c821 */ 	addu	$t9,$t0,$t8
/*     f9a8:	55c0000b */ 	bnezl	$t6,.NB0000f9d8
/*     f9ac:	24420001 */ 	addiu	$v0,$v0,0x1
/*     f9b0:	952f035c */ 	lhu	$t7,0x35c($t1)
/*     f9b4:	972d0032 */ 	lhu	$t5,0x32($t9)
/*     f9b8:	01ed2023 */ 	subu	$a0,$t7,$t5
/*     f9bc:	24840001 */ 	addiu	$a0,$a0,0x1
/*     f9c0:	00a4082b */ 	sltu	$at,$a1,$a0
/*     f9c4:	50200004 */ 	beqzl	$at,.NB0000f9d8
/*     f9c8:	24420001 */ 	addiu	$v0,$v0,0x1
/*     f9cc:	00802825 */ 	or	$a1,$a0,$zero
/*     f9d0:	00403825 */ 	or	$a3,$v0,$zero
/*     f9d4:	24420001 */ 	addiu	$v0,$v0,0x1
.NB0000f9d8:
/*     f9d8:	2841002d */ 	slti	$at,$v0,0x2d
/*     f9dc:	1420ffef */ 	bnez	$at,.NB0000f99c
/*     f9e0:	24630001 */ 	addiu	$v1,$v1,0x1
/*     f9e4:	3c048009 */ 	lui	$a0,0x8009
/*     f9e8:	8c847e20 */ 	lw	$a0,0x7e20($a0)
/*     f9ec:	30eaffff */ 	andi	$t2,$a3,0xffff
/*     f9f0:	00c01825 */ 	or	$v1,$a2,$zero
/*     f9f4:	1080000d */ 	beqz	$a0,.NB0000fa2c
/*     f9f8:	00001025 */ 	or	$v0,$zero,$zero
/*     f9fc:	30e5ffff */ 	andi	$a1,$a3,0xffff
/*     fa00:	3406ffff */ 	dli	$a2,0xffff
.NB0000fa04:
/*     fa04:	946e0000 */ 	lhu	$t6,0x0($v1)
/*     fa08:	24420001 */ 	addiu	$v0,$v0,0x1
/*     fa0c:	54ae0005 */ 	bnel	$a1,$t6,.NB0000fa24
/*     fa10:	0044082b */ 	sltu	$at,$v0,$a0
/*     fa14:	a4660000 */ 	sh	$a2,0x0($v1)
/*     fa18:	3c048009 */ 	lui	$a0,0x8009
/*     fa1c:	8c847e20 */ 	lw	$a0,0x7e20($a0)
/*     fa20:	0044082b */ 	sltu	$at,$v0,$a0
.NB0000fa24:
/*     fa24:	1420fff7 */ 	bnez	$at,.NB0000fa04
/*     fa28:	24630002 */ 	addiu	$v1,$v1,0x2
.NB0000fa2c:
/*     fa2c:	3c0f8009 */ 	lui	$t7,0x8009
/*     fa30:	8def7e24 */ 	lw	$t7,0x7e24($t7)
/*     fa34:	03a08025 */ 	or	$s0,$sp,$zero
/*     fa38:	26100047 */ 	addiu	$s0,$s0,0x47
/*     fa3c:	000b6880 */ 	sll	$t5,$t3,0x2
/*     fa40:	3618000f */ 	ori	$t8,$s0,0xf
/*     fa44:	01ed7021 */ 	addu	$t6,$t7,$t5
/*     fa48:	8dc5fffc */ 	lw	$a1,-0x4($t6)
/*     fa4c:	3b04000f */ 	xori	$a0,$t8,0xf
/*     fa50:	00808025 */ 	or	$s0,$a0,$zero
/*     fa54:	afac0024 */ 	sw	$t4,0x24($sp)
/*     fa58:	a7aa00aa */ 	sh	$t2,0xaa($sp)
/*     fa5c:	24060040 */ 	addiu	$a2,$zero,0x40
/*     fa60:	0c00366e */ 	jal	dmaExecHighPriority
/*     fa64:	30f1ffff */ 	andi	$s1,$a3,0xffff
/*     fa68:	8e040000 */ 	lw	$a0,0x0($s0)
/*     fa6c:	0c003d30 */ 	jal	sndLoadEnvelope
/*     fa70:	3225ffff */ 	andi	$a1,$s1,0xffff
/*     fa74:	ae020000 */ 	sw	$v0,0x0($s0)
/*     fa78:	8e040008 */ 	lw	$a0,0x8($s0)
/*     fa7c:	0c003dcb */ 	jal	sndLoadWavetable
/*     fa80:	3225ffff */ 	andi	$a1,$s1,0xffff
/*     fa84:	ae020008 */ 	sw	$v0,0x8($s0)
/*     fa88:	8e040004 */ 	lw	$a0,0x4($s0)
/*     fa8c:	0c003d53 */ 	jal	sndLoadKeymap
/*     fa90:	3225ffff */ 	andi	$a1,$s1,0xffff
/*     fa94:	8e010000 */ 	lw	$at,0x0($s0)
/*     fa98:	3c08800a */ 	lui	$t0,0x800a
/*     fa9c:	25088190 */ 	addiu	$t0,$t0,-32368
/*     faa0:	97aa00aa */ 	lhu	$t2,0xaa($sp)
/*     faa4:	0011c100 */ 	sll	$t8,$s1,0x4
/*     faa8:	ae020004 */ 	sw	$v0,0x4($s0)
/*     faac:	0118c821 */ 	addu	$t9,$t0,$t8
/*     fab0:	af213e14 */ 	sw	$at,0x3e14($t9)
/*     fab4:	8e0d0004 */ 	lw	$t5,0x4($s0)
/*     fab8:	af2d3e18 */ 	sw	$t5,0x3e18($t9)
/*     fabc:	8e010008 */ 	lw	$at,0x8($s0)
/*     fac0:	af213e1c */ 	sw	$at,0x3e1c($t9)
/*     fac4:	8e0d000c */ 	lw	$t5,0xc($s0)
/*     fac8:	af2d3e20 */ 	sw	$t5,0x3e20($t9)
/*     facc:	8fb80024 */ 	lw	$t8,0x24($sp)
/*     fad0:	8d0e0000 */ 	lw	$t6,0x0($t0)
/*     fad4:	01d87821 */ 	addu	$t7,$t6,$t8
/*     fad8:	a5f10000 */ 	sh	$s1,0x0($t7)
.NB0000fadc:
/*     fadc:	3c09800a */ 	lui	$t1,0x800a
/*     fae0:	2529e6c0 */ 	addiu	$t1,$t1,-6464
/*     fae4:	9539035c */ 	lhu	$t9,0x35c($t1)
/*     fae8:	000a6840 */ 	sll	$t5,$t2,0x1
/*     faec:	000ac100 */ 	sll	$t8,$t2,0x4
/*     faf0:	010d7021 */ 	addu	$t6,$t0,$t5
/*     faf4:	01181021 */ 	addu	$v0,$t0,$t8
/*     faf8:	24423e14 */ 	addiu	$v0,$v0,0x3e14
/*     fafc:	a5d90032 */ 	sh	$t9,0x32($t6)
.NB0000fb00:
/*     fb00:	8fbf001c */ 	lw	$ra,0x1c($sp)
/*     fb04:	8fb00014 */ 	lw	$s0,0x14($sp)
/*     fb08:	8fb10018 */ 	lw	$s1,0x18($sp)
/*     fb0c:	03e00008 */ 	jr	$ra
/*     fb10:	27bd00b0 */ 	addiu	$sp,$sp,0xb0
);
#endif

void seqInit(struct seqinstance *seq)
{
	u32 stack;
	ALSeqpConfig config;

	config.maxVoices = 44;
	config.maxEvents = 64;
	config.maxChannels = 16;
	config.debugFlags = 0;
	config.heap = &g_SndHeap;

	func00030c98(&config);

	if (IS4MB()) {
		g_SeqBufferSize = 0x3800;
	} else {
		g_SeqBufferSize = 0x4800;
	}

	seq->data = alHeapAlloc(&g_SndHeap, 1, g_SeqBufferSize);
	seq->seqp = alHeapAlloc(&g_SndHeap, 1, sizeof(N_ALCSPlayer));

	n_alCSPNew(seq->seqp, &config);

	n_alCSPSetBank(seq->seqp, var80095204);
}

void snd0000f76c(void);

GLOBAL_ASM(
glabel snd0000f76c
/*     f76c:	3c0e800a */ 	lui	$t6,%hi(g_SndCache+0x3e14)
/*     f770:	25ce9024 */ 	addiu	$t6,$t6,%lo(g_SndCache+0x3e14)
/*     f774:	008e082b */ 	sltu	$at,$a0,$t6
/*     f778:	1420000d */ 	bnez	$at,.L0000f7b0
/*     f77c:	3c0f800a */ 	lui	$t7,%hi(g_SndCache+0x40d4)
/*     f780:	25ef92e4 */ 	addiu	$t7,$t7,%lo(g_SndCache+0x40d4)
/*     f784:	01e4082b */ 	sltu	$at,$t7,$a0
/*     f788:	14200009 */ 	bnez	$at,.L0000f7b0
/*     f78c:	3c038009 */ 	lui	$v1,%hi(g_SndCache)
/*     f790:	24635210 */ 	addiu	$v1,$v1,%lo(g_SndCache)
/*     f794:	0083c023 */ 	subu	$t8,$a0,$v1
/*     f798:	2719c1ec */ 	addiu	$t9,$t8,-15892
/*     f79c:	00194103 */ 	sra	$t0,$t9,0x4
/*     f7a0:	00681021 */ 	addu	$v0,$v1,$t0
/*     f7a4:	90490004 */ 	lbu	$t1,0x4($v0)
/*     f7a8:	252a0001 */ 	addiu	$t2,$t1,0x1
/*     f7ac:	a04a0004 */ 	sb	$t2,0x4($v0)
.L0000f7b0:
/*     f7b0:	03e00008 */ 	jr	$ra
/*     f7b4:	00000000 */ 	nop
);

void snd0000f7b8(void);

GLOBAL_ASM(
glabel snd0000f7b8
/*     f7b8:	3c0e800a */ 	lui	$t6,%hi(g_SndCache+0x3e14)
/*     f7bc:	25ce9024 */ 	addiu	$t6,$t6,%lo(g_SndCache+0x3e14)
/*     f7c0:	008e082b */ 	sltu	$at,$a0,$t6
/*     f7c4:	1420000d */ 	bnez	$at,.L0000f7fc
/*     f7c8:	3c0f800a */ 	lui	$t7,%hi(g_SndCache+0x40d4)
/*     f7cc:	25ef92e4 */ 	addiu	$t7,$t7,%lo(g_SndCache+0x40d4)
/*     f7d0:	01e4082b */ 	sltu	$at,$t7,$a0
/*     f7d4:	14200009 */ 	bnez	$at,.L0000f7fc
/*     f7d8:	3c038009 */ 	lui	$v1,%hi(g_SndCache)
/*     f7dc:	24635210 */ 	addiu	$v1,$v1,%lo(g_SndCache)
/*     f7e0:	0083c023 */ 	subu	$t8,$a0,$v1
/*     f7e4:	2719c1ec */ 	addiu	$t9,$t8,-15892
/*     f7e8:	00194103 */ 	sra	$t0,$t9,0x4
/*     f7ec:	00681021 */ 	addu	$v0,$v1,$t0
/*     f7f0:	90490004 */ 	lbu	$t1,0x4($v0)
/*     f7f4:	252affff */ 	addiu	$t2,$t1,-1
/*     f7f8:	a04a0004 */ 	sb	$t2,0x4($v0)
.L0000f7fc:
/*     f7fc:	03e00008 */ 	jr	$ra
/*     f800:	00000000 */ 	nop
);

void sndInit(void)
{
	ALSndpConfig sndpconfig;

#if VERSION >= VERSION_PAL_FINAL
	u32 heaplen = 1024 * 446;
#elif VERSION >= VERSION_NTSC_1_0
	u32 heaplen = 1024 * 441;
#else
	u32 heaplen = 1024 * 438;
#endif

	g_Vars.langfilteron = false;

	if (IS4MB()) {
		g_SndMaxFxBusses = 1;

#if VERSION >= VERSION_PAL_FINAL
		heaplen -= 1024 * 6;
#else
		heaplen -= 1024 * 38;
#endif
		heaplen -= 1024 * 137;
		heaplen -= 1024 * 12;
		heaplen -= 1024 * 23;

		g_SndMp3Enabled = false;
	} else {
		g_SndMp3Enabled = true;
		g_SndMaxFxBusses = 2;

		if (argFindByPrefix(1, "-nomp3")) {
			g_SndMp3Enabled = false;
		}
	}

	if (!g_SndDisabled) {
		// Allocate memory for the audio heap,
		// clear it and give it to the audio library
		ALSynConfig synconfig;
#if VERSION >= VERSION_PAL_FINAL
		u32 settings[3];
#endif
		u8 *ptr = mempAlloc(heaplen, MEMPOOL_PERMANENT);
		u32 len = &_seqctlSegmentRomEnd - &_seqctlSegmentRomStart;
		s32 i;
		u32 seqromaddr = (u32) &_sequencesSegmentRomStart;
		u8 *heapstart = ptr;
		u8 *end = heapstart + heaplen;
		ALBankFile *bankfile;

		while (ptr < end) {
			*ptr = 0;
			ptr++;
		}

		alHeapInit(&g_SndHeap, heapstart, heaplen);

		// Allocate some space at the start of the heap for a string identifier.
		// This might be used to determine if the heap has overflowed.
		g_SndGuardStringPtr = alHeapAlloc(&g_SndHeap, 1, 32);
		strcpy(g_SndGuardStringPtr, g_SndGuardString);

		// Load sfx.ctl
		sndLoadSfxCtl();

		// Load seq.ctl
		var80095200 = 0xffffffff;
		bankfile = alHeapAlloc(&g_SndHeap, 1, len);
		dmaExec(bankfile, (u32) &_seqctlSegmentRomStart, len);

		// Load seq.tbl
		alBnkfNew(bankfile, &_seqtblSegmentRomStart);

		// Load the sequences table. To do this, load the header of the
		// sequences segment and read the number of sequences, then allocate
		// enough space for the table and load it.
		var80095204 = bankfile->bankArray[0];
		g_SeqTable = alHeapDBAlloc(0, 0, &g_SndHeap, 1, 0x10);
		dmaExec(g_SeqTable, seqromaddr, 0x10);

		len = g_SeqTable->count * sizeof(struct seqtableentry) + 4;
		g_SeqTable = alHeapDBAlloc(0, 0, &g_SndHeap, 1, len);
		dmaExec(g_SeqTable, seqromaddr, len + 0xf & 0xfffffff0);

		// Promote segment-relative offsets to ROM addresses
		for (i = 0; i < g_SeqTable->count; i++) {
			g_SeqTable->entries[i].romaddr += seqromaddr;
		}

		synconfig.maxVVoices = 44;
		synconfig.maxPVoices = 30;
		synconfig.maxUpdates = 64;
		synconfig.dmaproc = NULL;
		synconfig.outputRate = 0;
		synconfig.heap = &g_SndHeap;
		synconfig.maxFXbusses = g_SndMaxFxBusses;

		for (i = 0; i < g_SndMaxFxBusses; i++) {
			synconfig.fxTypes[i] = 6;
		}

		sndpconfig.maxEvents = 64;
		sndpconfig.maxStates = 64;
		sndpconfig.maxSounds = 20;
		sndpconfig.unk10 = 9;
		sndpconfig.heap = &g_SndHeap;

#if VERSION >= VERSION_PAL_FINAL
		settings[0] = 22020;
		settings[1] = 1;
		settings[2] = 2000;

		amgrCreate(&synconfig, settings);
#else
		amgrCreate(&synconfig);
#endif

		if (g_SndMp3Enabled) {
			osSyncPrintf("RWI : Initialising the new and improved MP3 player\n");

			mp3Init(&g_SndHeap);
			func00037f08(0x7fff, 1);
			func00037f5c(0, true);

			osSyncPrintf("RWI : MP3 player Initialising Done\n");
		}

		for (i = 0; i < 3; i++) {
			seqInit(&g_SeqInstances[i]);
		}

		osSyncPrintf("gsSndpNew\n");

		n_alSndpNew(&sndpconfig);

		osSyncPrintf("Set the sample callbacks\n");

		func00033378(snd0000f76c);
		func00033634(snd0000f7b8);

		amgrStartThread();

		sndSetSoundMode(g_SoundMode);
	}
}

bool sndIsMp3(s16 soundnum)
{
	union soundnumhack tmp;
	tmp.packed = soundnum;

	return tmp.mp3priority != 0;
}

bool snd0000fbc4(s16 arg0)
{
	if (!g_SndDisabled && g_SndMp3Enabled) {
		if (func00037ea4() && g_SndCurMp3.unk08 != 0) {
			return false;
		}

		func00037e1c();

		g_SndCurMp3.playing = false;
		g_SndCurMp3.responsetimer240 = -1;
	}

	return true;
}

void snd0000fc40(s32 arg0)
{
	// empty
}

#if VERSION >= VERSION_NTSC_1_0
bool seqPlay(struct seqinstance *seq, s32 tracknum)
{
	u32 stack;
	s32 binlen;
	u8 *binstart;
	s32 ziplen;
	u8 *zipstart;
	u8 scratch[1024 * 5];

	s32 state = n_alCSPGetState(seq->seqp);

	if (g_SndDisabled) {
		return false;
	}

	seq->tracknum = tracknum;

	if (g_SeqTable && tracknum);

	if (state != AL_STOPPED) {
		return false;
	}

	if (g_SeqTable->entries[seq->tracknum].romaddr < 0x10000) {
		return false;
	}

	binlen = ALIGN16(g_SeqTable->entries[seq->tracknum].binlen) + 0x40;

	if (binlen >= g_SeqBufferSize) {
		return false;
	}

	ziplen = ALIGN16(g_SeqTable->entries[seq->tracknum].ziplen);
	binstart = seq->data;
	zipstart = binstart + binlen - ziplen;

	dmaExec(zipstart, g_SeqTable->entries[seq->tracknum].romaddr, ziplen);

	rzipInflate(zipstart, binstart, scratch);

	n_alCSeqNew(&seq->seq, seq->data);
	n_alCSPSetSeq(seq->seqp, &seq->seq);
	seqSetVolume(seq, seqGetVolume(seq));
	n_alCSPPlay(seq->seqp);

	return true;
}
#else
const char var70055250nb[] = "DMA-Crash %s %d Ram: %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x";
const char var700552a8nb[] = "snd.c";

GLOBAL_ASM(
glabel seqPlay
/*    100e0:	27bdeaf8 */ 	addiu	$sp,$sp,-5384
/*    100e4:	afb00058 */ 	sw	$s0,0x58($sp)
/*    100e8:	00808025 */ 	or	$s0,$a0,$zero
/*    100ec:	afbf005c */ 	sw	$ra,0x5c($sp)
/*    100f0:	afa5150c */ 	sw	$a1,0x150c($sp)
/*    100f4:	0c00e838 */ 	jal	n_alCSPGetState
/*    100f8:	8c8400f8 */ 	lw	$a0,0xf8($a0)
/*    100fc:	3c0e8006 */ 	lui	$t6,0x8006
/*    10100:	8dcef6c0 */ 	lw	$t6,-0x940($t6)
/*    10104:	8fa4150c */ 	lw	$a0,0x150c($sp)
/*    10108:	3c03800a */ 	lui	$v1,0x800a
/*    1010c:	51c00004 */ 	beqzl	$t6,.NB00010120
/*    10110:	ae040104 */ 	sw	$a0,0x104($s0)
/*    10114:	10000066 */ 	beqz	$zero,.NB000102b0
/*    10118:	00001025 */ 	or	$v0,$zero,$zero
/*    1011c:	ae040104 */ 	sw	$a0,0x104($s0)
.NB00010120:
/*    10120:	10400003 */ 	beqz	$v0,.NB00010130
/*    10124:	8c638188 */ 	lw	$v1,-0x7e78($v1)
/*    10128:	10000061 */ 	beqz	$zero,.NB000102b0
/*    1012c:	00001025 */ 	or	$v0,$zero,$zero
.NB00010130:
/*    10130:	8e0f0104 */ 	lw	$t7,0x104($s0)
/*    10134:	3c010001 */ 	lui	$at,0x1
/*    10138:	000fc0c0 */ 	sll	$t8,$t7,0x3
/*    1013c:	00781021 */ 	addu	$v0,$v1,$t8
/*    10140:	8c450004 */ 	lw	$a1,0x4($v0)
/*    10144:	00a1082b */ 	sltu	$at,$a1,$at
/*    10148:	50200004 */ 	beqzl	$at,.NB0001015c
/*    1014c:	94470008 */ 	lhu	$a3,0x8($v0)
/*    10150:	10000057 */ 	beqz	$zero,.NB000102b0
/*    10154:	00001025 */ 	or	$v0,$zero,$zero
/*    10158:	94470008 */ 	lhu	$a3,0x8($v0)
.NB0001015c:
/*    1015c:	3c0a800a */ 	lui	$t2,0x800a
/*    10160:	8d4a818c */ 	lw	$t2,-0x7e74($t2)
/*    10164:	24e7000f */ 	addiu	$a3,$a3,0xf
/*    10168:	34f9000f */ 	ori	$t9,$a3,0xf
/*    1016c:	3b29000f */ 	xori	$t1,$t9,0xf
/*    10170:	25270040 */ 	addiu	$a3,$t1,0x40
/*    10174:	00ea082b */ 	sltu	$at,$a3,$t2
/*    10178:	54200004 */ 	bnezl	$at,.NB0001018c
/*    1017c:	9446000a */ 	lhu	$a2,0xa($v0)
/*    10180:	1000004b */ 	beqz	$zero,.NB000102b0
/*    10184:	00001025 */ 	or	$v0,$zero,$zero
/*    10188:	9446000a */ 	lhu	$a2,0xa($v0)
.NB0001018c:
/*    1018c:	8e0800fc */ 	lw	$t0,0xfc($s0)
/*    10190:	24c6000f */ 	addiu	$a2,$a2,0xf
/*    10194:	34cb000f */ 	ori	$t3,$a2,0xf
/*    10198:	3966000f */ 	xori	$a2,$t3,0xf
/*    1019c:	01076821 */ 	addu	$t5,$t0,$a3
/*    101a0:	01a62023 */ 	subu	$a0,$t5,$a2
/*    101a4:	afa414f8 */ 	sw	$a0,0x14f8($sp)
/*    101a8:	afa40064 */ 	sw	$a0,0x64($sp)
/*    101ac:	0c003664 */ 	jal	dmaExec
/*    101b0:	afa814fc */ 	sw	$t0,0x14fc($sp)
/*    101b4:	8fa40064 */ 	lw	$a0,0x64($sp)
/*    101b8:	8fa514fc */ 	lw	$a1,0x14fc($sp)
/*    101bc:	0c001da4 */ 	jal	rzipInflate
/*    101c0:	27a600f4 */ 	addiu	$a2,$sp,0xf4
/*    101c4:	1440002c */ 	bnez	$v0,.NB00010278
/*    101c8:	8fa314f8 */ 	lw	$v1,0x14f8($sp)
/*    101cc:	906e0000 */ 	lbu	$t6,0x0($v1)
/*    101d0:	3c057005 */ 	lui	$a1,0x7005
/*    101d4:	3c067005 */ 	lui	$a2,0x7005
/*    101d8:	afae0010 */ 	sw	$t6,0x10($sp)
/*    101dc:	906f0001 */ 	lbu	$t7,0x1($v1)
/*    101e0:	24c652a8 */ 	addiu	$a2,$a2,0x52a8
/*    101e4:	24a55250 */ 	addiu	$a1,$a1,0x5250
/*    101e8:	afaf0014 */ 	sw	$t7,0x14($sp)
/*    101ec:	90780002 */ 	lbu	$t8,0x2($v1)
/*    101f0:	27a40070 */ 	addiu	$a0,$sp,0x70
/*    101f4:	2407068c */ 	addiu	$a3,$zero,0x68c
/*    101f8:	afb80018 */ 	sw	$t8,0x18($sp)
/*    101fc:	90790003 */ 	lbu	$t9,0x3($v1)
/*    10200:	afb9001c */ 	sw	$t9,0x1c($sp)
/*    10204:	90690004 */ 	lbu	$t1,0x4($v1)
/*    10208:	afa90020 */ 	sw	$t1,0x20($sp)
/*    1020c:	906a0005 */ 	lbu	$t2,0x5($v1)
/*    10210:	afaa0024 */ 	sw	$t2,0x24($sp)
/*    10214:	906b0006 */ 	lbu	$t3,0x6($v1)
/*    10218:	afab0028 */ 	sw	$t3,0x28($sp)
/*    1021c:	906c0007 */ 	lbu	$t4,0x7($v1)
/*    10220:	afac002c */ 	sw	$t4,0x2c($sp)
/*    10224:	906d0008 */ 	lbu	$t5,0x8($v1)
/*    10228:	afad0030 */ 	sw	$t5,0x30($sp)
/*    1022c:	906e0009 */ 	lbu	$t6,0x9($v1)
/*    10230:	afae0034 */ 	sw	$t6,0x34($sp)
/*    10234:	906f000a */ 	lbu	$t7,0xa($v1)
/*    10238:	afaf0038 */ 	sw	$t7,0x38($sp)
/*    1023c:	9078000b */ 	lbu	$t8,0xb($v1)
/*    10240:	afb8003c */ 	sw	$t8,0x3c($sp)
/*    10244:	9079000c */ 	lbu	$t9,0xc($v1)
/*    10248:	afb90040 */ 	sw	$t9,0x40($sp)
/*    1024c:	9069000d */ 	lbu	$t1,0xd($v1)
/*    10250:	afa90044 */ 	sw	$t1,0x44($sp)
/*    10254:	906a000e */ 	lbu	$t2,0xe($v1)
/*    10258:	afaa0048 */ 	sw	$t2,0x48($sp)
/*    1025c:	906b000f */ 	lbu	$t3,0xf($v1)
/*    10260:	0c004fc1 */ 	jal	sprintf
/*    10264:	afab004c */ 	sw	$t3,0x4c($sp)
/*    10268:	0c003074 */ 	jal	crashSetMessage
/*    1026c:	27a40070 */ 	addiu	$a0,$sp,0x70
/*    10270:	240c0045 */ 	addiu	$t4,$zero,0x45
/*    10274:	a00c0000 */ 	sb	$t4,0x0($zero)
.NB00010278:
/*    10278:	02002025 */ 	or	$a0,$s0,$zero
/*    1027c:	0c00e840 */ 	jal	n_alCSeqNew
/*    10280:	8e0500fc */ 	lw	$a1,0xfc($s0)
/*    10284:	8e0400f8 */ 	lw	$a0,0xf8($s0)
/*    10288:	0c00ebec */ 	jal	n_alCSPSetSeq
/*    1028c:	02002825 */ 	or	$a1,$s0,$zero
/*    10290:	0c0040b1 */ 	jal	seqGetVolume
/*    10294:	02002025 */ 	or	$a0,$s0,$zero
/*    10298:	02002025 */ 	or	$a0,$s0,$zero
/*    1029c:	0c0040bb */ 	jal	seqSetVolume
/*    102a0:	3045ffff */ 	andi	$a1,$v0,0xffff
/*    102a4:	0c00ec00 */ 	jal	n_alCSPPlay
/*    102a8:	8e0400f8 */ 	lw	$a0,0xf8($s0)
/*    102ac:	24020001 */ 	addiu	$v0,$zero,0x1
.NB000102b0:
/*    102b0:	8fbf005c */ 	lw	$ra,0x5c($sp)
/*    102b4:	8fb00058 */ 	lw	$s0,0x58($sp)
/*    102b8:	27bd1508 */ 	addiu	$sp,$sp,0x1508
/*    102bc:	03e00008 */ 	jr	$ra
/*    102c0:	00000000 */ 	sll	$zero,$zero,0x0
);
#endif

u16 seqGetVolume(struct seqinstance *seq)
{
	return g_SndDisabled ? 0x7fff : seq->volume;
}

void seqSetVolume(struct seqinstance *seq, u16 volume)
{
	if (!g_SndDisabled) {
		u32 tmp = (var8005ecf8[seq->tracknum] * volume);
		tmp >>=	15;

		seq->volume = volume;

		if (tmp > 0x7fff) {
			tmp = 0x7fff;
		}

		n_alCSPSetVol(seq->seqp, tmp);
	}
}

void snd0000fe18(void)
{
	// empty
}

void snd0000fe20(void)
{
	if (g_SndMp3Enabled) {
		func00037e38();
	}
}

void snd0000fe50(void)
{
	if (g_SndMp3Enabled) {
		func00037e68();
	}
}

void snd0000fe80(void)
{
	// empty
}

void sndTick(void)
{
#if VERSION >= VERSION_NTSC_1_0
	struct sndstate *stateptrs[64];
	struct sndstate states[64];
	s32 i;
	s32 curtime;
	struct sndstate *state;
#endif
	OSPri prevpri;
	s32 s0;
	union soundnumhack sp50;
	s32 index;
	s32 stack;

#if VERSION >= VERSION_NTSC_1_0
	static s32 var8005edec = -1;

	sndIncrementAges();

	prevpri = osGetThreadPri(NULL);
	osSetThreadPri(0, osGetThreadPri(&g_AudioManager.thread) + 1);

	curtime = sndpGetCurTime();
	state = sndpGetHeadState();

	var8005ddd4 = 0;
	i = 0;

	while (state) {
		stateptrs[i] = state;
		states[i] = *state;

		var8005ddd4++;

		if ((state->flags & SNDSTATEFLAG_02) == 0
				&& stateptrs[i]->state == AL_PLAYING
				&& stateptrs[i]->unk48 > 0
				&& stateptrs[i]->unk48 < curtime
				) {
			state->unk48 = 0;
			func00033db0();
			break;
		}

		state = (struct sndstate *)state->node.next;
		i++;
	}

	osSetThreadPri(0, prevpri);

	if (var8005ddd8 < var8005ddd4) {
		var8005ddd8 = var8005ddd4;
	}

	if (var8005edec != var8005ddd8) {
		var8005edec = var8005ddd8;
	}
#endif

	if (!g_SndDisabled && g_SndMp3Enabled) {
		if (g_Vars.stagenum == STAGE_AIRFORCEONE) {
			sndTickNosedive();
		} else if (g_Vars.stagenum == STAGE_ESCAPE) {
			sndTickUfo();
		}

		if (g_Vars.tickmode == TICKMODE_CUTSCENE) {
			s0 = musicGetVolume() > g_SfxVolume ? musicGetVolume() : g_SfxVolume;

			if (s0 != snd0000e9dc()) {
				snd0000ea80(s0);
			}
		} else {
			if (g_SfxVolume != snd0000e9dc()) {
				snd0000ea80(g_SfxVolume);
			}
		}

		if (g_SndGuardStringPtr != NULL) {
			if (strcmp(g_SndGuardStringPtr, &g_SndGuardString) != 0) {
#if VERSION < VERSION_NTSC_1_0
				crashSetMessage("Snd Heap Check FAILED");
				CRASH();
#endif
			}
		}

		if (func00037ea4() == 0 && g_SndCurMp3.playing) {
			if (g_SndCurMp3.unk08) {
				mp3PlayFile(g_SndCurMp3.romaddr, g_SndCurMp3.romsize);
				return;
			}

			switch (g_SndCurMp3.responsetype) {
			case MP3RESPONSETYPE_ACKNOWLEDGE:
			case MP3RESPONSETYPE_WHISPER:
				g_SndCurMp3.responsetimer240 = PALDOWN(60);
				break;
			case MP3RESPONSETYPE_GREETING:
				g_SndCurMp3.responsetimer240 = 1;
				break;
			}

			g_SndCurMp3.playing = false;
			return;
		}

		if (g_SndCurMp3.responsetimer240 > 0) {
			g_SndCurMp3.responsetimer240 -= g_Vars.lvupdate240;

			if (g_SndCurMp3.responsetimer240 <= 0) {
				if (g_SndCurMp3.responsetype == MP3RESPONSETYPE_WHISPER) {
					do {
						index = random() % 4;
					} while (index == g_SndCurMp3.prevwhisper);

					g_SndCurMp3.prevwhisper = index;

					switch (index) {
					case 0: sp50.packed = MP3_JO_WHISPER_RECEIVED; break;
					case 1: sp50.packed = MP3_JO_WHISPER_UNDERSTOOD; break;
					case 2: sp50.packed = MP3_JO_WHISPER_AFFIRMATIVE; break;
					case 3: sp50.packed = MP3_JO_WHISPER_CONFIRMED; break;
					}
				} else if (g_SndCurMp3.responsetype == MP3RESPONSETYPE_ACKNOWLEDGE) {
					do {
						index = random() % 4;
					} while (index == g_SndCurMp3.prevacknowledge);

					g_SndCurMp3.prevacknowledge = index;

					switch (index) {
					case 0: sp50.packed = MP3_JO_ACKNOWLEDGE_CONFIRMED; break;
					case 1: sp50.packed = MP3_JO_ACKNOWLEDGE_UNDERSTOOD; break;
					case 2: sp50.packed = MP3_JO_ACKNOWLEDGE_AFFIRMATIVE; break;
					case 3: sp50.packed = MP3_JO_ACKNOWLEDGE_RECEIVED; break;
					}
				} else if (g_SndCurMp3.responsetype == MP3RESPONSETYPE_GREETING) {
					do {
						index = random() % 4;
					} while (index == g_SndCurMp3.prevgreeting);

					g_SndCurMp3.prevgreeting = index;

					switch (index) {
					case 0: sp50.packed = MP3_JO_GREETING_HELLO; break;
					case 1: sp50.packed = MP3_JO_GREETING_HI; break;
					case 2: sp50.packed = MP3_JO_GREETING_HEY; break;
					case 3: sp50.packed = MP3_JO_GREETING_HIYA; break;
					}
				}

				g_SndCurMp3.responsetimer240 = -1;
				sndStart(0, sp50.packed, 0, -1, -1, -1.0f, -1, -1);
			}
		}
	}
}

const char var70053be0[] = "Snd_Play_Universal : Overriding -> Link = %d\n";

#if VERSION < VERSION_NTSC_1_0
const char var700552f0nb[] = "Snd_Play_Mpeg : sndId=%d, vol=%d, pan=%d\n";
#endif

const char var70053c10[] = "Snd_Play_Mpeg : SYSTEM IS DISABLED\n";
const char var70053c34[] = "Snd_Play_Mpeg  : Lib called -> Adr=%x\n";
const char var70053c5c[] = "Snd_Play_Mpeg  : Chunk size -> Adr=%x\n";

GLOBAL_ASM(
glabel func0001034c
/*    1034c:	309907ff */ 	andi	$t9,$a0,0x7ff
/*    10350:	00194400 */ 	sll	$t0,$t9,0x10
/*    10354:	afa40000 */ 	sw	$a0,0x0($sp)
/*    10358:	03e00008 */ 	jr	$ra
/*    1035c:	00081403 */ 	sra	$v0,$t0,0x10
);

bool sndIsDisabled(void)
{
	return g_SndDisabled;
}

void sndStartMp3ByFilenum(u32 filenum)
{
	union soundnumhack sfxref;

	if (!g_SndDisabled && g_SndMp3Enabled) {
		sfxref.packed = 0;
		sfxref.unk02 = 0;
		sfxref.mp3priority = 1; // high priority
		sfxref.id = filenum;

		sndStart(0, sfxref.packed, NULL, -1, -1, -1, -1, -1);
	}
}

/**
 * Return true if the player has the language filter enabled
 * and the given audio ID is one that should be filtered out.
 */
bool sndIsFiltered(s32 audio_id)
{
	if (g_Vars.langfilteron) {
		union soundnumhack sfxref;
		sfxref.packed = audio_id;

		if (sfxref.hasconfig) {
			if (g_AudioConfigs[g_AudioRussMappings[sfxref.confignum].audioconfig_index].flags & AUDIOCONFIGFLAG_OFFENSIVE) {
				return true;
			}
		}
#if VERSION >= VERSION_NTSC_1_0
		else {
			// @bug: The masking here makes it impossible to match hangar guy's
			// audio ID, so his phrase can be said even with the lang filter on.
			// The switch value resolves to sfxref & 0x7ff, while hangar guy's
			// reference is 0x17ad.
			switch (sfxref.id) {
			case SFX_ELVIS_KISS_MY_ALIEN_BUTT:
			case SFX_ELVIS_ILL_KICK_YOUR_ASS:
			case MP3_HANGERGUY_DONT_SCREW_UP:
				return true;
			}
		}
#endif
	}

	return false;
}

void sndAdjust(struct sndstate **handle, s32 arg1, s32 arg2, s32 pan, s32 soundnum, f32 arg5, s32 arg6, s32 arg7, s32 arg8)
{
	s32 sp24 = -1;
	union soundnumhack sp20;
	union soundnumhack sp1c;
	struct audioconfig *config;

	if (arg8 || arg7 != -1) {
		if (arg7 != -1) {
			sp24 = arg7;
		} else {
			sp24 = 0;
		}

#if VERSION >= VERSION_NTSC_1_0
		if (pan != -1 && g_SoundMode == SOUNDMODE_SURROUND && (pan & 0x80)) {
			sp24 += 128;
		}
#else
		if (g_SoundMode == SOUNDMODE_SURROUND && (pan & 0x80)) {
			sp24 += 128;
		}
#endif
	}

	if (soundnum > 0) {
		sp20.packed = soundnum;

		if (sp20.hasconfig) {
			s32 index = g_AudioRussMappings[sp20.confignum].audioconfig_index;

			sp1c.packed = g_AudioRussMappings[sp20.confignum].soundnum;
			sp1c.hasconfig = false;

			soundnum = sp1c.packed;

			config = &g_AudioConfigs[index];

			if (config->unk10 != 100) {
				arg2 = arg2 * config->unk10 / 100;
			}

			if (config->unk14 != -1) {
				pan = config->unk14;
			}

			if (g_Vars.langfilteron && (config->flags & AUDIOCONFIGFLAG_OFFENSIVE)) {
				arg2 = 0;
			}
		}
	}

	if (arg1 != 0) {
		if (arg2 != -1) {
			arg2 = arg2 * snd0000e9dc() / 32767;
			func00037f08(arg2, true);
		}

		if (pan != -1) {
			func00037f5c(pan, true);
		}
	}

	if (*handle != NULL) {
		if (arg2 != -1) {
			audioPostEvent(*handle, 8, arg2);
		}

		if (pan != -1) {
			audioPostEvent(*handle, 4, pan & 0x7f);
		}

		if (arg5 != -1.0f) {
			audioPostEvent(*handle, 0x10, *(s32 *)&arg5);
		}

		if (sp24 != -1) {
			audioPostEvent(*handle, 0x100, sp24);
		}
	}
}

struct sndstate *snd00010718(struct sndstate **handle, s32 arg1, s32 arg2, s32 arg3, s32 arg4, f32 arg5, s32 arg6, s32 arg7, s32 arg8)
{
	OSPri prevpri = osGetThreadPri(NULL);
	s32 t0 = -1;
	struct sndstate *state;
	union soundnumhack sp30;
	union soundnumhack sp2c;
	struct audioconfig *config;

	osSetThreadPri(0, osGetThreadPri(&g_AudioManager.thread) + 1);

	if (arg8 || arg7 != -1) {
		if (arg7 != -1) {
			t0 = arg7;
		} else {
			t0 = 0;
		}

#if VERSION >= VERSION_NTSC_1_0
		if (arg3 != -1 && g_SoundMode == SOUNDMODE_SURROUND && (arg3 & 0x80)) {
			t0 += 128;
		}
#else
		if (g_SoundMode == SOUNDMODE_SURROUND && (arg3 & 0x80)) {
			t0 += 128;
		}
#endif
	}

	if (arg4) {
		sp30.packed = arg4;

		if (sp30.hasconfig) {
			s32 index = g_AudioRussMappings[sp30.confignum].audioconfig_index;

			sp2c.packed = g_AudioRussMappings[sp30.confignum].soundnum;
			sp2c.hasconfig = false;

			arg4 = sp2c.packed;

			config = &g_AudioConfigs[index];

			if (config->unk10 != 100) {
				arg2 = arg2 * config->unk10 / 100;
			}

			if (config->unk14 != -1) {
				arg3 = config->unk14;
			}

			if (g_Vars.langfilteron && (config->flags & AUDIOCONFIGFLAG_OFFENSIVE)) {
				arg2 = 0;
			}
		}
	}

	state = sndStart(var80095200, arg4, handle, arg2, arg3, arg5, arg6, t0);

	osSetThreadPri(0, prevpri);

	return state;
}

#if VERSION >= VERSION_NTSC_1_0
GLOBAL_ASM(
glabel sndStart
/*    10904:	27bdffb8 */ 	addiu	$sp,$sp,-72
/*    10908:	8fae0064 */ 	lw	$t6,0x64($sp)
/*    1090c:	2402ffff */ 	addiu	$v0,$zero,-1
/*    10910:	afbf0024 */ 	sw	$ra,0x24($sp)
/*    10914:	afa40048 */ 	sw	$a0,0x48($sp)
/*    10918:	afa5004c */ 	sw	$a1,0x4c($sp)
/*    1091c:	11c20003 */ 	beq	$t6,$v0,.L0001092c
/*    10920:	afa60050 */ 	sw	$a2,0x50($sp)
/*    10924:	10000002 */ 	b	.L00010930
/*    10928:	93a90067 */ 	lbu	$t1,0x67($sp)
.L0001092c:
/*    1092c:	00004825 */ 	or	$t1,$zero,$zero
.L00010930:
/*    10930:	8fa30060 */ 	lw	$v1,0x60($sp)
/*    10934:	44802000 */ 	mtc1	$zero,$f4
/*    10938:	24050001 */ 	addiu	$a1,$zero,0x1
/*    1093c:	10620003 */ 	beq	$v1,$v0,.L0001094c
/*    10940:	3c198006 */ 	lui	$t9,%hi(g_SndDisabled)
/*    10944:	10000001 */ 	b	.L0001094c
/*    10948:	306500ff */ 	andi	$a1,$v1,0xff
.L0001094c:
/*    1094c:	8fa30058 */ 	lw	$v1,0x58($sp)
/*    10950:	24080040 */ 	addiu	$t0,$zero,0x40
/*    10954:	240f7fff */ 	addiu	$t7,$zero,0x7fff
/*    10958:	10620003 */ 	beq	$v1,$v0,.L00010968
/*    1095c:	3c013f80 */ 	lui	$at,0x3f80
/*    10960:	10000001 */ 	b	.L00010968
/*    10964:	306800ff */ 	andi	$t0,$v1,0xff
.L00010968:
/*    10968:	50e20004 */ 	beql	$a3,$v0,.L0001097c
/*    1096c:	a7af003a */ 	sh	$t7,0x3a($sp)
/*    10970:	10000002 */ 	b	.L0001097c
/*    10974:	a7a7003a */ 	sh	$a3,0x3a($sp)
/*    10978:	a7af003a */ 	sh	$t7,0x3a($sp)
.L0001097c:
/*    1097c:	c7a2005c */ 	lwc1	$f2,0x5c($sp)
/*    10980:	4602203c */ 	c.lt.s	$f4,$f2
/*    10984:	00000000 */ 	nop
/*    10988:	45020004 */ 	bc1fl	.L0001099c
/*    1098c:	44810000 */ 	mtc1	$at,$f0
/*    10990:	10000003 */ 	b	.L000109a0
/*    10994:	46001006 */ 	mov.s	$f0,$f2
/*    10998:	44810000 */ 	mtc1	$at,$f0
.L0001099c:
/*    1099c:	00000000 */ 	nop
.L000109a0:
/*    109a0:	8f39dda0 */ 	lw	$t9,%lo(g_SndDisabled)($t9)
/*    109a4:	87b8004e */ 	lh	$t8,0x4e($sp)
/*    109a8:	24010037 */ 	addiu	$at,$zero,0x37
/*    109ac:	13200003 */ 	beqz	$t9,.L000109bc
/*    109b0:	a7b80044 */ 	sh	$t8,0x44($sp)
/*    109b4:	10000047 */ 	b	.L00010ad4
/*    109b8:	00001025 */ 	or	$v0,$zero,$zero
.L000109bc:
/*    109bc:	8fab0044 */ 	lw	$t3,0x44($sp)
/*    109c0:	97ad0044 */ 	lhu	$t5,0x44($sp)
/*    109c4:	87a40044 */ 	lh	$a0,0x44($sp)
/*    109c8:	000b67c2 */ 	srl	$t4,$t3,0x1f
/*    109cc:	11800006 */ 	beqz	$t4,.L000109e8
/*    109d0:	31ae7fff */ 	andi	$t6,$t5,0x7fff
/*    109d4:	000e7880 */ 	sll	$t7,$t6,0x2
/*    109d8:	3c048006 */ 	lui	$a0,%hi(g_AudioRussMappings)
/*    109dc:	008f2021 */ 	addu	$a0,$a0,$t7
/*    109e0:	10000001 */ 	b	.L000109e8
/*    109e4:	8484dde4 */ 	lh	$a0,%lo(g_AudioRussMappings)($a0)
.L000109e8:
/*    109e8:	308307ff */ 	andi	$v1,$a0,0x7ff
/*    109ec:	10610003 */ 	beq	$v1,$at,.L000109fc
/*    109f0:	24010009 */ 	addiu	$at,$zero,0x9
/*    109f4:	54610004 */ 	bnel	$v1,$at,.L00010a08
/*    109f8:	afa3002c */ 	sw	$v1,0x2c($sp)
.L000109fc:
/*    109fc:	10000035 */ 	b	.L00010ad4
/*    10a00:	00001025 */ 	or	$v0,$zero,$zero
/*    10a04:	afa3002c */ 	sw	$v1,0x2c($sp)
.L00010a08:
/*    10a08:	a7a40040 */ 	sh	$a0,0x40($sp)
/*    10a0c:	a3a5003e */ 	sb	$a1,0x3e($sp)
/*    10a10:	a3a8003d */ 	sb	$t0,0x3d($sp)
/*    10a14:	a3a9003f */ 	sb	$t1,0x3f($sp)
/*    10a18:	0c003ee8 */ 	jal	sndIsMp3
/*    10a1c:	e7a00034 */ 	swc1	$f0,0x34($sp)
/*    10a20:	8fa3002c */ 	lw	$v1,0x2c($sp)
/*    10a24:	87a40040 */ 	lh	$a0,0x40($sp)
/*    10a28:	93a5003e */ 	lbu	$a1,0x3e($sp)
/*    10a2c:	93a8003d */ 	lbu	$t0,0x3d($sp)
/*    10a30:	93a9003f */ 	lbu	$t1,0x3f($sp)
/*    10a34:	8faa0050 */ 	lw	$t2,0x50($sp)
/*    10a38:	1040000b */ 	beqz	$v0,.L00010a68
/*    10a3c:	c7a00034 */ 	lwc1	$f0,0x34($sp)
/*    10a40:	97a5003a */ 	lhu	$a1,0x3a($sp)
/*    10a44:	01003025 */ 	or	$a2,$t0,$zero
/*    10a48:	0c0042b9 */ 	jal	sndStartMp3
/*    10a4c:	00003825 */ 	or	$a3,$zero,$zero
/*    10a50:	8faa0050 */ 	lw	$t2,0x50($sp)
/*    10a54:	11400002 */ 	beqz	$t2,.L00010a60
/*    10a58:	00000000 */ 	nop
/*    10a5c:	ad400000 */ 	sw	$zero,0x0($t2)
.L00010a60:
/*    10a60:	1000001c */ 	b	.L00010ad4
/*    10a64:	00001025 */ 	or	$v0,$zero,$zero
.L00010a68:
/*    10a68:	3c188009 */ 	lui	$t8,%hi(g_NumSounds)
/*    10a6c:	8f184ea0 */ 	lw	$t8,%lo(g_NumSounds)($t8)
/*    10a70:	3c198009 */ 	lui	$t9,%hi(g_Is4Mb)
/*    10a74:	8fa40048 */ 	lw	$a0,0x48($sp)
/*    10a78:	0078082b */ 	sltu	$at,$v1,$t8
/*    10a7c:	10200015 */ 	beqz	$at,.L00010ad4
/*    10a80:	00001025 */ 	or	$v0,$zero,$zero
/*    10a84:	93390af0 */ 	lbu	$t9,%lo(g_Is4Mb)($t9)
/*    10a88:	24010001 */ 	addiu	$at,$zero,0x1
/*    10a8c:	97a6003a */ 	lhu	$a2,0x3a($sp)
/*    10a90:	17210003 */ 	bne	$t9,$at,.L00010aa0
/*    10a94:	01003825 */ 	or	$a3,$t0,$zero
/*    10a98:	10000002 */ 	b	.L00010aa4
/*    10a9c:	00001025 */ 	or	$v0,$zero,$zero
.L00010aa0:
/*    10aa0:	00a01025 */ 	or	$v0,$a1,$zero
.L00010aa4:
/*    10aa4:	00032c00 */ 	sll	$a1,$v1,0x10
/*    10aa8:	00055c03 */ 	sra	$t3,$a1,0x10
/*    10aac:	30ec007f */ 	andi	$t4,$a3,0x7f
/*    10ab0:	01803825 */ 	or	$a3,$t4,$zero
/*    10ab4:	01602825 */ 	or	$a1,$t3,$zero
/*    10ab8:	e7a00010 */ 	swc1	$f0,0x10($sp)
/*    10abc:	afa90014 */ 	sw	$t1,0x14($sp)
/*    10ac0:	afa20018 */ 	sw	$v0,0x18($sp)
/*    10ac4:	0c00ce08 */ 	jal	func00033820
/*    10ac8:	afaa001c */ 	sw	$t2,0x1c($sp)
/*    10acc:	10000002 */ 	b	.L00010ad8
/*    10ad0:	8fbf0024 */ 	lw	$ra,0x24($sp)
.L00010ad4:
/*    10ad4:	8fbf0024 */ 	lw	$ra,0x24($sp)
.L00010ad8:
/*    10ad8:	27bd0048 */ 	addiu	$sp,$sp,0x48
/*    10adc:	03e00008 */ 	jr	$ra
/*    10ae0:	00000000 */ 	nop
);

// Mismatch: Likely related to soundnumhack
//struct sndstate *sndStart(s32 arg0, s16 sound, struct sndstate **handle, s32 arg3, s32 arg4, f32 arg5, s32 arg6, s32 arg7)
//{
//	union soundnumhack sp44;
//	union soundnumhack sp40;
//	u8 sp3f;
//	u8 sp3e;
//	u8 sp3d;
//	u8 sp3c;
//	s16 sp3a;
//	u16 sp38;
//	f32 sp34;
//
//	// 91c
//	sp3f = arg7 != -1 ? arg7 : 0;
//
//	// 93c
//	sp3e = arg6 != -1 ? arg6 : 1;
//
//	// 958
//	sp3d = arg4 != -1 ? arg4 : 64;
//
//	// 968
//	sp3a = arg3 != -1 ? arg3 : 0x7fff;
//
//	// 988
//	sp34 = arg5 > 0.0f ? arg5 : 1.0f;
//
//	sp44.packed = sound & 0xffff;
//
//	// 9ac
//	if (g_SndDisabled) {
//		return NULL;
//	}
//
//	// 9cc
//	sp40.packed = sp44.hasconfig ? g_AudioRussMappings[sp44.confignum].soundnum : sp44.packed;
//
//	// 9ec
//	if (sp40.id == 0x37 || sp40.id == 9) {
//		return NULL;
//	}
//
//	// a18
//	if (sndIsMp3(sp40.packed)) {
//		sndStartMp3(sp40.packed, sp3a, sp3d, 0);
//
//		if (handle != NULL) {
//			*handle = NULL;
//		}
//
//		return NULL;
//	}
//
//	// a68
//	if (sp40.id < g_NumSounds) {
//		return func00033820(arg0, sp40.id, sp3a, sp3d & 0x7f,
//				sp34, sp3f, IS4MB() ? 0 : sp3e, handle);
//	}
//
//	return NULL;
//}
#else
GLOBAL_ASM(
glabel sndStart
/*    10ce0:	27bdffb8 */ 	addiu	$sp,$sp,-72
/*    10ce4:	8fae0064 */ 	lw	$t6,0x64($sp)
/*    10ce8:	2402ffff */ 	addiu	$v0,$zero,-1
/*    10cec:	afbf0024 */ 	sw	$ra,0x24($sp)
/*    10cf0:	afa40048 */ 	sw	$a0,0x48($sp)
/*    10cf4:	afa5004c */ 	sw	$a1,0x4c($sp)
/*    10cf8:	11c20003 */ 	beq	$t6,$v0,.NB00010d08
/*    10cfc:	afa60050 */ 	sw	$a2,0x50($sp)
/*    10d00:	10000002 */ 	beqz	$zero,.NB00010d0c
/*    10d04:	93a90067 */ 	lbu	$t1,0x67($sp)
.NB00010d08:
/*    10d08:	00004825 */ 	or	$t1,$zero,$zero
.NB00010d0c:
/*    10d0c:	8fa30060 */ 	lw	$v1,0x60($sp)
/*    10d10:	44802000 */ 	mtc1	$zero,$f4
/*    10d14:	24050001 */ 	addiu	$a1,$zero,0x1
/*    10d18:	10620003 */ 	beq	$v1,$v0,.NB00010d28
/*    10d1c:	3c198006 */ 	lui	$t9,0x8006
/*    10d20:	10000001 */ 	beqz	$zero,.NB00010d28
/*    10d24:	306500ff */ 	andi	$a1,$v1,0xff
.NB00010d28:
/*    10d28:	8fa30058 */ 	lw	$v1,0x58($sp)
/*    10d2c:	24080040 */ 	addiu	$t0,$zero,0x40
/*    10d30:	240f7fff */ 	addiu	$t7,$zero,0x7fff
/*    10d34:	10620003 */ 	beq	$v1,$v0,.NB00010d44
/*    10d38:	3c013f80 */ 	lui	$at,0x3f80
/*    10d3c:	10000001 */ 	beqz	$zero,.NB00010d44
/*    10d40:	306800ff */ 	andi	$t0,$v1,0xff
.NB00010d44:
/*    10d44:	50e20004 */ 	beql	$a3,$v0,.NB00010d58
/*    10d48:	a7af003a */ 	sh	$t7,0x3a($sp)
/*    10d4c:	10000002 */ 	beqz	$zero,.NB00010d58
/*    10d50:	a7a7003a */ 	sh	$a3,0x3a($sp)
/*    10d54:	a7af003a */ 	sh	$t7,0x3a($sp)
.NB00010d58:
/*    10d58:	c7a2005c */ 	lwc1	$f2,0x5c($sp)
/*    10d5c:	4602203c */ 	c.lt.s	$f4,$f2
/*    10d60:	00000000 */ 	sll	$zero,$zero,0x0
/*    10d64:	45020004 */ 	bc1fl	.NB00010d78
/*    10d68:	44810000 */ 	mtc1	$at,$f0
/*    10d6c:	10000003 */ 	beqz	$zero,.NB00010d7c
/*    10d70:	46001006 */ 	mov.s	$f0,$f2
/*    10d74:	44810000 */ 	mtc1	$at,$f0
.NB00010d78:
/*    10d78:	00000000 */ 	sll	$zero,$zero,0x0
.NB00010d7c:
/*    10d7c:	8f39f6c0 */ 	lw	$t9,-0x940($t9)
/*    10d80:	87b8004e */ 	lh	$t8,0x4e($sp)
/*    10d84:	24010037 */ 	addiu	$at,$zero,0x37
/*    10d88:	13200003 */ 	beqz	$t9,.NB00010d98
/*    10d8c:	a7b80044 */ 	sh	$t8,0x44($sp)
/*    10d90:	10000040 */ 	beqz	$zero,.NB00010e94
/*    10d94:	00001025 */ 	or	$v0,$zero,$zero
.NB00010d98:
/*    10d98:	8fab0044 */ 	lw	$t3,0x44($sp)
/*    10d9c:	97ad0044 */ 	lhu	$t5,0x44($sp)
/*    10da0:	87a40044 */ 	lh	$a0,0x44($sp)
/*    10da4:	000b67c2 */ 	srl	$t4,$t3,0x1f
/*    10da8:	11800006 */ 	beqz	$t4,.NB00010dc4
/*    10dac:	31ae7fff */ 	andi	$t6,$t5,0x7fff
/*    10db0:	000e7880 */ 	sll	$t7,$t6,0x2
/*    10db4:	3c048006 */ 	lui	$a0,0x8006
/*    10db8:	008f2021 */ 	addu	$a0,$a0,$t7
/*    10dbc:	10000001 */ 	beqz	$zero,.NB00010dc4
/*    10dc0:	8484f6f8 */ 	lh	$a0,-0x908($a0)
.NB00010dc4:
/*    10dc4:	308307ff */ 	andi	$v1,$a0,0x7ff
/*    10dc8:	10610003 */ 	beq	$v1,$at,.NB00010dd8
/*    10dcc:	24010009 */ 	addiu	$at,$zero,0x9
/*    10dd0:	54610004 */ 	bnel	$v1,$at,.NB00010de4
/*    10dd4:	afa3002c */ 	sw	$v1,0x2c($sp)
.NB00010dd8:
/*    10dd8:	1000002e */ 	beqz	$zero,.NB00010e94
/*    10ddc:	00001025 */ 	or	$v0,$zero,$zero
/*    10de0:	afa3002c */ 	sw	$v1,0x2c($sp)
.NB00010de4:
/*    10de4:	a7a40040 */ 	sh	$a0,0x40($sp)
/*    10de8:	a3a5003e */ 	sb	$a1,0x3e($sp)
/*    10dec:	a3a8003d */ 	sb	$t0,0x3d($sp)
/*    10df0:	a3a9003f */ 	sb	$t1,0x3f($sp)
/*    10df4:	0c00400e */ 	jal	sndIsMp3
/*    10df8:	e7a00034 */ 	swc1	$f0,0x34($sp)
/*    10dfc:	8fa3002c */ 	lw	$v1,0x2c($sp)
/*    10e00:	87a40040 */ 	lh	$a0,0x40($sp)
/*    10e04:	93a5003e */ 	lbu	$a1,0x3e($sp)
/*    10e08:	93a8003d */ 	lbu	$t0,0x3d($sp)
/*    10e0c:	93a9003f */ 	lbu	$t1,0x3f($sp)
/*    10e10:	8faa0050 */ 	lw	$t2,0x50($sp)
/*    10e14:	1040000b */ 	beqz	$v0,.NB00010e44
/*    10e18:	c7a00034 */ 	lwc1	$f0,0x34($sp)
/*    10e1c:	97a5003a */ 	lhu	$a1,0x3a($sp)
/*    10e20:	01003025 */ 	or	$a2,$t0,$zero
/*    10e24:	0c0043a9 */ 	jal	sndStartMp3
/*    10e28:	00003825 */ 	or	$a3,$zero,$zero
/*    10e2c:	8faa0050 */ 	lw	$t2,0x50($sp)
/*    10e30:	11400002 */ 	beqz	$t2,.NB00010e3c
/*    10e34:	00000000 */ 	sll	$zero,$zero,0x0
/*    10e38:	ad400000 */ 	sw	$zero,0x0($t2)
.NB00010e3c:
/*    10e3c:	10000015 */ 	beqz	$zero,.NB00010e94
/*    10e40:	00001025 */ 	or	$v0,$zero,$zero
.NB00010e44:
/*    10e44:	3c188009 */ 	lui	$t8,0x8009
/*    10e48:	931830e0 */ 	lbu	$t8,0x30e0($t8)
/*    10e4c:	24010001 */ 	addiu	$at,$zero,0x1
/*    10e50:	8fa40048 */ 	lw	$a0,0x48($sp)
/*    10e54:	17010003 */ 	bne	$t8,$at,.NB00010e64
/*    10e58:	97a6003a */ 	lhu	$a2,0x3a($sp)
/*    10e5c:	10000002 */ 	beqz	$zero,.NB00010e68
/*    10e60:	00001025 */ 	or	$v0,$zero,$zero
.NB00010e64:
/*    10e64:	00a01025 */ 	or	$v0,$a1,$zero
.NB00010e68:
/*    10e68:	00032c00 */ 	sll	$a1,$v1,0x10
/*    10e6c:	01003825 */ 	or	$a3,$t0,$zero
/*    10e70:	30eb007f */ 	andi	$t3,$a3,0x7f
/*    10e74:	0005cc03 */ 	sra	$t9,$a1,0x10
/*    10e78:	03202825 */ 	or	$a1,$t9,$zero
/*    10e7c:	01603825 */ 	or	$a3,$t3,$zero
/*    10e80:	e7a00010 */ 	swc1	$f0,0x10($sp)
/*    10e84:	afa90014 */ 	sw	$t1,0x14($sp)
/*    10e88:	afa20018 */ 	sw	$v0,0x18($sp)
/*    10e8c:	0c00d36c */ 	jal	func00033820
/*    10e90:	afaa001c */ 	sw	$t2,0x1c($sp)
.NB00010e94:
/*    10e94:	8fbf0024 */ 	lw	$ra,0x24($sp)
/*    10e98:	27bd0048 */ 	addiu	$sp,$sp,0x48
/*    10e9c:	03e00008 */ 	jr	$ra
/*    10ea0:	00000000 */ 	sll	$zero,$zero,0x0
);
#endif

void sndStartMp3(s16 soundnum, s32 arg1, s32 arg2, s32 arg3)
{
	union soundnumhack sp24;
	union soundnumhack sp20;

	sp24.packed = soundnum;

	if (!g_SndDisabled) {
		if (sp24.hasconfig) {
			sp20.packed = g_AudioRussMappings[sp24.confignum].soundnum;
			sp20.hasconfig = false;
		} else {
			sp20.packed = soundnum;
		}

		if (!g_SndCurMp3.playing
				|| ((sp20.mp3priority != 1 || g_SndCurMp3.sfxref.mp3priority != 1)
				 && sp20.mp3priority <= g_SndCurMp3.sfxref.mp3priority)) {
			if (sp24.hasconfig) {
				if (g_AudioConfigs[sp24.confignum].unk10 != -1) {
					arg1 = g_AudioConfigs[sp24.confignum].unk10 * 32767 / 100;
				}

				if (g_AudioConfigs[sp24.confignum].unk14 != -1) {
					arg2 = g_AudioConfigs[sp24.confignum].unk14;
				}

				// This is the same thing again
				if (g_AudioConfigs[sp24.confignum].unk14 != -1) {
					arg2 = g_AudioConfigs[sp24.confignum].unk14;
				}

				if (g_Vars.langfilteron && (g_AudioConfigs[sp24.confignum].flags & AUDIOCONFIGFLAG_OFFENSIVE)) {
					arg1 = 0;
				}
			}

			arg1 = arg1 * snd0000e9dc() / 0x7fff;

			g_SndCurMp3.romaddr = fileGetRomAddress(sp20.id);
			g_SndCurMp3.romsize = fileGetRomSizeByFileNum(sp20.id);

			func00037f08(arg1, true);
			func00037f5c(arg2, true);

			mp3PlayFile(g_SndCurMp3.romaddr, g_SndCurMp3.romsize);

			func00037f08(arg1, true);
			func00037f5c(arg2, true);

			g_SndCurMp3.sfxref.packed = sp20.packed;
			g_SndCurMp3.playing = true;
			g_SndCurMp3.responsetimer240 = -1;
			g_SndCurMp3.responsetype = MP3RESPONSETYPE_NONE;

			if (g_SndCurMp3.sfxref.unk02 == 2) {
				g_SndCurMp3.responsetype = MP3RESPONSETYPE_ACKNOWLEDGE;
			} else if (g_SndCurMp3.sfxref.unk02 == 1) {
				g_SndCurMp3.responsetype = MP3RESPONSETYPE_WHISPER;
			}

			if ((sp24.hasconfig && (g_AudioConfigs[sp24.confignum].flags & AUDIOCONFIGFLAG_04)) || (arg3 & 1)) {
				g_SndCurMp3.responsetype = MP3RESPONSETYPE_GREETING;
			}
		}
	}
}

void sndPlayNosedive(s32 seconds)
{
	g_SndNosediveDuration240 = seconds * PALDOWN(240);
	g_SndNosediveAge240 = 0;
	g_SndNosediveVolume = 0;
	g_SndNosediveHandle = NULL;
}

void sndStopNosedive(void)
{
	if (g_SndNosediveAge240 != -1) {
		g_SndNosediveAge240 = g_SndNosediveDuration240 + 1;
	}
}

void sndTickNosedive(void)
{
	f32 percentage;

	if (g_SndNosediveAge240 != -1) {
		g_SndNosediveAge240 += g_Vars.lvupdate240;

		percentage = (f32)g_SndNosediveAge240 / (f32)g_SndNosediveDuration240;

		if (percentage < 1.0f) { // less than 100% complete
			percentage += 0.44f;

			if (lvIsPaused()) {
				// Fade out volume during pause instead of stopping abruptly
				if (g_SndNosediveVolume > 0) {
					g_SndNosediveVolume -= g_Vars.diffframe240 * PALUP(80);

					if (g_SndNosediveVolume < 0) {
						g_SndNosediveVolume = 0;
					}
				}

				if (g_SndNosediveVolume) {
					sndAdjust(&g_SndNosediveHandle, 0, g_SndNosediveVolume, 64, -1, percentage, 0, -1, 1);
				} else if (g_SndNosediveHandle != NULL) {
					audioStop(g_SndNosediveHandle);
					g_SndNosediveHandle = NULL;
				}
			} else {
				// Not paused
				if (g_SndNosediveHandle == NULL) {
					sndStart(var80095200, SFX_NOSEDIVE, &g_SndNosediveHandle, -1, -1, -1, -1, -1);
				}

				// Fade in over about 2 seconds
				if (g_SndNosediveVolume < 20000) {
					g_SndNosediveVolume += g_Vars.diffframe240 * PALUP(40);

					if (g_SndNosediveVolume > 20000) {
						g_SndNosediveVolume = 20000;
					}
				}

				sndAdjust(&g_SndNosediveHandle, 0, g_SndNosediveVolume, 64, -1, percentage, 0, -1, 1);
			}
		} else {
			// Reached the configured fade out point
			if (g_SndNosediveVolume > 0) {
				g_SndNosediveVolume -= g_Vars.diffframe240 * PALUP(80);

				if (g_SndNosediveVolume < 0) {
					g_SndNosediveVolume = 0;
				}
			}

			if (g_SndNosediveVolume) {
				sndAdjust(&g_SndNosediveHandle, 0, g_SndNosediveVolume, 64, -1, -1, 0, -1, 1);
			} else if (g_SndNosediveHandle != NULL) {
				audioStop(g_SndNosediveHandle);
				g_SndNosediveHandle = NULL;
			}
		}
	}
}

void sndPlayUfo(s32 seconds)
{
	g_SndUfoDuration240 = seconds * PALDOWN(240);
	g_SndUfoAge240 = 0;
	g_SndUfoVolume = 0;
	g_SndUfoHandle = NULL;
}

void sndStopUfo(void)
{
	if (g_SndUfoAge240 != -1) {
		g_SndUfoAge240 = g_SndUfoDuration240 + 1;
	}
}

void sndTickUfo(void)
{
	f32 percentage;

	if (g_SndUfoAge240 != -1) {
		g_SndUfoAge240 += g_Vars.lvupdate240;

		percentage = (f32)g_SndUfoAge240 / (f32)g_SndUfoDuration240;

		if (percentage < 1.0f) { // less than 100% complete
			if (percentage < 0.65f) {
				percentage = percentage / 0.65f * 0.3f + 0.7f;
			} else {
				percentage = (percentage - 0.65f) / 0.35f * 0.8f + 1.0f;
			}

			if (lvIsPaused()) {
				// Fade out volume during pause instead of stopping abruptly
				if (g_SndUfoVolume > 0) {
					g_SndUfoVolume -= g_Vars.diffframe240 * PALUP(120);

					if (g_SndUfoVolume < 0) {
						g_SndUfoVolume = 0;
					}
				}

				if (g_SndUfoVolume) {
					sndAdjust(&g_SndUfoHandle, 0, g_SndUfoVolume, 64, -1, percentage, 0, -1, 1);
				} else if (g_SndUfoHandle != NULL) {
					audioStop(g_SndUfoHandle);
					g_SndUfoHandle = NULL;
				}
			} else {
				// Not paused
				if (g_SndUfoHandle == NULL) {
					sndStart(var80095200, SFX_UFOHUM, &g_SndUfoHandle, -1, -1, -1, -1, -1);
				}

				// Fade in over about 2.4 seconds
				if (g_SndUfoVolume < 32767) {
					g_SndUfoVolume += g_Vars.diffframe240 * PALUP(40);

					if (g_SndUfoVolume > 32767) {
						g_SndUfoVolume = 32767;
					}
				}

				sndAdjust(&g_SndUfoHandle, 0, g_SndUfoVolume, 64, -1, percentage, 0, -1, 1);
			}
		} else {
			// Reached the configured fade out point
			if (g_SndUfoVolume > 0) {
				g_SndUfoVolume -= g_Vars.diffframe240 * PALUP(120);

				if (g_SndUfoVolume < 0) {
					g_SndUfoVolume = 0;
				}
			}

			if (g_SndUfoVolume) {
				sndAdjust(&g_SndUfoHandle, 0, g_SndUfoVolume, 64, -1, -1, 0, -1, 1);
			} else if (g_SndUfoHandle != NULL) {
				audioStop(g_SndUfoHandle);
				g_SndUfoHandle = NULL;
			}
		}
	}
}
