/**
 * @file Tests/CoreTests/TestGrammarPlant.cpp
 * Contains the implementation of class CoreTests::TestGrammarPlant.
 *
 * @copyright Copyright (C) 2015 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core_tests.h"

namespace Tests { namespace CoreTests
{

using namespace Core::Data;
using namespace Core::Processing;
using namespace Core::Standard;

//==============================================================================
// Member Functions

void TestGrammarPlant::createGrammar()
{
  this->constTokenId = ID_GENERATOR->getId(STR("CONSTTOKEN"));

  // Instantiate handlers.
  this->stringLiteralHandler = std::make_shared<StringLiteralTokenizingHandler>();
  this->constTokenHandler = std::make_shared<ConstTokenizingHandler>(this->constTokenId);
  this->parsingHandler = std::make_shared<Handlers::GenericParsingHandler>();

  // Create lexer definitions.
  this->repository.set(STR("root:LexerDefs"), GrammarModule::create({}).get());
  this->createCharGroupDefinitions();
  this->createTokenDefinitions();

  // Create parser definitions.
  this->createProductionDefinitions();

  // Set start production and lexer module.
  GrammarModule *root = this->repository.getRoot().get();
  root->setStartRef(REF_PARSER->parseQualifier(STR("module:Program")));
  root->setLexerModuleRef(REF_PARSER->parseQualifier(STR("root:LexerDefs")));

  // Generate const token definitions from production definitions.
  this->constTokenPrefix = STR("LexerDefs");
  this->generateConstTokenDefinitions();
}


/**
 * Create a CharGroupDefinitionList and add all the required definitions for
 * the Core's grammar.
 */
void TestGrammarPlant::createCharGroupDefinitions()
{
  // BinDigit : char '0'..'1';
  this->repository.set(STR("root:LexerDefs.BinDigit"), CharGroupDefinition::create(
    SequenceCharGroupUnit::create(STR("0"), STR("1"))).get());

  // OctDigit : char '0'..'7';
  this->repository.set(STR("root:LexerDefs.OctDigit"), CharGroupDefinition::create(
    SequenceCharGroupUnit::create(STR("0"), STR("7"))).get());

  // DecDigit : char '0'..'9';
  this->repository.set(STR("root:LexerDefs.DecDigit"), CharGroupDefinition::create(
    SequenceCharGroupUnit::create(STR("0"), STR("9"))).get());

  // HexDigit : char '0'..'9', 'a'..'f', 'A'..'F';
  this->repository.set(STR("root:LexerDefs.HexDigit"), CharGroupDefinition::create(
    UnionCharGroupUnit::create({
      SequenceCharGroupUnit::create(STR("0"), STR("9")),
      SequenceCharGroupUnit::create(STR("a"), STR("f")),
      SequenceCharGroupUnit::create(STR("A"), STR("F"))
    })).get());

  // Letter : char 'a'..'z', 'A'..'Z', '_';
  this->repository.set(STR("root:LexerDefs.Letter"), CharGroupDefinition::create(
    UnionCharGroupUnit::create({
      SequenceCharGroupUnit::create(STR("a"), STR("z")),
      SequenceCharGroupUnit::create(STR("A"), STR("Z")),
      SequenceCharGroupUnit::create(STR("_"), STR("_"))
    })).get());

  // AnyCharNoEs : char ^('\\');
  this->repository.set(STR("root:LexerDefs.AnyCharNoEs"), CharGroupDefinition::create(
    InvertCharGroupUnit::create(
      SequenceCharGroupUnit::create(STR("\\"), STR("\\")))).get());

  // AnyCharNoEsOrSingleQuote : char ^("\\'");
  this->repository.set(STR("root:LexerDefs.AnyCharNoEsOrSingleQuote"), CharGroupDefinition::create(
    InvertCharGroupUnit::create(
      RandomCharGroupUnit::create(STR("\\'")))).get());

  // AnyCharNoEsOrDoubleQuote : char ^("\\\"");
  this->repository.set(STR("root:LexerDefs.AnyCharNoEsOrDoubleQuote"), CharGroupDefinition::create(
    InvertCharGroupUnit::create(
      RandomCharGroupUnit::create(STR("\\\"")))).get());

  // AnyCharNoReturn = ^('\\');
  this->repository.set(STR("root:LexerDefs.AnyCharNoReturn"), CharGroupDefinition::create(
    InvertCharGroupUnit::create(
      SequenceCharGroupUnit::create(STR("\n"), STR("\n")))).get());

  // Spacing : char " \n\r\t";
  this->repository.set(STR("root:LexerDefs.Spacing"), CharGroupDefinition::create(
    RandomCharGroupUnit::create(STR(" \r\n\t"))).get());
}


/**
 * Create a TokenDefinitionList and add all the required definitions for
 * the Core's grammar.
 */
