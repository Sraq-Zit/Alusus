/**
 * @file Spp/SeekerExtension.cpp
 * Contains the implementation of class Spp::SeekerExtension.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#include "spp.h"

namespace Spp
{

using namespace std::placeholders;

//==============================================================================
// Initialization Functions

SeekerExtension::Overrides* SeekerExtension::extend(Core::Data::Seeker *seeker, SharedPtr<Ast::Helper> const &astHelper)
{
  auto extension = std::make_shared<SeekerExtension>(seeker);
  seeker->addDynamicInterface(extension);

  auto overrides = new Overrides();
  extension->astHelper = astHelper;
  overrides->foreachRef = seeker->foreach.set(&SeekerExtension::_foreach).get();
  overrides->foreachByIdentifier_levelRef =
    seeker->foreachByIdentifier_level.set(&SeekerExtension::_foreachByIdentifier_level).get();
  overrides->foreachByIdentifier_functionRef =
    extension->foreachByIdentifier_function.set(&SeekerExtension::_foreachByIdentifier_function).get();
  overrides->foreachByIdentifier_dataTypeRef =
    extension->foreachByIdentifier_dataType.set(&SeekerExtension::_foreachByIdentifier_dataType).get();
  overrides->foreachByParamPassRef =
    extension->foreachByParamPass.set(&SeekerExtension::_foreachByParamPass).get();
  overrides->foreachByParamPass_routingRef =
    extension->foreachByParamPass_routing.set(&SeekerExtension::_foreachByParamPass_routing).get();
  overrides->foreachByParamPass_templateRef =
    extension->foreachByParamPass_template.set(&SeekerExtension::_foreachByParamPass_template).get();
  overrides->foreachByThisTypeRefRef =
    extension->foreachByThisTypeRef.set(&SeekerExtension::_foreachByThisTypeRef).get();

  return overrides;
}


void SeekerExtension::unextend(Core::Data::Seeker *seeker, Overrides *overrides)
{
  auto extension = ti_cast<SeekerExtension>(seeker);
  seeker->foreach.reset(overrides->foreachRef);
  seeker->foreachByIdentifier_level.reset(overrides->foreachByIdentifier_levelRef);
  extension->astHelper = SharedPtr<Ast::Helper>::null;
  extension->foreachByIdentifier_function.reset(overrides->foreachByIdentifier_functionRef);
  extension->foreachByIdentifier_dataType.reset(overrides->foreachByIdentifier_dataTypeRef);
  extension->foreachByParamPass.reset(overrides->foreachByParamPassRef);
  extension->foreachByParamPass_routing.reset(overrides->foreachByParamPass_routingRef);
  extension->foreachByParamPass_template.reset(overrides->foreachByParamPass_templateRef);
  extension->foreachByThisTypeRef.reset(overrides->foreachByThisTypeRefRef);

  seeker->removeDynamicInterface<SeekerExtension>();
  delete overrides;
}


//==============================================================================
// Seek Functions

Core::Data::Seeker::Verb SeekerExtension::_foreach(
  TiFunctionBase *base, TiObject *self, TiObject const *ref, TiObject *target,
  Core::Data::Seeker::ForeachCallback const &cb, Word flags
) {
  if (ref->isA<Data::Ast::ParamPass>()) {
    PREPARE_SELF(seekerExtension, SeekerExtension);
    return seekerExtension->foreachByParamPass(static_cast<Data::Ast::ParamPass const*>(ref), target, cb, flags);
  } else if (ref->isA<Spp::Ast::ThisTypeRef>()) {
    PREPARE_SELF(seekerExtension, SeekerExtension);
    return seekerExtension->foreachByThisTypeRef(target, cb, flags);
  } else {
    PREPARE_SELF(seeker, Core::Data::Seeker);
    return seeker->foreach.useCallee(base)(ref, target, cb, flags);
  }
}


Core::Data::Seeker::Verb SeekerExtension::_foreachByIdentifier_level(
  TiFunctionBase *base, TiObject *self, Data::Ast::Identifier const *identifier, TiObject *data,
  Core::Data::Seeker::ForeachCallback const &cb, Word flags
) {
  if (data->isDerivedFrom<Ast::Function>()) {
    PREPARE_SELF(seekerExtension, SeekerExtension);
    return seekerExtension->foreachByIdentifier_function(identifier, static_cast<Ast::Function*>(data), cb, flags);
  } else if (data->isDerivedFrom<Ast::DataType>()) {
    PREPARE_SELF(seekerExtension, SeekerExtension);
    return seekerExtension->foreachByIdentifier_dataType(identifier, static_cast<Ast::DataType*>(data), cb, flags);
  } else {
    PREPARE_SELF(seeker, Core::Data::Seeker);
    return seeker->foreachByIdentifier_level.useCallee(base)(identifier, data, cb, flags);
  }
}


Core::Data::Seeker::Verb SeekerExtension::_foreachByIdentifier_function(
  TiObject *self, Data::Ast::Identifier const *identifier, Ast::Function *function,
  Core::Data::Seeker::ForeachCallback const &cb, Word flags
) {
  auto argTypes = function->getType()->getArgTypes().get();
  if (argTypes == 0) return Core::Data::Seeker::Verb::MOVE;
  auto index = argTypes->findIndex(identifier->getValue().get());
  if (index >= 0) return cb(argTypes->getElement(index), 0);
  return Core::Data::Seeker::Verb::MOVE;
}


Core::Data::Seeker::Verb SeekerExtension::_foreachByIdentifier_dataType(
  TiObject *self, Data::Ast::Identifier const *identifier, Ast::DataType *type,
  Core::Data::Seeker::ForeachCallback const &cb, Word flags
) {
  PREPARE_SELF(seeker, Core::Data::Seeker);
  auto block = type->getBody().get();
  if ((block == 0) || (flags & Core::Data::Seeker::Flags::SKIP_OWNED)) {
    return Core::Data::Seeker::Verb::MOVE;
  } else {
    return seeker->foreachByIdentifier_level(identifier, block, cb, flags);
  }
}


Core::Data::Seeker::Verb SeekerExtension::_foreachByParamPass(
  TiObject *self, Data::Ast::ParamPass const *paramPass, TiObject *data,
  Core::Data::Seeker::ForeachCallback const &cb, Word flags
) {
  PREPARE_SELF(seeker, Core::Data::Seeker);
  PREPARE_SELF(seekerExtension, SeekerExtension);
  auto operand = paramPass->getOperand().get();
  return seeker->foreach(operand, data,
    [=](TiObject *newData, Core::Notices::Notice*)->Core::Data::Seeker::Verb
    {
      return seekerExtension->foreachByParamPass_routing(paramPass, newData, cb, flags);
    },
    flags
  );
}


Core::Data::Seeker::Verb SeekerExtension::_foreachByParamPass_routing(
  TiObject *self, Data::Ast::ParamPass const *paramPass, TiObject *data,
  Core::Data::Seeker::ForeachCallback const &cb, Word flags
) {
  PREPARE_SELF(seekerExtension, SeekerExtension);
  if (paramPass->getType() == Core::Data::Ast::BracketType::SQUARE) {
    auto param = paramPass->getParam().get();
    if (data->isDerivedFrom<Ast::Template>()) {
      return seekerExtension->foreachByParamPass_template(param, static_cast<Ast::Template*>(data), cb, flags);
    } else {
      auto notice = std::make_shared<Spp::Notices::InvalidSquareBracketOperandNotice>(
        Core::Data::Ast::findSourceLocation(paramPass)
      );
      return cb(0, notice.get());
    }
  } else {
    throw EXCEPTION(InvalidArgumentException, S("paramPass"), S("Invalid bracket type."), paramPass->getType());
  }
}


Core::Data::Seeker::Verb SeekerExtension::_foreachByParamPass_template(
  TiObject *self, TiObject *param, Ast::Template *tmplt, Core::Data::Seeker::ForeachCallback const &cb, Word flags
) {
  PREPARE_SELF(seekerExtension, SeekerExtension);
  TioSharedPtr result;
  if (tmplt->matchInstance(param, seekerExtension->astHelper, result)) {
    return cb(result.get(), 0);
  } else {
    auto notice = result.ti_cast_get<Core::Notices::Notice>();
    if (notice != 0) return cb(0, notice);
    else return Core::Data::Seeker::Verb::MOVE;
  }
}


Core::Data::Seeker::Verb SeekerExtension::_foreachByThisTypeRef(
  TiObject *self, TiObject *data, Core::Data::Seeker::ForeachCallback const &cb, Word flags
) {
  auto node = static_cast<Core::Data::Node*>(data);
  while (node != 0) {
    if (node->isDerivedFrom<Spp::Ast::DataType>()) {
      return cb(node, 0);
    }
    node = node->getOwner();
  }
  return Core::Data::Seeker::Verb::MOVE;
}

} // namespace
