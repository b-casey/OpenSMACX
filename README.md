# OpenSMACX
A project to decompile SMAC/X to C++ with the long term goal of creating a full open source clone.

Source tested and compiled with Visual Studio 2017.

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