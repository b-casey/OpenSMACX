/*
 * OpenSMACX - an open source clone of Sid Meier's Alpha Centauri.
 * Copyright (C) 2013-2020 Brendan Casey
 *
 * OpenSMACX is free software: you can redistribute it and / or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenSMACX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenSMACX. If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

 /*
  * Base related objects, variables and functions.
  */
enum facility {
	FAC_HEADQUARTERS = 1,
	FAC_CHILDREN_CRECHE = 2,
	FAC_RECYCLING_TANKS = 3,
	FAC_PERIMETER_DEFENSE = 4,
	FAC_TACHYON_FIELD = 5,
	FAC_RECREATION_COMMONS = 6,
	FAC_ENERGY_BANK = 7,
	FAC_NETWORK_NODE = 8,
	FAC_BIOLOGY_LAB = 9,
	FAC_SKUNKWORKS = 10,
	FAC_HOLOGRAM_THEATRE = 11,
	FAC_PARADISE_GARDEN = 12,
	FAC_TREE_FARM = 13,
	FAC_HYBRID_FOREST = 14,
	FAC_FUSION_LAB = 15,
	FAC_QUANTUM_LAB = 16,
	FAC_RESEARCH_HOSPITAL = 17,
	FAC_NANOHOSPITAL = 18,
	FAC_ROBOTIC_ASSEMBLY_PLANT = 19,
	FAC_NANOREPLICATOR = 20,
	FAC_QUANTUM_CONVERTER = 21,
	FAC_GENEJACK_FACTORY = 22,
	FAC_PUNISHMENT_SPHERE = 23,
	FAC_HAB_COMPLEX = 24,
	FAC_HABITATION_DOME = 25,
	FAC_PRESSURE_DOME = 26,
	FAC_COMMAND_CENTER = 27,
	FAC_NAVAL_YARD = 28,
	FAC_AEROSPACE_COMPLEX = 29,
	FAC_BIOENHANCEMENT_CENTER = 30,
	FAC_CENTAURI_PRESERVE = 31,
	FAC_TEMPLE_OF_PLANET = 32,
	FAC_PSI_GATE = 33,
	FAC_COVERT_OPS_CENTER = 34,
	FAC_BROOD_PIT = 35,
	FAC_AQUAFARM = 36,
	FAC_SUBSEA_TRUNKLINE = 37,
	FAC_THERMOCLINE_TRANSDUCER = 38,
	FAC_FLECHETTE_DEFENSE_SYS = 39,
	FAC_SUBSPACE_GENERATOR = 40,
	FAC_GEOSYNC_SURVEY_POD = 41,
	FAC_EMPTY_FACILITY_42 = 42,
	FAC_EMPTY_FACILITY_43 = 43,
	FAC_EMPTY_FACILITY_44 = 44,
	FAC_EMPTY_FACILITY_45 = 45,
	FAC_EMPTY_FACILITY_46 = 46,
	FAC_EMPTY_FACILITY_47 = 47,
	FAC_EMPTY_FACILITY_48 = 48,
	FAC_EMPTY_FACILITY_49 = 49,
	FAC_EMPTY_FACILITY_50 = 50,
	FAC_EMPTY_FACILITY_51 = 51,
	FAC_EMPTY_FACILITY_52 = 52,
	FAC_EMPTY_FACILITY_53 = 53,
	FAC_EMPTY_FACILITY_54 = 54,
	FAC_EMPTY_FACILITY_55 = 55,
	FAC_EMPTY_FACILITY_56 = 56,
	FAC_EMPTY_FACILITY_57 = 57,
	FAC_EMPTY_FACILITY_58 = 58,
	FAC_EMPTY_FACILITY_59 = 59,
	FAC_EMPTY_FACILITY_60 = 60,
	FAC_EMPTY_FACILITY_61 = 61,
	FAC_EMPTY_FACILITY_62 = 62,
	FAC_EMPTY_FACILITY_63 = 63,
	FAC_EMPTY_FACILITY_64 = 64,
	FAC_SKY_HYDRO_LAB = 65,
	FAC_NESSUS_MINING_STATION = 66,
	FAC_ORBITAL_POWER_TRANS = 67,
	FAC_ORBITAL_DEFENSE_POD = 68,
	FAC_STOCKPILE_ENERGY = 69,
	FAC_HUMAN_GENOME_PROJ = 70,
	FAC_COMMAND_NEXUS = 71,
	FAC_WEATHER_PARADIGM = 72,
	FAC_MERCHANT_EXCHANGE = 73,
	FAC_EMPATH_GUILD = 74,
	FAC_CITIZENS_DEFENSE_FORCE = 75,
	FAC_VIRTUAL_WORLD = 76,
	FAC_PLANETARY_TRANS_SYS = 77,
	FAC_XENOEMPATHY_DOME = 78,
	FAC_NEURAL_AMPLIFIER = 79,
	FAC_MARITIME_CONTROL_CENTER = 80,
	FAC_PLANETARY_DATALINKS = 81,
	FAC_SUPERCOLLIDER = 82,
	FAC_ASCETIC_VIRTUES = 83,
	FAC_LONGEVITY_VACCINE = 84,
	FAC_HUNTER_SEEKER_ALGO = 85,
	FAC_PHOLUS_MUTAGEN = 86,
	FAC_CYBORG_FACTORY = 87,
	FAC_THEORY_OF_EVERYTHING = 88,
	FAC_DREAM_TWISTER = 89,
	FAC_UNIVERSAL_TRANSLATOR = 90,
	FAC_NETWORK_BACKBONE = 91,
	FAC_NANO_FACTORY = 92,
	FAC_LIVING_REFINERY = 93,
	FAC_CLONING_VATS = 94,
	FAC_SELF_AWARE_COLONY = 95,
	FAC_CLINICAL_IMMORTALITY = 96,
	FAC_SPACE_ELEVATOR = 97,
	FAC_SINGULARITY_INDUCTOR = 98,
	FAC_BULK_MATTER_TRANSMITTER = 99,
	FAC_TELEPATHIC_MATRIX = 100,
	FAC_VOICE_OF_PLANET = 101,
	FAC_ASCENT_TO_TRANSCENDENCE = 102,
	FAC_MANIFOLD_HARMONICS = 103,
	FAC_NETHACK_TERMINUS = 104,
	FAC_CLOUDBASE_ACADEMY = 105,
	FAC_PLANETARY_ENERGY_GRID = 106,
	FAC_EMPTY_SP_38 = 107,
	FAC_EMPTY_SP_39 = 108,
	FAC_EMPTY_SP_40 = 109,
	FAC_EMPTY_SP_41 = 110,
	FAC_EMPTY_SP_42 = 111,
	FAC_EMPTY_SP_43 = 112,
	FAC_EMPTY_SP_44 = 113,
	FAC_EMPTY_SP_45 = 114,
	FAC_EMPTY_SP_46 = 115,
	FAC_EMPTY_SP_47 = 116,
	FAC_EMPTY_SP_48 = 117,
	FAC_EMPTY_SP_49 = 118,
	FAC_EMPTY_SP_50 = 119,
	FAC_EMPTY_SP_51 = 120,
	FAC_EMPTY_SP_52 = 121,
	FAC_EMPTY_SP_53 = 122,
	FAC_EMPTY_SP_54 = 123,
	FAC_EMPTY_SP_55 = 124,
	FAC_EMPTY_SP_56 = 125,
	FAC_EMPTY_SP_57 = 126,
	FAC_EMPTY_SP_58 = 127,
	FAC_EMPTY_SP_59 = 128,
	FAC_EMPTY_SP_60 = 129,
	FAC_EMPTY_SP_61 = 130,
	FAC_EMPTY_SP_62 = 131,
	FAC_EMPTY_SP_63 = 132,
	FAC_EMPTY_SP_64 = 133,
};

