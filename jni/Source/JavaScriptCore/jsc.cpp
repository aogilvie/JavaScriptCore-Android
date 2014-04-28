/*
 *  Copyright (C) 1999-2000 Harri Porten (porten@kde.org)
 *  Copyright (C) 2004, 2005, 2006, 2007, 2008, 2012, 2013 Apple Inc. All rights reserved.
 *  Copyright (C) 2006 Bjoern Graf (bjoern.graf@gmail.com)
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */

#include "config.h"

#include "ButterflyInlines.h"
#include "BytecodeGenerator.h"
#include "Completion.h"
#include "CopiedSpaceInlines.h"
#include "ExceptionHelpers.h"
#include "HeapStatistics.h"
#include "InitializeThreading.h"
#include "Interpreter.h"
#include "JSArray.h"
#include "JSArrayBuffer.h"
#include "JSCInlines.h"
#include "JSFunction.h"
#include "JSLock.h"
#include "JSProxy.h"
#include "JSString.h"
#include "SamplingTool.h"
#include "StackVisitor.h"
#include "StructureRareDataInlines.h"
#include "TestRunnerUtils.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <wtf/CurrentTime.h>
#include <wtf/MainThread.h>
#include <wtf/StringPrintStream.h>
#include <wtf/text/StringBuilder.h>

#if !OS(WINDOWS)
#include <unistd.h>
#endif

#if HAVE(READLINE)
// readline/history.h has a Function typedef which conflicts with the WTF::Function template from WTF/Forward.h
// We #define it to something else to avoid this conflict.
#define Function ReadlineFunction
#include <readline/history.h>
#include <readline/readline.h>
#undef Function
#endif

#if HAVE(SYS_TIME_H)
#include <sys/time.h>
#endif

#if HAVE(SIGNAL_H)
#include <signal.h>
#endif

#if COMPILER(MSVC) && !OS(WINCE)
#include <crtdbg.h>
#include <mmsystem.h>
#include <windows.h>
#endif

#if PLATFORM(IOS) && CPU(ARM_THUMB2)
#include <fenv.h>
#include <arm/arch.h>
#endif

#if PLATFORM(EFL)
#include <Ecore.h>
#endif

using namespace JSC;
using namespace WTF;

namespace {

class Element;
class ElementHandleOwner;
class Masuqerader;
class Root;

class Element : public JSNonFinalObject {
public:
    Element(VM& vm, Structure* structure, Root* root)
        : Base(vm, structure)
        , m_root(root)
    {
    }

    typedef JSNonFinalObject Base;
    static const bool needsDestruction = false;

    Root* root() const { return m_root; }
    void setRoot(Root* root) { m_root = root; }

    static Element* create(VM& vm, JSGlobalObject* globalObject, Root* root)
    {
        Structure* structure = createStructure(vm, globalObject, jsNull());
        Element* element = new (NotNull, allocateCell<Element>(vm.heap, sizeof(Element))) Element(vm, structure, root);
        element->finishCreation(vm);
        return element;
    }

    void finishCreation(VM&);

    static ElementHandleOwner* handleOwner();

    static Structure* createStructure(VM& vm, JSGlobalObject* globalObject, JSValue prototype)
    {
        return Structure::create(vm, globalObject, prototype, TypeInfo(ObjectType, StructureFlags), info());
    }

    DECLARE_INFO;

private:
    Root* m_root;
};

class ElementHandleOwner : public WeakHandleOwner {
public:
    virtual bool isReachableFromOpaqueRoots(Handle<JSC::Unknown> handle, void*, SlotVisitor& visitor)
    {
        Element* element = jsCast<Element*>(handle.slot()->asCell());
        return visitor.containsOpaqueRoot(element->root());
    }
};

class Masquerader : public JSNonFinalObject {
public:
    Masquerader(VM& vm, Structure* structure)
        : Base(vm, structure)
    {
    }

    typedef JSNonFinalObject Base;

    static Masquerader* create(VM& vm, JSGlobalObject* globalObject)
    {
        globalObject->masqueradesAsUndefinedWatchpoint()->fireAll();
        Structure* structure = createStructure(vm, globalObject, jsNull());
        Masquerader* result = new (NotNull, allocateCell<Masquerader>(vm.heap, sizeof(Masquerader))) Masquerader(vm, structure);
        result->finishCreation(vm);
        return result;
    }

    static Structure* createStructure(VM& vm, JSGlobalObject* globalObject, JSValue prototype)
    {
        return Structure::create(vm, globalObject, prototype, TypeInfo(ObjectType, StructureFlags), info());
    }

    DECLARE_INFO;

protected:
    static const unsigned StructureFlags = JSC::MasqueradesAsUndefined | Base::StructureFlags;
};

class Root : public JSDestructibleObject {
public:
    Root(VM& vm, Structure* structure)
        : Base(vm, structure)
    {
    }

    Element* element()
    {
        return m_element.get();
    }

    void setElement(Element* element)
    {
        Weak<Element> newElement(element, Element::handleOwner());
        m_element.swap(newElement);
    }

    static Root* create(VM& vm, JSGlobalObject* globalObject)
    {
        Structure* structure = createStructure(vm, globalObject, jsNull());
        Root* root = new (NotNull, allocateCell<Root>(vm.heap, sizeof(Root))) Root(vm, structure);
        root->finishCreation(vm);
        return root;
    }

