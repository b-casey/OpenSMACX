/*
 * OpenSMACX - an open source clone of Sid Meier's Alpha Centauri.
 * Copyright (C) 2013-2019 Brendan Casey
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
enum facility_id {
	FAC_HEADQUARTERS = 0x1,
	FAC_CHILDREN_CRECHE = 0x2,
	FAC_RECYCLING_TANKS = 0x3,
	FAC_PERIMETER_DEFENSE = 0x4,
	FAC_TACHYON_FIELD = 0x5,
	FAC_RECREATION_COMMONS = 0x6,
	FAC_ENERGY_BANK = 0x7,
	FAC_NETWORK_NODE = 0x8,
	FAC_BIOLOGY_LAB = 0x9,
	FAC_SKUNKWORKS = 0xA,
	FAC_HOLOGRAM_THEATRE = 0xB,
	FAC_PARADISE_GARDEN = 0xC,
	FAC_TREE_FARM = 0xD,
	FAC_HYBRID_FOREST = 0xE,
	FAC_FUSION_LAB = 0xF,
	FAC_QUANTUM_LAB = 0x10,
	FAC_RESEARCH_HOSPITAL = 0x11,
	FAC_NANOHOSPITAL = 0x12,
	FAC_ROBOTIC_ASSEMBLY_PLANT = 0x13,
	FAC_NANOREPLICATOR = 0x14,
	FAC_QUANTUM_CONVERTER = 0x15,
	FAC_GENEJACK_FACTORY = 0x16,
	FAC_PUNISHMENT_SPHERE = 0x17,
	FAC_HAB_COMPLEX = 0x18,
	FAC_HABITATION_DOME = 0x19,
	FAC_PRESSURE_DOME = 0x1A,
	FAC_COMMAND_CENTER = 0x1B,
	FAC_NAVAL_YARD = 0x1C,
	FAC_AEROSPACE_COMPLEX = 0x1D,
	FAC_BIOENHANCEMENT_CENTER = 0x1E,
	FAC_CENTAURI_PRESERVE = 0x1F,
	FAC_TEMPLE_OF_PLANET = 0x20,
	FAC_PSI_GATE = 0x21,
	FAC_COVERT_OPS_CENTER = 0x22,
	FAC_BROOD_PIT = 0x23,
	FAC_AQUAFARM = 0x24,
	FAC_SUBSEA_TRUNKLINE = 0x25,
	FAC_THERMOCLINE_TRANSDUCER = 0x26,
	FAC_FLECHETTE_DEFENSE_SYS = 0x27,
	FAC_SUBSPACE_GENERATOR = 0x28,
	FAC_GEOSYNC_SURVEY_POD = 0x29,
	FAC_EMPTY_FACILITY_42 = 0x2A,
	FAC_EMPTY_FACILITY_43 = 0x2B,
	FAC_EMPTY_FACILITY_44 = 0x2C,
	FAC_EMPTY_FACILITY_45 = 0x2D,
	FAC_EMPTY_FACILITY_46 = 0x2E,
	FAC_EMPTY_FACILITY_47 = 0x2F,
	FAC_EMPTY_FACILITY_48 = 0x30,
	FAC_EMPTY_FACILITY_49 = 0x31,
	FAC_EMPTY_FACILITY_50 = 0x32,
	FAC_EMPTY_FACILITY_51 = 0x33,
	FAC_EMPTY_FACILITY_52 = 0x34,
	FAC_EMPTY_FACILITY_53 = 0x35,
	FAC_EMPTY_FACILITY_54 = 0x36,
	FAC_EMPTY_FACILITY_55 = 0x37,
	FAC_EMPTY_FACILITY_56 = 0x38,
	FAC_EMPTY_FACILITY_57 = 0x39,
	FAC_EMPTY_FACILITY_58 = 0x3A,
	FAC_EMPTY_FACILITY_59 = 0x3B,
	FAC_EMPTY_FACILITY_60 = 0x3C,
	FAC_EMPTY_FACILITY_61 = 0x3D,
	FAC_EMPTY_FACILITY_62 = 0x3E,
	FAC_EMPTY_FACILITY_63 = 0x3F,
	FAC_EMPTY_FACILITY_64 = 0x40,
	FAC_SKY_HYDRO_LAB = 0x41,
	FAC_NESSUS_MINING_STATION = 0x42,
	FAC_ORBITAL_POWER_TRANS = 0x43,
	FAC_ORBITAL_DEFENSE_POD = 0x44,
	FAC_STOCKPILE_ENERGY = 0x45,
	FAC_HUMAN_GENOME_PROJ = 0x46,
	FAC_COMMAND_NEXUS = 0x47,
	FAC_WEATHER_PARADIGM = 0x48,
	FAC_MERCHANT_EXCHANGE = 0x49,
	FAC_EMPATH_GUILD = 0x4A,
	FAC_CITIZENS_DEFENSE_FORCE = 0x4B,
	FAC_VIRTUAL_WORLD = 0x4C,
	FAC_PLANETARY_TRANS_SYS = 0x4D,
	FAC_XENOEMPATYH_DOME = 0x4E,
	FAC_NEURAL_AMPLIFIER = 0x4F,
	FAC_MARITIME_CONTROL_CENTER = 0x50,
	FAC_PLANETARY_DATALINKS = 0x51,
	FAC_SUPERCOLLIDER = 0x52,
	FAC_ASCETIC_VIRTUES = 0x53,
	FAC_LONGEVITY_VACCINE = 0x54,
	FAC_HUNTER_SEEKER_ALGO = 0x55,
	FAC_PHOLUS_MUTAGEN = 0x56,
	FAC_CYBORG_FACTORY = 0x57,
	FAC_THEORY_OF_EVERYTHING = 0x58,
	FAC_DREAM_TWISTER = 0x59,
	FAC_UNIVERSAL_TRANSLATOR = 0x5A,
	FAC_NETWORK_BACKBONE = 0x5B,
	FAC_NANO_FACTORY = 0x5C,
	FAC_LIVING_REFINERY = 0x5D,
	FAC_CLONING_VATS = 0x5E,
	FAC_SELF_AWARE_COLONY = 0x5F,
	FAC_CLINICAL_IMMORTALITY = 0x60,
	FAC_SPACE_ELEVATOR = 0x61,
	FAC_SINGULARITY_INDUCTOR = 0x62,
	FAC_BULK_MATTER_TRANSMITTER = 0x63,
	FAC_TELEPATHIC_MATRIX = 0x64,
	FAC_VOICE_OF_PLANET = 0x65,
	FAC_ASCENT_TO_TRANSCENDENCE = 0x66,
	FAC_MANIFOLD_HARMONICS = 0x67,
	FAC_NETHACK_TERMINUS = 0x68,
	FAC_CLOUDBASE_ACADEMY = 0x69,
	FAC_PLANETARY_ENERGY_GRID = 0x6A,
	FAC_EMPTY_SP_38 = 0x6B,
	FAC_EMPTY_SP_39 = 0x6C,
	FAC_EMPTY_SP_40 = 0x6D,
	FAC_EMPTY_SP_41 = 0x6E,
	FAC_EMPTY_SP_42 = 0x6F,
	FAC_EMPTY_SP_43 = 0x70,
	FAC_EMPTY_SP_44 = 0x71,
	FAC_EMPTY_SP_45 = 0x72,
	FAC_EMPTY_SP_46 = 0x73,
	FAC_EMPTY_SP_47 = 0x74,
	FAC_EMPTY_SP_48 = 0x75,
	FAC_EMPTY_SP_49 = 0x76,
	FAC_EMPTY_SP_50 = 0x77,
	FAC_EMPTY_SP_51 = 0x78,
	FAC_EMPTY_SP_52 = 0x79,
	FAC_EMPTY_SP_53 = 0x7A,
	FAC_EMPTY_SP_54 = 0x7B,
	FAC_EMPTY_SP_55 = 0x7C,
	FAC_EMPTY_SP_56 = 0x7D,
	FAC_EMPTY_SP_57 = 0x7E,
	FAC_EMPTY_SP_58 = 0x7F,
	FAC_EMPTY_SP_59 = 0x80,
	FAC_EMPTY_SP_60 = 0x81,
	FAC_EMPTY_SP_61 = 0x82,
	FAC_EMPTY_SP_62 = 0x83,
	FAC_EMPTY_SP_63 = 0x84,
	FAC_EMPTY_SP_64 = 0x85,
};

struct base {
	__int16 xCoord;
	__int16 yCoord;
	char factionIDCurrent;
	char factionIDFormer;
	char populationSize;
	char assimilationTurnsLeft;
	char nerveStapleTurnsLeft;
	char unk1;
	char unk2;
	char factionPopulationSizeIntel[8];
	char nameString[25];
	__int16 unkX;
	__int16 unkY;
	int statusBitfield;
	int eventBitfield;
	int governorBitfield;
	int nutrientsAccumulated;
	int mineralsAccumulated;
	int productionIDLast;
	int ecoDamage;
	int queueSize;
	int queueProductionID[10];
	int resourceSquaresBitfield;
	int specialistTotal;
	int unk3;
	int unk4;
	int unk5;
	char facilitiesPresentTable[12];
	int mineralSurplusFinal;
	int mineralsAccumulated2;
	int unk6;
	int unk7;
	int unk8;
	int unk9;
	int nutrientIntake1;
	int mineralIntake1;
	int energyIntake1;
	int unusedIntake1;
	int nutrientIntake;
	int mineralIntake;
	int energyIntake;
	int unusedIntake;
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
	int unk10;
	__int16 autoforwardLandBaseID;
	__int16 autoforwardSeaBaseID;
	__int16 autoforwardAirBaseID;
	__int16 padding0;
	int talentTotal;
	int droneTotal;
	int superdroneTotal;
	int randomEventTurns;
	int nerveStapleCount;
	int padding1;
	int padding2;
};

struct rules_facility {
	LPSTR name;
	LPSTR effect;
	DWORD pad;
	int cost;
	int maint;
	int preqTech;
	int freeTech;
	int SP_AIFight;
	int SP_AIGrowth;
	int SP_AITech;
	int SP_AIWealth;
	int SP_AIPower;
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
constexpr int MaxFacilityNum = 134;
constexpr int FacilitySPStart = 70; // Special Project start offset
constexpr int MaxCitizenNum = 10;
constexpr int SP_Unbuilt = -1;
constexpr int SP_Destroyed = -2;

extern rules_facility *Facility;
extern rules_citizen *Citizen;
extern base *Base;
extern base_secret_project *SecretProject;

OPENSMACX_API BOOL __cdecl has_project(DWORD projectID, DWORD factionID);