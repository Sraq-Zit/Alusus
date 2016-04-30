/**
 * @file Core/Basic/TypeInfo.h
 * Contains the header of class Core::Basic::TypeInfo.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_BASIC_TYPE_INFO_H
#define CORE_BASIC_TYPE_INFO_H

namespace Core { namespace Basic
{

/**
 * @brief Contains run-time type information.
 * @ingroup basic_utils
 *
 * Classes derived from IdentifiableObject uses this object to pass the type
 * information at run time.
 */
class TypeInfo
{
  //============================================================================
  // Member Variables

  /// The name of the type without the namespace.
  private: Str typeName;

  /**
   * @brief The namespace to which the type belongs.
   *
   * This value includes the full namespace resolution, not just the immediate
   * namespace containing the type. Namespaces are separated by a dot.
   */
  private: Str typeNamespace;

  /**
   * @brief The name of the module that contains the type.
   *
   * This name can be anything that identifies the module, whether it's a
   * filename or just a title. Anything that can uniquely identify the module
   * is good enough.
   */
  private: Str moduleName;

  /**
   * @brief A URL associated with the module.
   *
   * This is used to help identify the module. It can be null (empty string).
   */
  private: Str url;

  /**
   * @brief A combination of all the other values to make up a unique name.
   *
   * This is a concatenation of the values of this type (type name,
   * namespace, module name, and url) to form a unique name that can be used
   * as a unique identifier.
   */
  private: Str uniqueName;

  /// Pointer to the type info of the base type.
  private: TypeInfo * baseTypeInfo;


  //============================================================================
  // Constructor

  /**
   * @brief Initializes the object's information.
   *
   * @param typeName The name of the type without the namespace.
   * @param typeNamespace The full namespace resolution to which the type
   *                        belongs. Namespaces should be separated by a dot.
   * @param moduleName The name of the module that contains the type. This
   *                    can be anything that identifies the module, whether
   *                    it's a filename or just a title.
   * @param url A URL associated with the module. This is used to help
   *            identify the module. It can be an empty string.
   * @param baseTypeInfo Pointer to the type info of the base type.
   */
  public: TypeInfo(Char const *typeName, Char const *typeNamespace, Char const *moduleName,
                   Char const *url, TypeInfo *baseTypeInfo) :
    typeName(typeName),
    typeNamespace(typeNamespace),
    moduleName(moduleName),
    url(url),
    baseTypeInfo(baseTypeInfo)
  {
    this->uniqueName = this->url + "#" + this->moduleName + "#" + this->typeNamespace + "." + this->typeName;
  }


  //============================================================================
  // Member Functions

  /// Get the name of the type without its namespace.
  public: Str const& getTypeName()
  {
    return this->typeName;
  }

  /**
   * @brief Get the namespace to which the type belongs.
   *
   * This value includes the full namespace resolution, not just the immediate
   * namespace containing the type. Namespaces are separated by a dot.
   */
  public: Str const& getTypeNamespace()
  {
    return this->typeNamespace;
  }

  /**
   * @brief Get the name of the module that contains the type.
   *
   * This name can be anything that identifies the module, whether it's a
   * filename or just a title. Anything that can uniquely identify the module
   * is good enough.
   */
  public: Str const& getModuleName()
  {
    return this->moduleName;
  }

  /**
   * @brief Get the URL associated with the module.
   *
   * This is used to help identify the module. It can be null (empty string).
   */
  public: Str const& getUrl()
  {
    return this->url;
  }

  /**
   * @brief Get the unique name of this type.
   *
   * This unique name is generated by concatenating the type name, namespace,
   * module name, and url.
   */
  public: Str const& getUniqueName()
  {
    return this->uniqueName;
  }

  /// Get a pointer to the type info of the base type.
  public: TypeInfo* getBaseTypeInfo()
  {
    return this->baseTypeInfo;
  }

}; // class

} } // namespace

#endif