    typedef JSDestructibleObject Base;

    DECLARE_INFO;
    static const bool needsDestruction = true;

    static Structure* createStructure(VM& vm, JSGlobalObject* globalObject, JSValue prototype)
    {
        return Structure::create(vm, globalObject, prototype, TypeInfo(ObjectType, StructureFlags), info());
    }

    static void visitChildren(JSCell* thisObject, SlotVisitor& visitor)
    {
        Base::visitChildren(thisObject, visitor);
        visitor.addOpaqueRoot(thisObject);
    }

private:
    Weak<Element> m_element;
};

const ClassInfo Element::s_info = { "Element", &Base::s_info, 0, 0, CREATE_METHOD_TABLE(Element) };
const ClassInfo Masquerader::s_info = { "Masquerader", &Base::s_info, 0, 0, CREATE_METHOD_TABLE(Masquerader) };
const ClassInfo Root::s_info = { "Root", &Base::s_info, 0, 0, CREATE_METHOD_TABLE(Root) };

ElementHandleOwner* Element::handleOwner()
{
    static ElementHandleOwner* owner = 0;
    if (!owner)
        owner = new ElementHandleOwner();
    return owner;
}

void Element::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    m_root->setElement(this);
}

}

static bool fillBufferWithContentsOfFile(const String& fileName, Vector<char>& buffer);

static EncodedJSValue JSC_HOST_CALL functionSetElementRoot(ExecState*);
static EncodedJSValue JSC_HOST_CALL functionCreateRoot(ExecState*);
static EncodedJSValue JSC_HOST_CALL functionCreateElement(ExecState*);
static EncodedJSValue JSC_HOST_CALL functionGetElement(ExecState*);
static EncodedJSValue JSC_HOST_CALL functionPrint(ExecState*);
static EncodedJSValue JSC_HOST_CALL functionDebug(ExecState*);
static EncodedJSValue JSC_HOST_CALL functionDescribe(ExecState*);
static EncodedJSValue JSC_HOST_CALL functionDescribeArray(ExecState*);
static EncodedJSValue JSC_HOST_CALL functionJSCStack(ExecState*);
static EncodedJSValue JSC_HOST_CALL functionGCAndSweep(ExecState*);
static EncodedJSValue JSC_HOST_CALL functionFullGC(ExecState*);
static EncodedJSValue JSC_HOST_CALL functionEdenGC(ExecState*);
#ifndef NDEBUG
static EncodedJSValue JSC_HOST_CALL functionReleaseExecutableMemory(ExecState*);
static EncodedJSValue JSC_HOST_CALL functionDumpCallFrame(ExecState*);
#endif
static EncodedJSValue JSC_HOST_CALL functionVersion(ExecState*);
static EncodedJSValue JSC_HOST_CALL functionRun(ExecState*);
static EncodedJSValue JSC_HOST_CALL functionLoad(ExecState*);
static EncodedJSValue JSC_HOST_CALL functionReadFile(ExecState*);
static EncodedJSValue JSC_HOST_CALL functionCheckSyntax(ExecState*);
static EncodedJSValue JSC_HOST_CALL functionReadline(ExecState*);
static EncodedJSValue JSC_HOST_CALL functionPreciseTime(ExecState*);
static EncodedJSValue JSC_HOST_CALL functionNeverInlineFunction(ExecState*);
static EncodedJSValue JSC_HOST_CALL functionNumberOfDFGCompiles(ExecState*);
static EncodedJSValue JSC_HOST_CALL functionReoptimizationRetryCount(ExecState*);
static EncodedJSValue JSC_HOST_CALL functionTransferArrayBuffer(ExecState*);
static NO_RETURN_WITH_VALUE EncodedJSValue JSC_HOST_CALL functionQuit(ExecState*);
static EncodedJSValue JSC_HOST_CALL functionFalse(ExecState*);
static EncodedJSValue JSC_HOST_CALL functionEffectful42(ExecState*);
static EncodedJSValue JSC_HOST_CALL functionMakeMasquerader(ExecState*);

#if ENABLE(SAMPLING_FLAGS)
static EncodedJSValue JSC_HOST_CALL functionSetSamplingFlags(ExecState*);
static EncodedJSValue JSC_HOST_CALL functionClearSamplingFlags(ExecState*);
#endif

struct Script {
    bool isFile;
    char* argument;

    Script(bool isFile, char *argument)
        : isFile(isFile)
        , argument(argument)
    {
    }
};

class CommandLine {
public:
    CommandLine(int argc, char** argv)
        : m_interactive(false)
        , m_dump(false)
        , m_exitCode(false)
        , m_profile(false)
    {
        parseArguments(argc, argv);
    }

    bool m_interactive;
    bool m_dump;
    bool m_exitCode;
    Vector<Script> m_scripts;
    Vector<String> m_arguments;
    bool m_profile;
    String m_profilerOutput;

    void parseArguments(int, char**);
};

static const char interactivePrompt[] = ">>> ";

class StopWatch {
public:
    void start();
    void stop();
    long getElapsedMS(); // call stop() first

private:
    double m_startTime;
    double m_stopTime;
};

