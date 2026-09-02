#include "winpch.h"
#include "items.h"
#include <algorithm>
#include <vector>
#include <ctype.h>
#include "display.h"
#include "player.h"
#include "repair.h"
#include "hiscore.h"
#include "editor.h"
#include "shop.h"
#include "vars.h"
#include "goal.h"
#include "log.h"
#include "bullet.h"
#include "math_extras.h"
#include "string_extras.h"

static const item_t baseItems[] = {
	{"None",0,0,0,0,0,0,0,0,{},ITA_NONE,ITP_FREEWALK,ITRG_NONE,ITB_NONE,0,0,0,"",0},

	// name, x/y offset, sprite number, from->to color, brightness
	// rarity, theme, appearance, passability, trigger, customJSP,
	// effect, effect amt, msg, sound

	// Basic items
	{"Hammer Up!",0,0,0,0,0,0,0, // animated
		0,ITH_COLLECTIBLE|ITH_POWERUP,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_HAMMER,1,"Hammer Up!",SND_HAMMERUP},
	{"Pants Of Power",0,0,0,1,0,0,0, // animated
		0,ITH_COLLECTIBLE|ITH_POWERUP,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_PANTS,1,"Pants Of Power!",SND_PANTS},
	{"Brain",0,0,0,2,0,0,0, // animated
		0,ITH_COLLECTIBLE,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_BRAIN,1,"",SND_GETBRAIN},
	{"Candle",0,0,0,3,0,0,0, // animated
		0,ITH_COLLECTIBLE,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_CANDLE,1,"",SND_CANDLEGET},
	{"Coin",0,0,0,4,0,0,0, // TODO: animate.
		100,ITH_COLLECTIBLE,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_COIN,1,"",SND_COINGET},

	{"Reverse Hammer",0,0,0,5,4,3,2, // animated
		0,ITH_POWERUP,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_POWERUP,PU_REVERSE,"Reverse Hammer!",SND_REVERSE},
	{"Sproingy Spring",0,0,0,6,0,0,0, // animated
		0,ITH_POWERUP,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_POWERUP,PU_REFLECT,"Sproingy Spring!",SND_REFLECT},
	{"Hammer Down.",0,0,0,0,4,0,-2, // animated
		0,ITH_POWERUP,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_HAMMER,-1,"Hammer Down...",SND_HAMMERDN},
	{"Pants of Futility",0,0,0,1,3,0,0, // animated
		0,ITH_POWERUP,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_PANTS,-1,"Pants of Futility...",SND_PANTSDN},

	// Weapons
	{"Missiles",0,0,0,7,0,0,0,
		50,ITH_WEAPON,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_WEAPON,WPN_MISSILES,"Missile Pack!",SND_WEAPON},
	{"AK-Laser",0,0,0,8,0,0,0, // TODO: make sprite
		50,ITH_WEAPON,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_WEAPON,WPN_AK8087,"AK-Laser!",SND_WEAPON},
	{"Bombs",0,0,0,9,0,0,0,
		50,ITH_WEAPON,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_WEAPON,WPN_BOMBS,"Cherry Bombs!",SND_WEAPON},
	{"Toaster",0,0,0,10,0,0,0,
		50,ITH_WEAPON,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_WEAPON,WPN_FLAME,"Toaster!",SND_WEAPON},
	{"BIG Axe",0,0,0,11,0,0,0,
		50,ITH_WEAPON,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_WEAPON,WPN_BIGAXE,"BIG Axe!",SND_WEAPON},
	{"Lightning Wand",0,0,0,12,0,0,0,
		50,ITH_WEAPON,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_WEAPON,WPN_LIGHTNING,"Lightning Wand!",SND_WEAPON},
	{"Spears",0,0,0,13,0,0,0,
		50,ITH_WEAPON,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_WEAPON,WPN_SPEAR,"Flimsy Spears!",SND_WEAPON},
	{"Machete",0,0,0,13,0,0,0, // TODO: make sprite
		50,ITH_WEAPON,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_WEAPON,WPN_MACHETE,"Machete!",SND_WEAPON},
	{"Mines",0,0,0,15,0,0,0,
		50,ITH_WEAPON,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_WEAPON,WPN_MINES,"Mines!",SND_WEAPON},
	{"Freeze Ray",0,0,0,16,0,0,0,
		50,ITH_WEAPON,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_WEAPON,WPN_FREEZE,"Freeze Ray!",SND_WEAPON},

	// New weapons
	{"Boomerang",0,0,0,17,0,0,0,
		50,ITH_WEAPON,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_WEAPON,WPN_BOOMERANG,"Boomerang!",SND_WEAPON},
	{"Megaphone",0,0,0,18,0,0,0, // TODO: make sprite
		50,ITH_WEAPON,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_WEAPON,WPN_MEGAPHONE,"Megaphone!",SND_WEAPON},
	{"Rocket Launcher",0,0,0,19,0,0,0, // TODO: make sprite
		50,ITH_WEAPON,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_WEAPON,WPN_ROCKETEER,"Rocket Launcher!",SND_WEAPON},
	{"Lunchbox",0,0,0,20,0,0,0, // TODO: make sprite
		50,ITH_WEAPON,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_WEAPON,WPN_LUNCHBOX,"Lunchbox!",SND_WEAPON},

	// Fancy weapons
	{"Torch",0,0,0,21,0,0,0, // TODO: make sprite
		0,ITH_WEAPON,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_WEAPON,WPN_TORCH,"Torch!",SND_WEAPON},
	{"Turrets",0,0,0,22,4,1,5, // TODO: make sprite
		0,ITH_WEAPON,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_WEAPON,WPN_TURRET,"Turrets!",SND_WEAPON},
	{"Mind Control Ray",0,0,0,23,0,0,0, // TODO: make sprite
		5,ITH_WEAPON,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_WEAPON,WPN_MINDCONTROL,"Mind Control Ray!",SND_WEAPON},
	{"Reflection Shield",0,0,0,24,0,0,0, // TODO: make sprite
		5,ITH_WEAPON,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_WEAPON,WPN_REFLECTOR,"Reflection Shield!",SND_WEAPON},
	{"Jetpack",0,0,0,25,0,0,0,
		0,ITH_WEAPON,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_WEAPON,WPN_JETPACK,"Jetpack!",SND_WEAPON},
	{"Swapgun",0,0,0,26,0,0,0, // TODO: make sprite
		0,ITH_WEAPON,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_WEAPON,WPN_SWAPGUN,"Swapgun!",SND_WEAPON},
	{"Scanner",0,0,0,27,0,0,0, // TODO: make sprite
		30,ITH_WEAPON,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_WEAPON,WPN_SCANNER,"Scanner!",SND_WEAPON},
	{"Stopwatch",0,0,0,28,0,0,0, // TODO: make sprite  (PRIORITY!)
		30,ITH_WEAPON,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_WEAPON,WPN_STOPWATCH,"Stopwatch!",SND_WEAPON},
	{"R.G. Blaster",0,0,0,29,0,0,0, // TODO: make sprite  (PRIORITY!)
		5,ITH_WEAPON,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_WEAPON,WPN_RAINBOWGUN,"R.G. Blaster!",SND_WEAPON},
	{"Abyssinator",0,0,0,30,0,0,0, // TODO: make sprite
		0,ITH_WEAPON,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_WEAPON,WPN_BLACKHOLE,"Abyssinator!",SND_WEAPON},
	{"Glue Gun",0,0,0,31,0,0,0, // TODO: make sprite
		30,ITH_WEAPON,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_WEAPON,WPN_GLUEGUN,"Glue Gun!",SND_WEAPON},
	{"Bouapha's Favorite Gun",0,0,0,32,0,0,0, // TODO: make sprite
		0,ITH_WEAPON,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_WEAPON,WPN_BFG,"B.F.G.!!!",SND_GONG},
	{"Throwing Stars",0,0,0,33,0,0,0, // TODO: make sprite (PRIORITY!)
		30,ITH_WEAPON,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_WEAPON,WPN_THROWSTARS,"Throwing Stars!",SND_WEAPON},
	{"Molecular Destabilizer",0,0,0,34,0,0,0, // TODO: make sprite
		30,ITH_WEAPON,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_WEAPON,WPN_DESTABILIZER,"Molecular Destabilizer!",SND_WEAPON},
	{"Decoy Kit",0,0,0,35,0,0,0, // TODO: make sprite  (PRIORITY!)
		30,ITH_WEAPON,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_WEAPON,WPN_DECOYKIT,"Decoy Kit!",SND_ROBOBOUAPHAON},

	// Mech weapons
	{"Power Armor",0,0,0,36,0,0,0, // TODO: make sprite
		0,ITH_WEAPON|ITH_POWERUP,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_WEAPON,WPN_PWRARMOR,"Power Armor!",SND_ROBOBOUAPHAON},
	{"Mini-Sub",0,0,0,37,0,0,0, // TODO: make sprite
		0,ITH_WEAPON|ITH_POWERUP,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_WEAPON,WPN_MINISUB,"Mini-Sub!",SND_ROBOBOUAPHAON},

	// Food + powerups
	{"Chinese Takeout",0,0,0,38,0,0,0, // TODO: make sprite
		100,ITH_COLLECTIBLE|ITH_POWERUP,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_HEALRANDOM,0,"",SND_FOOD},
	{"Energy Shield",0,0,0,39,0,0,0, // TODO: make (better) sprite
		10,ITH_POWERUP,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_POWERUP,PU_SHIELD,"",SND_SHIELD},
	{"Radioactive Chinese",0,0,0,40,0,0,0, // TODO: make (better) sprite
		5,ITH_POWERUP,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_WEIRD,0,"",SND_FOOD}, // maybe make this a new sound
	{"Pizza",0,0,0,41,0,0,0, // TODO: make sprite
		75,ITH_POWERUP,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_HEAL,32,"",SND_FOOD}, // make a PIZZA sound? maybe!
	{"Garlic",0,0,0,42,0,0,0,
		0,ITH_POWERUP,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_POWERUP,PU_GARLIC,"",SND_FOOD},
	{"Orbit Shooter",0,0,0,43,0,0,0, // TODO: make sprite
		25,ITH_POWERUP,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_ORBITER,0,"",SND_SHIELD}, // little orbiter noise
	{"Orbit Bomber",0,0,0,43,1,4,-2,
		5,ITH_POWERUP,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_ORBITER,1,"",SND_SHIELD}, // TODO: rework orbiter editor logic
	{"Orbit Zapper",0,0,0,43,1,7,4,
		15,ITH_POWERUP,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_ORBITER,2,"",SND_SHIELD},
	{"Particle Accelerator",0,0,0,44,0,0,0, // TODO: make sprite
		0,ITH_POWERUP,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_POWERUP,PU_SPEED,"Particle Accelerator!",SND_SPEEDUP},
	{"Power Belt",0,0,0,45,0,0,0, // TODO: make sprite
		5,ITH_POWERUP,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_POWERUP,PU_STRENGTH,"Power Belt!",SND_SPEEDUP},
	{"The Cloak Invisible",0,0,0,46,0,0,0, // TODO: make sprite
		0,ITH_POWERUP,ITA_GLOWING,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_POWERUP,PU_INVISO,"The Cloak Invisible!",SND_SPEEDUP},
	{"Ammo:Small",0,0,0,47,0,0,0, // TODO: make sprite
		0,ITH_POWERUP|ITH_COLLECTIBLE,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_POWERUP,PU_AMMO2,"Ammo Pack!",SND_WORLDTURN},
	{"Ammo:Medium",0,0,0,48,0,0,0, // TODO: make sprite
		0,ITH_POWERUP|ITH_COLLECTIBLE,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_POWERUP,PU_AMMO2,"Ammo Pack!",SND_WORLDTURN}, // TODO: make ammo thingy
	{"Ammo Crate",0,0,0,49,0,0,0, // TODO: make sprite
		0,ITH_POWERUP|ITH_COLLECTIBLE,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_POWERUP,PU_AMMO,"Ammo Crate!!",SND_AMMOCRATE},
	{"Scuba Gear",0,0,0,50,0,0,0, // TODO: make sprite
		0,ITH_POWERUP|ITH_COLLECTIBLE,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_OXYGEN,128,"Scuba Gear!",SND_SCUBA},
	{"Supreme Squeezy Cheese",0,0,0,51,0,0,0, // TODO: make sprite
		0,ITH_POWERUP,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_POWERUP,PU_CHEESE,"Supreme Squeezy Cheese!!",SND_GONG},
	{"Aqueous Boots",0,0,0,52,0,0,0, // TODO: make sprite
		0,ITH_POWERUP,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_POWERUP,PU_WATRWALK,"Aqueous Boots!!",SND_GONG},
	{"Pocket",0,0,0,53,0,0,0, // TODO: animate it!
		0,ITH_POWERUP,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_POCKET,1,"Pocket!",SND_WORLDTURN}, // also, make a pocket sound
	// larger versions of brain/coin/candle
	{"Brain:Big",0,0,0,54,0,0,0, // animated
		0,ITH_COLLECTIBLE,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_BRAIN,4,"",SND_BIGBRAIN},
	{"Candle:Big",0,0,0,55,0,0,0, // animated
		0,ITH_COLLECTIBLE,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_CANDLE,4,"",SND_BIGCANDLE},
	{"Coin:Big",0,0,0,56,0,0,0, // TODO: animate.
		20,ITH_COLLECTIBLE,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_COIN,5,"",SND_BIGCOIN},

	// Keys
	{"Key:Yellow",0,0,0,57,0,0,0, // animated
		0,ITH_COLLECTIBLE,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_KEY,1,"",SND_GETKEY},
	{"Key:Red",0,0,0,58,0,0,0, // animated
		0,ITH_COLLECTIBLE,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_RKEY,1,"",SND_GETKEY},
	{"Key:Green",0,0,0,59,0,0,0, // animated
		0,ITH_COLLECTIBLE,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_GKEY,1,"",SND_GETKEY},
	{"Key:Blue",0,0,0,60,0,0,0, // animated
		0,ITH_COLLECTIBLE,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_BKEY,1,"",SND_GETKEY},
	{"Keychain:Pumpkin",0,0,0,61,0,0,0, // TODO: make sprite, animate.
		0,ITH_COLLECTIBLE,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_KEYCHAIN,0,"COOL! A Pumpkin Keychain!!",SND_GETKEYCHAIN},
	{"Keychain:Shroom",0,0,0,62,0,0,0, // TODO: make sprite, animate.
		0,ITH_COLLECTIBLE,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_KEYCHAIN,1,"COOL! A Shroom Keychain!!",SND_GETKEYCHAIN},
	{"Keychain:Martian",0,0,0,63,0,0,0, // TODO: make sprite, animate.
		0,ITH_COLLECTIBLE,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_KEYCHAIN,2,"COOL! A Martian Keychain!!",SND_GETKEYCHAIN},
	{"Keychain:Frog",0,0,0,64,0,0,0, // TODO: make sprite, animate.
		0,ITH_COLLECTIBLE,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_KEYCHAIN,3,"COOL! A Frog Keychain!!",SND_GETKEYCHAIN},
	{"Keychain:Bodzha",0,0,0,65,0,0,0, // TODO: make sprite, animate.
		0,ITH_COLLECTIBLE,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_KEYCHAIN,4,"COOL! A Bodzha Keychain!!",SND_GETKEYCHAIN},
	{"Key of Lunacy",0,0,0,66,0,0,0, // TODO: animate.
		0,ITH_COLLECTIBLE,ITA_LOONYKEY,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_LOONYKEY,1,"KEY OF LUNACY!!!",SND_LOONYKEY},

	// Buildings / exits
	{"Exit Door",0,0,0,67,0,0,0, // TODO: make.
		0,ITH_DECOR|ITH_ENTRANCE,ITA_SHADOW,ITP_FREEWALK,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Hollow Tree",0,0,0,68,0,0,0,
		0,ITH_DECOR|ITH_ENTRANCE,ITA_SHADOW,ITP_FREEWALK,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"House",0,0,0,69,0,0,0,
		0,ITH_DECOR|ITH_ENTRANCE,ITA_SHADOW,ITP_FREEWALK,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Castle",0,0,0,70,0,0,0,
		0,ITH_DECOR|ITH_ENTRANCE,ITA_SHADOW,ITP_FREEWALK,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Igloo",0,0,0,71,0,0,0,
		0,ITH_DECOR|ITH_ENTRANCE,ITA_SHADOW,ITP_FREEWALK,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},

	// Terrain / scenery
	{"Grass 1",0,0,0,72,0,0,0,
		0,ITH_DECOR|ITH_PLANTS,ITA_NONE,ITP_FREEWALK,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Grass 2",0,0,0,73,0,0,0,
		0,ITH_DECOR|ITH_PLANTS,ITA_NONE,ITP_FREEWALK,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Cone",0,0,0,74,0,0,0,
		0,ITH_OBSTACLE|ITH_URBAN,ITA_SHADOW,ITP_SOLID,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Cone:Fallen",0,0,0,75,0,0,0,
		0,ITH_OBSTACLE|ITH_URBAN,ITA_NONE,ITP_FREEWALK,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Mailbox",0,0,0,77,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF|ITH_URBAN,ITA_SHADOW,ITP_BULLETPROOF,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Mailbox:Collection",0,0,0,79,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF|ITH_URBAN,ITA_SHADOW,ITP_BULLETPROOF,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Hydrant",0,0,0,80,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF|ITH_URBAN,ITA_SHADOW,ITP_BULLETPROOF,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Barrel:Metal",0,0,0,81,0,0,0,
		0,ITH_OBSTACLE|ITH_URBAN,ITA_SHADOW,ITP_SOLID,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Barrel:Toxic Waste",0,0,0,83,0,0,0,
		0,ITH_OBSTACLE|ITH_URBAN,ITA_SHADOW,ITP_SOLID,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Barrel:Oil Drum",0,0,0,82,0,0,0,
		0,ITH_OBSTACLE|ITH_URBAN,ITA_NONE,ITP_SOLID,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Crate:Wood",0,0,0,84,0,0,0,
		0,ITH_OBSTACLE|ITH_URBAN,ITA_NONE,ITP_SOLID,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Crate:Metal",0,0,0,85,0,0,0,
		0,ITH_OBSTACLE|ITH_URBAN,ITA_NONE,ITP_SOLID,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},

	{"Wooden Post",0,0,0,86,0,0,0,
		0,ITH_OBSTACLE|ITH_SIGNS,ITA_NONE,ITP_SOLID,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Sign:Metal",0,0,0,87,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF|ITH_SIGNS,ITA_SHADOW,ITP_BULLETPROOF,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Wooden Sign:Text",0,0,0,88,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF|ITH_SIGNS,ITA_SHADOW,ITP_BULLETPROOF,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Wooden Sign:Skull",0,0,0,89,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF|ITH_SIGNS,ITA_SHADOW,ITP_BULLETPROOF,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Wooden Sign:Left",0,0,0,90,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF|ITH_SIGNS,ITA_SHADOW,ITP_BULLETPROOF,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Wooden Sign:Right",0,0,0,91,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF|ITH_SIGNS,ITA_SHADOW,ITP_BULLETPROOF,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Wooden Sign:Info",0,0,0,92,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF|ITH_SIGNS,ITA_SHADOW,ITP_BULLETPROOF,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Sign:Danger",0,0,0,93,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF|ITH_URBAN|ITH_SIGNS,ITA_SHADOW,ITP_BULLETPROOF,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Sign:Stop",0,0,0,94,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF|ITH_URBAN|ITH_SIGNS,ITA_SHADOW,ITP_BULLETPROOF,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Sign:Do Not Enter",0,0,0,95,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF|ITH_URBAN|ITH_SIGNS,ITA_SHADOW,ITP_BULLETPROOF,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Sign:Left",0,0,0,96,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF|ITH_URBAN|ITH_SIGNS,ITA_SHADOW,ITP_BULLETPROOF,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Sign:Right",0,0,0,97,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF|ITH_URBAN|ITH_SIGNS,ITA_SHADOW,ITP_BULLETPROOF,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Sign:Down",0,0,0,98,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF|ITH_URBAN|ITH_SIGNS,ITA_SHADOW,ITP_BULLETPROOF,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Sign:Up",0,0,0,99,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF|ITH_URBAN|ITH_SIGNS,ITA_SHADOW,ITP_BULLETPROOF,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Streetlight",0,0,0,100,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF|ITH_URBAN,ITA_SHADOW,ITP_BULLETPROOF,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Sign:Path1",0,0,0,101,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF|ITH_URBAN,ITA_SHADOW,ITP_BULLETPROOF,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Sign:Path2",0,0,0,102,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF|ITH_URBAN,ITA_SHADOW,ITP_BULLETPROOF,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Wooden Barrel",0,0,0,103,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF|ITH_URBAN,ITA_SHADOW,ITP_BULLETPROOF,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Lever:Left",0,0,0,105,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF,ITA_SHADOW,ITP_BULLETPROOF,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Lever:Right",0,0,0,106,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF,ITA_SHADOW,ITP_BULLETPROOF,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Streetlight:Dim",0,0,0,100,5,0,-4,
		0,ITH_OBSTACLE|ITH_BULLETPROOF|ITH_URBAN,ITA_SHADOW,ITP_BULLETPROOF,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Traffic Light:Red",0,0,0,108,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF|ITH_URBAN,ITA_SHADOW,ITP_BULLETPROOF,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Traffic Light:Yellow",0,0,0,109,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF|ITH_URBAN,ITA_SHADOW,ITP_BULLETPROOF,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Traffic Light:Green",0,0,0,110,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF|ITH_URBAN,ITA_SHADOW,ITP_BULLETPROOF,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},

	// Trees / plants
	{"Tree 1",0,0,0,111,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF|ITH_PLANTS,ITA_NONE,ITP_BULLETPROOF,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Tree 2",0,0,0,112,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF|ITH_PLANTS,ITA_NONE,ITP_BULLETPROOF,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Tree 3",0,0,0,113,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF|ITH_PLANTS,ITA_NONE,ITP_BULLETPROOF,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Bush 1",0,0,0,114,0,0,0,
		0,ITH_OBSTACLE|ITH_PLANTS,ITA_NONE,ITP_SOLID,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Bush 2",0,0,0,115,0,0,0,
		0,ITH_OBSTACLE|ITH_PLANTS,ITA_NONE,ITP_SOLID,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Bush 3",0,0,0,116,0,0,0,
		0,ITH_OBSTACLE|ITH_PLANTS,ITA_NONE,ITP_SOLID,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Palm Tree 1",0,0,0,117,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF|ITH_PLANTS,ITA_NONE,ITP_BULLETPROOF,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Palm Tree 2",0,0,0,118,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF|ITH_PLANTS,ITA_NONE,ITP_BULLETPROOF,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Fat Palm 1",0,0,0,119,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF|ITH_PLANTS,ITA_NONE,ITP_BULLETPROOF,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Fat Palm 2",0,0,0,120,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF|ITH_PLANTS,ITA_NONE,ITP_BULLETPROOF,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Pine Tree",0,0,0,121,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF|ITH_PLANTS,ITA_NONE,ITP_BULLETPROOF,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Pine Tree:Christmas",0,0,0,122,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF|ITH_PLANTS,ITA_NONE,ITP_BULLETPROOF,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Fallen Tree",0,0,0,123,0,0,0,
		0,ITH_OBSTACLE|ITH_PLANTS,ITA_NONE,ITP_SOLID,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Dead Tree 1",0,0,0,124,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF|ITH_PLANTS,ITA_NONE,ITP_BULLETPROOF,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Dead Tree 2",0,0,0,125,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF|ITH_PLANTS,ITA_NONE,ITP_BULLETPROOF,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Tree Stump",0,0,0,126,0,0,0,
		0,ITH_OBSTACLE|ITH_PLANTS,ITA_NONE,ITP_SOLID,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Seaweed 1",0,0,0,127,0,0,0,
		0,ITH_OBSTACLE|ITH_PLANTS,ITA_NONE,ITP_SOLID,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Seaweed 2",0,0,0,128,0,0,0,
		0,ITH_OBSTACLE|ITH_PLANTS,ITA_NONE,ITP_SOLID,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Seaweed 3",0,0,0,129,0,0,0,
		0,ITH_OBSTACLE|ITH_PLANTS,ITA_NONE,ITP_SOLID,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Cattails",0,0,0,130,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF|ITH_PLANTS,ITA_NONE,ITP_BULLETPROOF,ITRG_PROJECTILE,ITB_NONE,0,
		IE_DESTROY,1,"",SND_ACIDSPLAT},
	{"Toadstool",0,0,0,131,0,0,0,
		0,ITH_OBSTACLE|ITH_PLANTS,ITA_NONE,ITP_SOLID,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},

	// Rocks
	{"Rocks:Large",0,0,0,132,0,0,0,
		0,ITH_OBSTACLE|ITH_ROCKS,ITA_NONE,ITP_SOLID,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Rocks:Small",0,0,0,133,0,0,0,
		0,ITH_OBSTACLE|ITH_ROCKS,ITA_NONE,ITP_SOLID,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Boulder:Tall",0,0,0,135,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF|ITH_ROCKS,ITA_NONE,ITP_BULLETPROOF,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Boulder:Short",0,0,0,136,0,0,0,
		0,ITH_OBSTACLE|ITH_ROCKS,ITA_NONE,ITP_SOLID,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Rocks:Huge",0,0,0,138,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF|ITH_ROCKS|ITH_LARGE,ITA_NONE,ITP_BULLETPROOF,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Pebble",0,0,0,137,0,0,0,
		0,ITH_DECOR|ITH_ROCKS,ITA_NONE,ITP_FREEWALK,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Bubbles",0,0,0,139,0,0,0,
		0,ITH_DECOR,ITA_GLOWING,ITP_FREEWALK,ITRG_NONE,ITB_BUBBLES,0,
		IE_NONE,0,"",0},

	// Doors / blocks
	{"Yellow Door:Front",0,0,0,140,0,0,0,
		0,ITH_BULLETPROOF|ITH_OBSTACLE|ITH_COLLECTIBLE,ITA_NONE,ITP_BULLETPROOF,ITRG_PLAYERBUMP,ITB_NONE,0,
		IE_DOOR,0,"",0},
	{"Red Door:Front",0,0,0,141,0,0,0,
		0,ITH_BULLETPROOF|ITH_OBSTACLE|ITH_COLLECTIBLE,ITA_NONE,ITP_BULLETPROOF,ITRG_PLAYERBUMP,ITB_NONE,0,
		IE_DOOR,1,"",0},
	{"Green Door:Front",0,0,0,142,0,0,0,
		0,ITH_BULLETPROOF|ITH_OBSTACLE|ITH_COLLECTIBLE,ITA_NONE,ITP_BULLETPROOF,ITRG_PLAYERBUMP,ITB_NONE,0,
		IE_DOOR,2,"",0},
	{"Blue Door:Front",0,0,0,143,0,0,0,
		0,ITH_BULLETPROOF|ITH_OBSTACLE|ITH_COLLECTIBLE,ITA_NONE,ITP_BULLETPROOF,ITRG_PLAYERBUMP,ITB_NONE,0,
		IE_DOOR,3,"",0},
	{"Yellow Door:Side",0,0,0,144,0,0,0,
		0,ITH_BULLETPROOF|ITH_OBSTACLE|ITH_COLLECTIBLE,ITA_NONE,ITP_BULLETPROOF,ITRG_PLAYERBUMP,ITB_NONE,0,
		IE_DOOR,0,"",0},
	{"Red Door:Side",0,0,0,145,0,0,0,
		0,ITH_BULLETPROOF|ITH_OBSTACLE|ITH_COLLECTIBLE,ITA_NONE,ITP_BULLETPROOF,ITRG_PLAYERBUMP,ITB_NONE,0,
		IE_DOOR,1,"",0},
	{"Green Door:Side",0,0,0,146,0,0,0,
		0,ITH_BULLETPROOF|ITH_OBSTACLE|ITH_COLLECTIBLE,ITA_NONE,ITP_BULLETPROOF,ITRG_PLAYERBUMP,ITB_NONE,0,
		IE_DOOR,2,"",0},
	{"Blue Door:Side",0,0,0,147,0,0,0,
		0,ITH_BULLETPROOF|ITH_OBSTACLE|ITH_COLLECTIBLE,ITA_NONE,ITP_BULLETPROOF,ITRG_PLAYERBUMP,ITB_NONE,0,
		IE_DOOR,3,"",0},

	// Furniture / miscellaneous
	{"Chair:DownRight",0,0,0,148,0,0,0,
		0,ITH_OBSTACLE|ITH_URBAN,ITA_NONE,ITP_SOLID,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Chair:DownLeft",0,0,0,149,0,0,0,
		0,ITH_OBSTACLE|ITH_URBAN,ITA_NONE,ITP_SOLID,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Chair:UpLeft",0,0,0,150,0,0,0,
		0,ITH_OBSTACLE|ITH_URBAN,ITA_NONE,ITP_SOLID,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Chair:UpRight",0,0,0,151,0,0,0,
		0,ITH_OBSTACLE|ITH_URBAN,ITA_NONE,ITP_SOLID,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Wooden Table",0,0,0,152,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF|ITH_URBAN,ITA_NONE,ITP_BULLETPROOF,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Trash Can",0,0,0,153,0,0,0,
		0,ITH_OBSTACLE|ITH_URBAN,ITA_NONE,ITP_SOLID,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Trash Can:Filled",0,0,0,154,0,0,0,
		0,ITH_OBSTACLE|ITH_URBAN,ITA_NONE,ITP_SOLID,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Target",0,0,0,155,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF,ITA_NONE,ITP_SOLID,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},

	// Potions / collectibles
	{"Mana Potion",0,0,0,156,0,0,0, // TODO: make item effect
		0,ITH_POWERUP,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_NONE,1,"",0},
	{"Rage Potion",0,0,0,156,0,0,0, // TODO: animate
		0,ITH_POWERUP,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_RAGE,128,"",0},
	{"Present",0,0,0,158,0,0,0, // TODO: make item effect (random weapon)
		50,ITH_POWERUP|ITH_WEAPON,ITA_NONE,ITP_BARRIER,ITRG_PROJECTILE,ITB_NONE,0,
		IE_DROPITEM,0,"",0},
	{"Small Gemstone",0,0,0,159,0,0,0, // TODO: make add score effect
		0,ITH_COLLECTIBLE,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_SCORE,100,"",0},
	{"Large Gemstone",0,0,0,160,0,0,0, // TODO: make add score effect
		0,ITH_COLLECTIBLE,ITA_NONE,ITP_PICKUP,ITRG_PICKUP,ITB_NONE,0,
		IE_SCORE,500,"",0},

	// Miscellaneous scenery
	{"Mine Block",0,0,0,161,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF,ITA_NONE,ITP_BULLETPROOF,ITRG_MINECART,ITB_NONE,0,
		IE_DESTROY,2,"",0},
	{"Tall Grass",0,0,0,162,0,0,0,
		0,ITH_OBSTACLE|ITH_PLANTS,ITA_NONE,ITP_SOLID,ITRG_MACHETE,ITB_NONE,0,
		IE_DESTROY,1,"",0},
	{"Hay Bale",0,0,0,163,0,0,0, // todo: have them burn
		0,ITH_OBSTACLE,ITA_NONE,ITP_SOLID,ITRG_NONE,ITB_NONE,0,
		IE_DESTROY,5,"",0},
	{"Powder Keg",0,0,0,164,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF,ITA_NONE,ITP_BULLETPROOF,ITRG_MACHETE,ITB_NONE,0,
		IE_DESTROY,1,"",0},
	{"Sakura Tree 1",0,0,0,165,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF|ITH_PLANTS,ITA_NONE,ITP_BULLETPROOF,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Sakura Tree 2",0,0,0,166,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF|ITH_PLANTS,ITA_NONE,ITP_BULLETPROOF,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Jade Crystals 1",0,0,0,167,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF|ITH_ROCKS|ITH_LARGE,ITA_NONE,ITP_BULLETPROOF,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Jade Crystals 2",0,0,0,168,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF|ITH_ROCKS|ITH_LARGE,ITA_NONE,ITP_BULLETPROOF,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"Gong",0,0,0,169,0,0,0,
		0,ITH_OBSTACLE|ITH_BULLETPROOF|ITH_LARGE,ITA_NONE,ITP_BULLETPROOF,ITRG_NONE,ITB_NONE,0,
		IE_NONE,0,"",0},
	{"New1",0,0,0,0,0,0,0,0,{},ITA_NONE,ITP_FREEWALK,ITRG_NONE,ITB_NONE,0,0,0,"",0},
	{"New2",0,0,0,0,0,0,0,0,{},ITA_NONE,ITP_FREEWALK,ITRG_NONE,ITB_NONE,0,0,0,"",0},
	{"New3",0,0,0,0,0,0,0,0,{},ITA_NONE,ITP_FREEWALK,ITRG_NONE,ITB_NONE,0,0,0,"",0},
	{"New4",0,0,0,0,0,0,0,0,{},ITA_NONE,ITP_FREEWALK,ITRG_NONE,ITB_NONE,0,0,0,"",0},
};
static_assert(std::size(baseItems) == NUM_ORIGINAL_ITEMS);

static const item_t emptyItem = { "Custom Item",0,0,0,0,0,0,0,0,ITH_CUSTOM,ITA_NONE,ITP_FREEWALK,ITRG_NONE,ITB_NONE,0,0,0,"",0 };

static sprite_set_t *itmSpr;
static sprite_set_t *animItmSpr;
static sprite_set_t *customItmSpr;
static byte glowism;
static int rndItem;
static std::vector<item_t> items;
static int totalRare;

static char customSpriteFilename[64] = "";

void InitItems(void)
{
	items.assign(std::begin(baseItems), std::end(baseItems));

	itmSpr=new sprite_set_t("graphics/items.jsp");
	animItmSpr = new sprite_set_t("graphics/items_animated.jsp");
	customItmSpr=NULL;
	glowism=0;
	SetupRandomItems();
	rndItem=GetRandomItem();

	ham_strcpy(customSpriteFilename, "");
	CalculateItemRenderExtents();
}

sprite_t* GetItemSprite(int spr)
{
	return itmSpr->GetSprite(spr);
}

sprite_t* GetAnimatedItemSprite(int spr)
{
	return animItmSpr->GetSprite(spr);
}

void ExitItems(void)
{
	delete itmSpr;
	delete animItmSpr;
	if (customItmSpr)
	{
		delete customItmSpr;
		customItmSpr = NULL;
	}
	items.clear();
}

int NumItemSprites(void)
{
	return itmSpr->GetCount();
}

int NewItem(void)
{
	if(items.size()==MAX_ITEMS)
		return -1;

	items.push_back(emptyItem);

	return items.size()-1;
}

void DeleteItem(int itm)
{
	if(itm<CUSTOM_ID_START)
		return;
	if(itm>=NumItems())
		return;

	RepairItems(itm);

	items.erase(items.begin() + itm);
}

void SetCustomItemSprites(const char* name)
{
	if (!strcmp(customSpriteFilename, name))
		return;
	ham_strcpy(customSpriteFilename, name);

	if (customItmSpr) delete customItmSpr;

	customItmSpr=new sprite_set_t();

	char buf[64];
	sprintf(buf, "user/%s", name);
	if (!customItmSpr->Load(buf))
	{
		// failed to load
		delete customItmSpr;
		customItmSpr = nullptr;
	}
	CalculateItemRenderExtents();
}

static void DetectCustomItemSprites(const world_t *world)
{
	// extract filename out of first special if possible
	for (const special_t &me : world->map[0]->special)
	{
		for (const effect_t &eff : me.effect)
		{
			if (eff.type == EFF_ITEMGRAPHICS)
			{
				SetCustomItemSprites(eff.text);
				return;
			}
		}
	}
}

sprite_set_t* CustomItemSprites()
{
	if (customItmSpr == NULL)
	{
		DetectCustomItemSprites(editing ? EditorGetWorld() : &curWorld);
	}
	return customItmSpr;
}

int NumCustomSprites(void)
{
	return CustomItemSprites() ? customItmSpr->GetCount() : 0;
}



void RenderAnimatedItem(int x, int y, word start, byte frames, byte type, char bright, MapRenderFlags flags, byte fromCol=255, byte toCol=255)
{
	byte b;
	sprite_t* curSpr;
	int clock = player.clock;

	switch(type)
	{
		case IT_UNHAMMER:
		case IT_UNPANTS:
		case IT_RADIOFOOD:
			clock /= 2;
			break;
	}

	switch(type)
	{
		case IT_PROJMIRROR: // reverse motion
			curSpr = animItmSpr->GetSprite(start - ((clock / 4) % frames));
			break;
		default:
			curSpr = animItmSpr->GetSprite(start + ((clock / 4) % frames));
			break;
	}

	
	if (items[type].appearance == ITA_SHADOW || items[type].appearance == ITA_LOONYKEY) // has a shadow
		SprDraw(x + items[type].xofs, y, -items[type].yofs + 1, 0, bright + items[type].bright, curSpr, DISPLAY_DRAWME | DISPLAY_SHADOW);


	if (items[type].appearance == ITA_LOONYKEY) // loony color (for loonykey?)
	{
		b = abs(16 - (glowism & 31));
		SprDraw(x + items[type].xofs, y, -items[type].yofs + 1, glowism / 32, bright + b + items[type].bright, curSpr, DISPLAY_DRAWME);
	}
	else if (items[type].appearance == ITA_GLOWING) // glowing
	{
		SprDraw(x + items[type].xofs, y, -items[type].yofs + 1, 0, bright + items[type].bright, curSpr, DISPLAY_DRAWME | DISPLAY_GLOW);
	}
	if (fromCol != toCol && fromCol != 255) // recolor
	{
		SprDrawOff(x >> FIXSHIFT, y >> FIXSHIFT, 0, fromCol, toCol, bright + items[type].bright, curSpr, DISPLAY_DRAWME);
	}
	else
	{
		SprDraw(x >> FIXSHIFT, y >> FIXSHIFT, 0, 255, bright + items[type].bright, curSpr, DISPLAY_DRAWME);
	}
}

#define ASPR_HAMMER		0
#define ASPR_HAMRVS		7 // reversed
#define ASPR_CHINESE	8
#define ASPR_PANTS		16
#define ASPR_BRAIN		24
#define ASPR_CANDLE		32
#define ASPR_KEYRGB		40
#define ASPR_SPRING		48
#define ASPR_CHEATZY	56	// todo: make 8 frames!
#define ASPR_BIGBRAIN	64

void RenderItem(int x,int y,int type,char bright,MapRenderFlags flags)
{
	sprite_t* sprite;
	byte b;

	if(!(flags&(MAP_SHOWPICKUPS|MAP_SHOWOTHERITEMS)))
		return;

	if(type==IT_RANDOM)
		type=rndItem;

	if(type>=NumItems() || type==0)
		return;

	if(!(flags&MAP_SHOWPICKUPS))
		if(items[type].passability == ITP_PICKUP)
			return;
	if(!(flags&MAP_SHOWOTHERITEMS))	// if not showing other items, show ONLY those with pickup
		if(items[type].passability != ITP_PICKUP)
			return;

	if(items[type].appearance == ITA_TILEIMG)
	{
		// can't shadow, glow, loonycolor, or even offcolor!
		SprDrawTile(x+items[type].xofs,y+items[type].yofs+1,items[type].sprNum,
					bright+items[type].bright,DISPLAY_DRAWME);
	}
	else
	{
		x-=3;

		if (items[type].customJSP)
		{
			sprite_set_t* custom = CustomItemSprites();
			if (custom)
				sprite = custom->GetSprite(items[type].sprNum < custom->GetCount() ? items[type].sprNum : 0);
			else
				sprite = itmSpr->GetSprite(8); // red X indicating invalid custom JSP file
		}
		else
			sprite = itmSpr->GetSprite(items[type].sprNum);


		switch(type)
		{
			case IT_HAMMERUP:
				RenderAnimatedItem((x + items[type].xofs) * FIXAMT, (y+items[type].yofs) * FIXAMT, ASPR_HAMMER, 8, type, bright, flags);
				return;
			case IT_PROJMIRROR:
				RenderAnimatedItem((x + items[type].xofs) * FIXAMT, (y+items[type].yofs) * FIXAMT, ASPR_HAMRVS, 8, type, bright, flags, 4, 3);
				return;
			case IT_UNHAMMER:
				RenderAnimatedItem((x + items[type].xofs) * FIXAMT, (y+items[type].yofs) * FIXAMT, ASPR_HAMMER, 8, type, bright, flags, 4, 0);
				return;
			case IT_TAKEOUT:
				RenderAnimatedItem((x + items[type].xofs) * FIXAMT, (y+items[type].yofs) * FIXAMT, ASPR_CHINESE, 8, type, bright, flags);
				return;
			case IT_PANTS:
				RenderAnimatedItem((x + items[type].xofs) * FIXAMT, (y + items[type].yofs) * FIXAMT, ASPR_PANTS, 8, type, bright, flags);
				return;
			case IT_UNPANTS:
				RenderAnimatedItem((x + items[type].xofs) * FIXAMT, (y + items[type].yofs) * FIXAMT, ASPR_PANTS, 8, type, bright, flags, 3, 0);
				return;
			case IT_BRAIN:
				RenderAnimatedItem((x + items[type].xofs) * FIXAMT, (y + items[type].yofs) * FIXAMT, ASPR_BRAIN, 8, type, bright, flags);
				return;
			case IT_CANDLE:
				RenderAnimatedItem((x + items[type].xofs) * FIXAMT, (y + items[type].yofs) * FIXAMT, ASPR_CANDLE, 8, type, bright, flags);
				return;
			case IT_KEYRED:
				RenderAnimatedItem((x + items[type].xofs) * FIXAMT, (y + items[type].yofs) * FIXAMT, ASPR_KEYRGB, 8, type, bright, flags);
				return;
			case IT_KEYGRN:
				RenderAnimatedItem((x + items[type].xofs) * FIXAMT, (y + items[type].yofs) * FIXAMT, ASPR_KEYRGB, 8, type, bright, flags, 4, 1);
				return;
			case IT_KEYBLU:
				RenderAnimatedItem((x + items[type].xofs) * FIXAMT, (y + items[type].yofs) * FIXAMT, ASPR_KEYRGB, 8, type, bright, flags, 4, 3);
				return;
			case IT_KEYONE: // todo: separate yellow key sprite?!
				RenderAnimatedItem((x + items[type].xofs) * FIXAMT, (y + items[type].yofs) * FIXAMT, ASPR_KEYRGB, 8, type, bright, flags, 4, 5);
				return;
			case IT_PROJBOUNCE:
				RenderAnimatedItem((x + items[type].xofs) * FIXAMT, (y + items[type].yofs) * FIXAMT, ASPR_SPRING, 8, type, bright, flags);
				return;
			case IT_BIGBRAIN:
				RenderAnimatedItem((x + items[type].xofs) * FIXAMT, (y + items[type].yofs) * FIXAMT, ASPR_BIGBRAIN, 8, type, bright, flags);
				return;
		}
		if(items[type].appearance == ITA_SHADOW || items[type].appearance == ITA_LOONYKEY)
		{
			SprDraw(x+items[type].xofs,y,-items[type].yofs+1,0,bright+items[type].bright,
				sprite,DISPLAY_DRAWME|DISPLAY_SHADOW);
		}
		switch (items[type].appearance)
		{
			case ITA_LOONYKEY:
				b = abs(16 - (glowism & 31));
				SprDraw(x + items[type].xofs, y, -items[type].yofs + 1, glowism / 32, bright + b + items[type].bright,
					sprite, DISPLAY_DRAWME);
				break;
			case ITA_GLOWING:
				SprDraw(x + items[type].xofs, y, -items[type].yofs + 1, 0, bright + items[type].bright,
					sprite, DISPLAY_DRAWME | DISPLAY_GLOW);
				break;
			default:
				if (items[type].fromColor == items[type].toColor)
					SprDraw(x + items[type].xofs, y, -items[type].yofs + 1, 255, bright + items[type].bright,
						sprite, DISPLAY_DRAWME);
				else
					SprDrawOff(x + items[type].xofs, y, -items[type].yofs + 1, items[type].fromColor, items[type].toColor,
						bright + items[type].bright, sprite, DISPLAY_DRAWME | DISPLAY_OFFCOLOR);
				break;
		}
	}
}

void InstaRenderItem(int x,int y,int type,char bright,MGLDraw *mgl)
{
	sprite_t* sprite;
	byte b;

	if(type==IT_RANDOM)
		type=rndItem;

	if(type>=NumItems() || type==0)
		return;

	if (items[type].customJSP)
	{
		sprite_set_t* custom = CustomItemSprites();
		if (custom)
			sprite = custom->GetSprite(items[type].sprNum < custom->GetCount() ? items[type].sprNum : 0);
		else
			sprite = itmSpr->GetSprite(8);
	} else
		sprite = itmSpr->GetSprite(items[type].sprNum);

	if(items[type].appearance == ITA_TILEIMG)
	{
		// can't shadow, glow, loonycolor, or even offcolor!
		RenderFloorTileTrans(x+items[type].xofs-TILE_WIDTH/2,y+items[type].yofs-TILE_HEIGHT/2,items[type].sprNum,
					bright+items[type].bright);
	}
	else
	{
		x-=3;
	
		// draw shadow
		if (items[type].appearance == ITA_SHADOW)
		{
			sprite->DrawShadow(x + items[type].xofs, y + items[type].yofs, mgl);
		}
		
		if (items[type].appearance == ITA_GLOWING) // draw glowing
		{
			sprite->DrawGlow(x + items[type].xofs, y + items[type].yofs, mgl, bright + items[type].bright);
		}
		else if (items[type].appearance == ITA_LOONYKEY) // draw loonykey
		{
			sprite->DrawShadow(x + items[type].xofs, y + items[type].yofs, mgl);
			b = abs(16 - (glowism & 31));
			sprite->DrawColored(x + items[type].xofs, y + items[type].yofs, mgl, glowism / 32, bright + b + items[type].bright);
		}
		
		// recolor
		if (items[type].fromColor == items[type].toColor)
		{
			sprite->DrawBright(x + items[type].xofs, y + items[type].yofs, mgl, bright + items[type].bright);
		}
		else
		{
			sprite->DrawOffColor(x + items[type].xofs, y + items[type].yofs, mgl, items[type].fromColor, items[type].toColor, bright + items[type].bright);
		}
	}
}

void DrawRedX(int x,int y,bool candle,MGLDraw *mgl)
{
	itmSpr->GetSprite(171)->Draw(x-5,y-11,mgl);
}

item_t *GetItem(int type)
{
	if(type<0 || type>=NumItems())
		return NULL;
	return &items[type];
}

word NumItems(void)
{
	return items.size();
}

void UpdateItems(void)
{
	glowism++;

	if(editing==1)
	{
		if((glowism&7)==0)
			rndItem=GetRandomItem();
	}
}

void SetupRandomItems(void)
{
	totalRare=0;
	for (const item_t &item : items)
	{
		totalRare += item.rarity;
	}
	if(totalRare==0)
		totalRare=1;
}

int GetRandomItem(void)
{
	int i,last;
	int rareNum,curRare;

	rareNum=Random(totalRare);
	curRare=0;
	last=0;
	for(i=0;i<NumItems();i++)
	{
		curRare+=items[i].rarity;
		if(items[i].rarity>0 && rareNum<=curRare)
			return i;
		if(items[i].rarity)
			last=i;
	}
	// should never fall through!  But just in case...
	return last;
}

int GetTotalRarity(void)
{
	return totalRare;
}

const item_t *GetBaseItem(int type)
{
	if(type<0 || type>=CUSTOM_ID_START)
		return NULL;
	return &baseItems[type];
}

int BrainsGiven(int type)
{
	if(type<0 || type>=NumItems())
		return 0;
	if(items[type].effect!=IE_BRAIN)
		return 0;
	return items[type].effectAmt;
}

int CandlesGiven(int type)
{
	if(type<0 || type>=NumItems())
		return 0;
	if(items[type].effect!=IE_CANDLE)
		return 0;
	return items[type].effectAmt;
}

void RepairItemToItem(int n)	// when item N is deleted, repair references to it and others in
								// all items
{
	int i;

	for(i=0;i<NumItems();i++)
	{
		if(items[i].effect==IE_BECOME)
		{
			if(items[i].effectAmt==n)
				items[i].effect=IE_NONE;	// can't become the item, so don't become anything
			else if(items[i].effectAmt>n)
				items[i].effectAmt--;	// must shift down the reference
		}
	}
}

void RepairItemToSound(int n)	// when sound N is deleted, repair references to it and others in
								// all items
{
	int i;

	for(i=0;i<NumItems();i++)
	{
		if(items[i].sound==n)
			items[i].sound=0;
		else if(items[i].sound>n)
			items[i].sound--;	// must shift down the reference
	}
}

void RepairItemToTile(const SwapTable &table)	// when tiles are messed with, repair the references in items
{
	int i;

	for(i=0;i<NumItems();i++)
	{
		if(items[i].appearance == ITA_TILEIMG)
		{
			items[i].sprNum=table.GetSwap(items[i].sprNum);
		}
	}
}

//--------------------------------------------------------------------

void ItemGetEffect(int x,int y)
{
	// sparkles and whatnot
	ColorRing(8,(x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
				(y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,FIXAMT*5,16,4);
}

byte TriggerItem(Guy *me,mapTile_t *m,int x,int y)
{
	int i;

	switch(items[m->item].effect)
	{
		case IE_NONE:
			return 1;
			break;
		case IE_DESTROY:
			ExplodeParticlesColor(items[m->item].effectAmt,(x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
				(y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,FIXAMT*5,8,8);
			m->item=IT_NONE;
			return 1;
		case IE_HEAL:
			if(items[m->item].effectAmt>=0)
			{
				if(player.life==goodguy->maxHP)
					return 0;
				PlayerHeal((byte)items[m->item].effectAmt);
			}
			else
				goodguy->GetShot(0,0,-items[m->item].effectAmt,curMap,&curWorld);
			return 1;
		case IE_WEAPON:
			return PlayerGetWeapon(items[m->item].effectAmt,x,y);
			break;
		case IE_HAMMER:
			return PlayerGetHammer(items[m->item].effectAmt);
		case IE_PANTS:
			return PlayerGetPants(items[m->item].effectAmt);
		case IE_KEY: // single-use keys
			if(player.keys[0]+items[m->item].effectAmt>3)
			{
				if(player.keys[0]==3)
					return 0;	// leave it on the ground
				else
				{
					if(!editing && !player.cheated && verified)
					{
						profile.progress.keysFound+=3-player.keys[0];
						if(profile.progress.keysFound>=100)
							CompleteGoal(73);
					}
					player.keys[0]=3;
					return 1;	// go ahead and pick it up if it's a 'multi-key' that would
								// push you over the limit
				}
			}
			else if(player.keys[0]+items[m->item].effectAmt<0)
			{
				if(player.keys[0]==0)
					return 0;	// leave it on the ground
				else
				{
					player.keys[0]=0;
					return 1;	// pick up multi-key-removers that push you under the limit
				}
			}
			else
			{
				if(!editing && !player.cheated && verified)
				{
					profile.progress.keysFound+=items[m->item].effectAmt;
					if(profile.progress.keysFound>=100)
							CompleteGoal(73);
				}
				player.keys[0]+=items[m->item].effectAmt;
			}
			return 1;
			break;
		case IE_RKEY: // multi-use keys
			return PlayerGetKey(1, items[m->item].effectAmt);
		case IE_GKEY:
			return PlayerGetKey(2, items[m->item].effectAmt);
		case IE_BKEY:
			return PlayerGetKey(3, items[m->item].effectAmt);
		case IE_LOONYKEY:
			if(items[m->item].effectAmt>0)
			{
				if(!editing && (player.worldProg->keychains&KC_LOONY)==0 && verified)
				{
					profile.progress.loonyKeys++;
					if(profile.progress.loonyKeys>=10)
						CompleteGoal(72);
				}
				player.worldProg->keychains|=KC_LOONY;
			}
			else // why would you do this???
			{
				if(!editing && (player.worldProg->keychains&KC_LOONY) && verified)
				{
					profile.progress.loonyKeys--;
					if(profile.progress.loonyKeysUsed>profile.progress.loonyKeys)
						profile.progress.loonyKeysUsed=profile.progress.loonyKeys;
				}
				player.worldProg->keychains&=(~KC_LOONY);
			}
			return 1;
		case IE_COIN:
			return PlayerGetCoin(items[m->item].effectAmt);
		case IE_CANDLE:
			return PlayerGetCandle(items[m->item].effectAmt);
		case IE_OXYGEN:
			return PlayerGetOxygen(items[m->item].effectAmt*256);
		case IE_RAGE:
			return PlayerGetRage(items[m->item].effectAmt*256);
		case IE_WEIRD:
			PlayerRadioactiveFood();
			return 1;
		case IE_BECOME:
			m->item=items[m->item].effectAmt;
			return 1;
		case IE_SUMMON:
			AddGuy((x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,10*FIXAMT,items[m->item].effectAmt,2);
			m->item=IT_NONE;
			SmokeTile(x,y);
			return 1;
		case IE_POWERUP:
			return PlayerPowerup(items[m->item].effectAmt);
		case IE_HEALRANDOM:
			if(player.life==goodguy->maxHP)
				return 0;
			PlayerChineseFood();
			return 1;
		case IE_BRAIN:
			return PlayerGetBrain(items[m->item].effectAmt);
		case IE_KEYCHAIN:
			return PlayerGetKeychain(items[m->item].effectAmt);
		case IE_DOOR:
			if(!me || me->aiType!=MONS_BOUAPHA)
				return 0;	// won't go away for non-Bouaphas
			else if(player.keys[items[m->item].effectAmt])
			{
				//DoorOpenEffect();
				if(items[m->item].effectAmt==0)	// yellow keys get used up
					player.keys[items[m->item].effectAmt]--;
				m->item=IT_NONE;
				ScoreEvent(SE_DOOR,1);
				if(!editing && !player.cheated && verified)
				{
					profile.progress.doorsOpened++;
					if(profile.progress.doorsOpened>=100)
						CompleteGoal(74);
				}
				return 1;
			}
			else return 0;
		case IE_ORBITER:
				switch(items[m->item].effectAmt)
				{
					case 0:
						FireBullet(goodguy->x, goodguy->y, Random(8), BLT_ORBITER, goodguy->friendly);
						break;
					case 1:
						FireBullet(goodguy->x, goodguy->y, Random(8), BLT_ORBITER2, goodguy->friendly);
						break;
					case 2:
						FireBullet(goodguy->x, goodguy->y, Random(8), BLT_ORBITER3, goodguy->friendly);
						break;
				}
			return 1;
		case IE_ORBITER2: // merged into IE_ORBITER
			return 1;
		case IE_PUSH:
			if(!me)
			{
				i=0;
				if(items[m->item].triggerType == ITRG_PROJECTILE)
				{
					int bdx,bdy;

					GetBulletDeltas(&bdx,&bdy);
					i=0;

					if(abs(bdx)>abs(bdy))
					{
						if(bdx>0)
							i=TryToPushItem(x,y,x+1,y,curMap,&curWorld);
						else
							i=TryToPushItem(x,y,x-1,y,curMap,&curWorld);
					}
					else if(bdx!=0 && bdy!=0)	// don't do anything for non-moving bullets
					{
						if(bdy>0)
							i=TryToPushItem(x,y,x,y+1,curMap,&curWorld);
						else
							i=TryToPushItem(x,y,x,y-1,curMap,&curWorld);
					}
				}
				return i;
			}
			else
			{
				if(me->mapx!=x && me->mapy!=y)
					return 0;	// you can't be diagonal from it
				if(me->dx==0 && me->dy==0)
					return 0;	// doesn't work if you're non-moving
				if(me->mapx==x && me->mapy==y)
				{
					// in the case of pushable non-obstacles, this could be an issue
					if(abs(me->dx)>abs(me->dy))
					{
						if(me->dx<0)
							i=2;
						else
							i=0;
					}
					else
					{
						if(me->dy<0)
							i=3;
						else
							i=1;
					}
				}
				else
				{
					if(me->mapx<x)
						i=0;
					else if(me->mapy<y)
						i=1;
					else if(me->mapx>x)
						i=2;
					else
						i=3;
				}
				switch(i)
				{
					case 0:
						return TryToPushItem(x,y,x+1,y,curMap,&curWorld);
						break;
					case 1:
						return TryToPushItem(x,y,x,y+1,curMap,&curWorld);
						break;
					case 2:
						return TryToPushItem(x,y,x-1,y,curMap,&curWorld);
						break;
					case 3:
						return TryToPushItem(x,y,x,y-1,curMap,&curWorld);
						break;
				}
			}
			return 1;
		case IE_INCVAR:
			SetVar(items[m->item].effectAmt,GetVar(items[m->item].effectAmt)+1);
			return 1;
		case IE_DECVAR:
			SetVar(items[m->item].effectAmt,GetVar(items[m->item].effectAmt)-1);
			return 1;
		case IE_MOVE:
			curMap->GetTile(x, y)->opaque = 1;
			return 0;
		case IE_POCKET:
			return PlayerAddPockets(items[m->item].effectAmt);
	}
	return 0;
}

void MoveMovableItem(int x,int y,Map *map,world_t *world)
{
	byte yes;
	byte type;

	type=map->GetTile(x, y)->item;
	switch(items[type].effectAmt)
	{
		case 0:
			yes=TryToPushItem(x,y,x+1,y,curMap,&curWorld);
			break;
		case 1:
			yes=TryToPushItem(x,y,x,y+1,curMap,&curWorld);
			break;
		case 2:
			yes=TryToPushItem(x,y,x-1,y,curMap,&curWorld);
			break;
		case 3:
			yes=TryToPushItem(x,y,x,y-1,curMap,&curWorld);
			break;
		default:
			yes=0;
			break;
	}
	if(yes)
	{
		MakeSound(items[type].sound,(x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,SND_CUTOFF,1000);
		if(items[type].msg[0])
			NewMessage(items[type].msg,75,0);
	}
}

byte IsCustomItem(word type)
{
	return type >= CUSTOM_ID_START;
}

byte GetFlyingSolidCheck(word type, Guy* me, byte* result)
{
	switch (items[type].passability)
	{
		case ITP_SOLID:
		case ITP_BULLETPROOF:
			return (MonsterFlags(me->type, me->aiType) & MF_FLYING);
		case ITP_BARRIER:
			return !(MonsterFlags(me->type, me->aiType) & MF_FLYING);
		default:
			return 1;
	}
}

byte GetTeamBumpCheck(word type, Guy* me)
{
	switch (items[type].triggerType)
	{
		case ITRG_PLAYERBUMP:
			return (me->aiType == (short)EntityType::Player);
		case ITRG_FRIENDBUMP:
			return (goodguy && me->friendly == goodguy->friendly);
		case ITRG_ENEMYBUMP:
			return (goodguy && me->friendly != goodguy->friendly);
		case ITRG_ENTITYBUMP:
			return 1;
		default:
			return 0;
	}
}

byte GetVehicleCheck(word type, Guy* me)
{
	switch (items[type].triggerType)
	{
		case ITRG_MINECART:
			switch (me->aiType)
			{
				case (short)EntityType::Minecart:
				case (short)EntityType::YouGo:
				case (short)EntityType::Autozoid:
				case (short)EntityType::CarTraffic:
					return 1;
				default:
					return 0;
			}
			break;
		default:
			return 0;
	}
}

byte GetSolidCheck(word type)
{
	switch (items[type].passability)
	{
		case ITP_SOLID:
		case ITP_BULLETPROOF:
			return 1;
		default:
			return 0;
	}
}

void DoFlyingSolidCheck(word type, Guy* me, byte* result)
{
	if (!result)
		return;

	*result = GetFlyingSolidCheck(type, me, result);

	if (MonsterFlags(me->type, me->aiType) & MF_WALLWALK)
		*result = 1; // no item can contain a ghost!
}

byte InteractWithItem(Guy *me,mapTile_t *m,int x,int y)
{
	byte result;
	word type;

	type=m->item;

	if(shopping && IsCustomItem(type) && me->aiType==MONS_BOUAPHA)
	{
		coro::launch(std::bind(InitShopping, x, y));	// bumped a shoppable item in the mall
		printf("Bumped into mall item\n");
		return 0;
	}

	result=1;

	DoFlyingSolidCheck(type,me,&result); // check if the entity is flying

	// pickup-able item
	if(me->aiType==(short)EntityType::Player && (items[type].passability == ITP_PICKUP) && me->hp>0)
	{
		result=1;
		if(items[type].triggerType == ITRG_PICKUP)
			result=TriggerItem(me,m,x,y);	// returns 0 if you can't pick it up, 2 if sound should be overriden

		if(result>0)
		{
			if(result==1) // brains/candles override sound if you've got all brains/candles
				MakeSound(items[type].sound,(x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,SND_CUTOFF,1000);
			if(items[type].msg[0])
				NewMessage(items[type].msg,75,0);

			ItemGetEffect(x,y);
			m->item=IT_NONE;
			EventOccur(EVT_GET,type,x,y,me);
		}
		return 1;
	}

	if(GetTeamBumpCheck(type,me))
	{
		if(TriggerItem(me,m,x,y))
		{
			MakeSound(items[type].sound,(x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,SND_CUTOFF,1000);
			if(items[type].msg[0])
				NewMessage(items[type].msg,75,0);
		}
	}

	if(GetVehicleCheck(type,me))
	{
		if(TriggerItem(me,m,x,y))
		{
			MakeSound(items[type].sound,(x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,SND_CUTOFF,1000);
			if(items[type].msg[0])
				NewMessage(items[type].msg,75,0);
		}
		if(GetSolidCheck(m->item)) // todo: if doesn't work, check supreme
			result=1;	// special case: if the minecart hit smashed it away, then count it passable immediately
	}

	return result;
}

byte BulletHitItem(bullet_t *me,mapTile_t *m,int x,int y)
{
	byte result;
	byte type;

	type=m->item;
	result=1;

	// bulletproof, no getting past it
	if(items[type].passability == ITP_BULLETPROOF || items[type].passability == ITP_BARRIER)
		result=0;

	switch (items[type].triggerType)
	{
		case ITRG_PROJECTILE: // any projectile
			if (TriggerItem(NULL, m, x, y))
			{
				MakeSound(items[type].sound, (x * TILE_WIDTH + TILE_WIDTH / 2) * FIXAMT, (y * TILE_HEIGHT + TILE_HEIGHT / 2) * FIXAMT, SND_CUTOFF, 1000);
				if (items[type].msg[0])
					NewMessage(items[type].msg, 75, 0);
			}
			break;
		case ITRG_MACHETE: // any projectile
			if (me->type == BLT_SLASH && TriggerItem(NULL, m, x, y))
			{
				if (!editing && !player.cheated && verified)
					profile.progress.grassChopped++;
				if (profile.progress.grassChopped >= 100)
					CompleteGoal(89);

				MakeSound(items[type].sound, (x * TILE_WIDTH + TILE_WIDTH / 2) * FIXAMT, (y * TILE_HEIGHT + TILE_HEIGHT / 2) * FIXAMT, SND_CUTOFF, 1000);
				if (items[type].msg[0])
					NewMessage(items[type].msg, 75, 0);
			}
			break;
	}
	return result;
}

void UpdateItem(mapTile_t *m,int width,int offset)
{
	int x,y;

	if(items[m->item].triggerType == ITRG_ALWAYS)
	{
		x=offset%width;
		y=offset/width;
		if(TriggerItem(NULL,m,x,y))
		{
			MakeSound(items[m->item].sound,(x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,SND_CUTOFF,1000);
			if(items[m->item].msg[0])
				NewMessage(items[m->item].msg,75,0);
		}
	}

	switch (items[m->item].behavior)
	{
		case ITB_BUBBLES:
			x = offset % width;
			y = offset / width;
			BlowBubble((x * TILE_WIDTH + TILE_WIDTH / 2) * FIXAMT, (y * TILE_HEIGHT + TILE_HEIGHT / 2) * FIXAMT, 0, FIXAMT / 2);
			break;
		case ITB_FIRE:
			x = offset % width;
			y = offset / width;
			Burn((x * TILE_WIDTH + TILE_WIDTH / 2) * FIXAMT, (y * TILE_HEIGHT + TILE_HEIGHT / 2) * FIXAMT, FIXAMT*12);
			break;
	}
}

int FindItemByName(const char *name)
{
	int i;

	for(i=0;i<NumItems();i++)
	{
		if(!strcmp(name,items[i].name))
			return i;
	}
	return 0;
}

static const char pwrUpName[][32] = {
	"",
	"Reverse Hammer",
	"Reflect Hammer",
	"Energy Shield",
	"Garlic",
	"Speed Up",
	"Invisibility",
	"Infinite Ammo",
	"Reload Weapon",
	"Hammer Enhance",
	"Poison",
	"Ignite",
	"Freeze",
	"Weakness",
	"Slowness",
	"Strength",
	"Water Walk"
};
static_assert(std::size(pwrUpName) == MAX_POWERUP, "Must give new powerups a name");

const char *GetPowerupName(int powerup)
{
	if (powerup >= 0 && powerup < MAX_POWERUP)
		return pwrUpName[powerup];
	return "???";
}

static ItemRenderExtents extents;

void CalculateItemRenderExtents()
{
	// Calculate the union bounding box of item sprites so we know how far away
	// to search for items to draw, with no pop-in and minimal overdraw.
	SDL_Rect everything = {};
	for (const item_t &item : items)
	{
		SDL_Rect rect;
		if (item.appearance == ITA_TILEIMG)
		{
			rect.x = item.xofs;
			rect.y = item.yofs+1;
			rect.w = 32;
			rect.h = 32;
		}
		else
		{
			sprite_t* spr;

			if (item.customJSP)
			{
				sprite_set_t* custom = CustomItemSprites();
				if (custom)
					spr = custom->GetSprite(item.sprNum < custom->GetCount() ? item.sprNum : 0);
				else
					spr = itmSpr->GetSprite(8); // red X indicating invalid custom JSP file
			}
			else
				spr = itmSpr->GetSprite(item.sprNum);

			if (spr)
			{
				// Use precise sprite boundaries.
				rect.x = item.xofs - spr->ofsx;
				rect.y = 0 - spr->ofsy - std::clamp(-item.yofs+1, -DISPLAY_YBORDER, DISPLAY_YBORDER);
				rect.w = spr->width;
				rect.h = spr->height;

				SDL_GetRectUnion(&everything, &rect, &everything);

				switch (item.appearance)
				{
					case ITA_SHADOW:
					case ITA_LOONYKEY:
						// Use precise shadow sprite boundaries.
						rect.x = item.xofs - spr->ofsx - spr->height / 2;
						rect.y = 0 - spr->ofsy / 2 - std::clamp(-item.yofs + 1, -DISPLAY_YBORDER, DISPLAY_YBORDER);
						rect.w = spr->height / 2 + spr->width;
						rect.h = spr->height / 2;

						SDL_GetRectUnion(&everything, &rect, &everything);
						break;
				}
			}
		}
	}

	// Bounds are reversed here, because the further left from an item's origin
	// it extends, the further right off the edge of the screen we need to seek
	// items to draw. The minimums are for tile/wall rendering.
	extents.left = floor_div(everything.x + everything.w + TILE_WIDTH/2, TILE_WIDTH).quot;
	extents.right = -floor_div(everything.x + TILE_WIDTH/2, TILE_WIDTH).quot;
	extents.up = floor_div(everything.y + everything.h + TILE_HEIGHT/2, TILE_HEIGHT).quot;
	extents.down = -floor_div(everything.y + TILE_HEIGHT/2, TILE_HEIGHT).quot;

#ifndef NDEBUG
	LogDebug("item bounds: (x: %d, y: %d, w: %d, h: %d) tile spread: (x: %+d to %+d, y: %+d to %+d), inclusive",
		everything.x, everything.y, everything.w, everything.h,
		-extents.left, extents.right, -extents.up, extents.down);
#endif
}

ItemRenderExtents GetItemRenderExtents()
{
	return extents;
}
