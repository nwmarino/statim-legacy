#ifndef TYPE_STATIMC_H
#define TYPE_STATIMC_H

/// Types for intermediate representations.
/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <string>
#include <vector>

/// Type - Base class for all types.
///
/// This class is the base class for all types in the intermediate representation.
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


/// BuiltinType - A primitive type in the language.
///
/// This class represents a primitive type in the intermediate representation.
/// All primitive types are represented by this class, for example `i32`, `bool`, etc.
class BuiltinType : public Type
{

};


/// RefType - Represents a reference to a possibly (un)defined type.
///
/// This class represents a reference to a type in the intermediate representation.
/// It is used as a stepping stone between parsing and semantic analysis, since types
/// cannot be guaranteed to exist in scope at parse time.
class RefType final : public Type
{
private:
  const std::string __ident;

public:
  /// @param ident The identifier of the type.
  RefType(const std::string &ident);1

  /// Returns the name which this reference type refers to.
  const std::string get_name(void) const;
};


/// ArrayType - Represents an array type.
///
/// This class represents an array type in the intermediate representation.
/// All defined array types are represented by this class.
class ArrayType final : public Type 
{
private:
  const unsigned int __size;
  const Type *__elem_type;

public:
  /// @param T The element type of the array.
  ArrayType(unsigned int size, const Type *T);

  /// Returns true if the element type is valid.
  bool is_valid_element(void) const;
};


/// RuneType - Represents a rune type.
///
/// This class represents a rune type in the intermediate representation.
/// The rune type is a pointer to an object.
class RuneType final : public Type
{
private:
  const Type *__elem_type;

public:
  /// @param T The type which the rune points to.
  RuneType(const Type *T);

  /// Returns true if the element type is valid.
  bool is_valid_element(void) const;
};


/// IntegerType - Represents an integer type.
///
/// This class represents an integer type in the intermediate representation.
/// All primitive integer types are represented by this class.
class IntegerType final : public Type
{
private:
  const unsigned int __width;

public:
  /// @param width The bit width of the integer type.
  IntegerType(unsigned int width);

  /// Returns the bit width of this type.
  unsigned int bit_width(void) const;
};


/// StructType - Represents a struct type.
///
/// This class represents a struct type in the intermediate representation.
/// All defined struct types are represented by this class.
class StructType final : public Type
{
private:
  const std::string __struct_name;

public:

  /// Returns the name which this type refers to.
  const std::string get_name(void) const;
};

#endif  // TYPE_STATIMC_H
