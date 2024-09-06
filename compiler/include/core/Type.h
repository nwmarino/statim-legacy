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
  virtual bool is_integer(void) const = 0;
  virtual bool is_float(void) const = 0;
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
  PrimitiveType(PrimitiveKind K);
  bool is_bool_evaluable(void) const override { return true; }
  bool is_null(void) const override { return false; }
  bool is_void(void) const override { return false; }

private:
  const PrimitiveKind __kind;

public:
  PrimitiveKind get_kind(void) const { return __kind; }
  bool is_integer(void) const override { return get_kind() <= __INT64; }
  bool is_float(void) const override { return get_kind() == __FP32 || get_kind() == __FP64; }
};


/// DefinedType - Represents a type defined by the source.
class DefinedType : public Type
{
public:
  virtual ~DefinedType() = default;
  bool is_bool_evaluable(void) const override { return false; }
  bool is_null(void) const override { return false; }
  bool is_void(void) const override { return false; }
  bool is_integer(void) const override { return false; }
  bool is_float(void) const override { return false; }
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
  bool is_valid_element(void) const;
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
  bool is_valid_element(void) const;
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
  const std::string get_name(void) const { return __struct_name; }
};

#endif  // TYPE_STATIMC_H
