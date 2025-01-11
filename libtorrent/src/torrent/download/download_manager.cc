// libTorrent - BitTorrent library
// Copyright (C) 2005-2011, Jari Sundell
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// In addition, as a special exception, the copyright holders give
// permission to link the code of portions of this program with the
// OpenSSL library under certain conditions as described in each
// individual source file, and distribute linked combinations
// including the two.
//
// You must obey the GNU General Public License in all respects for
// all of the code used other than OpenSSL.  If you modify file(s)
// with this exception, you may extend this exception to your version
// of the file(s), but you are not obligated to do so.  If you do not
// wish to do so, delete this exception statement from your version.
// If you delete this exception statement from all source files in the
// program, then also delete it here.
//
// Contact:  Jari Sundell <jaris@ifi.uio.no>
//
//           Skomakerveien 33
//           3185 Skoppum, NORWAY

#include "config.h"

#include "torrent/exceptions.h"

#include "download/download_wrapper.h"
#include "download_manager.h"

namespace torrent {

DownloadManager::iterator
DownloadManager::insert(DownloadWrapper* d) {
  if (find(d->info()->hash()) != end())
    throw internal_error("Could not add torrent as it already exists.");

  lookup_cache.emplace(d->info()->hash(), size());
  obfuscated_to_hash.emplace(d->info()->hash_obfuscated(), d->info()->hash());

  return base_type::insert(end(), d);
}

DownloadManager::iterator
DownloadManager::erase(DownloadWrapper* d) {
  auto itr = find(d->info()->hash());

  if (itr == end())
    throw internal_error("Tried to remove a torrent that doesn't exist");

  lookup_cache.erase(lookup_cache.find(d->info()->hash()));
  obfuscated_to_hash.erase(obfuscated_to_hash.find(d->info()->hash_obfuscated()));
    
  delete *itr;
  return base_type::erase(itr);
}

void
DownloadManager::clear() {
  base_type::clear();
  lookup_cache.clear();
  obfuscated_to_hash.clear();
}

DownloadManager::iterator
DownloadManager::find(const std::string& hash) {
  return find(*HashString::cast_from(hash));
}

DownloadManager::iterator
DownloadManager::find(const HashString& hash) {
  auto cached = lookup_cache.find(hash);

  if (cached == lookup_cache.end()) {
    return end();
  }

  auto cached_i = cached->second;

  auto itr = cached_i < size() ? begin() + cached_i : end();
  if (itr == end() || (*itr)->info()->hash() != hash) {
    itr = std::find_if(begin(), end(), [hash](DownloadWrapper* wrapper) {
      return hash == wrapper->info()->hash();
    });
  }

  lookup_cache[hash] = itr - begin();

  return itr;
}

DownloadManager::iterator
DownloadManager::find(DownloadInfo* info) {
  return std::find_if(begin(), end(), [info](const auto& wrapper){ return info == wrapper->info(); });
}

DownloadManager::iterator
DownloadManager::find_chunk_list(ChunkList* cl) {
  return std::find_if(begin(), end(), [cl](const auto& wrapper){ return cl == wrapper->chunk_list(); });
}

DownloadMain*
DownloadManager::find_main(const char* hash) {
  auto itr = find(*HashString::cast_from(hash));

  if (itr == end()) {
    return nullptr;
  }

  return (*itr)->main();
}

DownloadMain*
DownloadManager::find_main_obfuscated(const char* obfuscated) {
  auto hash_itr = obfuscated_to_hash.find(*HashString::cast_from(obfuscated));

  if (hash_itr == obfuscated_to_hash.end()) {
    return nullptr;
  }

  auto itr = find(hash_itr->second);

  if (itr == end()) {
    return nullptr;
  }

  return (*itr)->main();
}

}
