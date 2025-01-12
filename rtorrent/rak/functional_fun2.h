// rak - Rakshasa's toolbox
// Copyright (C) 2005-2007, Jari Sundell
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

#ifndef RAK_FUNCTIONAL_FUN2_H
#define RAK_FUNCTIONAL_FUN2_H

#include <cstddef>
#include <functional>

namespace rak {

// Lightweight callback function including pointer to object. Should
// be replaced by TR1 stuff later. Requires an object to bind, instead
// of using a seperate functor for that.

template <typename Ret>
class ptr_fun0 {
public:
  typedef Ret result_type;
  typedef Ret (*Function)();

  ptr_fun0() {}
  ptr_fun0(Function f) : m_function(f) {}

  bool is_valid() const { return m_function; }

  Ret operator () () { return m_function(); }
  
private:
  Function m_function;
};

template <typename Object, typename Ret>
class mem_fun0 {
public:
  typedef Ret result_type;
  typedef Ret (Object::*Function)();

  mem_fun0() : m_object(NULL) {}
  mem_fun0(Object* o, Function f) : m_object(o), m_function(f) {}

  bool is_valid() const { return m_object; }

  Ret operator () () { return (m_object->*m_function)(); }
  
private:
  Object* m_object;
  Function m_function;
};

template <typename Object, typename Ret>
class const_mem_fun0 {
public:
  typedef Ret result_type;
  typedef Ret (Object::*Function)() const;

  const_mem_fun0() : m_object(NULL) {}
  const_mem_fun0(const Object* o, Function f) : m_object(o), m_function(f) {}

  bool is_valid() const { return m_object; }

  Ret operator () () const { return (m_object->*m_function)(); }
  
private:
  const Object* m_object;
  Function      m_function;
};

template <typename Object, typename Ret, typename Arg1>
class mem_fun1 {
public:
  typedef Ret result_type;
  typedef Ret (Object::*Function)(Arg1);

  mem_fun1() : m_object(NULL) {}
  mem_fun1(Object* o, Function f) : m_object(o), m_function(f) {}

  bool is_valid() const { return m_object; }

  Ret operator () (Arg1 a1) { return (m_object->*m_function)(a1); }
  
private:
  Object* m_object;
  Function m_function;
};

template <typename Object, typename Ret, typename Arg1>
class const_mem_fun1 {
public:
  typedef Ret result_type;
  typedef Ret (Object::*Function)(Arg1) const;

  const_mem_fun1() : m_object(NULL) {}
  const_mem_fun1(const Object* o, Function f) : m_object(o), m_function(f) {}

  bool is_valid() const { return m_object; }

  Ret operator () (Arg1 a1) const { return (m_object->*m_function)(a1); }
  
private:
  const Object* m_object;
  Function      m_function;
};

template <typename Object, typename Ret, typename Arg1, typename Arg2>
class mem_fun2 : public std::binary_function<Arg1, Arg2, Ret> {
public:
  typedef Ret result_type;
  typedef Ret (Object::*Function)(Arg1, Arg2);
  typedef Object object_type;

  mem_fun2() : m_object(NULL) {}
  mem_fun2(Object* o, Function f) : m_object(o), m_function(f) {}

  bool         is_valid() const { return m_object; }

  object_type*       object() { return m_object; }
  const object_type* object() const { return m_object; }

  Ret          operator () (Arg1 a1, Arg2 a2) { return (m_object->*m_function)(a1, a2); }
  
private:
  Object* m_object;
  Function m_function;
};

template <typename Object, typename Ret, typename Arg1, typename Arg2, typename Arg3>
class mem_fun3 {
public:
  typedef Ret result_type;
  typedef Ret (Object::*Function)(Arg1, Arg2, Arg3);

  mem_fun3() : m_object(NULL) {}
  mem_fun3(Object* o, Function f) : m_object(o), m_function(f) {}

  bool is_valid() const { return m_object; }

  Ret operator () (Arg1 a1, Arg2 a2, Arg3 a3) { return (m_object->*m_function)(a1, a2, a3); }
  
private:
  Object* m_object;
  Function m_function;
};

template <typename Object, typename Ret>
inline mem_fun0<Object, Ret>
make_mem_fun(Object* o, Ret (Object::*f)()) {
 return mem_fun0<Object, Ret>(o, f);
}

template <typename Object, typename Ret>
inline const_mem_fun0<Object, Ret>
make_mem_fun(const Object* o, Ret (Object::*f)() const) {
 return const_mem_fun0<Object, Ret>(o, f);
}

template <typename Object, typename Ret, typename Arg1>
inline mem_fun1<Object, Ret, Arg1>
make_mem_fun(Object* o, Ret (Object::*f)(Arg1)) {
 return mem_fun1<Object, Ret, Arg1>(o, f);
}

template <typename Object, typename Ret, typename Arg1>
inline const_mem_fun1<Object, Ret, Arg1>
make_mem_fun(const Object* o, Ret (Object::*f)(Arg1) const) {
 return const_mem_fun1<Object, Ret, Arg1>(o, f);
}

template <typename Object, typename Ret, typename Arg1, typename Arg2>
inline mem_fun2<Object, Ret, Arg1, Arg2>
make_mem_fun(Object* o, Ret (Object::*f)(Arg1, Arg2)) {
 return mem_fun2<Object, Ret, Arg1, Arg2>(o, f);
}

template <typename Object, typename Ret, typename Arg1, typename Arg2, typename Arg3>
inline mem_fun3<Object, Ret, Arg1, Arg2, Arg3>
make_mem_fun(Object* o, Ret (Object::*f)(Arg1, Arg2, Arg3)) {
 return mem_fun3<Object, Ret, Arg1, Arg2, Arg3>(o, f);
}

}

#endif