void StopWatch::start()
{
    m_startTime = monotonicallyIncreasingTime();
}

void StopWatch::stop()
{
    m_stopTime = monotonicallyIncreasingTime();
}

long StopWatch::getElapsedMS()
{
    return static_cast<long>((m_stopTime - m_startTime) * 1000);
}

class GlobalObject : public JSGlobalObject {
private:
    GlobalObject(VM&, Structure*);

public:
    typedef JSGlobalObject Base;

    static GlobalObject* create(VM& vm, Structure* structure, const Vector<String>& arguments)
    {
        GlobalObject* object = new (NotNull, allocateCell<GlobalObject>(vm.heap)) GlobalObject(vm, structure);
        object->finishCreation(vm, arguments);
        vm.heap.addFinalizer(object, destroy);
        object->setGlobalThis(vm, JSProxy::create(vm, JSProxy::createStructure(vm, object, object->prototype()), object));
        return object;
    }

    static const bool needsDestruction = false;

    DECLARE_INFO;
    static const GlobalObjectMethodTable s_globalObjectMethodTable;

    static Structure* createStructure(VM& vm, JSValue prototype)
    {
        return Structure::create(vm, 0, prototype, TypeInfo(GlobalObjectType, StructureFlags), info());
    }

    static bool javaScriptExperimentsEnabled(const JSGlobalObject*) { return true; }

protected:
    void finishCreation(VM& vm, const Vector<String>& arguments)
    {
        Base::finishCreation(vm);
        
        addFunction(vm, "debug", functionDebug, 1);
        addFunction(vm, "describe", functionDescribe, 1);
        addFunction(vm, "describeArray", functionDescribeArray, 1);
        addFunction(vm, "print", functionPrint, 1);
        addFunction(vm, "quit", functionQuit, 0);
        addFunction(vm, "gc", functionGCAndSweep, 0);
        addFunction(vm, "fullGC", functionFullGC, 0);
        addFunction(vm, "edenGC", functionEdenGC, 0);
#ifndef NDEBUG
        addFunction(vm, "dumpCallFrame", functionDumpCallFrame, 0);
        addFunction(vm, "releaseExecutableMemory", functionReleaseExecutableMemory, 0);
#endif
        addFunction(vm, "version", functionVersion, 1);
        addFunction(vm, "run", functionRun, 1);
        addFunction(vm, "load", functionLoad, 1);
        addFunction(vm, "readFile", functionReadFile, 1);
        addFunction(vm, "checkSyntax", functionCheckSyntax, 1);
        addFunction(vm, "jscStack", functionJSCStack, 1);
        addFunction(vm, "readline", functionReadline, 0);
        addFunction(vm, "preciseTime", functionPreciseTime, 0);
        addFunction(vm, "neverInlineFunction", functionNeverInlineFunction, 1);
        addFunction(vm, "noInline", functionNeverInlineFunction, 1);
        addFunction(vm, "numberOfDFGCompiles", functionNumberOfDFGCompiles, 1);
        addFunction(vm, "reoptimizationRetryCount", functionReoptimizationRetryCount, 1);
        addFunction(vm, "transferArrayBuffer", functionTransferArrayBuffer, 1);
#if ENABLE(SAMPLING_FLAGS)
        addFunction(vm, "setSamplingFlags", functionSetSamplingFlags, 1);
        addFunction(vm, "clearSamplingFlags", functionClearSamplingFlags, 1);
#endif
        addConstructableFunction(vm, "Root", functionCreateRoot, 0);
        addConstructableFunction(vm, "Element", functionCreateElement, 1);
        addFunction(vm, "getElement", functionGetElement, 1);
        addFunction(vm, "setElementRoot", functionSetElementRoot, 2);
        
        putDirectNativeFunction(vm, this, Identifier(&vm, "DFGTrue"), 0, functionFalse, DFGTrue, DontEnum | JSC::Function);
        
        addFunction(vm, "effectful42", functionEffectful42, 0);
        addFunction(vm, "makeMasquerader", functionMakeMasquerader, 0);
        
        JSArray* array = constructEmptyArray(globalExec(), 0);
        for (size_t i = 0; i < arguments.size(); ++i)
            array->putDirectIndex(globalExec(), i, jsString(globalExec(), arguments[i]));
        putDirect(vm, Identifier(globalExec(), "arguments"), array);
    }

    void addFunction(VM& vm, const char* name, NativeFunction function, unsigned arguments)
    {
        Identifier identifier(&vm, name);
        putDirect(vm, identifier, JSFunction::create(vm, this, arguments, identifier.string(), function));
    }
    
    void addConstructableFunction(VM& vm, const char* name, NativeFunction function, unsigned arguments)
    {
        Identifier identifier(&vm, name);
        putDirect(vm, identifier, JSFunction::create(vm, this, arguments, identifier.string(), function, NoIntrinsic, function));
    }
};

const ClassInfo GlobalObject::s_info = { "global", &JSGlobalObject::s_info, 0, ExecState::globalObjectTable, CREATE_METHOD_TABLE(GlobalObject) };
const GlobalObjectMethodTable GlobalObject::s_globalObjectMethodTable = { &allowsAccessFrom, &supportsProfiling, &supportsRichSourceInfo, &shouldInterruptScript, &javaScriptExperimentsEnabled, 0, &shouldInterruptScriptBeforeTimeout };


