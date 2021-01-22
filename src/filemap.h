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
  * FileMap class: Handles file IO operations.
  */
class DLLEXPORT Filemap {
 public:
  Filemap() // 00628380
      : map_view_addr_(NULL), file_(INVALID_HANDLE_VALUE), file_map_(NULL), file_size_(0) { }
  ~Filemap() { close(); }  // 006283E0

  Filemap *init(LPCSTR file_name, BOOL is_sequential);
  Filemap *init(LPCSTR file_name);
  LPVOID open_read(LPCSTR file_name, BOOL is_sequential);
  LPVOID open(LPCSTR file_name, BOOL is_sequential);
  LPVOID create(LPCSTR file_name, uint32_t size, BOOL is_sequential);
  void close();
  void close(LPVOID new_addr);
  // additional functions to assist with encapsulation
  uint32_t get_size() { return file_size_; }

 private:
  LPVOID map_view_addr_; // (+0)
  HANDLE file_;          // (+4)
  HANDLE file_map_;      // (+8)
  uint32_t file_size_;   // (+12)
};
