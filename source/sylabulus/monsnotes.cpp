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
	"skulls are a pain to model, especially if you're a beginner ",
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
	// Pelican
	"",
	// Oui Ninja
};

char *MonsterNotes(dword type)
{
	return monsNotes[type];
}
