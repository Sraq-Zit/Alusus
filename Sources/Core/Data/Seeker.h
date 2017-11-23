/**
 * @file Core/Data/Seeker.h
 * Contains the header of class Core::Data::Seeker.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_SEEKER_H
#define CORE_DATA_SEEKER_H

namespace Core { namespace Data
{

class Seeker : public TiObject, public virtual DynamicBindings, public virtual DynamicInterfaces
{
  //============================================================================
  // Type Info

  TYPE_INFO(Seeker, TiObject, "Core.Data", "Core", "alusus.net", (
    INHERITANCE_INTERFACES(DynamicBindings, DynamicInterfaces),
    OBJECT_INTERFACE_LIST(interfaceList)
  ));


  //============================================================================
  // Types

  public: ti_s_enum(Verb, Integer, "Core.Data", "Core", "alusus.net",
                    MOVE, STOP, PERFORM_AND_MOVE, PERFORM_AND_STOP);

  public: typedef std::function<Verb(TiObject *&obj, Notice *notice)> SetCallback;
  public: typedef std::function<Verb(TiObject *obj, Notice *notice)> RemoveCallback;
  public: typedef std::function<Verb(TiObject *obj, Notice *notice)> ForeachCallback;


  //============================================================================
  // Types

  private: struct PrecomputedRecord
  {
    TiObject const *ref;
    TiObject *target;
    TiObject *result;
    PrecomputedRecord(TiObject const *ref, TiObject *target, TiObject *result)
      : ref(ref), target(target), result(result)
    {
    }
  };


  //============================================================================
  // Member Variables

  private: std::vector<PrecomputedRecord> precomputedRecords;


  //============================================================================
  // Implementations

  IMPLEMENT_DYNAMIC_BINDINGS(bindingMap);
  IMPLEMENT_DYNAMIC_INTERFACES(interfaceList);


  //============================================================================
  // Constructors

  Seeker()
  {
    this->initBindingCaches();
    this->initBindings();
  }

  Seeker(Seeker *parent)
  {
    this->initBindingCaches();
    this->inheritBindings(parent);
    this->inheritInterfaces(parent);
  }


  //============================================================================
  // Member Functions

  /// @name Initialization
  /// @{

  private: void initBindingCaches();

  private: void initBindings();

  /// @}

  /// @name Helper Functions
  /// @{

  public: void doSet(TiObject const *ref, TiObject *target, SetCallback const &cb)
  {
    this->set(ref, target, cb);
  }

  public: void doRemove(TiObject const *ref, TiObject *target, RemoveCallback const &cb)
  {
    this->remove(ref, target, cb);
  }

  public: void doForeach(TiObject const *ref, TiObject *target, ForeachCallback const &cb)
  {
    for (PrecomputedRecord &record : this->precomputedRecords) {
      if (record.ref == ref && record.target == target) {
        cb(record.result, 0);
        return;
      }
    }
    this->foreach(ref, target, cb);
  }

  public: void doContinueForeach(
    TiObject const *continueRef, TiObject *continueResult, TiObject const *ref, TiObject *target,
    ForeachCallback const &cb
  ) {
    this->precomputedRecords.push_back(PrecomputedRecord(continueRef, target, continueResult));
    this->doForeach(ref, target, cb);
    this->precomputedRecords.pop_back();
  }

  public: Bool trySet(TiObject const *ref, TiObject *target, TiObject *val);

  public: void doSet(TiObject const *ref, TiObject *target, TiObject *val)
  {
    if (!this->trySet(ref, target, val)) {
      throw EXCEPTION(GenericException, STR("Reference pointing to a missing element/tree."));
    }
  }

  public: Bool tryRemove(TiObject const *ref, TiObject *target);

  public: void doRemove(TiObject const *ref, TiObject *target)
  {
    if (!this->tryRemove(ref, target)) {
      throw EXCEPTION(GenericException, STR("Reference pointing to a missing element/tree."));
    }
  }

  public: Bool tryGet(TiObject const *ref, TiObject *target, TiObject *&retVal);

  public: TiObject* tryGet(TiObject const *ref, TiObject *target)
  {
    TiObject *result = 0;
    this->tryGet(ref, target, result);
    return result;
  }

  public: TiObject* doGet(TiObject const *ref, TiObject *target)
  {
    TiObject *retVal = this->tryGet(ref, target);
    if (retVal == 0) {
      throw EXCEPTION(GenericException, STR("Reference pointing to a missing element/tree."));
    }
    return retVal;
  }

  public: static Bool isPerform(Verb verb)
  {
    return verb == Verb::PERFORM_AND_STOP || verb == Verb::PERFORM_AND_MOVE;
  }

  public: static Bool isMove(Verb verb)
  {
    return verb == Verb::MOVE || verb == Verb::PERFORM_AND_MOVE;
  }

  /// @}

  /// @name Main Seek Functions
  /// @{

  public: METHOD_BINDING_CACHE(set, void, (TiObject const*, TiObject*, SetCallback const&));
  public: METHOD_BINDING_CACHE(remove, void, (TiObject const*, TiObject*, RemoveCallback const&));
  public: METHOD_BINDING_CACHE(foreach, void, (TiObject const*, TiObject*, ForeachCallback const&));

  private: static void _set(TiObject *self, TiObject const *ref, TiObject *target, SetCallback const &cb);
  private: static void _remove(TiObject *self, TiObject const *ref, TiObject *target, RemoveCallback const &cb);
  private: static void _foreach(TiObject *self, TiObject const *ref, TiObject *target, ForeachCallback const &cb);

  /// @}

  /// @name Identifier Seek Functions
  /// @{

  public: METHOD_BINDING_CACHE(setByIdentifier,
    void, (Data::Ast::Identifier const*, TiObject*, SetCallback const&)
  );
  public: METHOD_BINDING_CACHE(setByIdentifier_level,
    Verb, (Data::Ast::Identifier const*, TiObject*, SetCallback const&)
  );
  public: METHOD_BINDING_CACHE(setByIdentifier_scope,
    Verb, (Data::Ast::Identifier const*, Ast::Scope*, SetCallback const&)
  );

  private: static void _setByIdentifier(
    TiObject *self, Data::Ast::Identifier const *identifier, TiObject *data, SetCallback const &cb
  );
  private: static Verb _setByIdentifier_level(
    TiObject *self, Data::Ast::Identifier const *identifier, TiObject *data, SetCallback const &cb
  );
  private: static Verb _setByIdentifier_scope(
    TiObject *self, Data::Ast::Identifier const *identifier, Ast::Scope *scope, SetCallback const &cb
  );

  public: METHOD_BINDING_CACHE(removeByIdentifier,
    void, (Data::Ast::Identifier const*, TiObject*, RemoveCallback const&)
  );
  public: METHOD_BINDING_CACHE(removeByIdentifier_level,
    Verb, (Data::Ast::Identifier const*, TiObject*, RemoveCallback const&)
  );
  public: METHOD_BINDING_CACHE(removeByIdentifier_scope,
    Verb, (Data::Ast::Identifier const*, Ast::Scope*, RemoveCallback const&)
  );

  private: static void _removeByIdentifier(
    TiObject *self, Data::Ast::Identifier const *identifier, TiObject *data, RemoveCallback const &cb
  );
  private: static Verb _removeByIdentifier_level(
    TiObject *self, Data::Ast::Identifier const *identifier, TiObject *data, RemoveCallback const &cb
  );
  private: static Verb _removeByIdentifier_scope(
    TiObject *self, Data::Ast::Identifier const *identifier, Ast::Scope *scope, RemoveCallback const &cb
  );

  public: METHOD_BINDING_CACHE(foreachByIdentifier,
    void, (Data::Ast::Identifier const*, TiObject*, ForeachCallback const&)
  );
  public: METHOD_BINDING_CACHE(foreachByIdentifier_level,
    Verb, (Data::Ast::Identifier const*, TiObject*, ForeachCallback const&)
  );
  public: METHOD_BINDING_CACHE(foreachByIdentifier_scope,
    Verb, (Data::Ast::Identifier const*, Ast::Scope*, ForeachCallback const&)
  );

  private: static void _foreachByIdentifier(
    TiObject *self, Data::Ast::Identifier const *identifier, TiObject *data, ForeachCallback const &cb
  );
  private: static Verb _foreachByIdentifier_level(
    TiObject *self, Data::Ast::Identifier const *identifier, TiObject *data, ForeachCallback const &cb
  );
  private: static Verb _foreachByIdentifier_scope(
    TiObject *self, Data::Ast::Identifier const *identifier, Ast::Scope *scope, ForeachCallback const &cb
  );

  /// @}

  /// @name LinkOperator Seek Functions
  /// @{

  public: METHOD_BINDING_CACHE(setByLinkOperator,
    void, (Data::Ast::LinkOperator const*, TiObject*, SetCallback const&)
  );
  public: METHOD_BINDING_CACHE(setByLinkOperator_routing,
    Verb, (Data::Ast::LinkOperator const*, TiObject*, SetCallback const&)
  );
  public: METHOD_BINDING_CACHE(setByLinkOperator_scopeDotIdentifier,
    Verb, (Data::Ast::Identifier const*, Data::Ast::Scope*, SetCallback const&)
  );
  public: METHOD_BINDING_CACHE(setByLinkOperator_mapDotIdentifier,
    Verb, (Data::Ast::Identifier const*, Data::MapContainer*, SetCallback const&)
  );

  private: static void _setByLinkOperator(
    TiObject *self, Data::Ast::LinkOperator const *link, TiObject *data, SetCallback const &cb
  );
  private: static Verb _setByLinkOperator_routing(
    TiObject *self, Data::Ast::LinkOperator const *link, TiObject *data, SetCallback const &cb
  );
  private: static Verb _setByLinkOperator_scopeDotIdentifier(
    TiObject *self, Data::Ast::Identifier const *identifier, Data::Ast::Scope *scope, SetCallback const &cb
  );
  private: static Verb _setByLinkOperator_mapDotIdentifier(
    TiObject *self, Data::Ast::Identifier const *identifier, Data::MapContainer *map, SetCallback const &cb
  );

  public: METHOD_BINDING_CACHE(removeByLinkOperator,
    void, (Data::Ast::LinkOperator const*, TiObject*, RemoveCallback const&)
  );
  public: METHOD_BINDING_CACHE(removeByLinkOperator_routing,
    Verb, (Data::Ast::LinkOperator const*, TiObject*, RemoveCallback const&)
  );
  public: METHOD_BINDING_CACHE(removeByLinkOperator_scopeDotIdentifier,
    Verb, (Data::Ast::Identifier const*, Data::Ast::Scope*, RemoveCallback const&)
  );
  public: METHOD_BINDING_CACHE(removeByLinkOperator_mapDotIdentifier,
    Verb, (Data::Ast::Identifier const*, Data::MapContainer*, RemoveCallback const&)
  );

  private: static void _removeByLinkOperator(
    TiObject *self, Data::Ast::LinkOperator const *link, TiObject *data, RemoveCallback const &cb
  );
  private: static Verb _removeByLinkOperator_routing(
    TiObject *self, Data::Ast::LinkOperator const *link, TiObject *data, RemoveCallback const &cb
  );
  private: static Verb _removeByLinkOperator_scopeDotIdentifier(
    TiObject *self, Data::Ast::Identifier const *identifier, Data::Ast::Scope *scope, RemoveCallback const &cb
  );
  private: static Verb _removeByLinkOperator_mapDotIdentifier(
    TiObject *self, Data::Ast::Identifier const *identifier, Data::MapContainer *map, RemoveCallback const &cb
  );

  public: METHOD_BINDING_CACHE(foreachByLinkOperator,
    void, (Data::Ast::LinkOperator const*, TiObject*, ForeachCallback const&)
  );
  public: METHOD_BINDING_CACHE(foreachByLinkOperator_routing,
    Verb, (Data::Ast::LinkOperator const*, TiObject*, ForeachCallback const&)
  );
  public: METHOD_BINDING_CACHE(foreachByLinkOperator_scopeDotIdentifier,
    Verb, (Data::Ast::Identifier*, Data::Ast::Scope*, ForeachCallback const&)
  );
  public: METHOD_BINDING_CACHE(foreachByLinkOperator_mapDotIdentifier,
    Verb, (Data::Ast::Identifier const*, Data::MapContainer*, ForeachCallback const&)
  );

  private: static void _foreachByLinkOperator(
    TiObject *self, Data::Ast::LinkOperator const *link, TiObject *data, ForeachCallback const &cb
  );
  private: static Verb _foreachByLinkOperator_routing(
    TiObject *self, Data::Ast::LinkOperator const *link, TiObject *data, ForeachCallback const &cb
  );
  private: static Verb _foreachByLinkOperator_scopeDotIdentifier(
    TiObject *self, Data::Ast::Identifier *identifier, Data::Ast::Scope *scope, ForeachCallback const &cb
  );
  private: static Verb _foreachByLinkOperator_mapDotIdentifier(
    TiObject *_self, Data::Ast::Identifier const *identifier, Data::MapContainer *map, ForeachCallback const &cb
  );

  /// @}

}; // class

} } // namespace

#endif