GlobalObject::GlobalObject(VM& vm, Structure* structure)
    : JSGlobalObject(vm, structure, &s_globalObjectMethodTable)
{
}

static inline String stringFromUTF(const char* utf8)
{
    // Find the the first non-ascii character, or nul.
    const char* pos = utf8;
    while (*pos > 0)
        pos++;
    size_t asciiLength = pos - utf8;
    
    // Fast case - string is all ascii.
    if (!*pos)
        return String(utf8, asciiLength);
    
    // Slow case - contains non-ascii characters, use fromUTF8WithLatin1Fallback.
    ASSERT(*pos < 0);
    ASSERT(strlen(utf8) == asciiLength + strlen(pos));
    return String::fromUTF8WithLatin1Fallback(utf8, asciiLength + strlen(pos));
}

static inline SourceCode jscSource(const char* utf8, const String& filename)
{
    String str = stringFromUTF(utf8);
    return makeSource(str, filename);
}

EncodedJSValue JSC_HOST_CALL functionPrint(ExecState* exec)
{
    for (unsigned i = 0; i < exec->argumentCount(); ++i) {
        if (i)
            putchar(' ');

        printf("%s", exec->uncheckedArgument(i).toString(exec)->value(exec).utf8().data());
    }

    putchar('\n');
    fflush(stdout);
    return JSValue::encode(jsUndefined());
}

#ifndef NDEBUG
EncodedJSValue JSC_HOST_CALL functionDumpCallFrame(ExecState* exec)
{
    if (!exec->callerFrame()->isVMEntrySentinel())
        exec->vm().interpreter->dumpCallFrame(exec->callerFrame());
    return JSValue::encode(jsUndefined());
}
#endif

EncodedJSValue JSC_HOST_CALL functionDebug(ExecState* exec)
{
    fprintf(stderr, "--> %s\n", exec->argument(0).toString(exec)->value(exec).utf8().data());
    return JSValue::encode(jsUndefined());
}

EncodedJSValue JSC_HOST_CALL functionDescribe(ExecState* exec)
{
    if (exec->argumentCount() < 1)
        return JSValue::encode(jsUndefined());
    return JSValue::encode(jsString(exec, toString(exec->argument(0))));
}

EncodedJSValue JSC_HOST_CALL functionDescribeArray(ExecState* exec)
{
    if (exec->argumentCount() < 1)
        return JSValue::encode(jsUndefined());
    JSObject* object = jsDynamicCast<JSObject*>(exec->argument(0));
    if (!object)
        return JSValue::encode(jsString(exec, "<not object>"));
    return JSValue::encode(jsString(exec, toString("<Public length: ", object->getArrayLength(), "; vector length: ", object->getVectorLength(), ">")));
}

class FunctionJSCStackFunctor {
public:
    FunctionJSCStackFunctor(StringBuilder& trace)
        : m_trace(trace)
    {
    }

    StackVisitor::Status operator()(StackVisitor& visitor)
    {
        m_trace.append(String::format("    %zu   %s\n", visitor->index(), visitor->toString().utf8().data()));
        return StackVisitor::Continue;
    }

private:
    StringBuilder& m_trace;
};

EncodedJSValue JSC_HOST_CALL functionJSCStack(ExecState* exec)
{
    StringBuilder trace;
    trace.appendLiteral("--> Stack trace:\n");

    FunctionJSCStackFunctor functor(trace);
    exec->iterate(functor);
    fprintf(stderr, "%s", trace.toString().utf8().data());
    return JSValue::encode(jsUndefined());
}

EncodedJSValue JSC_HOST_CALL functionCreateRoot(ExecState* exec)
{
    JSLockHolder lock(exec);
    return JSValue::encode(Root::create(exec->vm(), exec->lexicalGlobalObject()));
}

EncodedJSValue JSC_HOST_CALL functionCreateElement(ExecState* exec)
{
    JSLockHolder lock(exec);
    JSValue arg = exec->argument(0);
    return JSValue::encode(Element::create(exec->vm(), exec->lexicalGlobalObject(), arg.isNull() ? nullptr : jsCast<Root*>(exec->argument(0))));
}

EncodedJSValue JSC_HOST_CALL functionGetElement(ExecState* exec)
{
    JSLockHolder lock(exec);
    Element* result = jsCast<Root*>(exec->argument(0).asCell())->element();
    return JSValue::encode(result ? result : jsUndefined());
}

EncodedJSValue JSC_HOST_CALL functionSetElementRoot(ExecState* exec)
{
    JSLockHolder lock(exec);
    Element* element = jsCast<Element*>(exec->argument(0));
    Root* root = jsCast<Root*>(exec->argument(1));
    element->setRoot(root);
    return JSValue::encode(jsUndefined());
}

EncodedJSValue JSC_HOST_CALL functionGCAndSweep(ExecState* exec)
{
    JSLockHolder lock(exec);
    exec->heap()->collectAllGarbage();
    return JSValue::encode(jsUndefined());
}