enum  secret_project {
	SP_HUMAN_GENOME_PROJ = 0,
	SP_COMMAND_NEXUS = 1,
	SP_WEATHER_PARADIGM = 2,
	SP_MERCHANT_EXCHANGE = 3,
	SP_EMPATH_GUILD = 4,
	SP_CITIZENS_DEFENSE_FORCE = 5,
	SP_VIRTUAL_WORLD = 6,
	SP_PLANETARY_TRANS_SYS = 7,
	SP_XENOEMPATYH_DOME = 8,
	SP_NEURAL_AMPLIFIER = 9,
	SP_MARITIME_CONTROL_CENTER = 10,
	SP_PLANETARY_DATALINKS = 11,
	SP_SUPERCOLLIDER = 12,
	SP_ASCETIC_VIRTUES = 13,
	SP_LONGEVITY_VACCINE = 14,
	SP_HUNTER_SEEKER_ALGO = 15,
	SP_PHOLUS_MUTAGEN = 16,
	SP_CYBORG_FACTORY = 17,
	SP_THEORY_OF_EVERYTHING = 18,
	SP_DREAM_TWISTER = 19,
	SP_UNIVERSAL_TRANSLATOR = 20,
	SP_NETWORK_BACKBONE = 21,
	SP_NANO_FACTORY = 22,
	SP_LIVING_REFINERY = 23,
	SP_CLONING_VATS = 24,
	SP_SELF_AWARE_COLONY = 25,
	SP_CLINICAL_IMMORTALITY = 26,
	SP_SPACE_ELEVATOR = 27,
	SP_SINGULARITY_INDUCTOR = 28,
	SP_BULK_MATTER_TRANSMITTER = 29,
	SP_TELEPATHIC_MATRIX = 30,
	SP_VOICE_OF_PLANET = 31,
	SP_ASCENT_TO_TRANSCENDENCE = 32,
	SP_MANIFOLD_HARMONICS = 33,
	SP_NETHACK_TERMINUS = 34,
	SP_CLOUDBASE_ACADEMY = 35,
	SP_PLANETARY_ENERGY_GRID = 36,
	SP_EMPTY_38 = 37,
	SP_EMPTY_39 = 38,
	SP_EMPTY_40 = 39,
	SP_EMPTY_41 = 40,
	SP_EMPTY_42 = 41,
	SP_EMPTY_43 = 42,
	SP_EMPTY_44 = 43,
	SP_EMPTY_45 = 44,
	SP_EMPTY_46 = 45,
	SP_EMPTY_47 = 46,
	SP_EMPTY_48 = 47,
	SP_EMPTY_49 = 48,
	SP_EMPTY_50 = 49,
	SP_EMPTY_51 = 50,
	SP_EMPTY_52 = 51,
	SP_EMPTY_53 = 52,
	SP_EMPTY_54 = 53,
	SP_EMPTY_55 = 54,
	SP_EMPTY_56 = 55,
	SP_EMPTY_57 = 56,
	SP_EMPTY_58 = 57,
	SP_EMPTY_59 = 58,
	SP_EMPTY_60 = 59,
	SP_EMPTY_61 = 60,
	SP_EMPTY_62 = 61,
	SP_EMPTY_63 = 62,
	SP_EMPTY_64 = 63,
};

