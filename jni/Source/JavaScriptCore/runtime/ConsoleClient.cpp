/*
 * Copyright (C) 2013, 2014 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "ConsoleClient.h"

#include "ScriptArguments.h"
#include "ScriptCallStack.h"
#include "ScriptCallStackFactory.h"
#include "ScriptValue.h"
#include <stdio.h>
#include <wtf/text/CString.h>
#include <wtf/text/WTFString.h>

using namespace Inspector;

namespace JSC {

void ConsoleClient::printURLAndPosition(const String& url, unsigned lineNumber, unsigned columnNumber)
{
    if (url.isEmpty())
        return;

    if (lineNumber > 0 && columnNumber > 0)
        printf("%s:%u:%u", url.utf8().data(), lineNumber, columnNumber);
    else if (lineNumber > 0)
        printf("%s:%u", url.utf8().data(), lineNumber);
    else
        printf("%s", url.utf8().data());
}

void ConsoleClient::printMessagePrefix(MessageSource source, MessageType type, MessageLevel level)
{
    const char* sourceString;
    switch (source) {
    case MessageSource::XML:
        sourceString = "XML";
        break;
    case MessageSource::JS:
        sourceString = "JS";
        break;
    case MessageSource::Network:
        sourceString = "NETWORK";
        break;
    case MessageSource::ConsoleAPI:
        sourceString = "CONSOLE";
        break;
    case MessageSource::Storage:
        sourceString = "STORAGE";
        break;
    case MessageSource::AppCache:
        sourceString = "APPCACHE";
        break;
    case MessageSource::Rendering:
        sourceString = "RENDERING";
        break;
    case MessageSource::CSS:
        sourceString = "CSS";
        break;
    case MessageSource::Security:
        sourceString = "SECURITY";
        break;
    case MessageSource::Other:
        sourceString = "OTHER";
        break;
    default:
        ASSERT_NOT_REACHED();
        sourceString = "UNKNOWN";
        break;
    }

    const char* levelString;
    switch (level) {
    case MessageLevel::Debug:
        levelString = "DEBUG";
        break;
    case MessageLevel::Log:
        levelString = "LOG";
        break;
    case MessageLevel::Warning:
        levelString = "WARN";
        break;
    case MessageLevel::Error:
        levelString = "ERROR";
        break;
    default:
        ASSERT_NOT_REACHED();
        levelString = "UNKNOWN";
        break;
    }

    if (type == MessageType::Trace)
        levelString = "TRACE";

    printf("%s %s:", sourceString, levelString);
}

void ConsoleClient::printConsoleMessage(MessageSource source, MessageType type, MessageLevel level, const String& message, const String& url, unsigned lineNumber, unsigned columnNumber)
{
    ConsoleClient::printURLAndPosition(url, lineNumber, columnNumber);
    printf(": ");
    ConsoleClient::printMessagePrefix(source, type, level);
    printf(" %s\n", message.utf8().data());
}

void ConsoleClient::printConsoleMessageWithArguments(MessageSource source, MessageType type, MessageLevel level, JSC::ExecState* exec, PassRefPtr<ScriptArguments> prpArguments)
{
    RefPtr<ScriptArguments> arguments = prpArguments;

    bool isTraceMessage = type == MessageType::Trace;
    size_t stackSize = isTraceMessage ? ScriptCallStack::maxCallStackSizeToCapture : 1;
    RefPtr<ScriptCallStack> callStack(createScriptCallStackForConsole(exec, stackSize));
    const ScriptCallFrame& lastCaller = callStack->at(0);

    ConsoleClient::printURLAndPosition(lastCaller.sourceURL(), lastCaller.lineNumber(), lastCaller.columnNumber());
    printf(": ");
    ConsoleClient::printMessagePrefix(source, type, level);
    for (size_t i = 0; i < arguments->argumentCount(); ++i) {
        String argAsString = arguments->argumentAt(i).toString(arguments->globalState());
        printf(" %s", argAsString.utf8().data());
    }
    printf("\n");

    if (isTraceMessage) {
        for (size_t i = 0; i < callStack->size(); ++i) {
            const ScriptCallFrame& callFrame = callStack->at(i);
            String functionName = String(callFrame.functionName());
            if (functionName.isEmpty())
                functionName = ASCIILiteral("(unknown)");
            printf("%lu: %s (", static_cast<unsigned long>(i), functionName.utf8().data());
            ConsoleClient::printURLAndPosition(callFrame.sourceURL(), callFrame.lineNumber(), callFrame.columnNumber());
            printf(")\n");
        }
    }
}

void ConsoleClient::internalMessageWithTypeAndLevel(MessageType type, MessageLevel level, JSC::ExecState* exec, PassRefPtr<ScriptArguments> prpArguments, ArgumentRequirement argumentRequirement)
{
    RefPtr<ScriptArguments> arguments = prpArguments;
    if (argumentRequirement == ArgumentRequired && !arguments->argumentCount())
        return;

    messageWithTypeAndLevel(type, level, exec, arguments.release());
}

void ConsoleClient::logWithLevel(ExecState* exec, PassRefPtr<ScriptArguments> arguments, MessageLevel level)
{
    internalMessageWithTypeAndLevel(MessageType::Log, level, exec, arguments, ArgumentRequired);
}

void ConsoleClient::clear(ExecState* exec, PassRefPtr<ScriptArguments> arguments)
{
    internalMessageWithTypeAndLevel(MessageType::Clear, MessageLevel::Log, exec, arguments, ArgumentNotRequired);
}

void ConsoleClient::dir(ExecState* exec, PassRefPtr<ScriptArguments> arguments)
{
    internalMessageWithTypeAndLevel(MessageType::Dir, MessageLevel::Log, exec, arguments, ArgumentRequired);
}

void ConsoleClient::dirXML(ExecState* exec, PassRefPtr<ScriptArguments> arguments)
{
    internalMessageWithTypeAndLevel(MessageType::DirXML, MessageLevel::Log, exec, arguments, ArgumentRequired);
}

void ConsoleClient::table(ExecState* exec, PassRefPtr<ScriptArguments> arguments)
{
    internalMessageWithTypeAndLevel(MessageType::Table, MessageLevel::Log, exec, arguments, ArgumentRequired);
}

void ConsoleClient::trace(ExecState* exec, PassRefPtr<ScriptArguments> arguments)
{
    internalMessageWithTypeAndLevel(MessageType::Trace, MessageLevel::Log, exec, arguments, ArgumentNotRequired);
}

void ConsoleClient::assertCondition(ExecState* exec, PassRefPtr<ScriptArguments> arguments, bool condition)
{
    if (condition)
        return;

    internalMessageWithTypeAndLevel(MessageType::Assert, MessageLevel::Error, exec, arguments, ArgumentNotRequired);
}

void ConsoleClient::group(ExecState* exec, PassRefPtr<ScriptArguments> arguments)
{
    internalMessageWithTypeAndLevel(MessageType::StartGroup, MessageLevel::Log, exec, arguments, ArgumentNotRequired);
}

void ConsoleClient::groupCollapsed(ExecState* exec, PassRefPtr<ScriptArguments> arguments)
{
    internalMessageWithTypeAndLevel(MessageType::StartGroupCollapsed, MessageLevel::Log, exec, arguments, ArgumentNotRequired);
}

void ConsoleClient::groupEnd(ExecState* exec, PassRefPtr<ScriptArguments> arguments)
{
    internalMessageWithTypeAndLevel(MessageType::EndGroup, MessageLevel::Log, exec, arguments, ArgumentNotRequired);
}

} // namespace JSC