EncodedJSValue JSC_HOST_CALL functionFullGC(ExecState* exec)
{
    JSLockHolder lock(exec);
    Heap* heap = exec->heap();
    heap->setShouldDoFullCollection(true);
    exec->heap()->collect();
    return JSValue::encode(jsUndefined());
}

EncodedJSValue JSC_HOST_CALL functionEdenGC(ExecState* exec)
{
    JSLockHolder lock(exec);
    Heap* heap = exec->heap();
    heap->setShouldDoFullCollection(false);
    heap->collect();
    return JSValue::encode(jsUndefined());
}

#ifndef NDEBUG
EncodedJSValue JSC_HOST_CALL functionReleaseExecutableMemory(ExecState* exec)
{
    JSLockHolder lock(exec);
    exec->vm().releaseExecutableMemory();
    return JSValue::encode(jsUndefined());
}
#endif

EncodedJSValue JSC_HOST_CALL functionVersion(ExecState*)
{
    // We need this function for compatibility with the Mozilla JS tests but for now
    // we don't actually do any version-specific handling
    return JSValue::encode(jsUndefined());
}

EncodedJSValue JSC_HOST_CALL functionRun(ExecState* exec)
{
    String fileName = exec->argument(0).toString(exec)->value(exec);
    Vector<char> script;
    if (!fillBufferWithContentsOfFile(fileName, script))
        return JSValue::encode(exec->vm().throwException(exec, createError(exec, "Could not open file.")));

    GlobalObject* globalObject = GlobalObject::create(exec->vm(), GlobalObject::createStructure(exec->vm(), jsNull()), Vector<String>());

    JSArray* array = constructEmptyArray(globalObject->globalExec(), 0);
    for (unsigned i = 1; i < exec->argumentCount(); ++i)
        array->putDirectIndex(globalObject->globalExec(), i - 1, exec->uncheckedArgument(i));
    globalObject->putDirect(
        exec->vm(), Identifier(globalObject->globalExec(), "arguments"), array);

    JSValue exception;
    StopWatch stopWatch;
    stopWatch.start();
    evaluate(globalObject->globalExec(), jscSource(script.data(), fileName), JSValue(), &exception);
    stopWatch.stop();

    if (!!exception) {
        exec->vm().throwException(globalObject->globalExec(), exception);
        return JSValue::encode(jsUndefined());
    }
    
    return JSValue::encode(jsNumber(stopWatch.getElapsedMS()));
}

EncodedJSValue JSC_HOST_CALL functionLoad(ExecState* exec)
{
    String fileName = exec->argument(0).toString(exec)->value(exec);
    Vector<char> script;
    if (!fillBufferWithContentsOfFile(fileName, script))
        return JSValue::encode(exec->vm().throwException(exec, createError(exec, "Could not open file.")));

    JSGlobalObject* globalObject = exec->lexicalGlobalObject();
    
    JSValue evaluationException;
    JSValue result = evaluate(globalObject->globalExec(), jscSource(script.data(), fileName), JSValue(), &evaluationException);
    if (evaluationException)
        exec->vm().throwException(exec, evaluationException);
    return JSValue::encode(result);
}

EncodedJSValue JSC_HOST_CALL functionReadFile(ExecState* exec)
{
    String fileName = exec->argument(0).toString(exec)->value(exec);
    Vector<char> script;
    if (!fillBufferWithContentsOfFile(fileName, script))
        return JSValue::encode(exec->vm().throwException(exec, createError(exec, "Could not open file.")));

    return JSValue::encode(jsString(exec, stringFromUTF(script.data())));
}

EncodedJSValue JSC_HOST_CALL functionCheckSyntax(ExecState* exec)
{
    String fileName = exec->argument(0).toString(exec)->value(exec);
    Vector<char> script;
    if (!fillBufferWithContentsOfFile(fileName, script))
        return JSValue::encode(exec->vm().throwException(exec, createError(exec, "Could not open file.")));

    JSGlobalObject* globalObject = exec->lexicalGlobalObject();

    StopWatch stopWatch;
    stopWatch.start();

    JSValue syntaxException;
    bool validSyntax = checkSyntax(globalObject->globalExec(), jscSource(script.data(), fileName), &syntaxException);
    stopWatch.stop();

    if (!validSyntax)
        exec->vm().throwException(exec, syntaxException);
    return JSValue::encode(jsNumber(stopWatch.getElapsedMS()));
}

#if ENABLE(SAMPLING_FLAGS)
EncodedJSValue JSC_HOST_CALL functionSetSamplingFlags(ExecState* exec)
{
    for (unsigned i = 0; i < exec->argumentCount(); ++i) {
        unsigned flag = static_cast<unsigned>(exec->uncheckedArgument(i).toNumber(exec));
        if ((flag >= 1) && (flag <= 32))
            SamplingFlags::setFlag(flag);
    }
    return JSValue::encode(jsNull());
}

EncodedJSValue JSC_HOST_CALL functionClearSamplingFlags(ExecState* exec)
{
    for (unsigned i = 0; i < exec->argumentCount(); ++i) {
        unsigned flag = static_cast<unsigned>(exec->uncheckedArgument(i).toNumber(exec));
        if ((flag >= 1) && (flag <= 32))
            SamplingFlags::clearFlag(flag);
    }
    return JSValue::encode(jsNull());
}
#endif