enum base_state_bitfield {
	BSTATE_UNK_1 = 0x1,
	BSTATE_DRONE_RIOTS_ACTIVE = 0x2,
	BSTATE_GOLDEN_AGE_ACTIVE = 0x4,
	BSTATE_COMBAT_LOSS_LAST_TURN = 0x8, // Reset on upkeep
	BSTATE_UNK_10 = 0x10,
	BSTATE_UNK_20 = 0x20,
	BSTATE_RESEARCH_DATA_STOLEN = 0x40,
	BSTATE_UNK_80 = 0x80,
	BSTATE_UNK_100 = 0x100,
	BSTATE_FACILITY_SCRAPPED = 0x200, // Only one facility can be scrapped/recycled per turn
	BSTATE_ARTIFACT_LINKED = 0x400, // Alien Artifact linked to Network Node
	BSTATE_ARTIFACT_ALREADY_LINKED = 0x800, // Only show already linked alert once
	BSTATE_UNK_1000 = 0x1000,
	BSTATE_UNK_2000 = 0x2000,
	BSTATE_UNK_4000 = 0x4000,
	BSTATE_UNK_8000 = 0x8000,
	BSTATE_UNK_10000 = 0x10000,
	BSTATE_GENETIC_PLAGUE_INTRO = 0x20000,
	BSTATE_ASSISTANT_KILLER_HOME = 0x40000, // Veh home base (or closest) of Assistant worm killer
	BSTATE_UNK_80000 = 0x80000,
	BSTATE_UNK_100000 = 0x100000,
	BSTATE_UNK_200000 = 0x200000,
	BSTATE_ENERGY_RESERVES_DRAINED = 0x400000,
	BSTATE_PRODUCTION_DONE = 0x800000,
	BSTATE_UNK_1000000 = 0x1000000,
	BSTATE_UNK_2000000 = 0x2000000,
	BSTATE_UNK_4000000 = 0x4000000,
	BSTATE_UNK_8000000 = 0x8000000,
	BSTATE_NET_LOCKED = 0x10000000,
	BSTATE_PSI_GATE_USED = 0x20000000,
	BSTATE_PRODUCTION_HURRIED = 0x40000000,
	BSTATE_UNK_8000000000 = 0x80000000,
};

