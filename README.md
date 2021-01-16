# OpenSMACX
A project to decompile SMAC/X to C++ with the long term goal of creating a full open source clone.

Source tested and compiled as an x86 DLL with Visual Studio 2019 using Visual C++ with default 
settings.

Tested with CodeBlocks 20.03 using GCC 8.1.0 compiler. Project file (cbp) courtesy of induktio.

The patcher script found under tools is compatible with the latest GOG version as well as the most
recent version of my unofficial patch. Cursory testing shows it is compatible with PRACX.

You can follow development progress, discuss ideas or issues here:  
https://alphacentauri2.info/index.php?board=23.0

v0.2.3
* Lots of additional analysis, clean up, refactoring, and optimization of the existing code base.
* Fixed four bugs within OpenSMACX code. 
* Created new probe related source files (two net new functions, shifted one existing).
* Implemented one of the probe related fixes from my unofficial patch.
* Additional work on Path class (almost complete, 3 functions left).
* 55 net new functions across multiple areas.
* Cumulative decompiled and redirected function count: 420

Bug Fixes:
* Fixed a bug with OpenSMACX where a small change to how the Random class was reseeded from the 
  default behavior caused rivers globally to shift. This happened whenever a Borehole or Condenser 
  was built by any faction. I'm still unsure as to why. Something to review again in the future when 
  more of the world code is decompiled.
* Fixed a bug with OpenSMACX where after selecting a random faction for a game, afterward on game
  start the game would throw a faction parsing error related to JENN282. This was due to the 
  JENN282 code not being implemented in read_factions(). Originally, it looked like this was left 
  over debug code so it was skipped. As it turns out, JENN282 is the internal faction identifier 
  that SMACX uses for a random faction. Thankfully, this was the only place in the project where 
  code wasn't implemented like this (excluding obvious optimizations). Multiple improvements to the 
  code's performance and fixes for fringe cases were applied to this parsing code.
* Fixed an issue with OpenSMACX where two functions (tech_ai, rnd) weren't having rand() reseeded
  properly. This was caused by my_srand() only having the old built-in version of srand(). Both need 
  to be initialized while the project straddles new and old code base.
* Fixed a preference parsing bug with OpenSMACX where "Time Controls" default value could have been 
  set as "0" rather than "1". This was due to an oversight in how the return value was processed 
  by prefs_get(LPCSTR, int, BOOL) when the useDefault parameter was set to true. This code is part 
  of some of the earliest decompiled functions prior to my including comprehensive regression tests
  into the decompile process.
* Fixed a bug inside success_rates() where a probe wouldn't receive a penalty to its survival rate 
  when it targeted a faction that has the Hunter-Seeker Algorithm secret project. Instead, its 
  success rate would be given an erroneous 2nd penalty. This issue was somewhat masked because the
  2nd penalty happened after the success rate had already been written for display by the UI. This 
  bug caused a lower success rate and increased survival rate of probes targeting an HSA faction. 
* Fixed various issues related to the Scenario Editor undo/redo mechanic. Undo auto-saves weren't 
  properly saved outside of the 1st one because auto_undo() didn't include the ".SAV" extension
  to process previous auto-saves. Undo auto-saves weren't properly removed because wipe_undo() 
  didn't include the ".SAV" extension. Added in logic to prevent being able to attempt a redo before 
  an undo has taken place. Fixed a map redraw issue after loading an undo or redo where units and 
  tiles wouldn't be properly redrawn.

v0.2.2
* Lots of clean up and optimization of the existing code base.
* Initial work on the Path class.
* Added some Council related functions.
* 50 new functions across almost every game related area with a focus on Map and Technology.
* Decompiled and redirected function count: 365

Bug Fixes:
* Fixed a bug under certain conditions that the end game function to determine whether a faction is
  nearing a diplomatic win (Supreme Leader) would return incorrect results. The function aah_ooga() 
  is called with the 2nd parameter set to -1 in certain instances. The original code would then 
  attempt to use this value to check the pact status within the player_data structure of the 1st 
  faction parameter. In these instances, it would actually be trying to do the diplomacy check 
  against tutorialMoreBases value. There was a check to skip the pact check if the 2nd parameter 
  was 0.  The fix now accepts -1 or 0 to skip the pact check.

v0.2.1
* Some additional map functions to get ready for working on Path class.
* Miscellaneous clean up.
* Fixed an issue with the patcher script incorrectly patching best_specialist() and base_making().
* Decompiled and redirected function count: 315

Bug Fixes:
* Factions with the FREEPROTO flag (Spartans) will gain free retooling in their bases as long as the
  production switch is within the same category (unit to unit, base facility to base facility) and
  they've discovered the necessary tech for Skunkworks (default is "Advanced Subatomic Theory"). 
  This is to resolve the issue with FREEPROTO factions never being able to gain the undocumented 
  retooling ability of a Skunkworks found in base_making(). There is good indication this was likely
  an oversight and that the FREEPROTO flag should be the equivalent to a free Skunkworks for 
  the faction's bases.

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
* <strike>Removed debug code related to non-existent faction JENN282. Nothing of value. SMACX binary only.</strike>
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
