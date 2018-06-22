/**
 * @file Spp/Ast/IntegerType.h
 * Contains the header of class Spp::Ast::IntegerType.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_AST_INTEGERTYPE_H
#define SPP_AST_INTEGERTYPE_H

namespace Spp::Ast
{

class IntegerType : public DataType
{
  //============================================================================
  // Type Info

  TYPE_INFO(IntegerType, DataType, "Spp.Ast", "Spp", "alusus.net");

  IMPLEMENT_AST_MAP_CLONABLE(IntegerType);

  IMPLEMENT_AST_MAP_PRINTABLE(IntegerType);


  //============================================================================
  // Member Variables

  private: mutable TioSharedPtr bitCountRef;


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(IntegerType);

  IMPLEMENT_ATTR_CONSTRUCTOR(IntegerType);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(IntegerType);


  //============================================================================
  // Member Functions

  public: Word getBitCount(Helper *helper) const;

  public: virtual Bool isEqual(Type const *type, Helper *helper, ExecutionContext const *ec) const;

  public: virtual Bool isImplicitlyCastableTo(Type const *type, Helper *helper, ExecutionContext const *ec) const;

  public: virtual Bool isExplicitlyCastableTo(Type const *type, Helper *helper, ExecutionContext const *ec) const;

}; // class

} // namespace

#endif
