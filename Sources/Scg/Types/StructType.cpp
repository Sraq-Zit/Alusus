/**
* @file Scg/Types/StructType.cpp
*
* @copyright Copyright (C) 2016 Rafid Khalid Abdullah
*
* @license This file is released under Alusus Public License, Version 1.0.
* For details on usage and copying conditions read the full license in the
* accompanying license file or at <http://alusus.net/alusus_license_1_0>.
*/
//==============================================================================

#include <prerequisites.h>

// Scg header files
#include <Types/StructType.h>

namespace Scg
{

//==============================================================================
// Constructors & Destructor

StructType::StructType(const std::string &name) : name(name)
{
  this->typeSpec = std::make_shared<ValueTypeSpecByName>(name.c_str());
  // We don't delete a structure type we create as it is defined using
  // LLVM's BumpPtrAllocator.
  this->llvmType = llvm::StructType::create(LlvmContainer::getContext(), this->name.c_str());
}


//==============================================================================
// Member Functions

void StructType::setFields(ValueTypeNameArray const &fields)
{
  this->fields = fields;
  std::vector<llvm::Type *> llvmFields;

  for (auto field : this->fields)
    llvmFields.push_back(field.getValueType()->getLlvmType());

  static_cast<llvm::StructType *>(this->llvmType)->setBody(llvmFields);
}


void StructType::initCastingTargets() const
{
  this->implicitCastingTargets.push_back(this);

  this->explicitCastingTargets.push_back(this);
}


bool StructType::isEqualTo(ValueType const *other) const
{
  auto otherAsStruct = dynamic_cast<const StructType *>(other);

  if (otherAsStruct == nullptr) {
    return false;
  }

  // TODO: For now we are just comparing the name. Later
  return otherAsStruct->getName().compare(otherAsStruct->getName()) == 0;
}

} // namespace
