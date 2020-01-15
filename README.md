# OpenSMACX
A project to decompile SMAC/X to C++ with the long term goal of creating a full open source clone.

Source tested and compiled as an x86 DLL with Visual Studio 2017 using Visual C++ with default 
settings. Source was also tested using Visual Studio 2015 Visual C++.

Tested with CodeBlocks using GCC 8.1.0 compiler. Added project file (cbp) courtesy of induktio.

v0.2
* Veh, Map and Base related code that sets the groundwork to break down more complex functions.
* Engine classes: Font, Spot, Time.
* Decompiled and redirected function count: 308

Bug Fixes:
* Added an additional check to facility_avail() that prevents the Caretakers from being given the 
  ability of selecting the secret project "Ascent to Transcendence". This goes against their core
  philosophy and would cause them to declare war on themselves if initiated.
* Sealurks no longer receive a movement penalty when moving through Sea Fungus. They are now treated 
  the same as an "Isle of the Deep" as intended. This was likely an oversight to add the specific 
  check to hex_cost().
* Added a new check inside facility_avail() that prevents building Paradise Garden if you have
  Punishment Sphere built or in queue. There is a check to prevent Punishment Sphere from being 
  constructed if you have Paradise Garden but not vise versa. Paradise Garden and Punishment Sphere 
  are antithetical facilities where you can build either one in any given base, but you were never 
  suppose to be able to build both.
* Fixed a bug in DirectPlay multiplayer that likely caused a performance hit when moving units due 
  to a faulty coordinate check. Whenever the stack_fix() function was called by moving a unit or 
  other actions, it would cause every stack of units on the map to be sorted in a certain way (at 
  least temporarily) as well as redundantly sorting the same stack once for every unit on the map.
* Removed an extra space displayed for certain prototype statistics (Ex. Transport Foil) shown in
  the Military Command Nexus via say_stats() function.
* Fixed logic inside offense_proto() and armor_proto() where under certain conditions the game 
  would try to compare an arbitrary memory value against the Spore Launcher basic unit id. If they 
  matched, non-PSI units would display incorrect offense and defense values. This happened any time 
  you clicked on a non-PSI unit when calculating its offensive value. It was just incredibly rare 
  the memory address value would match the Spore Launcher id. While it was unlikely this would be
  triggered by armor_proto() due to logic flow, added in a preventative bound check anyway.
* Fixed a bug in Time::pulse(void(__cdecl *)(int), int, uint32_t, uint32_t) where the Timer event
  could have persisted and executed continuously when it should have executed only once. It seems 
  affected code branch was never used by the original game.
* Miscellaneous additional error handling or bound checks to various functions (see source).

Enhancement:
* Added the ability to set a reactor value (1-4) for #UNITS inside alpha/x.txt. To do so, add a 
  comma after the Abil field with the value of the reactor you want for the unit. If no value or a 
  value of 0 is set, it defaults to 1 (Fission Plant) like the original code.  For SMACX only, there 
  are two exceptions where the default isn't 1 but 2 (Battle Ogre MK2) and 3 (Battle Ogre MK3). The 
  Ogre defaults can still be overridden.
  Ex. "Colony Pod,..., 00000000000000000000000000,4" will give Colony Pods a Singularity Engine.
* Added some basic randomization for sea base name order. This mostly affects Pirates since 
  the randomization method isn't great for small lists. Base name count increases once
  all names inside faction files and basename.txt are exhausted rather than stopping.

v0.1
* Parsing in alpha/x.txt and faction text files complete

Bug fixes:
* Fixed crash if BaseButton::set_name(LPCSTR input) parameter was NULL.
* Fixed bug with TextIndex having carriage return at end of "HEADER\r" breaking compare. This could 
  have had significant performance increases back in 1999. Not so much in the time of cheap SSDs.
* Fixed bug if Heap::get() realloc ended up shifting memory to a different location (unlikely).
* Fixed crash in proto_cost() found via fuzzing/testing. Could be triggered by mods.
* Fixed Retool strictness in alpha/x.txt to accept a value of 3 (never free).
* Fixed setting alpha/x.txt probe teams can steal technologies now only to accept 0 or 1.
* Fixed setting alpha/x.txt humans always contact in net games now only to accept 0 or 1.
* Fixed setting alpha/x.txt humans always contact in hotseat/email games now only to accept 0 or 1.
* Fixed setting alpha/x.txt obliterating a base counts as an atrocity now only to accept 0 or 1.
* Fixed socIdeologyEffect overriding faction text files with -1, disabling AI faction "Emphasis".
* Fixed reactor power value being ignored when parsing. Game still won't use this value until
  more code is decompiled since hardcoded values are being used throughout.

Changes:
* Improved how Random::reseed sets new seed value.
* Improved performance of proto_cost() without changes to original logic.
* Removed debug code related to non-existent faction JENN282. Nothing of value. SMACX binary only.
* Various optimizations without changes to original logic.

Enhancement: 
* The original code explicitly sets facility freetech value to disabled (-2) overriding alpha/x.txt.  
  It states in #FACILITIES alpha/x.txt: "Free  = No longer supported". This mechanic could have been 
  removed for balance reasons or dropped due to time constraints. There is code that checks this 
  value and sets the free facility only for new bases built after discovering the tech. It looks 
  like existing bases do not get it. This will have to be reviewed more. For now, this mechanic is 
  included as is. You can revert to vanilla behavior by modifying the four entries below in 
  alpha/x.txt #FACILITIES with free tech parameter set to "Disabled".

	Recycling Tanks, 4, 0, Biogen, EcoEng2,   > free with "Adv.Ecological Engineering"
	Recreation Commons, 4, 1, Psych, SentEco, > free with "Sentient Econometrics"
	Energy Bank, 8, 1, IndEcon, QuanMac,      > free with "Quantum Machinery"
	Network Node, 8, 1, InfNet, HAL9000,      > free with "Self - Aware Machines"