EncodedJSValue JSC_HOST_CALL functionReadline(ExecState* exec)
{
    Vector<char, 256> line;
    int c;
    while ((c = getchar()) != EOF) {
        // FIXME: Should we also break on \r? 
        if (c == '\n')
            break;
        line.append(c);
    }
    line.append('\0');
    return JSValue::encode(jsString(exec, line.data()));
}

EncodedJSValue JSC_HOST_CALL functionPreciseTime(ExecState*)
{
    return JSValue::encode(jsNumber(currentTime()));
}

EncodedJSValue JSC_HOST_CALL functionNeverInlineFunction(ExecState* exec)
{
    return JSValue::encode(setNeverInline(exec));
}

EncodedJSValue JSC_HOST_CALL functionNumberOfDFGCompiles(ExecState* exec)
{
    return JSValue::encode(numberOfDFGCompiles(exec));
}

EncodedJSValue JSC_HOST_CALL functionReoptimizationRetryCount(ExecState* exec)
{
    if (exec->argumentCount() < 1)
        return JSValue::encode(jsUndefined());
    
    CodeBlock* block = getSomeBaselineCodeBlockForFunction(exec->argument(0));
    if (!block)
        return JSValue::encode(jsNumber(0));
    
    return JSValue::encode(jsNumber(block->reoptimizationRetryCounter()));
}

EncodedJSValue JSC_HOST_CALL functionTransferArrayBuffer(ExecState* exec)
{
    if (exec->argumentCount() < 1)
        return JSValue::encode(exec->vm().throwException(exec, createError(exec, "Not enough arguments")));
    
    JSArrayBuffer* buffer = jsDynamicCast<JSArrayBuffer*>(exec->argument(0));
    if (!buffer)
        return JSValue::encode(exec->vm().throwException(exec, createError(exec, "Expected an array buffer")));
    
    ArrayBufferContents dummyContents;
    buffer->impl()->transfer(dummyContents);
    
    return JSValue::encode(jsUndefined());
}

EncodedJSValue JSC_HOST_CALL functionQuit(ExecState*)
{
    exit(EXIT_SUCCESS);

#if COMPILER(MSVC) && OS(WINCE)
    // Without this, Visual Studio will complain that this method does not return a value.
    return JSValue::encode(jsUndefined());
#endif
}

EncodedJSValue JSC_HOST_CALL functionFalse(ExecState*)
{
    return JSValue::encode(jsBoolean(false));
}

EncodedJSValue JSC_HOST_CALL functionEffectful42(ExecState*)
{
    return JSValue::encode(jsNumber(42));
}

EncodedJSValue JSC_HOST_CALL functionMakeMasquerader(ExecState* exec)
{
    return JSValue::encode(Masquerader::create(exec->vm(), exec->lexicalGlobalObject()));
}

// Use SEH for Release builds only to get rid of the crash report dialog
// (luckily the same tests fail in Release and Debug builds so far). Need to
// be in a separate main function because the jscmain function requires object
// unwinding.

#if COMPILER(MSVC) && !defined(_DEBUG) && !OS(WINCE)
#define TRY       __try {
#define EXCEPT(x) } __except (EXCEPTION_EXECUTE_HANDLER) { x; }
#else
#define TRY
#define EXCEPT(x)
#endif

int jscmain(int argc, char** argv);

static double s_desiredTimeout;

static NO_RETURN_DUE_TO_CRASH void timeoutThreadMain(void*)
{
    auto timeout = std::chrono::microseconds(static_cast<std::chrono::microseconds::rep>(s_desiredTimeout * 1000000));
    std::this_thread::sleep_for(timeout);
    
    dataLog("Timed out after ", s_desiredTimeout, " seconds!\n");
    CRASH();
}

int main(int argc, char** argv)
{
#if PLATFORM(IOS) && CPU(ARM_THUMB2)
    // Enabled IEEE754 denormal support.
    fenv_t env;
    fegetenv( &env );
    env.__fpscr &= ~0x01000000u;
    fesetenv( &env );
#endif

#if OS(WINDOWS)
#if !OS(WINCE)
    // Cygwin calls ::SetErrorMode(SEM_FAILCRITICALERRORS), which we will inherit. This is bad for
    // testing/debugging, as it causes the post-mortem debugger not to be invoked. We reset the
    // error mode here to work around Cygwin's behavior. See <http://webkit.org/b/55222>.
    ::SetErrorMode(0);

#if defined(_DEBUG)
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
#endif
#endif

    timeBeginPeriod(1);
#endif

#if PLATFORM(EFL)
    ecore_init();
#endif

    // Initialize JSC before getting VM.
#if ENABLE(SAMPLING_REGIONS)
    WTF::initializeMainThread();
#endif
    JSC::initializeThreading();

#if !OS(WINCE)
    if (char* timeoutString = getenv("JSC_timeout")) {
        if (sscanf(timeoutString, "%lf", &s_desiredTimeout) != 1) {
            dataLog(
                "WARNING: timeout string is malformed, got ", timeoutString,
                " but expected a number. Not using a timeout.\n");
        } else
            createThread(timeoutThreadMain, 0, "jsc Timeout Thread");
    }
#endif

#if PLATFORM(IOS)
    Options::crashIfCantAllocateJITMemory() = true;
#endif

    // We can't use destructors in the following code because it uses Windows
    // Structured Exception Handling
    int res = 0;
    TRY
        res = jscmain(argc, argv);
    EXCEPT(res = 3)
    if (Options::logHeapStatisticsAtExit())
        HeapStatistics::reportSuccess();

#if PLATFORM(EFL)
    ecore_shutdown();
#endif

    return res;
}