enum base_event_bitfield {
	BEVENT_UNK_100 = 0x100,
	BEVENT_BUMPER = 0x200,
	BEVENT_FAMINE = 0x400,
	BEVENT_INDUSTRY = 0x800,
	BEVENT_BUST = 0x1000,
	BEVENT_HEAT_WAVE = 0x2000,
	BEVENT_CLOUD_COVER = 0x4000,
	BEVENT_OBJECTIVE = 0x8000,
};

enum governor_base_bitfield {
	GOV_MANAGES_PRODUCTION = 0x1,
	// 0x2
	// 0x4
	// 0x8
	// 0x10
	GOV_MAY_HURRY_PRODUCTION = 0x20,
	GOV_MANAGES_CITIZENS_SPECS = 0x40, // incl. drone riots
	GOV_NEW_VEH_FULLY_AUTO = 0x80,
	// 0x100
	GOV_MAY_PROD_LAND_COMBAT = 0x200,
	GOV_MAY_PROD_NAVAL_COMBAT = 0x400,
	GOV_MAY_PROD_AIR_COMBAT = 0x800,
	GOV_MAY_PROD_LAND_DEFENS = 0x1000,
	GOV_MAY_PROD_AIR_DEFENS = 0x2000,
	// 0x4000 ; Naval defense?
	GOV_MAY_PROD_TERRAFORMS = 0x8000,
	GOV_MAY_PROD_FACILITIES = 0x10000,
	GOV_MAY_PROD_COLONY_POD = 0x20000,
	GOV_MAY_PROD_SP = 0x40000,
	GOV_MAY_PROD_PROTOTYPE = 0x80000,
	GOV_MAY_PROD_PROBES = 0x100000,
	GOV_MULTI_PRIORITIES = 0x200000, // or no priorities
	GOV_MAY_PROD_EXPLR_VEH = 0x400000, // scout/exploration units
	GOV_MAY_PROD_TRANSPORT = 0x800000,
	GOV_PRIORITY_EXPLORE = 0x1000000,
	GOV_PRIORITY_DISCOVER = 0x2000000,
	GOV_PRIORITY_BUILD = 0x4000000,
	GOV_PRIORITY_CONQUER = 0x8000000,
	// 0x10000000
	// 0x20000000
	GOV_UNK_40000000 = 0x40000000,
	GOV_ACTIVE = 0x80000000,
};

