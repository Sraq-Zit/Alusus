/**
 * @file Core/Processing/UnrecognizedCharMsg.h
 * Contains the header of class Core::Processing::UnrecognizedCharMsg.
 *
 * @copyright Copyright (C) 2015 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_PROCESSING_UNRECOGNIZEDCHARMSG_H
#define CORE_PROCESSING_UNRECOGNIZEDCHARMSG_H

namespace Core { namespace Processing
{

/**
 * @brief A build message for the "Unrecognized Char" error.
 * @ingroup processing_lexer
 *
 * This message class is for error code L1001, which is raised when the
 * lexer faces characters that are not recognized by any token.
 */
class UnrecognizedCharMsg : public BuildMsg
{
  //============================================================================
  // Type Info

  TYPE_INFO(UnrecognizedCharMsg, Processing::BuildMsg, "Core.Processing", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  /**
   * @brief The source text.
   *
   * This string contains all characters in the group of consecutive
   * characters that caused the error or the warning. However, this string
   * should contain a maximum of LEXER_ERROR_BUFFER_MAX_CHARACTERS characters.
   * If the error text is bigger than LEXER_ERROR_BUFFER_MAX_CHARACTERS, it should
   * be clipped.
   */
  private: Str text;


  //============================================================================
  // Constructor / Destructor

  public: UnrecognizedCharMsg(Char const *t, Data::SourceLocation const &sl) : BuildMsg(sl), text(t)
  {
  }

  public: virtual ~UnrecognizedCharMsg()
  {
  }


  //============================================================================
  // Member Functions

  /// @sa Processing::BuildMsg::getCode()
  public: virtual Str const& getCode() const
  {
    static Str code("L1001");
    return code;
  }

  /// @sa Processing::BuildMsg::getSeverity()
  public: virtual Int getSeverity() const
  {
    return 1;
  }

  /// @sa Processing::BuildMsg::buildDescription()
  public: virtual void buildDescription(Str &str) const;

  /**
   * @brief Set the source text.
   *
   * This string contains all characters in the group of consecutive
   * characters that caused the error or the warning. However, this string
   * should contain a maximum of LEXER_ERROR_BUFFER_MAX_CHARACTERS characters.
   * If the error text is bigger than LEXER_ERROR_BUFFER_MAX_CHARACTERS, it should
   * be clipped.
   *
   * @param t A pointer to a string containing the error or warning text.
   */
  public: void setText(Char const *t)
  {
    this->text = t;
  }

  /**
   * @brief Append a character to the source text.
   *
   * The source text string contains all characters in a group of consecutive
   * characters that caused the error or the warning. However, this string
   * should contain a maximum of LEXER_ERROR_BUFFER_MAX_CHARACTERS characters.
   * If the error text is bigger than LEXER_ERROR_BUFFER_MAX_CHARACTERS, it should
   * be clipped.
   *
   * @param ch The character to append to the source text.
   * @param sl The source location at which the character appeared. This value
   *           will only be considered if this character is the first in the
   *           buffer.
   */
  public: void appendText(Char ch, Data::SourceLocation const &sl)
  {
    if (this->getSourceLocation().line == 0) {
      this->setSourceLocation(sl);
    }
    this->text.append(1, ch);
  }

  /**
   * @brief Append a string to the source text.
   *
   * The source text string contains all characters in a group of consecutive
   * characters that caused the error or the warning. However, this string
   * should contain a maximum of LEXER_ERROR_BUFFER_MAX_CHARACTERS characters.
   * If the error text is bigger than LEXER_ERROR_BUFFER_MAX_CHARACTERS, it should
   * be clipped.
   *
   * @param str The string to append to the source text.
   * @param sl The source location at which the character appeared. This value
   *           will only be considered if this character is the first in the
   *           buffer.
   */
  public: void appendText(Char const *str, Data::SourceLocation const &sl)
  {
    if (str == 0 || str[0] == CHR('\0')) return;
    if (this->getSourceLocation().line == 0) {
      this->setSourceLocation(sl);
    }
    this->text.append(str);
  }

  /**
   * @brief Get a pointer to the source text.
   *
   * This string contains all characters in the group of consecutive characters
   * that caused the error or the warning. However, this string should contain
   * a maximum of LEXER_ERROR_BUFFER_MAX_CHARACTERS characters. If the error text is
   * bigger than LEXER_ERROR_BUFFER_MAX_CHARACTERS, it should be clipped.
   *
   * @return A pointer to the source text string.
   */
  public: Str const& getText() const
  {
    return this->text;
  }

  /**
   * @brief Get the length of the source text.
   *
   * This string contains all characters in the group of consecutive characters
   * that caused the error or the warning. However, this string should contain
   * a maximum of LEXER_ERROR_BUFFER_MAX_CHARACTERS characters. If the error text is
   * bigger than LEXER_ERROR_BUFFER_MAX_CHARACTERS, it should be clipped.
   *
   * @return The number of characters in the error text.
   */
  public: Int getTextLength() const
  {
    return this->text.size();
  }

  /**
   * Clear the source text and reset the line and column to -1 and the code to
   * 0.
   */
  public: void clear()
  {
    this->text.clear();
    this->setSourceLocation(Data::SourceLocation());
  }

}; // class

} } // namespace

#endif