static bool runWithScripts(GlobalObject* globalObject, const Vector<Script>& scripts, bool dump)
{
    const char* script;
    String fileName;
    Vector<char> scriptBuffer;

    if (dump)
        JSC::Options::dumpGeneratedBytecodes() = true;

    VM& vm = globalObject->vm();

#if ENABLE(SAMPLING_FLAGS)
    SamplingFlags::start();
#endif

    bool success = true;
    for (size_t i = 0; i < scripts.size(); i++) {
        if (scripts[i].isFile) {
            fileName = scripts[i].argument;
            if (!fillBufferWithContentsOfFile(fileName, scriptBuffer))
                return false; // fail early so we can catch missing files
            script = scriptBuffer.data();
        } else {
            script = scripts[i].argument;
            fileName = "[Command Line]";
        }

        vm.startSampling();

        JSValue evaluationException;
        JSValue returnValue = evaluate(globalObject->globalExec(), jscSource(script, fileName), JSValue(), &evaluationException);
        success = success && !evaluationException;
        if (dump && !evaluationException)
            printf("End: %s\n", returnValue.toString(globalObject->globalExec())->value(globalObject->globalExec()).utf8().data());
        if (evaluationException) {
            printf("Exception: %s\n", evaluationException.toString(globalObject->globalExec())->value(globalObject->globalExec()).utf8().data());
            Identifier stackID(globalObject->globalExec(), "stack");
            JSValue stackValue = evaluationException.get(globalObject->globalExec(), stackID);
            if (!stackValue.isUndefinedOrNull())
                printf("%s\n", stackValue.toString(globalObject->globalExec())->value(globalObject->globalExec()).utf8().data());
        }

        vm.stopSampling();
        globalObject->globalExec()->clearException();
    }

#if ENABLE(SAMPLING_FLAGS)
    SamplingFlags::stop();
#endif
#if ENABLE(SAMPLING_REGIONS)
    SamplingRegion::dump();
#endif
    vm.dumpSampleData(globalObject->globalExec());
#if ENABLE(SAMPLING_COUNTERS)
    AbstractSamplingCounter::dump();
#endif
#if ENABLE(REGEXP_TRACING)
    vm.dumpRegExpTrace();
#endif
    return success;
}

#define RUNNING_FROM_XCODE 0

static void runInteractive(GlobalObject* globalObject)
{
    String interpreterName("Interpreter");
    
    bool shouldQuit = false;
    while (!shouldQuit) {
#if HAVE(READLINE) && !RUNNING_FROM_XCODE
        ParserError error;
        String source;
        do {
            error = ParserError();
            char* line = readline(source.isEmpty() ? interactivePrompt : "... ");
            shouldQuit = !line;
            if (!line)
                break;
            source = source + line;
            source = source + '\n';
            checkSyntax(globalObject->vm(), makeSource(source, interpreterName), error);
            if (!line[0])
                break;
            add_history(line);
        } while (error.m_syntaxErrorType == ParserError::SyntaxErrorRecoverable);
        
        if (error.m_type != ParserError::ErrorNone) {
            printf("%s:%d\n", error.m_message.utf8().data(), error.m_line);
            continue;
        }
        
        
        JSValue evaluationException;
        JSValue returnValue = evaluate(globalObject->globalExec(), makeSource(source, interpreterName), JSValue(), &evaluationException);
#else
        printf("%s", interactivePrompt);
        Vector<char, 256> line;
        int c;
        while ((c = getchar()) != EOF) {
            // FIXME: Should we also break on \r? 
            if (c == '\n')
                break;
            line.append(c);
        }
        if (line.isEmpty())
            break;
        line.append('\0');

        JSValue evaluationException;
        JSValue returnValue = evaluate(globalObject->globalExec(), jscSource(line.data(), interpreterName), JSValue(), &evaluationException);
#endif
        if (evaluationException)
            printf("Exception: %s\n", evaluationException.toString(globalObject->globalExec())->value(globalObject->globalExec()).utf8().data());
        else
            printf("%s\n", returnValue.toString(globalObject->globalExec())->value(globalObject->globalExec()).utf8().data());

        globalObject->globalExec()->clearException();
    }
    printf("\n");
}

