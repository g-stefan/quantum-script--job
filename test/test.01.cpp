// Created by Grigore Stefan <g_stefan@yahoo.com>
// Public domain (Unlicense) <http://unlicense.org>
// SPDX-FileCopyrightText: 2016-2024 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: Unlicense

#include <XYO/QuantumScript.hpp>
#include <XYO/QuantumScript.Extension/Console.hpp>
#include <XYO/QuantumScript.Extension/Buffer.hpp>
#include <XYO/QuantumScript.Extension/Application.hpp>
#include <XYO/QuantumScript.Extension/Shell.hpp>
#include <XYO/QuantumScript.Extension/ShellFind.hpp>
#include <XYO/QuantumScript.Extension/Thread.hpp>
#include <XYO/QuantumScript.Extension/Math.hpp>
#include <XYO/QuantumScript.Extension/Job.hpp>

using namespace XYO::QuantumScript;

void initExecutive(Executive *executive) {
	Extension::Console::registerInternalExtension(executive);
	Extension::Buffer::registerInternalExtension(executive);
	Extension::Application::registerInternalExtension(executive);
	Extension::Shell::registerInternalExtension(executive);
	Extension::ShellFind::registerInternalExtension(executive);
	Extension::Thread::registerInternalExtension(executive);
	Extension::Math::registerInternalExtension(executive);
	Extension::Job::registerInternalExtension(executive);
};

void test(int cmdN, char *cmdS[]) {

	const char *codeFile="../../test/test.01.js";	

	if (ExecutiveX::initExecutive(cmdN, cmdS, initExecutive)) {
		ExecutiveX::includePath(Shell::getFilePath(codeFile));		
		if (ExecutiveX::executeFile(codeFile)) {
			ExecutiveX::endProcessing();
			return;
		};		
		printf("%s\n", (ExecutiveX::getError()).value());
		printf("%s", (ExecutiveX::getStackTrace()).value());			
		ExecutiveX::endProcessing();

		throw std::runtime_error("Code");
	};
};

int main(int cmdN, char *cmdS[]) {
	try {

		test(cmdN,cmdS);

		return 0;

	} catch (const std::exception &e) {
		printf("* Error: %s\n", e.what());
	} catch (...) {
		printf("* Error: Unknown\n");
	};

	return 1;
};