void TestGrammarPlant::createTokenDefinitions()
{
  // Identifier : trule as { Letter + (Letter || DecDigit)*(0,endless) };
  this->repository.set(STR("root:LexerDefs.Identifier"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::TERM, SharedList::create({
         CharGroupTerm::create(STR("module:Letter")),
         MultiplyTerm::create({
           {TermElement::TERM, AlternateTerm::create({
              {TermElement::TERM, SharedList::create({
                 CharGroupTerm::create(STR("module:Letter")),
                 CharGroupTerm::create(STR("module:DecDigit"))
               })}
            })}
         })
       })}
    })},
   {SymbolDefElement::FLAGS, SymbolFlags::ROOT_TOKEN}
  }).get());

  // IntLiteral : trule as {
  //   (DecIntLiteral || BinIntLiteral || OctIntLiteral || HexIntLiteral) +
  //   ("u" || "U")*(0,1) + (("i" || "I") + DecIntLiteral)*(0,1)
  // };
  this->repository.set(STR("root:LexerDefs.IntLiteral"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::TERM, SharedList::create({
         AlternateTerm::create({
           {TermElement::TERM, SharedList::create({
              ReferenceTerm::create(STR("module:DecIntLiteral")),
              ReferenceTerm::create(STR("module:BinIntLiteral")),
              ReferenceTerm::create(STR("module:OctIntLiteral")),
              ReferenceTerm::create(STR("module:HexIntLiteral"))
           })}
         }),
         MultiplyTerm::create({
           {TermElement::MAX, Integer::create(1)},
           {TermElement::TERM, AlternateTerm::create({
              {TermElement::TERM, SharedList::create({
                 ConstTerm::create(0, STR("u")),
                 ConstTerm::create(0, STR("U"))
              })}
           })}
         }),
         MultiplyTerm::create({
           {TermElement::MAX, Integer::create(1)},
           {TermElement::TERM, ConcatTerm::create({
              {TermElement::TERM, SharedList::create({
                AlternateTerm::create({
                  {TermElement::TERM, SharedList::create({
                     ConstTerm::create(0, STR("i")),
                     ConstTerm::create(0, STR("I"))
                  })}
                }),
                ReferenceTerm::create(STR("module:DecIntLiteral"))
              })}
           })}
         })
       })}
    })},
    {SymbolDefElement::FLAGS, SymbolFlags::ROOT_TOKEN}
  }).get());

  // @inner DecIntLiteral : trule as { DecDigit*(1,endless) };
  this->repository.set(STR("root:LexerDefs.DecIntLiteral"), SymbolDefinition::create({
    {SymbolDefElement::TERM, MultiplyTerm::create({
      {TermElement::MIN, Integer::create(1)},
      {TermElement::TERM, CharGroupTerm::create(STR("module:DecDigit"))}
    })}
  }).get());

  // @inner BinIntLiteral : trule as { ("0b" || "0B") + BinDigit*(1,endless) };
  this->repository.set(STR("root:LexerDefs.BinIntLiteral"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::TERM, SharedList::create({
         AlternateTerm::create({
           {TermElement::TERM, SharedList::create({
              ConstTerm::create(0, STR("0b")),
              ConstTerm::create(0, STR("0B"))
           })}
         }),
         MultiplyTerm::create({
           {TermElement::MIN, Integer::create(1)},
           {TermElement::TERM, CharGroupTerm::create(STR("module:BinDigit"))}
         })
       })}
    })}
  }).get());

  // @inner OctIntLiteral : trule as { ("0o" || "0O") + OctDigit*(1,endless) };
  this->repository.set(STR("root:LexerDefs.OctIntLiteral"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::TERM, SharedList::create({
         AlternateTerm::create({
           {TermElement::TERM, SharedList::create({
              ConstTerm::create(0, STR("0o")),
              ConstTerm::create(0, STR("0O"))
           })}
         }),
         MultiplyTerm::create({
           {TermElement::MIN, Integer::create(1)},
           {TermElement::TERM, CharGroupTerm::create(STR("module:OctDigit"))}
         })
       })}
    })}
  }).get());

  // @inner HexIntLiteral : trule as { ("0h" || "0H") + HexDigit*(1,endless) };
  this->repository.set(STR("root:LexerDefs.HexIntLiteral"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::TERM, SharedList::create({
         AlternateTerm::create({
           {TermElement::TERM, SharedList::create({
              ConstTerm::create(0, STR("0h")),
              ConstTerm::create(0, STR("0H"))
           })}
         }),
         MultiplyTerm::create({
           {TermElement::MIN, Integer::create(1)},
           {TermElement::TERM, CharGroupTerm::create(STR("module:HexDigit"))}
         })
       })}
    })}
  }).get());

  // FloatLiteral : trule as {
  //   DecDigit*(1,endless) + FloatPostfix ||
  //   DecDigit*(1,endless) + FloatExponent + FloatPostfix*(0,1) ||
  //   DecDigit*(1,endless) + "." + DecDigit*(1,endless) +
  //     FloatExponent*(0,1) + FloatPostfix*(1,1)
  // };
  this->repository.set(STR("root:LexerDefs.FloatLiteral"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
      {TermElement::TERM, SharedList::create({
         ConcatTerm::create({
           {TermElement::TERM, SharedList::create({
              MultiplyTerm::create({
                {TermElement::MIN, Integer::create(1)},
                {TermElement::TERM, CharGroupTerm::create(STR("module:DecDigit"))}
              }),
              ReferenceTerm::create(STR("module:FloatPostfix"))
           })}
         }),
         ConcatTerm::create({
           {TermElement::TERM, SharedList::create({
              MultiplyTerm::create({
                {TermElement::MIN, Integer::create(1)},
                {TermElement::TERM, CharGroupTerm::create(STR("module:DecDigit"))}
              }),
              ReferenceTerm::create(STR("module:FloatExponent")),
              MultiplyTerm::create({
                {TermElement::MAX, Integer::create(1)},
                {TermElement::TERM, ReferenceTerm::create(STR("module:FloatPostfix"))}
              })
           })}
         }),
         ConcatTerm::create({
           {TermElement::TERM, SharedList::create({
              MultiplyTerm::create({
                {TermElement::TERM, CharGroupTerm::create(STR("module:DecDigit"))}
              }),
              ConstTerm::create(0, STR(".")),
              MultiplyTerm::create({
                {TermElement::MIN, Integer::create(1)},
                {TermElement::TERM, CharGroupTerm::create(STR("module:DecDigit"))}
              }),
              MultiplyTerm::create({
                {TermElement::MAX, Integer::create(1)},
                {TermElement::TERM, ReferenceTerm::create(STR("module:FloatExponent"))}
              }),
              MultiplyTerm::create({
                {TermElement::MAX, Integer::create(1)},
                {TermElement::TERM, ReferenceTerm::create(STR("module:FloatPostfix"))}
              })
           })}
         })
       })}
    })},
   {SymbolDefElement::FLAGS, SymbolFlags::ROOT_TOKEN}
  }).get());

  // @inner FloatExponent : trule as { ("e" || "E") + ("+" || "-")*(0,1) + DecDigit*(1,endless) };
  this->repository.set(STR("root:LexerDefs.FloatExponent"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::TERM, SharedList::create({
         AlternateTerm::create({
           {TermElement::TERM, SharedList::create({
              ConstTerm::create(0, STR("e")),
              ConstTerm::create(0, STR("E"))
           })}
         }),
         MultiplyTerm::create({
           {TermElement::MAX, Integer::create(1)},
           {TermElement::TERM, AlternateTerm::create({
              {TermElement::TERM, SharedList::create({
                 ConstTerm::create(0, STR("+")),
                 ConstTerm::create(0, STR("-"))
              })}
           })}
         }),
         MultiplyTerm::create({
           {TermElement::MIN, Integer::create(1)},
           {TermElement::TERM, CharGroupTerm::create(STR("module:DecDigit"))}
         })
       })}
    })}
  }).get());

  // @inner FloatPostfix : trule as { ("f" || "F") + DecDigit*(0,endless) };
  this->repository.set(STR("root:LexerDefs.FloatPostfix"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::TERM, SharedList::create({
         AlternateTerm::create({
           {TermElement::TERM, SharedList::create({
              ConstTerm::create(0, STR("f")),
              ConstTerm::create(0, STR("F"))
           })}
         }),
         MultiplyTerm::create({
           {TermElement::TERM, CharGroupTerm::create(STR("module:DecDigit"))}
         })
       })}
    })}
  }).get());

  // CharLiteral : trule as { "'" + EsCharWithDoubleQuote + "'" + CharCodePostfix*(0,1) };
  // TODO: Add the char_code_postfix part
  this->repository.set(STR("root:LexerDefs.CharLiteral"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::TERM, SharedList::create({
         ConstTerm::create(0, STR("'")),
         ReferenceTerm::create(STR("module:EsCharWithDoubleQuote")),
         ConstTerm::create(0, STR("'"))
       })}
    })},
    {SymbolDefElement::FLAGS, SymbolFlags::ROOT_TOKEN}
  }).get());

  // StringLiteral : trule as {
  //   StringLiteralPart + (Spacing*(0,endless) + StringLiteralPart)*(0,endless) +
  //   CharCodePostfix*(0,1)
  // };
  // TODO: Add the CharCodePostfix part
  this->repository.set(STR("root:LexerDefs.StringLiteral"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::TERM, SharedList::create({
         ReferenceTerm::create(STR("module:StringLiteralPart")),
         MultiplyTerm::create({
           {TermElement::TERM, ConcatTerm::create({
              {TermElement::TERM, SharedList::create({
                 MultiplyTerm::create({
                   {TermElement::TERM, CharGroupTerm::create(STR("module:Spacing"))}
                 }),
                 ReferenceTerm::create(STR("module:StringLiteralPart"))
              })}
           })}
         })
       })}
    })},
    {SymbolDefElement::HANDLER, this->stringLiteralHandler},
    {SymbolDefElement::FLAGS, SymbolFlags::ROOT_TOKEN}
  }).get());

  // @inner StringLiteralPart : trule as { "\"" + EsCharWithSingleQuote*(0,endless) + "\"" };
  this->repository.set(STR("root:LexerDefs.StringLiteralPart"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::TERM, SharedList::create({
         ConstTerm::create(0, STR("\"")),
         MultiplyTerm::create({
           {TermElement::TERM, ReferenceTerm::create(STR("module:EsCharWithSingleQuote"))}
         }),
         ConstTerm::create(0, STR("\""))
       })}
    })}
  }).get());

  // @inner EsCharWithSingleQuote : trule as {
  //   AnyCharNoEsOrDoubleQuote || EsSequence ||
  //   alternate (root.TokenData.escapeSequences:es)->( es )
  // };
  // TODO: Add the heap.escape_sequences part
  this->repository.set(STR("root:LexerDefs.EsCharWithSingleQuote"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
      {TermElement::TERM, SharedList::create({
         CharGroupTerm::create(STR("module:AnyCharNoEsOrDoubleQuote")),
         ReferenceTerm::create(STR("module:EsSequence"))
       })}
    })}
  }).get());

  // @inner EsCharWithDoubleQuote : trule as {
  //   AnyCharNoEsOrSingleQuote || EsSequence ||
  //   alternate (root.TokenData.escapeSequences:es)->( es )
  // };
  // TODO: Add the root.TokenData.escapeSequences part
  this->repository.set(STR("root:LexerDefs.EsCharWithDoubleQuote"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
      {TermElement::TERM, SharedList::create({
         CharGroupTerm::create(STR("module:AnyCharNoEsOrSingleQuote")),
         ReferenceTerm::create(STR("module:EsSequence"))
       })}
    })}
  }).get());

  // @inner EsCharWithQuotes : trule as {
  //   AnyCharNoEs || EsSequence || alternate (root.TokenData.escapeSequences:es)->( es )
  // };
  // TODO: Add the heap.escape_sequences part
  this->repository.set(STR("root:LexerDefs.EsCharWithQuotes"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
      {TermElement::TERM, SharedList::create({
         CharGroupTerm::create(STR("module:AnyCharNoEs")),
         ReferenceTerm::create(STR("module:EsSequence"))
       })}
    })}
  }).get());

  // @inner EsSequence : trule as {
  //   '\\' + ('c' + HexDigit*(2,2) ||
  //           'u' + HexDigit*(4,4) ||
  //           'w' + HexDigit*(8,8) ||
  //           'n' || 't' || 'r')
  // };
  this->repository.set(STR("root:LexerDefs.EsSequence"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::TERM, SharedList::create({
         ConstTerm::create(0, STR("\\")),
         AlternateTerm::create({
           {TermElement::TERM, SharedList::create({
              ConstTerm::create(0, STR("n")),
              ConstTerm::create(0, STR("t")),
              ConstTerm::create(0, STR("r")),
              ConcatTerm::create({
                {TermElement::TERM, SharedList::create({
                   ConstTerm::create(0, STR("c")),
                   CharGroupTerm::create(STR("module:HexDigit")),
                   CharGroupTerm::create(STR("module:HexDigit"))
                })}
              }),
              ConcatTerm::create({
                {TermElement::TERM, SharedList::create({
                   ConstTerm::create(0, STR("u")),
                   CharGroupTerm::create(STR("module:HexDigit")),
                   CharGroupTerm::create(STR("module:HexDigit")),
                   CharGroupTerm::create(STR("module:HexDigit")),
                   CharGroupTerm::create(STR("module:HexDigit"))
                })}
              }),
              ConcatTerm::create({
                {TermElement::TERM, SharedList::create({
                   ConstTerm::create(0, STR("w")),
                   CharGroupTerm::create(STR("module:HexDigit")),
                   CharGroupTerm::create(STR("module:HexDigit")),
                   CharGroupTerm::create(STR("module:HexDigit")),
                   CharGroupTerm::create(STR("module:HexDigit")),
                   CharGroupTerm::create(STR("module:HexDigit")),
                   CharGroupTerm::create(STR("module:HexDigit")),
                   CharGroupTerm::create(STR("module:HexDigit")),
                   CharGroupTerm::create(STR("module:HexDigit"))
                })}
              })
           })}
         })
       })}
    })}
  }).get());

  //// IGNORED TOKENS

  // ignore { Spacing*(1,endless) };
  this->repository.set(STR("root:LexerDefs.IgnoredSpaces"), SymbolDefinition::create({
    {SymbolDefElement::TERM, MultiplyTerm::create({
      {TermElement::MIN, Integer::create(1)},
      {TermElement::TERM, CharGroupTerm::create(STR("module:Spacing"))}
    })},
    {SymbolDefElement::FLAGS, SymbolFlags::ROOT_TOKEN|SymbolFlags::IGNORED_TOKEN}
  }).get());

  // @minimum ignore { "//" + any*(0,endless) + "\n" }
  // For now this is implemented as:
  // ignore { "//" + AnyCharNoReturn*(0,endless) + "\n" }
  // because the lexer still doesn't support the @minimum modifier.
  this->repository.set(STR("root:LexerDefs.LineComment"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::TERM, SharedList::create({
         ConstTerm::create(0, STR("//")),
         MultiplyTerm::create({
           {TermElement::TERM, CharGroupTerm::create(STR("module:AnyCharNoReturn"))}
         }),
         ConstTerm::create(0, STR("\n"))
       })}
    })},
    {SymbolDefElement::FLAGS, SymbolFlags::ROOT_TOKEN|SymbolFlags::IGNORED_TOKEN}
  }).get());
}


