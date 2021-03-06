/**
 * @file Core/Processing/Handlers/DumpAstParsingHandler.cpp
 * Contains the implementation of Core::Processing::Handlers::DumpAstParsingHandler.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core::Processing::Handlers
{

using namespace Core::Data;

//==============================================================================
// Overloaded Abstract Functions

void DumpAstParsingHandler::onProdEnd(Parser *parser, ParserState *state)
{
  using SeekVerb = Data::Seeker::Verb;

  auto data = state->getData().ti_cast_get<Containing<TiObject>>()->getElement(1);
  ASSERT(data != 0);
  auto metadata = ti_cast<Core::Data::Ast::MetaHaving>(data);
  ASSERT(metadata != 0);

  try {
    Bool found = false;
    this->rootManager->getSeeker()->foreach(data, state->getDataStack(),
      [=, &found](TiObject *obj, Notices::Notice*)->SeekVerb
      {
        if (obj != 0) {
          outStream << S("------------------ Parsed Data Dump ------------------\n");
          dumpData(outStream, obj, 0);
          outStream << S("\n------------------------------------------------------\n");
          found = true;
        }
        return SeekVerb::MOVE;
      }, 0
    );
    if (!found) {
      state->addNotice(std::make_shared<Notices::InvalidDumpArgNotice>(metadata->findSourceLocation()));
    }
  } catch (InvalidArgumentException) {
    state->addNotice(std::make_shared<Notices::InvalidDumpArgNotice>(metadata->findSourceLocation()));
  }

  state->setData(SharedPtr<TiObject>(0));
}

} // namespace
