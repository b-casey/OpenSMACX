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
  * Heap class: Handles managing or allocating memory.
  */
class DLLEXPORT Heap {
 public:
  Heap() : err_flags_(0), base_(0), current_(0), base_size_(0), free_size_(0) { } // 005D4560
  ~Heap() { shutdown(); }                                                         // 005D4580

  void shutdown();
  void squeeze(int toggle);
  BOOL init(size_t req_size);
  LPVOID get(size_t req_size);
  // additional functions to assist with encapsulation
  size_t get_base_size() { return base_size_; } // Get base total memory size
  LPVOID get_base() { return base_; }   // Get base memory address pointer

 private:
  int8_t err_flags_; // (+0) -> error toggle
  LPVOID base_;      // (+4) -> pointer to base memory address
  LPVOID current_;   // (+8) -> current memory address position
  size_t base_size_; // (+12) -> size of total memory
  size_t free_size_; // (+16) -> size of free available memory
};