enum citizen_specialists {
	CITIZEN_TECHNICIAN = 0,
	CITIZEN_DOCTOR = 1,
	CITIZEN_LIBRARIAN = 2,
	CITIZEN_ENGINEER = 3,
	CITIZEN_EMPATH = 4,
	CITIZEN_THINKER = 5,
	CITIZEN_TRANSCEND = 6,
};

struct base {
	int16_t xCoord;
	int16_t yCoord;
	uint8_t factionIDCurrent;
	uint8_t factionIDFormer;
	int8_t populationSize;
	uint8_t assimilationTurnsLeft;
	uint8_t nerveStapleTurnsLeft;
	uint8_t unk_1; // AI plan state?
	uint8_t visibility; // faction bitfield of those who can see base (mapped: dim/bright)
	uint8_t factionPopSizeIntel[8]; // last know population size for each faction
	char nameString[25];
	int16_t unkX; // terraforming related
	int16_t unkY; // terraforming related
	uint32_t state; // see base_state_bitfield
	uint32_t event; // see base_event_bitfield
	uint32_t governor; // see governor_base_bitfield
	int nutrientsAccumulated;
	int mineralsAccumulated;
	int productionIDLast;
	int ecoDamage;
	uint32_t queueSize;
	int queueProductionID[10]; // items in the production queue
	int resourceSqrRadius; // worked tiles
	int specialistTotal;
	int specialist_unk_1; // related to Governor controlling specialists, non-psych spec count?
	/*
	* Specialist types for the first 16 (see citizen_specialists, 4 bits per specialist)
	* Stored from left to right in base view, with left most represented by lower bits of spec
	* Specialists after 16th in base are auto assigned based on best_specialist()
	* Isn't correct when specialistTotal is 0, not reset? Or Governor actively controlling specs?
	*/
	uint32_t spec[2]; // specialist types
	uint8_t facilitiesBuilt[12];
	int mineralSurplusFinal;
	int mineralsAccumulated2; // original minerals before retooling?
	int unk_6; // unused?
	int unk_7; // unused?
	int unk_8; // unused?
	int unk_9; // unused?
	int nutrientIntake1;
	int mineralIntake1;
	int energyIntake1;
	int unusedIntake1;
	int nutrientIntake2;
	int mineralIntake2;
	int energyIntake2;
	int unusedIntake2;
	int nutrientSurplus;
	int mineralSurplus;
	int energySurplus;
	int unusedSurplus;
	int nutrientInefficiency;
	int mineralInefficiency;
	int energyInefficiency;
	int unusedInefficiency;
	int nutrientConsumption;
	int mineralConsumption;
	int energyConsumption;
	int unusedConsumption;
	int economyTotal;
	int psychTotal;
	int labsTotal;
	int unk_10;
	int16_t autoforwardLandBaseID;
	int16_t autoforwardSeaBaseID;
	int16_t autoforwardAirBaseID;
	int16_t padding0;
	int talentTotal;
	int droneTotal;
	int superdroneTotal;
	int randomEventTurns;
	int nerveStapleCount;
	int unk_11; // effectively unused, only set to 0 by base_init()
	int unk_12; // effectively unused, only set to 0 by base_init()
};

struct rules_facility {
	LPSTR name;
	LPSTR effect;
	uint32_t pad;
	int cost;
	uint32_t maint;
	int preqTech;
	int freeTech;
	int SP_AI_Fight;
	int SP_AI_Growth;
	int SP_AI_Tech;
	int SP_AI_Wealth;
	int SP_AI_Power;
};

struct rules_citizen {
	LPSTR singularName;
	LPSTR pluralName;
	int preqTech;
	int obsolTech;
	int opsBonus;
	int psychBonus;
	int researchBonus;
};

