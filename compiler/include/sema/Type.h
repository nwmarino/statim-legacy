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


};


/// ArrayType - Represents an array type.
///
/// This class represents an array type in the intermediate representation.
/// All defined array types are represented by this class.
class ArrayType final : public Type 
{
private:
  const unsigned int __size;
  const Type *__type;

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
  const Type *__type;

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


/// CharType - Represents a char type.
///
/// This class represents a char type in the intermediate representation.
/// The char type is a singular character.
class CharType final : public Type
{

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
  std::vector<Type *> fields() const;
  /// Returns the name which this type refers to.
  const std::string get_name(void) const;
};

#endif  // TYPE_STATIMC_H
