/**
 * @file Spp/SeekerExtension.h
 * Contains the header of class Spp::SeekerExtension.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_SEEKEREXTENSION_H
#define SPP_SEEKEREXTENSION_H

namespace Spp
{

class SeekerExtension : public ObjTiInterface
{
  //============================================================================
  // Type Info

  OBJ_INTERFACE_INFO(SeekerExtension, ObjTiInterface, "Spp", "Spp", "alusus.org");


  //============================================================================
  // Types

  public: struct Overrides
  {
    TiFunctionBase *foreachRef;
    TiFunctionBase *foreachByIdentifier_levelRef;
    TiFunctionBase *foreachByIdentifier_functionRef;
    TiFunctionBase *foreachByIdentifier_dataTypeRef;
    TiFunctionBase *foreachByParamPassRef;
    TiFunctionBase *foreachByParamPass_routingRef;
    TiFunctionBase *foreachByParamPass_templateRef;
    TiFunctionBase *foreachByThisTypeRefRef;
  };

  public: s_enum(Flags, SKIP_USES = (1<<16), SKIP_OWNERS_AND_USES = (1<<16) | 1);


  //============================================================================
  // Member Variables

  private: TiObject *owner;


  //============================================================================
  // Constructor

  public: SeekerExtension(TiObject *o) : owner(o)
  {
    Basic::initBindingCaches(this->owner, {
      &this->astHelper,
      &this->foreachByIdentifier_function,
      &this->foreachByIdentifier_dataType,
      &this->foreachByParamPass,
      &this->foreachByParamPass_routing,
      &this->foreachByParamPass_template,
      &this->foreachByThisTypeRef
    });
  }


  //============================================================================
  // Member Properties

  public: BINDING_CACHE(astHelper, Ast::Helper);


  //============================================================================
  // Member Functions

  /// @name ObjTiInterface Implementation
  /// @{

  public: virtual TiObject* getTiObject()
  {
    return this->owner;
  }

  public: virtual TiObject const* getTiObject() const
  {
    return this->owner;
  }

  /// @}

  /// @name Setup Functions
  /// @{

  public: static Overrides* extend(Core::Data::Seeker *seeker, SharedPtr<Ast::Helper> const &astHelper);
  public: static void unextend(Core::Data::Seeker *seeker, Overrides *overrides);

  /// @}

  /// @name Seek Functions
  /// @{

  private: static Core::Data::Seeker::Verb _foreach(
    TiFunctionBase *base, TiObject *self, TiObject const *ref, TiObject *target,
    Core::Data::Seeker::ForeachCallback const &cb, Word flags
  );

  private: static Core::Data::Seeker::Verb _foreachByIdentifier_level(
    TiFunctionBase *base, TiObject *self, Data::Ast::Identifier const *identifier, TiObject *data,
    Core::Data::Seeker::ForeachCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(foreachByIdentifier_function,
    Core::Data::Seeker::Verb, (
      Data::Ast::Identifier const*, Ast::Function*, Core::Data::Seeker::ForeachCallback const&, Word
    )
  );
  private: static Core::Data::Seeker::Verb _foreachByIdentifier_function(
    TiObject *self, Data::Ast::Identifier const *identifier, Ast::Function *function,
    Core::Data::Seeker::ForeachCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(foreachByIdentifier_dataType,
    Core::Data::Seeker::Verb, (
      Data::Ast::Identifier const*, Ast::DataType*, Core::Data::Seeker::ForeachCallback const&, Word
    )
  );
  private: static Core::Data::Seeker::Verb _foreachByIdentifier_dataType(
    TiObject *self, Data::Ast::Identifier const *identifier, Ast::DataType *type,
    Core::Data::Seeker::ForeachCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(foreachByParamPass,
    Core::Data::Seeker::Verb, (Data::Ast::ParamPass const*, TiObject*, Core::Data::Seeker::ForeachCallback const&, Word)
  );
  private: static Core::Data::Seeker::Verb _foreachByParamPass(
    TiObject *self, Data::Ast::ParamPass const *paramPass, TiObject *data,
    Core::Data::Seeker::ForeachCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(foreachByParamPass_routing,
    Core::Data::Seeker::Verb, (
      Data::Ast::ParamPass const*, TiObject *, Core::Data::Seeker::ForeachCallback const&, Word
    )
  );
  private: static Core::Data::Seeker::Verb _foreachByParamPass_routing(
    TiObject *self, Data::Ast::ParamPass const *paramPass, TiObject *data,
    Core::Data::Seeker::ForeachCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(foreachByParamPass_template,
    Core::Data::Seeker::Verb, (TiObject*, Spp::Ast::Template*, Core::Data::Seeker::ForeachCallback const&, Word)
  );
  private: static Core::Data::Seeker::Verb _foreachByParamPass_template(
    TiObject *self, TiObject *param, Spp::Ast::Template *tmplt, Core::Data::Seeker::ForeachCallback const &cb,
    Word flags
  );

  public: METHOD_BINDING_CACHE(foreachByThisTypeRef,
    Core::Data::Seeker::Verb, (TiObject*, Core::Data::Seeker::ForeachCallback const&, Word)
  );
  private: static Core::Data::Seeker::Verb _foreachByThisTypeRef(
    TiObject *self, TiObject *data, Core::Data::Seeker::ForeachCallback const &cb, Word flags
  );

  /// @}

}; // class

} // namespace

#endif
