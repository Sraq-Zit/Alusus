/**
 * @file Spp/LlvmCodeGen/VariableGenerator.h
 * Contains the header of class Spp::LlvmCodeGen::VariableGenerator.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_LLVMCODEGEN_VARIABLEGENERATOR_H
#define SPP_LLVMCODEGEN_VARIABLEGENERATOR_H

namespace Spp { namespace LlvmCodeGen
{

class VariableGenerator : public TiObject, public virtual DynamicBindings, public virtual DynamicInterfaces
{
  //============================================================================
  // Type Info

  TYPE_INFO(VariableGenerator, TiObject, "Spp.LlvmCodeGen", "Spp", "alusus.net", (
    INHERITANCE_INTERFACES(DynamicBindings, DynamicInterfaces),
    OBJECT_INTERFACE_LIST(interfaceList)
  ));


  //============================================================================
  // Implementations

  IMPLEMENT_DYNAMIC_BINDINGS(bindingMap);
  IMPLEMENT_DYNAMIC_INTERFACES(interfaceList);


  //============================================================================
  // Member Variables

  private: Generator *generator;


  //============================================================================
  // Constructors & Destructor

  public: VariableGenerator(Generator *g = 0) : generator(g)
  {
    this->initBindingCaches();
    this->initBindings();
  }

  public: VariableGenerator(VariableGenerator *parent)
  {
    this->initBindingCaches();
    this->inheritBindings(parent);
    this->inheritInterfaces(parent);
    this->generator = parent->getGenerator();
  }

  public: virtual ~VariableGenerator()
  {
  }


  //============================================================================
  // Member Functions

  /// @name Initialization Functions
  /// @{

  private: void initBindingCaches();
  private: void initBindings();

  public: void setGenerator(Generator *g)
  {
    this->generator = g;
  }

  public: Generator* getGenerator() const
  {
    return this->generator;
  }

  /// @}

  /// @name Code Generation Functions
  /// @{

  public: METHOD_BINDING_CACHE(generateDefinition, Bool, (Core::Data::Ast::Definition*));
  private: static Bool _generateDefinition(TiObject *self, Core::Data::Ast::Definition *definition);

  public: METHOD_BINDING_CACHE(generateReference,
    Bool, (
      TiObject*, llvm::IRBuilder<>*, llvm::Function*, Spp::Ast::Type*&, llvm::Value*&
    )
  );
  private: static Bool _generateReference(
    TiObject *self, TiObject *astNode, llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc,
    Spp::Ast::Type *&resultType, llvm::Value *&llvmResult
  );

  public: METHOD_BINDING_CACHE(generateMemberReference,
    Bool, (
      Core::Data::Ast::Identifier*, llvm::Value*, Ast::Type*, llvm::IRBuilder<>*, llvm::Function*,
      Ast::Type*&, llvm::Value*&
    )
  );
  private: static Bool _generateMemberReference(
    TiObject *self, Core::Data::Ast::Identifier *astNode, llvm::Value *llvmPtr, Ast::Type *astType,
    llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc,
    Ast::Type *&resultType, llvm::Value *&llvmResult
  );

  public: METHOD_BINDING_CACHE(generateArrayElementReference,
    Bool, (
      llvm::Value*, llvm::Value*, Ast::Type*, llvm::IRBuilder<>*, llvm::Function*, Ast::Type*&, llvm::Value*&
    )
  );
  private: static Bool _generateArrayElementReference(
    TiObject *self, llvm::Value *llvmIndex, llvm::Value *llvmPtr, Ast::Type *astType,
    llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc,
    Ast::Type *&resultType, llvm::Value *&llvmResult
  );

  /// @}

}; // class

} } // namespace

#endif
