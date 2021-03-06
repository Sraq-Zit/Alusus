/**
 * @file Core/Basic/SharedMap.h
 * Contains the header of class Core::Basic::SharedMap.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_BASIC_SHAREDMAP_H
#define CORE_BASIC_SHAREDMAP_H

namespace Core::Basic
{

template<class CTYPE> class SharedMap : public SharedMapBase<CTYPE, TiObject>
{
  //============================================================================
  // Type Info

  typedef SharedMapBase<CTYPE, TiObject> _MyBase;
  TEMPLATE_TYPE_INFO(SharedMap, _MyBase, "Core.Basic", "Core", "alusus.org", (CTYPE));


  //============================================================================
  // Constructors & Destructor

  public: SharedMap(Bool useIndex = false) : _MyBase(useIndex)
  {
  }

  public: SharedMap(std::initializer_list<Argument> const &args, Bool useIndex = false) : _MyBase(useIndex)
  {
    this->add(args);
  }

  public: virtual ~SharedMap()
  {
    this->destruct();
  }

  public: static SharedPtr<SharedMap<CTYPE>> create(
    std::initializer_list<Argument> const &args, Bool useIndex = false
  ) {
    return std::make_shared<SharedMap<CTYPE>>(args, useIndex);
  }


  //============================================================================
  // Member Functions

  /// @name Abstract Functions Implementation
  /// @{

  protected: virtual SharedPtr<CTYPE> prepareForSet(
    Char const *key, Int index, SharedPtr<CTYPE> const &obj, Bool inherited, Bool newEntry
  ) {
    return obj;
  }

  protected: virtual void finalizeSet(
    Char const *key, Int index, SharedPtr<CTYPE> const &obj, Bool inherited, Bool newEntry
  ) {
  }

  protected: virtual void prepareForUnset(
    Char const *key, Int index, SharedPtr<CTYPE> const &obj, Bool inherited
  ) {
  }

  /// @}

  /// @name Inheritted Functions
  /// @{

  public: void setBase(SharedMap<CTYPE> *b)
  {
    SharedMapBase<CTYPE, TiObject>::setBase(b);
  }

  public: SharedMap<CTYPE>* getBase() const
  {
    return static_cast<SharedMap<CTYPE>*>(this->base);
  }

  /// @}

}; // class

} // namespace

#endif
