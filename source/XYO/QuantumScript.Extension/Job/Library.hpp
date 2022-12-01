// Quantum Script Extension Job
// Copyright (c) 2022 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2022 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_QUANTUMSCRIPT_EXTENSION_JOB_LIBRARY_HPP
#define XYO_QUANTUMSCRIPT_EXTENSION_JOB_LIBRARY_HPP

#ifndef XYO_QUANTUMSCRIPT_EXTENSION_JOB_DEPENDENCY_HPP
#	include <XYO/QuantumScript.Extension/Job/Dependency.hpp>
#endif

namespace XYO::QuantumScript::Extension::Job {

	XYO_QUANTUMSCRIPT_EXTENSION_JOB_EXPORT void initExecutive(Executive *executive, void *extensionId);
	XYO_QUANTUMSCRIPT_EXTENSION_JOB_EXPORT void registerInternalExtension(Executive *executive);

};

#endif
