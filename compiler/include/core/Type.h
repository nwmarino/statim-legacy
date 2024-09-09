#ifndef TYPE_STATIMC_H
#define TYPE_STATIMC_H

/// Types for intermediate representations.
/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <string>

/// Type - Base class for all types.
class Type
{
public:
  virtual ~Type() = default;
  virtual bool is_bool_evaluable(void) const = 0;
  virtual bool is_null(void) const = 0;
  virtual bool is_void(void) const = 0;
  virtual bool is_bool(void) const = 0;
  virtual bool is_integer(void) const = 0;
  virtual bool is_float(void) const = 0;
  virtual bool is_builtin(void) const = 0;
  virtual bool is_matchable(void) const = 0;
  virtual std::string to_string(void) const = 0;
};


/// TypeRef - Represents a reference to a possibly undefined type.
///
/// This class represents a reference to an undefined type in the intermediate representation.
/// It is used to represent a type that is not yet defined. For example, a struct type that is not yet defined.
/// In particular, this class sees use in the parsing stage of the compiler.
class TypeRef final : public Type
{
private:
  const std::string ident;
  const Type *T;

public:
  TypeRef(const std::string &ident) : ident(ident), T(nullptr){};
  void set_type(Type *T) { this->T = T; }
  const std::string get_ident(void) const { return ident; }
  const Type *get_type(void) const { return T; }
  bool is_bool_evaluable(void) const override { return false; }
  bool is_null(void) const override { return false; }
  bool is_void(void) const override { return false; }
  bool is_bool(void) const override { return false; }
  bool is_integer(void) const override { return false; }
  bool is_float(void) const override { return false; }
  bool is_builtin(void) const override { return false; }
  bool is_matchable(void) const override { return false; }
  std::string to_string(void) const override { return ident; }
};


/// PrimitiveType - A primitive type in the language.
///
/// This class represents a primitive type in the intermediate representation.
/// All primitive types are represented by this class, for example `i32`, `bool`, etc.
class PrimitiveType final : public Type
{
public:
  enum PrimitiveKind {
    __UINT1,
    __UINT32,
    __INT32,
    __INT64,
    __FP32,
    __FP64,
    __CHAR,
  };
  bool is_bool_evaluable(void) const override { return true; }
  bool is_null(void) const override { return false; }
  bool is_void(void) const override { return false; }
  bool is_builtin(void) const override { return true; }
  bool is_matchable(void) const override { return true; }

private:
  const PrimitiveKind __kind;

public:
  PrimitiveType(PrimitiveKind K) : __kind(K){};
  bool compare(const Type *T) const {
    if (const PrimitiveType *P = dynamic_cast<const PrimitiveType *>(T)) {
      if (this->is_integer()) {
        return T->is_integer();
      } else if (this->is_float()) {
        return T->is_float();
      } else {
        return this->get_kind() == P->get_kind();
      }
    }
    return false;
  }
  PrimitiveKind get_kind(void) const { return __kind; }
  bool is_bool(void) const override { return get_kind() == __UINT1; }
  bool is_integer(void) const override { return get_kind() <= __INT64; }
  bool is_float(void) const override { return get_kind() == __FP32 || get_kind() == __FP64; }
  std::string to_string(void) const override {
    switch (get_kind()) {
      case __UINT1: return "bool";
      case __UINT32: return "u32";
      case __INT32: return "i32";
      case __INT64: return "i64";
      case __FP32: return "f32";
      case __FP64: return "f64";
      case __CHAR: return "char";
      default: return "unknown";
    }
  }
};


/// DefinedType - Represents a type defined by the source.
class DefinedType : public Type
{
public:
  virtual ~DefinedType() = default;
  bool is_bool_evaluable(void) const override { return false; }
  bool is_null(void) const override { return false; }
  bool is_void(void) const override { return false; }
  bool is_bool(void) const override { return false; }
  bool is_integer(void) const override { return false; }
  bool is_float(void) const override { return false; }
  bool is_builtin(void) const override { return false; }
  bool is_matchable(void) const override { return false; }
};


/// ArrayType - Represents an array type.
///
/// This class represents an array type in the intermediate representation.
/// All defined array types are represented by this class.
class ArrayType final : public DefinedType 
{
private:
  const unsigned int __len;
  const Type *__type;

public:
  /// @param T The element type of the array.
  ArrayType(unsigned int size, const Type *T) : __len(size), __type(T){};
  bool is_builtin(void) const override { return false; }
  bool is_valid_element(void) const;
  std::string to_string(void) const override { return __type->to_string() + "[" + std::to_string(__len) + "]"; }
};


/// RuneType - Represents a rune type.
///
/// This class represents a rune type in the intermediate representation.
/// The rune type is a pointer to an object.
class RuneType final : public DefinedType
{
private:
  const Type *__type;

public:
  /// @param T The type which the rune points to.
  RuneType(const Type *T) : __type(T){};
  bool is_builtin(void) const override { return false; }
  bool is_valid_element(void) const;
  std::string to_string(void) const override { return '#' + __type->to_string(); }
};


/// StructType - Represents a struct type.
///
/// This class represents a struct type in the intermediate representation.
/// All defined struct types are represented by this class.
class StructType final : public DefinedType
{
private:
  const std::string __struct_name;

public:
  /// @param name The name of the struct.
  StructType(const std::string &name) : __struct_name(name){};
  bool is_builtin(void) const override { return false; }
  const std::string get_name(void) const { return __struct_name; }
  std::string to_string(void) const override { return __struct_name; }
};

#endif  // TYPE_STATIMC_H