// Secret Projects > stores BaseID that built SP
struct base_secret_project {
	int HumanGenomeProject;
	int CommandNexus;
	int WeatherParadigm;
	int MerchantExchange;
	int EmpathGuild;
	int CitizensDefenseForce;
	int VirtualWorld;
	int PlanetaryTransitSystem;
	int XenoempathyDome;
	int NeuralAmplifier;
	int MaritimeControlCenter;
	int PlanetaryDatalinks;
	int Supercollider;
	int AsceticVirtues;
	int LongevityVaccine;
	int HunterSeekerAlgorithm;
	int PholusMutagen;
	int CyborgFactory;
	int TheoryOfEverything;
	int DreamTwister;
	int UniversalTranslator;
	int NetworkBackbone;
	int NanoFactory;
	int LivingRefinery;
	int CloningVats;
	int SelfAwareColony;
	int ClinicalImmortality;
	int SpaceElevator;
	int SingularityInductor;
	int BulkMatterTransmitter;
	int TelepathicMatrix;
	int VoiceOfPlanet;
	int AscentToTranscendence;
	int ManifoldHarmonics; // SMACX Only
	int NethackTerminus; // SMACX Only
	int CloudbaseAcademy; // SMACX Only
	int PlanetaryEnergyGrid; // SMACX Only
	int UserDefined38; // no direct ref in code logic?
	int UserDefined39;
	int UserDefined40;
	int UserDefined41;
	int UserDefined42;
	int UserDefined43;
	int UserDefined44;
	int UserDefined45;
	int UserDefined46;
	int UserDefined47;
	int UserDefined48;
	int UserDefined49;
	int UserDefined50;
	int UserDefined51;
	int UserDefined52;
	int UserDefined53;
	int UserDefined54;
	int UserDefined55;
	int UserDefined56;
	int UserDefined57;
	int UserDefined58;
	int UserDefined59;
	int UserDefined60;
	int UserDefined61;
	int UserDefined62;
	int UserDefined63;
	int UserDefined64;
};

constexpr int MaxBaseNum = 512;
constexpr int MaxFacilityNum = 134; // 0 slot unused
constexpr int MaxSecretProjectNum = 64;
constexpr int FacilitySPStart = 70; // Special Project start offset
constexpr int FacilityRepStart = 65; // Sky Hydro Lab; Nessus Mining Stat; OPT; ODP; Stockpile Eng
constexpr int MaxCitizenNum = 10;
constexpr int MaxSpecialistNum = 7;
constexpr int SP_Unbuilt = -1;
constexpr int SP_Destroyed = -2;

extern rules_facility *Facility;
extern rules_citizen *Citizen;
extern base *Base;
extern base_secret_project *SecretProject;
extern int *BaseIDCurrentSelected;
extern int *BaseCurrentCount;
extern int *BaseFindDist;
extern base **BaseCurrent;
extern uint32_t *ScnVictFacilityObj;
extern uint32_t *BaseUpkeepStage;
extern uint32_t *BaseCurrentConvoyFrom; // resources convoyed from base
extern uint32_t *BaseCurrentConvoyTo; // resources convoyed/extracted to base
extern int *BaseCurrentGrowthRate;
extern uint32_t *BaseCurrentVehPacifismCount;
extern uint32_t *BaseCurrentForcesSupported; // Forces Supported by Base
extern uint32_t *BaseCurrentForcesMaintCost; // Minerals expended for Forces Supported maintenance

OPENSMACX_API BOOL __cdecl has_fac(int facilityID, int baseID, int queueCount);
OPENSMACX_API void __cdecl set_base(int baseID);
OPENSMACX_API void __cdecl say_base(LPSTR strBase, int baseID);
OPENSMACX_API int __cdecl base_find(int xCoord, int yCoord);
OPENSMACX_API int __cdecl base_find(int xCoord, int yCoord, uint32_t factionID);
OPENSMACX_API int __cdecl base_find(int xCoord, int yCoord, int factionID, int region,
	int factionID2, int factionID3);
