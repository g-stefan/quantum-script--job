// Quantum Script Extension Job
// Copyright (c) 2016-2023 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2023 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#include <XYO/QuantumScript.Extension/Job/Library.hpp>
#include <XYO/QuantumScript.Extension/Job/Copyright.hpp>
#include <XYO/QuantumScript.Extension/Job/License.hpp>
#include <XYO/QuantumScript.Extension/Job/Version.hpp>

#include <XYO/QuantumScript.Extension/Job/Library.Source.cpp>

namespace XYO::QuantumScript::Extension::Job {

	void registerInternalExtension(Executive *executive) {
		executive->registerInternalExtension("Job", initExecutive);
	};

	void initExecutive(Executive *executive, void *extensionId) {

		String info = "Job\r\n";
		info << License::shortLicense().c_str();

		executive->setExtensionName(extensionId, "Job");
		executive->setExtensionInfo(extensionId, info);
		executive->setExtensionVersion(extensionId, Extension::Job::Version::versionWithBuild());
		executive->setExtensionPublic(extensionId, true);

		executive->compileStringX(librarySource);
	};

};

#ifdef XYO_COMPILE_DYNAMIC_LIBRARY
extern "C" XYO_QUANTUMSCRIPT_EXTENSION_JOB_EXPORT void quantumScriptExtension(XYO::QuantumScript::Executive *executive, void *extensionId) {
	XYO::QuantumScript::Extension::Job::initExecutive(executive, extensionId);
};
#endif
