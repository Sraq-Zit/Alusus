/**
 * @file Spp/CodeGen/ExtraDataAccessor.h
 * Contains the header of class Spp::CodeGen::ExtraDataAccessor.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_CODEGEN_EXTRADATAACCESSOR_H
#define SPP_CODEGEN_EXTRADATAACCESSOR_H

#define DEFINE_EXTRA_ACCESSORS(name) \
  public: template <class DT, class OT> inline DT* tryGet##name(OT *object) { \
    return tryGetExtra<DT>(object, this->id##name.c_str()); \
  } \
  public: template <class DT, class OT> inline DT* get##name(OT *object) { \
    return getExtra<DT>(object, this->id##name.c_str()); \
  } \
  public: template <class DT, class OT> inline void set##name(OT *object, SharedPtr<DT> const &data) { \
    setExtra(object, this->id##name.c_str(), data); \
  } \
  public: template <class OT> inline void remove##name(OT *object) { \
    removeExtra(object, this->id##name.c_str()); \
  }

namespace Spp::CodeGen
{

class ExtraDataAccessor : public TiObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(ExtraDataAccessor, TiObject, "Spp.CodeGen", "Spp", "alusus.org");


  //============================================================================
  // Member Variables

  private: Str idPrefix;
  private: Str idCodeGenData;
  private: Str idAutoCtor;
  private: Str idAutoDtor;
  private: Str idCodeGenFailed;
  private: Str idInitStatementGenIndex;


  //============================================================================
  // Constructor & Destructor

  public: ExtraDataAccessor(Char const *prefix = "")
  {
    this->setIdPrefix(prefix);
  }


  //============================================================================
  // Member Functions

  public: void setIdPrefix(Char const *prefix)
  {
    this->idPrefix = prefix;
    this->idCodeGenData = this->idPrefix + S("codeGenData");
    this->idAutoCtor = this->idPrefix + S("autoCtor");
    this->idAutoDtor = this->idPrefix + S("autoDtor");
    this->idCodeGenFailed = this->idPrefix + S("codeGenFailed");
    this->idInitStatementGenIndex = this->idPrefix + S("initStatementGenIndex");
  }

  public: Str const& getIdPrefix() const
  {
    return this->idPrefix;
  }

  DEFINE_EXTRA_ACCESSORS(CodeGenData);
  DEFINE_EXTRA_ACCESSORS(AutoCtor);
  DEFINE_EXTRA_ACCESSORS(AutoDtor);

  // didCodeGenFail

  public:
  template <class OT, typename std::enable_if<std::is_base_of<Core::Data::Ast::MetaHaving, OT>::value, int>::type = 0>
  inline Bool didCodeGenFail(OT *object)
  {
    auto f = object->getExtra(this->idCodeGenFailed.c_str()).template ti_cast_get<TiBool>();
    return f && f->get();
  }

  public:
  template <class OT, typename std::enable_if<!std::is_base_of<Core::Data::Ast::MetaHaving, OT>::value, int>::type = 0>
  inline Bool didCodeGenFail(OT *object)
  {
    auto metadata = ti_cast<Core::Data::Ast::MetaHaving>(object);
    if (metadata == 0) return false;
    auto f = metadata->getExtra(this->idCodeGenFailed.c_str()).template ti_cast_get<TiBool>();
    return f && f->get();
  }

  // setCodeGenFailed

  public:
  template <class OT, typename std::enable_if<std::is_base_of<Core::Data::Ast::MetaHaving, OT>::value, int>::type = 0>
  inline void setCodeGenFailed(OT *object, Bool f)
  {
    object->setExtra(this->idCodeGenFailed.c_str(), TiBool::create(f));
  }

  public:
  template <class OT, typename std::enable_if<!std::is_base_of<Core::Data::Ast::MetaHaving, OT>::value, int>::type = 0>
  inline void setCodeGenFailed(OT *object, Bool f)
  {
    auto metadata = ti_cast<Core::Data::Ast::MetaHaving>(object);
    if (metadata == 0) {
      throw EXCEPTION(InvalidArgumentException, S("object"), S("Object does not implement the MetaHaving interface."));
    }
    metadata->setExtra(this->idCodeGenFailed.c_str(), TiBool::create(f));
  }

  // resetCodeGenFailed

  public:
  template <class OT, typename std::enable_if<std::is_base_of<Core::Data::Ast::MetaHaving, OT>::value, int>::type = 0>
  inline void resetCodeGenFailed(OT *object)
  {
    object->removeExtra(this->idCodeGenFailed.c_str());
  }

  public:
  template <class OT, typename std::enable_if<!std::is_base_of<Core::Data::Ast::MetaHaving, OT>::value, int>::type = 0>
  inline void resetCodeGenFailed(OT *object)
  {
    auto metadata = ti_cast<Core::Data::Ast::MetaHaving>(object);
    if (metadata == 0) {
      throw EXCEPTION(InvalidArgumentException, S("object"), S("Object does not implement the MetaHaving interface."));
    }
    metadata->removeExtra(this->idCodeGenFailed.c_str());
  }

  // getInitStatementsGenIndex

  public:
  template <class OT, typename std::enable_if<std::is_base_of<Core::Data::Ast::MetaHaving, OT>::value, int>::type = 0>
  inline Int getInitStatementsGenIndex(OT *object)
  {
    auto i = object->getExtra(this->idInitStatementGenIndex.c_str()).template ti_cast_get<TiInt>();
    return i == 0 ? 0 : i->get();
  }

  public:
  template <class OT, typename std::enable_if<!std::is_base_of<Core::Data::Ast::MetaHaving, OT>::value, int>::type = 0>
  inline Int getInitStatementsGenIndex(OT *object)
  {
    auto metadata = ti_cast<Core::Data::Ast::MetaHaving>(object);
    if (metadata == 0) return false;
    auto i = metadata->getExtra(this->idInitStatementGenIndex.c_str()).template ti_cast_get<TiInt>();
    return i == 0 ? 0 : i->get();
  }

  // setInitStatementsGenIndex

  template <class OT, typename std::enable_if<std::is_base_of<Core::Data::Ast::MetaHaving, OT>::value, int>::type = 0>
  inline void setInitStatementsGenIndex(OT *object, Int i)
  {
    auto index = object->getExtra(this->idInitStatementGenIndex.c_str()).template ti_cast_get<TiInt>();
    if (index == 0) {
      object->setExtra(this->idInitStatementGenIndex.c_str(), TiInt::create(i));
    } else {
      index->set(i);
    }
  }

  template <class OT, typename std::enable_if<!std::is_base_of<Core::Data::Ast::MetaHaving, OT>::value, int>::type = 0>
  inline void setInitStatementsGenIndex(OT *object, Int i)
  {
    auto metadata = ti_cast<Core::Data::Ast::MetaHaving>(object);
    if (metadata == 0) {
      throw EXCEPTION(InvalidArgumentException, S("object"), S("Object does not implement the MetaHaving interface."));
    }
    auto index = metadata->getExtra(this->idInitStatementGenIndex.c_str()).template ti_cast_get<TiInt>();
    if (index == 0) {
      metadata->setExtra(this->idInitStatementGenIndex.c_str(), TiInt::create(i));
    } else {
      index->set(i);
    }
  }

  // resetInitStatementsGenIndex

  template <class OT, typename std::enable_if<std::is_base_of<Core::Data::Ast::MetaHaving, OT>::value, int>::type = 0>
  inline void resetInitStatementsGenIndex(OT *object)
  {
    object->removeExtra(this->idInitStatementGenIndex.c_str());
  }

  template <class OT, typename std::enable_if<!std::is_base_of<Core::Data::Ast::MetaHaving, OT>::value, int>::type = 0>
  inline void resetInitStatementsGenIndex(OT *object)
  {
    auto metadata = ti_cast<Core::Data::Ast::MetaHaving>(object);
    if (metadata == 0) {
      throw EXCEPTION(InvalidArgumentException, S("object"), S("Object does not implement the MetaHaving interface."));
    }
    metadata->removeExtra(this->idInitStatementGenIndex.c_str());
  }

}; // class

} // namespace

#endif