static NO_RETURN void printUsageStatement(bool help = false)
{
    fprintf(stderr, "Usage: jsc [options] [files] [-- arguments]\n");
    fprintf(stderr, "  -d         Dumps bytecode (debug builds only)\n");
    fprintf(stderr, "  -e         Evaluate argument as script code\n");
    fprintf(stderr, "  -f         Specifies a source file (deprecated)\n");
    fprintf(stderr, "  -h|--help  Prints this help message\n");
    fprintf(stderr, "  -i         Enables interactive mode (default if no files are specified)\n");
#if HAVE(SIGNAL_H)
    fprintf(stderr, "  -s         Installs signal handlers that exit on a crash (Unix platforms only)\n");
#endif
    fprintf(stderr, "  -p <file>  Outputs profiling data to a file\n");
    fprintf(stderr, "  -x         Output exit code before terminating\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "  --options                  Dumps all JSC VM options and exits\n");
    fprintf(stderr, "  --dumpOptions              Dumps all JSC VM options before continuing\n");
    fprintf(stderr, "  --<jsc VM option>=<value>  Sets the specified JSC VM option\n");
    fprintf(stderr, "\n");

    exit(help ? EXIT_SUCCESS : EXIT_FAILURE);
}

void CommandLine::parseArguments(int argc, char** argv)
{
    int i = 1;
    bool needToDumpOptions = false;
    bool needToExit = false;

    for (; i < argc; ++i) {
        const char* arg = argv[i];
        if (!strcmp(arg, "-f")) {
            if (++i == argc)
                printUsageStatement();
            m_scripts.append(Script(true, argv[i]));
            continue;
        }
        if (!strcmp(arg, "-e")) {
            if (++i == argc)
                printUsageStatement();
            m_scripts.append(Script(false, argv[i]));
            continue;
        }
        if (!strcmp(arg, "-i")) {
            m_interactive = true;
            continue;
        }
        if (!strcmp(arg, "-d")) {
            m_dump = true;
            continue;
        }
        if (!strcmp(arg, "-p")) {
            if (++i == argc)
                printUsageStatement();
            m_profile = true;
            m_profilerOutput = argv[i];
            continue;
        }
        if (!strcmp(arg, "-s")) {
#if HAVE(SIGNAL_H)
            signal(SIGILL, _exit);
            signal(SIGFPE, _exit);
            signal(SIGBUS, _exit);
            signal(SIGSEGV, _exit);
#endif
            continue;
        }
        if (!strcmp(arg, "-x")) {
            m_exitCode = true;
            continue;
        }
        if (!strcmp(arg, "--")) {
            ++i;
            break;
        }
        if (!strcmp(arg, "-h") || !strcmp(arg, "--help"))
            printUsageStatement(true);

        if (!strcmp(arg, "--options")) {
            needToDumpOptions = true;
            needToExit = true;
            continue;
        }
        if (!strcmp(arg, "--dumpOptions")) {
            needToDumpOptions = true;
            continue;
        }

        // See if the -- option is a JSC VM option.
        // NOTE: At this point, we know that the arg starts with "--". Skip it.
        if (JSC::Options::setOption(&arg[2])) {
            // The arg was recognized as a VM option and has been parsed.
            continue; // Just continue with the next arg. 
        }

        // This arg is not recognized by the VM nor by jsc. Pass it on to the
        // script.
        m_scripts.append(Script(true, argv[i]));
    }

    if (m_scripts.isEmpty())
        m_interactive = true;

    for (; i < argc; ++i)
        m_arguments.append(argv[i]);

    if (needToDumpOptions)
        JSC::Options::dumpAllOptions(stderr);
    if (needToExit)
        exit(EXIT_SUCCESS);
}

int jscmain(int argc, char** argv)
{
    // Note that the options parsing can affect VM creation, and thus
    // comes first.
    CommandLine options(argc, argv);
    VM* vm = VM::create(LargeHeap).leakRef();
    int result;
    {
        JSLockHolder locker(vm);

        if (options.m_profile && !vm->m_perBytecodeProfiler)
            vm->m_perBytecodeProfiler = adoptPtr(new Profiler::Database(*vm));
    
        GlobalObject* globalObject = GlobalObject::create(*vm, GlobalObject::createStructure(*vm, jsNull()), options.m_arguments);
        bool success = runWithScripts(globalObject, options.m_scripts, options.m_dump);
        if (options.m_interactive && success)
            runInteractive(globalObject);

        result = success ? 0 : 3;

        if (options.m_exitCode)
            printf("jsc exiting %d\n", result);
    
        if (options.m_profile) {
            if (!vm->m_perBytecodeProfiler->save(options.m_profilerOutput.utf8().data()))
                fprintf(stderr, "could not save profiler output.\n");
        }
    }
    
    return result;
}

static bool fillBufferWithContentsOfFile(const String& fileName, Vector<char>& buffer)
{
    FILE* f = fopen(fileName.utf8().data(), "r");
    if (!f) {
        fprintf(stderr, "Could not open file: %s\n", fileName.utf8().data());
        return false;
    }

    size_t bufferSize = 0;
    size_t bufferCapacity = 1024;

    buffer.resize(bufferCapacity);

    while (!feof(f) && !ferror(f)) {
        bufferSize += fread(buffer.data() + bufferSize, 1, bufferCapacity - bufferSize, f);
        if (bufferSize == bufferCapacity) { // guarantees space for trailing '\0'
            bufferCapacity *= 2;
            buffer.resize(bufferCapacity);
        }
    }
    fclose(f);
    buffer[bufferSize] = '\0';

    if (buffer[0] == '#' && buffer[1] == '!')
        buffer[0] = buffer[1] = '/';

    return true;
}
