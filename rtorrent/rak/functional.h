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

#ifndef RAK_FUNCTIONAL_H
#define RAK_FUNCTIONAL_H

#include <cstddef>
#include <functional>

namespace rak {

template <typename Type>
struct reference_fix {
  typedef Type type;
};

template <typename Type>
struct reference_fix<Type&> {
  typedef Type type;
};

template <typename Type>
struct value_t {
  value_t(Type v) : m_v(v) {}

  Type operator () () const { return m_v; }

  Type m_v;
};

template <typename Type>
inline value_t<Type>
value(Type v) {
  return value_t<Type>(v);
}

// Operators:

template <typename Type, typename Ftor>
struct equal_t {
  typedef bool result_type;

  equal_t(Type t, Ftor f) : m_t(t), m_f(f) {}

  template <typename Arg>
  bool operator () (Arg& a) {
    return m_t == m_f(a);
  }

  Type m_t;
  Ftor m_f;
};

template <typename Type, typename Ftor>
inline equal_t<Type, Ftor>
equal(Type t, Ftor f) {
  return equal_t<Type, Ftor>(t, f);
}

template <typename Type, typename Ftor>
struct less_t {
  typedef bool result_type;

  less_t(Type t, Ftor f) : m_t(t), m_f(f) {}

  template <typename Arg>
  bool operator () (Arg& a) {
    return m_t < m_f(a);
  }

  Type m_t;
  Ftor m_f;
};

template <typename Type, typename Ftor>
inline less_t<Type, Ftor>
less(Type t, Ftor f) {
  return less_t<Type, Ftor>(t, f);
}

template <typename FtorA, typename FtorB>
struct less2_t : public std::binary_function<typename FtorA::argument_type, typename FtorB::argument_type, bool> {
  less2_t(FtorA f_a, FtorB f_b) : m_f_a(f_a), m_f_b(f_b) {}

  bool operator () (typename FtorA::argument_type a, typename FtorB::argument_type b) {
    return m_f_a(a) < m_f_b(b);
  }

  FtorA m_f_a;
  FtorB m_f_b;
};

template <typename FtorA, typename FtorB>
inline less2_t<FtorA, FtorB>
less2(FtorA f_a, FtorB f_b) {
  return less2_t<FtorA,FtorB>(f_a,f_b);
}

template <typename Type, typename Ftor>
struct less_equal_t {
  typedef bool result_type;

  less_equal_t(Type t, Ftor f) : m_t(t), m_f(f) {}

  template <typename Arg>
  bool operator () (Arg& a) {
    return m_t <= m_f(a);
  }

  Type m_t;
  Ftor m_f;
};

template <typename Type, typename Ftor>
inline less_equal_t<Type, Ftor>
less_equal(Type t, Ftor f) {
  return less_equal_t<Type, Ftor>(t, f);
}

template<typename Tp>
struct invert : public std::unary_function<Tp, Tp> {
  Tp
  operator () (const Tp& x) const { return ~x; }
};

template <typename Src, typename Dest>
struct on_t : public std::unary_function<typename Src::argument_type, typename Dest::result_type> {
  typedef typename Dest::result_type result_type;

  on_t(Src s, Dest d) : m_dest(d), m_src(s) {}

  result_type operator () (typename reference_fix<typename Src::argument_type>::type arg) {
    return m_dest(m_src(arg));
  }

  Dest m_dest;
  Src m_src;
};
    
template <typename Src, typename Dest>
inline on_t<Src, Dest>
on(Src s, Dest d) {
  return on_t<Src, Dest>(s, d);
}  

// Creates a functor for accessing a member.
template <typename Class, typename Member>
struct mem_ptr_t : public std::unary_function<Class*, Member&> {
  mem_ptr_t(Member Class::*m) : m_member(m) {}

  Member& operator () (Class* c) {
    return c->*m_member;
  }

  const Member& operator () (const Class* c) {
    return c->*m_member;
  }

  Member Class::*m_member;
};

template <typename Class, typename Member>
inline mem_ptr_t<Class, Member>
mem_ptr(Member Class::*m) {
  return mem_ptr_t<Class, Member>(m);
}

template <typename Class, typename Member>
struct mem_ref_t : public std::unary_function<Class&, Member&> {
  mem_ref_t(Member Class::*m) : m_member(m) {}

  Member& operator () (Class& c) {
    return c.*m_member;
  }

  Member Class::*m_member;
};

template <typename Class, typename Member>
struct const_mem_ref_t : public std::unary_function<const Class&, const Member&> {
  const_mem_ref_t(const Member Class::*m) : m_member(m) {}

  const Member& operator () (const Class& c) {
    return c.*m_member;
  }

  const Member Class::*m_member;
};

template <typename Class, typename Member>
inline mem_ref_t<Class, Member>
mem_ref(Member Class::*m) {
  return mem_ref_t<Class, Member>(m);
}

template <typename Class, typename Member>
inline const_mem_ref_t<Class, Member>
const_mem_ref(const Member Class::*m) {
  return const_mem_ref_t<Class, Member>(m);
}

template <typename Operation>
class bind1st_t : public std::unary_function<typename Operation::second_argument_type, typename Operation::result_type> {
public:
  typedef typename reference_fix<typename Operation::first_argument_type>::type  value_type;
  typedef typename reference_fix<typename Operation::second_argument_type>::type argument_type;

  bind1st_t(const Operation& op, const value_type v) :
    m_op(op), m_value(v) {}

  typename Operation::result_type
  operator () (const argument_type arg) {
    return m_op(m_value, arg);
  }

protected:
  Operation  m_op;
  value_type m_value;
};

template <typename Operation, typename Type>
inline bind1st_t<Operation>
bind1st(const Operation& op, const Type& val) {
  return bind1st_t<Operation>(op, val);
}

template <typename Operation>
class bind2nd_t : public std::unary_function<typename Operation::first_argument_type, typename Operation::result_type> {
public:
  typedef typename reference_fix<typename Operation::first_argument_type>::type argument_type;
  typedef typename reference_fix<typename Operation::second_argument_type>::type value_type;

  bind2nd_t(const Operation& op, const value_type v) :
    m_op(op), m_value(v) {}

  typename Operation::result_type
  operator () (argument_type arg) {
    return m_op(arg, m_value);
  }

protected:
  Operation  m_op;
  value_type m_value;
};

template <typename Operation, typename Type>
inline bind2nd_t<Operation>
bind2nd(const Operation& op, const Type& val) {
  return bind2nd_t<Operation>(op, val);
}

}

#endif
