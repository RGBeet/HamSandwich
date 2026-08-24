#include "monsnotes.h"
#include "monster.h"

char monsNotes[NUM_MONSTERS][512]={
	// NULL
	"No notes!",
	// Bouapha
	"The titular hero himself! Little known fact: "
	"\"Bouapha\" is a Laotian surname meaning \"flower\". "
	"While he usually wears a green shirt, he wears red "
	"in Project Sylabulus for stylistic reasons. Maybe he "
	"left the green shirt in the wash? As for his hammer, "
	"my art professor suggested it possibly represents \"justice\" - "
	"I found that symbolism quite intriguing, especially given "
	"the present themes.",
	// Bonehead
	"One of, if not the most iconic Hamumu enemies. They "
	"intially appeared in Jamul's 1996 gamelet \"Boboli\", "
	"way before Dr. Lunatic. For their original model, Jamul "
	"opted to use a skull from a model pack. I don't blame him - "
	"skulls are a pain to model, especially if you're a beginner "
	"modeler like myself. That being said, they're an important "
	"part of the spooky ecosystem.",
	// Mumble
	"These guys used to be much more of a pushover, but I felt "
	"they get a bad wrap. Now they move a little faster and make "
	"excellent use of the new pathfinding mechanics. They will "
	"chase you down and remember (almost) exactly where you go. "
	"Still, they're great hammer fodder (as shown in the tutorial) "
	"and an excellent audience (they clap at everything!)",
	// Zombie
	"Another classic enemy, now improved! It felt kind of weird the "
	"zombies never tried biting into your cranium, so I gave them "
	"a modest biting animation for when leaping felt excessive. "
	"Biting aside, they're just as dumb and reckless as ever. I'd "
	"imagine they're an endangered species after the events of "
	"Dr. Lunatic, for lore reasons.",
	// Lantern
	"The first original enemy designed for Project Sylabulus! "
	"They're designed after the chochin obake, or paper lantern yokai, "
	"and are reminiscent of the jack-o-lanterns of the west - "
	"a happy coincidence. These guys veer more on the \"cute\" side, "
	"but that's how Hamumu designs usually go. I like how they stick out "
	"their tongue. It's very mischevious!",
	// Hardhat
	"The unintentional common mook of Sylabulus. I actually modeled their "
	"animations after the Bonehead given I had no idea what I was doing. "
	"This was my first time animating a humanoid model, fingers and all. ",
	"I'd say it's worth it as the Hardhat does its job excellently; in a sense, "
	"they represent unfettered colonialism and gentrification. That's been their "
	"modus operandi since their debut in Hitsuji: Shears of Fury."
	// Pelican
	"",
	// Oui Ninja
	"",
	// Kaptana
	"",
	// Aquazoid
	"The zoids have returned to Bouapha's adventures! They haven't changed "
	"much from their Dr. Luantic counterparts as they're pretty much perfect. "
	"I personally think Aquazoids work excellently in any level with a body of "
	"water... just give the player enough wiggle room. Water IS deadly, after all.",
	// Tsuchizoid
	"The first zoid to move on land! Based on the tsuchinoko (child of hammer) yokai, "
	"these unusual snake-serpents are quite tricky. It resists hammers and delivers "
	"a rather poisonous bite should you get too close, both behaviors that reflect "
	"its inspiration. As for the side stepping, I tried to make it so they don't "
	"exactly run away from you, but invite you to pursue them.",
	// Buddy Bunny
	"",
	// Buddy Bunny:Path
	"",
	// Buddy Bunny:Follow
	"",
	// Jimbo
	"",
	// Ninjabread Man
	"",
	// Pumpkin
	"",
	// Kabocha
	"",
	// Crazy Bush
	"",
	// Mikasa
	"",
	// Eyeball Tree
	"",
	// Gingersnap
	"",
	// Happy Stick Ninja
	"",
	// Noh-Mann (1)
	"",
	// Noh-Mann (2)
	"WARNING! This is the alternate form of Noh-Mann. While using it won't crash your game, "
	"it may yield unexpected results. Edit with caution!",
	// Noh-Mann
	"WARNING! This is the left arm of Noh-Mann. While using it won't crash your game, "
	"it may yield unexpected results. Edit with caution!",
	// Noh-Mann
	"WARNING! This is the right arm of Noh-Mann. While using it won't crash your game, "
	"it may yield unexpected results. Edit with caution!",
	// Yerfdog (helper)
	"",
	// Yerfdog (boss)
	"",
	// Nobody
	"",
	// Eensy Weensy
	"",
	// Spitter
	"",
	// Egg Sac
	"",
	// Mama Spdier
	"",
	// Shroom
	"",
	// Shroomlord
	"",
	// Mush the Shroom
	"",
	// Crazybone
	"",
	// Manic Mumble
	"",
	// Bombie
	"Whereas the Zombie zombs, the Bombie explodes. These guys are tough to kill because "
	"they're supposed to divebomb you in hilarious fashion. Although they originally hail from "
	"Loonyland, I revamped their AI to make them more memorable - for better and worse. Their "
	"explosion sound, a legacy GameMaker sound effect, also appears in Deltarune as part of "
	"a running gag.",
	// Bob the Biscuit
	"",
	// Pumpkin Patch
	"",
	// Boomkin
	"",
	// Squash
	"",
	// Jalapeno
	"",
	// Mutant Hardhat
	"",
	// Madkap
	"",
	// Purple Kaptana
	"",
	// Thornbush
	"",
	// Hot N' Sour
	"",
	// Death Cap
	"",
	// Treasure Chest
	"",
	// Sneaky Shark
	"",
	// Turret
	"",
	// Missile Turret
	"",
	// Death Turret
	"",
	// Good Turret
	"",
	// Evil Clone
	"",
	// Mine Cart
	"",
	// Power Armor
	"WARNING! This entity is used for the Power Armor power-up. While using it"
	"won't crash your game, it may yield unexpected results. Edit with caution!",
	// Mini-Sub
	"WARNING! This entity is used for the Mini-Sub power-up. While using it"
	"won't crash your game, it may yield unexpected results. Edit with caution!",
	// You-Go
	"",
	// LunaChick
	"",
	// Shtupid Shroom
	"",
	// Creeping Cone
	"",
	// Autozoid
	"",
	// Parked Car
	"",
	// Traffic
	"",
	// Mailboxer
	"",
	// Wet Willy
	"",
	// STOP! Sign
	"",
	// Dull Drum
	"",
	// Bodzha
	"",
	// Punk Bunny
	"",
	// Moley
	"",
	// Gargoyle
	"",
	// Nasty Tree
	"",
	// Chainball
	"",
	// Foreman
	"",
	// MechaTruck
	"",
	// King Cone
	"",
	// Teckhead
	"",
	// Edison Coil
	"",
	// EDISON
	"",
	// EDISON'S brain
	"WARNING! This is EDISON's second form. While using it"
	"won't crash your game, it may yield unexpected results. Edit with caution!",
	// Punkin
	"",
	// Chef Angelo
	"",
};

char *MonsterNotes(dword type)
{
	return monsNotes[type];
}
