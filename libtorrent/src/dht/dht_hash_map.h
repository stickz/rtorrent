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

#ifndef LIBTORRENT_DHT_HASH_MAP_H
#define LIBTORRENT_DHT_HASH_MAP_H

#include "config.h"

#include <unordered_map>

#include "torrent/hash_string.h"

#include "dht_node.h"
#include "dht_tracker.h"

namespace torrent {

class DhtNodeList : public std::unordered_map<const HashString*, DhtNode*> {
public:
  using base_type = std::unordered_map<const HashString*, DhtNode*>;

  // Define accessor iterator with more convenient access to the key and
  // element values.  Allows changing the map definition more easily if needed.
  template<typename T>
  struct accessor_wrapper : public T {
    accessor_wrapper(const T& itr)
      : T(itr) {}

    const HashString& id() const {
      return *(**this).first;
    }
    DhtNode* node() const {
      return (**this).second;
    }
  };

  using const_accessor = accessor_wrapper<const_iterator>;
  using accessor       = accessor_wrapper<iterator>;

  DhtNode* add_node(DhtNode* n);
};

class DhtTrackerList : public std::unordered_map<HashString, DhtTracker*> {
public:
  using base_type = std::unordered_map<HashString, DhtTracker*>;

  template<typename T>
  struct accessor_wrapper : public T {
    accessor_wrapper(const T& itr)
      : T(itr) {}

    const HashString& id() const {
      return (**this).first;
    }
    DhtTracker* tracker() const {
      return (**this).second;
    }
  };

  using const_accessor = accessor_wrapper<const_iterator>;
  using accessor       = accessor_wrapper<iterator>;
};

inline DhtNode*
DhtNodeList::add_node(DhtNode* n) {
  emplace((const HashString*)n, (DhtNode*)n);
  return n;
}

} // namespace torrent

#endif
