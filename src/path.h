/*
 * OpenSMACX - an open source clone of Sid Meier's Alpha Centauri.
 * Copyright (C) 2013-2021 Brendan Casey
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
  * Path related objects, variables and functions.
  */
class DLLEXPORT Path {
 public:
  Path() : map_table_(0), x_table_(0), y_table_(0), index1_(0), index2_(0), faction_id_(0), 
           x_dst_(0), y_dst_(0), unk_(5), faction_id_2_(0), proto_id_(0) { } // n/a
  ~Path() { shutdown(); } // 0059A320

  void init();
  void shutdown();
  int get(uint32_t x, uint32_t y);
  void set(uint32_t x, uint32_t y, int val);
  int zoc_path(uint32_t x, uint32_t y, uint32_t faction_id);
  int find(int x_src, int y_src, int x_dst, int y_dst, int proto_id, int faction_id, int unk1, 
           int unk2);
  int move(uint32_t veh_id, int faction_id);
  void make_abstract();
  void merge(uint32_t region_old, uint32_t region_new);
  void territory(uint32_t x, uint32_t y, int region, int faction_id);
  void continent(uint32_t x, uint32_t y, uint32_t region);
  void continents();
  BOOL sensors(uint32_t faction_id, int *x_sensor, int *y_sensor);

 private:
  int *map_table_;
  int16_t *x_table_;
  int16_t *y_table_;
  int index1_; // specific territory count
  int index2_; // overall territory count
  int faction_id_;
  int x_dst_;
  int y_dst_;
  int unk_;
  int faction_id_2_;
  int proto_id_;
};

// global
extern Path *Paths;
