##
## Copyright 2009, The Android Open Source Project
##
## Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions
## are met:
##  * Redistributions of source code must retain the above copyright
##    notice, this list of conditions and the following disclaimer.
##  * Redistributions in binary form must reproduce the above copyright
##    notice, this list of conditions and the following disclaimer in the
##    documentation and/or other materials provided with the distribution.
##
## THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
## EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
## IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
## PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
## CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
## EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
## PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
## PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
## OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
## (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
## OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
##

## 
## Tephra Update
## From Webkit Commit 2012-12-01
## 

# not compile corefundation library for android.
# API/JSStringRefCF.cpp  

LOCAL_SRC_FILES := \
	API/JSBase.cpp \
	API/JSCallbackConstructor.cpp \
	API/JSCallbackFunction.cpp \
	API/JSCallbackObject.cpp \
	API/JSClassRef.cpp \
	API/JSContextRef.cpp \
	API/JSObjectRef.cpp \
	API/JSStringRef.cpp \
	API/JSValueRef.cpp \
	API/OpaqueJSString.cpp \
	assembler/ARMAssembler.cpp \
	assembler/ARMv7Assembler.cpp \
	assembler/LinkBuffer.cpp \
	assembler/MacroAssembler.cpp \
	assembler/MacroAssemblerARM.cpp \
	assembler/MacroAssemblerSH4.cpp \
	bytecode/ArrayAllocationProfile.cpp \
	bytecode/ArrayProfile.cpp \
	bytecode/CallLinkInfo.cpp \
	bytecode/CallLinkStatus.cpp \
	bytecode/CodeType.cpp \
	bytecode/CodeBlock.cpp \
	bytecode/CodeBlockHash.cpp \
	bytecode/CodeOrigin.cpp \
	bytecode/DFGExitProfile.cpp \
	bytecode/ExecutionCounter.cpp \
	bytecode/GetByIdStatus.cpp \
	bytecode/JumpTable.cpp \
	bytecode/LazyOperandValueProfile.cpp \
	bytecode/MethodOfGettingAValueProfile.cpp \
	bytecode/Opcode.cpp \
	bytecode/PolymorphicPutByIdList.cpp \
	bytecode/SpeculatedType.cpp \
	bytecode/PutByIdStatus.cpp \
	bytecode/ResolveGlobalStatus.cpp \
	bytecode/SamplingTool.cpp \
	bytecode/SpecialPointer.cpp \
	bytecode/StructureStubInfo.cpp \
	bytecode/StructureStubClearingWatchpoint.cpp \
	bytecode/UnlinkedCodeBlock.cpp \
	bytecode/Watchpoint.cpp \
	bytecompiler/BytecodeGenerator.cpp \
	bytecompiler/NodesCodegen.cpp \
	dfg/DFGAbstractState.cpp \
	dfg/DFGArgumentsSimplificationPhase.cpp \
	dfg/DFGArrayMode.cpp \
	dfg/DFGAssemblyHelpers.cpp \
	dfg/DFGByteCodeParser.cpp \
	dfg/DFGCFGSimplificationPhase.cpp \
	dfg/DFGCapabilities.cpp \
	dfg/DFGCFAPhase.cpp \
	dfg/DFGCorrectableJumpPoint.cpp \
	dfg/DFGConstantFoldingPhase.cpp \
	dfg/DFGCSEPhase.cpp \
	dfg/DFGDisassembler.cpp \
	dfg/DFGDominators.cpp \
	dfg/DFGDriver.cpp \
	dfg/DFGFixupPhase.cpp \
	dfg/DFGGraph.cpp \
	dfg/DFGJITCompiler.cpp \
	dfg/DFGMinifiedNode.cpp \
	dfg/DFGNodeFlags.cpp \
	dfg/DFGOperations.cpp \
	dfg/DFGOSREntry.cpp \
	dfg/DFGOSRExitCompiler32_64.cpp \
	dfg/DFGOSRExitCompiler64.cpp \
	dfg/DFGOSRExitCompiler.cpp \
	dfg/DFGOSRExit.cpp \
	dfg/DFGPhase.cpp \
	dfg/DFGPredictionPropagationPhase.cpp \
	dfg/DFGRepatch.cpp \
	dfg/DFGSpeculativeJIT32_64.cpp \
	dfg/DFGSpeculativeJIT64.cpp \
	dfg/DFGSpeculativeJIT.cpp \
	dfg/DFGStructureCheckHoistingPhase.cpp \
	dfg/DFGThunks.cpp \
	dfg/DFGValueSource.cpp \
	dfg/DFGVariableEvent.cpp \
	dfg/DFGVariableEventStream.cpp \
	dfg/DFGValidate.cpp \
	dfg/DFGVariableAccessDataDump.cpp \
	dfg/DFGVirtualRegisterAllocationPhase.cpp \
	disassembler/Disassembler.cpp \
	heap/CopiedSpace.cpp \
    heap/CopyVisitor.cpp \
	heap/ConservativeRoots.cpp \
	heap/DFGCodeBlocks.cpp \
	heap/HandleSet.cpp \
    heap/HeapTimer.cpp \
	heap/IncrementalSweeper.cpp \
    heap/SlotVisitor.cpp \
	heap/HandleStack.cpp \
	heap/BlockAllocator.cpp \
    heap/GCThreadSharedData.cpp \
    heap/GCThread.cpp \
	heap/Heap.cpp \
    heap/HeapStatistics.cpp \
	heap/JITStubRoutineSet.cpp \
	heap/MachineStackMarker.cpp \
	heap/MarkStack.cpp \
	heap/MarkedAllocator.cpp \
	heap/MarkedBlock.cpp \
	heap/MarkedSpace.cpp \
	heap/VTableSpectrum.cpp \
	heap/WeakBlock.cpp \
	heap/WeakHandleOwner.cpp \
	heap/WeakSet.cpp \
	heap/WriteBarrierSupport.cpp \
	debugger/DebuggerActivation.cpp \
	debugger/DebuggerCallFrame.cpp \
	debugger/Debugger.cpp \
	interpreter/AbstractPC.cpp \
	interpreter/CallFrame.cpp \
	interpreter/Interpreter.cpp \
	interpreter/JSStack.cpp \
	interpreter/VMInspector.cpp \
	jit/ClosureCallStubRoutine.cpp \
	jit/ExecutableAllocator.cpp \
	jit/GCAwareJITStubRoutine.cpp \
	jit/HostCallReturnValue.cpp \
	jit/JITArithmetic32_64.cpp \
	jit/JITArithmetic.cpp \
	jit/JITCall32_64.cpp \
	jit/JITCall.cpp \
	jit/JITCode.cpp \
	jit/JITDisassembler.cpp \
	jit/JIT.cpp \
	jit/JITExceptions.cpp \
	jit/JITOpcodes32_64.cpp \
	jit/JITOpcodes.cpp \
	jit/JITPropertyAccess32_64.cpp \
	jit/JITPropertyAccess.cpp \
	jit/JITStubRoutine.cpp \
	jit/JITStubs.cpp \
	jit/JumpReplacementWatchpoint.cpp \
	jit/ThunkGenerators.cpp \
	llint/LLIntCLoop.cpp \
	llint/LLIntData.cpp \
	llint/LLIntEntrypoints.cpp \
	llint/LLIntExceptions.cpp \
	llint/LLIntSlowPaths.cpp \
	llint/LLIntThunks.cpp \
	llint/LowLevelInterpreter.cpp \
	parser/Lexer.cpp \
	parser/Nodes.cpp \
	parser/ParserArena.cpp \
	parser/Parser.cpp \
	parser/SourceProviderCache.cpp \
	profiler/Profile.cpp \
	profiler/ProfileGenerator.cpp \
	profiler/ProfileNode.cpp \
	profiler/Profiler.cpp \
	runtime/ArgList.cpp \
	runtime/Arguments.cpp \
	runtime/ArrayConstructor.cpp \
	runtime/ArrayPrototype.cpp \
	runtime/BooleanConstructor.cpp \
	runtime/BooleanObject.cpp \
	runtime/BooleanPrototype.cpp \
	runtime/CallData.cpp \
	runtime/CodeCache.cpp \
	runtime/CodeSpecializationKind.cpp \
	runtime/CommonIdentifiers.cpp \
	runtime/Completion.cpp \
	runtime/ConstructData.cpp \
	runtime/DateConstructor.cpp \
	runtime/DateConversion.cpp \
	runtime/DateInstance.cpp \
	runtime/DatePrototype.cpp \
	runtime/ErrorConstructor.cpp \
	runtime/Error.cpp \
	runtime/ErrorInstance.cpp \
	runtime/ErrorPrototype.cpp \
	runtime/ExceptionHelpers.cpp \
	runtime/Executable.cpp \
	runtime/FunctionConstructor.cpp \
	runtime/FunctionPrototype.cpp \
	runtime/GCActivityCallback.cpp \
	runtime/GetterSetter.cpp \
	runtime/Identifier.cpp \
	runtime/IndexingType.cpp \
	runtime/InitializeThreading.cpp \
	runtime/InternalFunction.cpp \
	runtime/JSActivation.cpp \
	runtime/JSAPIValueWrapper.cpp \
	runtime/JSArray.cpp \
	runtime/JSCell.cpp \
	runtime/JSDateMath.cpp \
	runtime/JSFunction.cpp \
	runtime/JSBoundFunction.cpp \
	runtime/JSGlobalData.cpp \
	runtime/JSGlobalObject.cpp \
	runtime/JSGlobalObjectFunctions.cpp \
	runtime/JSProxy.cpp \
	runtime/JSLock.cpp \
	runtime/JSNotAnObject.cpp \
	runtime/JSObject.cpp \
	runtime/JSONObject.cpp \
	runtime/JSPropertyNameIterator.cpp \
	runtime/JSSegmentedVariableObject.cpp \
	runtime/JSWithScope.cpp \
	runtime/JSNameScope.cpp \
	runtime/JSScope.cpp \
	runtime/JSStringJoiner.cpp \
	runtime/JSString.cpp \
	runtime/JSSymbolTableObject.cpp \
	runtime/JSValue.cpp \
	runtime/JSVariableObject.cpp \
	runtime/JSWrapperObject.cpp \
	runtime/LiteralParser.cpp \
	runtime/Lookup.cpp \
	runtime/MathObject.cpp \
	runtime/NameConstructor.cpp \
	runtime/NameInstance.cpp \
	runtime/NamePrototype.cpp \
	runtime/NativeErrorConstructor.cpp \
	runtime/NativeErrorPrototype.cpp \
	runtime/NumberConstructor.cpp \
	runtime/NumberObject.cpp \
	runtime/NumberPrototype.cpp \
	runtime/ObjectConstructor.cpp \
	runtime/ObjectPrototype.cpp \
	runtime/Operations.cpp \
	runtime/Options.cpp \
	runtime/PropertyDescriptor.cpp \
	runtime/PropertyNameArray.cpp \
	runtime/PropertySlot.cpp \
	runtime/RegExpCache.cpp \
	runtime/RegExpConstructor.cpp \
	runtime/RegExp.cpp \
	runtime/RegExpMatchesArray.cpp \
	runtime/RegExpCachedResult.cpp \
	runtime/RegExpObject.cpp \
	runtime/RegExpPrototype.cpp \
	runtime/SamplingCounter.cpp \
	runtime/SmallStrings.cpp \
	runtime/SparseArrayValueMap.cpp \
	runtime/StrictEvalActivation.cpp \
	runtime/StringConstructor.cpp \
	runtime/StringObject.cpp \
	runtime/StringPrototype.cpp \
	runtime/StringRecursionChecker.cpp \
	runtime/StructureChain.cpp \
	runtime/Structure.cpp \
	runtime/SymbolTable.cpp \
	runtime/TimeoutChecker.cpp \
	tools/CodeProfile.cpp \
	tools/CodeProfiling.cpp \
	yarr/YarrCanonicalizeUCS2.cpp \
	yarr/YarrInterpreter.cpp \
	yarr/YarrJIT.cpp \
	yarr/YarrPattern.cpp \
	yarr/YarrSyntaxChecker.cpp \
	\
	wtf/Assertions.cpp \
	wtf/ByteArray.cpp \
	wtf/CryptographicallyRandomNumber.cpp \
	wtf/CurrentTime.cpp \
	wtf/DateMath.cpp \
	wtf/DecimalNumber.cpp \
	wtf/FastMalloc.cpp \
	wtf/HashTable.cpp \
	wtf/MD5.cpp \
	wtf/MainThread.cpp \
	wtf/OSAllocatorPosix.cpp \
	wtf/OSRandomSource.cpp \
	wtf/PageAllocationAligned.cpp\
	wtf/PageBlock.cpp\
	wtf/RandomNumber.cpp \
	wtf/RefCountedLeakCounter.cpp \
	wtf/SHA1.cpp \
	wtf/StackBounds.cpp \
	wtf/TCSystemAlloc.cpp \
	wtf/Threading.cpp \
	wtf/ThreadingNone.cpp \
	wtf/TypeTraits.cpp \
	wtf/WTFThreadData.cpp \
	wtf/dtoa.cpp \
	\
	wtf/android/MainThreadAndroid.cpp \
	\
	wtf/text/AtomicString.cpp \
	wtf/text/CString.cpp \
	wtf/text/StringBuilder.cpp \
	wtf/text/StringImpl.cpp \
	wtf/text/StringStatics.cpp \
	wtf/text/WTFString.cpp \
	\
	wtf/unicode/CollatorDefault.cpp \
	wtf/unicode/UTF8.cpp \
	\
	wtf/unicode/icu/CollatorICU.cpp \
	\
	wtf/url/src/URLCharacterTypes.cpp \
	wtf/url/src/URLEscape.cpp \
	wtf/url/src/URLSegments.cpp \
	\
	wtf/PrintStream.cpp \
	wtf/MetaAllocator.cpp \
	wtf/DataLog.cpp \
	wtf/FilePrintStream.cpp \
	wtf/GregorianDateTime.cpp \
	\


LOCAL_LDLIBS := -llog -lm