/**
 * @file Core/Notices/notices.cpp
 * Contains the global implementations of Notices namespace's declarations.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

#ifdef _WIN32
#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <windows.h>
#endif

namespace Core::Notices
{

void printNotice(Notice const *msg)
{
  // We will only print the error message if we have a source location for it.
  if (Data::getSourceLocationRecordCount(msg->getSourceLocation().get()) == 0) return;

  // Print severity.
#ifdef _WIN32
  // Get STDOUT handle.
  HANDLE h = GetStdHandle (STD_OUTPUT_HANDLE);
  WORD wOldColorAttrs;
  CONSOLE_SCREEN_BUFFER_INFO csbiInfo;

  // Save the current color information.
  GetConsoleScreenBufferInfo(h, &csbiInfo);
  wOldColorAttrs = csbiInfo.wAttributes;

  // Set the console color and print the severity type of the message.
  switch (msg->getSeverity()) {
    case 0:
      SetConsoleTextAttribute(h, FOREGROUND_INTENSITY | FOREGROUND_RED); // Red.
      outStream << L18nDictionary::getSingleton()->get(S("BLOCKER"), S("BLOCKER")) << S(" ");
      break;
    case 1:
      SetConsoleTextAttribute(h, FOREGROUND_INTENSITY | FOREGROUND_RED); // Red.
      outStream << L18nDictionary::getSingleton()->get(S("ERROR"), S("ERROR")) << S(" ");
      break;
    case 2:
    case 3:
      SetConsoleTextAttribute(h, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN); // Yellow.
      outStream << L18nDictionary::getSingleton()->get(S("WARNING"), S("WARNING")) << S(" ");
      break;
    case 4:
      SetConsoleTextAttribute(h, FOREGROUND_INTENSITY | FOREGROUND_BLUE); // Blue.
      outStream << L18nDictionary::getSingleton()->get(S("ATTN"), S("ATTN")) << S(" ");
      break;
  }
#else
  switch (msg->getSeverity()) {
    case 0:
      outStream << S("\033[0;31m") << L18nDictionary::getSingleton()->get(S("BLOCKER"), S("BLOCKER")) << S(" ");
      break;
    case 1:
      outStream << S("\033[0;31m") << L18nDictionary::getSingleton()->get(S("ERROR"), S("ERROR")) << S(" ");
      break;
    case 2:
    case 3:
      outStream << S("\033[1;33m") << L18nDictionary::getSingleton()->get(S("WARNING"), S("WARNING")) << S(" ");
      break;
    case 4:
      outStream << S("\033[0;34m") << L18nDictionary::getSingleton()->get(S("ATTN"), S("ATTN")) << S(" ");
      break;
  }
#endif

  // Print msg code.
  outStream << msg->getCode() << " @ ";
  // Print location.
  auto sl = msg->getSourceLocation().get();
  if (sl->isDerivedFrom<Data::SourceLocationRecord>()) {
    auto slRecord = static_cast<Data::SourceLocationRecord*>(sl);
    outStream << slRecord->filename->c_str() << " (" << slRecord->line << "," << slRecord->column << ")";
  } else {
    auto stack = static_cast<Data::SourceLocationStack*>(sl);
    for (Int i = stack->getCount() - 1; i >= 0; --i) {
      if (i < stack->getCount() -1) {
        outStream << NEW_LINE << L18nDictionary::getSingleton()->get(S("FROM"), S("from")) << S(" ");
      }
      outStream << stack->get(i)->filename->c_str()
        << " (" << stack->get(i)->line << "," << stack->get(i)->column << ")";
    }
  }
  outStream << S(": ");
  // Print description.
  outStream << msg->getDescription() << S("\033[0m") << NEW_LINE;

#ifdef _WIN32
  // Restore the original text color.
  SetConsoleTextAttribute(h, wOldColorAttrs);
#endif
}

} // namespace
