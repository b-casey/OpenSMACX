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
#include "stdafx.h"
#include "base.h"

rules_facility *Facility = (rules_facility *)0x009A4B68;
rules_citizen *Citizen = (rules_citizen *)0x00946020;
Base *Bases = (Base *)0x0097D040; // 512

// Secret Projects > stores BaseID that built SP
int *SP_HumanGenomeProject = (int *)0x009A6514;
int *SP_CommandNexus = (int *)0x009A6518;
int *SP_WeatherParadigm = (int *)0x009A651C;
int *SP_MerchantExchange = (int *)0x009A6520;
int *SP_EmpathGuild = (int *)0x009A6524;
int *SP_CitizensDefenseForce = (int *)0x009A6528;
int *SP_VirtualWorld = (int *)0x009A652C;
int *SP_PlanetaryTransitSystem = (int *)0x009A6530;
int *SP_XenoempathyDome = (int *)0x009A6534;
int *SP_NeuralAmplifier = (int *)0x009A6538;
int *SP_MaritimeControlCenter = (int *)0x009A653C;
int *SP_PlanetaryDatalinks = (int *)0x009A6540;
int *SP_Supercollider = (int *)0x009A6544;
int *SP_AsceticVirtues = (int *)0x009A6548;
int *SP_LongevityVaccine = (int *)0x009A654C;
int *SP_HunterSeekerAlgorithm = (int *)0x009A6550;
int *SP_PholusMutagen = (int *)0x009A6554;
int *SP_CyborgFactory = (int *)0x009A6558;
int *SP_TheoryOfEverything = (int *)0x009A655C;
int *SP_DreamTwister = (int *)0x009A6560;
int *SP_UniversalTranslator = (int *)0x009A6564;
int *SP_NetworkBackbone = (int *)0x009A6568;
int *SP_NanoFactory = (int *)0x009A656C;
int *SP_LivingRefinery = (int *)0x009A6570;
int *SP_CloningVats = (int *)0x009A6574;
int *SP_SelfAwareColony = (int *)0x009A6578;
int *SP_ClinicalImmortality = (int *)0x009A657C;
int *SP_SpaceElevator = (int *)0x009A6580;
int *SP_SingularityInductor = (int *)0x009A6584;
int *SP_BulkMatterTransmitter = (int *)0x009A6588;
int *SP_TelepathicMatrix = (int *)0x009A658C;
int *SP_VoiceOfPlanet = (int *)0x009A6590;
int *SP_AscentToTranscendence = (int *)0x009A6594;
int *SP_ManifoldHarmonics = (int *)0x009A6598; // SMACX Only
int *SP_NethackTerminus = (int *)0x009A659C; // SMACX Only
int *SP_CloudbaseAcademy = (int *)0x009A65A0; // SMACX Only
int *SP_PlanetaryEnergyGrid = (int *)0x009A65A4; // SMACX Only
int *SP_UserDefined = (int *)0x009A65A8; // [27], unreferenced in code?