OPENSMACX_API uint32_t  __cdecl best_specialist();
OPENSMACX_API void __cdecl name_base(int factionID, LPSTR nameOut, BOOL isFinal, BOOL isSeaBase);
OPENSMACX_API void __cdecl base_mark(uint32_t baseID);
OPENSMACX_API int __cdecl cost_factor(uint32_t factionID, uint32_t rscType, int baseID);
OPENSMACX_API int __cdecl base_making(int productionID, int baseID);
OPENSMACX_API int __cdecl base_lose_minerals(int baseID, int productionID);
OPENSMACX_API void __cdecl set_fac(int facilityID, int baseID, BOOL set);
OPENSMACX_API BOOL __cdecl has_fac_announced(int factionID, int facilityID);
OPENSMACX_API void __cdecl set_fac_announced(int factionID, int facilityID, BOOL set);
OPENSMACX_API void __cdecl base_first(uint32_t baseID);
OPENSMACX_API uint32_t __cdecl morale_mod(uint32_t baseID, uint32_t factionID, uint32_t triad);
OPENSMACX_API uint32_t __cdecl breed_mod(uint32_t baseID, uint32_t factionID);
OPENSMACX_API uint32_t __cdecl worm_mod(uint32_t baseID, uint32_t factionID);
OPENSMACX_API void __cdecl base_nutrient();
OPENSMACX_API void __cdecl base_minerals();
OPENSMACX_API uint32_t __cdecl black_market(int energy);
OPENSMACX_API void __cdecl psych_check(uint32_t factionID, uint32_t *drones, uint32_t *talents);
OPENSMACX_API void __cdecl base_psych();
OPENSMACX_API int __cdecl base_rank(uint32_t faction_id, uint32_t to_rank);
OPENSMACX_API uint32_t __cdecl pop_goal_fac(uint32_t baseID);
OPENSMACX_API uint32_t __cdecl pop_goal(uint32_t baseID);
OPENSMACX_API BOOL __cdecl base_queue(uint32_t baseID);
OPENSMACX_API void __cdecl base_energy_costs();
OPENSMACX_API uint32_t __cdecl fac_maint(uint32_t facilityID, uint32_t factionID);
OPENSMACX_API void __cdecl base_maint();
OPENSMACX_API void __cdecl make_base_unique(uint32_t baseID);
OPENSMACX_API BOOL __cdecl has_project(uint32_t projectID, uint32_t factionID);
OPENSMACX_API BOOL __cdecl has_fac_built(uint32_t facilityID);
OPENSMACX_API BOOL __cdecl has_fac_built(uint32_t facilityID, uint32_t baseID);
OPENSMACX_API int __cdecl base_project(uint32_t project_id);
OPENSMACX_API uint32_t __cdecl attack_from(uint32_t base_id, uint32_t faction_id);
OPENSMACX_API int __cdecl value_of_base(int baseID, uint32_t factionIDReq, uint32_t factionIDRes,
	uint32_t overmatchDegree, BOOL tgl);
OPENSMACX_API uint32_t __cdecl garrison_check(uint32_t baseID);
OPENSMACX_API uint32_t __cdecl defensive_check(uint32_t baseID);
OPENSMACX_API BOOL __cdecl is_port(int baseID, BOOL isBaseRadius);
OPENSMACX_API int __cdecl vulnerable(uint32_t factionID, int xCoord, int yCoord);
OPENSMACX_API BOOL __cdecl is_objective(int baseID);
OPENSMACX_API BOOL __cdecl transcending(int factionID);
OPENSMACX_API BOOL __cdecl ascending(int factionID);
OPENSMACX_API BOOL __cdecl redundant(int facilityID, int factionID);
OPENSMACX_API BOOL __cdecl facility_avail(int facilityID, int factionID, int baseID,
	int queueCount);
OPENSMACX_API int __cdecl facility_offset(LPCSTR facilSearch);
