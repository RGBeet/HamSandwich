#include "monster.h"
#include "monsterai.h"

#define MT_WEIRD MT_BITS

#define NULL_ANIM	{0,255}
MonsterType monsType[NUM_MONSTERS]=
	{
		{"Null",255,255,0,NULL,
		 0,0,0,0,"",0,{},MT_NONE,{},0},
		{"Bouapha",255,255,0,NULL,
		 11,29,128,0,"graphics/bouapha.jsp",0,MF_WATERWALK,
		 MT_GOOD | MT_HUMAN,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,14,15,16,255},	// attack
				{0,0,0,0,0,0,0,26,27,28,255},		// die
				{23,24,25,24,23,255},	// use item
				{17,18,19,20,21,22,21,20,21,22,21,20,19,18,17,255},	// bored
				{1,3,1,0,4,6,4,0,1,3,1,0,0,0,26,27,28,255},	// watery death
				{7,8,9,10,11,12,13,14,15,16,255},	// attack
				NULL_ANIM,
			},0},
		{"Dummy",255,255,0,AI_Mumble,
		 15,28,30,100,"graphics/mumble.jsp",0,MF_NOMOVE,
		 MT_EVIL | MT_TRAP,
			{
				{0,255},	// idle
				{1,2,3,4,5,0,6,7,8,9,10,0,255},	// move
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				{19,20,21,22,23,24,25,26,27,255},	// die
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Bonehead",255,255,0,AI_Bonehead,
		 8,38,10,50,"graphics/skeleton.jsp",0,{},
		 MT_SPOOKY | MT_EVIL,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{20,21,22,23,24,255},	// attack
				{7,8,9,10,11,12,13,14,15,16,255},		// die
				{25,26,27,28,29,30,255},	// chomp/spit projectile
				{31,32,32,32,31,255},	// point at bouapha
				{16,15,14,13,12,11,10,9,8,7,255},	// resurrect=A3
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Mumble",255,255,0,AI_Mumble,
		 15,28,30,100,"graphics/mumble.jsp",0,MF_NOMOVE,
		 MT_EVIL | MT_DESERT | MT_SPOOKY,
			{
				{0,255},	// idle
				{1,2,3,4,5,0,6,7,8,9,10,0,255},	// move
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				{19,20,21,22,23,24,25,26,27,255},	// die
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Zombie",255,255,0,AI_Zombie,
		 11,32,20,150,"graphics/zombie.jsp",0,{},
		 MT_SPOOKY | MT_ZOMBIE | MT_EVIL,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,9,10,11,12,13,13,14,15,16,17,18,19,255},	// attack
				{20,21,22,23,24,25,26,27,28,29,30,31,255},	// die
				{7,8,9,9,10,11,12,13,13,14,15,16,17,18,19,255},	// attack
				{7,8,9,9,10,11,12,13,13,14,15,16,17,18,19,255},	// attack
				{7,8,9,9,10,11,12,13,13,14,15,16,17,18,19,255},	// attack
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Lantern",255,255,0,AI_Lantern, // not yet implemented
		 8,24,20,50,"graphics/lantern.jsp",0,MF_FLYING | MF_NOMOVE,
		 MT_EVIL | MT_FLYING | MT_SPOOKY,
			{
				{0,255},	// idle
				{0,255},	// move??
				{1,2,3,4,5,6,7,8,7,6,5,4,3,2,9,10,11,12,11,10,9,255},	// attack 1
				{13,14,15,16,17,18,19,20,21,22,23,255},	// defeated
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Hard Hat",255,255,0,AI_Hardhat, // not yet implemented
		 8,38,10,50,"graphics/hardhat.jsp",0,{},
		 MT_EVIL | MT_HUMAN,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack 1
				{0,255},	// defeated
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Pelican",255,255,0,AI_Pelican, // not yet implemented
		 8,38,10,50,"graphics/pelican.jsp",0,MF_FLYING,
		 MT_EVIL | MT_ANIMAL | MT_FLYING,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack 1
				{0,255},	// defeated
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Oui Ninja",255,255,0,AI_Ninja, // not yet implemented
		 8,38,10,50,"graphics/ninja.jsp",0,{},
		 MT_EVIL | MT_HUMAN,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack 1
				{0,255},	// defeated
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Kaptana",255,255,0,AI_Kappa, // not yet implemented
		 8,38,10,50,"graphics/kappa.jsp",0,{},
		 MT_EVIL | MT_ANIMAL,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack 1
				{0,255},	// defeated
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Aquazoid",255,255,0,AI_Serpent,
		 15,20,15,75,"graphics/serpent.jsp",0,MF_AQUATIC | MF_WATERWALK,
		 MT_AQUATIC | MT_EVIL | MT_ZOID,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,255},	// move
				{4,5,6,7,8,9,10,11,12,255},	// spit
				{13,14,15,16,17,18,19,255},	// die
				{4,5,6,7,8,9,10,11,12,255},	// spit
				{4,5,6,7,8,9,10,11,12,255},	// spit
				{4,5,6,7,8,9,10,11,12,255},	// spit
				{0,255},
				{0,255},
			},0},
		{"Tsuchizoid",255,255,0,AI_Tsuchizoid, // not yet implemented
		 8,38,10,50,"graphics/tsuchizoid.jsp",0,{},
		 MT_EVIL | MT_ANIMAL | MT_ZOID,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack 1
				{0,255},	// defeated
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0 },
		{"Buddy Bunny",2,0,3,AI_BuddyBunny,
		 10,8,30,1000,"graphics/buddybunny.jsp",0,{},
		 MT_GOOD | MT_ANIMAL,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,0,255},	// move
				{0,255},	// attack
				{0,0,0,0,255},	// die
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Buddy Bunny",2,0,3,AI_BuddyBunny,
		 10,8,30,1000,"graphics/buddybunny.jsp",0,{},
		 MT_GOOD | MT_ANIMAL,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,0,255},	// move
				{0,255},	// attack
				{0,0,0,0,255},	// die
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Buddy Bunny",2,0,3,AI_BuddyBunny,
		 10,8,30,1000,"graphics/buddybunny.jsp",0,{},
		 MT_GOOD | MT_ANIMAL,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,0,255},	// move
				{0,255},	// attack
				{0,0,0,0,255},	// die
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Jimbo",255,255,0,AI_Jimbo, // not yet implemented
		 8,38,10,50,"graphics/jimbo.jsp",0,{},
		 MT_GOOD | MT_ANIMAL,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack 1
				{0,255},	// defeated
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Ninjabread Man",255,255,0,AI_Ginger,
		 12,33,50,300,"graphics/ginger.jsp",0,{},
		 MT_EVIL | MT_WACKY,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,255},	// move
				{7,8,9,10,11,12,13,14,15,16,17,18,19,20,19,20,19,20,21,22,23,255},	// triple punch
				{27,28,29,30,31,32,32,32,32,255},	// die
				{24,25,26,26,26,26,26,255},	// jumpkick (A1)
				{25,24,255},	// unjumpkick (A2)
				{7,8,9,10,11,12,13,14,15,16,17,18,19,20,19,20,19,20,21,22,23,255},	// triple punch
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Pumpkin",255,255,0,AI_Pumpkin,
		 11,6,5,500,"graphics/pumpkin.jsp",0,{},
		 MT_EVIL | MT_PUMPKIN | MT_VEGGIE,
			{
				{0,255},	// idle
				{1,2,3,4,5,0,255},	// move
				{1,2,3,4,5,0,255},	// move
				{0,0,255},	// die (just long enough to spew shrapnel)
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Kabocha",255,255,0,AI_Kabocha, // not yet implemented
		 8,38,10,50,"graphics/kabocha.jsp",0,{},
		 MT_EVIL | MT_PUMPKIN | MT_VEGGIE,
			{
				{0,255},	// idle
				{1,2,3,4,5,0,255},	// move
				{1,2,3,4,5,0,255},	// move
				{0,0,255},	// die (just long enough to spew shrapnel)
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Crazy Bush",255,255,0,AI_CrazyBush, // not yet implemented
		 10,8,20,5,"graphics/crazybush.jsp",0,{},
		 MT_EVIL | MT_VEGGIE,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{0,255},	// attack
				{5,6,7,255},	// die
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Umbrello",255,255,0,AI_Umbrello, // not yet implemented
		 8,38,10,50,"graphics/umbrello.jsp",0,{},
		 MT_EVIL | MT_SPOOKY | MT_WACKY,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack 1
				{0,255},	// defeated
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Eyeball Tree",255,255,0,AI_EyeballTree, // not yet implemented
		 8,38,10,50,"graphics/eyeballtree.jsp",0,MF_ONEFACE | MF_NOMOVE | MF_SPRITEBOX,
		 MT_EVIL | MT_VEGGIE | MT_WACKY,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack 1
				{0,255},	// defeated
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Gingersnap",2,4,5,AI_Gingersnap,
		 12,33,50,300,"graphics/ginger.jsp",0,{},
		 MT_EVIL | MT_WACKY,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,255},	// move
				{7,8,9,10,11,12,13,14,15,16,17,18,19,20,19,20,19,20,21,22,23,255},	// triple punch
				{27,28,29,30,31,32,32,32,32,255},	// die
				{24,25,26,26,26,26,26,255},	// jumpkick (A1)
				{25,24,255},	// unjumpkick (A2)
				{7,8,9,10,11,12,13,14,15,16,17,18,19,20,19,20,19,20,21,22,23,255},	// triple punch
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Happy Stick Ninja",255,255,0,AI_StickNinja, // not yet implemented
		 8,38,10,50,"graphics/stickninja.jsp",0,MF_ONEFACE,
		 MT_EVIL | MT_WACKY | MT_MINIBOSS | MT_HUMAN,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack 1
				{0,255},	// defeated
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Noh-Mann",255,255,0,AI_NohMannComedy, // not yet implemented
		 8,38,10,50,"graphics/nohmann1.jsp",0,MF_NOMOVE | MF_ONEFACE | MF_FACECMD | MF_NOGRAV | MF_ENEMYWALK | MF_FREEWALK,
		 MT_EVIL | MT_BOSS | MT_SPOOKY,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack 1
				{0,255},	// defeated
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Noh-Mann",255,255,0,AI_NohMannComedy, // not yet implemented
		 8,38,10,50,"graphics/nohmann1.jsp",0,MF_NOMOVE | MF_ONEFACE | MF_FACECMD | MF_NOGRAV | MF_ENEMYWALK | MF_FREEWALK,
		 MT_BITS,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack 1
				{0,255},	// defeated
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Noh-Mann's Arm",255,255,0,AI_NohMannArm, // not yet implemented
		 8,38,10,50,"graphics/noharm1.jsp",0,MF_NOMOVE | MF_ONEFACE | MF_SPRITEBOX | MF_NOGRAV | MF_ENEMYWALK | MF_FREEWALK,
		 MT_BITS,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack 1
				{0,255},	// defeated
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0 },
		{"Noh-Mann's Arm",255,255,0,AI_NohMannArm, // not yet implemented
		 8,38,10,50,"graphics/noharm2.jsp",0,MF_NOMOVE | MF_ONEFACE | MF_SPRITEBOX | MF_NOGRAV | MF_ENEMYWALK | MF_FREEWALK,
		 MT_BITS,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack 1
				{0,255},	// defeated
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Yerfdog",0,0,0,AI_Yerfdog,
		 10,1,30,1000,"graphics/yerf3d.jsp",0,MF_NOHIT | MF_FREEWALK | MF_ENEMYWALK | MF_WATERWALK,
		 MT_GOOD | MT_ANIMAL,
			{
				{0,255},	// idle
				{0,255},	// nope
				{0,255},	// nope
				{0,255},	// nope
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Yerfdog",255,255,0,AI_Yerfboss, // not yet implemented
		 8,38,10,50,"graphics/yerfboss.jsp",0,MF_ONEFACE | MF_NOMOVE,
		 MT_EVIL | MT_WACKY | MT_ANIMAL | MT_BOSS,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack 1
				{0,255},	// defeated
				{0,255},
				{0,255},
				{0,255},
				{0,255},
				{0,255},
			},0},
		{"Nobody",255,255,0,AI_Nobody,
		 1,1,1,1,"!1",0,MF_INVINCIBLE | MF_NOHIT | MF_FREEWALK | MF_ENEMYWALK | MF_WALLWALK,
		 MT_NONE,
			{
				{254,255},	// ???
				{254,255},	// ???
				{254,255},	// ???
				{254,255},	// ???
				{254,255},	// ???
				{254,255},	// ???
				{254,255},	// ???
				{254,255},	// ???
				{254,255},	// ???
			},0},
		{"Eensy Weensy",255,255,0,AI_Spider,
		 4,22,1,10,"graphics/spider.jsp",0,MF_ENEMYWALK,
		 MT_EVIL | MT_SPIDER | MT_ANIMAL,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,8,9,10,11,0,255},	// move
				{12,13,14,13,12,255},	// attack
				{15,16,17,18,19,18,19,18,19,20,20,21,21,255},	// die
				{12,13,14,13,12,255},	// attack
				{12,13,14,13,12,255},	// attack
				{12,13,14,13,12,255},	// attack
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Spitter",255,255,0,AI_BigSpider,
		 8,21,10,50,"graphics/bigspdr.jsp",0,{},
		 MT_EVIL | MT_SPIDER | MT_ANIMAL,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,12,13,13,255},	// attack
				{14,15,16,17,18,17,18,17,18,19,19,20,20,255},	// die
				{7,8,9,10,11,12,12,13,13,255},	// attack
				{7,8,9,10,11,12,12,13,13,255},	// attack
				{7,8,9,10,11,12,12,13,13,255},	// attack
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Egg Sac",255,255,0,AI_EggSac,
		 20,19,150,1000,"graphics/eggsac.jsp",0,MF_ONEFACE | MF_ENEMYWALK | MF_NOMOVE,
		 MT_EVIL | MT_GENERATE | MT_SPIDER,
			{
				{0,0,0,0,0,0,0,0,1,2,1,0,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,0,255},	// attack
				{19,20,21,22,23,24,25,255},	// die
				{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,0,255},	// attack
				{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,0,255},	// attack
				{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,0,255},	// attack
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Mama Spider",255,255,0,AI_MamaSpider,
		 30,30,200,1500,"graphics/mamaspdr.jsp",0,MF_NOMOVE,
		 MT_EVIL | MT_MINIBOSS | MT_SPIDER | MT_ANIMAL,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,12,13,13,255},	// spit (attack)
				{19,20,21,22,23,24,25,25,25,26,27,28,29,255},	// die
				{14,15,16,17,18,255},	// bite (A1)
				{7,8,9,10,11,12,12,13,13,255},	// spit (attack)
				{7,8,9,10,11,12,12,13,13,255},	// spit (attack)
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Shroom",255,255,0,AI_Shroom,
		 15,23,10,50,"graphics/shroom.jsp",0,{},
		 MT_EVIL | MT_VEGGIE,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,14,255},	// attack
				{15,16,17,18,19,20,21,22,255},	// die
				{7,8,9,10,11,12,13,14,255},	// a1
				{7,8,9,10,11,12,13,14,255},	// attack
				{7,8,9,10,11,12,13,14,255},	// attack
				NULL_ANIM,
				NULL_ANIM,
			},0 },
		{"Shroomlord",255,255,0,AI_ShroomLord,
		 15,14,40,300,"graphics/shrmlord.jsp",0,{},
		 MT_EVIL | MT_VEGGIE,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,7,8,8,8,9,10,11,12,12,13,13,255},	// spit spores
				{0,0,255},	// die
				{7,7,8,8,8,9,10,11,12,12,13,13,255},	// spit spores
				{7,7,8,8,8,9,10,11,12,12,13,13,255},	// spit spores
				{7,7,8,8,8,9,10,11,12,12,13,13,255},	// spit spores
				{7,7,8,8,8,9,10,11,12,12,13,13,255},	// spit spores
				{7,7,8,8,8,9,10,11,12,12,13,13,255},	// spit spores
			},0},
		{"Mush the Shroom",255,255,0,AI_Mush,
		 32,27,200,800,"graphics/bgshroom.jsp",0,MF_NOMOVE,
		 MT_EVIL | MT_MINIBOSS | MT_VEGGIE,
			{
				{0,255},	// idle
				{1,2,2,1,0,3,4,4,3,0,255},	// move
				{5,6,6,7,8,9,10,255},	// attack=cannon fire
				{21,22,23,24,25,26,26,26,26,255},	// die
				{11,12,13,12,13,12,13,12,13,12,13,12,13,12,13,12,13,14,15,16,255},	// A1=sporegun fire
				{17,18,19,20,19,18,19,20,19,18,17,255},	// A2=angry
				{5,6,6,7,8,9,10,255},	// attack=cannon fire
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Crazybone",0,3,0,AI_CrazyBone,
		 8,38,10,100,"!2",0,{},
		 MT_EVIL | MT_SPOOKY,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{20,21,22,23,24,255},	// attack
				{7,8,9,10,11,12,13,14,15,16,255},		// die
				{25,26,27,28,29,30,255},	// chomp/spit projectile
				{31,32,32,32,31,255},	// point at bouapha
				{16,15,14,13,12,11,10,9,8,7,255},	// resurrect=A3
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Manic Mumble",0,7,4,AI_Mumble,
		 15,28,20,100,"!4",0,MF_NOMOVE,
		 MT_EVIL | MT_SPOOKY | MT_DESERT,
			{
				{0,255},	// idle
				{1,2,3,4,5,0,6,7,8,9,10,0,255},	// move
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				{19,20,21,22,23,24,25,26,27,255},	// die
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
			},0},
		{"Bombie",0,4,5,AI_Bombie, // the explosive zombie, leaps up
		 11,32,80,150,"!5",0,{},
		 MT_EVIL | MT_SPOOKY | MT_ZOMBIE | MT_TRAP,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,9,10,11,12,13,13,14,15,16,17,18,19,255},	// attack
				{19,255},	// die
				{19,18,18,19,0,7,8,9,10,11,11,12,12,13,13,14,14,15,255},	// leaps up and kills
				{7,8,9,9,10,11,12,13,13,14,15,16,17,18,19,255},	// attack
				{7,8,9,9,10,11,12,13,13,14,15,16,17,18,19,255},	// attack
				{0,255},
				{0,255},
			},0},
		{"Bob The Biscuit",255,255,-7,AI_Bob,
		 12,33,70,300,"!17",0,{},
		 MT_EVIL | MT_WACKY,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,255},	// move
				{7,8,9,10,11,12,13,14,15,16,17,18,19,20,19,20,19,20,21,22,23,255},	// triple punch
				{27,28,29,30,31,32,32,32,32,255},	// die
				{24,25,26,26,26,26,26,255},	// jumpkick (A1)
				{25,24,255},	// unjumpkick (A2)
				{23,22,21,20,19,20,19,20,21,22,23,255}, // summon help (A3)
				{7,8,9,10,11,12,13,14,15,16,17,18,19,20,19,20,19,20,21,22,23,255},	// triple punch
				{7,8,9,10,11,12,13,14,15,16,17,18,19,20,19,20,19,20,21,22,23,255},	// triple punch
			},0},
		{"Pumpkin Patch",255,255,2,AI_Patch,
		 16,17,150,1000,"graphics/patch.jsp",0,MF_NOMOVE | MF_ENEMYWALK | MF_ONEFACE | MF_NOSHADOW,
		 MT_EVIL | MT_VEGGIE | MT_PUMPKIN | MT_GENERATE,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,0,2,0,1,2,1,0,1,0,2,1,2,1,0,2,1,2,255},	// attack
				{11,12,13,14,15,16,255},	// die
				{3,4,5,6,7,8,9,10,255},	// A1=summon
				{3,4,5,6,7,8,9,10,255},	// A1=summon
				{3,4,5,6,7,8,9,10,255},	// A1=summon
				{3,4,5,6,7,8,9,10,255},	// A1=summon
				{3,4,5,6,7,8,9,10,255},	// A1=summon
			},0},
		{"Boomkin",2,4,5,AI_Pumpkin,
		 11,6,1,500,"!19",0,{},
		 MT_EVIL | MT_VEGGIE | MT_PUMPKIN,
			{
				{0,255},	// idle
				{1,2,3,4,5,0,255},	// move
				{0,255},	// attack unused
				{0,0,255},	// die (just long enough to spew shrapnel)
				{1,2,3,4,5,0,255},	// move
				{1,2,3,4,5,0,255},	// move
				{1,2,3,4,5,0,255},	// move
				{1,2,3,4,5,0,255},	// move
				{1,2,3,4,5,0,255},	// move
			},0},
		{"Squash",255,255,0,AI_Squash,
		 8,14,10,100,"graphics/squash.jsp",0,{},
		 MT_EVIL | MT_VEGGIE | MT_PUMPKIN,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,9,8,7,255},	// attack
				{11,12,13,255},	// die
				{7,8,9,10,9,8,7,255},	// attack
				{7,8,9,10,9,8,7,255},	// attack
				{7,8,9,10,9,8,7,255},	// attack
				{7,8,9,10,9,8,7,255},	// attack
				{7,8,9,10,9,8,7,255},	// attack
			},0},
		{"Jalapeno",1,4,4,AI_Jalapeno,
		 8,14,5,100,"!85",0,{},
		 MT_EVIL | MT_VEGGIE | MT_PUMPKIN,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,9,8,7,255},	// attack
				{11,12,13,255},	// die
				{7,8,9,10,9,8,7,255},	// attack
				{7,8,9,10,9,8,7,255},	// attack
				{7,8,9,10,9,8,7,255},	// attack
				{7,8,9,10,9,8,7,255},	// attack
				{7,8,9,10,9,8,7,255},	// attack
			},0},
		{"Mutant Hard Hat",255,255,0,AI_Hardhat, // not yet implemented
		 8,38,10,50,"graphics/hardhat2.jsp",0,{},
		 MT_EVIL | MT_HUMAN,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack 1
				{0,255},	// defeated
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Madkap",1,4,0,AI_Kappa, // not yet implemented
		 8,38,10,50,"!10",0,{},
		 MT_EVIL | MT_ANIMAL,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack 1
				{0,255},	// defeated
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Purple Kaptana",1,7,0,AI_Kappa, // not yet implemented
		 8,38,10,50,"!10",0,{},
		 MT_EVIL | MT_ANIMAL,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack 1
				{0,255},	// defeated
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Thorn Bush",1,4,-2,AI_CrazyBush, // not yet implemented
		 10,8,20,5,"!20",0,{},
		 MT_EVIL | MT_VEGGIE,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{0,255},	// attack
				{5,6,7,255},	// die
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
			},0},
		{"Hot N' Sour",1,4,0,AI_HotShroom,
		 15,23,10,50,"!36",0,{},
		 MT_EVIL | MT_VEGGIE,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,14,255},	// attack
				{15,16,17,18,19,20,21,22,255},	// die
				{7,8,9,10,11,12,13,14,255},	// attack
				{7,8,9,10,11,12,13,14,255},	// attack
				{7,8,9,10,11,12,13,14,255},	// attack
				{7,8,9,10,11,12,13,14,255},	// attack
				{7,8,9,10,11,12,13,14,255},	// attack
			},0},
		{"Deathcap",1,7,-8,AI_Deathcap,
		 15,23,30,50,"!36",0,{},
		 MT_EVIL | MT_VEGGIE | MT_TRAP,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,14,255},	// attack
				{15,16,17,18,19,20,21,22,255},	// die
				{7,8,9,10,11,12,13,14,255},	// a1
				{7,8,9,10,11,12,13,14,255},	// attack
				{7,8,9,10,11,12,13,14,255},	// attack
				{0,255},
				{0,255},
			},0},
		{"Treasure Chest",255,255,0,AI_Chest,
		 16,5,50,1000,"graphics/chest.jsp",0,MF_NOMOVE | MF_ONEFACE,
		 MT_TRAP,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack
				{1,2,3,4,255},	// die
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Sneaky Shark",255,255,0,AI_Sneaky2,
		 11,32,50,1000,"graphics/snkyshrk2.jsp",0,MF_NOMOVE | MF_ONEFACE,
		 MT_EVIL | MT_UNDERSEA | MT_TRAP,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,6,5,4,3,2,1,255},	// attack
				{0,0,0,255},	// die
				{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,6,5,4,3,2,1,255},	// attack
				{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,6,5,4,3,2,1,255},	// attack
				{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,6,5,4,3,2,1,255},	// attack
				{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,6,5,4,3,2,1,255},	// attack
				{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,6,5,4,3,2,1,255},	// attack
			},0},
		{"Turret",255,255,0,AI_Turret, // NEW GRAPHICS?!
		 10,8,80,200,"graphics/turret.jsp",0,MF_NOMOVE,
		 MT_EVIL | MT_TRAP | MT_HIGHTECH,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,1,2,3,4,5,6,7,255},	// attack
				{0,1,2,3,4,5,255},	// die
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0 },
		{"Missile Turret",4,3,4,AI_Turret,
		 10,8,80,200,"graphics/turret.jsp",0,MF_NOMOVE,
		 MT_EVIL | MT_TRAP | MT_HIGHTECH,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,1,2,3,4,5,6,7,255},	// attack
				{0,1,2,3,4,5,255},	// die
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
			},0},
		{"Death Turret",4,7,-4,AI_Turret,
		 10,8,80,200,"graphics/turret.jsp",0,MF_NOMOVE,
		 MT_EVIL | MT_TRAP | MT_HIGHTECH,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,1,2,3,4,5,6,7,255},	// attack
				{0,1,2,3,4,5,255},	// die
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Good Turret",4,1,5,AI_GoodTurret,
		 10,8,30,200,"graphics/turret.jsp",0,MF_NOMOVE,
		 MT_GOOD | MT_TRAP | MT_HIGHTECH,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,1,2,3,4,5,6,7,255},	// attack
				{0,1,2,3,4,5,255},	// die
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Evil Clone",1,3,0,AI_EvilClone,
		 11,29,128,0,"!1",0,{},
		 MT_EVIL | MT_HUMAN,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,14,15,16,255},	// attack
				{0,0,0,0,0,0,0,26,27,28,255},		// die
				{23,24,25,24,23,255},	// use item
				{17,18,19,20,21,22,21,20,21,22,21,20,19,18,17,255},	// bored
				{1,3,1,0,4,6,4,0,1,3,1,0,0,0,26,27,28,255},	// watery death
				{7,8,9,10,11,12,13,14,15,16,255},	// attack
				{7,8,9,10,11,12,13,14,15,16,255},	// attack
			},0},
		/////////
		{"Mine Cart",255,255,0,AI_MineCart,
		 11,8,100,500,"graphics/minecar.jsp",0,MF_NOMOVE | MF_INVINCIBLE | MF_FREEWALK | MF_NOHIT,
		 MT_VEHICLE | MT_GOOD,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,0,255},	// move
				{1,2,3,4,5,6,7,0,255},	// move
				{1,2,3,4,5,6,7,0,255},	// move
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Power Armor",255,255,0,NULL,	// in power armor
		 20,14,128,0,"graphics/pwrarmor.jsp",0,{},
		 MT_WEIRD,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,9,9,9,8,7,255},	// attack
				{10,11,12,13,255},		// die
				{7,7,7,255},		// fire missiles
				{13,12,11,10,255},	// activate
				{7,8,9,9,9,9,8,7,255},	// attack
				{7,8,9,9,9,9,8,7,255},	// attack
				{7,8,9,9,9,9,8,7,255},	// attack
			},0},
		{"Mini-Sub",255,255,0,NULL,	// in mini-sub
		 11,1,128,0,"graphics/minisub.jsp",0,MF_FLYING,
		 MT_WEIRD,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack
				{0,0,0,255}, // die
				{0,255},	// fire missiles
				{0,255},	// activate
				{0,0,0,255}, // die
				{0,0,0,255}, // die
				{0,0,0,255}, // die
			},0},
		{"You-Go",255,255,0,AI_Yugo,
		 14,16,5,1000,"graphics/yugo.jsp",0,MF_NOMOVE | MF_NOHIT | MF_INVINCIBLE | MF_FREEWALK | MF_WATERWALK,
		 MT_VEHICLE | MT_GOOD,
			{
				{0,255},	// idle
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
				{0,255},	// attack
				{0,0,0,255},	// die
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"LunaChick",255,255,0,NULL,	// playable char
		 11,27,250,1000,"graphics/lunachick.jsp",0,MF_WATERWALK,
		 MT_WEIRD,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,13,0,0,255},	// attack=fire gun
				{14,15,16,17,18,19,20,21,22,23,24,25,26,255},	// die
				{7,9,10,12,13,255},	// A1=use weapon
				{14,15,16,17,18,17,16,15,14,15,17,15,14,0,0,255},	// A2=bored
				{1,3,1,0,4,6,4,0,1,3,1,0,0,0,24,25,26,255},	// A3=watery death
				{7,8,9,10,11,12,13,13,0,0,255},	// attack=fire gun
				{7,8,9,10,11,12,13,13,0,0,255},	// attack=fire gun
			},0},
		{"Stupid Shroom",1,4,0,NULL,
		 11,14,40,300,"!192",0,MF_WATERWALK,
		 MT_WEIRD,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,7,8,8,9,10,11,12,12,13,255},	// attack
				{0,0,0,0,0,0,0,0,0,0,255},	// die
				{7,8,10,11,13,255},	// use item
				{7,8,9,9,9,9,9,9,8,7,0,0,0,0,0,255},	// bored
				{1,3,1,0,4,6,4,0,1,3,1,0,0,0,0,0,0,255},	// watery death
				{7,7,8,8,9,10,11,12,12,13,255},	// attack
				{7,7,8,8,9,10,11,12,12,13,255},	// attack
			},0},
		{"Creeping Cone",255,255,0,AI_Cone,
		 8,21,30,25,"graphics/cone.jsp",0,{},
		 MT_EVIL | MT_URBAN | MT_WACKY | MT_TRAP,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,0,255},	// move
				{8,9,10,9,8,255},	// attack=poke
				{17,18,19,20,255},	// die
				{11,12,13,14,14,15,16,15,14,13,12,11,255}, // A1=shoot
				{8,9,10,9,8,255},	// attack=poke
				{8,9,10,9,8,255},	// attack=poke
				{8,9,10,9,8,255},	// attack=poke
				{8,9,10,9,8,255},	// attack=poke
			},0},
		{"Autozoid",255,255,0,AI_Autozoid,
		 15,16,150,1000,"graphics/autozoid.jsp",0,{},
		 MT_EVIL | MT_ZOID | MT_URBAN | MT_VEHICLE,
			{
				{0,255},	// idle
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
				{0,255},	// attack
				{0,0,0,255},	// die
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Parked Car",4,3,0,AI_ParkedCar,
		 15,16,15,1000,"!152",0,MF_NOMOVE,
		 MT_VEHICLE | MT_TRAP | MT_URBAN,
			{
				{0,255},	// idle
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
				{0,255},	// attack
				{0,0,0,255},	// die
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Traffic",4,6,0,AI_Traffic,
		 11,16,200,1000,"!152",0,MF_NOMOVE | MF_WALLWALK,
		 MT_VEHICLE | MT_TRAP | MT_EVIL | MT_URBAN,
			{
				{0,255},	// idle
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
				{0,255},	// attack
				{0,0,0,255},	// die
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Mailboxer",255,255,0,AI_Mailboxer,
		 12,32,40,1000,"graphics/mboxer.jsp",0,{},
		 MT_EVIL | MT_URBAN,
			{
				{0,1,2,3,2,1,0,4,5,6,5,4,255},	// idle
				{21,22,21,0,23,24,23,0,255},	// move
				{7,8,9,10,11,12,13,255},	// attack
				{25,26,27,28,29,30,31,254,31,254,31,254,31,254,31,254,31,255},	// die
				{14,14,15,15,16,16,17,17,18,19,20,255},	// A1=punch2
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Wet Willy",255,255,0,AI_WetWilly,
		 12,20,30,1000,"graphics/willy.jsp",0,{},
		 MT_EVIL | MT_URBAN,
			{
				{0,1,2,3,4,5,6,7,8,9,10,11,255},	// idle
				{0,1,2,3,4,5,6,7,8,9,10,11,255},	// move
				{12,13,14,15,15,15,15,15,14,13,12,255},	// attack
				{16,17,18,19,17,18,19,18,19,255},	// die
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"STOP! Sign",255,255,0,NULL, // not yet implemented
		 8,38,10,50,"graphics/stopsign.jsp",0,{},
		 MT_EVIL | MT_URBAN,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack 1
				{0,255},	// defeated
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Dull Drum",255,255,0,NULL, // not yet implemented
		 8,38,10,50,"graphics/oildrum.jsp",0,{},
		 MT_EVIL | MT_URBAN,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack 1
				{0,255},	// defeated
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Bodzha",255,255,0,NULL, // not yet implemented
		 8,38,10,50,"graphics/bodzha.jsp",0,{},
		 MT_EVIL | MT_ANIMAL,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack 1
				{0,255},	// defeated
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0 },
		{"Punk Bunny",255,255,0,AI_PunkBunny,
		 8,16,40,100,"graphics/punkbunny.jsp",0,{},
		 MT_ANIMAL | MT_URBAN | MT_GOOD,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// walk
				{5,6,7,8,9,7,8,9,7,8,9,10,7,6,5,255},	// attack (machine gun)
				{0,0,254,254,255},	// die
				{11,12,13,14,15,255},	// A1 = bomb throw
				{5,6,7,8,9,7,8,9,7,8,9,10,7,6,5,255},	// attack (machine gun)
				{5,6,7,8,9,7,8,9,7,8,9,10,7,6,5,255},	// attack (machine gun)
				{5,6,7,8,9,7,8,9,7,8,9,10,7,6,5,255},	// attack (machine gun)
				{5,6,7,8,9,7,8,9,7,8,9,10,7,6,5,255},	// attack (machine gun)
			},0},
		{"Moley",255,255,0,NULL, // not yet implemented
		 8,38,10,50,"graphics/moley.jsp",0,{},
		 MT_EVIL | MT_ANIMAL,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack 1
				{0,255},	// defeated
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0 },
		{"Gargoyle",255,255,0,NULL, // not yet implemented
		 8,38,10,50,"graphics/gargoyle.jsp",0,{},
		 MT_EVIL | MT_SPOOKY,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack 1
				{0,255},	// defeated
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Nasty Tree",255,255,0,AI_NastyTree, // not yet implemented
		 8,38,10,50,"graphics/tree.jsp",0,{},
		 MT_EVIL | MT_VEGGIE | MT_WACKY,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack 1
				{0,255},	// defeated
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Chainball",255,255,0,AI_SpikeBall, // not yet implemented
		 8,38,10,50,"graphics/spikeball.jsp",0,{},
		 MT_EVIL | MT_WACKY,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack 1
				{0,255},	// defeated
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Foreman",255,255,0,NULL, // not yet implemented
		 8,38,10,50,"graphics/foreman.jsp",0,{},
		 MT_EVIL | MT_HUMAN | MT_URBAN,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack 1
				{0,255},	// defeated
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"MechaTruck",255,255,0,NULL, // not yet implemented
		 8,38,10,50,"graphics/mechatruck.jsp",0,{},
		 MT_EVIL | MT_HIGHTECH | MT_VEHICLE | MT_WACKY,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack 1
				{0,255},	// defeated
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"King Cone",255,255,0,AI_KingCone,
		 64,27,500,1000,"graphics/kingcone.jsp",0,MF_NOMOVE | MF_ONEFACE | MF_FACECMD | MF_ENEMYWALK,
		 MT_EVIL | MT_WACKY | MT_URBAN | MT_MINIBOSS,
			{
				{0,255},	// idle
				{5,6,7,7,8,8,9,0,255},	// move (hop to summon cones)
				{10,11,12,13,14,15,16,17,18,255},	// attack (crown spin)
				{19,19,20,20,21,21,22,22,23,23,24,24,25,25,26,255},	// die
				{10,11,12,13,14,15,16,17,18,255},	// attack (crown spin)
				{10,11,12,13,14,15,16,17,18,255},	// attack (crown spin)
				{10,11,12,13,14,15,16,17,18,255},	// attack (crown spin)
				{10,11,12,13,14,15,16,17,18,255},	// attack (crown spin)
				{10,11,12,13,14,15,16,17,18,255},	// attack (crown spin)
			},0},
		{"Teckhead",255,255,0,NULL, // not yet implemented
		 8,38,10,50,"graphics/mechatruck.jsp",0,{},
		 MT_EVIL | MT_HUMAN | MT_URBAN,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack 1
				{0,255},	// defeated
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"Edison Coil",255,255,0,NULL, // not yet implemented
		 8,38,10,50,"graphics/edisoncoil.jsp",0,{},
		 MT_EVIL | MT_HIGHTECH | MT_URBAN,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack 1
				{0,255},	// defeated
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0},
		{"EDISON",255,255,0,NULL, // not yet implemented
		 8,38,10,50,"graphics/edison.jsp",0,{},
		 MT_EVIL | MT_BOSS | MT_HIGHTECH,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack 1
				{0,255},	// defeated
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0 },
		{"EDISON's Brain",255,255,0,NULL, // not yet implemented
		 8,38,10,50,"graphics/edison.jsp",0,{},
		 MT_BITS,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack 1
				{0,255},	// defeated
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0 },
		{"Punkin",255,255,0,AI_Punkin,
		 9,10,10,1000,"graphics/punkin.jsp",0,{},
		 MT_EVIL | MT_PUMPKIN | MT_VEGGIE | MT_URBAN,
			{
				{0,255},	// idle
				{0,1,2,3,4,5,6,7,8,9,255},	// move
				{0,255},	// attack
				{0,0,0,0,255},	// die
				{0,1,2,3,4,5,6,7,8,9,255},	// move
				{0,1,2,3,4,5,6,7,8,9,255},	// move
				{0,1,2,3,4,5,6,7,8,9,255},	// move
				{0,1,2,3,4,5,6,7,8,9,255},	// move
				{0,1,2,3,4,5,6,7,8,9,255},	// move
			},0},
		{"Chef Angelo",255,255,0,NULL, // not yet implemented
		 8,38,10,50,"graphics/angelo.jsp",0,{},
		 MT_EVIL | MT_HUMAN | MT_WACKY,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack 1
				{0,255},	// defeated
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
				NULL_ANIM,
			},0},
	};
