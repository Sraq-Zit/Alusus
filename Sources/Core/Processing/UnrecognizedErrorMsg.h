/**
 * @file Core/Processing/UnrecognizedErrorMsg.h
 * Contains the header of class Core::Processing::UnrecognizedErrorMsg.
 *
 * @copyright Copyright (C) 2015 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef PROCESSING_UNRECOGNIZEDERRORMSG_H
#define PROCESSING_UNRECOGNIZEDERRORMSG_H

namespace Core { namespace Processing
{

// TODO: DOC

class UnrecognizedErrorMsg : public BuildMsg
{
  //============================================================================
  // Type Info

  TYPE_INFO(UnrecognizedErrorMsg, BuildMsg, "Core.Processing", "Core", "alusus.net");


  //============================================================================
  // Constructor / Destructor

  public: UnrecognizedErrorMsg(Data::SourceLocation const &sl) : BuildMsg(sl)
  {
  }

  public: virtual ~UnrecognizedErrorMsg()
  {
  }


  //============================================================================
  // Member Functions

  /// @sa BuildMsg::getCode()
  public: virtual Str const& getCode() const
  {
    static Str code("G1001");
    return code;
  }

  /// @sa BuildMsg::getSeverity()
  public: virtual Int getSeverity() const
  {
    return 1;
  }

  /// @sa BuildMsg::getCode()
  public: virtual void buildDescription(Str &str) const
  {
    str = STR("Unrecognized Error.");
  }

}; // class

} } // namespace

#endif
