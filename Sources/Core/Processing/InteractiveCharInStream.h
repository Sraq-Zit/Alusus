/**
 * @file Core/Processing/InteractiveCharInStream.h
 * Contains the header of class Core::Processing::InteractiveCharInStream.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_PROCESSING_INTERACTIVECHARINSTREAM_H
#define CORE_PROCESSING_INTERACTIVECHARINSTREAM_H

#ifdef _WIN32
#include "Win32Helpers.h"
#endif

namespace Core::Processing
{

class InteractiveCharInStream : public TiObject, public CharInStreaming
{
  //============================================================================
  // Type Info

  TYPE_INFO(InteractiveCharInStream, TiObject, "Core.Data.Ast", "Core", "alusus.org", (
    INHERITANCE_INTERFACES(CharInStreaming)
  ));


  //============================================================================
  // Member Variables

  private: Int lineNumber;
#ifdef _WIN32
  private: Bool isPreviousLF;
#endif


  //============================================================================
  // Constructors & Destructor

  public: InteractiveCharInStream();

  public: virtual ~InteractiveCharInStream();


  //============================================================================
  // Member Functions

#ifdef _WIN32
  public: virtual WChar get();
#else
  public: virtual Char get();
#endif

  public: virtual Bool isEof();

  public: virtual CharInStreaming::CharInStreamingType getType();

}; // class

} // namespace

#endif