/**
 * Create a Production_Definition_List and add all the required definitions for
 * the Core's grammar. The created list will have assigned parsing handlers.
 */
void TestGrammarPlant::createProductionDefinitions()
{
  // TODO: Replace the generic parsing handler for the root with the appropriate one.
  // Program = Statement*v1;
  this->repository.set(STR("Program"), SymbolDefinition::create({
    {SymbolDefElement::TERM, MultiplyTerm::create({
       {TermElement::PRIORITY, std::make_shared<Integer>(1)},
       {TermElement::MIN, std::make_shared<Integer>(1)},
       {TermElement::TERM, ReferenceTerm::create(STR("root:Statement"))}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler}
  }).get());

  // Statement = Command + ";" || Expression + ";";
  this->repository.set(STR("Statement"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::ESPI, 1},
       {TermElement::TERM, SharedList::create({
          ReferenceTerm::create(STR("root:SubStatement")),
          TokenTerm::create(0, this->constTokenId, STR(";"))
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler},
  }).get());

  // Statement = Command || Expression;
  this->repository.set(STR("SubStatement"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
       {TermElement::TERM, SharedList::create({
          ReferenceTerm::create(STR("root:Command")),
          ReferenceTerm::create(STR("root:Expression"))
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler}
  }).get());

  // Command = "do" + Expression;
  this->repository.set(STR("Command"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::TERM, SharedList::create({
          TokenTerm::create(0, this->constTokenId, STR("do")),
          ReferenceTerm::create(STR("root:Expression"))
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler},
    {SymbolDefElement::FLAGS, ParsingFlags::ENFORCE_PROD_OBJ}
  }).get());

  // Set = "{" + Statement*v + "}";
  this->repository.set(STR("Set"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::TERM, SharedList::create({
          TokenTerm::create(0, this->constTokenId, STR("{")),
          MultiplyTerm::create({
            {TermElement::PRIORITY, std::make_shared<Integer>(1)},
            {TermElement::FLAGS, ParsingFlags::PASS_ITEMS_UP},
            {TermElement::TERM, ReferenceTerm::create(STR("root:Statement"))}
          }),
          TokenTerm::create(0, this->constTokenId, STR("}"))
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler},
  }).get());

  // Expression = ListExp + ("?" + Expression)*o;
  this->repository.set(STR("Expression"), SymbolDefinition::create({
     {SymbolDefElement::TERM, ConcatTerm::create({
        {TermElement::TERM, SharedList::create({
           ReferenceTerm::create(STR("root:ListExp")),
           MultiplyTerm::create({
             {TermElement::PRIORITY, std::make_shared<Integer>(1)},
             {TermElement::FLAGS, ParsingFlags::PASS_ITEMS_UP},
             {TermElement::MAX, std::make_shared<Integer>(1)},
             {TermElement::TERM, ConcatTerm::create({
                {TermElement::TERM, SharedList::create({
                   TokenTerm::create(0, this->constTokenId, STR("?")),
                   ReferenceTerm::create(STR("root:Expression"))
                 })}
              })}
           })
         })}
      })},
     {SymbolDefElement::HANDLER, this->parsingHandler},
   }).get());

  // ListExp = ","*v + ColonPairExp + ("," + ColonPairExp*o)*v;
  this->repository.set(STR("ListExp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::TERM, SharedList::create({
          MultiplyTerm::create({
            {TermElement::PRIORITY, std::make_shared<Integer>(1)},
            {TermElement::FLAGS, ParsingFlags::PASS_ITEMS_UP},
            {TermElement::TERM, TokenTerm::create(0, this->constTokenId, STR(","))}
          }),
          ReferenceTerm::create(STR("root:ColonPairExp")),
          MultiplyTerm::create({
            {TermElement::PRIORITY, std::make_shared<Integer>(1)},
            {TermElement::FLAGS, ParsingFlags::PASS_ITEMS_UP},
            {TermElement::TERM, ConcatTerm::create({
               {TermElement::FLAGS, ParsingFlags::PASS_ITEMS_UP},
               {TermElement::TERM, SharedList::create({
                  TokenTerm::create(0, this->constTokenId, STR(",")),
                  MultiplyTerm::create({
                    {TermElement::PRIORITY, std::make_shared<Integer>(1)},
                    {TermElement::FLAGS, ParsingFlags::PASS_ITEMS_UP},
                    {TermElement::MAX, std::make_shared<Integer>(1)},
                    {TermElement::TERM, ReferenceTerm::create(STR("root:ColonPairExp"))}
                  })
                })}
             })}
          })
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler}
  }).get());

  // ColonPairExp = AssignmentExp + (":" + AssignmentExp)*o;
  this->repository.set(STR("ColonPairExp"), SymbolDefinition::create({
     {SymbolDefElement::TERM, ConcatTerm::create({
        {TermElement::TERM, SharedList::create({
           ReferenceTerm::create(STR("root:AssignmentExp")),
           MultiplyTerm::create({
             {TermElement::PRIORITY, std::make_shared<Integer>(1)},
             {TermElement::FLAGS, ParsingFlags::PASS_ITEMS_UP},
             {TermElement::MAX, std::make_shared<Integer>(1)},
             {TermElement::TERM, ConcatTerm::create({
                {TermElement::FLAGS, ParsingFlags::PASS_ITEMS_UP},
                {TermElement::TERM, SharedList::create({
                   TokenTerm::create(0, this->constTokenId, STR(":")),
                   ReferenceTerm::create(STR("root:AssignmentExp"))
                 })}
              })}
           })
         })}
      })},
     {SymbolDefElement::HANDLER, this->parsingHandler}
   }).get());

  // AssignmentExp = LogExp + (AssignmentOp + AssignmentExp)*o;
  this->repository.set(STR("AssignmentExp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::TERM, SharedList::create({
          ReferenceTerm::create(STR("root:LogExp")),
          MultiplyTerm::create({
            {TermElement::PRIORITY, std::make_shared<Integer>(1)},
            {TermElement::FLAGS, ParsingFlags::PASS_ITEMS_UP},
            {TermElement::MAX, std::make_shared<Integer>(1)},
            {TermElement::TERM, ConcatTerm::create({
               {TermElement::FLAGS, ParsingFlags::PASS_ITEMS_UP},
               {TermElement::TERM, SharedList::create({
                  ReferenceTerm::create(STR("root:AssignmentOp")),
                  ReferenceTerm::create(STR("root:AssignmentExp"))
                })}
             })}
          })
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler}
  }).get());

  // LogExp = ComparisonExp + (LogOp + ComparisonExp)*v;
  this->repository.set(STR("LogExp"), SymbolDefinition::create({
     {SymbolDefElement::TERM, ConcatTerm::create({
        {TermElement::TERM, SharedList::create({
           ReferenceTerm::create(STR("root:ComparisonExp")),
           MultiplyTerm::create({
             {TermElement::PRIORITY, std::make_shared<Integer>(1)},
             {TermElement::FLAGS, ParsingFlags::PASS_ITEMS_UP},
             {TermElement::TERM, ConcatTerm::create({
                {TermElement::FLAGS, ParsingFlags::PASS_ITEMS_UP},
                {TermElement::TERM, SharedList::create({
                   ReferenceTerm::create(STR("root:LogOp")),
                   ReferenceTerm::create(STR("root:ComparisonExp"))
                 })}
              })}
           })
         })}
      })},
     {SymbolDefElement::HANDLER, this->parsingHandler}
   }).get());

  // ComparisonExp = AddExp + (ComparisonOp + AddExp)*v;
  this->repository.set(STR("ComparisonExp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::TERM, SharedList::create({
          ReferenceTerm::create(STR("root:AddExp")),
          MultiplyTerm::create({
            {TermElement::PRIORITY, std::make_shared<Integer>(1)},
            {TermElement::FLAGS, ParsingFlags::PASS_ITEMS_UP},
            {TermElement::TERM, ConcatTerm::create({
               {TermElement::FLAGS, ParsingFlags::PASS_ITEMS_UP},
               {TermElement::TERM, SharedList::create({
                  ReferenceTerm::create(STR("root:ComparisonOp")),
                  ReferenceTerm::create(STR("root:AddExp"))
                })}
             })}
          })
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler}
  }).get());

  // AddExp = MulExp + (AddOp + MulExp)*v;
  this->repository.set(STR("AddExp"), SymbolDefinition::create({
     {SymbolDefElement::TERM, ConcatTerm::create({
        {TermElement::TERM, SharedList::create({
           ReferenceTerm::create(STR("root:MulExp")),
           MultiplyTerm::create({
             {TermElement::PRIORITY, std::make_shared<Integer>(1)},
             {TermElement::FLAGS, ParsingFlags::PASS_ITEMS_UP},
             {TermElement::TERM, ConcatTerm::create({
                {TermElement::FLAGS, ParsingFlags::PASS_ITEMS_UP},
                {TermElement::TERM, SharedList::create({
                   ReferenceTerm::create(STR("root:AddOp")),
                   ReferenceTerm::create(STR("root:MulExp"))
                 })}
              })}
           })
         })}
      })},
     {SymbolDefElement::HANDLER, this->parsingHandler}
   }).get());

  // MulExp = BitwiseExp + (MulOp + BitwiseExp)*v;
  this->repository.set(STR("MulExp"), SymbolDefinition::create({
     {SymbolDefElement::TERM, ConcatTerm::create({
        {TermElement::TERM, SharedList::create({
           ReferenceTerm::create(STR("root:BitwiseExp")),
           MultiplyTerm::create({
             {TermElement::PRIORITY, std::make_shared<Integer>(1)},
             {TermElement::FLAGS, ParsingFlags::PASS_ITEMS_UP},
             {TermElement::TERM, ConcatTerm::create({
                {TermElement::FLAGS, ParsingFlags::PASS_ITEMS_UP},
                {TermElement::TERM, SharedList::create({
                   ReferenceTerm::create(STR("root:MulOp")),
                   ReferenceTerm::create(STR("root:BitwiseExp"))
                 })}
              })}
           })
         })}
      })},
     {SymbolDefElement::HANDLER, this->parsingHandler}
   }).get());

  // BitwiseExp = UnaryExp + (BitwiseOp + UnaryExp)*v;
  this->repository.set(STR("BitwiseExp"), SymbolDefinition::create({
     {SymbolDefElement::TERM, ConcatTerm::create({
        {TermElement::TERM, SharedList::create({
           ReferenceTerm::create(STR("root:UnaryExp")),
           MultiplyTerm::create({
             {TermElement::PRIORITY, std::make_shared<Integer>(1)},
             {TermElement::FLAGS, ParsingFlags::PASS_ITEMS_UP},
             {TermElement::TERM, ConcatTerm::create({
                {TermElement::FLAGS, ParsingFlags::PASS_ITEMS_UP},
                {TermElement::TERM, SharedList::create({
                   ReferenceTerm::create(STR("root:BitwiseOp")),
                   ReferenceTerm::create(STR("root:UnaryExp"))
                 })}
              })}
           })
         })}
      })},
     {SymbolDefElement::HANDLER, this->parsingHandler}
   }).get());

  // UnaryExp = PrefixOp*o + FunctionalExp + PostfixOp*o;
  this->repository.set(STR("UnaryExp"), SymbolDefinition::create({
     {SymbolDefElement::TERM, ConcatTerm::create({
        {TermElement::TERM, SharedList::create({
           MultiplyTerm::create({
             {TermElement::PRIORITY, std::make_shared<Integer>(1)},
             {TermElement::FLAGS, ParsingFlags::PASS_ITEMS_UP},
             {TermElement::MAX, std::make_shared<Integer>(1)},
             {TermElement::TERM, ReferenceTerm::create(STR("root:PrefixOp"))}
           }),
           ReferenceTerm::create(STR("root:FunctionalExp")),
           MultiplyTerm::create({
             {TermElement::PRIORITY, std::make_shared<Integer>(1)},
             {TermElement::FLAGS, ParsingFlags::PASS_ITEMS_UP},
             {TermElement::MAX, std::make_shared<Integer>(1)},
             {TermElement::TERM, ReferenceTerm::create(STR("root:PostfixOp"))}
           })
         })}
      })},
     {SymbolDefElement::HANDLER, this->parsingHandler}
   }).get());

  // FunctionalExp = Operand + (LinkExp || ParamPassExp)*v;
  this->repository.set(STR("FunctionalExp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::TERM, SharedList::create({
          ReferenceTerm::create(STR("root:Operand")),
          MultiplyTerm::create({
            {TermElement::PRIORITY, std::make_shared<Integer>(1)},
            {TermElement::FLAGS, ParsingFlags::PASS_ITEMS_UP},
            {TermElement::TERM, AlternateTerm::create({
               {TermElement::FLAGS, ParsingFlags::PASS_ITEMS_UP},
               {TermElement::TERM, SharedList::create({
                  ReferenceTerm::create(STR("root:LinkExp")),
                  ReferenceTerm::create(STR("root:ParamPassExp"))
                })}
             })}
          })
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler}
  }).get());

  // LinkExp = LinkOp + Operand;
  this->repository.set(STR("LinkExp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::TERM, SharedList::create({
          ReferenceTerm::create(STR("root:LinkOp")),
          ReferenceTerm::create(STR("root:Operand"))
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler},
  }).get());

  // ParamPassExp = "(" + Expression*o + ")" || "[" + Expression*o + "]";
  this->repository.set(STR("ParamPassExp"), SymbolDefinition::create({
     {SymbolDefElement::TERM, AlternateTerm::create({
        {TermElement::TERM, SharedList::create({
           ConcatTerm::create({
             {TermElement::FLAGS, ParsingFlags::PASS_ITEMS_UP},
             {TermElement::TERM, SharedList::create({
                TokenTerm::create(0, this->constTokenId, STR("(")),
                MultiplyTerm::create({
                  {TermElement::PRIORITY, std::make_shared<Integer>(1)},
                  {TermElement::FLAGS, ParsingFlags::PASS_ITEMS_UP},
                  {TermElement::MAX, std::make_shared<Integer>(1)},
                  {TermElement::TERM, ReferenceTerm::create(STR("root:Expression"))}
                }),
                TokenTerm::create(0, this->constTokenId, STR(")"))
              })}
           }),
           ConcatTerm::create({
             {TermElement::FLAGS, ParsingFlags::PASS_ITEMS_UP},
             {TermElement::TERM, SharedList::create({
                TokenTerm::create(0, this->constTokenId, STR("[")),
                MultiplyTerm::create({
                  {TermElement::PRIORITY, std::make_shared<Integer>(1)},
                  {TermElement::FLAGS, ParsingFlags::PASS_ITEMS_UP},
                  {TermElement::MAX, std::make_shared<Integer>(1)},
                  {TermElement::TERM, ReferenceTerm::create(STR("root:Expression"))}
                }),
                TokenTerm::create(0, this->constTokenId, STR("]"))
              })}
           })
         })}
      })},
     {SymbolDefElement::HANDLER, this->parsingHandler},
   }).get());

  // Operand = Parameter || Command || Set || "(" + Expression*o + ")" || "[" + Expression*o + "]";
  this->repository.set(STR("Operand"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
       {TermElement::TERM, SharedList::create({
          ReferenceTerm::create(STR("root:Parameter")),
          ReferenceTerm::create(STR("root:Set")),
          ConcatTerm::create({
            {TermElement::FLAGS, ParsingFlags::PASS_ITEMS_UP},
            {TermElement::TERM, SharedList::create({
               TokenTerm::create(0, this->constTokenId, STR("(")),
               MultiplyTerm::create({
                 {TermElement::PRIORITY, std::make_shared<Integer>(1)},
                 {TermElement::FLAGS, ParsingFlags::PASS_ITEMS_UP},
                 {TermElement::MAX, std::make_shared<Integer>(1)},
                 {TermElement::TERM, ReferenceTerm::create(STR("root:SubStatement"))}
               }),
               TokenTerm::create(0, this->constTokenId, STR(")"))
             })}
          }),
          ConcatTerm::create({
            {TermElement::FLAGS, ParsingFlags::PASS_ITEMS_UP},
            {TermElement::TERM, SharedList::create({
               TokenTerm::create(0, this->constTokenId, STR("[")),
               MultiplyTerm::create({
                 {TermElement::PRIORITY, std::make_shared<Integer>(1)},
                 {TermElement::FLAGS, ParsingFlags::PASS_ITEMS_UP},
                 {TermElement::MAX, std::make_shared<Integer>(1)},
                 {TermElement::TERM, ReferenceTerm::create(STR("root:SubStatement"))}
               }),
               TokenTerm::create(0, this->constTokenId, STR("]"))
             })}
          })
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler},
  }).get());

  // Parameter = identifier || Literal;
  this->repository.set(STR("Parameter"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
       {TermElement::TERM, SharedList::create({
          TokenTerm::create(0, std::make_shared<Integer>(ID_GENERATOR->getId(STR("LexerDefs.Identifier")))),
          ReferenceTerm::create(STR("root:Literal"))
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler}
  }).get());

  // Literal = int_literal || float_literal || char_literal || string_literal;
  this->repository.set(STR("Literal"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
       {TermElement::TERM, SharedList::create({
          TokenTerm::create(0, std::make_shared<Integer>(ID_GENERATOR->getId(STR("LexerDefs.IntLiteral")))),
          TokenTerm::create(0, std::make_shared<Integer>(ID_GENERATOR->getId(STR("LexerDefs.FloatLiteral")))),
          TokenTerm::create(0, std::make_shared<Integer>(ID_GENERATOR->getId(STR("LexerDefs.CharLiteral")))),
          TokenTerm::create(0, std::make_shared<Integer>(ID_GENERATOR->getId(STR("LexerDefs.StringLiteral"))))
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler}
  }).get());

  // AssignmentOp = ":=" || "+=" || "-=" || "*=" || "/=" || "%=" || "&=" || "|=" || "$=" || "^:=" || "<<=" || ">>=";
  this->repository.set(STR("AssignmentOp"), SymbolDefinition::create({
     {SymbolDefElement::TERM, AlternateTerm::create({
        {TermElement::TERM, SharedList::create({
           TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR(":=")),
           TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR("+=")),
           TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR("-=")),
           TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR("*=")),
           TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR("/=")),
           TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR("%=")),
           TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR("&=")),
           TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR("|=")),
           TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR("$=")),
           TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR("^:=")),
           TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR("<<=")),
           TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR(">>="))
         })}
      })},
     {SymbolDefElement::HANDLER, this->parsingHandler}
   }).get());

  // ComparisonOp = "=" || "^=" || "==" || "^==" || "<" || ">" || "<=" || ">=";
  this->repository.set(STR("ComparisonOp"), SymbolDefinition::create({
     {SymbolDefElement::TERM, AlternateTerm::create({
        {TermElement::TERM, SharedList::create({
           TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR("=")),
           TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR("^=")),
           TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR("==")),
           TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR("^==")),
           TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR("<")),
           TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR(">")),
           TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR("<=")),
           TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR(">="))
         })}
      })},
     {SymbolDefElement::HANDLER, this->parsingHandler}
   }).get());

  // AddOp = "+" || "-";
  this->repository.set(STR("AddOp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
       {TermElement::TERM, SharedList::create({
          TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR("+")),
          TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR("-"))
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler}
  }).get());

  // MulOp = "*" || "/" || "%";
  this->repository.set(STR("MulOp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
       {TermElement::TERM, SharedList::create({
          TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR("*")),
          TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR("/")),
          TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR("%"))
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler}
  }).get());

  // BitwiseOp = "|" || "^|" || "$" || "^$" || "&" || "^&" || "<<" || ">>";
  this->repository.set(STR("BitwiseOp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
       {TermElement::TERM, SharedList::create({
          TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR("|")),
          TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR("^|")),
          TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR("$")),
          TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR("^$")),
          TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR("&")),
          TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR("^&")),
          TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR("<<")),
          TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR(">>"))
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler}
  }).get());

  // LogOp = "||" || "^||" || "$$" || "^$$" || "&&" || "^&&";
  this->repository.set(STR("LogOp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
       {TermElement::TERM, SharedList::create({
          TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR("||")),
          TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR("^||")),
          TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR("$$")),
          TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR("^$$")),
          TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR("&&")),
          TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR("^&&"))
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler}
  }).get());

  // PrefixOp = "++" || "--" || "+" || "-" || "^" || "^^";
  this->repository.set(STR("PrefixOp"), SymbolDefinition::create({
     {SymbolDefElement::TERM, AlternateTerm::create({
        {TermElement::TERM, SharedList::create({
           TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR("++")),
           TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR("--")),
           TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR("+")),
           TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR("-")),
           TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR("^")),
           TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR("^^"))
         })}
      })},
     {SymbolDefElement::HANDLER, this->parsingHandler}
   }).get());

  // PostfixOp = "++" || "--";
  this->repository.set(STR("PostfixOp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
       {TermElement::TERM, SharedList::create({
          TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR("++")),
          TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR("--"))
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler}
  }).get());

  // LinkOp = "." || "->" || "=>";
  this->repository.set(STR("LinkOp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
       {TermElement::TERM, SharedList::create({
          TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR(".")),
          TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR("->")),
          TokenTerm::create(ParsingFlags::ENFORCE_TOKEN_OBJ, this->constTokenId, STR("=>"))
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler}
  }).get());
}


SharedPtr<Data::SymbolDefinition> TestGrammarPlant::createConstTokenDef(Char const *text)
{
  return SymbolDefinition::create({{SymbolDefElement::TERM, ConstTerm::create(0, text)},
                                   {SymbolDefElement::FLAGS, SymbolFlags::ROOT_TOKEN},
                                   {SymbolDefElement::HANDLER, this->constTokenHandler}});
}

} } // namespace
